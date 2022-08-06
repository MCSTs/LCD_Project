#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/mman.h>
#include "list.h"

#define LCDSIZE 800*480
#define HSIZE 480
#define WSIZE 800


void sys_error(const char *mes);

int *lcdInit();
void lcdClear(int *p, int color);
void drawRectangle(int *p, int x, int y, int height, int width, int color);
void drawCircle(int *p, int x, int y, int r, int color);
void drawAnc(int *p, int x, int y, int r);

void drawBmp(int *p, const char *picName, int x0, int y0);


void showIMG(int fdFile, int *p, ListHead *head);
void touchEvent(int fdFile, pid_t parID);

void playMusic();
void switchTouch(int *p, ListNode **cur);
void HomePage(int *p, const char *picName, ListHead *head, int fdFile);
