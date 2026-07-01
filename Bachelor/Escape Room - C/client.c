#include "funzioni_client.c"

int main(int argc,char*argv[]){

    struct identity clt_info;  //contiene le informazioni che identificano il client

    in_port_t c_port; //porta usata dal client
    in_port_t s_port; //porta in cui si trova il server

    int comms_socket; //socket per la comunicazione col server

    char username[20]; //contiene il nome utente dell'account su cui si e' fatto l'accesso

    char old_message[MAX_MESS_SIZE];// utilizzato per vedere i messaggi vecchi durante la partita
    char buffer[MAX_CLIENT_BUFFER]; // buffer utilizzato dal client per i trasferimenti
    struct s_header header; //struct per ricevere e gestire l'header dei messaggi

    //tengo traccia dello stato del client
    enum ClientTracker client_status;
    enum TipoMessaggio room_type; //memorizza il tipo di stanza (single player o coop)

    struct sockaddr_in server_address,client_address;

     // variabili di appoggio per gestione comandi
    enum TipoMessaggio command_type;
    int command_size; // dimensione del comando, per capire che sezione del buffer inviare

    int max_room_number; //numero di stanze nel server

    int i;
    int status; //usata per gestire il ritorno delle funzioni

    fd_set entrambi; // il client userà solo stdin e il socket di comunicazione
    fd_set pronti;
    int both[2]; //per velocizzare la ricerca del file descriptor pronto
    int max_fd;

    //azzero le informazioni del client in quanto l'id verra' ricevuto dal server
    clt_info.id=0;
    clt_info.room=0;
    clt_info.info=0;
    client_status=OFFLINE;
    room_type=0;

    command_type=0;
    command_size=0;
    
    c_port=0;
    s_port=4242;

    max_room_number=0;

    FD_ZERO(&entrambi);
    FD_ZERO(&pronti);

    //inserisco stdin all'interno di "entrambi"
    FD_SET(STDIN_FILENO,&entrambi);
    max_fd=STDIN_FILENO;
    both[0]=STDIN_FILENO; //sara' uno dei 2 fd che controllo

    if(argc==2){ //estraggo la porta da argv (se presente)
        c_port=atoi(argv[1]);
        printf("PORTA SELEZIONATA: %i\n",c_port);
    }

    //mi connetto staticamente alla porta 4242
    /*
    print_connect_screen(s_port);
    //permetto all'utente di scegliere su quale porta tentare la connessione
    //in caso il server non si trovi su quella di default
    while(1){
        fgets(buffer,15,stdin);
        if(!strcmp(buffer,"stop\n")){
            exit(0);
        }
        if(!strncmp(buffer,"connect ",8) && strlen(buffer)>8){ 
            s_port=atoi(&buffer[8]);
            break;
        }
        printf("COMANDO NON RICONOSCIUTO\n");
    }
    */

    //inizializzo le strutture dati necessarie per la connessione
    memset(&server_address,0,sizeof(server_address));
    memset(&client_address,0,sizeof(client_address));

    server_address.sin_family=AF_INET;
    client_address.sin_family=AF_INET;

    server_address.sin_port=htons(s_port);
    client_address.sin_port=htons(c_port);

    inet_pton(AF_INET,IP_SERVER,&server_address.sin_addr); //indirizzo del server
    client_address.sin_addr.s_addr=INADDR_ANY;
    
    printf("Connessione col Server in corso... (PORTA: %i)\n",s_port);
    //creo il socket di comunicazione
    comms_socket=socket(AF_INET,SOCK_STREAM,0);
    FD_SET(comms_socket,&entrambi);
    both[1]=comms_socket;
    if(comms_socket>max_fd)
        max_fd=comms_socket;

    //ignoro la porta ricevuta
    /*
    if(c_port){ 
        while(bind(comms_socket,(struct sockaddr*)&client_address,sizeof(client_address))==-1){
            perror("ERRORE INIZIALIZZAZIONE SOCKET CLIENT: ");
            printf("Provo con la porta %i...\n",++c_port); //se la porta non va bene ne provo un altra
            client_address.sin_port=htons(c_port);
        }
    }
    */

    //il processo tenta la connessione ogni 2 secondi finche il server non risponde
    while(connect(comms_socket,(struct sockaddr*)&server_address,sizeof(server_address))==-1){
        sleep(2); 
    }
    printf("Connessione completata!\n");
    client_status=CONNECTED;
    printf("Creazione sessione in corso...\n");

    //invio al server la richiesta di assegnazione id, poi attendo la risposta
    if(invia_messaggio(&clt_info,comms_socket,CLIENT_HELLO,NULL,NULL,0)!=0
        || ricevi_messaggio(comms_socket,&header,buffer)!=0){
            printf("Creazione sessione fallita\n");
            exit(-1);
    }
    memcpy(&clt_info.id,buffer,2);
    clt_info.id=ntohs(clt_info.id);
    
    intro_screen(clt_info.id); //schermata di login per il client
    
    while(1){ //ciclo di attivita' del client
        pronti=entrambi;
        select(max_fd+1,&pronti,NULL,NULL,NULL);
        for(i=0;i<2;i++){
            if(FD_ISSET(both[i],&pronti)){ // controllo solamente i 2 fd che mi interessano
                if(both[i]==STDIN_FILENO){
                    fgets(buffer,MAX_MESS_SIZE,stdin); 
                    //COMANDI ACCETTATI IN QUALSIASI STATO SI TROVI IL CLIENT
                    if(!strcmp(buffer,"\n")) //ignoro ritorno carrello
                        continue;
                    if(!strcmp(buffer,"quit\n") || !strcmp(buffer,"end\n")){
                        printf("Arresto in corso...\n");
                        close(comms_socket);
                        exit(0);
                    }
                    //a seconda dello stato del client utilizzo set di comandi differenti
                    switch(client_status){
                        case CONNECTED: // comandi di accesso
                            if(!strcmp(buffer,"login\n") || !strcmp(buffer,"signup\n")){
                                login_attempt(&clt_info,comms_socket,buffer[0],buffer);
                                strcpy(username,buffer); //buffer contiene ancora l'username
                                client_status=WAITING;  //non posso fare altro se non aspettare la risposta del server    
                            }else
                                printf("COMANDO NON RICONOSCIUTO\n");
                        break;

                        case ONLINE: // comandi menu principale
                            room_type=0;
                            if(!strncmp(buffer,"start ",6) && strlen(buffer)>6)
                                room_type=JOIN_ROOM;
                            else if(!strncmp(buffer,"coop ",5) && strlen(buffer)>5)
                                room_type=COOP_ROOM;

                            if(room_type!=0){
                                // guardo la prima lettera del comando precedente per ottenere la posizione del numero della stanza
                                clt_info.room=atoi(&buffer[ (buffer[0]=='s')?6:5 ]);
                                if(!clt_info.room || clt_info.room>max_room_number){
                                    printf("NUMERO STANZA %i NON VALIDO!\n",clt_info.room);
                                    continue;
                                }
                                client_status=WAITING; // attendo che la sessione di gioco sia pronta
                                if(room_type==JOIN_ROOM)
                                    printf("Attendo la risposta del server...\n");
                                else if(room_type==COOP_ROOM)
                                    printf("In attesa di un altro giocatore...\n");
                                status=invia_messaggio(&clt_info,comms_socket,room_type,NULL,NULL,0);
                                check_client_error(status);
                                continue;
                            }else if(!(strcmp(buffer,"logout\n"))){
                                room_type=0;
                                //avviso il server che esco dall'account
                                status=invia_messaggio(&clt_info,comms_socket,CLIENT_LOGOUT,NULL,NULL,0);
                                check_client_error(status);
                                client_status=CONNECTED;
                                intro_screen(clt_info.id);
                                continue;
                            }else{
                                printf("COMANDO NON RICONOSCIUTO\n");
                            }
                        break;

                        case INGAME: // comandi in partita
                            buffer[strlen(buffer)-1]='\0'; //rimuovo ritorno carrello
                            if(!strcmp(buffer,"leave")){ //permetto all'utente di tornare al menu principale (avvisando il server)
                                room_type=0;
                                printf("HAI ABBANDONATO LA PARTITA!\nRitorno al menu in corso...\n");
                                status=invia_messaggio(&clt_info,comms_socket,ROOM_LEAVE,NULL,NULL,0);
                                client_status=WAITING;
                                status=invia_messaggio(&clt_info,comms_socket,ROOM_INFO,NULL,NULL,0); //richiedo nuovamente le informazioni al server
                                check_client_error(status);
                                continue;
                            }
                            if(!strcmp(buffer,"objs")){
                                status=invia_messaggio(&clt_info,comms_socket,OBJS,NULL,NULL,0);
                                check_client_error(status);
                                continue;
                            }
                            if(room_type==COOP_ROOM){ //comandi disponibili solamente in una sessione a 2 giocatori
                                if(!strncmp(buffer,"give ",5) && strlen(buffer)>5){
                                    command_size=0;
                                    clt_info.info=atoi(&buffer[5]) - 1;//lo invio gia decrementato visto che verra usato come indice di un vettore
                                    if(clt_info.info<0 || clt_info.info>INVENTORY_SIZE-1){
                                        printf("LO SLOT DEVE ESSERE COMPRESO NELL'INTERVALLO [1-%i]\n",INVENTORY_SIZE);
                                        continue;
                                    }
                                    buffer[0]='\0';//evito di inviare piu del necessario
                                    command_type=GIVE;
                                }
                                else if(!strncmp(buffer,"say ",4) && strlen(buffer)>4){
                                    command_type=SAY;  
                                    command_size=4;                                
                                }
                            }
                            if(!strncmp(buffer,"look",4)){
                                if(buffer[4]==' ' && strlen(buffer) >5){ //look con argomento
                                    command_type=LOOK;
                                    command_size=5;
                                }else if(buffer[4]=='\0' || buffer[4]==' '){ //look senza argomenti
                                    command_type=LOOK;
                                    command_size=0;
                                    buffer[0]='\0';
                                }
                            }
                            else if(!strncmp(buffer,"take ",5) && strlen(buffer)>5){
                                command_type=TAKE;
                                command_size=5;
                            }
                            else if(!strncmp(buffer,"use ",4) && strlen(buffer)>4){
                                command_type=USE;
                                command_size=4;   
                            }else if(!strncmp(buffer,"drop ",5) && strlen(buffer)>5){
                                command_size=0;
                                clt_info.info=atoi(&buffer[5]) - 1;//lo invio gia decrementato visto che verra usato come indice di un vettore
                                if(clt_info.info<0 || clt_info.info>INVENTORY_SIZE-1){
                                    printf("SLOT DEVE ESSERE COMPRESO NELL'INTERVALLO [1-%i]\n",INVENTORY_SIZE);
                                    continue;
                                }
                                buffer[command_size+1]='\0';//evito di inviare piu del necessario
                                command_type=DROP;
                            }
                            if(command_type!=0){ //se un comando e' stato scelto
                                status=invia_messaggio(&clt_info,comms_socket,command_type,NULL,&buffer[command_size],strlen(&buffer[command_size])+1);
                                check_client_error(status);
                                command_type=0;
                            }else{
                                printf("COMANDO NON RICONOSCIUTO\n");
                            }
                        break;

                        case SOLVING: //il client ha ricevuto un enigma: qualsiasi cosa digitata verra' inviata come risposta al server
                            //il numero dell'enigma si trova su clt_info.info
                            buffer[strlen(buffer)-1]='\0';
                            status=invia_messaggio(&clt_info,comms_socket,RIDDLE,NULL,buffer,strlen(buffer)+1);
                            check_client_error(status);
                            client_status=INGAME;
                        break;

                        default:
                            //non faccio niente (WAITING)
                        break;
                    }
                }else{ //e' il socket di comunicazione
                    status=ricevi_messaggio(both[i],&header,buffer);
                    check_client_error(status);

                    switch(header.type){
                        case TERMINATE: //la connessione è stata interrotta volontariamente
                            printf("SEI STATO DISCONNESSO DAL SERVER\n");
                            close(comms_socket);
                            exit(0);
                        break;

                        case LOGIN_ACK:
                        case SIGNUP_ACK:
                            if(header.type==LOGIN_ACK)
                                printf("LOGIN");
                            else
                                printf("SIGNUP");
                            printf(" ESEGUITO CORRETTAMENTE\n");
                            printf("BENVENUTO %s!\n",username);
                            status=invia_messaggio(&clt_info,comms_socket,ROOM_INFO,NULL,NULL,0); //richiedo info delle stanze al server
                            check_client_error(status);
                            //client_status=WAITING; //mi trovo ancora in questo stato 
                            //devo aspettare la risposta dal server
                        break;

                        case BAD_LOGIN:
                        case BAD_SIGNUP:
                            if(header.type==BAD_LOGIN)
                                printf("LOGIN");
                            else
                                printf("SIGNUP");
                            printf(" FALLITO: RIPROVA\n");
                            intro_screen(clt_info.id);
                            client_status=CONNECTED; //permetto all'utente di riutilizzare i comandi login e signup
                        break;

                        case ROOM_INFO_ANSW:
                            client_status=ONLINE; //posso ricominciare a prendere input da tastiera
                            separatore();
                            printf("\tESCAPE ROOM: EDIZIONE NOVEMBRE 2024\tACCOUNT: %s\n\n",username);
                            if(!header.info){//header.info contiene il numero di stanze in questo caso
                                printf("\tNessuna stanza disponibile al momento...\n");
                                printf("\tRiprova tra qualche giorno\n");
                                printf("\tArresto in corso...\n");
                                close(comms_socket);
                                exit(0);
                            }
                            else{
                                max_room_number=header.info;
                                print_rooms(max_room_number,buffer);
                                print_main_menu();
                            }
                            separatore();
                        break;

                        case GAME_READY:
                            client_status=INGAME;
                            system("clear");
                            printf("PARTITA AVVIATA: BUON DIVERTIMENTO\n\n");
                            old_message[0]='\0'; //svuoto la storia dei messaggi

                        case COOP_LEAVE: //se si riceve COOP_LEAVE si passa a modalita' giocatore singolo
                            if(header.type==COOP_LEAVE) //if necessario per evitare di passarci dopo aver ricevuto GAME_READY
                                room_type=JOIN_ROOM;
                            
                        case GAME_MESSAGE:
                            if(client_status!=SOLVING){ //se sto risolvendo un enigma non visualizzo subito il messaggio
                                print_user_interface(username,&header);
                                printf("\n> %s\n\n",old_message);
                                printf("\n> %s\n\n",buffer);
                                print_ingame_cmd(room_type);
                            }
                            strcpy(old_message,buffer);//salvo il messaggio ricevuto
                        break;

                        case RIDDLE:
                            if(client_status!=SOLVING){ //ignoro eventuali richieste multiple
                                client_status=SOLVING;
                                clt_info.info=header.info; //salvo il numero di enigma contenuto in header.info su clt_info 
                                print_user_interface(username,&header);
                                printf("\nHai ricevuto un enigma: \n\n");
                                printf("\n> %s\n\n",buffer);
                                printf("Soluzione:\n");
                                strcpy(old_message,buffer);
                            }
                        break;

                        case VICTORY:
                        case DEFEAT:
                            system("clear");
                            room_type=0;
                            separatore();
                            if(header.type==VICTORY)
                                printf("HAI VINTO!!");
                            else
                                printf("HAI PERSO!!");
                            printf(" %s\n",buffer); //stampo il messaggio che spiega il motivo della vittoria/sconfitta
                            separatore();
                            printf("Premi invio per continuare...\n");
                            fgets(buffer,10,stdin);
                            client_status=WAITING; //levo il controllo all'utente
                            printf("Ritorno al Menu Principale...\n");
                            status=invia_messaggio(&clt_info,comms_socket,ROOM_INFO,NULL,NULL,0); //richiedo info stanze al server
                            check_client_error(status);
                        break;

                        default:
                        break;
                    }
                }
            }
        }
    }
}