#include "status.h"
#include "jkinterface.h"
#include <string.h>
#include <stdlib.h>
static int _base64_char_value(char base64char)
 {
    if (base64char >= 'A' && base64char <= 'Z')
         return base64char-'A';
    if (base64char >= 'a' && base64char <= 'z')
         return base64char-'a'+26;
    if (base64char >= '0' && base64char <= '9')
         return base64char-'0'+2*26;
    if (base64char == '+')
         return 2*26+10;
    if (base64char == '/')
         return 2*26+11;
    return -1;
}

static int _base64_decode_triple(char quadruple[4], unsigned char *result)
 {
    int i, triple_value, bytes_to_decode = 3, only_equals_yet = 1;
    int char_value[4];

    for (i=0; i<4; i++)
         char_value[i] = _base64_char_value(quadruple[i]);

    for (i=3; i>=0; i--)
    {
         if (char_value[i]<0)
         {
             if (only_equals_yet && quadruple[i]=='=')
             {
                  char_value[i]=0;
                  bytes_to_decode--;
                  continue;
             }
             return 0;
         }
         only_equals_yet = 0;
    }

    if (bytes_to_decode < 0)
         bytes_to_decode = 0;

    triple_value = char_value[0];
    triple_value *= 64;
    triple_value += char_value[1];
    triple_value *= 64;
    triple_value += char_value[2];
    triple_value *= 64;
    triple_value += char_value[3];

    for (i=bytes_to_decode; i<3; i++)
         triple_value /= 256;
    for (i=bytes_to_decode-1; i>=0; i--)
    {
         result[i] = triple_value%256;
         triple_value /= 256;
    }

    return bytes_to_decode;
}

static size_t Base64Decode(char *source, unsigned char *target, size_t targetlen)
 {
    char *src, *tmpptr;
    char quadruple[4], tmpresult[3];
    int i; size_t tmplen = 3;
    size_t converted = 0;

    src = (char *)malloc(strlen(source)+5);
    if (src == NULL)
         return -1;
    strcpy(src, source);
    strcat(src, "====");
    tmpptr = src;
    while (tmplen == 3)
    {
         /* get 4 characters to convert */
         for (i=0; i<4; i++)
         {
             while (*tmpptr != '=' && _base64_char_value(*tmpptr)<0)
                  tmpptr++;
             quadruple[i] = *(tmpptr++);
         }
         tmplen = _base64_decode_triple(quadruple, (unsigned char*)tmpresult);
         if (targetlen < tmplen)
         {
             free(src);
             return converted;
         }
         memcpy(target, tmpresult, tmplen);
         target += tmplen;
         targetlen -= tmplen;
         converted += tmplen;
    }
    free(src);
    return converted;
}

int DecodeStatusFromDeviceID(char* device_id, PRINTER_STATUS* status)
{
    if (device_id==NULL || status==NULL) {
        return -1;
    }
    char *p = device_id;

    while (*p && strncmp(p,"STS:",4)!=0) // Look for "STS:"
        p++;

    if (!*p)	{ // "STS:" not found
        LOGLOG("STS: not found");
        return -1;
    }
    p += 4;	// Skip "STS:"

    if (Base64Decode(p, (unsigned char*)status, sizeof(PRINTER_STATUS)) != sizeof(PRINTER_STATUS))
    {
//        return -1;
    }
    return 0;
}

//int getStatusFromDevice(DeviceIO* device ,PRINTER_STATUS* ps)
//{
//    if(!device || !ps){
//        return -1;
//    }
//    int ret = 0;
//    char buffer[1024];
//    ret = device->getDeviceId(buffer ,sizeof(buffer));
//    if(!ret){
////        LOGLOG("device id:%s" ,buffer);
//        ret = DecodeStatusFromDeviceID(buffer ,ps);
//    }
//    return ret;
//}

int getStatusFromDevice(DeviceIO* device ,Printer_struct* printer ,PRINTER_STATUS* ps)
{
    if(!device || !printer || !ps){
        return -1;
    }
    int ret = 0;
    char buffer[1024];
    memset(buffer ,0 ,sizeof(buffer));
    ret = device->getDeviceId(printer ,buffer ,sizeof(buffer));
    if(!ret){
//        LOGLOG("device get device id size:%d :%s" ,strlen(buffer) ,buffer);
        ret = DecodeStatusFromDeviceID(buffer ,ps);
//        LOGLOG("get device status:0x%02x" ,ps->PrinterStatus);
    }else{
//        LOGLOG("can not get device status");
    }
    return ret;
}

int getStatusFromDevice(DeviceIOManager* device_manager ,Printer_struct* printer ,PRINTER_STATUS* ps)
{
    if(!device_manager || !printer || !ps){
        return -1;
    }
    DeviceIO* device = device_manager->getDevice(printer);
    return getStatusFromDevice(device ,printer ,ps);
}
