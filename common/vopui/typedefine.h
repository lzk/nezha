#ifndef TYPEDEFINE_H
#define TYPEDEFINE_H
//0:No Security 1:WEP 64/128 bit 2.WPA-PSK-TKIP  3. WPA2-PSK-AES 4.Mixed Mode PSK
enum EncrypType  {NO_Securty, WEP, WPA_PSK_TKIP,  WPA2_PSK_AES, Mixed_Mode_PSK};
typedef struct {
   QString SSID;
   QString APStatus;
   QString Password;
   EncrypType  encryType;
   int wepKeyID;
} APInfo;

#endif // TYPEDEFINE_H
