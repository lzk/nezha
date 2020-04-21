#ifndef SNMPAPI_H
#define SNMPAPI_H

typedef void (*callback_snmp)(const char* ip ,char* buffer ,int bufsize ,void* data);
int snmp_get_deviceid(char* ip ,char* buffer ,int bufsize);
int snmp_get_deviceid_broadcast(char *ip, char *buffer, int bufsize ,callback_snmp callback ,void* data);
int snmp_broadcast(char**ips, int numOfips ,char *buffer, int bufsize
                   ,callback_snmp callback ,void* data);
int snmp_broadcast6(char**ips, int numOfips ,char *buffer, int bufsize
                   ,callback_snmp callback ,void* data);
#endif // SNMPAPI_H
