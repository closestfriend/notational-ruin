#include "image_processing.hpp"
#include "png_writer.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
void testImage(); void testScore();
int main(){testImage();testScore();Image im(4,4,0);assert(writePng(im,"test-output.png"));std::ifstream f("test-output.png",std::ios::binary);char sig[8]{};f.read(sig,8);const char expected[8]={char(137),'P','N','G',13,10,26,10};for(int i=0;i<8;++i)assert(sig[i]==expected[i]);std::cout<<"all tests passed\n";return 0;}
