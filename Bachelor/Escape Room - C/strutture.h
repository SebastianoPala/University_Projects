#ifndef STRUTTURE
#define STRUTTURE
#include "costanti.h"
//enumerato usato per gestire i vari tipi di messaggi
enum TipoMessaggio{
    //connessione e autenticazione
    CLIENT_HELLO, //richiesta assegnazione id
    SERVER_HELLO, //assegnazione id
    CLIENT_LOGIN,
    CLIENT_SIGNUP,
    CLIENT_LOGOUT,
    TERMINATE,
    LOGIN_ACK,  //esito positivo
    SIGNUP_ACK,
    BAD_LOGIN,  //esito negativo
    BAD_SIGNUP,

    //stanze
    ROOM_INFO,
    ROOM_INFO_ANSW,
    JOIN_ROOM,
    COOP_ROOM,
    GAME_READY,

    //partita
    GAME_MESSAGE,
    RIDDLE,
    ROOM_LEAVE,
    COOP_LEAVE,
    VICTORY,
    DEFEAT,

    //comandi ingame
    LOOK,
    TAKE,
    USE,
    OBJS,
    DROP,
    //coop only
    SAY,
    GIVE
};

//enumerato usato per tenere traccia dello stato del client
enum ClientTracker{
    OFFLINE,
    CONNECTED,
    ONLINE,
    WAITING,
    INGAME,
    SOLVING
};

//enumerato usato per gestire lo stato degli oggetti
enum ItemState{
    SPECIAL_LOCKED,
    TOKEN,
    LOCKED,
    FREE,
    TAKEN,
    UNAVAILABLE
};

//utilizzata da ogni utente per memorizzare le informazioni di identificazione
struct identity{
    int16_t id;
    int8_t room;
    int32_t info; //il campo info puo' contenere valori diversi a seconda della situazione
};

//struct usata per facilitare scrittura/lettura header messaggi secondo il
//protocollo adottato
struct s_header{
    int16_t len; //lunghezza del messaggio (header escluso)
    int16_t id; //id mittente
    int16_t time_remaining;//informazioni sulla partita in corso
    int8_t token_collected;
    int8_t token_required;
    int8_t type; //tipo del messaggio
    int8_t room; //stanza mittente
    int32_t info; //info aggiuntive
};

#define HEADER_SIZE sizeof(struct s_header)

//struct usata per collegare un utente al suo username e al socket corrispondente
struct u_online{
    int16_t id; //id utente
    int8_t room; //stanza in cui si trova
    int u_socket; //socket associato
    char*name; //username (null se l'utente non ha ancora effettuato il login)
    struct game_room*game_room; //puntatore alla stanza in cui si trova l'utente
    struct u_online*next;
};

// struct usata per gestire le informazioni di un tipo di stanza
// generica: puo essere usata per qualsiasi stanza
// ogni istanza di stanza dello stesso tipo puntera' alla game_info corrispondente (per risparmiare memoria)
struct game_info{
    char*room_name;
    int8_t oggetti[MAX_OBJ+1]; //stato iniziale degli oggetti, comprende anche lo stato delle locazioni (entrambi gestiti allo stesso modo)
    int8_t first_location; //indice del primo "oggetto" che non puo essere raccolto (locazioni o altro)
    int8_t tokens; //il numero di token necessari da raccogliere per vincere
    char*nome_oggetti[MAX_OBJ];
    char*desc_oggetti[MAX_OBJ]; //descrizione oggetti dopo che sono stati sbloccati
    char*desc_lock_oggetti[MAX_OBJ]; //descrizione oggetti da bloccati
    char*enigmi[MAX_OBJ];
    char*soluzioni[MAX_OBJ];
    char*mess_soluzioni[MAX_OBJ];//messaggi di soluzione
};

//struct che tiene traccia dell andamento di un istanza di stanza, quindi di una partita
// ogni puntatore puntera' al campo corrispondente nella game_info dello stesso tipo
struct game_session{
    char*room_name;
    int8_t oggetti[MAX_OBJ]; //copia privata dello stato degli oggetti
    int8_t first_location;
    int8_t required_tokens;
    int8_t collected_tokens;
    char**nome_oggetti;
    char**desc_oggetti;
    char**desc_lock_oggetti;
    char**enigmi;
    char**soluzioni;
    char**mess_soluzioni;
};

//contiene le informazioni dei giocatori che si trovano nella stanza
struct game_room{
    int type; //il numero della stanza

    struct u_online* player1;
    char* p1_items[INVENTORY_SIZE]; //inventario
    
    struct u_online* player2;
    char *p2_items[INVENTORY_SIZE]; //inventario
    
    struct game_session session;

    time_t start_time; // tempo di avvio di una partita

    struct game_room*next;
};
#endif