#include "score.hpp"
#include "image_processing.hpp"
#include "svg_renderer.hpp"
#include "raster_renderer.hpp"
#include <cassert>
void testScore() {
    ScoreParameters p; p.seed=99; p.width=320; p.height=240;
    Score a=generateScore(p), b=generateScore(p);
    assert(a.staffs.size()>=3 && a.staffs.size()<=8);
    assert(a.strokes.size()>0 && a.staffs.size()==b.staffs.size() && a.strokes.size()==b.strokes.size());
    assert(renderSvg(a).find("<svg")!=std::string::npos);
    Image raster=renderRaster(a); assert(raster.width==320 && raster.height==240);
    Image blurred=blurImage(raster,1.5f); assert(blurred.pixels != raster.pixels);
    Image dark=thresholdImage(blurred,.52f); for(uint8_t pixel:dark.pixels) assert(pixel==0 || pixel==255);
}
