#makefile
all: server client other

server: server.c funzioni_server.c var_server.c utility.c costanti.h strutture.h
	gcc -Wall server.c -o server

client: client.c funzioni_client.c utility.c costanti.h strutture.h
	gcc -Wall client.c -o client

other: client.c funzioni_client.c utility.c costanti.h strutture.h
	gcc -Wall client.c -o other

clean:
	rm *.o server client

run: all
	gnome-terminal  --tab --title="Client Terminal" --command="bash -c './client 6000; exec bash'" \
			   --tab --title="Other Terminal" --command="bash -c './other 6100; exec bash'" \
				--tab --title="Server Terminal" --command="bash -c './server 4242; exec bash'"  