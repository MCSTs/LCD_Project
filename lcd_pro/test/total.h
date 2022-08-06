#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>
#include <linux/input.h>
#include <sys/mman.h>

#define LCDSIZE 480 * 800
#define HSIZE 480
#define WSIZE 800

void sys_error(const char *mes);

int *lcdInit();
void drawBmp(int *p, const char *picNameBuf, int x0, int y0, bool *isclick);
void mplayerCMD(int fifoFD, bool *isclick);
