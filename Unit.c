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
//#include "scopeFunctions.h"
#include "Unit.h"
#include "Scope.h"

uint16_t list_id_ch[4]={0x0899,0x0999,0x0A99,0x0B99};
uint16_t list_id_trigg[4]={0x0C99,0x0D99,0x0E99,0x0F99};

void initialize_parameter_lists(){
    
    printf("initializing parameter lists\n");
    int ch;
    
    
    dig_mode_params[0]=0x0199;
    dig_mode_params[1]=0x0012;
    dig_mode_params[8]=0x6666;
    
    readout_window_params[0]=0x0299; // header,list number
    readout_window_params[1]=0x0016; // byte count
    readout_window_params[2]=0x0190; // pre coin time 1
    readout_window_params[3]=0x0640; // post coin time 1
    readout_window_params[4]=0x0190; // pre coin time 2
    readout_window_params[5]=0x0640; // post coin time 2
    readout_window_params[6]=0x0190; // pre coin time 3
    readout_window_params[7]=0x0640; // post coin time 3
    readout_window_params[8]=0x0190; // pre coin time 4
    readout_window_params[9]=0x0640; // post coin time 5
    readout_window_params[10]=0x6666; // end

    
    
    
    for(ch=0; ch<4; ch++){
        ch_property_params[ch][0]=list_id_ch[ch];      // start, list number
        ch_property_params[ch][1]=0x12;      // byte count
        ch_property_params[ch][2]=0x3fa9;      // gain correction
        ch_property_params[ch][3]=0x007f;      // offset correction
        ch_property_params[ch][4]=0x3e80;      // base min
        ch_property_params[ch][5]=0x0000;      // base max
        ch_property_params[ch][6]=0x00;  // PM voltage, filter
        ch_property_params[ch][7]=0x00;  // spare
        ch_property_params[ch][8]=0x6666;  // end
    
        ch_trigger_params[ch][0]=list_id_trigg[ch] ;    // start,list number
        ch_trigger_params[ch][1]=0x0012 ;    // byte count
        ch_trigger_params[ch][2]=0x1f40 ;    // T1
        ch_trigger_params[ch][3]=0x0190 ;    // T2
        ch_trigger_params[ch][4]=0x0a1e ;    // tprev+tper
        ch_trigger_params[ch][5]=0x64ff ;    // tcmac+ncmax
        ch_trigger_params[ch][6]=0x03e8 ;    // ncmin+qmax
        ch_trigger_params[ch][7]=0x0000 ;    // qmin+spare
        ch_trigger_params[ch][8]=0x6666 ;    // end

    }
}



void build_property_ctrlist(char *buff, int32_t size){
    
    //printf("building control list %04x: \n",buff[1]);
    //printf("buffer size %d: \n",size);

    //unsigned short ctrllist1[9]={0x0899,0x0012,0x3fa9,0x007f,0x3e80,0x0000,0x0000,0x0000,0x6666};
    //uint8_t set_param_list[PARAM_NUM_LIST][PARAM_LIST_MAXSIZE];
    
    uint8_t i=buff[1];
    if(i==0x20){
        int ch=buff[2]-1;
 
        //printf("ch: %d\n",ch);

       // ch_HV[ch]=i= buff[3] << 8 | buff[4];
        ch_HV[ch]= buff[3] << 8 | buff[4];
        printf("          --> %d: \n",ch_HV[ch]);
        //trigger_condition[ch]=buff[5];
        ch_property_params[ch][2]= buff[9] << 8 | buff[10];  // gain correction
        ch_property_params[ch][3]= buff[11] << 8 | buff[12];  // offset correction, integration time
        ch_property_params[ch][4]= buff[13] << 8 | buff[14];   // base max
        ch_property_params[ch][5]= buff[15] << 8 | buff[16];  // base min
 
        readout_window_params[2*(ch)+2]=buff[5] << 8 | buff[6];
        readout_window_params[2*(ch)+3]=buff[7] << 8 | buff[8];
        
        printf("pre-coincidence time: %d\n",readout_window_params[2*(ch)+2]);
        
        
        ch_trigger_params[ch][2]= buff[17] << 8 | buff[18];    // signal threshold T1
        printf("T1: %d\n",ch_trigger_params[ch][2]);

        ch_trigger_params[ch][3]= buff[19] << 8 | buff[20];    // noise threshold T2
        ch_trigger_params[ch][4]= buff[21] << 8 | buff[22];    // t_prev (n*5ns) +t_per
        ch_trigger_params[ch][5]= buff[23] << 8 | buff[24];    // tcmax (n*5ns) +ncmax
        ch_trigger_params[ch][6]= buff[25] << 8 | buff[26];    // ncmin + qmax
        ch_trigger_params[ch][7]= 1 << 8 | buff[27];    // qmin
 
    }
    

    
    

}


void build_mode_ctrlist(char *buff, int32_t size){
    
    //printf("building mode param list %04x: \n",buff[1]);
    //printf("buffer size %d: \n",size);
    //trigger_condition=
    dig_mode_params[2]= buff[2] << 8 | buff[3]; // control register
    dig_mode_params[3]= buff[4] << 8 | buff[5]; // trigger enable mask
    dig_mode_params[4]= buff[6] << 8 | buff[7]; // ch mask, trigger rate divider
    dig_mode_params[5]= buff[8] << 8 | buff[9]; // common coincidence readout
    trigger_condition=buff[10]; //how many channels required for trigger
    dig_mode_params[6]=0x00;
    dig_mode_params[7]=0x00;

}
