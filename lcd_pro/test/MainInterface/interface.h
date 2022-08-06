#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/input.h>

#define LCDSIZE 480 * 800
#define HSIZE 480
#define WSIZE 800

void sys_error(const char *mes);
void mplayerCMD();

int *lcdInit();
void InterfaceSelect( int *p, const char *picNameBuf, int x0, int y0 );
void drawBmp(int *p, const char *picNameBuf, int x0, int y0);
