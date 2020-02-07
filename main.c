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
#include <time.h>
#include "ad_shm.h"
#include "Scope.h"
#include "Socket.h"

#define    PORT1    3317
#define    PORT2    3318
int dtime=2;

shm_struct shm_ev; //!< shared memory containing all event info, including read/write pointers
shm_struct shm_gps; //!< shared memory containing all GPS info, including read/write pointers

//EV_DATA *eventbuf1;  //!< buffer that holds all triggered events (points to shared memory)
GPS_DATA *gpsbuf; //!< buffer to hold GPS information
EV_DATA *gpsbuf2; //!< buffer to hold GPS information



socket_connection sock_listen;
socket_connection sock_send;


//int sock=0, valread;






int main(int argc,char **argv){
    
    ///*  remove for testing
    
    if(ad_shm_create(&shm_ev,BUFSIZE,sizeof(EV_DATA)/sizeof(uint16_t)) <0){ //ad_shm_create is in shorts!
        printf("Cannot create EVENT shared memory !!\n");
        exit(-1);
    }
    printf("Created EVENT shared memory \n");
    
    

    if(ad_shm_create(&shm_gps,GPSSIZE,sizeof(GPS_DATA)/sizeof(uint16_t)) <0){ //ad_shm_create is in shorts!
        printf("Cannot create GPS shared memory !!\n");
        exit(-1);
    }
    printf("Created GPS shared memory \n");
    
    
    *(shm_gps.next_read) = 0;
    *(shm_gps.next_write) = 0;
    gpsbuf2 = (EV_DATA *) shm_ev.Ubuf;

    gpsbuf = (GPS_DATA *) shm_gps.Ubuf;
    //gpsbuf2 = (EV_DATA *) shm_ev.Ubuf;
    
     
  
    
    
    time_t time1,time2 ;
    time1=time(NULL) ;
    printf("start time: %d\n",time1);
    int exit=0;
    int r=0;
    
    initialize_parameter_lists();
    printf("checking initialization: %04x\n",ch_property_params[0][0]);
    sock_send.port=PORT1;
    sock_listen.port=PORT2;
    
    
    sleep(5);
    printf("____________________________________________\n");
    printf("making socket to send\n");
    make_socket(&sock_send);
    r=connect_socket(&sock_send);
    
    sleep(5);
    printf("____________________________________________\n");
    printf("making socket to listen\n");
    make_socket(&sock_listen);
    r=connect_socket(&sock_listen);
    printf("____________________________________________\n");

    
    if(r>=0){
        end_param=0;
        error_count=0;
        int t=0;

        //get control parameters
       while(end_param<1){
         r=func_read_message(sock_listen.sockfd);
         if(end_param==1){
            printf("received message to finish control messages\n");
        }
        
        
       }
        
    
    int i;
    
    printf("mode parameters: {");
    for(i=0;i<LEN_MODE_PARAM; i++){
        printf("%x  ",dig_mode_params[i]);
    }
    printf("}\n");

    printf("readout window: {");
    for(i=0;i<LEN_READOUT_PARAM; i++){
        printf("%x  ",readout_window_params[i]);
    }
    printf("}\n");
    
    printf("ch1 property params: {");
    for(i=0;i<LEN_CH_PROPERTY_PARAM; i++){
        printf("%x  ",ch_property_params[0][i]);
    }
    printf("}\n");
    
    printf("ch2 property params: {");
    for(i=0;i<LEN_CH_PROPERTY_PARAM; i++){
        printf("%x  ",ch_property_params[1][i]);
    }
    printf("}\n");
    
    printf("ch3 property params: {");
    for(i=0;i<LEN_CH_PROPERTY_PARAM; i++){
        printf("%x  ",ch_property_params[2][i]);
    }
    printf("}\n");
    
    printf("ch4 property params: {");
    for(i=0;i<LEN_CH_PROPERTY_PARAM; i++){
        printf("%x  ",ch_property_params[3][i]);
    }
    printf("}\n");
    
    printf("ch1 trigger params: {");
    for(i=0;i<LEN_CH_TRIGGER_PARAM; i++){
        printf("%x  ",ch_trigger_params[0][i]);
    }
    printf("}\n");
    printf("ch2 trigger params: {");
    for(i=0;i<LEN_CH_TRIGGER_PARAM; i++){
        printf("%x  ",ch_trigger_params[1][i]);
    }
    printf("}\n");
    printf("ch3 trigger params: {");
    for(i=0;i<LEN_CH_TRIGGER_PARAM; i++){
        printf("%x  ",ch_trigger_params[2][i]);
    }
    printf("}\n");
    printf("ch4 trigger params: {");
    for(i=0;i<LEN_CH_TRIGGER_PARAM; i++){
        printf("%x  ",ch_trigger_params[3][i]);
    }
    printf("}\n");
    
    event_len=8*(readout_window_params[6]+dig_mode_params[5]+readout_window_params[7])+72;
    printf("total event length = 72+ 8x(%d + %d + %d) = %d\n", readout_window_params[6],dig_mode_params[5],readout_window_params[7],event_len);
    printf("starting scope related things......\n");
    
    
    
    ls_get_station_id();

    scope_main();

    }

    //sleep(10);
    close(sock_send.sockfd);
    close(sock_listen.sockfd);
     
    ad_shm_delete(&shm_gps);
    ad_shm_delete(&shm_ev);
    printf("clean exit... yay!\n");
    

    return 0;
}
