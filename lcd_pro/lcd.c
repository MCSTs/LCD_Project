#include "lcd.h"
#include <pthread.h>

int *lcdInit()
{
    int lcd_fd = open("/dev/fb0", O_RDWR);
    if ( lcd_fd == -1 )
        sys_error("open");

    int *p = mmap(NULL, LCDSIZE * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    if ( p == MAP_FAILED)
        sys_error("mmap");

    return p;
}



void lcdClear(int *p, int color)
{
    for (int i=0; i<LCDSIZE; i++) {
        p[i] = color;
    }
}

void drawRectangle(int *p, int x, int y, int height, int width, int color)
{
    p = p + WSIZE * y + x;
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
            *(p++ + j) = color; 
        p = p + WSIZE-width;
    }
}

void drawCircle(int *p, int x, int y, int r, int color)
{
    for (int i=0; i<HSIZE; i++)
        for ( int j=0; j<WSIZE; j++ )
            if ( (x-j)*(x-j)+(y-i)*(y-i) <=r*r )
                *(p + i*WSIZE + j) = color;
}



void drawAnc(int *p, int x, int y, int r)
{
    int *tmp = p;
    for (int i=0; i<HSIZE; i++)
        for ( int j=0; j<WSIZE; j++ )
        {
            int sum = (x-j)*(x-j)+(y-i)*(y-i);
            int top = (x-j)*(x-j)+((y-r/2)-i)*((y-r/2)-i);
            int bottom = (x-j)*(x-j)+((y+r/2)-i)*((y+r/2)-i);

            if ( sum <= r*r && sum >= (r-3)*(r-3) )
                *(p + i*WSIZE + j) = 0x00000000;
            if ( top <= (r/2)*(r/2) || (j>=x && sum <= r*r && bottom > (r/2)*(r/2)) )
                *(p + i*WSIZE + j) = 0x00000000;
        }
    drawCircle(tmp, x, y-r/2, r/6, 0x00ffffff);
    drawCircle(tmp, x, y+r/2, r/6, 0x00000000);
}

void drawPoint(int x, int y, int *p, int color)
{
    if ( x<0 || x>=WSIZE || y>=HSIZE || y<0 ) {
        fprintf(stderr, "Over Screen Size...\n");
        exit(EXIT_FAILURE);
    }
    *(p + y * WSIZE + x) = color;
}


struct doublePointer{
    int (*midBuf)[WSIZE];
    int *p;
    int height;
    int width; 
    int x;
    int y;
};
void *drawThread(void *zom)
{
    struct doublePointer *p = (struct doublePointer *)zom;
    for ( int i=(p->height)/2; i<p->height; i++ ) 
    {
        for ( int j=0; j<p->width; j++ )
        {
            *(p->p + (i+p->y)*WSIZE + (j+p->x)) = p->midBuf[i][j];
        }
    }
    free(p);
    return NULL;
}
void drawBmp(int *p, const char *picName, int x0, int y0)
{
    char picNameBuf[256];
    sprintf(picNameBuf, "BMP/%s", picName);
    int fd = open(picNameBuf, O_RDONLY);
    if ( fd == -1 )
        sys_error("open");

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


    //printf("w: %d\n", picWidth);
    //printf("h: %d\n", picHight);
    //printf("b: %d\n", picBitDeep);
    //printf("p: %d\n", pad);

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
    pthread_t tid;
    struct doublePointer *Tp = malloc(sizeof(struct doublePointer));
    Tp->height = picHight;
    Tp->width = picWidth;
    Tp->p = p;
    Tp->midBuf = midBuf;
    Tp->x = x0;
    Tp->y = y0;
    pthread_create(&tid, NULL, drawThread, (void *)Tp);
    for ( int i=0; i<picHight/2; i++ ) 
    {
        for ( int j=0; j<picWidth; j++ )
        {
            *(p + (i+y0)*WSIZE + (j+x0)) = midBuf[i][j];
        }
    }
    pthread_join(tid, NULL);

    int re = close(fd);
    if ( re == -1 )
        sys_error("close");
}
