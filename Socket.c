#include <stdio.h>
#include <signal.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdlib.h>
#include "Unit.h"
#include "Socket.h"
#include "ad_shm.h"

#define MAX 200
#define SA struct sockaddr



void make_socket(socket_connection* sock){
    // socket create and verification
    
    sock->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock->sockfd == -1) {
        printf("socket creation failsed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&sock->servaddr, sizeof(sock->servaddr));
    
    // assign IP, PORT
    sock->servaddr.sin_family = AF_INET;
    //sock->servaddr.sin_addr.s_addr = inet_addr("192.168.61.100");
    sock->servaddr.sin_addr.s_addr = inet_addr("192.168.56.99");

    sock->servaddr.sin_port = htons(sock->port);
    
}


void connect_socket(socket_connection* sock)
{
    
    if (connect(sock->sockfd, (SA*)&sock->servaddr, sizeof(sock->servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
}


void func_write(int sockfd)
{
    char buff[MAX];
    int n;
    for (;;) {
        bzero(buff, sizeof(buff));
        printf("Enter the string : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n');
        write(sockfd, buff, sizeof(buff));
        
        if ((strncmp(buff, "exit", 4)) == 0) {
            //printf("Client Exit...\n");
            break;
        }
        bzero(buff, sizeof(buff));
    }
}

void func_write_auto(int sockfd)
{
    char buff[MAX];
    int n;
    int i;
    for (i=0; i<1; i++) {
        bzero(buff, sizeof(buff));
        strncpy(buff, "Hi, I'm connected\n", sizeof buff - 1);
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
    }
    strncpy(buff, "exit\n", sizeof buff - 1);
    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));

}

void func_read(int sockfd1)
{
    int i;
    char buff[MAX];
    int n;
    // infinite loop for chat
    for (i=0; i<5; i++) {
        bzero(buff, MAX);
        // read the message from client and copy it in buffer
        read(sockfd1, buff, sizeof(buff));
        // print buffer which contains the client contents
        //printf("From client: %s", buff);
        if ((strncmp(buff, "exit", 4)) == 0) {
            
            //printf("Client Exit...\n");
            break;
        }
        
        bzero(buff, MAX);
        n = 0;
    }
}

int func_read_message(int sockfd1)
{
    //printf("\n\n");
    int exit=0;
    char buff[MAX];
    bzero(buff, MAX);
    read(sockfd1, buff, sizeof(buff));

   // printf("\n\n    From server (%d): %x,  %x,  %x\n", sizeof(buff),buff[0],buff[1],buff[2]);
    
    if(buff[0]!=0x99){
        printf("    message header not recognized: %x\n",buff[0]);
    }
    if(buff[0]==0x99){
        //printf("start new message: %x\n",buff[0]);
        //printf("message type: %x\n",buff[1]);
        if(buff[1]==0xaa){
            exit=1;
            //printf("received exit message\n");
        }
    }
    if(buff[1]==0x20){
        printf("    received parameter list %d\n",buff[2]);
        build_property_ctrlist(buff,sizeof(buff));
    }
    
    if(buff[1]==0x21){
        printf("    received mode parameter list\n");
        build_mode_ctrlist(buff,sizeof(buff));
    }
    if(buff[1]==0x25){
        printf("    received finish message\n");
        end_param=1;
    }
    if(buff[0]==0){
        printf("    problem reading: %x\n",buff[0]);
        error_count=error_count+1;
    }
    
    bzero(buff, MAX);
    return exit;
}


int func_listen(int sockfd1)
{
    //printf("\n\n");
    int exit=0;
    char buff[MAX];
    bzero(buff, MAX);
    read(sockfd1, buff, sizeof(buff));
    printf("size of received buffer: %d\n", sizeof(buff));
    
    printf("From server: %x,  %x,  %x\n", buff[0],buff[1],buff[2]);
    
    if(buff[0]!=0x99){
        printf("message header not recognized: %x\n",buff[0]);
    }
    if(buff[0]==0x99){
        //printf("start new message: %x\n",buff[0]);
        //printf("message type: %x\n",buff[1]);
        if(buff[1]==0xaa){
            exit=1;
            //printf("received exit message\n");
        }
    }
    if(buff[1]==0x20){
        printf("received parameter list\n",buff[0]);
        build_property_ctrlist(buff,sizeof(buff));
    }
    
    if(buff[1]==0x21){
        printf("received mode parameter list\n",buff[0]);
        build_mode_ctrlist(buff,sizeof(buff));
    }
    
    bzero(buff, MAX);
    return exit;
}
