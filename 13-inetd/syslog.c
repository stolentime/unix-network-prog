#include <syslog.h>

void log_e(const char *msg)
{
    syslog(LOG_ERR, "%s", msg);
}

void log_i(const char *msg)
{
    syslog(LOG_INFO, "%s", msg);
}

void log_w(const char *msg)
{
    syslog(LOG_WARNING, "%s", msg);
}
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h> 

#define MAXFD   64

//extern int daemon_proc;

int daemon_init(const char *path,int facility)
{
    int i;
    pid_t pid;
    if((pid = fork()) < 0) //create child process, FIRST FORK
      return -1;
    else if(pid)
      _exit(0);//parent terminates   _exit() is different from exit(), it does not call any atexit() routines
      //and do not flush stdio buffers,better for daemon

    umask(0);//set file mode creation mask,so that created files are accessible to all
    if(setsid() < 0) //become session leader
      return -1;
    
    signal(SIGHUP,SIG_IGN);//ignore SIGHUP signal,SIGUP signal usually sent to process when its controlling terminal is closed
    if (( pid = fork()) < 0) //ensure cannot acquire a controlling terminal,TWICE FOR RELIABILITY
      return -1;
    // else if(pid) //parent terminates is not necessary, because daemon will exit after fork(),here parent is FIRST FORK
    //   _exit(0);  //child terminates

    //daemon_proc = 1;
    if(chdir("/") < 0) //change working directory to root,
      return -1;
    //close off file descriptors
    for(i = 0; i < MAXFD; ++i)
      close(i);
    //redirect stdin,stdout and stderr to /dev/null
    open("/dev/null",O_RDWR);
    dup2(0,1);//dup2 is like dup,but it will close the old file descriptor first
    dup2(0,2); //dup2 is used to duplicate a file descriptor to another location in the descriptor table

    openlog("syslog_test", LOG_PID, facility);
    log_e("daemon started");
    closelog();
    return (0);
}
#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc,char** argv)
{
  int listenfd,connfd;
  socklen_t addrlen,len;
  struct sockaddr *clientaddr;
  char buf[1024];
  time_t ticks;

  if (argc < 2|| argc > 3)
  {
    fprintf(stderr,"usage: %s <port> [<facility>]\n",argv[0]);
    exit(1);
  }
  int facility = 0;
  if(argc == 3)
    facility = atoi(argv[2]);

  daemon_init(argv[0],facility);

 
  listenfd = tcp_server(NULL,atoi(argv[1]),0);
  addrlen = sizeof(struct sockaddr); 

  clientaddr = (struct sockaddr*)malloc(addrlen);
  for(;;)
  {
    connfd = accept(listenfd,(struct sockaddr*)clientaddr,&len);
    ticks = time(NULL);
    snprintf(buf,sizeof(buf),"%.24s\r\n",ctime(&ticks));
    write(connfd,buf,strlen(buf));
    close(connfd);
  }
}