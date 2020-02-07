#include <stdio.h>
#include <signal.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdlib.h>
#include "Unit.h"
#include "Socket.h"
#include "ad_shm.h"
#include "Scope.h"

#define MAX 200
#define SA struct sockaddr
int PORT_NUM=3301 ;


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
    //sock->servaddr.sin_addr.s_addr = inet_addr("192.168.56.99");
    //sock->servaddr.sin_addr.s_addr = inet_addr("172.18.1.100");
    sock->servaddr.sin_addr.s_addr = inet_addr("192.168.88.1");
    //sock->servaddr.sin_addr.s_addr = inet_addr("192.168.40.100");

    sock->servaddr.sin_port = htons(sock->port);
    

}
int connect_socket2(socket_connection* sock)
{
    
    Host_Addr.sin_family=AF_INET ;
    Host_Addr.sin_port=htons(PORT_NUM) ;
    Host_Addr.sin_addr=*(struct in_addr*)Host->h_addr ;
    
    printf("Address=%u\n",Host_Addr.sin_addr.s_addr) ;
    memset(Host_Addr.sin_zero,'\0',sizeof Host_Addr.sin_zero) ;
    
    if((sock->sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("socket(): Error\n") ;
        exit(1) ;
    }
    return 0 ;
}


int connect_socket(socket_connection* sock)
{
    int x;
    int r=0;
    
    if (connect(sock->sockfd, (SA*)&sock->servaddr, sizeof(sock->servaddr)) != 0) {
        printf("connection with the server failed...\n");
        r=-1;
    }
    else{
        x=fcntl(sock->sockfd,F_GETFL,0) ;        // Get socket flags
        fcntl(sock->sockfd,F_SETFL,x | O_NONBLOCK) ;    // Add non-blocking flag
        printf("connected to the server..\n");
    }
    return r;
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
    int len=0;
    bzero(buff, MAX);
    len=read(sockfd1, buff, sizeof(buff));

    printf("From server (%d)\n", len);
    
    if(buff[0]!=0x99){
        //printf("    message header not recognized: %x\n",buff[0]);
    }
    if(buff[0]==0x99){
        printf("start new message: %x\n",buff[0]);
        printf("message type: %x\n",buff[1]);
        if(buff[1]==0xAA){
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


void send_dummy(int sockfd){
    
    char buff[MAX];
    bzero(buff, sizeof(buff));
    buff[0]=0x99;
    //strncpy(buff, "dummy\n", sizeof buff - 1);
    //printf("sent: %s", buff);

    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));


}

void send_event(int sockfd){
    
    char buff[MAX];
    bzero(buff, sizeof(buff));
    buff[0]=0;//x99;
    //strncpy(buff, "dummy\n", sizeof buff - 1);
    //printf("sent event: %x   %d\n",fake_event[0],sizeof(fake_event));
    write(sockfd, buff, sizeof(buff));

    //rite(sockfd, fake_event, sizeof(fake_event));
    //bzero(buff, sizeof(buff));
    
    
}

int Write_Data(int sockfd,uint8_t* data, int l)
{
    int write_bytes;
    //printf("sizeof:   %lu  %d\n",sizeof(data), l);
    write_bytes=write(sockfd,data,l) ;
    //printf("write bytes=%d\n",write_bytes) ;
    return(write_bytes) ;
}
