#ifndef IMAGETRANS_H
#define IMAGETRANS_H

#include "imageinfo.h"
enum{
    ImageTransMode_rawToBmp = 0x01,
    ImageTransMode_bmpToRaw = 1 << 1,
    ImageTransMode_GammaBrightnessContrast = 1 << 8,
};

enum{
    ImageTransFormat_raw,
    ImageTransFormat_bmp,
    ImageTransFormat_jpg,
};

enum{
    ImageTransMedia_Photo,
    ImageTransMedia_Graph,
    ImageTransMedia_Text,
};


typedef struct
{
    int gamma;
    int brightness;
    int contrast;
    int mediaO;
}
    ImageTransParameter;

class ImageTrans;
typedef struct
{
    ImageTrans* image_trans;
    Image_Data_Struct* source;
    Image_Data_Struct* target;
    ImageInfo target_image_info;
    ImageTransParameter image_trans_parameter;

    int source_dpi_x;
    int source_dpi_y;
    int source_total_lines;
    int source_pixelsOfWidth;

    char* resume_buffer;
    int resume_buf_size;
    int scanned_buf_size;
    int resume_bytes;
    int source_line_buf_size;
    char* target_buffer;
    int target_buf_size;
    int target_line_buf_size;
//    int source_lines_per_line;
//    int lines_per_source_line;
    int source_lines_per_10_lines;

//    char* real_target_buf;
//    const char* real_source_buf;
    int real_line;
//    int ready_line;
//    int header_size;

    char* scan_buffer;
    int each_source_size;
    int lines;
    const char* filename;
}
    ImageTransInfo;

class NtdcmsApi;
class ImageTrans
{
public:
    ImageTrans(NtdcmsApi* api = 0);
    ~ImageTrans();
    int init(ImageTransInfo* source_info);
//    int trans(ImageTransInfo* source_info);
    int exit();
    int process(ImageTransInfo* source_info);
private:
    static int trans_header(ImageTransInfo* source_info);
    NtdcmsApi* pNtdcmsApi;
    bool self_api;
};

#endif // IMAGETRANS_H
