#ifndef NTDCMSAPI_H
#define NTDCMSAPI_H

#include "ntdcmslib.h"
#include "imagetrans.h"

class NtdcmsApi
{
public:
    NtdcmsApi();

    int caculate_parameter(ImageTransInfo* image_trans_info);
    int process(unsigned char* target ,unsigned char* source ,int band_size = 1 ,int line_index = 0);
    int endjob();

private:
    int m_nJobNum;
    NTDCOLOR m_ntdColor;
    ADJUSTSTR m_adjuststr;
};

#endif // NTDCMSAPI_H
