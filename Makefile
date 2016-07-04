websocket : msp.o configure_port.o websocket.o
	g++ $? -lboost_system -lboost_chrono -lpthread -I../websocketpp -lserialport -o $@
websocket.o : testweb.cpp
	g++ -c -Wall $? -lboost_system -lboost_chrono -lpthread -I../websocketpp -lserialport -o $@

msp.o : ../cMultiWii/msp.c
	gcc -c -lserialport $? -o $@
configure_port.o : ../cMultiWii/configure_port.c 
	gcc -c -lserialport $? -o $@ 
clean : 
	rm -f msp.o websocket.o websocketO.o websocket websocketO
