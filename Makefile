all: encryption.c 
	gcc -Wall -g -I/usr/local/include/openssl -L/usr/lib -lssl -lcrypto encryption.c -o binary
clean: 
	rm -f encryption