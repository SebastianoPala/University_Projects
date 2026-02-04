#include "costanti.h"
#include "strutture.h"

//funzioni utilizzate sia dal client che dal server

void separatore(){
    printf("----------------------------------------------------------------------\n");
}

/// @brief inverte i dati contenuti all'interno delle strutture dati s_header
void hton_struct(struct s_header*struttura){
    struttura->len=htons(struttura->len);
    struttura->id=htons(struttura->id);
    struttura->time_remaining=htons(struttura->time_remaining);
    struttura->info=htonl(struttura->info);
    //gli altri campi sono da un byte;
}

//elimina il padding dall'header prima di inviarlo scrrivendo i campi all'interno del buffer
//in sequenza
void serializza_header(char*buffer,struct s_header*header){
    int lenght=0;

    // porto la struct in formato big-endian
    hton_struct(header);

    memcpy(&buffer[lenght],&header->len,sizeof(header->len));
    lenght+=sizeof(header->len);
    memcpy(&buffer[lenght],&header->id,sizeof(header->id));
    lenght+=sizeof(header->id);
    memcpy(&buffer[lenght],&header->time_remaining,sizeof(header->time_remaining));
    lenght+=sizeof(header->time_remaining);
    memcpy(&buffer[lenght],&header->token_collected,sizeof(header->token_collected));
    lenght+=sizeof(header->token_collected);
    memcpy(&buffer[lenght],&header->token_required,sizeof(header->token_required));
    lenght+=sizeof(header->token_required);
    memcpy(&buffer[lenght],&header->type,sizeof(header->type));
    lenght+=sizeof(header->type);
    memcpy(&buffer[lenght],&header->room,sizeof(header->room));
    lenght+=sizeof(header->room);
    memcpy(&buffer[lenght],&header->info,sizeof(header->info));
}
//estrae da buffer i campi dell'header
void deserializza_header(char*buffer,struct s_header*header){
    int lenght=0;
    
    memcpy(&header->len,&buffer[lenght],sizeof(header->len));
    lenght+=sizeof(header->len);
    memcpy(&header->id,&buffer[lenght],sizeof(header->id));
    lenght+=sizeof(header->id);
    memcpy(&header->time_remaining,&buffer[lenght],sizeof(header->time_remaining));
    lenght+=sizeof(header->time_remaining);
    memcpy(&header->token_collected,&buffer[lenght],sizeof(header->token_collected));
    lenght+=sizeof(header->token_collected);
    memcpy(&header->token_required,&buffer[lenght],sizeof(header->token_required));
    lenght+=sizeof(header->token_required);
    memcpy(&header->type,&buffer[lenght],sizeof(header->type));
    lenght+=sizeof(header->type);
    memcpy(&header->room,&buffer[lenght],sizeof(header->room));
    lenght+=sizeof(header->room);
    memcpy(&header->info,&buffer[lenght],sizeof(header->info));

    // porto la struct in formato little-endian
    hton_struct(header);
}

// inizializza l'header del messaggio con le informazioni specificate, poi invia header e messaggio nel socket specificato
/// @return restituisce 0 se termina correttamente, -1 se ci sono errori
int invia_messaggio(struct identity*usr_info,int socket_s ,int8_t type,struct game_room*game,void*buffer,int buf_size){
    struct s_header header; //creo l'header per il messaggio che devo inviare
    time_t clock; //gestisce il tempo nel caso si voglia inviare informazioni relative alla partita
    char aux_char[HEADER_SIZE]; //vettore che conterra' l'header dopo essere stato convertito
    int status=0; //gestisce ritorno funzioni

    //inizializzo l'header del messaggio
    header.len=buf_size;
    header.type=type;
    header.id=usr_info->id;
    header.room=usr_info->room;
    header.info=usr_info->info;

    if(game){ //se giocatore in partita, include info
        clock=time(NULL);
        header.token_collected=game->session.collected_tokens;
        header.token_required=game->session.required_tokens;
        header.time_remaining=TIME_LIMIT - (clock - game->start_time);
    }
    
    //converto la struct prima di inviarla
    serializza_header(aux_char,&header);

    status=send(socket_s,aux_char,HEADER_SIZE,0);//Invio solo l'header, di dimensione fissa
    if(status<HEADER_SIZE){
        return -1;
    }
    if(buf_size && buffer){//se e'presente un messaggio, lo invio
        status=send(socket_s,buffer,buf_size,0);
        if(status<buf_size)
            return -1;
    }
    return 0;
}

/// @brief riceve il messaggio e l header separatamente
/// @return restituisce 0 se termina correttamente, -1 se ci sono errori
int ricevi_messaggio(int socket_s,struct s_header *header,void*buffer){
    char aux_char[HEADER_SIZE];
    int status=0;
    status=recv(socket_s,aux_char,HEADER_SIZE,0);//ricevo prima il messaggio contenente l'header serializzato, di dimensione fissa
    if(status<HEADER_SIZE)
        return -1;

    deserializza_header(aux_char,header); //estraggo l'header dal messaggio ricevuto
    if(header->len){
        status=recv(socket_s,buffer,header->len,0);//se l'header contiene una lunghezza diversa da 0, allora ricevo anche il messaggio
        if(status<header->len)
            return -1;
    }
    return 0;
}
