#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <linux/input.h>
#include <stdio.h>
#include "log.h"

int myexit = 0;

void handler_sig(int sig)
{
     if(sig == SIGINT)
       myexit = 1;
}

//int main(int argc, char **argv)
int main(void)
{
    int fd;
    struct input_event  input; 

    klog_init();

    klog_set_level(KLOG_INFO_LEVEL);
    INFO("starting bt_blink....");
    /*
    if(argc < 2)
    {
        ERROR("bt_blink read input parameter error.\n");
        return -1;
    }
    */
    signal(SIGINT, handler_sig);
    /*
    fd = open(argv[1], O_RDONLY);
    if(fd < 0)
    {
        printf("open %s failed.\n", argv[1]);
        return -1;
    }
    */
    while(myexit == 0)
    {
        //read(fd, &input, sizeof(struct input_event));
        //printf("type:%d, code:%d, value:%d\n", input.type, input.code, input.value);
        sleep(1);
    }

    //close(fd);

    return 0;
}
