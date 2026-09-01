#include "image_processing.hpp"
#include <algorithm>
#include <cmath>

Image blurImage(const Image& source, float radius) {
    int r = std::max(0, static_cast<int>(std::ceil(radius)));
    if (r == 0) return source;
    Image result(source.width, source.height);
    for (int y = 0; y < source.height; ++y) for (int x = 0; x < source.width; ++x) {
        int sum = 0, count = 0;
        for (int yy = std::max(0, y-r); yy <= std::min(source.height-1, y+r); ++yy)
            for (int xx = std::max(0, x-r); xx <= std::min(source.width-1, x+r); ++xx) { sum += source.at(xx, yy); ++count; }
        result.at(x, y) = static_cast<uint8_t>(sum / count);
    }
    return result;
}

static uint32_t hashPixel(int x, int y, uint32_t seed) {
    uint32_t h = seed ^ (static_cast<uint32_t>(x) * 374761393u) ^ (static_cast<uint32_t>(y) * 668265263u);
    h = (h ^ (h >> 13)) * 1274126177u;
    return h ^ (h >> 16);
}

Image addNoise(const Image& source, float amount, uint32_t seed) {
    Image result = source;
    for (int y = 0; y < source.height; ++y) for (int x = 0; x < source.width; ++x) {
        float n = (static_cast<float>(hashPixel(x, y, seed) & 0xffffu) / 65535.0f - 0.5f) * 255.0f * amount;
        result.at(x, y) = static_cast<uint8_t>(std::clamp(source.at(x, y) + n, 0.0f, 255.0f));
    }
    return result;
}

Image thresholdImage(const Image& source, float threshold) {
    Image result(source.width, source.height);
    uint8_t cutoff = static_cast<uint8_t>(std::clamp(threshold, 0.0f, 1.0f) * 255.0f);
    for (int y = 0; y < source.height; ++y) for (int x = 0; x < source.width; ++x) result.at(x, y) = source.at(x, y) < cutoff ? 0 : 255;
    return result;
}

Image adaptiveThreshold(const Image& source, float threshold, float variation, uint32_t seed) {
    Image result(source.width, source.height);
    for (int y = 0; y < source.height; ++y) for (int x = 0; x < source.width; ++x) {
        // TINKER HERE: change the spatial frequency of threshold variation.
        float wave = std::sin(x * 0.021f + seed * 0.001f) * std::cos(y * 0.017f);
        float local = std::clamp(threshold + variation * (0.65f * wave + 0.35f * (hashPixel(x/12, y/12, seed) / 4294967295.0f - 0.5f)), 0.0f, 1.0f);
        result.at(x, y) = source.at(x, y) < local * 255.0f ? 0 : 255;
    }
    return result;
}

static Image morphology(const Image& source, int iterations, bool erode) {
    Image current = source;
    for (int pass = 0; pass < iterations; ++pass) {
        Image next(source.width, source.height);
        for (int y = 0; y < source.height; ++y) for (int x = 0; x < source.width; ++x) {
            bool ink = erode;
            for (int yy = std::max(0,y-1); yy <= std::min(source.height-1,y+1); ++yy) for (int xx = std::max(0,x-1); xx <= std::min(source.width-1,x+1); ++xx)
                ink = erode ? (ink && current.at(xx,yy) == 0) : (ink || current.at(xx,yy) == 0);
            next.at(x,y) = ink ? 0 : 255;
        }
        current = next;
    }
    return current;
}

Image erodeImage(const Image& source, int iterations) { return morphology(source, std::max(0, iterations), true); }
Image dilateImage(const Image& source, int iterations) { return morphology(source, std::max(0, iterations), false); }

Image xeroxGeneration(const Image& source, float blur, float threshold, float noise, uint32_t seed) {
    // TINKER HERE: make blur increase with each photocopy generation.
    return thresholdImage(addNoise(blurImage(source, blur), noise, seed), threshold);
}
