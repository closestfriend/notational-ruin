#include "image.hpp"
#include <cassert>

void testImage() {
    Image source(2, 2, 255);
    source.at(0, 0) = 0;
    source.at(1, 1) = 0;
    Image enlarged = resizeNearest(source, 4, 4);
    assert(enlarged.width == 4 && enlarged.height == 4);
    assert(enlarged.at(0, 0) == 0);
    assert(enlarged.at(3, 3) == 0);
    assert(enlarged.at(3, 0) == 255);
}
