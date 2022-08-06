#include "lcd.h"
#include "list.h"
#include <termios.h>
#include <linux/input.h>

static bool FLAG=true;

void clickQuit(int sig)
{
    FLAG = false;
    return;
}

void ignore(int sig)
{
    return;
}

void autoSwitch(int fdFile, int *p, ListNode **cur)
{
    signal(SIGUSR2, clickQuit);
    signal(SIGUSR1, ignore);

    printf("Show Automatically...\n");
    printf("Click Screen To Quit...\n");
    char act;
    int re;
    while ( FLAG )
    {
        lseek(fdFile, 0, SEEK_SET);
        re = read(fdFile, &act, sizeof(act));
        if ( re == -1 )
            sys_error("read into aotoSwitch");
        if ( act == '1' ) {
            lseek(fdFile, 0, SEEK_SET);
            re = write(fdFile, "0", sizeof("0"));
            if (re == -1)
                sys_error("write into aotoSwitch");
            return;
        }
        printf("Showing Img: %s\n", (*cur)->fileName);
        drawBmp(p, (*cur)->fileName, 0, 0);
        *cur = (*cur)->next;

        printf("pause start...\n");
        pause();
        printf("pause end...\n");
        //sleep(3);
    }
    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);
}



void manSwitch(int *p, ListNode **cur)
{

    struct termios termios_p;
    struct termios term_src;
    tcgetattr(STDIN_FILENO, &termios_p);
    term_src = termios_p;
    termios_p.c_lflag &= ~ECHO;
    termios_p.c_lflag &= ~ICANON;

    // start
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_p);

    (*cur) = (*cur)->next;
    drawBmp(p, (*cur)->fileName, 0, 0);
    
    char act;
    int flag = 1;
    printf("Manualy switch...\n'j'--> next\n'k'--> pre\n'q'--> quit\n");
    while ( flag )
    {
        int re = read(STDIN_FILENO, &act, 1);
        if ( re == -1 )
            sys_error("read");
        switch(act) {
            case 'j': 
                (*cur) = (*cur)->next;
                drawBmp(p, (*cur)->fileName, 0, 0);
                break;
            case 'k':
                (*cur) = (*cur)->pre;
                drawBmp(p, (*cur)->fileName, 0, 0);
                break;
            case 'q':
                flag = 0;
                break;
        }
    }

    // end
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term_src);

}


void showIMG(int fdFile, int *p, ListHead *head)
{

    struct termios termios_p;
    struct termios term_src;
    tcgetattr(STDIN_FILENO, &termios_p);
    term_src = termios_p;
    termios_p.c_lflag &= ~ECHO;
    termios_p.c_lflag &= ~ICANON;

    // start
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_p);


    char act;
    int flag = true;
    ListNode *cur = head->head;

    while ( flag ) {
        printf("Show Image...\n");
        printf("'m' to manualy show\n");
        printf("'a' to autoly show\n");
        printf("'t' to touch switching...\n");
        printf("'q' to quit\n");
        FLAG = true;
        //usleep(100);
        read(STDIN_FILENO, &act, 1);
        switch(act) {
            case 'm': 
                manSwitch(p, &cur);
                break;
            case 'a': 
                autoSwitch(fdFile, p, &cur);
                break;
            case 't': 
                switchTouch(p, &cur);
                break;
            case 'q': 
                flag = false;
                break;
        }
    }

    // end
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term_src);

}


void HomePage(int *p, const char *picName, ListHead *head, int fdFile)
{
    ListNode *cur = head->head;
    bool flag = true;
    //int isHomePage = 0;

    int fd = open("/dev/input/event0", O_RDONLY);
    if ( fd == -1 )
        sys_error("open");
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    //int x, y;
    int x, y, startX, endX;
    int startY, endY;

    while ( flag ) {

        FLAG = true;
        drawBmp(p, picName, 0, 0);

        int re = read(fd, &ev, sizeof(ev));
        if (re == -1)
            sys_error("read");

        if ( ev.type == EV_ABS ) {
            if ( ev.code == ABS_X )
                x = ev.value;
            if ( ev.code == ABS_Y )
                y = ev.value;
        }
        if ( ev.code==BTN_TOUCH && ev.value!=0) 
        {
            startX = x;
            startY= y;
        }
        if ( ev.type==EV_KEY && ev.code==BTN_TOUCH && ev.value == 0 ) 
        {
            endX = x;
            endY= y;

            if ( startX>=352 && startX<=601 && startY>=240 && startY<=282 ) {
                autoSwitch(fdFile, p, &cur);
                printf("autoSwitch done\n");
            }
            //else if ( startX>=352 && endX<=400 && startY>=253 && startY<=276 )
            else if ( startX>=352 && startX<=525 && startY>=312 && startY<=350 )
                manSwitch(p, &cur);
            //else if ( startX>=352 && endX<=349 && startY>=306 && startY<=336 )
            else if ( startX>=352 && startX<=455 && startY>=380 && startY<=420 )
                switchTouch(p, &cur);
            else if ( startX>=352 && startX<=455 && startY>=450 && startY<=490 ) {
                flag = false;
                //drawBmp(p, picName, 0, 0);
                //break;
            }
            startX = startY = endX = endY = x = y = 0;
            memset(&ev, 0, sizeof(ev));
        }
    }
}
