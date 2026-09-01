#include "image.hpp"
#include <algorithm>
#include <stdexcept>

Image resizeNearest(const Image& source, int width, int height) {
    if (width <= 0 || height <= 0) throw std::invalid_argument("image dimensions must be positive");
    Image result(width, height);
    for (int y = 0; y < height; ++y) for (int x = 0; x < width; ++x) {
        int sx = std::min(source.width - 1, x * source.width / width);
        int sy = std::min(source.height - 1, y * source.height / height);
        result.at(x, y) = source.at(sx, sy);
    }
    return result;
}
