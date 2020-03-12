#include "trans_jpg.h"
#include <stdio.h>
#include <jpeglib.h>
#include <string.h>
#include "log.h"
class JPG_Decompress
{
public:
    int file_open(const char* filename);
    int file_read_lines(char* read_buffer ,int bufsize ,int lines);
    int file_close();

private:
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE * infile;		/* source file */
    JSAMPARRAY buffer;		/* Output row buffer */
    int row_stride;		/* physical row width in output buffer */
};

class JPG_Compress
{
public:
    int file_open(const char* filename ,int image_width ,int image_height ,int color_mode ,int quality);
    int file_write_lines(char* buffer ,int bufsize ,int lines ,int line_size);
    int file_close();

private:
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE * outfile;		/* target file */
    JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
//    int row_stride;		/* physical row width in image buffer */
    int color_mode;
};

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  (*cinfo->err->output_message) (cinfo);

}

int JPG_Decompress::file_open(const char* filename)
{
    if ((infile = fopen(filename, "rb")) == NULL) {
      LOGLOG("can't open %s\n", filename);
      return -1;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jerr.error_exit = my_error_exit;

    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);

    (void) jpeg_read_header(&cinfo, TRUE);

    (void) jpeg_start_decompress(&cinfo);

    row_stride = cinfo.output_width * cinfo.output_components;

    buffer = (*cinfo.mem->alloc_sarray)
          ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    return 0;
}

int JPG_Decompress::file_read_lines(char* read_buffer ,int bufsize ,int lines)
{
    if(!read_buffer)
        return -1;

    int to_lines = lines;
    if(cinfo.output_scanline + to_lines > cinfo.output_height)
        to_lines = cinfo.output_height - cinfo.output_scanline;
    if(row_stride * to_lines < bufsize){
        to_lines = -1;
    }
    char* p = read_buffer;
    for(int i = 0 ;i < to_lines ;i++){
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);
        memcpy(p ,buffer[0] ,row_stride);
        p += row_stride;
    }
    return to_lines;
}
int JPG_Decompress::file_close()
{
    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
}

int JPG_Compress::file_open(const char* filename ,int image_width ,int image_height ,int color_mode ,int quality)
{

    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_compress(&cinfo);

    if ((outfile = fopen(filename, "wb")) == NULL) {
      LOGLOG("can't open %s\n", filename);
      return -1;
    }
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = image_width;
    cinfo.image_height = image_height;
    if(color_mode == JPG_COLORMODE_color){
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_RGB;
    }else{
        cinfo.input_components = 1;
        cinfo.in_color_space = JCS_GRAYSCALE;
    }

    jpeg_set_defaults(&cinfo);

    jpeg_set_quality(&cinfo, quality, TRUE);

    jpeg_start_compress(&cinfo, TRUE);

//    row_stride = image_width * cinfo.input_components;
    this->color_mode = color_mode;
    return 0;
}

int JPG_Compress::file_write_lines(char* buffer ,int bufsize ,int lines ,int line_size)
{
    if(!buffer)
        return -1;

    int to_lines = lines;
    if(cinfo.next_scanline + to_lines > cinfo.image_height)
        to_lines = cinfo.image_height - cinfo.next_scanline;
    if(line_size * to_lines < bufsize){
        to_lines = -1;
    }
    char* p = buffer;
    if(color_mode == JPG_COLORMODE_bw){
        char real_buffer[cinfo.image_width];
        for(int i = 0 ;i < to_lines ;i++){
            for(int i = 0 ;i < cinfo.image_width ;i++){
                real_buffer[i] = (p[i / 8] & (1 << (7-(i%8)))) ?255 :0;
            }
            row_pointer[0] = (unsigned char*)real_buffer;
            (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
            p += line_size;
        }
    }else{
        for(int i = 0 ;i < to_lines ;i++){
            row_pointer[0] = (unsigned char*)p;
            (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
            p += line_size;
        }
    }
    return to_lines;
}

int JPG_Compress::file_close()
{
    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
    return 0;
}

Trans_jpg::Trans_jpg()
    :jpg_decompress(NULL)
    ,jpg_compress(NULL)
{

}
Trans_jpg::~Trans_jpg()
{
    if(jpg_decompress)
        delete jpg_decompress;
    if(jpg_compress)
        delete jpg_compress;
}

int Trans_jpg::read_file_open(const char* filename)
{
    if(!jpg_decompress)
        jpg_decompress = new JPG_Decompress;
    return jpg_decompress->file_open(filename);
}

int Trans_jpg::read_file_read_lines(char* read_buffer ,int bufsize ,int lines)
{
    if(!jpg_decompress)
        return -1;
    return jpg_decompress->file_read_lines(read_buffer ,bufsize ,lines);
}

int Trans_jpg::read_file_close()
{
    if(!jpg_decompress)
        return -1;
    int ret = jpg_decompress->file_close();
    delete jpg_decompress;
    jpg_decompress = NULL;
    return ret;
}

int Trans_jpg::write_file_open(const char* filename ,int image_width ,int image_height ,int color_mode ,int quality)
{
    if(!jpg_compress)
        jpg_compress = new JPG_Compress;
    return jpg_compress->file_open(filename ,image_width ,image_height ,color_mode ,quality);
}
int Trans_jpg::write_file_write_lines(char* buffer ,int bufsize ,int lines ,int line_size)
{
    if(!jpg_compress)
        return -1;
    return jpg_compress->file_write_lines(buffer ,bufsize ,lines ,line_size);
}
int Trans_jpg::write_file_close()
{
    if(!jpg_compress)
        return -1;
    int ret = jpg_compress->file_close();
    delete jpg_compress;
    jpg_compress = NULL;
    return ret;
}
