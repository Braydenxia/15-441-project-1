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

#define     ECHO_PORT       8080
#define     BUF_SIZE        4096
#define     CLIENT_LIMIT    2000

typedef struct DeList{                  //always return the largest list
    int sock;
    struct DeList* next;
    struct DeList* prev;
}DeList;

//always return the newly appended item's address
struct DeList* DeListAppend(struct DeList *original, int new_sock){
    struct DeList* new_item = (struct DeList*) malloc(sizeof(struct DeList));
    new_item -> sock = new_sock;
    new_item -> next = original -> next;
    new_item -> prev = original;
    original -> next = new_item;
    return new_item;
}
void DeListDisplay(struct DeList *head){
    printf("The Client list is:");
    while(head != NULL){
        printf("\t%d", head->sock);
        head = head -> next;
    }
    printf("\n");
    return;
}
void CircleTestDeList(struct DeList *head, struct DeList *end){
    if(end -> next != NULL){
        printf("ERROR: The DeList shows circles!\n");
        printf("The Delists is shown as below: \n");
        DeListDisplay(head);
    }
    return;
}

void DeleteFromDeList(struct DeList **head, struct DeList **end, struct DeList *item){
    if(item == *head){
        *head = item -> next;
    }
    if(item == *end){
        *end = item -> prev;
    }
    if(item -> prev)
        item -> prev -> next = item -> next;
    if(item -> next)
        item -> next -> prev = item -> prev;
    free(item);
    return;
}
void DeListCollect(struct DeList *head){
    struct DeList *next = head;
    while(head != NULL){
        next = head -> next;
        free(head);
        head = next;
    }
    return;
}

int close_socket(int sock)
{
    if (close(sock))
    {
        fprintf(stderr, "Failed closing socket.\n");
        return 1;
    }
    return 0;
}
