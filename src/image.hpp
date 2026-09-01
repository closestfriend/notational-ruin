#pragma once
#include <cstdint>
#include <algorithm>
#include <vector>

struct Image {
    int width = 0;
    int height = 0;
    std::vector<uint8_t> pixels;
    Image() = default;
    Image(int w, int h, uint8_t value = 255) : width(w), height(h), pixels(static_cast<size_t>(w * h), value) {}
    uint8_t& at(int x, int y) { return pixels[static_cast<size_t>(y * width + x)]; }
    uint8_t at(int x, int y) const { return pixels[static_cast<size_t>(y * width + x)]; }
    void fill(uint8_t value) { std::fill(pixels.begin(), pixels.end(), value); }
};

Image resizeNearest(const Image& source, int width, int height);
