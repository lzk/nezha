#ifndef TRANS_JPG_H
#define TRANS_JPG_H

enum{
    JPG_COLORMODE_color,
    JPG_COLORMODE_gray,
    JPG_COLORMODE_bw,
};

class JPG_Decompress;
class JPG_Compress;
class Trans_jpg
{
public:
    Trans_jpg();
    ~Trans_jpg();

    int write_file_open(const char* filename ,int image_width ,int image_height ,int color_mode ,int quality);
    int write_file_write_lines(char* buffer ,int bufsize ,int lines ,int line_size);
    int write_file_close();

    int read_file_open(const char* filename);
    int read_file_read_lines(char* buffer ,int bufsize ,int lines);
    int read_file_close();

private:
    JPG_Decompress* jpg_decompress;
    JPG_Compress* jpg_compress;
};
#endif // TRANS_JPG_H
