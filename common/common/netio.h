#ifndef NETIO_H
#define NETIO_H

#include <QHostAddress>
#include "deviceio.h"
class QTcpSocket;
class NetIO :public DeviceIO
{
public:
    NetIO();
    ~NetIO();

    virtual int type();
    virtual int close(void);
    virtual int write(char *buffer, int bufsize);
    virtual int read(char *buffer, int bufsize);
    virtual int getDeviceId_without_open(char* ,int);
    virtual int write_bulk(char *buffer, int bufsize);
    virtual int read_bulk(char *buffer, int bufsize);

protected:
    virtual int open(int port = 9100);
    virtual bool isConnected();
    virtual int getDeviceId(char *buffer, int bufsize);
    virtual int resolveUrl(const char* url);
    virtual const char* getDeviceAddress();

private:
    QTcpSocket* tcpSocket;
    QHostAddress hostAddress;
    QString resolved_url;
};
#endif // NETIO_H
