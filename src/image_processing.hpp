#pragma once
#include "image.hpp"

Image blurImage(const Image& source, float radius);
Image addNoise(const Image& source, float amount, uint32_t seed);
Image thresholdImage(const Image& source, float threshold);
Image adaptiveThreshold(const Image& source, float threshold, float variation, uint32_t seed);
Image erodeImage(const Image& source, int iterations);
Image dilateImage(const Image& source, int iterations);
Image xeroxGeneration(const Image& source, float blur, float threshold, float noise, uint32_t seed);
