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
#include "Scope.h"

uint16_t list_id_ch[4]={0x099,0x0999,0x0A99,0x0B99};
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
    
    printf("building control list %04x: \n",buff[1]);
    printf("buffer size %d: \n",size);

    //unsigned short ctrllist1[9]={0x0899,0x0012,0x3fa9,0x007f,0x3e80,0x0000,0x0000,0x0000,0x6666};
    //uint8_t set_param_list[PARAM_NUM_LIST][PARAM_LIST_MAXSIZE];
    
    uint8_t i=buff[1];
    if(i==0x20){
        int ch=buff[2];
        printf("ch: %d\n",ch);
        
       // ch_HV[ch]=i= buff[3] << 8 | buff[4];
        ch_HV[ch]= buff[3] << 8 | buff[4];
        printf("          --> %d: \n",ch_HV[ch]);
        
        ch_property_params[ch][2]= buff[10] << 8 | buff[11];  // gain correction
        ch_property_params[ch][3]= buff[12] << 8 | buff[13];  // offset correction, integration time
        ch_property_params[ch][4]= buff[14] << 8 | buff[15];   // base max
        ch_property_params[ch][5]= buff[16] << 8 | buff[17];  // base min
 
        readout_window_params[4*(ch+1)+0]=buff[6] << 8 | buff[7];
        readout_window_params[4*(ch+1)+1]=buff[8] << 8 | buff[9];
        
        
        ch_trigger_params[ch][2]= buff[18] << 8 | buff[19];    // signal threshold T1
        ch_trigger_params[ch][3]= buff[20] << 8 | buff[21];    // noise threshold T2
        ch_trigger_params[ch][4]= buff[22] << 8 | buff[23];    // t_prev (n*5ns)
        ch_trigger_params[ch][5]= buff[24] << 8 | buff[25];    // tcmax (n*5ns)
        ch_trigger_params[ch][6]= buff[26] << 8 | buff[27];    // ncmin
        ch_trigger_params[ch][7]= buff[28] << 8 | 0x00;    // qmin
 
    }
    

    /*
    printf("%04x: header\n",buff[0]);
    printf("%04x: type (params)\n",buff[1]);
    printf("%04x: channel\n",buff[2]);
    printf("%04x: none\n",buff[3]);
    
    printf("%04x: threshold 0\n",buff[4]);
    printf("%04x: threshold 1\n",buff[5]);
    int i= buff[4] << 8 | buff[5];
    printf("          --> %08x: \n",i);
    printf("          --> %d: \n",i);


    printf("%04x: HV 0\n",buff[6]);
    printf("%04x: HV 1\n",buff[7]);
    i= buff[6] << 8 | buff[7];
    printf("          --> %08x: \n",i);
    printf("          --> %d: \n",i);
    
    printf("%04x: trigger 0\n",buff[8]);
    printf("%04x: trigger 1\n",buff[9]);
    i= buff[8] << 8 | buff[9];
    printf("          --> %08x: \n",i);
    printf("          --> %d: \n",i);
    
    printf("%04x: pre-coin time 0\n",buff[10]);
    printf("%04x: pre-coin time 1\n",buff[11]);
    i= buff[10] << 8 | buff[11];
    printf("          --> %08x: \n",i);
    printf("          --> %d: \n",i);
    
    printf("%04x: coin time 0\n",buff[12]);
    printf("%04x: coin time 1\n",buff[13]);
    i= buff[12] << 8 | buff[13];
    printf("          --> %08x: \n",i);
    printf("          --> %d: \n",i);
    
    printf("%04x: post-coin time 0\n",buff[14]);
    printf("%04x: post-coin time 1\n",buff[15]);
    i= buff[14] << 8 | buff[15];
    printf("          --> %08x: \n",i);
    printf("          --> %d: \n",i);
    */
    

}


void build_mode_ctrlist(char *buff, int32_t size){
    
    printf("building mode param list %04x: \n",buff[1]);
    printf("buffer size %d: \n",size);
    
    dig_mode_params[2]= buff[3] << 8 | buff[4]; // control register
    dig_mode_params[3]= buff[5] << 8 | buff[6]; // trigger enable mask
    dig_mode_params[4]= buff[7] << 8 | buff[8]; // ch mask, trigger rate divider
    dig_mode_params[5]= buff[9] << 8 | buff[10]; // common coincidence readout
    dig_mode_params[6]=0x00;
    dig_mode_params[7]=0x00;

}
