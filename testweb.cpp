#include <iostream>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;
using namespace std;



struct quad_info_struct{
    uint8_t con_count = 0;
    bool is_armed = false;

} quad_info;

/**
* Open function. This will call once a connection is successfully
* opened. It is used to make sure that there is only one connection
* to the pi.
* 
* Currently this function's only role is to increment the connection count
* con_count
*/
void on_open(websocketpp::connection_hdl hdl){
    quad_info.con_count++;
}

/**
* Close function. This will be called once a connection closes.
* This will hold the responsibility of disarming
*
*/
void on_close(websocketpp::connection_hdl hdl){
    if(quad_info.is_armed){
        //kill the copter
    } 
    
}

/**
* Message function. This will be called everytime there is a message
* sent. It also parses the data
*
*/
void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg) {
        string data = msg->get_payload();
        cout << data << endl;
        
        if(data.length = 16){//the appropriate size of rc data
            try{
                //recieving in order roll, pitch, yaw, throttle
                int roll = stoi(data.substr(0,4));
                int pitch = stoi(data.substr(4,4));
                int yaw = stoi(data.substr(8,4));
                int throttle = stoi(data.substr(12,4));

                //now to send the data to the multiwii
            }
            catch(std::invalid_argument e){
                cout << e.what() << endl;
                cout << "odd data --" << data << "--" << endl;
            }
        }
        else{
            cout << "odd data --" << data << "--" << endl;
        }
}


int main() {
    server com_server;

    com_server.set_open_handler( &on_open );
    com_server.set_close_hander( &on_close );
    com_server.set_message_handler( &on_message );

    com_server.init_asio();
    com_server.listen(80);
    com_server.start_accept();

    com_server.run();
}
/*
    so, there are a few main functions they are all from the on_message
    
        rcData
        
    actually, that is all we need to do. just send rcData
    
    build in a saftey, such that you can only connect and send data once. 
    
    how should i package the data?
    well, I need to send 4 4 digit numbers
    so maybe: xxxxxxxxxxxxxxxx
    like:     1000150020001200



*/
