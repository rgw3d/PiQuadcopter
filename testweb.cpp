extern "C"{
#include "../cMultiWii/msp.h"
}
#include <iostream>
#include <string>
#include <unistd.h>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;
using namespace std;

struct sp_port * multiwii;
struct sp_port_config * mconfig;
uint16_t * rc_data;

uint16_t con_count = 0;
uint16_t roll =0;
uint16_t pitch =0;
uint16_t yaw = 0;
uint16_t throttle = 0;
uint16_t aux1 = 0;
uint16_t aux2 = 0;
uint16_t aux3 = 0;
uint16_t aux4 = 0;

/**
* Open function. This will call once a connection is successfully
* opened. It is used to make sure that there is only one connection
* to the pi.
* 
* Currently this function's only role is to increment the connection count
* con_count
*/
void on_open(websocketpp::connection_hdl hdl){
   con_count++;
    cout << con_count<< endl;

}

/**
* Close function. This will be called once a connection closes.
* This will hold the responsibility of disarming
*
*/
void on_close(websocketpp::connection_hdl hdl){	
    cout<<"On Close"<<endl;
    uint16_t * tmp_data;
    tmp_data = new uint16_t[8];
    tmp_data[0] = rc_data[0];//set roll to 1000
    tmp_data[1] = rc_data[1];//
    tmp_data[2] = 1000;
    tmp_data[3] = 1000;//set throttle to 1000
    tmp_data[4] = rc_data[4];
    tmp_data[5] = rc_data[5];
    tmp_data[6] = rc_data[6];
    tmp_data[7] = rc_data[7];
    rc_data = tmp_data;
    int i =0;
    while(i<500){
        send_RAW_RC(multiwii, rc_data);
        rc_data = tmp_data;
        i++;
    }
    delete[] tmp_data;
    
}

/**
* Message function. This will be called everytime there is a message
* sent. It also parses the data
*
*/
void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg) {
        string data = msg->get_payload();
        cout << data << endl;
        
        if(data.length() == 39){//the appropriate size of rc data
            try{
                //recieving in order roll, pitch, yaw, throttle
                rc_data[0]=roll=(uint16_t) atoi(data.substr(0,4).c_str());
                rc_data[1]=pitch=(uint16_t) atoi(data.substr(5,4).c_str());
                rc_data[2]=yaw =(uint16_t) atoi(data.substr(10,4).c_str());
                rc_data[3]=throttle =(uint16_t) atoi(data.substr(15,4).c_str());
                rc_data[4]=aux1 =(uint16_t) atoi(data.substr(20,4).c_str());
                rc_data[5]=aux2 =(uint16_t) atoi(data.substr(25,4).c_str());
                rc_data[6]=aux3 =(uint16_t) atoi(data.substr(30,4).c_str());
                rc_data[7]=aux4 =(uint16_t) atoi(data.substr(35,4).c_str());
                
                cout << "Roll: " << roll << " Pitch: "<<pitch <<" Yaw: "<<yaw<<" Throttle: "<<throttle<< " Aux1: "<<aux1 << " Aux2: "<<aux2<<" Aux3: " <<aux3<<" Aux4 "<<aux4<<endl;
                send_RAW_RC(multiwii, rc_data);

                //now to send the data to the multiwii
            }
            catch(invalid_argument e){
                cout << e.what() << endl;
                cout << "odd data --" << data << "--" << endl;
            }
        }
        else{
            cout << "odd data --" << data << "--" << endl;
        }
}


int main() {
    int config_return = init_configs(&mconfig);
    if(config_return>0){
        printf("Error during configuration");
        printf("Error code: %d", config_return);
        printf("Exiting now");
        return config_return;
    }

    int init_return = init_port(&multiwii, mconfig);
    if(init_return>0){
        printf("Error during port configuration");
        printf("Error code: %d", init_return);
        printf("Exiting now");
        return init_return;
    }
    rc_data = new uint16_t[8];
    cout<<"Sleeping now 5 seconds to wait for MultiWii"<<endl;
    sleep(5);
    cout<<"Running server now"<<endl;

    server com_server;
    con_count = 0;
    com_server.set_open_handler( &on_open );
    com_server.set_close_handler( &on_close );
    com_server.set_message_handler( &on_message );

    com_server.init_asio();
    com_server.listen(80);
    com_server.start_accept();

    com_server.run();
    delete[] rc_data;
}
