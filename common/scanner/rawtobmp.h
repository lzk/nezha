#ifndef RAWTOBMP_H
#define RAWTOBMP_H

#include "imageinfo.h"

class RawToBmp
{
public:
    RawToBmp();
    static int trans_header(char* buffer ,int buf_size ,ImageInfo* source_info);
    static int rgb2bgr(char* buffer ,int buf_size ,const char* source ,int source_size);
};

#endif // RAWTOBMP_H
