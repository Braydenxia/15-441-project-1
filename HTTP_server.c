/******************************************************************************
* echo_server.c                                                               *
*                                                                             *
* Description: This file contains the C source code for an echo server.  The  *
*              server runs on a hard-coded port and simply write back anything*
*              sent to it by connected clients.  It does not support          *
*              concurrent clients.                                            *
*                                                                             *
* Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                         *
*          Wolf Richter <wolf@cs.cmu.edu>                                     *
*                                                                             *
* Update: Brayden Xia <braydenxia@gmail.com>                                  *
*         Updated to support select from multiple socket @Mar 11, 2017        *
*                                                                             *
*******************************************************************************/

#include "HTTP_server.h"
void str_echo(int sockfd){
    ssize_t readret;
    char buf[BUF_SIZE];
    for( ; ; ){
        if((readret = recv(sockfd, buf, BUF_SIZE, 0)) >= 1)
        {
            //fprintf(server_log, "recieving message from client with socket = %d\n", active_sock);
            if (send(sockfd, buf, readret, 0) != readret)
            {
                Close(sockfd);
            }
            //fprintf(server_log, "sending message back to client with socket = %d\n", active_sock);
            memset(buf, 0, BUF_SIZE);
        }
        if( readret == 0)
            return;
    }
}
int main(int argc, char* argv[])
{

    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in servaddr, cliaddr;

    //create a listen file descriptor
    listenfd = Socket(PF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(HTTP_PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    //set socket options, modify latter
    int yes=1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
        perror("setsockopt");
        exit(1);
    }

    //bind the sock with file descriptor

    Bind(listenfd, &servaddr, sizeof(servaddr));

    //listen
    Listen(listenfd, 5);

    //install signal handler
    Signal(SIGCHLD, sig_chld);

    for( ; ; ){

        clilen = sizeof(cliaddr);
        //if Accept is break by
        if( (connfd = Accept(listenfd, (struct sockaddr_in*) &cliaddr, &clilen)) == 0 )
            continue;

        if( (childpid = Fork()) == 0){
            Close(listenfd);
            str_echo(connfd);
            exit(0);
        }

        Close(connfd);
    }
}
