#include "netio.h"
#include <QTcpSocket>
#include <QHostInfo>
#include <QUrl>
#include "api_libcups.h"
#include "log.h"

QHostAddress get_ip_address(const QString& host)
{
    QHostAddress hostAddress;

    hostAddress = QHostAddress(host);
    if(!hostAddress.isNull())
        return hostAddress;

    QHostInfo info;
    info = QHostInfo::fromName(host);
    if (!info.addresses().isEmpty()) {
        bool found = false;
        foreach (QHostAddress address, info.addresses()) {
            if(QAbstractSocket::IPv4Protocol == address.protocol()){
                hostAddress = address;
                found = true;
                break;
            }
        }
        if(!found){
            foreach (QHostAddress address, info.addresses()) {
                if(QAbstractSocket::IPv6Protocol == address.protocol()){
                    hostAddress = address;
                    found = true;
                    break;
                }
            }
        }
        if(!found)
            hostAddress = info.addresses().first();
//        qDebug()<<"host name:" << host << "addresses:" <<info.addresses();
    }
    return hostAddress;
}

NetIO::NetIO():
    tcpSocket(NULL)
{
}
NetIO::~NetIO()
{
    if(tcpSocket)
        delete tcpSocket;
}

QString NetIO::resolve_uri(const char* url)
{
    QString str_url(url);
    QUrl real_url;
    if(str_url.startsWith("dnssd://")){
        char uri[256];
        cups_resolve_uri(url ,uri ,sizeof(uri));
        real_url = QUrl(uri);
    }
    if(real_url.isEmpty() || real_url.host().isEmpty())
        real_url = QUrl(url);
    QString host = real_url.host();//resolve_uri(url);
//    LOGLOG("net io rosolve printer uri:%s" ,_url);
//    LOGLOG("net io rosolved printer uri:%s" ,url.host().toLatin1().constData());
    if(host.isEmpty())
        return QString();
    QHostAddress hostAddress = get_ip_address(host);

    return hostAddress.toString();
}

int NetIO::resolveUrl(const char* url)
{
    if(!url)
        return -1;
    QString str_url(url);
    if(!resolved_url.isEmpty() && ((device_uri[0] != 0) && !str_url.compare(device_uri))){
        return 0;
    }
    int ret = DeviceIO::resolveUrl(url);
    if(ret)
        return ret;
    QUrl real_url;
    if(str_url.startsWith("dnssd://")){
        char uri[256];
        cups_resolve_uri(url ,uri ,sizeof(uri));
        real_url = QUrl(uri);
    }
    if(real_url.isEmpty() || real_url.host().isEmpty())
        real_url = QUrl(url);
    QString host = real_url.host();//resolve_uri(url);
//    LOGLOG("net io rosolve printer uri:%s" ,_url);
//    LOGLOG("net io rosolved printer uri:%s" ,url.host().toLatin1().constData());
    if(host.isEmpty())
        return -1;
    hostAddress = get_ip_address(host);
    if(hostAddress.protocol() == QAbstractSocket::IPv6Protocol){
        resolved_url = QString("socket://[") + hostAddress.toString() +"]";
    }else{
        resolved_url = QString("socket://") + hostAddress.toString();
    }
    return 0;
}

int NetIO::type()
{
    return DeviceIO::Type_net;
}

int NetIO::open(int port)
{
    if(device_is_open){
        LOGLOG("device is opened");
        return -1;
    }
    if(hostAddress.toString().startsWith("fe80" ,Qt::CaseInsensitive)){
        LOGLOG("can not support fe80 address");
        return -1;
    }
    if(tcpSocket)
        delete tcpSocket;
    tcpSocket = new QTcpSocket;
//    if(!tcpSocket)
//        tcpSocket = new QTcpSocket;
    tcpSocket->connectToHost(hostAddress, port);
    if (!tcpSocket->waitForConnected(5000)) {
        LOGLOG("tcpsocket error code:%d",tcpSocket->error());
        LOGLOG("tcpsocket error:%s" ,tcpSocket->errorString().toLatin1().constData());
        tcpSocket->close();
        return -1;
    }
    device_is_open = true;
//    tcpSocket->setSocketOption(QAbstractSocket::LowDelayOption ,1);
//    qDebug()<<"option:"<<tcpSocket->socketOption(QAbstractSocket::LowDelayOption);
    return 0;

}

int NetIO::close(void)
{
    if(!tcpSocket)
        return -1;
//    tcpSocket->disconnectFromHost();
//    tcpSocket->waitForDisconnected();
//    tcpSocket->abort();
    if(device_is_open){
        device_is_open = false;
        tcpSocket->close();
    }
    return 0;
}

int NetIO::write(char *buffer, int bufsize)
{
    if(!device_is_open){
        return -1;
    }
    if(!tcpSocket)
        return -1;
//    int state = tcpSocket->state();
//    LOG_PARA("tcp_socket state:%d" ,state);
    int ret;
    ret = tcpSocket->write(buffer ,bufsize);

    if(!tcpSocket->waitForBytesWritten(15000)){
        LOGLOG("tcp_socket write timeout");
        return -1;
    }
    return ret;
}

int NetIO::read(char *buffer, int bufsize)
{
    if(!device_is_open){
        return -1;
    }
    if(!tcpSocket)
        return -1;
    int bytesAvailable;
#if 0
    do{
        bytesAvailable = tcpSocket->bytesAvailable();
        if(bytesAvailable < bufsize){
//            LOG_PARA("not enough bytesAvailable:%d,buffer size:%d",bytesAvailable ,bufsize);
//            if(!tcpSocket->waitForReadyRead(10)){
            if(!tcpSocket->waitForReadyRead(15000)){
                return -1;
            }
        }else{
            break;
        }
    }
    while(1);
//    LOG_PARA("bytesAvailable:%d,buffer size:%d",bytesAvailable ,bufsize);
    return tcpSocket->read(buffer ,bufsize);
#else
    int numRead = 0, numReadTotal = 0;
    do{
 //       if (!tcpSocket->waitForReadyRead(15000)){
 //           LOGLOG("%s" ,tcpSocket->errorString().toUtf8().constData());
 //           break;
 //       }
        bytesAvailable = tcpSocket->bytesAvailable();
        if(bytesAvailable <= 0){
            if (!tcpSocket->waitForReadyRead(30000)){
    //            qDebug()<<"tcp error:"<< tcpSocket->error();
    //            qDebug()<< tcpSocket->errorString();
                LOGLOG("reading error:" ,tcpSocket->errorString().toUtf8().constData());
                break;
            }
            bytesAvailable = tcpSocket->bytesAvailable();
        }
        if(bytesAvailable > bufsize - numReadTotal)
            bytesAvailable = bufsize - numReadTotal;
        if(bytesAvailable > 0){
            numRead = tcpSocket->read(buffer + numReadTotal ,bytesAvailable);
            if(numRead <= 0){
                break;
            }else{
                numReadTotal += numRead;
            }
        }else{
            break;
        }
    }while(numReadTotal < bufsize);
    if(numReadTotal != bufsize)
        return -1;
    return numReadTotal;
#endif
}

int NetIO::getDeviceId(char *buffer, int bufsize)
{
    return getDeviceId_without_open(buffer ,bufsize);
}

static int _platform_net_get_device_id(const QString& device_uri,char *buffer, int bufsize);
static int _platform_net_get_device_id(const QHostAddress& host_address,char *buffer, int bufsize);
int NetIO::getDeviceId_without_open(char *buffer, int bufsize)
{
    if(hostAddress.toString().startsWith("fe80" ,Qt::CaseInsensitive)){
//        LOGLOG("can not support fe80 address");
        return -1;
    }
//    return _platform_net_get_device_id(resolved_url ,buffer ,bufsize);
    return _platform_net_get_device_id(hostAddress ,buffer ,bufsize);
}

bool NetIO::isConnected()
{
    char buffer[1024];
    return !getDeviceId_without_open(buffer ,sizeof(buffer));
//    bool is_connected = open(9100) == 0 ?true :false;
//    if(is_connected)
//        close();
//    return is_connected;
}

const char* NetIO::getDeviceAddress()
{
    return hostAddress.toString().toLatin1().constData();
}

int NetIO::write_bulk(char *buffer, int bufsize)
{
    return write(buffer ,bufsize);
}

int NetIO::read_bulk(char *buffer, int bufsize)
{
    return read(buffer ,bufsize);
}

#if 0
//can not work well when ipv6,data only reach dest, can not back.
#include <QUdpSocket>
#include <unistd.h>
static int _platform_net_get_device_id(const QString& device_uri,char *buffer, int bufsize)
{
    QHostAddress host_address = get_ip_address(QUrl(device_uri).host());
//    LOGLOG("get device id from host address:%s" ,host_address.toString().toLatin1().constData());

    const unsigned char data[] =
    {0x30, 0x30, 0x02, 0x01, 0x01, 0x04, 0x06, 0x70, 0x75, 0x62, 0x6c, 0x69, 0x63, 0xa0, 0x23,
            0x02, 0x04, 0x63, 0xd5 ,0xdb ,0xc9 ,0x02, 0x01, 00, 0x02, 0x01, 00, 0x30, 0x15, 0x30, 0x13, 0x06,
            0x0f, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x95, 0x0b, 0x01, 0x02, 0x01, 0x02, 0x01,
     0x01, 0x03, 0x01, 0x05, 00};
//    const unsigned char data[] =
//    {0x30, 0x2d, 0x02, 0x01, 00, 0x04, 0x06, 0x70, 0x75, 0x62, 0x6c, 0x69, 0x63, 0xa0, 0x20,
//            0x02, 0x01, 0x04, 0x02, 0x01, 00, 0x02, 0x01, 00, 0x30, 0x15, 0x30, 0x13, 0x06,
//            0x0f, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x95, 0x0b, 0x01, 0x02, 0x01, 0x02, 0x01,
//     0x01, 0x03, 0x01, 0x05, 00};

    int bytes = sizeof(data);

    QUdpSocket udpSocket;
    udpSocket.open(QIODevice::ReadWrite);
    udpSocket.connectToHost(host_address ,161);
    if (!udpSocket.waitForConnected(5000)) {
        LOGLOG("udp connect fial!");
        return -1;
    }
    int ret;
    ret = udpSocket.write((const char*)data ,bytes);
//    ret = udpSocket.writeDatagram((const char*)data ,bytes ,host_address ,161);
    if(ret != bytes){
        LOGLOG("udp write fial!");
        return -1;
    }

    if(!udpSocket.waitForReadyRead(5000)){
//        LOGLOG("udp no data to read!");
        return -1;
    }
    QByteArray ba;
    while (udpSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket.pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udpSocket.readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);
        ba += datagram;
    }
    udpSocket.close();

    ret = -1;
    int index = ba.indexOf("MFG");
    if(index > 0){
        ret = 0;
        memcpy(buffer ,ba.data() + index ,ba.size() - index -1);
    }
    return ret;
}
#else
#include "snmpapi.h"
static int _platform_net_get_device_id(const QString& device_uri,char *buffer, int bufsize)
{
    //some host name can not get device id. change to ipv4 first.
//    return snmpGetDeviceID(resolved_url.toLatin1().constData() ,buffer ,bufsize);
    QHostAddress host_address = get_ip_address(QUrl(device_uri).host());
    QString address;
    if(host_address.protocol() == QAbstractSocket::IPv6Protocol){
        address= QString("udp6:[") + host_address.toString() +"]";
    }else{
        address= host_address.toString();
    }
    return snmp_get_deviceid(address.toLatin1().data() ,buffer ,bufsize);
}
static int _platform_net_get_device_id(const QHostAddress& host_address,char *buffer, int bufsize)
{
    QString address;
    if(host_address.protocol() == QAbstractSocket::IPv6Protocol){
        address= QString("udp6:[") + host_address.toString() +"]";
    }else{
        address= host_address.toString();
    }
    return snmp_get_deviceid(address.toLatin1().data() ,buffer ,bufsize);
}
#endif
