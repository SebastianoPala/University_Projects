// funzioni di appoggio per client.c
#include "utility.c"

//FUNZIONI DI STAMPA
/*
void print_connect_screen(in_port_t s_port){
    separatore();
    printf("\t\t\tCLIENT\n");
    printf("Digita un comando:\t\t\tDEFAULT SERVER PORT: %i\n\n",s_port);
    printf("1) connect %i -- Connessione al server sulla porta <port>\n",s_port);
    printf("2) stop  -- Arresta il client\n");
    separatore();
}
*/
void intro_screen(int16_t id){
    separatore();
    printf("\t\tBENVENUTO!!\n");
    printf("Effettua l'accesso per entrare nell'Escape Room\n");
    printf("Id sessione corrente: %i\n\n",id);
    printf("Digita un comando:\n");
    printf("1) login -- Accedi ad un account esistente\n");
    printf("2) signup -- Crea un nuovo account\n");
    separatore();
}
void print_rooms(int8_t room_number,char*buffer){
    int i;
    int byte_letti=0; // variabile usata per scorrere il vettore
    //i nomi delle stanze arrivano concatenati ma separati da '\0'
    for(i=1;i<=room_number;i++){
        printf("%i) - %s\n",i,&buffer[byte_letti]);
        byte_letti+=strlen(&buffer[byte_letti])+1;
    }
    printf("\n");
}
void print_main_menu(){
    printf("Digita un comando:\n");
    printf("1) start <room> -- Avvia una sessione di gioco nella stanza <room>\n");
    printf("2) coop <room> -- Avvia una sessione a 2 giocatori nella stanza <room>\n");
    printf("3) logout -- Esci dall'account\n");
    printf("4) quit -- Arresta l'applicazione\n");
}
void print_user_interface(char*username,struct s_header*header){
    system("clear");
    separatore();
    printf("User: %s",username);
    printf("\t\tToken:%i/%i",header->token_collected,header->token_required);
    printf("\t\tTempo Rimanente: %i min - %i sec\n",header->time_remaining/60,header->time_remaining%60);
}
void print_ingame_cmd(enum TipoMessaggio role){
    separatore();
    printf("Comandi:\n");
    printf("> look <target> -- fornisce una breve descrizione di <target>\n");
    printf("> take <target> -- raccoglie <target>\n");
    printf("> use  <target1> [ <target2> ] -- usa <target1> da solo o su <target2>\n");
    printf("> objs  -- mostra l'inventario\n");
    if(role==COOP_ROOM){ //se sessione a 2 giocatori, stampo anche i comandi COOP
        printf("> give <slot> -- dona l'oggetto nello slot [1-%i] all'altro giocatore\n",INVENTORY_SIZE);
        printf("> say <mess> -- invia il messaggio <mess> all'altro giocatore\n");
    }
    printf("> drop <slot> -- lascia l'oggetto nello slot [1-%i] dell'inventario\n",INVENTORY_SIZE);
    printf("> leave -- torna al menu principale\n");
    printf("> end  -- termina la sessione e chiude la connessione\n");
    separatore();
}

/// @brief  invia i dati per signup/login nel socket di comunicazione
/// @param clt_info identificatore mittente, in questo caso tutti 0
/// @param comms_socket socket su cui inviare
/// @param access_mode login/signup, dipende dalla prima lettera del comando utilizzato
/// @param buffer vettore di appoggio
void login_attempt(struct identity*clt_info,int comms_socket,char access_mode,char*buffer){
    int16_t user_name_lenght;
    int16_t password_lenght;
    int status;//usata per gestire ritorno funzioni
    int8_t type=(access_mode=='l')?CLIENT_LOGIN:CLIENT_SIGNUP; //a seconda della prima lettera del comando utilizzato, scelgo che operazione fare

    
    separatore();
    printf("Inserire dati:\n");
    do{ 
        printf("Nome Utente (MIN: 4 | MAX: 20):\n");
        scanf("%s",buffer);
        user_name_lenght=strlen(buffer);
        if(user_name_lenght<4)
            printf("Nome Utente troppo corto!\n");
        if(user_name_lenght>20)
            printf("Nome Utente troppo lungo!\n");
    }while(user_name_lenght<4 || user_name_lenght>20);
                                
    do{
        printf("Password (MIN: 4 | MAX: 20):\n");
        scanf("%s",&buffer[user_name_lenght+1]); //scrivo la password dopo il fine stringa nome utente
        password_lenght=strlen(&buffer[user_name_lenght+1]);
        if(password_lenght<4)
            printf("Password troppo corta!\n");
        if(password_lenght>20)
            printf("Password troppo lunga!\n");
    }while(password_lenght<4 || password_lenght>20);

    separatore();
    printf("Tentativo di ");
    if(access_mode=='l')
        printf("login");
    else
        printf("signup");
    printf(" in corso...\n");
    status=invia_messaggio(clt_info,comms_socket,type,NULL,buffer,user_name_lenght+1+password_lenght+1);//mando nome utente e password nello stesso messaggio separati da '\0'
    if(status!=0){
        perror("LOGIN FALLITO: ");
        exit(-1);
    }           
}

void check_client_error(int status){
    if(status!=0){ //connessione interrotta involontariamente
        perror("COMUNICAZIONE COL SERVER INTERROTTA: ");
        exit(-1);
    }                                
}

