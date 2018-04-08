#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <linux/input.h>
#include <stdio.h>
#include <termios.h>
#include <poll.h>
#include "log.h"

#define GPSPTS  "/data/getnmea"
#define MAXNMEA  1024

int myexit = 0;
int gpsptsfd;

void handler_sig(int sig)
{
     if(sig == SIGINT)
       myexit = 1;
}

static int initPtsGps()
{

    gpsptsfd = open(GPSPTS, O_RDWR);
    if(gpsptsfd < 0)
    {
        ERROR("open %s failed.\n", GPSPTS);
        return -1;
    }
    return 0;
}

static int readPtsGps(char *pbuf)
{
    int len = 0;
    struct pollfd  fds[1];
   
    if(gpsptsfd > 0)
    {
        fds[0].fd = gpsptsfd;
        fds[0].events = POLLIN;
        
        if(poll(fds, 1, -1) < 0)
        {
           ERROR("poll failed.\n");
           return len;
        }
        len = read(gpsptsfd, pbuf, MAXNMEA);
    }
    return len;
}

static void closePtsGps()
{
    if(gpsptsfd > 0)
    {
        close(gpsptsfd);
    }
}

int main(void)
{
    char buf[MAXNMEA];
    int len;

    klog_init();

    klog_set_level(KLOG_INFO_LEVEL);
    INFO("starting test gps read....");
    
    if(initPtsGps() < 0)
    {
        return -1;
    }

    signal(SIGINT, handler_sig);
    memset(buf, 0, MAXNMEA);

    while(myexit == 0)
    {
        //if((len = readPtsGps(buf)) > 0)        
        len = readPtsGps(buf);   
        if(len > 1)    
          printf("len:%d, nmea:%s\n", len, buf);
        else 
          printf("len:%d, nmea:0x%x\n", len, buf[0]);
        memset(buf, 0, MAXNMEA);
    }

    closePtsGps();

    return 0;
}
