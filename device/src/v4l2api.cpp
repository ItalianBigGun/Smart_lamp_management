#include "v4l2api.h"
#include <QDebug>
#include <QImage>
#include <math.h>
V4l2Api::V4l2Api(const char *dname, int count):deviceName(dname),count(count)
{
    adjustLum = 0;
    adjustRed = 0;
    adjustGreen = 0;
    adjustBlue = 0;
    switchflag = 0;
    videoflag = 0;
    mask.pos.setX(0);
    mask.pos.setY(0);
    mask.filepath="./logo.png";
    avino = 0;
    this->open();
}

V4l2Api::~V4l2Api() 
{
    this->close();
}

void V4l2Api::open()
{
    video_init();

    video_mmap();
#if 1
    //开始采集
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int ret  =  ioctl(this->vfd, VIDIOC_STREAMON, &type);
    if(ret < 0)
    {
        perror("start fail");
    }
#endif
}

void V4l2Api::close()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int ret  =  ioctl(this->vfd, VIDIOC_STREAMOFF, &type);
    if(ret < 0)
    {
        perror("stop fail");
    }
    //释放映射
    for(int i=0; i<this->framebuffers.size(); i++)
    {
        munmap(framebuffers.at(i).start, framebuffers.at(i).length);
    }
    ::close(this->vfd);
}



void V4l2Api::grapImage(char *imageBuffer, int *length)
{
    struct v4l2_buffer readbuf;
    readbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    readbuf.memory = V4L2_MEMORY_MMAP;
    //perror("read");
    if(ioctl(this->vfd, VIDIOC_DQBUF, &readbuf)<0)//取一针数据
    {
        perror("read image fail");
    }
    *length = readbuf.length;
    memcpy(imageBuffer,framebuffers[readbuf.index].start, framebuffers[readbuf.index].length);

    //把用完的队列空间放回队列中重复使用
    if(ioctl(vfd, VIDIOC_QBUF, &readbuf)<0)
    {
        perror("destroy fail");
        exit(1);
    }
}

void V4l2Api::video_init()
{
    //1.打开设备
    this->vfd = ::open(deviceName.c_str(), O_RDWR);
    if(this->vfd < 0)
    {
        perror("open fail");
        VideoException vexp("open fail");//创建异常对象
        //抛异常
        throw vexp;
    }
    //2.配置采集属性
    struct v4l2_format vfmt;
    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; //
    vfmt.fmt.pix.width = WIDTH;
    vfmt.fmt.pix.height = HEIGHT;
    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;//（设置视频输出格式，但是要摄像头支持4:2:2）
    //通过ioctl把属性写入设备
    int ret  = ioctl(this->vfd, VIDIOC_S_FMT, &vfmt);
    if(ret < 0)
    {
        VideoException vexp("set fail");//创建异常对象
        throw vexp;
    }
    //通过ioctl从设备获取属性
    memset(&vfmt, 0, sizeof(vfmt));
    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(this->vfd, VIDIOC_G_FMT, &vfmt);
    if(ret < 0)
    {
        VideoException vexp("get fail");//创建异常对象
        throw vexp;
    }

    if(vfmt.fmt.pix.width == WIDTH && vfmt.fmt.pix.height==HEIGHT
            && vfmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV)
    {

    }else {
        VideoException vexp("set error");//创建异常对象
        throw vexp;
    }
}

void V4l2Api::video_mmap()
{
    //1申请缓冲区队列
    struct v4l2_requestbuffers reqbuffer;
    reqbuffer.count = this->count;//申请缓冲区队列长度
    reqbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuffer.memory = V4L2_MEMORY_MMAP;
    int ret = ioctl(this->vfd, VIDIOC_REQBUFS, &reqbuffer);
    if(ret < 0)
    {
        VideoException vexp("req buffer fail");//创建异常对象
        throw vexp;
    }

    //2.映射
    for(int i=0; i<this->count; i++)
    {
        struct VideoFrame frame;

        struct v4l2_buffer mapbuffer;
        mapbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        mapbuffer.index = i;
        mapbuffer.memory = V4L2_MEMORY_MMAP;
        //从队列中拿到内核空间
        ret  = ioctl(this->vfd, VIDIOC_QUERYBUF, &mapbuffer);
        if(ret < 0)
        {
            perror("query fail");
        }
        //映射
        frame.length = mapbuffer.length;
        frame.start = (char *)mmap(NULL, mapbuffer.length, PROT_READ|PROT_WRITE, MAP_SHARED, this->vfd, mapbuffer.m.offset);

        //空间放回队列中（内核空间）
        ret = ioctl(this->vfd, VIDIOC_QBUF, &mapbuffer);

        //把frame添加到容器framebuffers
        framebuffers.push_back(frame);
    }
}

//将YUYV数据转成RGB,含有调色功能
bool V4l2Api::yuyv_to_rgb888(unsigned char *yuyvdata, unsigned char *rgbdata, int picw, int pich)
{
    int i, j;
    unsigned char y1,y2,u,v;
    int r1,g1,b1,r2,g2,b2;

    //确保所转的数据或要保存的地址有效
    if(yuyvdata == NULL || rgbdata == NULL)
    {
        return false;
    }

    int tmpw = picw/2;
    for(i=0; i<pich; i++)
    {
        for(j=0; j<tmpw; j++)// 640/2  == 320
        {
            //yuv422
            //R = 1.164*(Y-16) + 1.159*(V-128);
            //G = 1.164*(Y-16) - 0.380*(U-128)+ 0.813*(V-128);
            //B = 1.164*(Y-16) + 2.018*(U-128));

            //下面的四个像素为：[Y0 U0 V0] [Y1 U1 V1] -------------[Y2 U2 V2] [Y3 U3 V3]
            //存放的码流为：    Y0 U0 Y1 V1------------------------Y2 U2 Y3 V3
            //映射出像素点为：  [Y0 U0 V1] [Y1 U0 V1]--------------[Y2 U2 V3] [Y3 U2 V3]

            //获取每个像素yuyv数据   YuYv
            y1 = *(yuyvdata + (i*tmpw+j)*4);                //yuv像素的Y
            u  = *(yuyvdata + (i*tmpw+j)*4+1);              //yuv像素的U
            y2 = *(yuyvdata + (i*tmpw+j)*4+2);
            v  = *(yuyvdata + (i*tmpw+j)*4+3);

            //qDebug()<<"r g b:"<<adjustRed<<adjustGreen<<adjustBlue;
            //把yuyv数据转换为rgb数据
            r1 = y1 + 1.042*(v-128) + adjustRed;
            g1 = y1 - 0.34414*(u-128) +adjustGreen;
            b1 = y1 + 1.772*(u-128)+adjustBlue;

            r2 = y2 + 1.042*(v-128) + adjustRed;
            g2 = y2 - 0.34414*(u-128) + adjustGreen;
            b2 = y2 + 1.772*(u-128) + adjustBlue;

            if(r1 > 255) r1=255;
            else if(r1 < 0) r1 = 0;

            if(g1 > 255) g1=255;
            else if(g1 < 0) g1 = 0;

            if(b1 > 255) b1=255;
            else if(b1 < 0) b1 = 0;

            if(r2 > 255) r2=255;
            else if(r2 < 0) r2 = 0;

            if(g2 > 255) g2=255;
            else if(g2 < 0) g2 = 0;

            if(b2 > 255) b2=255;
            else if(b2 < 0) b2 = 0;

            //把rgb值保存于rgb空间 数据为反向
            rgbdata[((i)*tmpw+j)*6]     = (unsigned char)r1;
            rgbdata[((i)*tmpw+j)*6 + 1] = (unsigned char)g1;
            rgbdata[((i)*tmpw+j)*6 + 2] = (unsigned char)b1;
            rgbdata[((i)*tmpw+j)*6 + 3] = (unsigned char)r2;
            rgbdata[((i)*tmpw+j)*6 + 4] = (unsigned char)g2;
            rgbdata[((i)*tmpw+j)*6 + 5] = (unsigned char)b2;
        }
    }
    return true;
}

//控制摄像开关函数
void V4l2Api::setStart(int flag)
{
    this->switchflag = flag;
}



//采集视频线程函数
void V4l2Api::run()
{
    static int i = 0;
    VideoWriter writer;
    int len;
    char buffer[WIDTH*HEIGHT*3];
    char rgbbuffer[WIDTH*HEIGHT*3];
    while(switchflag)
    {
        grapImage((char *)buffer, &len);
        yuyv_to_rgb888((unsigned char *)buffer, (unsigned char *)rgbbuffer,WIDTH,HEIGHT);
        QImage image((unsigned char *)rgbbuffer,WIDTH,HEIGHT,QImage::Format_RGB888);
        //发送图像数据给主线程显示
        emit sendImage(image);

        msleep(40);
    }
}
