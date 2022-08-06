#include "interface.h"

void sys_error(const char *mes)
{
    perror(mes);
    exit(EXIT_FAILURE);
}

void InterfaceSelect( int *p, const char *picNameBuf, int x0, int y0 )
{
    int fd = open("/dev/input/event0", O_RDONLY);
    if ( fd == -1 )
        sys_error("open");

    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    int x, y, startX, endX;
    int startY, endY;

    while ( true ) {
        drawBmp(p, picNameBuf, 0, 0);

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
            startY = y;
        }
        if ( ev.type==EV_KEY && ev.code==BTN_TOUCH && ev.value == 0 )
        {
            endX = x;
            endY = y;
            // Select program
            if ( startY>=265 && startY<=456 ) {

                // Play Video
                if ( startX>=64 && startX<=205 ) {
                    printf("Execute ./PlayVideo\n");
                    system("./PlayVideo");
                }

                // Play Picture
                else if ( startX>=412 && startX<=550 ) {
                    printf("Execute ./PlayMode\n");
                    system("./PlayMode");
                }

                // Game
                else if ( startX>=780 && startX<=915 ) {
                    printf("Execute ./Game\n");
                }
            }

            // Quit
            else if ( startX>=940 && startY<=63 ) {
                printf("Quit\n");
                break;
            }
        }
        //x = y = startX = startY = endX = endY = 0;
    }

    close(fd);
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

void drawBmp(int *p, const char *picNameBuf, int x0, int y0)
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
    for ( int i=0; i<picHight; i++ ) 
    {
        for ( int j=0; j<picWidth; j++ )
        {
            *(p + (i+y0)*WSIZE + (j+x0)) = midBuf[i][j];
        }
    }
    close(fd);
}
