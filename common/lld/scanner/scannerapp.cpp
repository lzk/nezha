#include "scannerapp.h"
#include "scanner.h"
#include "imagetrans.h"
#include "log.h"
#include <unistd.h>
#include "trans_jpg.h"
void caculate_image_trans_data(ScanSettings* settings);
void calculate_parameters(ScanSettings* scan_settings);

//const char* scan_tmp_file = "/tmp/.scan_tmp";
//const ScanSettings default_scan_settings = {
//    .scan_doctype = T_Photo,
//    .scan_dpi = Scan_300DPI,
//    .colorModel = Color,
//    .contrast = 50,
//    .brightness = 50,
//    .scan_size = Scan_A4,
//};

FILE* cache_file = NULL;
FILE* source_file = NULL;
const char* tmp_file_name;

int cache_buffer_open(const char* mode ,int type)
{
    int ret = 0;
    const char* cache_file_name;
    switch (type) {
    case ImageTransFormat_raw:
        cache_file_name = tmp_file_name;
        break;
    case ImageTransFormat_jpg:
        cache_file_name = tmp_file_name;
        break;
    default:
        cache_file_name = NULL;
        break;
    }
    cache_file = NULL;
    if(cache_file_name){
        cache_file = fopen(cache_file_name ,mode);
    }
    if(!cache_file){
        ret = -1;
    }
    return ret;
}
int write_cache_init(int type = 0);
int write_cache_init(int type)
{
    return cache_buffer_open("wb+" ,type);
}

int write_cache_write(char* buffer ,int buf_size)
{
    if(!cache_file)
        return -1;
    return fwrite(buffer ,sizeof(char) ,buf_size ,cache_file);
}

int write_cache_exit()
{
    if(!cache_file)
        return -1;
    fclose(cache_file);
    cache_file = NULL;
    return 0;
}

static Trans_jpg* tj = NULL;
int read_cache_init(int type)
{
    int ret = -1;
    switch (type) {
    case ImageTransFormat_jpg:
        if(!tj)
            tj = new Trans_jpg;
        ret = tj->read_file_open(tmp_file_name);
        break;
    case ImageTransFormat_raw:
        source_file = fopen(tmp_file_name ,"rb");
        if(source_file){
            ret = 0;
        }
        break;
    default:
        break;
    }
    return ret;
}

int read_cache_read(int type ,char* buffer ,int buf_size ,int lines)
{
    int ret = -1;
    switch (type) {
    case ImageTransFormat_jpg:
        if(tj){
            ret = tj->read_file_read_lines(buffer ,buf_size ,lines);
        }
        break;
    case ImageTransFormat_raw:
        if(source_file){
            ret = fread(buffer ,sizeof(char) ,buf_size ,source_file);
        }
        break;
    default:
        break;
    }
    return ret;
}

int read_cache_exit(int type)
{
    int ret = -1;
    switch (type) {
    case ImageTransFormat_jpg:
        if(tj){
            ret = tj->read_file_close();
            delete tj;
            tj = NULL;
        }
        break;
    case ImageTransFormat_raw:
        if(source_file){
            fclose(source_file);
            source_file = NULL;
            ret = 0;
        }
        break;
    default:
        break;
    }
    return ret;
}

//int trans_init(ScanSettings* settings)
//{
//    int ret;
//    ImageTransInfo* info = settings->info;

//    caculate_image_trans_data(settings);

//    ret = settings->info->image_trans->init(info);
//    if(ret <= 0){
//        return ret;
//    }
//    info->target_buf_size = ret;

//    settings->file = fopen(settings->filename ,"w+b");
//    if(!settings->file)
//        ret = -1;
//    //save head
//    else{
//        fwrite(info->target_buffer ,sizeof(char) ,info->target_buf_size ,settings->file);
//        fflush(settings->file);
//    }
//    return ret;
//}

//int trans_trans(ScanSettings* settings)
//{
//    ImageTransInfo* info = settings->info;
//    int ret;
//    ret = settings->info->image_trans->trans(info);
//    //save image
//    if(settings->file){
//        fwrite(info->target_buffer ,sizeof(char) ,info->target_buf_size ,settings->file);
//        fflush(settings->file);
//    }
//    return ret;
//}

//void trans_exit(ScanSettings* settings)
//{
//    if(settings->file){
//        fclose(settings->file);
//        settings->file = NULL;
//    }
//    settings->info->image_trans->exit();
//}

//static void image_trans_thread(void *reference)
//{
//    int ret;
//    ScanSettings* settings = (ScanSettings*)reference;
//    ret = trans_init(settings);
//    if(ret < 0)
//        return;
//    while (1) {
//        ret = trans_trans(settings);
//        if(ret)
//            break;
//    }
//    trans_exit(settings);
//}


ScannerApp::ScannerApp(DeviceIOManager* _device_manager)
    :device_manager(_device_manager)
{
    scanner = new Scanner(this);
    image_trans = new ImageTrans;
}

ScannerApp::~ScannerApp()
{
    delete scanner;
    delete image_trans;
}

//int ScannerApp::init_scan(ScanSettings* settings)
//{
//    settings->info->image_trans = image_trans;
//    calculate_parameters(settings);
//    int ret = trans_init(settings);
//    if(ret < 0)
//        return ret;
//    else
//        ret = 0;

////    if (pthread_create(&thread, NULL, image_trans_thread, (void*)settings))
////    {
////        ret = -1;
////    }

//    return ret;
//}

int ScannerApp::save_scan_data(ScanSettings* settings ,char* buffer ,int buf_size)
{
//    trans_trans(settings);
    settings->received_bytes += buf_size;
    if(settings->calc_data.source_size){
        settings->progress = 100 * settings->received_bytes / settings->calc_data.source_size;
//        LOGLOG("%ld/%ld = %d" ,settings->received_bytes ,settings->calc_data.source_size ,settings->progress);
    }
    if(settings->callback)
        settings->callback(settings);
    write_cache_write(buffer ,buf_size);
    return 0;
}

//int ScannerApp::exit_scan(ScanSettings* settings)
//{
//    trans_exit(settings);
//    return 0;
//}

#define SCAN_BUFFER_SIZE 	0x10000
int ScannerApp::trans_process(ScanSettings* settings)
{
    ImageTransInfo* info = settings->info;
    int ret;
    ret = read_cache_init(info->source->format);
    if(ret){
        return ret;
    }

    int each_lines = 3;
    int each_source_size =  each_lines * info->source_line_buf_size;
    char* buffer = new char[each_source_size];
//    char* buffer = new char[SCAN_BUFFER_SIZE];
    info->scan_buffer = buffer;

//    int source_size = each_lines * info->source_line_buf_size;
//    char* jerry_buffer = new char[each_source_size * 3 + 0x10000];
//    info->resume_buf_size = each_source_size * 3 + 0x10000;
    char* jerry_buffer = new char[SCAN_BUFFER_SIZE * 10];
    info->resume_buf_size = SCAN_BUFFER_SIZE * 10;
    info->resume_buffer = jerry_buffer;

    int target_lines = each_lines * 10 / info->source_lines_per_10_lines;
    int target_size = target_lines * info->target_line_buf_size;
    char* target_buffer = new char[target_size];
    info->target_buf_size = target_size;
//    char* target_buffer = new char[SCAN_BUFFER_SIZE];
//    info->target_buf_size = SCAN_BUFFER_SIZE;
    info->target_buffer = target_buffer;

    info->filename = (const char*)settings->filename;
    image_trans->init(info);
    int lines;
    for(int rest_lines = info->source_total_lines ;rest_lines > 0 ;rest_lines = rest_lines - each_lines){
        if(rest_lines > each_lines){
            lines = each_lines;
        }else{
            lines = rest_lines;
        }
        each_source_size = lines * info->source_line_buf_size;
        read_cache_read(info->source->format ,buffer ,each_source_size ,lines);
        info->each_source_size = each_source_size;
        info->lines = lines;
        image_trans->process(info);
    }
    read_cache_exit(info->source->format);
    image_trans->exit();

    delete [] buffer;
    delete [] jerry_buffer;
    delete [] target_buffer;
    return ret;
}

#define Test_Jerry 1
int ScannerApp::scan(Printer_struct* printer ,ScanSettings* settings)
{
    int ret = 0;
    settings->printer = printer;
    ImageTransInfo image_trans_info;
    image_trans_info.source = &settings->calc_data.source;
    image_trans_info.target = &settings->calc_data.target;
    settings->info = &image_trans_info;
    calculate_parameters(settings);
    caculate_image_trans_data(settings);
#if Test_Jerry
//    ImageTransInfo* info = settings->info;
    Calc_Data *pCalc = &settings->calc_data;
    Image_Data_Struct* source = &pCalc->source;
    char* source_buf = new char[SCAN_BUFFER_SIZE];
    pCalc->scan_buffer = source_buf;
    pCalc->scan_buffer_size = SCAN_BUFFER_SIZE;
//    settings->file = NULL;

    tmp_file_name = settings->tmpfilename;
    ret = write_cache_init(source->format);
//    ret = init_scan(settings);
    if(ret){
        return STATUS_Error_App;
    }

    set_cancel(false);
    settings->received_bytes = 0;
//    ret = scanner->flat_scan(printer ,settings);
    for(int i = 0 ;i < 3 ;i++){
        ret = scanner->flat_scan(printer ,settings);
        if(ret != STATUS_Error_App && ret != STATUS_Error_lock && ret != STATUS_Error_busy)
            break;
        else
            usleep(100 * 1000);
    }
    if(STATUS_Error_lock == ret)
        ret = STATUS_Error_machine;
//    exit_scan(settings);
    write_cache_exit();
    delete [] source_buf;
#endif
    if(!ret){
        trans_process(settings);
        settings->progress  = -2;
        if(settings->callback)
            settings->callback(settings);
    }else{
        settings->progress  = -3;
        if(settings->callback)
            settings->callback(settings);
    }
    remove(tmp_file_name);
    return ret;
}

void ScannerApp::set_cancel(bool cancel)
{
    scanner->set_cancel(cancel);
}

int ScannerApp::start_scan(ScanSettings* settings)
{
    settings->progress = -1;//start
    if(settings->callback)
        settings->callback(settings);
}
