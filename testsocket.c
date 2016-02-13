#include <stdio.h>
#include <libserialport.h>



void list_ports(){
	struct sp_port ** ports;
	sp_list_ports(&ports);
	int i;
	for(i = 0; ports[i]; i++){
		printf("Availiable port: %s \n",sp_get_port_name(ports[i]));
	}
	sp_free_port_list(ports);
}

void init_configs(struct sp_port_config ** mconfig_in){
	struct serial_port_init_data{
		char * port_name = "/dev/tty1";
		int baudrate = 115200;
		int bits = 8;
		enum sp_flowcontrol flow = SP_FLOWCONTROL_NONE;
		enum sp_parity parity = SP_PARITY_NONE;
		int stopbits = 1;		

	} sp_init_data;

	struct sp_port_config * mcconfig = *mconfig_in;

	enum sp_return config_status = sp_new_config(&mconfig);
	if(config_status != 0){
		printf("ERROR CREATING sp_new_config. HALTING");
		return 1;
	}
	config_status = sp_set_config_baudrate(mconfig, sp_init_data.baudrate);
	if(config_status != 0){
		printf("ERROR SETTING sp_config_set_baudrate. HALTING");
		return 1;
	}
	config_status = sp_set_config_bits(mconfig, sp_init_data.bits);
	if(config_status != 0){
		printf("ERROR SETTING sp_config_set_bits. HALTING");
		return 1;
	}

	config_status = sp_set_config_flowcontrol(mconfig, sp_init_data.flow);
	if(config_status != 0){
		printf("ERROR SETTING sp_config_set_flowcontrol. HALTING");
		return 1;
	}

	config_status = sp_set_config_parity(mconfig, sp_init_data.parity);
	if(config_status != 0){
		printf("ERROR SETTING sp_config_set_parity. HALTING");
		return 1;
	}

	config_status = sp_set_config_stopbits(mconfig, sp_init_data.stopbits);
	if(config_status != 0){
		printf("ERROR SETTING sp_config_set_stopbits. HALTING");
		return 1;
	}

}

int init_port(){	
	
	
	//show availiable ports
	list_ports();
	
	struct sp_port_config * mconfig;

	//open multiwii port
	struct sp_port * multiwii;
	enum sp_return port_status = sp_get_port_by_name(port_name, &multiwii);	

	if(SP_OK != port_status || port_status!=0){
		printf("UNABLE TO ACCESS PORT OR PORT ERROR. HALTING");	
		return 1;
	}
    
	sp_free_port(multiwii);
	return 0;
}


int main(void){
	return init_port();

}

