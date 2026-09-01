#include "image_processing.hpp"
#include "png_writer.hpp"
#include "raster_renderer.hpp"
#include "score.hpp"
#include "svg_renderer.hpp"
#include <fstream>
#include <iostream>
#include <string>

static void help(){std::cout<<"usage: notational-ruin [ruin|prefix] [options]\noptions: --seed N --width N --height N --density F --chaos F --threshold F --threshold-variation F --blur F --noise F --erosion N --dilation N --downsample F --xerox-depth N\n";}
int main(int argc,char** argv){
    if(argc<2){help();return 1;} std::string prefix="output"; int i=1; if(std::string(argv[i])=="ruin")++i; else prefix=argv[i++];
    ScoreParameters q; float threshold=.52f, variation=0, blur=1.8f, noise=.15f, downsample=1; int erosion=1,dilation=0,depth=3;
    auto need=[&](const char* name){if(i+1>=argc){throw std::runtime_error(std::string("missing value for ")+name);}return std::string(argv[++i]);};
    try{for(;i<argc;++i){std::string a=argv[i]; if(a=="--help"){help();return 0;} else if(a=="--seed")q.seed=std::stoul(need("--seed")); else if(a=="--width")q.width=std::stoi(need("--width")); else if(a=="--height")q.height=std::stoi(need("--height")); else if(a=="--density")q.density=std::stof(need("--density")); else if(a=="--chaos")q.chaos=std::stof(need("--chaos")); else if(a=="--threshold")threshold=std::stof(need("--threshold")); else if(a=="--threshold-variation")variation=std::stof(need("--threshold-variation")); else if(a=="--blur")blur=std::stof(need("--blur")); else if(a=="--noise")noise=std::stof(need("--noise")); else if(a=="--erosion")erosion=std::stoi(need("--erosion")); else if(a=="--dilation")dilation=std::stoi(need("--dilation")); else if(a=="--downsample")downsample=std::stof(need("--downsample")); else if(a=="--xerox-depth")depth=std::stoi(need("--xerox-depth")); else throw std::runtime_error("unknown option: "+a);}
    if(q.width<16||q.height<16||downsample<=0||downsample>1||depth<0)throw std::runtime_error("invalid dimensions or degradation values");
    Score score=generateScore(q); std::ofstream svg(prefix+"_clean.svg"); svg<<renderSvg(score); svg.close(); Image image=renderRaster(score);
    // TINKER HERE: change how resolution loss accumulates before photocopying.
    int smallW=std::max(1,(int)(q.width*downsample)),smallH=std::max(1,(int)(q.height*downsample)); image=resizeNearest(image,smallW,smallH); image=blurImage(image,blur); image=addNoise(image,noise,q.seed); image=variation>0?adaptiveThreshold(image,threshold,variation,q.seed):thresholdImage(image,threshold); image=erodeImage(image,erosion); image=dilateImage(image,dilation);
    for(int gen=0;gen<depth;++gen){ // TINKER HERE: make old reproduction generations drift horizontally.
        image=xeroxGeneration(image,blur*.55f+gen*.12f,threshold+(gen%2?.01f:-.01f),noise*.45f,q.seed+static_cast<uint32_t>(gen+1));
    }
    image=resizeNearest(image,q.width,q.height); if(!writePng(image,prefix+"_ruined.png"))throw std::runtime_error("could not write PNG"); std::cout<<"wrote "<<prefix<<"_clean.svg and "<<prefix<<"_ruined.png\n";
    }catch(const std::exception& e){std::cerr<<"error: "<<e.what()<<"\n";return 2;} return 0;
}
