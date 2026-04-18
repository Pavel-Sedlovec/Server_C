all:
	gcc -g src/main.c src/db.c src/server.c src/net.c src/signals.c src/http.c src/cJSON.c -I./include -I/usr/include/postgresql -o serv -pthread -lpq
clean:
	rm -f serv