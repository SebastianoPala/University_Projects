#include "funzioni_server.c"

int main(int argc,char*argv[]){


    int listen_socket; //socket di ascolto
    int comms_socket; //socket di comunicazione

    int i;
    int max_fd; //file descriptor di valore piu alto
    int sk; //variabile utilizzata per iterare fra i fd pronti alla lettura

    bool server_online=false; //variabile che controlla lo stato del server

    int16_t id_counter; //contatore che tiene traccia degli id da assegnare

    struct s_header header; //struct per ricevere e gestire l'header dei messaggi
    char buffer[MAX_SERVER_BUFFER]; // buffer utilizzato dal server per i trasferimenti
    int status; //variabile utilizzata per gestire ritorno funzioni
    

    in_port_t port; // porta

    fd_set pronti;

    struct sockaddr_in server_address,client_address;
    in_addr_t lenght_client_ip;

    //info sul mittente
    struct u_online*mittente;
    struct game_room*mitt_room;
    struct game_session*m_session;
    char**m_inventory;

    char**other_inventory; //inventario del giocatore nella stessa stanza del mittente (se presente)

    //variabili di appoggio
    int indice,slot,aux_socket;
    char*aux_char;
    enum TipoMessaggio aux_type;

    id_counter=0; //utilizzata per assegnare l'id a chi si connette
    total_players=0; //numero totale di utenti in partita
    port=0;
    
    //inizializzo la variabile globale svr_info con le informazioni del server
    svr_info.id=SERVER_ID;
    svr_info.room=SERVER_ROOM;
    svr_info.info=0;

    list_init(); //inizializzo le liste
    room_info_init(buffer); //inizializzo le stanze leggendo da file

    //azzero
    FD_ZERO(&tutti);
    FD_ZERO(&pronti);

    //inserisco stdin nella lista
    FD_SET(STDIN_FILENO,&tutti);
    max_fd=STDIN_FILENO;

    if(argc<2){ 
        printf("INSERIRE LA PORTA\nSintassi: ./server <port>\n");
        exit(-1);
    }
    if(argc>2){
        printf("TROPPI ARGOMENTI\n");
        exit(-1);
    }
    if(argc==2){ //estraggo la porta da argv
        printf("PORTA SELEZIONATA: %i\n",port=atoi(argv[1]));
    }

        intro_screen(port);
    
    //pulizia
    memset(&server_address,0,sizeof(server_address));
    memset(&client_address,0,sizeof(client_address));
    
    //inizializzo la struct server_address
    server_address.sin_family=AF_INET;
    //server_address.sin_port=port; // la porta viene inizializzata piu avanti
    inet_pton(AF_INET,IP_SERVER,&server_address.sin_addr); //indirizzo del server   
    
    while(1){ //ciclo di attivita' del server
        pronti=tutti;
        select(max_fd+1,&pronti,NULL,NULL,NULL); //il server rimane in attesa nella select
        for(sk=0;sk<=max_fd;sk++){ //verifico quale fd sia pronto alla lettura
            if(FD_ISSET(sk,&pronti)){    
                if(sk==STDIN_FILENO){ //leggo da tastiera
                    fgets(buffer,15,stdin);
                    if(!strcmp(buffer,"\n")) //ignoro ritorno carrello
                        continue;
                    if(!strcmp(buffer,"stop\n")){
                        if(!total_players){ //se non ci sono partite in corso arresto il server
                            if(user_list){ //se ci sono utenti connessi li avviso della terminazione volontaria
                                printf("CHIUSURA CONNESSIONI IN CORSO...\n");
                                kill_all_comms();
                            }
                            printf("ARRESTO IN CORSO...\n");
                            exit(0);
                        }else{
                            printf("Impossibile arrestare, ancora %i utenti in partita\n",total_players);
                            continue;
                        }
                    }
                    if(server_online){
                        if(!strcmp(buffer,"rooms\n")){  //fgets preleva pure il ritorno carrello
                            show_all_games(); //mostro le informazioni sulle stanze
                            continue;
                        }
                        if(!strcmp(buffer,"online\n")){
                            print_online_users();
                            continue;
                        }
                        if(!strncmp(buffer,"dc ",3) && strlen(buffer)>3){
                            disconnect_user(atoi(&buffer[3]));
                            continue;
                        }
                    }else{ 
                        /*
                        if(!server_online && !strncmp(buffer,"start ",6) && strlen(buffer)>6){ //se il server non e' online leggo la porta da stdin
                            port=atoi(&buffer[6]);
                        */
                        if(!server_online && !strcmp(buffer,"start\n")){ //faccio partire il server con solo il comando start

                            server_address.sin_port=htons(port);

                            // inizializzo socket di ascolto e lo aggiungo a "tutti"
                            listen_socket=socket(AF_INET,SOCK_STREAM,0); //socket TCP bloccante

                            if(bind(listen_socket,(struct sockaddr*)&server_address,sizeof(server_address))==-1 ||
                                    listen(listen_socket,20)){

                                if(errno==EADDRINUSE){
                                    printf("INDIRIZZO O PORTA IN USO! RIPROVARE\n");
                                    continue;
                                }
                                perror("ERRORE BIND: ");
                                exit(-1);
                            }

                            FD_SET(listen_socket,&tutti);
                            if(max_fd<listen_socket)
                                max_fd=listen_socket;

                            separatore();
                            server_online=true;
                            printf("Server avviato correttamente sulla porta: %i\n",port);
                            stampa_comandi();
                            continue;
                        }
                    }
                    printf("Comando NON riconosciuto\n");
                    if(server_online)
                        stampa_comandi();
                }
                else{
                    if(sk==listen_socket){ //nuova richiesta di connessione
                        printf("\nRicevuta richiesta di connessione!\n");
                        comms_socket=accept(listen_socket,(struct sockaddr*)&client_address,&lenght_client_ip); //creo il socket di comunicazione
                        if(comms_socket>=0){ //se ha avuto successo lo aggiungo a tutti
                            FD_SET(comms_socket,&tutti);
                            if(comms_socket>max_fd)
                                max_fd=comms_socket;
                        }else
                            printf("Connessione Fallita!\n");
                    }
                    else{ //nuovi messaggi da leggere, sezione di gestione e risposta

                        status=ricevi_messaggio(sk,&header,buffer);
                         //se ho errori chiudo la comunicazione
                        if(check_close_comms(status,mittente,sk)){
                            continue;
                        }
                        //estraggo le informazioni sul mittente, se presenti
                        mittente=search_user(header.id,sk,NULL);
                        if(mittente && mittente->game_room){
                            if(check_for_timeout(mittente->game_room)) //se si trova in partita, guardo se il comando sia arrivato dopo il timeout
                                continue;
                            mitt_room=mittente->game_room;
                            m_session=&mitt_room->session;

                            if(mitt_room->player1==mittente){
                                m_inventory=mitt_room->p1_items;
                            }else
                                m_inventory=mitt_room->p2_items;
                        }

                        switch(header.type){
                            case CLIENT_HELLO: //richiesta del client dell'id di sessione
                                id_counter++;
                                if(!id_counter) //prevenzione overflow
                                    id_counter++;
                                id_counter=htons(id_counter); //invio id_counter in formato network
                                status=invia_messaggio(&svr_info,sk,SERVER_HELLO,NULL,&id_counter,2); // invio id
                                if(check_close_comms(status,mittente,sk))
                                    continue;
                                id_counter=ntohs(id_counter);
                                printf("\nConnessione stabilita con Utente %i\n",id_counter);
                                new_connected_user(id_counter,sk); //aggiungo l'utente alla lista degli utenti connessi
                            break;
        
                            case CLIENT_LOGIN:
                            case CLIENT_SIGNUP:
                                printf("\nRicevuta richiesta di ");
                                if(header.type==CLIENT_LOGIN)
                                    printf("LOGIN");
                                else
                                    printf("SIGNUP");
                                printf(" da Utente %i\n",header.id);
                                if(!search_auth_info(sk,&header,buffer,PASSWORD_PATH)){ // controllo se le informazioni sono presenti nel file delle password
                                    check_close_comms(-1,NULL,sk);
                                }
                            break;

                            case ROOM_INFO:
                                indice=cat_room_name(buffer); //concateno il nome delle stanze
                                svr_info.info=NROOMS; //il campo header.info di questo messaggio contiene il numero delle stanze
                                status=invia_messaggio(&svr_info,sk,ROOM_INFO_ANSW,NULL,buffer,indice);
                                check_close_comms(status,mittente,sk);
                            break;

                            case CLIENT_LOGOUT:
                                printf("%s sta effettuando il logout...\n",mittente->name);
                                logout_handler(mittente);
                            break;

                            // GESTIONE STANZE
                            case JOIN_ROOM:
                                indice=header.room-1; //decremento per usare "indice" come indice di vettore
                                room_init(indice,mittente,NULL); //creo la stanza
                                printf("\n%s e' in partita nella stanza %i\n",mittente->name,indice+1);
                            break;

                            case COOP_ROOM:
                                indice=header.room-1; //decremento per usare "indice" come indice di vettore
                                if(!coop_waiting[indice]){ //se non c'e' un altro giocatore che ha eseguito il comando coop, metto in attesa
                                    coop_waiting[indice]=mittente;
                                    printf("\n%s sta aspettando che un altro Giocatore lo raggiunga nella stanza %i\n",coop_waiting[indice]->name,indice+1);
                                }else{ //altrimenti creo la stanza
                                    room_init(indice,coop_waiting[indice],mittente);                 

                                    printf("\nCOOP ROOM AVVIATA:\nP1: %s - P2: %s\n",coop_waiting[indice]->name,mittente->name);
                                    coop_waiting[indice]=NULL;
                                }
                            break;

                            case ROOM_LEAVE:
                                printf("\n%s ha abbandonato la partita\n",mittente->name);
                                remove_player(mittente);
                            break;

                            case RIDDLE: //l'utente ha inviato la risposta ad un enigma
                                indice=header.info; //il numero dell enigma si trova su header.info (inviato per evitare di risolvere un enigma diverso da quello richiesto precedentemente dall utente)
                                if(strcmp(buffer,m_session->soluzioni[indice])){
                                    invia_messaggio(&svr_info,sk,GAME_MESSAGE,mitt_room,WRONG_ANSW_TXT,strlen(WRONG_ANSW_TXT)+1);//risposta sbagliata
                                    printf("%s ha fallito la risoluzione di un enigma...\n",mittente->name);
                                    check_close_comms(status,mittente,sk);
                                    continue;
                                }
                                if(m_session->oggetti[indice]==FREE || m_session->oggetti[indice]== TOKEN || m_session->oggetti[indice]==TAKEN){ // un utente ha inviato la risposta ad un enigma gia'risolto (possibile in partita coop)
                                    invia_messaggio(&svr_info,sk,GAME_MESSAGE,mitt_room,ALR_ANSW_TXT,strlen(ALR_ANSW_TXT)+1);//enigma gia risolto
                                    check_close_comms(status,mittente,sk);
                                    continue;
                                }
                                aux_char=m_session->mess_soluzioni[indice];//altrimenti preparo il messaggio di soluzione
                                if(m_session->oggetti[indice]==LOCKED)
                                    m_session->oggetti[indice]=FREE;
                                else
                                    m_session->oggetti[indice]=TOKEN;//se non era LOCKED o TAKEN allora era per forza SPECIAL_LOCKED
                                invia_messaggio(&svr_info,sk,GAME_MESSAGE,mitt_room,aux_char,strlen(aux_char)+1);
                                printf("%s ha appena risolto un enigma!\n",mittente->name);
                                check_close_comms(status,mittente,sk);
                            break;

           ///////////////////////////COMANDI DI GIOCO//////////////////////////
                            case LOOK:
                                if(header.len==1){ //look senza argomenti
                                    indice=m_session->first_location;
                                    aux_char=m_session->desc_oggetti[indice];//descrizione stanza
                                    printf("%s sta guardando la stanza\n",mittente->name);
                                    status=invia_messaggio(&svr_info,sk,GAME_MESSAGE,mitt_room,aux_char,strlen(aux_char)+1);
                                    check_close_comms(status,mittente,sk);
                                    continue;
                                }
                                //altrimenti cerco se il target esiste
                                indice=ricerca_indice(buffer,m_session->nome_oggetti,MAX_OBJ,true);
                                if(indice==-1 || m_session->oggetti[indice]==UNAVAILABLE){ //l'oggetto non esiste
                                    status=invia_messaggio(&svr_info,sk,GAME_MESSAGE,mitt_room,NO_LOOK_TXT,strlen(NO_LOOK_TXT)+1);
                                    check_close_comms(status,mittente,sk);
                                    continue;
                                }
                                printf("%s sta guardando \"%s\"\n",mittente->name,m_session->nome_oggetti[indice]);
                                if(m_session->oggetti[indice]==FREE || m_session->oggetti[indice]==TAKEN || m_session->oggetti[indice]==TOKEN){
                                    aux_char=m_session->desc_oggetti[indice]; //invio la descrizione libera
                                }else{
                                    aux_char=m_session->desc_lock_oggetti[indice]; //invio la descrizione locked
                                }
                                status=invia_messaggio(&svr_info,sk,GAME_MESSAGE,mitt_room,aux_char,strlen(aux_char)+1);
                                check_close_comms(status,mittente,sk);
                            break;

                            case TAKE:
                                slot=slot_libero(m_inventory);
                                if(slot==-1){ //guardo se l'inventario ha spazio
                                    printf("%s ha l'inventario pieno...\n",mittente->name);
                                    status=invia_messaggio(&svr_info,sk,GAME_MESSAGE,mitt_room,FULL_OBJ_TXT,strlen(FULL_OBJ_TXT)+1);
                                    check_close_comms(status,mittente,sk);
                                    continue;
                                }     
                                //verifico se il target si trova negli oggetti raccoglibili [0,first_location -1]
                                indice=ricerca_indice(buffer,m_session->nome_oggetti,m_session->first_location,true);

                                if(indice==-1){ //se non e' un oggetto raccoglibile, invio il messaggio di errore
                                    status=invia_messaggio(&svr_info,sk,GAME_MESSAGE,mitt_room,NO_PICKUP_TXT,strlen(NO_PICKUP_TXT)+1);
                                    check_close_comms(status,mittente,sk);
                                    continue;
                                }
                                aux_type=GAME_MESSAGE;
                                switch(m_session->oggetti[indice]){
                                    case SPECIAL_LOCKED:
                                    case LOCKED:
                                        if(!m_session->enigmi[indice])//se non ha un enigma,si risolve con use
                                            aux_char=m_session->desc_lock_oggetti[indice];
                                        else{//si risolve con take
                                            printf("%s sta risolvendo un enigma\n",mittente->name);
                                            aux_type=RIDDLE; //comunico al client che sto inviando un enigma
                                            svr_info.info=indice; //invio il numero dell'enigma per evitare che ne risolva un altro (inviando la soluzione di un altro enigma)
                                            strcpy(buffer,m_session->enigmi[indice]);
                                            aux_char=buffer;
                                        }
                                    break;

                                    case TOKEN: //se un gettone e' stato raccolto, avviso entrambi i giocatori
                                        m_session->oggetti[indice]=TAKEN;
                                        m_session->collected_tokens++;
                                        printf("%s ha appena raccolto un TOKEN!\n",mittente->name);
                                        if(m_session->collected_tokens<m_session->required_tokens){
                                            aux_char=TOKEN_TXT;
                                        }else{//ho raccolto tutti i gettoni: termino la partita
                                            printf("\nE'stata completata una stanza di tipo: \"%s\"\n",m_session->room_name);
                                            aux_char=VICTORY_TXT;
                                            aux_type=VICTORY;
                                        }
                                        if(mitt_room->player1){ //remove_player() si occupera' della distruzione della stanza
                                            invia_messaggio(&svr_info,mitt_room->player1->u_socket,aux_type,mitt_room,aux_char,strlen(aux_char)+1);
                                            if(aux_type==VICTORY) //se si tratta un messaggio di vittoria, rimuovo il giocatore
                                                remove_player(mitt_room->player1);
                                        }
                                        if(mitt_room->player2){
                                            invia_messaggio(&svr_info,mitt_room->player2->u_socket,aux_type,mitt_room,aux_char,strlen(aux_char)+1);
                                            if(aux_type==VICTORY) //se si tratta un messaggio di vittoria, rimuovo il giocatore
                                                remove_player(mitt_room->player2);
                                        }
                                        continue;
                                    break;

                                    case FREE: //lo aggiungo all'inventario
                                        m_inventory[slot]=m_session->nome_oggetti[indice];
                                        m_session->oggetti[indice]=TAKEN;
                                        printf("%s ha raccolto l'oggetto \"%s\"\n",mittente->name,m_inventory[slot]);
                                        sprintf(buffer,"L'oggetto \"%s\" e' stato raccolto!",m_inventory[slot]);
                                        aux_char=buffer;
                                    break;

                                    case TAKEN:
                                        aux_char=ALR_PICKUP_TXT;
                                    break;

                                    case UNAVAILABLE:
                                        aux_char=NO_PICKUP_TXT;
                                    break;
                                }
                                status=invia_messaggio(&svr_info,sk,aux_type,mitt_room,aux_char,strlen(aux_char)+1);
                                check_close_comms(status,mittente,sk);
                            break;

                            case USE:
                                indice=ricerca_indice(buffer,m_session->soluzioni,MAX_OBJ,true); //verifico se la soluzione e' presente
                                if(indice==-1 || m_session->oggetti[indice]==FREE || m_session->oggetti[indice]==TAKEN){ //se non lo e' oppure l'oggetto corrispondente e' gia stato liberato
                                    status=invia_messaggio(&svr_info,sk,GAME_MESSAGE,mitt_room,NO_USE_TXT,strlen(NO_USE_TXT)+1);
                                    check_close_comms(status,mittente,sk);
                                    continue;
                                }
                                slot=ricerca_indice(buffer,m_inventory,INVENTORY_SIZE,false);//cerco se l'oggetto e' presente nell inventario
                                //se non e' presente: cerco se la soluzione ha bisogno effettivamente del possesso di un oggetto
                                if(slot==-1 && ricerca_indice(m_session->soluzioni[indice],m_session->nome_oggetti,m_session->first_location,false)!=-1){ 
                                    status=invia_messaggio(&svr_info,sk,GAME_MESSAGE,mitt_room,NO_OWN_USE,strlen(NO_OWN_USE)+1);
                                    check_close_comms(status,mittente,sk);
                                    continue;
                                }
                                //se si arriva fin qua allora use e' andata a buon fine
                                printf("%s ha usato correttamente un oggetto\n",mittente->name);
                                aux_char=m_session->mess_soluzioni[indice];

                                do{  //libero tutti gli oggetti e locazioni con la stessa soluzione in modo da aggiornarne la descrizione
                                    if(m_session->oggetti[indice]==LOCKED)
                                        m_session->oggetti[indice]=FREE; //ora l oggetto e' libero
                                    else if(m_session->oggetti[indice]==SPECIAL_LOCKED)
                                        m_session->oggetti[indice]=TOKEN;
                                    indice++;
                                    slot=ricerca_indice(buffer,&m_session->soluzioni[indice],MAX_OBJ-(indice),true);
                                    indice+=slot;
                                }
                                while(slot!=-1);
                                
                                status=invia_messaggio(&svr_info,sk,GAME_MESSAGE,mitt_room,aux_char,strlen(aux_char)+1);
                                check_close_comms(status,mittente,sk);
                            break;

                            case OBJS:
                                printf("%s vuole visualizzare il proprio inventario\n",mittente->name);
                                strcpy(buffer,"Inventario:\n\n");
                                for(i=0;i<INVENTORY_SIZE;i++){
                                    sprintf(buffer+strlen(buffer),"%i) ",i+1);
                                    if(m_inventory[i]){
                                        strcat(buffer,m_inventory[i]); //concateno gli oggetti contenuti nell inventario
                                    }else
                                        strcat(buffer,"------");
                                    strcat(buffer,"\n");
                                }
                                status=invia_messaggio(&svr_info,sk,GAME_MESSAGE,mitt_room,buffer,strlen(buffer)+1);
                                check_close_comms(status,mittente,sk);
                            break;

                            case DROP:
                                printf("%s sta cercando di lasciare un oggetto...\n",mittente->name);
                                if(!drop_object(mittente,header.info)){//header.info contiene lo slot dell'inventario scelto
                                    aux_char=NO_DROP_TXT;
                                }else
                                    aux_char=DROP_TXT;
                                status=invia_messaggio(&svr_info,sk,GAME_MESSAGE,mitt_room,aux_char,strlen(aux_char)+1);
                                check_close_comms(status,mittente,sk);
                            break;

                            //comandi COOP
                            case SAY: 
                                printf("%s sta inviando il messaggio \"%s\" al compagno di stanza\n",mittente->name,buffer);
                                aux_socket=(mittente==mitt_room->player1)?mitt_room->player2->u_socket:mitt_room->player1->u_socket;
                                prepare_message(mittente->name,buffer); //attacco l'username del mittente al messaggio

                                status=invia_messaggio(&svr_info,aux_socket,GAME_MESSAGE,mitt_room,buffer,strlen(buffer)+1);
                                check_close_comms(status,mittente,aux_socket);

                                status=invia_messaggio(&svr_info,sk,GAME_MESSAGE,mitt_room,SAY_TXT,strlen(SAY_TXT)+1); //invio la conferma dell'invio al mittente
                                check_close_comms(status,mittente,aux_socket);
                            break;

                            case GIVE:
                                indice=header.info; //header.info contiene lo slot dell'inventario scelto
                                if(m_inventory[indice]==NULL){ //non ho nulla da dare
                                    printf("%s ha tentato di donare uno slot vuoto\n",mittente->name);
                                    status=invia_messaggio(&svr_info,sk,GAME_MESSAGE,mitt_room,NO_GIVE_TXT,strlen(NO_GIVE_TXT)+1);
                                    check_close_comms(status,mittente,sk);
                                    continue;
                                }
                                //prendo l'inventario del destinatario
                                other_inventory=(mitt_room->p1_items==m_inventory)?mitt_room->p2_items:mitt_room->p1_items;
                                slot=slot_libero(other_inventory);
                                if(slot==-1){ //il destinatario ha l'inventario pieno
                                    printf("%s ha fallito la donazione al compagno di stanza (inventario pieno)\n",mittente->name);
                                    status=invia_messaggio(&svr_info,sk,GAME_MESSAGE,mitt_room,FULL_GIVE_TXT,strlen(FULL_GIVE_TXT)+1);
                                    check_close_comms(status,mittente,sk);
                                    continue;
                                }
                                printf("%s sta donando un oggetto al compagno di stanza\n",mittente->name);
                                other_inventory[slot]=m_inventory[indice]; //passaggio dell'oggetto
                                m_inventory[indice]=NULL;

                                status=invia_messaggio(&svr_info,sk,GAME_MESSAGE,mitt_room,GIVE_ANSW_TXT,strlen(GIVE_ANSW_TXT)+1);
                                check_close_comms(status,mittente,aux_socket);

                                //prendo il socket del destinatario
                                aux_socket=(mitt_room->player1==mittente)?mitt_room->player2->u_socket:mitt_room->player1->u_socket;
                                
                                status=invia_messaggio(&svr_info,aux_socket,GAME_MESSAGE,mitt_room,GIVE_TXT,strlen(GIVE_TXT)+1);
                                check_close_comms(status,NULL,aux_socket);
                            break;

                            default:
                            break;
                        }
                    }
                }
            }
        }
    }
}
