#include <stdio.h>
#include <libserialport.h>
#define LIBSERIALPORT_DEBUG = 1

struct serial_port_init_data{ //struct for config data
	char port_name[13];
	int baudrate;
	int bits;
	enum sp_flowcontrol flow;
	enum sp_parity parity;
	int stopbits;		
	
};
struct serial_port_init_data sp_init_data = { //initilizing config data
		.port_name = "/dev/ttyUSB0",
		.baudrate = 115200,
		.bits = 8,
		.flow = SP_FLOWCONTROL_NONE,
		.parity = SP_PARITY_NONE,
		.stopbits = 1		
	};

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
	config_status = sp_set_config_baudrate(*mconfig, sp_init_data.baudrate);
	if(config_status != 0){
		printf("ERROR SETTING sp_config_set_baudrate. HALTING");
		return 1;
	}
	config_status = sp_set_config_bits(*mconfig, sp_init_data.bits);
	if(config_status != 0){
		printf("ERROR SETTING sp_config_set_bits. HALTING");
		return 1;
	}

	config_status = sp_set_config_flowcontrol(*mconfig, sp_init_data.flow);
	if(config_status != 0){
		printf("ERROR SETTING sp_config_set_flowcontrol. HALTING");
		return 1;
	}

	config_status = sp_set_config_parity(*mconfig, sp_init_data.parity);
	if(config_status != 0){
		printf("ERROR SETTING sp_config_set_parity. HALTING");
		return 1;
	}

	config_status = sp_set_config_stopbits(*mconfig, sp_init_data.stopbits);
	if(config_status != 0){
		printf("ERROR SETTING sp_config_set_stopbits. HALTING");
		return 1;
	}
	return 0;

}

int init_port(){	
	//show availiable ports
	list_ports();
	
	//set up configs
	struct sp_port_config * mconfig;
	int config_return = init_configs(&mconfig);
	if(config_return != 0){
		return config_return;
	}

	//open multiwii port
	struct sp_port * multiwii;
	enum sp_return port_status = sp_get_port_by_name(sp_init_data.port_name, &multiwii);	
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

	sp_close(multiwii);
	sp_free_port(multiwii);
	sp_free_config(mconfig);
	return 0;
}


int main(void){
	return init_port();
}

