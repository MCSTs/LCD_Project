#include "lcd.h"
#include <linux/input.h>

void touchEvent(int fdFile, pid_t parID)
{
    int fd = open("/dev/input/event0", O_RDONLY);
    if ( fd == -1 )
        sys_error("open dev");

    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    while ( true ) {
        int re = read(fd, &ev, sizeof(ev));
        if (re == -1)
            sys_error("read");

        //if ( ev.type == EV_ABS ) {
        if ( ev.type==EV_KEY && ev.code==BTN_TOUCH && ev.value == 0 ) {

            kill(parID, SIGUSR2);

            //re = lseek(fdFile, 0, SEEK_SET);
            //if (re == -1)
                //sys_error("lseek");
            //re = write(fdFile, "1", strlen("1"));
            //if (re == -1)
                //sys_error("write");
        }
    }
    close(fd);
}


void switchTouch(int *p, ListNode **cur)
{
    printf("Show Img touching...\n");
    int start=0, end=0, x;
    int startY=0, endY=0, y;
    int fd = open("/dev/input/event0", O_RDONLY);
    if ( fd == -1 )
        sys_error("open");

    (*cur) = (*cur)->pre;
    drawBmp(p, (*cur)->fileName, 0, 0);

    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    while ( true ) {
        while ( true ) {
            int re = read(fd, &ev, sizeof(ev));
            if ( re == -1 )
                sys_error("read");
            if ( ev.type == EV_ABS ) {
                if ( ev.code == ABS_X )
                    x = ev.value;
                if (ev.code == ABS_Y)
                    y = ev.value;
            }

            if ( ev.code==BTN_TOUCH && ev.value!=0) {
                start = x;
                startY = y;
            }
            if (ev.type==EV_KEY && ev.code==BTN_TOUCH && ev.value==0) {
                end = x;
                endY = y;
                break;
            }
        }
        if ( abs(end-start)>=100 ) {
            if ( end > start && abs(startY-endY)<100 ) {
                (*cur) = (*cur)->next;
                drawBmp(p, (*cur)->fileName, 0, 0);
            }
            else if ( end < start && abs(startY-endY)<100 )
            {
                (*cur) = (*cur)->pre;
                drawBmp(p, (*cur)->fileName, 0, 0);
            }
            if ( abs(startY-endY)>=100) {
                break;
            }

        }
    }
    close(fd);
}
