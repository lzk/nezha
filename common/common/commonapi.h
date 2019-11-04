#ifndef COMMONAPI_H
#define COMMONAPI_H
#include <QString>
#include <QVariant>
#include "serverthread.h"
#include "devicemanager.h"
#include "usbio.h"
#include "netio.h"
extern const char* g_config_file;
bool appSettings(const QString& key ,QVariant& value ,const QVariant& defaultValue ,bool set = false);

bool is_app_running(const char* server_path);
QString get_string_from_shell_cmd(const QString& cmd ,int mode = 0);

bool printer_is_printing(const QString& printer_name);
bool is_disk_no_space(const char* path ,unsigned int prefer_size);
#endif // COMMON_H
