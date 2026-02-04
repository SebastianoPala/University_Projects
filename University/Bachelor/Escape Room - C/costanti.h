#ifndef COSTANTI
#define COSTANTI

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

#define IP_SERVER "127.0.0.1"
#define SERVER_ID 0
#define SERVER_ROOM -1

#define MAX_SERVER_BUFFER 512
#define MAX_CLIENT_BUFFER 512

#define MAX_MESS_SIZE 512

#define NROOMS 1 //numero totale di stanze esistenti

#define MIN 60
#define TIME_LIMIT 10*MIN

#define MAX_OBJ 25 //numero di "oggetti" massimo per stanza. un "oggetto" puo' essere sia un oggetto, che una locazione
#define INVENTORY_SIZE 4

//percorso file utilizzato per salvare le informazioni degli account
#define PASSWORD_PATH "shadow.txt"

//sezione messaggi
#define DEFAULT_TXT "Benvenuto, usa \"look\" per guardarti attorno"
#define TIMEOUT_TXT "Il tempo a disposizione e' terminato!!"
#define VICTORY_TXT "Hai raccolto tutti i gettoni in tempo!!"
#define COOP_LEAVE_TXT "L'altro giocatore ha abbandonato la partita"

#define NO_LOOK_TXT "Non vedi nulla del genere"

#define NO_PICKUP_TXT "Non puoi raccogliere quest'oggetto"
#define ALR_PICKUP_TXT "L'oggetto e' gia' stato raccolto"

#define TOKEN_TXT "E' stato raccolto un gettone!"
#define NO_OWN_USE "Non puoi usare un oggetto che non possiedi"
#define NO_USE_TXT "Non si usa in questo modo"

#define FULL_OBJ_TXT "Inventario pieno!"

#define GIVE_TXT "L'altro giocatore ti ha dato un oggetto"
#define NO_GIVE_TXT "Nulla da dare"
#define GIVE_ANSW_TXT "Hai dato l'oggetto all'altro giocatore"
#define FULL_GIVE_TXT "L'altro giocatore ha l'inventario pieno"

#define DROP_TXT "Oggetto lasciato"
#define NO_DROP_TXT "Nessun oggetto da lasciare"

#define SAY_TXT "Messaggio inviato correttamente"

#define ALR_ANSW_TXT "L'enigma e' gia stato risolto"
#define WRONG_ANSW_TXT "Risposta errata"
#endif