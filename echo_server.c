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
*******************************************************************************/
#include "echo_server.h"

int main(int argc, char* argv[])
{
    int sock, active_sock, max_fds, new_sock, select_result;
    ssize_t readret;
    socklen_t cli_size;
    struct sockaddr_in addr, cli_addr;
    char buf[BUF_SIZE];
    fd_set read_fds, master;
    struct timeval tv;
    struct DeList *clients_head, *clients_iter, *clients_end;

    //Inital values
    FD_ZERO(&read_fds);
    tv.tv_sec = 2;
    tv.tv_usec = 500000;

    fprintf(stdout, "----- Echo Server -----\n");

    /* all networked programs must create a socket */
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Failed creating socket.\n");
        return EXIT_FAILURE;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(ECHO_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* servers bind sockets to ports---notify the OS they accept connections */
    //bind & listen

    int yes=1;
    //char yes='1'; // Solaris people use this

    // lose the pesky "Address already in use" error message
    if (setsockopt(sock,SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
        perror("setsockopt");
        exit(1);
    }

    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)))
    {
        close_socket(sock);
        fprintf(stderr, "Failed binding socket.\n");
        return EXIT_FAILURE;
    }
    if (listen(sock, 5))
    {
        close_socket(sock);
        fprintf(stderr, "Error listening on socket.\n");
        return EXIT_FAILURE;
    }
    //initialize clients_list, read_fds
    clients_head = malloc(sizeof(struct DeList));
    clients_head -> sock = sock;
    clients_head -> next = NULL;
    clients_end = clients_head;
    FD_SET(sock, &master);
    int max_clients = 0, i;
    /* finally, loop waiting for input and then write it back */
    while (1)
    {
        read_fds = master;
        //printf("begin(accept)\n");
        max_fds = clients_end -> sock;
        cli_size = sizeof(cli_addr);
        //select will hold until some message recieves
        if((select_result =
            select(max_fds + 1, &read_fds, NULL, NULL, NULL)) < 0){
            if(select_result == -1){
                close(sock);
                fprintf(stderr, "Error Selecting connection.\n");
                DeListCollect(clients_head);
                return EXIT_FAILURE;
            }
        }
        //printf( "breakpoint: end of select\n");
        // If message comes from sock: new connections
        if(FD_ISSET(sock, &read_fds)){
            printf( "new client is connecting\n");
            if ((new_sock = accept(sock, (struct sockaddr *) &cli_addr,
                                        &cli_size)) == -1)
            {
                close(sock);
                fprintf(stderr, "Error accepting connection.\n");
                DeListCollect(clients_head);
                return EXIT_FAILURE;
            }
            clients_end = DeListAppend(clients_end, new_sock);
            FD_SET(new_sock, &master);
            DeListDisplay(clients_head);
            max_clients ++;
            CircleTestDeList(clients_head, clients_end);
        }
        // If message not comes from master sock: new message
        else{
            //printf("client is sending message\n");
            active_sock = -1;
            i = 0;
            for(clients_iter = clients_head; clients_iter;
                clients_iter = clients_iter -> next){
                i ++;
                if(i++ < 10)
                    printf("testing socket = %d\n", clients_iter -> sock);
                if(FD_ISSET(clients_iter->sock, &read_fds)){
                    active_sock = clients_iter -> sock;
                    readret = 0;
                    if((readret = recv(active_sock, buf, BUF_SIZE, 0)) >= 1)
                    {
                        if (send(active_sock, buf, readret, 0) != readret)
                        {
                            close_socket(active_sock);
                            close_socket(sock);
                            fprintf(stderr, "Error sending to client.\n");
                            DeListCollect(clients_head);
                            return EXIT_FAILURE;
                        }
                        memset(buf, 0, BUF_SIZE);
                    }
                    if(readret == 0){
                        //the client is closing;
                        FD_CLR(active_sock, &master);
                        max_fds = clients_end -> sock;
                        max_clients --;
                        DeleteFromDeList(&clients_head, &clients_end, clients_iter);
                        printf("client with socket = %d disconnects\n", active_sock);
                        DeListDisplay(clients_head);
                        printf("max_fds = %d\n", max_fds);
                        CircleTestDeList(clients_head, clients_end);
                    }
                    if (readret == -1)
                    {
                        close_socket(active_sock);
                        close_socket(sock);
                        fprintf(stderr, "Error reading from client socket.\n");
                        DeListCollect(clients_head);
                        return EXIT_FAILURE;
                    }

                }
            }
       }
   }
   close_socket(sock);
   DeListCollect(clients_head);
   return EXIT_SUCCESS;
}
