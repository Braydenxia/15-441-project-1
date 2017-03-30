/******************************************************************************
* HTTP_server.h                                                               *
*                                                                             *
* Description: This file contains the C Header code for an HTTP server.       *
*              Including the following parts:                                 *
*              1. Warpper function of socket, accept, bind, listen, etc       *
*              2. err_sys function                                            *
*              3. signal handler ,handle the SIGCHLD                          *
*                                                                             *
* Authors: Braydenxia <braydenxia@gmail.com>                                  *
*                                                                             *
*******************************************************************************/


#include        <netinet/in.h>
#include        <netinet/ip.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <sys/socket.h>
#include        <unistd.h>
#include        <sys/time.h>
#include        <sys/types.h>
#include        <unistd.h>
#include        <signal.h>
#include        <errno.h>

#define     HTTP_PORT       8080
#define     BUF_SIZE        4096
#define     CLIENT_LIMIT    2000
#define     MAXLINE         50

void err_sys(const char* x)
{
    perror(x);
    exit(1);
}

int Socket(int family, int type, int protocol){
    int sock;
    if ((sock = socket(family, type, protocol)) == -1)
    {
        err_sys("socket error");
    }
    return sock;
}

void Bind(int sock, struct sockaddr_in *addr, socklen_t addrlen){
    if (bind(sock, (struct sockaddr *)addr, addrlen))
        err_sys("bind error");
    return;
}

void Listen(int fd, int backlog){
    //char *ptr;

    //if( (ptr = getenv("LISTENQ")) != NULL)
    //    backlog = atoi(ptr);

    if(listen(fd, backlog))
        err_sys("listen error");

    return;
}
// Accept function will not exit() and will return 0, if
// erron == EINTR
int Accept(int sockfd, struct sockaddr_in *cliaddr, socklen_t *clilen){
    int connfd;
    if( (connfd = accept(sockfd, (struct sockaddr*) cliaddr, clilen)) == 0){
        if (errno == EINTR)
            return connfd;
        else
            err_sys("accept error");
    }
    return connfd;
}

int Close(int sock)
{
    if (close(sock))
    {
        err_sys("close error");
    }
    return 0;
}

int Fork(){
    return fork();
}

//Self-defined Signal Function to call sigaction to build a signal hanlder
typedef     void    Sigfunc(int);

Sigfunc *Signal (int signo, Sigfunc *func){
    struct sigaction act, oact;

    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if(signo == SIGALRM){
#ifdef  SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif
    } else {
#ifdef  SA_RESTART
                act.sa_flags |= SA_RESTART;
#endif
    }

    if(sigaction(signo, &act, &oact) < 0)
        return (SIG_ERR);
    return(oact.sa_handler);
}
//Self-defined sig_child function (for process to clear zombie )

void sig_chld(int signo){
    pid_t   pid;
    int     stat;

    while( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    return;
}
