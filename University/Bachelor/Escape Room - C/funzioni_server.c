#include "var_server.c"
#include "utility.c"

//funzioni di appoggio per server.c

//stampa la schermata iniziale del server
void intro_screen(in_port_t port){
    separatore();
    printf("\t\tSERVER\n");
    printf("Digita un comando:\n\n");
    //printf("1) start %i   -- Avvia il server di gioco (start <port> per cambiare porta)\n",port);
    printf("1) start   -- Avvia il server di gioco\n");
    printf("2) stop\t-- Interrompe l'esecuzione\n");
    separatore();
}
void stampa_comandi(){
    separatore();
    printf("DIGITA \"online\" PER LISTA UTENTI ONLINE\n");
    printf("DIGITA \"rooms\" PER INFORMAZIONI SULLE PARTITE IN CORSO\n");
    printf("DIGITA \"dc <id>\" PER DISCONNETTERE UN UTENTE\n");
    printf("DIGITA \"stop\" PER ARRESTARE IL SERVER\n");
    separatore();
}

// inizializza la lista di attesa per partite coop e la lista di stanze
void list_init(){
    int i;
    for(i=0;i<NROOMS;i++){         
        coop_waiting[i]=NULL;                         
        room_list[i]=NULL;
    }
}

//converte lo stato letto dal file degli oggetti con gli enumerati
void convert_item_state(struct game_info*info){
    int i,j;
    int8_t*obj=info->oggetti;
    if(!obj)
        return;
    for(i=0,j=0;i<MAX_OBJ+1;i++,j++){
        switch(obj[i]){
            case '|':
                info->first_location=i;//finiti gli oggetti, le prossime saranno location
                j--; //faccio tornare indietro j per recuperare lo spazio perso dal separatore '|'
            break;
            case 'F':
                obj[j]=FREE;
            break;

            case 'L':
                obj[j]=LOCKED;
            break;

            case 'S':
                obj[j]=SPECIAL_LOCKED;
                info->tokens++; //il numero di gettoni di una stanza dipende dal numero di SPECIAL_LOCKED letti dal file
            break;

            default:
                obj[j]=UNAVAILABLE;
            break;
        }
    }
}

// effettua la lettura dal file target di una stringa fino al carattere '\n' (compreso), evitando eventuali righe vuote
char*read_from_room_file(FILE*target,char*buffer){
    int len;
    char*work;
    while(fgets(buffer,512,target) && strlen(buffer)==1){} //evito tutti i \n
    if(!strcmp(buffer,"0\n"))//se leggo uno 0 e basta allora salto quest'informazione
        return NULL;

    len=strlen(buffer);
    buffer[len-1]='\0'; //levo il '\n'

    work=(char*)malloc(len); //alloco il vettore che conterra' la stringa appena letta
    memcpy(work,buffer,len);
    return work;
}

void room_name_init(FILE*target,struct game_info*game,char*buffer){
    while(fgets(buffer,100,target) && strlen(buffer)==1){} //faccio una lettura a vuoto (formattazione del file)
    game->room_name=read_from_room_file(target,buffer); //leggo il nome del file
}

void room_obj_init(FILE*target,struct game_info*game,char*buffer){
    int i,quanti,len;
    while(fgets(buffer,100,target) && strlen(buffer)==1){} //evito tutti i \n poi scarto quello che ho appena letto (formattazione del file)

    fgets(buffer,100,target); //leggo lo stato degli oggetti
    len=strlen(buffer)-1;
    memset(game->oggetti,0,MAX_OBJ+1); //prevenzione caratteri casuali nel caso strlen(buffer)<MAX_OBJ
    memcpy(game->oggetti,buffer,len);
    convert_item_state(game); //converto il valore degli oggetti da char a enumerati

    while(fgets(buffer,100,target) && strlen(buffer)==1){}//evito tutti i \n e lettura a vuoto
    fgets(buffer,100,target); //estraggo quanti oggetti sono presenti nel file
    quanti=atoi(buffer);

    for(i=0;i<quanti;i++){ //leggo le informazioni in quest'ordine
        //locazioni e oggetti sono visti allo stesso modo
        game->nome_oggetti[i]=read_from_room_file(target,buffer);
        game->desc_oggetti[i]=read_from_room_file(target,buffer);
        game->desc_lock_oggetti[i]=read_from_room_file(target,buffer);
        game->enigmi[i]=read_from_room_file(target,buffer);
        game->soluzioni[i]=read_from_room_file(target,buffer);
        game->mess_soluzioni[i]=read_from_room_file(target,buffer);
    }
}

void room_info_init(char*buffer){
    struct game_info*game;
    char path[13];
    int i;
    FILE*target=NULL;
    for(i=1;i<=NROOMS;i++){
        sprintf(path,"room%i.txt",i); //le stanze devono avere un file chiamato roomN.txt
        target=fopen(path,"rb");
        if(!target){
            printf("APERTURA FILE %s FALLITA: ",path);
            perror("");
            exit(-1);
        }
        game=&room_info[i-1];
        memset(game,0,sizeof(struct game_info));

        room_name_init(target,game,buffer);
        room_obj_init(target,game,buffer);
        fclose(target);
    }
}

//la funzione scrive in buffer tutti i nomi delle stanze presenti, ognuno separato da '\0'
int cat_room_name(char*buffer){
    int i;
    int pos=0;
    for(i=0;i<NROOMS;i++){
        strcpy(&buffer[pos],room_info[i].room_name); //scrivo il nome della stanza
        pos+=strlen(room_info[i].room_name)+1; //sposto la posizione di partenza di (nome della stanza)+1 posti
    }
    return pos;
}

// la funzione aggiunge "username: " al messaggio contenuto in buffer
void prepare_message(char*username,char*buffer){
    char*string=(char*)malloc(strlen(buffer)+1);
    memcpy(string,buffer,strlen(buffer)+1);
    sprintf(buffer,"%s: %s",username,string);
    free(string);
}

//stampa gli utenti attualmente collegati e i socket corrispondenti
//considero come online solo quelli che hanno eseguito il login
void print_online_users(){
    struct u_online*work;
    bool nousers=true; //variabile usata per gestire output
    work=user_list;
    printf("\n");
    separatore();
    printf("UTENTI IN GIOCO: %i\n",total_players);
    while(work){
        if(work->name){ //l'utente risulta online solo se ha effettuato il login
            if(nousers){
                nousers=false;
                printf("UTENTI ONLINE:\n");
            }
            printf("Id: %d, Utente: %s, Stanza: ",work->id,work->name);
            if(work->room)
                printf("%i\n",work->room);
            else
                printf("Menu Principale\n");
        }
        work=work->next;
    }
    if(nousers)
        printf("Nessun utente online...\n");
    separatore();
}
//chiamata da show_all_games() per stampare tutte le informazioni che riguardano lo svolgimento della partita, tra cui il contenuto degli inventari,
//lo stato degli oggetti, numero di token raccolti ecc.
void print_sessione(struct game_room*room,struct game_session*sesh){
    int i;
    separatore();
    printf("INFO SESSIONE:\n");
    printf("NOME STANZA: %s\n\n",sesh->room_name);
    printf("INFO OGGETTI:\n");
    for(i=0;i<MAX_OBJ;i++){
        if(sesh->oggetti[i]==UNAVAILABLE){
            continue;
        }
        if(!sesh->nome_oggetti[i]){ //se non e' presente il nome dell'oggetto allora e' la descrizione 
            printf("-----\n");       //della stanza, siamo arrivati al primo degli oggetti che non possiamo raccogliere
            printf("%i) Descrizione Stanza - ",i+1);
        }else
            printf("%i) %s - ",i+1,sesh->nome_oggetti[i]);
        printf("Stato: ");
        if(sesh->oggetti[i]==LOCKED)
            printf("BLOCCATO\n");
        else if(sesh->oggetti[i]==SPECIAL_LOCKED)
            printf("BLOCCATO (TOKEN)\n");
        else if(sesh->oggetti[i]==FREE)
            printf("LIBERO\n");
        else if(sesh->oggetti[i]==TAKEN)
            printf("PRESO\n");
        else if(sesh->oggetti[i]==TOKEN)
            printf("TOKEN\n");
        else{
            printf("INACCESSIBILE\n");
        }
    }
    if(room->player1){
        printf("\n\nINVENTARIO %s:\n",room->player1->name);
            for(i=0;i<INVENTORY_SIZE;i++)
                printf("%i) %s\n",i+1,room->p1_items[i]);
    }
    if(room->player2){
    printf("\nINVENTARIO %s:\n",room->player2->name);
        for(i=0;i<INVENTORY_SIZE;i++)
            printf("%i) %s\n",i+1,room->p2_items[i]);
    }
    printf("\nTOKEN RACCOLTI: %i/%i\n\n",sesh->collected_tokens,sesh->required_tokens);
        
}
//stampa tutte le informazioni che riguardano una partita in corso, e lo stato della lista di attesa
void show_all_games(){
    struct game_room*work;
    time_t ora;
    int tempo_ini;
    int tempo_rim;
    int i,j=1;
    printf("\n\nINFO STANZE:\n");
    for(i=0;i<NROOMS;i++){
    separatore();
        j=1;
        work=room_list[i];
        if(work){
            while(work){
                ora=time(NULL);
                printf("Stanza numero %i di tipo %i\n",j,i+1);
                if(work->player1)
                    printf("P1: %s\n",work->player1->name);
                if(work->player2)
                printf("P2: %s\n",work->player2->name);
                
                print_sessione(work,&work->session);

                tempo_rim=TIME_LIMIT-(ora-work->start_time);
                if(tempo_rim>0){
                    printf("Tempo rimasto: %i min %i sec\n",tempo_rim/60,tempo_rim%60);
                }else
                    printf("TEMPO SCADUTO!\n");
                ora-=work->start_time;
                tempo_ini=*(int*)&ora;
                printf("Creata da: %i min %i sec\n\n\n",tempo_ini/60,tempo_ini%60);
                j++;
                work=work->next;
            }
        }else
            printf("nessuna stanza di tipo %i...\n",i+1);
        separatore();
    }
    for(i=0;i<NROOMS;i++){
        printf("[COOP] Giocatore in attesa per la stanza %i:\n",i+1);
        if(coop_waiting[i]){
            printf("U: %s\n",coop_waiting[i]->name);
        }
        else
            printf("Nessuno...\n");
    }
    separatore();
}



//cerca un utente attualmente collegato al server in base all'id, socket usato o username, e ne restituisce il puntatore
struct u_online* search_user(u_int16_t user,int u_socket,char*username){
    struct u_online*work=user_list;
    while(work && work->id!= user && work->u_socket != u_socket){
        if(username && work->name && !strcmp(username,work->name)){ //se sono presenti username e work->name li confronto
            break;
        }
        work=work->next;
    }
    return work;
}

//crea e aggiunge un nuovo utente in testa alla lista degli utenti connessi
void new_connected_user(u_int16_t new_user,int new_socket){
    struct u_online*work; //creo una nuova struct
    work=(struct u_online*)malloc(sizeof(struct u_online));
    work->id=new_user;
    work->room=0;
    work->u_socket=new_socket;
    work->name=NULL;
    work->game_room=NULL;
    work->next=user_list;
    user_list=work; 
}

//assegna il nome utente all'utente identificato da id, rendendolo online agli occhi del server
bool assign_username(int16_t id, char*username){
    struct u_online*work;
    work = search_user(-1,-1,username); //cerco se c è un altro utente con lo stesso nome
    if(work){
        printf("\nL'Utente %s e' gia' online!\n",username);
        return false;
    }else{
        work=search_user(id,-1,NULL);
        if(work){//dovrebbe esserci sempre ma non si sa mai
            if(work->name==NULL){
                work->name=username;
                printf("%s -- online\n",username);
                return true;
            }else{ //caso limite
                printf("\nL'Utente %s ha gia effettuato il login!!\n",work->name);
                return false;
            }
        }
    }
    return false;
}

//dichiarazioni di funzioni definite piu in basso
void remove_player(struct u_online*user);
bool drop_object(struct u_online*user,int slot);

//rimuove un utente dalla lista degli utenti connessi
void new_offline_user(u_int16_t u_user,int u_socket){
    if(!(user_list)){
        return;
    }else{
        struct u_online**work= &user_list;
        struct u_online*found;
        while(*work){
            if((*work)->id==u_user || (*work)->u_socket==u_socket){
                found=*work;
                *work=(*work)->next;

                if(found->game_room){ //se il giocatore si trovava in partita lo rimuovo
                    printf("\nGiocatore in partita! Rimuovo...\n");
                    remove_player(found);
                }
                printf("\nUtente [ ");
                if(!found->name){
                    printf("ID: %i ] -- Disconnesso\n",found->id);
                }
                else{
                    printf("%s ] -- OFFLINE\n",found->name);
                    free(found->name);
                    found->name=NULL;
                }
                free(found);
                break;
            }
            work=&((*work)->next);
        }
    }
}

//invia il segnale di chiusura al client: il client si disconnettera' e la routine di
//chiusura si occupera' del resto
void disconnect_user(int id){
    struct u_online*work=search_user(id,-1,NULL);
    if(work){
        invia_messaggio(&svr_info,work->u_socket,TERMINATE,NULL,NULL,0);
    }else
        printf("\nUTENTE NON CONNESSO\n");
}

//avvisa tutti gli utenti connessi che la connessione verra' interrotta
void kill_all_comms(){
    struct u_online*work=user_list;
    while(work){
        printf("Chiudo la connessione con ");
        if(work->name)
            printf("%s - ",work->name);
        printf("ID: %d\n",work->id);
        invia_messaggio(&svr_info,work->u_socket,TERMINATE,NULL,NULL,0);
        work=work->next;
    }
    printf("CHIUSURA CONNESSIONI COMPLETATA!\n");
}

//rimuove il nome utente all'utente user rendendolo offline, ma mantenendo la connessione al
//server in modo che possa effettuare di nuovo il login
void logout_handler(struct u_online*user){
    if(user && user->name){
        printf("\nUtente [ %s ] -- Disconnesso\n",user->name);
        free(user->name);
        user->name=NULL;
    }
}

// GESTIONE STANZE

//crea una stanza e associa i giocatori
struct game_room* room_create(struct u_online*player1,struct u_online*player2){
    struct game_room*room=(struct game_room*)malloc(sizeof(struct game_room));

    memset(room,0,sizeof(struct game_room));
    if(player1){
        room->player1=player1;
        total_players++;
        player1->game_room=room;
    }
    if(player2){
        room->player2=player2;
        total_players++;
        player2->game_room=room;
    }
    return room;
}

//crea un'istanza di una stanza di tipo room number 
bool room_init(int room_number,struct u_online*player1,struct u_online*player2){
    int status; //gestisce il ritorno funzioni
    int descr_len; //usata per evitare di chiamare strlen() piu' volte
    struct game_room*room; //una volta inizializzato puntera' alla stanza creata
    struct game_session*ses; //punta alla sessione dentro la stanza
    struct game_info*info; //punta alla struct da cui estrarre le informazioni

    printf("\nInizializzo una nuova stanza - Tipo : %i\n",room_number+1);
    room=room_create(player1,player2);
    ses=&room->session;
    info=&room_info[room_number];
    room->type=room_number;

    //copio lo stato iniziale della stanza
    memcpy(ses->oggetti,info->oggetti,MAX_OBJ);
    ses->required_tokens=info->tokens;

    //assegno i puntatori
    ses->room_name=info->room_name;
    ses->first_location=info->first_location;
    ses->nome_oggetti=info->nome_oggetti;
    ses->desc_oggetti=info->desc_oggetti;
    ses->desc_lock_oggetti=info->desc_lock_oggetti;
    ses->enigmi=info->enigmi;
    ses->soluzioni=info->soluzioni;
    ses->mess_soluzioni=info->mess_soluzioni;

    descr_len=strlen(DEFAULT_TXT)+1;
    room->start_time=time(NULL); //faccio partire il timer
    
    //avviso il\i giocatore\i che la stanza e' pronta
    if(player1){
        player1->room=room_number+1;
        status=invia_messaggio(&svr_info,player1->u_socket,GAME_READY,room,DEFAULT_TXT,descr_len);
        if(status!=0)
            return false;
    }
    if(player2){
        player2->room=room_number+1;
        status=invia_messaggio(&svr_info,player2->u_socket,GAME_READY,room,DEFAULT_TXT,descr_len);
        if(status!=0)
            return false;
    }
    room->next=room_list[room_number]; // aggiungo la nuova stanza in testa alla lista
    room_list[room_number]=room;

    return true;
}

//distrugge una stanza, rimuovendola dalla lista in cui si trovava
void destroy_room(struct game_room*room){
    //rimuovo dalla lista di partite
    struct game_room**work=&room_list[room->type];

    while(*work && *work!=room){
        work=&((*work)->next);
    }
    if(*work){
        *work=(*work)->next;
        if(room){
            printf("Distruggo la stanza...\n");
            free(room);
        }
    }
}

//rimuove un giocatore da una partita in corso
void remove_player(struct u_online*user){
    struct game_room*room;
    struct u_online*p1; //giocatore 1
    struct u_online*p2; //giocatore 2
    struct u_online*other; //giocatore rimanente
    int i;

    if(!user || !user->game_room) //se l utente non è in partita non faccio nulla
        return;
    room=user->game_room;
    p1=room->player1;
    p2=room->player2;
    other=NULL;


    total_players--;

    if(p1 && p2){ //se ci sono 2 giocatori, devo svuotare l'inventario per non impedire all'altro giocatore di completare la stanza
        for(i=0;i<INVENTORY_SIZE;i++)
            drop_object(user,i);
    }
     //rimuovo le informazioni dall'utente e dalla stanza
    if(user==p1){
        p1->room=0;
        p1->game_room=NULL;
        room->player1=NULL;
        other=p2;
    }
    else{
        p2->room=0;
        p2->game_room=NULL;
        room->player2=NULL;       
        other=p1;
    }
    if(p1 && p2){ //se 2 giocatori: avviso il rimanente della dipartita del partner
        invia_messaggio(&svr_info,other->u_socket,COOP_LEAVE,room,COOP_LEAVE_TXT,strlen(COOP_LEAVE_TXT)+1);
        return;
    }
    //SE SI arriva fin qua: un solo giocatore, posso distruggere la stanza
    destroy_room(room);
}

//fine gestione stanze

//rimuove user dalla lista di attesa per coop
void remove_from_waiting(struct u_online*user,int u_socket){
    int i;
    struct u_online*work;
    if(!user)
        work=search_user(-1,u_socket,NULL); //se user non è fornito, lo cerco tramite il socket
    else
        work=user;
    
    for(i=0;i<NROOMS;i++){
        if(work==coop_waiting[i]){
            printf("\nTrovato! (COOP) Rimuovo %s...\n",work->name);
            coop_waiting[i]=NULL;
            break;
        }
    }
}

//se la comunicazione fallisce provvedo a chiudere la connessione
bool check_close_comms(int status,struct u_online*user,int target_socket){
    if(status!=0){
        printf("Chiudo Connessione...\n");
        close(target_socket); //chiudo il socket
        remove_from_waiting(user,target_socket); //rimuovo l'utente dalla lista d'attesa (se ci si trovava)
        new_offline_user(-1,target_socket); //rimuovo l'utente dalla lista degli utenti connessi
        FD_CLR(target_socket,&tutti); //rimuovo il suo socket da tutti
        return true;
    }
    return false;
}

//controlla se la stanza ha oltrepassato il tempo limite
bool check_for_timeout(struct game_room*room){
    time_t time_passed;
    if(!room)
        return false;
    time_passed=time(NULL)-room->start_time;
    if(time_passed>TIME_LIMIT){
        int len=strlen(TIMEOUT_TXT)+1;
        //se il tempo e' scaduto, avviso i giocatori presenti nella stanza
        if(room->player1){
            invia_messaggio(&svr_info,room->player1->u_socket,DEFEAT,NULL,TIMEOUT_TXT,len);
            remove_player(room->player1);
        }
        if(room->player2){
            invia_messaggio(&svr_info,room->player2->u_socket,DEFEAT,NULL,TIMEOUT_TXT,len);
            remove_player(room->player2);
        }
        //remove_player si occupera' di distruggere la stanza
        return true;
    }
    return false;
}

//cerca una stringa in un vettore di stringhe
//full compare==true : la stringa deve essere uguale nella sua interezza
//full compare==false: solo la parte che corrisponde a strlen(source[i]) deve corrispondere (anche se la stringa e' piu lunga)
int ricerca_indice(char*buffer,char**source,int size,bool full_compare){
    int i;
    if(!buffer || !source)
        return -1;
    for(i=0;i<size;i++){ 
        if(source[i]){
            if((full_compare && !strcmp(buffer,source[i])) || //confronto tutta la stringa
                (!full_compare && !strncmp(buffer,source[i],strlen(source[i])))){ //confronto solo fino alla dimensione della seconda stringa
                 break;
            }        
        }
    }
    if(i==size) //se non ho trovato nulla restituisco -1
        i=-1;
    return i;
}

//rilascia l'oggetto nella posizione slot nell'inventario dell'utente
bool drop_object(struct u_online*user,int slot){
    struct game_room*room;
    char**inventory; //puntatore a inventario
    int aux; //variabile di appoggio
    room=user->game_room;
    if(room->player1==user){
        inventory=room->p1_items;
    }else{
        inventory=room->p2_items;
    }
    if(inventory[slot]){//Oggetto presente: lo droppo
        printf("\nOggetto da droppare: %s - rimetto al suo posto...\n",inventory[slot]);
        aux=ricerca_indice(inventory[slot],room->session.nome_oggetti,MAX_OBJ,true); //cerco il posto in cui rimettere l'oggetto
        if(aux!=-1){
            room->session.oggetti[aux]=FREE; //l'oggetto puo' essere nuovamente raccolto
            inventory[slot]=NULL;
            return true;
        }
    }
    return false;
}

//cerca uno slot libero nell'inventario
int slot_libero(char**inv){
    int i;
    for(i=0;i<INVENTORY_SIZE;i++){
        if(inv[i]==NULL)
            break;
    }
    if(i==INVENTORY_SIZE)
        return -1;
    else
        return i;
}

//cerca il nome utente e la password all'interno del file di cui si e' specificato il percorso
//se ha successo, inserisce il nome utente nella lista di utenti connessi
//e invia la risposta al client
//restituisce falso se ci sono errori irreparabili durante la comunicazione
bool search_auth_info(int comms_socket,struct s_header* header,char*buffer,const char*path){
    char*username;
    char*password;
    FILE*target; //file delle password
    int status; //variabile per gestire ritorni
    int8_t type;
    struct stat info; //conterra'le info del file
    int8_t counter=-1; // tiene traccia dello stato della stringa prelevata (se nome utente o password)
    bool found_username=false;
    bool found_password=false;

    //estrazione dimensione file
    status=stat(path,&info);

    if(status==-1 && header->type==CLIENT_LOGIN){ //il file non è presente, impossibile fare login
        printf("Login Utente %i fallito\n",header->id);
        status=invia_messaggio(&svr_info,comms_socket,BAD_LOGIN,NULL,NULL,0);
        if(status!=0)
            return false;
        return true;
    }

    int file_size=info.st_size;

    if(file_size<10 && header->type==CLIENT_LOGIN){ // dimensione minima: username (4), \n(1) , password (4) ,\n (1)
        printf("Login Utente %i fallito\n",header->id); //il file non contiene informazioni valide
        status=invia_messaggio(&svr_info,comms_socket,BAD_LOGIN,NULL,NULL,0);
        if(status!=0)
            return false;
        return true;
    }

    target=fopen(PASSWORD_PATH,"a+"); //apro il file in lettura e scrittura senza troncarlo e se non esiste lo creo
    if(target)                         
        fseek(target,0,SEEK_SET);       //mi sposto all inizio
    else{
        type=(header->type==CLIENT_LOGIN)?BAD_LOGIN:BAD_SIGNUP;
        status=invia_messaggio(&svr_info,comms_socket,type,NULL,NULL,0);
        if(status!=0)
            return false;
        return true;
    }

    //inizializzo username e password per poter riusare buffer 
    username=(char*)malloc(strlen(buffer)+1);
    password=(char*)malloc(strlen(&buffer[strlen(buffer)+1])+1);

    strcpy(username,buffer);
    strcpy(password,&buffer[strlen(username)+1]);

    //ricerca nel file
    while((status = fscanf(target,"%s",buffer))>0){
        counter= -counter;
        if(counter>0){ //nel buffer è contenuto un username
            if(!strcmp(buffer,username))
                found_username=true;
        }else{ //nel buffer è contenuta una password
            if(!strcmp(buffer,password))
                found_password=true;
            if(found_username || found_password){ //se ho trovato almeno uno dei 2 in questa iterazione posso uscire dal ciclo
                free(password); //la password non mi serve piu se la condizione è vera
                password=NULL;
                break;
            }
        }
    }

    if((found_username || found_password) && header->type==CLIENT_SIGNUP){ // sicuramente non posso fare signup se esiste un nome utente o password uguale a quella inserita
        printf("\nSignup Utente %i fallito\n",header->id);
        free(username);
        //free(password); password gia liberata 
        status=invia_messaggio(&svr_info,comms_socket,BAD_SIGNUP,NULL,NULL,0); //invio solo l'header
        fclose(target);
        if(status!=0)
            return false;
        return true;
    }
    if((!found_username || !found_password) && header->type==CLIENT_LOGIN){ // sicuramente non posso fare login se non ho trovato anche solo uno dei 2
        printf("\nLogin Utente %i fallito\n",header->id);
        free(username);
        if(password)
            free(password);
        status=invia_messaggio(&svr_info,comms_socket,BAD_LOGIN,NULL,NULL,0);
        fclose(target);
        if(status!=0)
            return false;
        return true;
    }

    if(!assign_username(header->id,username)){ //utente gia online oppure non trovato
        free(username);
        if(password){
            free(password);
            password=NULL;
        }
        if(header->type==CLIENT_LOGIN)
            type=BAD_LOGIN;
        else
            type=BAD_SIGNUP;
    }else{
        if(header->type==CLIENT_LOGIN){ // si puo' fare il login
            printf("Login");
            type=LOGIN_ACK;

        }else{  //si puo fare il signup
            fseek(target,0,SEEK_END); //vado in fondo al file
            fprintf(target,"%s\n%s\n\n",username,password); //aggiungo le informazioni
            type=SIGNUP_ACK;
            printf("Signup");
        }
        printf(" %s completato\n",username);
    }
    status=invia_messaggio(&svr_info,comms_socket,type,NULL,NULL,0);
    fclose(target);
    if(password)
        free(password);
    if(status!=0)
            return false;
    return true;
}