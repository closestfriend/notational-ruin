#pragma once
#include <random>
struct Random {
    std::mt19937 engine;
    explicit Random(uint32_t seed) : engine(seed) {}
    float unit() { return std::uniform_real_distribution<float>(0.0f, 1.0f)(engine); }
    float range(float a, float b) { return std::uniform_real_distribution<float>(a, b)(engine); }
    int integer(int a, int b) { return std::uniform_int_distribution<int>(a, b)(engine); }
    bool chance(float p) { return unit() < p; }
};
