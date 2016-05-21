#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <libserialport.h>
#include <time.h>

void delay(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock()){printf("hello?");}
}

struct serial_port_config{ //Config data
    char port_name[13];
    int baudrate;
    int bits;
    //enum sp_flowcontrol flow;
    enum sp_parity parity;
    int stopbits;       
    int xonxoff;
    int rtscts;
    int dsrdtr;
};

struct serial_port_config sp_config= { //initilizing config data
    .port_name = "/dev/ttyUSB0",
    .baudrate = 115200,
    .bits = 8,
    //.flow = SP_FLOWCONTROL_NONE,
    .parity = SP_PARITY_NONE,
    .stopbits = 1,
    .xonxoff = 0,
    .rtscts = 0,
    .dsrdtr = 0
    
};

typedef struct serial_port_request{
    uint8_t pre1;
    uint8_t pre2;
    uint8_t pre3;
    uint8_t size;
    uint8_t command;
    uint8_t crc;
}serial_port_request;

struct serial_port_protocol{
    char preamble[2];
    char directionSend[1];
    
    uint8_t MSP_IDENT;

};

struct serial_port_protocol sp_protocol = {
    .preamble = "$M",
    .directionSend = "<",
    .MSP_IDENT = 100

};

void sendRequest(struct sp_port * multiwii, uint8_t command_in){
    uint8_t size = 0;
    uint8_t command = command_in;
    uint8_t crc = 0;
    crc ^= size;
    crc ^= command;

    int a = sp_blocking_write(multiwii,(char *)"$",1,0);
    int b = sp_blocking_write(multiwii,(char *)"M",1,0);
    int c = sp_blocking_write(multiwii,(char *)"<",1,0);
    int d = sp_blocking_write(multiwii, &size, 1,0);
    int e = sp_blocking_write(multiwii, &command, 1,0);
    int f = sp_blocking_write(multiwii, &crc, 1,0);
    if(a<0 || b<0 || c<0 ||d<0 ||e<0||f<0){
        printf("writing error");
    }
    
    printf("$M<, size %d, command %d, crc %d\n", size, command, crc);
    sp_drain(multiwii);//wait for data to be sent
}


void getIDENT(struct sp_port * multiwii){
    char h1 = 't';//Buffer for '$' part of preamble
    char h2 = 's';//Buffer for 'M' part of preamble
    char h3 = 't';//Buffer for '>' direction
    uint8_t version = 0;//Version
    uint8_t type = 0;// Type of Multiwii (what type of copter)
    uint8_t msp = 1;//MSP version
    uint32_t cap = 0;//Capability. 32 bit variable to indicate capability of FC board

    printf("input waiting: %d ",sp_input_waiting(multiwii));
    while(h1 != '$'){
        sendRequest(multiwii,100);
        sp_blocking_read(multiwii, &h1, 1,100);//read $
    }


    sp_blocking_read(multiwii, &h2, 1,100);//read M
    sp_blocking_read(multiwii, &h3, 1,100);//read > direction
    sp_blocking_read(multiwii, &version, 1,100);//read uint8 version
    sp_blocking_read(multiwii, &type,1,100);//read uint8 multiwii type
    sp_blocking_read(multiwii, &msp,1,100);//read uint8 msp version
    sp_blocking_read(multiwii, &cap, 4, 100);//read uint32 capability 

    printf("%c",h1);
    printf("%c",h2);
    printf("%c",h3);
    printf("version: %d, type: %d, msp: %d, cap: %d\n",version,type,msp,cap);
}

void getSTATUS(struct sp_port * multiwii){
    char h1 = 't';//Buffer for '$' part of preamble
    char h2 = 's';//Buffer for 'M' part of preamble
    char h3 = 't';//Buffer for '>' direction
    uint16_t cycleTime = 0; //cycleTime in microseconds
    uint16_t i2c_errors_count = 0; //count of i2c errors
    uint16_t sensors = 0;//BARO<<1|MAG<<2|GPS<<3|SONAR<<4
    uint32_t flag = 0; //Bit variable indivating whtihc BOX are active
    uint8_t config = 0; //indicate current global config setting
    printf("input waiting: %d ",sp_input_waiting(multiwii));
    
    while(h1 != '$'){
        sendRequest(multiwii,101);
        sp_blocking_read(multiwii, &h1, 1,100);//read $
    }
    sp_blocking_read(multiwii, &h2, 1,100);//read M
    sp_blocking_read(multiwii, &h3, 1,100);//read > direction
    sp_blocking_read(multiwii, &cycleTime, 2,100);//read uint16 cycleTime 
    sp_blocking_read(multiwii, &i2c_errors_count,2,100);//read uint16 i2c_errors
    sp_blocking_read(multiwii, &sensors,2,100);//read uint16 sensors
    sp_blocking_read(multiwii, &flag,4,100);//read uint32 flags
    sp_blocking_read(multiwii, &config,1,100);//read uint8 config

    printf("%c",h1);
    printf("%c",h2);
    printf("%c",h3);
    printf("cycleTime: %d, i2c_errors: %d, sensor: %d, flag: %d, global_conf: %d\n",cycleTime, i2c_errors_count, sensors, flag, config);

}

void list_ports(){
    struct sp_port ** ports;
    sp_list_ports(&ports);
    int i;
    for(i = 0; ports[i]; i++){
        printf("Availiable port: %s \n",sp_get_port_name(ports[i]));
    }
    sp_free_port_list(ports);
}

int init_configs(struct sp_port_config ** mconfig){

    enum sp_return config_status = sp_new_config(mconfig);
    if(config_status != 0){
        printf("ERROR CREATING sp_new_config. HALTING");
        return 1;
    }
    config_status = sp_set_config_baudrate(*mconfig, sp_config.baudrate);
    if(config_status != 0){
        printf("ERROR SETTING sp_config_set_baudrate. HALTING");
        return 1;
    }
    config_status = sp_set_config_bits(*mconfig, sp_config.bits);
    if(config_status != 0){
        printf("ERROR SETTING sp_config_set_bits. HALTING");
        return 1;
    }
//  config_status = sp_set_config_flowcontrol(*mconfig, sp_config.flow);
//  if(config_status != 0){
//      printf("ERROR SETTING sp_config_set_flowcontrol. HALTING");
//      return 1;
//  }

    config_status = sp_set_config_parity(*mconfig, sp_config.parity);
    if(config_status != 0){
        printf("ERROR SETTING sp_config_set_parity. HALTING");
        return 1;
    }

    config_status = sp_set_config_stopbits(*mconfig, sp_config.stopbits);
    if(config_status != 0){
        printf("ERROR SETTING sp_config_set_stopbits. HALTING");
        return 1;
    }

    config_status = sp_set_config_xon_xoff(*mconfig, sp_config.xonxoff);
    if(config_status != 0){
        printf("ERROR SETTING sp_config_set_xon_xoff. HALTING");
        return 1;
    }
    config_status = sp_set_config_rts(*mconfig, sp_config.rtscts);
    if(config_status != 0){
        printf("ERROR SETTING sp_config_set_rts. HALTING");
        return 1;
    }
    config_status = sp_set_config_cts(*mconfig, sp_config.rtscts);
    if(config_status != 0){
        printf("ERROR SETTING sp_config_set_cts. HALTING");
        return 1;
    }
    config_status = sp_set_config_dtr(*mconfig, sp_config.dsrdtr);
    if(config_status != 0){
        printf("ERROR SETTING sp_config_set_dsr. HALTING");
        return 1;
    }
    config_status = sp_set_config_dsr(*mconfig, sp_config.dsrdtr);
    if(config_status != 0){
        printf("ERROR SETTING sp_config_set_dtr. HALTING");
        return 1;
    }
    return 0;

}

int init_port(){    
    //show availiable ports
    //list_ports();
    
    //set up configs
    struct sp_port_config * mconfig;
    int config_return = init_configs(&mconfig);
    if(config_return != 0){
        return config_return;
    }

    //open multiwii port
    struct sp_port * multiwii;
    enum sp_return port_status = sp_get_port_by_name(sp_config.port_name, &multiwii);   
    if(port_status != 0){
        printf("UNABLE TO ACCESS PORT OR PORT ERROR. HALTING"); 
        return 1;
    }
    
    port_status = sp_open(multiwii, SP_MODE_READ_WRITE);//open
    if(port_status != 0){
        printf("UNABLE TO OPEN PORT. HALTING"); 
        return 1;
    }
    
    //set configuration to port
    port_status = sp_set_config(multiwii, mconfig);
    if(port_status != 0){
        printf("ERROR sp_set_config. HALTING \n");  
        printf("ERROR: %d", port_status);
        return 1;
    }

    getIDENT(multiwii);
    getSTATUS(multiwii);

    sp_close(multiwii);
    sp_free_port(multiwii);
    sp_free_config(mconfig);
    return 0;
}


int main(void){
    return init_port();
}
