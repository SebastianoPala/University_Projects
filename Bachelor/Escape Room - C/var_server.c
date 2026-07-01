#ifndef GLOBAL_VAR
#define GLOBAL_VAR
#include "costanti.h"
#include "strutture.h"
//VARIABILI GLOBALI USATE IN SERVER.C E FUNZIONI_SERVER.C

struct identity svr_info; //contiene le informazioni che identificano il server

struct u_online*user_list=NULL; //lista contenente gli utenti online e i socket associati

struct u_online*coop_waiting[NROOMS]; //vettore di puntatori: in ogni puntatore si ferma un giocatore in attesa per fare una partita a 2 giocatori

struct game_room*room_list[NROOMS]; //vettore di liste di sessioni di gioco

int total_players; //numero giocatori in partita

fd_set tutti; //contiene stdin, socket di ascolto e i socket di comunicazione

struct game_info room_info[NROOMS]; //vettore che contiene le informazioni utilizzate in ogni stanza
#endif