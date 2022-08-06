#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>



typedef struct list {
    char fileName[256];
    struct list *next;
    struct list *pre;

} ListNode;


typedef struct head {
    int ListLen;
    ListNode *head;
    ListNode *tail;

} ListHead;


ListHead *initList();
void destroyList(ListHead *head);
void createFileNameList(ListHead *head, const char *dirname);
void showList(ListHead *head);


