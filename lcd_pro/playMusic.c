#include "list.h"
#include "lcd.h"


static ListHead *head = NULL;
static int re = 0;

void playMusic()
{
    head = initList();
    createFileNameList(head, "Music");
    ListNode *p = head->head;

    char musName[1024];
    while ( true )
    {
        sprintf(musName, "madplay -Q Music/%s", p->fileName);
        re = system(musName);
        p = p->next;
    }
}
