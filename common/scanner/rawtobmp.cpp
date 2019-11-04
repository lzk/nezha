#include "rawtobmp.h"

#include <stdint.h>
#include <string.h>
#include <math.h>
#include "log.h"
#pragma pack(2)
typedef struct
{
    uint16_t bfType;
//    char bfType[2];
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
}
    BitmapFileHeader;

typedef struct
{
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;

}
    BitmapInfoHeader;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint8_t rgbBlue;
    uint8_t rgbGreen;
    uint8_t rgbRed;
    uint8_t rgbReserved;
}
    RgbQuad;
#pragma pack()

RawToBmp::RawToBmp()
{

}

int RawToBmp::trans_header(char* buffer ,int buf_size ,ImageInfo* source_info)
{
    int HeaderSize;
    BitmapFileHeader bmfh;
    BitmapInfoHeader bmih;

    RgbQuad *pColorTable;
    uint32_t dwClrNum = 0;
    uint32_t dwOffset = 0 ;
    int nBitsPerPixel;

    memset(&bmih, 0, sizeof(bmih));

    HeaderSize = sizeof(BitmapFileHeader) ;
    HeaderSize += sizeof(BitmapInfoHeader);

    nBitsPerPixel = source_info->BitsPerPixel;

    if(nBitsPerPixel <= 8)
    {
        dwClrNum = (uint32_t)pow(2.0, (double)nBitsPerPixel);
        HeaderSize += (dwClrNum * sizeof(RgbQuad));
    }

    if(HeaderSize > buf_size)
    {
        return -1;
    }

    bmih.biSize = sizeof(bmih);
    bmih.biWidth = source_info->PixelsOfWidth;
    bmih.biHeight = 0 - source_info->TotalScanLines;
    bmih.biPlanes = 1;
    bmih.biBitCount = (uint16_t)nBitsPerPixel;
    bmih.biCompression = 0;
    bmih.biSizeImage = ((bmih.biWidth * nBitsPerPixel + 31) / 32 * 4) * source_info->TotalScanLines;
    bmih.biXPelsPerMeter = (uint32_t)((double)source_info->Resolution * 39.374);
    bmih.biYPelsPerMeter = bmih.biXPelsPerMeter;
    bmih.biClrUsed = dwClrNum;
    bmih.biClrImportant = 0;

    memset(&bmfh, 0, sizeof(bmfh));

    bmfh.bfType = ((uint16_t)('M'<<8) | 'B');
//    bmfh.bfType[0] = 'B';
//    bmfh.bfType[1] = 'M';
 //   char bfType[2] = {'B', 'M'};
    bmfh.bfOffBits = HeaderSize;
    bmfh.bfSize = bmfh.bfOffBits + bmih.biSizeImage;

    memcpy(buffer, &bmfh, sizeof(bmfh));
    dwOffset += (sizeof(bmfh));
//    memcpy(buffer, &bmfh, 2);
//    memcpy(buffer + 2, &bmfh + 4, sizeof(bmfh) - 4);
//    dwOffset += (sizeof(bmfh) - 2);

    memcpy(buffer + dwOffset, &bmih, sizeof(bmih));
    dwOffset += sizeof(bmih);

    if(dwClrNum > 0)
    {
        pColorTable = (RgbQuad *)(buffer + dwOffset);
        uint8_t bClrVal;
        uint32_t dwTemp;
        uint32_t i;
        for(i=0; i < dwClrNum; i++)
        {
            dwTemp = 255 * i / (dwClrNum -1);
            bClrVal = (uint8_t)dwTemp;

            pColorTable[i].rgbBlue = bClrVal;
            pColorTable[i].rgbGreen = bClrVal;
            pColorTable[i].rgbRed = bClrVal;
            pColorTable[i].rgbReserved = 0;

        }
    }
    return HeaderSize;
}

int RawToBmp::rgb2bgr(char* buffer ,int buf_size ,const char* source ,int source_size)
{
    if(buf_size < source_size)
        return -1;
    char tmp;
    for(int j = 0; j < buf_size; j += 3)
    {
        tmp = source[j];
        buffer[j] = source[j +2];
        buffer[j + 1] = source[j + 1];
        buffer[j + 2] = tmp;
    }
}

