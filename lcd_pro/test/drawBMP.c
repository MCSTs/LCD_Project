#include "total.h"
#include <pthread.h>
#include <signal.h>

void ignore(int sig)
{
    return;
}

void *clickCheck(void *arg)
{
    bool *isclick = (bool *)arg;
    int fdScr = open("/dev/input/event0", O_RDONLY);
    if ( fdScr == -1 )
        sys_error("open dev");
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    int clickY = 0;

    while ( true )
    {
        int re = read(fdScr, &ev, sizeof(ev));
        if (re == -1)
            sys_error("read");
        if ( ev.type == EV_ABS ) {

            //if ( ev.code == ABS_X )
                //x = ev.value;
            if ( ev.code == ABS_Y )
                clickY = ev.value;
        }
        if ( ev.type==EV_KEY && ev.code==BTN_TOUCH 
                && ev.value == 0 ) {
            if ( clickY <= 538 ) {
                if ( *isclick == true )
                    *isclick = false;
                else
                    *isclick = true;
                kill(getpid(), SIGUSR2);
            }
        }
        //clickY = 0;
        //usleep(100);
    }

    int re = close(fdScr);
    if ( re == -1 )
        sys_error("close");

    return NULL;
}

int *lcdInit()
{
    int lcd_fd = open("/dev/fb0", O_RDWR);
    if ( lcd_fd == -1 )
        sys_error("open");

    int *p = (int *)mmap(NULL, LCDSIZE * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    if ( p == MAP_FAILED)
        sys_error("mmap");
    return p;
}

void drawBmp(int *p, const char *picNameBuf, int x0, int y0, bool *isclick)
{
    int fd = open(picNameBuf, O_RDONLY);
    if ( fd == -1 )
        sys_error("open bmp");

    unsigned char R, G, B, A;
    int color;

    int picWidth, picHight, pad, picLine;
    short picBitDeep;


    lseek(fd, 0x12, SEEK_SET);
    read(fd, &picWidth, sizeof(picWidth));
    picWidth = abs(picWidth);

    lseek(fd, 0x16, SEEK_SET);
    read(fd, &picHight, sizeof(picHight));
    picHight = abs(picHight);

    lseek(fd, 0x1c, SEEK_SET);
    read(fd, &picBitDeep, sizeof(picBitDeep));

    pad = 4 - picWidth * picBitDeep/8 % 4;
    if ( pad == 4 )
        pad = 0;
    picLine = picWidth * picBitDeep/8 + pad;


    unsigned char buf[picHight * picLine];
    memset(buf, 0, sizeof(buf));
    unsigned char *tmp = buf;

    lseek(fd, 0x36, SEEK_SET);
    read(fd, buf, sizeof(buf));

    int midBuf[HSIZE][WSIZE]={0};
    //p += (HSIZE-1) * WSIZE;
    for (int i=picHight-1; i>=0; i--) {
        for (int j=0; j<picWidth; j++) {
            B = *tmp++;
            G = *tmp++;
            R = *tmp++;
            if ( picBitDeep == 32 )
                A = *tmp++;
            else
                A = 0;
            color = (A << 24) | (R << 16) | (G << 8) | B;
            //drawPoint(j+x0, i+y0, p, color);
            if ( (j+x0)<0 || (j+x0)>=WSIZE || (i+y0)>=HSIZE || (i+y0)<0 ) {
                fprintf(stderr, "Over Screen Size...\n");
                exit(EXIT_FAILURE);
            }
            midBuf[i][j] = color;
            //*(p + (i+y0)*WSIZE + (j+x0)) = color;

        }
        tmp += pad;
    }


    signal(SIGUSR2, ignore);
    pthread_t tid;
    pthread_create(&tid, NULL, clickCheck, (void *)isclick);

    while ( true )
    {
        pause();

        while ( *isclick )
        {
            for ( int i=0; i<picHight; i++ ) 
            {
                for ( int j=0; j<picWidth; j++ )
                {
                    *(p + (i+y0)*WSIZE + (j+x0)) = midBuf[i][j];
                }
            }
        }
    }

    close(fd);
}
