#ifndef IMAGEINFO_H
#define IMAGEINFO_H

typedef struct
{
    int PixelsOfWidth;
    int TotalScanLines;
    int BitsPerPixel;
    int Resolution;
}
    ImageInfo;

typedef struct
{
    int dpi_x;
    int dpi_y;
    int pixels_per_line;
    int total_lines;
    int bits_per_pixel;
    int format;
}
    Image_Data_Struct;
#endif // IMAGEINFO_H
