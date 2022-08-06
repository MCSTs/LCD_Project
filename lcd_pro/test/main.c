#include "total.h"


void sys_error(const char *mes)
{
  perror(mes);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
  bool isclick = true;
  // create and open named pipe
  if( access("/tmp/demo.fifo", F_OK) )
  {
    if( mkfifo("/tmp/demo.fifo", 0777) == -1 )
      sys_error("mkfifo");
  }
  int fifoFD = open("/tmp/demo.fifo", O_RDWR);
  if ( fifoFD == -1 )
    sys_error("open fifo");

  // create nameless pipe
  //int re;
  //int fdPipe[2];
  //re = pipe(fdPipe);
  //if ( re == -1 )
    //sys_error("pipe");

  int i;
  pid_t pid[2];

  // create 2 child process
  for (i=0; i<2; i++) {
    pid[i] = fork();
    if ( pid[i] == -1 )
      sys_error("fork");
    if ( pid[i] == 0 )
      break;
  }

  // parent proc
  if ( i == 2 ) {
    //close(fdPipe[1]);

    mplayerCMD(fifoFD, &isclick);

    kill(pid[0], SIGINT);
    kill(pid[1], SIGINT);
  }

  // child[0] --> mplayer
  if ( i == 0 ) {
    //close(fdPipe[0]);
    //re = dup2(fdPipe[1], STDOUT_FILENO);
    //if ( re == -1 )
      //sys_error("dup2");
    //sleep(30);
    
    printf("play begin\n");
    execlp("mplayer", "mplayer", "-slave", 
        "-input","file=/tmp/demo.fifo", "-really-quiet", "video/2.mp4", NULL);
    sys_error("execlp");
  }

  // child[1] --> draw control panel
  if ( i == 1 ) {
    int *p = lcdInit();
    drawBmp(p, "0.bmp", 0, 430, &isclick);
  }

  return 0;
}
