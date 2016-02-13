sock : testsocket.o
	gcc $? -lserialport -o $@

testsocket.o : testsocket.c 
	gcc -g -c -lserialport -Wall $? -o $@ 
clean :
	rm -f testsocket.o sock

