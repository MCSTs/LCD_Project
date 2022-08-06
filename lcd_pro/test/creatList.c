#include "list.h"


ListHead *initList()
{
    ListHead *head = (ListHead *)malloc(sizeof(ListHead));
    memset(head, 0, sizeof(ListHead));
    return head;
}

void destroyList(ListHead *head)
{
    if ( head->head == NULL )
        return;
    ListNode *tmp = NULL;
    while ( head->head != head->tail ) {
        tmp = head->head;
        head->head = head->head->next;
        free(tmp);
    }
    free(head->head);
    free(head);
}

static void sys_error(const char *mes)
{
    perror(mes);
    exit(EXIT_FAILURE);
}

void createFileNameList(ListHead *head, const char *dirname)
{
    DIR *df = opendir(dirname);
    if ( df == NULL )
        sys_error("opendir");

    struct dirent *dfStat = NULL;

    while ( (dfStat = readdir(df)) != NULL ) {
        if ( dfStat->d_name[0] == '.' || dfStat->d_name[0] == '0')
            continue;

        ListNode *newNode = (ListNode *)malloc(sizeof(ListNode));
        memset(newNode, 0, sizeof(ListNode));
        strcpy(newNode->fileName, dfStat->d_name);

        if ( head->head == NULL) {
            head->head = newNode;
            head->tail = newNode;
        }
        else {
            newNode->pre = head->tail;
            head->tail->next = newNode;
            head->tail = newNode;
        }

        head->ListLen++;
        head->head->pre = head->tail;
        head->tail->next = head->head;


    }

    if (dfStat == NULL && errno != 0 ) {
        sys_error("readdir");
    }
}


void showList(ListHead *head)
{
    ListNode *p = head->head;
    while ( p != head->tail ) {
        printf("%s\n", p->fileName);
        p = p->next;
    }
    printf("%s\n", p->fileName);
}
