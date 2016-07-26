websocket : msp.o configure_port.o websocket.o
	g++ $? -lboost_system -lboost_chrono -lpthread -Isetup/websocketpp -lserialport -o $@
websocket.o : websocket.cpp
	g++ -O1 -c -Wall $? -lboost_system -lboost_chrono -lpthread -Isetup/websocketpp -lserialport -o $@

msp.o : setup/cMultiWii/msp.c
	gcc -O1 -c -lserialport $? -o $@

configure_port.o : setup/cMultiWii/configure_port.c 
	gcc -O1 -c -lserialport $? -o $@ 
clean : 
	rm -f msp.o configure_port.o websocket.o websocket
