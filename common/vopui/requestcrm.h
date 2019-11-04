#ifndef REQUESTCRM_H
#define REQUESTCRM_H
#define NEW_CRM 1
#include <QString>
class RequestCRM
{
public:
    RequestCRM();

    static int verify_mobilecode(const QString& phonenumber ,const QString& verifycode);
    static int send_verifycode(const QString& phonenumber);
    static int get_userinfo(const QString& phonenumber ,QString& hyperlink);
    static int append_PrinterData(const QString& post_str);

private:
    static QString get(const QString& ,const QString&);
    static QString post(const QString& ,const QString&);
    static QString get_tocken();
};

#endif // REQUESTCRM_H
