#include "total.h"
#include "list.h"


void mplayerCMD(int fifoFD, bool *isclick)
{

    ListHead *head = NULL;
    head = initList();
    createFileNameList(head, "video");
    showList(head);
    ListNode *curVideo = head->head;
    curVideo = curVideo->next;
    

    write(fifoFD, "volume 50 1\n", strlen("volume 50 1\n"));
    write(fifoFD, "loadfile video/U2.mp4 1\n", strlen("loadfile video/U2.mp4 1\n"));

    int fd = open("/dev/input/event0", O_RDONLY);
    if ( fd == -1 )
        sys_error("open");

    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    int x, y, startX, endX;
    int startY, endY;

    while ( true ) {
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
        if ( ev.type==EV_KEY && ev.code==BTN_TOUCH && ev.value == 0 && *isclick ) 
        {
            endX = x;
            endY= y;

            // Pause and continue
            if ( startX>=470 && endX<=535 && startY>=535 && startY<=600 ) {
                printf("In pause\n");
                char *cmd = "pause\n";
                re = write(fifoFD, cmd, strlen(cmd));
                //re = write(fifoFD, "pause\n", strlen("pause"));
                if ( re == -1 )
                    sys_error("write");
            }

            // Speed
            else if ( startX>=550 && endX<=614 && startY>=535 && startY<=600 ) {
                printf("In ->>\n");
                char *cmd = "seek +5\n";
                re = write(fifoFD, cmd, strlen(cmd));
                if ( re == -1 )
                sys_error("write");
            }
            // Fast backward
            else if ( startX>=385 && endX<=450 && startY>=535 && startY<=600 ) {
                printf("In <<-\n");
                char *cmd = "seek -5\n";
                re = write(fifoFD, cmd, strlen(cmd));
                if ( re == -1 )
                sys_error("write");
            }

            // Next video
            else if ( startX>=806 && endX<=864 && startY>=535 && startY<=600 ) {
                printf("In >\n");
                char *fileVideo = curVideo->fileName;
                curVideo = curVideo->next;
                char cmd[64];
                sprintf(cmd, "loadfile video/%s\n", fileVideo);
                printf("cmd: %s", cmd);
                re = write(fifoFD, cmd, strlen(cmd));
                if ( re == -1 )
                sys_error("write");
            }
            // Previous video
            else if ( startX>=180 && endX<=235 && startY>=535 && startY<=600 ) {
                printf("In <\n");
                char *fileVideo = curVideo->fileName;
                curVideo = curVideo->pre;
                char cmd[64];
                sprintf(cmd, "loadfile video/%s\n", fileVideo);
                printf("cmd: %s", cmd);
                re = write(fifoFD, cmd, strlen(cmd));
                if ( re == -1 )
                sys_error("write");
            }

            // Volume increase
            else if ( startX>=288 && endX<=340 && startY>=535 && startY<=600 ) {
                printf("In +\n");
                char *cmd = "volume +10\n";
                re = write(fifoFD, cmd, strlen(cmd));
                if ( re == -1 )
                sys_error("write");
            }

            // Volume decrease
            else if ( startX>=690 && endX<=750 && startY>=535 && startY<=600 ) {
                printf("In -\n");
                char *cmd = "volume -10\n";
                re = write(fifoFD, cmd, strlen(cmd));
                if ( re == -1 )
                sys_error("write");
            }

            // Brightness --
            else if (endY > startY && abs(endY - startY)>=10) {
                printf("Brightness -5\n");
                char *cmd = "Brightness -5\n";
                re = write(fifoFD, cmd, strlen(cmd));
                if ( re == -1 )
                sys_error("write");
            }

            // Brightness ++
            else if (endY < startY && abs(endY - startY)>=10) {
                printf("Brightness +5\n");
                char *cmd = "Brightness +5\n";
                re = write(fifoFD, cmd, strlen(cmd));
                if ( re == -1 )
                sys_error("write");
            }
            else if ( startX>=965 && endX<=1024 && startY>=535 && startY<=600 ) {
                printf("Quit...\n");
                break;
            }
            startX = startY = endX = endY = x = y = 0;
            //usleep(100);
        }
    }

    close(fd);
    close(fifoFD);
    destroyList(head);
}
