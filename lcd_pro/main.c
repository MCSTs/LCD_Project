#include "list.h"
#include "lcd.h"
#include <signal.h>
#include <sys/wait.h>
#include <linux/input.h>

void sys_error(const char *mes)
{
    perror(mes);
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{

    //if ( argc < 2 ) {
        //fprintf(stderr, "missing arg...\n");
        //fprintf(stderr, "showImg <pathname of BMP Picture>\n");
        //return -1;
    //}

    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);


    int fdFile = open("tmp", O_RDWR | O_CREAT | O_NONBLOCK, 0644);
    if (fdFile == -1)
        sys_error("open tmp");
    int re = write(fdFile, "0", strlen("0"));
    if (re == -1)
        sys_error("write");


     //creat 3 child process
    pid_t parID = getpid();
    pid_t pid[3];
    int i = 0;
    for (i=0; i<3; i++) {
        pid[i] = fork();
        if (pid[i] == -1)
            sys_error("fork");

        if ( pid[i] == 0 )
            break;
    }

    //parent process
    if ( i == 3 ) {

        int *p = lcdInit();

        //lcdClear(p, 0x00ffffff);

        ListHead *head = initList();
        //createFileNameList(head, argv[1]);
        createFileNameList(head, "BMP");
        //ListNode *tmp = head->head;

        //showIMG(fdFile, p, head);
        HomePage(p, "PlayMode.bmp", head, fdFile); 

        printf("show done..\n");

        kill(pid[0], SIGINT);
        kill(pid[1], SIGINT);
        kill(pid[2], SIGKILL);
        system("killall -9 madplay");

        pid_t childPID;
        while ((childPID = wait(NULL)) != -1) {
            printf("wait pid: %d\n", childPID);
        }
        if (errno != ECHILD)
            sys_error("wait");

        printf("wait done...\n");

        unlink("tmp");
        close(fdFile);

        // go back main interface
        //system("/IOT/iot");
    }


    //child process 1 --> click
    if ( i == 0 ) {
        touchEvent(fdFile, parID);
    }


    //child process 2 --> send auto play signal
    if ( i == 1 ) {
        while (true)
        {
            sleep(3);
            kill(parID, SIGUSR1);
        }
    }

    //child process 3 --> play
    if (i == 2) {
        playMusic();
    }

    return 0;
}
