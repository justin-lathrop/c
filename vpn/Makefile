INC=/usr/local/ssl/include/
LIB=/usr/local/ssl/lib/

all:
	gcc -I$(INC) -L$(LIB) minivpn.c -o minivpn -lssl -lcrypto -ldl

clean:
	rm -rf *~ minivpn
