#include "leddev.h"
#include <QDebug>
LedDev::LedDev()
{
    fd = open("/dev/Led",O_RDWR);
    if(fd==-1){
        qDebug()<<"open led failed";
    }
    my::Message *message;
    my::DevNode *dev;
    message = dev_node.mutable_m_message();
}

int LedDev::openLed(QString n)
{
    if(n=="LED1"){
        ioctl(fd,LED1,LED_ON);
        return 1;
    }else{
        if(n=="LED2"){
            ioctl(fd,LED2,LED_ON);
            return 1;
        }else{
            if(n=="LED3"){
                ioctl(fd,LED3,LED_ON);
                return 1;
            }else{
                if(n=="LED4"){
                    ioctl(fd,LED4,LED_ON);
                    return 1;
                }else{
                    return 0;
                }
            }
        }
		return 0;
    }
    return 0;
}

int LedDev::closeLed(QString n)
{
    if(n=="LED1"){
        ioctl(fd,LED1,LED_OFF);
        return 1;
    }else{
        if(n=="LED2"){
            ioctl(fd,LED2,LED_OFF);
            return 1;
        }else{
            if(n=="LED3"){
                ioctl(fd,LED3,LED_OFF);
                return 1;
            }else{
                if(n=="LED4"){
                    ioctl(fd,LED4,LED_OFF);
                    return 1;
                }else{
                    return 0;
                }
            }
        }
		return 0;
    }
    return 0;
}
