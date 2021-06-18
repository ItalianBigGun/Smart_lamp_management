#ifndef LEDDEV_H
#define LEDDEV_H
#include <QString>
#include "test.pb.h"
extern "C"
{
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>

    #include <unistd.h>
}

#define TEST_MAGIC 'x'                    //定义幻数

#define LED1 _IO(TEST_MAGIC, 0)
#define LED2 _IO(TEST_MAGIC, 1)
#define LED3 _IO(TEST_MAGIC, 2)
#define LED4 _IO(TEST_MAGIC, 3)

#define LED_ON  	0	//灯亮
#define LED_OFF		1   //灯灭

class LedDev
{
public:
    LedDev();
    int openLed(QString n);
    int closeLed(QString n);
    my::Node dev_node;

private:
    int fd;
};

#endif // LEDDEV_H
