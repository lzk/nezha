#include "devicemanager.h"
#include "jkinterface.h"
#include "usbio.h"
#include "netio.h"
DeviceManager::DeviceManager()
    :device(NULL)
    ,usbIO(new UsbIO)
    ,netIO(new NetIO)
{

}

DeviceManager::~DeviceManager()
{
    delete usbIO;
    delete netIO;
}

DeviceIO* DeviceManager::getDevice(Printer_struct* printer)
{
    int type = getDeviceType(printer->deviceUri);
    switch (type) {
    case DeviceIO::Type_usb:
        device = usbIO;
        break;

    case DeviceIO::Type_net:
        device = netIO;
        break;

    default:
        device = NULL;
        break;
    }
    if(device_uri.compare(printer->deviceUri)){
//        LOGLOG("update device:%s" ,device_uri);
        this->device_uri = printer->deviceUri;
        if(device){
            device->resolve(printer);
        }
    }
    return device;
}

int DeviceManager::getDeviceType(const char* device_uri)
{
    int type = DeviceIO::Type_unknown;
    if(!strncmp(device_uri,"usb://" ,6)){
        type = DeviceIO::Type_usb;
    }else{
        type = DeviceIO::Type_net;
    }
    return type;
}

int DeviceManager::get_device_connect_status(Printer_struct* printer)
{
    DeviceIO* device = new_device(printer);
    if(device){
        device->resolve(printer);
        printer->isConnected = device->isConnected(printer);
        strcpy(printer->connectTo
           ,device->getDeviceAddress(printer));
        delete device;
    }
    return 0;
}

DeviceIO* DeviceManager::new_device(Printer_struct* printer)
{
    int type = getDeviceType(printer->deviceUri);
    DeviceIO* device;
    switch (type) {
    case DeviceIO::Type_usb:
        device = new UsbIO;
        break;

    case DeviceIO::Type_net:
        device = new NetIO;
        break;

    default:
        device = NULL;
        break;
    }
    return device;
}
