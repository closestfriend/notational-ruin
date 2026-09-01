#include "raster_renderer.hpp"
#include <algorithm>
#include <cmath>
static void pixel(Image& im,int x,int y,int radius=1){ for(int yy=-radius;yy<=radius;++yy)for(int xx=-radius;xx<=radius;++xx)if(x+xx>=0&&x+xx<im.width&&y+yy>=0&&y+yy<im.height)im.at(x+xx,y+yy)=0; }
static void line(Image& im,ScorePoint a,ScorePoint b,float width){ int n=std::max(1,(int)std::hypot(b.x-a.x,b.y-a.y)); int r=std::max(0,(int)std::round(width/2)); for(int i=0;i<=n;++i)pixel(im,(int)(a.x+(b.x-a.x)*i/n),(int)(a.y+(b.y-a.y)*i/n),r); }
Image renderRaster(const Score& s){ Image im(s.width,s.height,255); auto draw=[&](const Stroke& st){for(size_t i=1;i<st.points.size();++i)line(im,st.points[i-1],st.points[i],st.width);}; for(const auto& st:s.staffs)for(const auto& l:st.lines)draw(l); for(const auto& st:s.strokes)draw(st); for(const auto& t:s.texts){int x=(int)t.position.x,y=(int)t.position.y; for(int i=0;i<(int)t.text.size()*std::max(3.f,t.size/5);++i)line(im,{(float)(x+i), (float)(y-std::max(5.f,t.size))},{(float)(x+i),(float)y},1); } return im; }
