#include "ntdcmsapi.h"
#include "log.h"
#include <string.h>

int (*jkInitNTDCMS)(NTDCOLOR *);
int (*jkStartFilterJob)(int JobNum, int FilterMode, int Gain, int Clamp, int XSize, int YSize, char *Array);
int (*jkStartAdjustJob)(int JobNum, ADJUSTSTR *IntputStr);
int (*jkStartAdjustCurveJob)(int JobNum, unsigned char *RGBcurve, ADJUSTSTR *IntputStr);
int (*jkCreateNTDCMS)(int JobNum);
int (*jkNTDCMS)(int JobNum,unsigned char *Src,unsigned char *Info,int Bandsize,int LineIndex);
int (*jkEndNTDCMS)(int JobNum);
int (*jkStartCisInfoJob)(int JobNum ,int ChipMode,int GapSize,int GapOffset,int ChipSize,int RegistMode,int YSize,char *Array);
int (*jkUpdateMatrixConvert)(int JobNum,double *Matrix,int GammaSize, WORD *GammaR, WORD *GammaG, WORD *GammaB);
int (*jkCreateRGBCurve)(int CurveType, int Gamma[3], int Brightness[3], int Contrast[3], int Minlevel[3], int Maxlevel[3],unsigned char Curve[768]);

unsigned char* pImageBuffer = NULL;
int target_line_buf_size;
int* real_lines;
unsigned char* lock_buffer()
{
    return pImageBuffer;
}

void unlock_buffer(int cstart ,int cend ,int mstart ,int mend)
{
    pImageBuffer += target_line_buf_size;
    (*real_lines) ++;
}

NtdcmsApi::NtdcmsApi()
    :m_nJobNum(-1)
{
#if NEZHA_SFP
    jkInitNTDCMS = NULL;
    jkStartFilterJob = NULL;
    jkStartAdjustJob = NULL;
    jkStartAdjustCurveJob = NULL;
    jkCreateNTDCMS = NULL;
    jkNTDCMS = NULL;
    jkEndNTDCMS = NULL;
    jkStartCisInfoJob = NULL;
    jkUpdateMatrixConvert = NULL;
    jkUpdateMatrixConvert = NULL;
    jkCreateRGBCurve = NULL;
#else
    jkInitNTDCMS = InitNTDCMS;
    jkStartFilterJob = StartFilterJob;
    jkStartAdjustJob = StartAdjustJob;
    jkStartAdjustCurveJob = StartAdjustCurveJob;
    jkCreateNTDCMS = CreateNTDCMS;
    jkNTDCMS = NTDCMS;
    jkEndNTDCMS = EndNTDCMS;
    jkStartCisInfoJob = StartCisInfoJob;
    jkUpdateMatrixConvert = UpdateMatrixConvert;
    jkUpdateMatrixConvert = UpdateMatrixConvert;
    jkCreateRGBCurve = CreateRGBCurve;
#endif
    memset(&m_adjuststr ,0 ,sizeof(m_adjuststr));
    memset(&m_ntdColor , 0 ,sizeof(m_ntdColor));
}

int NtdcmsApi::caculate_parameter(ImageTransInfo* info)
{
    int ret = 0;
    ImageInfo* target = &info->target_image_info;
    ImageTransParameter* para = &info->image_trans_parameter;

//    pImageBuffer = (unsigned char*)info->resume_buffer;
    target_line_buf_size = info->target_line_buf_size;
    real_lines = &info->real_line;

    int method = 0;
//    method |= 1 << 15;//input 32 pixels align
//    if(target->Resolution != info->source_dpi_x || target->Resolution != info->source_dpi_y){
//        method += 1 << 8;//Enable Bilinear Scaling algorithm
//    }
//    method += 1 << 2;//Matrix color convert
    int htWidth = info->source_pixelsOfWidth;
    float scalex =  (float)target->Resolution / info->source_dpi_x;
    float scaley = (float)target->Resolution / info->source_dpi_y;
    int mediaI = 7;
    int scan_adjust = 0;
    switch (para->mediaO) {
    case ImageTransMedia_Photo:
        mediaI = 6;
        scan_adjust |= 1;
        break;
    case ImageTransMedia_Graph:
        break;
    case ImageTransMedia_Text:
        scan_adjust |= (20 << 8);
        break;
    default:
        break;
    }
    int resolute = info->source_dpi_x / 50;
    int colorI = 2;
    int colorO = 2;
    int threshold = 127;
    switch(info->target_image_info.BitsPerPixel){
    case 24:
        colorI = 2;
        colorO = 2;
        method += 1 << 8;//Enable Bilinear Scaling algorithm
        method += 1 << 2;//Matrix color convert
        method |= 1 << 1;//ICM color convert
        break;
    case 8:
        colorI = 1;
        colorO = 1;
        method += 1 << 9;//De-screen
        method += 1 << 3;//special color convert
        threshold = 1;
        break;
    case 1:
        colorI = 1;
        colorO = 1;
        method += 1 << 8;//Enable Bilinear Scaling algorithm
        method += 1 << 2;//Matrix color convert
        method += 1 << 3;//special color convert
        method += 1 << 5;//order dither
        threshold = 180;
        break;
    default:
        break;
    }

    m_ntdColor.Lock			= lock_buffer;//function for get the pointer of output beffer
    m_ntdColor.Unlock		= unlock_buffer;//function for write data to pointer
    m_ntdColor.AddEmptyLine	= NULL;//funciotn for insert enpty line
    m_ntdColor.HTMethod		= method;
    m_ntdColor.HTWidth = htWidth;		//Input Image Width
    m_ntdColor.HTScaleX		= scalex * 10000;//Scale ratio of X(10/100~100)*100
    m_ntdColor.HTScaleY		= scaley * 10000;
    m_ntdColor.HTMediaI		= mediaI; //Input Image Media : 5:Nature Scan
    m_ntdColor.HTMediaO		= 0; //Output Image Media: 0:Default(monitor)
    m_ntdColor.HTResolute	= resolute;
    m_ntdColor.HTDescreen	= scan_adjust&0xff; //For descreen quality 0:skip, 1:Fine Press, 2:Magazine, 3:News Paper
    m_ntdColor.HTColorI		= colorI; //Input color mode  1:Gray, 2:RGB 11:Gray16, 12:RGB16
    m_ntdColor.HTColorO		= colorO; //Output color mode   1:Gray, 2:RGB 11:Gray16, 12:RGB16
    m_ntdColor.HTQualityX	= 1; //Printer Resolution 0:300dpi, 1:600dpi, 2:1200dpi, 3:1800dpi, 4:2400dpi
    m_ntdColor.HTQualityY	= threshold; //B/W scan threshold
    m_ntdColor.HTLevels		= 2;//Setting the level number for input image 2:Gray 1bit, 222:RGB 1bit, 444:RGB 2bits, 4444:CMYK 2bits, 2222:CMYK 1bit, 444444:CMYKcm 2bits,222222:CMYKcm 1bit
    m_ntdColor.HTFormat		= 0;//Format type for swatch 0:Sequence
    m_ntdColor.HTPath		= NULL;//LUT path for convert

    try{
        if(m_nJobNum >= 0)
            jkEndNTDCMS(m_nJobNum);
        m_nJobNum = jkInitNTDCMS(&m_ntdColor);

        if(1){
//        if(para->brightness != 50 || para->contrast != 50 || para->gamma != 1000){
            method += 1 << 14;//color adjustment
            int gamma = para->gamma;
            int brightness = para->brightness * 2 - 100;
            int contranst = para->contrast * 2 - 100;
            int chroma = 0;
            m_adjuststr.Flag = 4;//16;
            m_adjuststr.ProfilePathI = NULL;//"/tmp/modulename.icm";
            m_adjuststr.ProfilePathO = NULL;//"/tmp/RGB Color Space Profile.icm";
            m_adjuststr.Temperature = 0;
            m_adjuststr.GammaValue = gamma;
            m_adjuststr.Brightness[0] = m_adjuststr.Brightness[1] =m_adjuststr.Brightness[2] = m_adjuststr.Brightness[3] = brightness;
            m_adjuststr.Contrast[0] = m_adjuststr.Contrast[1] =m_adjuststr.Contrast[2] = m_adjuststr.Contrast[3] = contranst;
            m_adjuststr.Chroma[0] = m_adjuststr.Chroma[1] =m_adjuststr.Chroma[2] = m_adjuststr.Chroma[3] = chroma;
            m_adjuststr.BalanceK = 0;
            m_adjuststr.BalanceC = 0;
            m_adjuststr.BalanceM = 0;
            m_adjuststr.BalanceY = 0;

//            unsigned char curve[768];
//            int Gamma[3], Brightness[3], Contrast[3], Minlevel[3], Maxlevel[3];
//            jkCreateRGBCurve(0 ,Gamma ,Brightness ,Contrast ,Minlevel ,Maxlevel ,curve);
//            jkStartAdjustCurveJob(m_nJobNum ,curve ,&m_adjuststr);
            jkStartAdjustJob(m_nJobNum ,&m_adjuststr);
        }

        if(scan_adjust & 0xff00){
            int filter_type = 0;
            if((colorI == 2) && (mediaI == 7)){
                filter_type = 16;
            }
            jkStartFilterJob(m_nJobNum ,filter_type ,0 ,(scan_adjust & 0xff00)>>8 ,0 ,0 ,NULL);
        }

        int jobindex = jkCreateNTDCMS(m_nJobNum);
        LOGLOG("create ntdcms job index:%d" ,jobindex);

    }catch(...){
        m_nJobNum = -1;
        ret = -1;
    }

    return ret;
}

int NtdcmsApi::process(unsigned char* target ,unsigned char* source ,int band_size ,int line_index)
{
    if(m_nJobNum < 0)
        return -1;
    int ret = 0;
    try{
        pImageBuffer = target;
        jkNTDCMS(m_nJobNum ,source ,NULL ,band_size ,line_index);
    }catch(...){
        ret = -1;
    }
    return ret;
}

int NtdcmsApi::endjob()
{
    if(m_nJobNum < 0)
        return -1;

    jkEndNTDCMS(m_nJobNum);
    LOGLOG("end ntdcms job");
    m_nJobNum = -1;
    return 0;
}
