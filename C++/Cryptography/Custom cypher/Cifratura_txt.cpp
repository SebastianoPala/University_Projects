#include <iostream>
#include <fstream>
#include <cstdio>

#include <windows.h>
#include <stdlib.h>
#include <direct.h>
#include <string.h>

#define MAX_CHIAVE 1200 //multiplo di 3 possibilmente
#define MIN_CHIAVE 6
#define MAX_PART_CHIAVE MAX_CHIAVE/3
#define MIN_MESS 20
#define MAX_MESS 15000 //mutliplo di MAX_CHIAVE possibilmente
#define MAX_OUTPUT 80
#define MAX_LNG_CMD 12 //verifica
#define MIN_LNG_CMD 4

//formattazione output
#define COLORE "\033[36m"
#define COLORE2 "\033[32m"
#define ERRORE "\033[31m"
#define NORMALE "\033[0m"
#define CLEAR "\033[2;1H\033[J"
#define CLEAR_ROW "\033[K"
#define CLEAR_BOTTOM "\033[J"
#define SU "\033[A"
#define GIU "\033[B"
#define TOP "\033[2;1H"
#define SAVE "\033[s"
#define LOAD "\033[u"


using namespace std;

int head;
int DIMCHIAVE=0;
int DIMCHIAVE1;
int DIMCHIAVE2;
int DIMCHIAVE3;
int DIMMESSAGGIO;

char EVEN='9';
char ODD='d';
char mode; //cifrare o decifrare
char dafile;
char choose; //variabile usata per input
char critempty=0;
char cmd_vct[MAX_LNG_CMD];
char chiave[MAX_CHIAVE];
char chiave1[MAX_PART_CHIAVE];
char chiave2[MAX_PART_CHIAVE];
char chiave3[MAX_PART_CHIAVE];
char messaggio[MAX_MESS];
char err1[9]={"Comando "};
char err2[19]={" non riconosciuto\n"};
char errdr[51]={"!!!CREAZIONE CARTELLA FALLITA: PERMESSO NEGATO!!!\n"};
char errdr2[27]={"!!!IMPOSSIBILE SALVARE!!!\n"};
char errcrit[40]={"!!!NESSUN CRITTOGRAMMA DA DECIFRARE!!!\n"};
char main_folder[15]={"File_Cifratura"};
char keyfolder[8]={"/Chiavi"};
char messfolder[10]={"/Messaggi"};
char critfolder[14]={"/Crittogrammi"};
char pathkey[7]={"Chiave"};
char pathmess[10]={"Messaggio"};
char pathcrit[13]={"Crittogramma"};
char punto[5]={".txt"};
char separatore[15]={"-------------\n"};
char tempchar[2];
char helpvett[70]={"HELP\nSHOWK\nSHOWM\nSHOWC\nSHOWALL\nDELALL\nDELK\nDELM\nDELC\nCLEAR\nEXIT\n----\n"};
char username[50];

bool showall;
bool keyempty[9];
bool filevuoto=true;
bool saved=false;

void comando_sbagliato(char c){
	cout<<LOAD<<CLEAR_ROW<<ERRORE<<err1<<c<<err2<<CLEAR_BOTTOM<<NORMALE;
}

char mask(char c){ //"fusione" minuscola e maiuscola
	return c&0xDF;
}

void shuffle_key(char*c,int dim){
	for(int i=0; i<dim;i++){
		if((i%2)==0){
			if(mode=='C')
				c[i]+=(EVEN+i);
			else
				c[i]-=(EVEN+i);
		}else{
			if(mode=='C')
				c[i]-=(ODD+i);
			else
				c[i]+=(ODD+i);
		}
	}
}
char* unisci_path(char*path,char*i){ //restituisce vettore allocato nell heap! deallocare dopo l uso
	char*tempstr = new char[strlen(path)+strlen(punto)+3];
	tempstr[0]='/';
	strcpy(&tempstr[1],path);
	strcat(tempstr,i);
	strcat(tempstr,punto);
	return tempstr;
}
char* unisci_directory(char*path1,char*path2,char*path3=nullptr){ //restituisce vettore allocato nell heap! deallocare dopo l uso
	char*tempstr = new char[strlen(path1)+strlen(path2)+((!path3)?0:strlen(path3))+1];
	strcpy(tempstr,path1);
	strcat(tempstr,path2);
	if(path3)
		strcat(tempstr,path3);
	return tempstr;

}
void gestore_sottocartella(char*subdr,bool cancella=false){
	char*svolazzante=unisci_directory(main_folder,subdr);
	if(!cancella)
		_mkdir(svolazzante);
	else
		_rmdir(svolazzante); //funziona solo se cartella vuota
	delete[] svolazzante;
}
char* crea_file_path(char*dir1,char*dir2,char*path,char i){
	if(i<'1' || i>'9'){
		cerr<<ERRORE<<"!!! ERRORE NELLA crea_file_path() !!!\n"<<NORMALE;
		exit(0);
	}
	tempchar[0]=i;
	char*nomefile=unisci_path(path,tempchar);
	char*pathfile=new char[strlen(dir1)+((!dir2)?0:strlen(dir2))+strlen(nomefile)+1];
	strcpy(pathfile,dir1);
	if(dir2){
		strcat(pathfile,dir2);
	}
	strcat(pathfile,nomefile);
	delete[] nomefile;
	return pathfile;
}
void status(char*nome,char i,char*contenuto,int quanti,int cont_lenght=0){
	if(quanti)
		cout<<COLORE2;
	else
		cout<<ERRORE;
	cout<<nome<<' '<<i<<" ("<<quanti<<"):"<<NORMALE;
	if(!quanti){
		cout<<separatore;
	}else{
		contenuto[quanti]='\0';
		if(cont_lenght && cont_lenght<quanti){
			contenuto[cont_lenght]='\0';
			strcat(contenuto,"...");
		}
		cout<<contenuto<<endl;
	}
}
void show_file(int type,char*appoggio,char*subfolder,char*path,int counter){ //type: 0 chiavi, 1 messaggi, 2 crittogrammi
	if(type>2 || type <0)
		return;
	FILE* fd;
	char pp='1';
	for(int i=0;i<9;i++){
		int quanti=0;
		int size=0;
		char*dealloc=crea_file_path(main_folder,subfolder,path,pp);
		fd=fopen(dealloc,"rb");
		if(fd){
			quanti=fread(appoggio,1,counter,fd); //prelevo i primi 'counter' caratteri
			fseek(fd,0,SEEK_END);
			size=ftell(fd);		  //prelevo dimensione del file
			fclose(fd);
		}
		if(type != 2){
			status(path,pp,appoggio,(!fd || quanti<((!type)?MIN_CHIAVE:MIN_MESS))?0:size,MAX_OUTPUT);
		}else{
			if(fd && quanti){
				cout<<COLORE2<<path<<' '<<pp<<" Presente ["<<size<<"]\n"<<NORMALE;
			}else{
				cout<<ERRORE<<path<<' '<<pp<<" Vuoto [X]\n"<<NORMALE;
				critempty++;
			}
		}
		if(!type)
			keyempty[i]=(!fd || quanti<MIN_CHIAVE)?true:false;
		delete[] dealloc;
		pp++;
	}
}
void visualizza_chiavi(){
	char*tempchiave= new char[MAX_OUTPUT+4];
	show_file(0,tempchiave,keyfolder,pathkey,MAX_OUTPUT);
	delete[] tempchiave;
}
void visualizza_messaggi(){
	char*tempmessaggio= new char[MAX_OUTPUT+4];
	show_file(1,tempmessaggio,messfolder,pathmess,MAX_OUTPUT);
	delete[] tempmessaggio;
}
void visualizza_crittogrammi(){
	critempty=0;
	char temp;
	show_file(2,&temp,critfolder,pathcrit,1);
}
void visualizza_tutto(){
	cout<<COLORE<<&keyfolder[1]<<": \n"<<NORMALE;
	visualizza_chiavi();
	cout<<separatore;
	cout<<COLORE<<&messfolder[1]<<": \n"<<NORMALE;
	visualizza_messaggi();
	cout<<separatore;
	cout<<COLORE<<&critfolder[1]<<": \n"<<NORMALE;
	visualizza_crittogrammi();
	cout<<separatore;
}

void flush_file(int type){ // 0 = chiavi, 1 = messaggi, 2 = crittogrammi
	char pp='1';
	for(int i=0;i<9;i++){
		char*dealloc=crea_file_path(main_folder,
		(!type)?keyfolder:((type==1)?messfolder:critfolder),
		(!type)?pathkey:((type==1)?pathmess:pathcrit),pp);
		remove(dealloc);
		delete[] dealloc;
		pp++;
	}
	gestore_sottocartella((!type)?keyfolder:((type==1)?messfolder:critfolder),true);
	cout<<ERRORE<<'"';
	if(!type)
		cout<<&keyfolder[1];
	else if(type==1)
		cout<<&messfolder[1];
	else{
		cout<<&critfolder[1];
		critempty=9;
	}
	cout<<'"'<<" Cancellato Correttamente"<<NORMALE<<endl;
}

void flush_all(){
	for(int i=0;i<3;i++)
		flush_file(i);
}

void full_clear(){
	system("cls");
	cout<<SU<<COLORE<<"-------------Cifratore V4.0-------------\n"<<NORMALE;
}
void rm_showall(){
	showall=false;
	full_clear();
	//sezione username
	DWORD username_len=50;
	cout<<COLORE;
	if(GetUserName(username,&username_len)){
		cout<<"Hello "<<username<<endl;
	}
	cout<<"Vuoi Cifrare o Decifrare? (C/D) (i = Info | help = Lista comandi)\n"<<NORMALE;
}

bool check_command(char*cmd){ // controlla se la stringa è un comando
	int lenght=strlen(cmd);
	for(int i=0;i<lenght;i++)
		cmd[i]=mask(cmd[i]);
	if(lenght==4){
		if(!strcmp(cmd,"HELP")){
			cout<<COLORE2<<"Comandi:\n"<<NORMALE<<helpvett<<NORMALE;
			return true;
		}if(!strcmp(cmd,"DELK")){
			flush_file(0);
			return true;
		}if(!strcmp(cmd,"DELM")){
			flush_file(1);
			return true;
		}if(!strcmp(cmd,"DELC")){
			flush_file(2);
			return true;
		}if(!strcmp(cmd,"EXIT")){
			system("cls");
			exit(0);
		}
	}else if(lenght==5){
		if(!strcmp(cmd,"CLEAR")){
			rm_showall();
			return true;
	    }
		if(!strcmp(cmd,"SHOWK")){
			cout<<COLORE<<&keyfolder[1]<<": \n"<<NORMALE;
			visualizza_chiavi();
			cout<<separatore;
			return true;
		}
		if(!strcmp(cmd,"SHOWM")){
			cout<<COLORE<<&messfolder[1]<<": \n"<<NORMALE;
			visualizza_messaggi();
			cout<<separatore;
			return true;
		}
		if(!strcmp(cmd,"SHOWC")){
			cout<<COLORE<<&critfolder[1]<<": \n"<<NORMALE;
			visualizza_crittogrammi();
			cout<<separatore;
			return true;
		}
	}else if(lenght==6){
		if(!strcmp(cmd,"DELALL")){
			flush_all();
			return true;
		}
	}else if(lenght==7){
		if(!strcmp(cmd,"SHOWALL")){
			showall=true;
			visualizza_tutto();
			return true;
		}
	}
	return false;
}

void estrai_even_odd(){
	char*temp=&ODD;
	char*antitemp=&EVEN;
	for(int i=DIMCHIAVE-1;i>=0;i--){
		if(chiave[i]>*temp){
			*temp=chiave[i];
			if(temp==&ODD){
				temp=&EVEN;
				antitemp=&ODD;
			}else
				break;
		}
	}
}
int size_inc(int size){
	while(size%5)
		size++;
	return size;
}
void calcola_permutazione(int*permutazione,int*ordine,int size){
	int posizione=0;
	int min_index;
	int appoggio=0;
	for(int i=0;i<size;i++){
		ordine[i]=i;
		min_index=appoggio;
		while(chiave[min_index]==-1)
			min_index++;
		appoggio=min_index;
		for(int j=0;j<size;j++){
			if(chiave[j]==-1)
			continue;
			if(chiave[j]<chiave[min_index]){
				min_index=j;
			}
		}
		permutazione[min_index]=posizione++;
		chiave[min_index]=-1;
	}
}
int find_index(int num,int*vett,int size){
	for(int i=0;i<size;i++)
		if(num==vett[i])
			return i;
	return 0;
}
void swap_colonne(char*matrice,int rows,int col,int col1, int col2){//sposta la colonna da col1 a col2
	char aux;
	for(int i=0;i<rows;i++){
		aux=matrice[col2+i*col];
		matrice[col2+i*col]=matrice[col1+i*col];
		matrice[col1+i*col]=aux;
	}

}
void smistamento_mess(int*perm_start,int*perm_finish,int size){ //va da una permutazione ad un altra
	int aux;
	int index;
	int same=0;
	int rows=DIMMESSAGGIO/size;
	while(same<size){
		same=0;
		for(int i=0;i<size;i++){
			if(perm_start[i]==perm_finish[i]){
				++same;
				continue;
			}
			index=find_index(perm_start[i],perm_finish,size);
			swap_colonne(messaggio,rows,size,i,index);
			aux=perm_start[index];
			perm_start[index]=perm_start[i];
			perm_start[i]=aux;
		}
	}
}
void mescola_mess(){ //arrivati a sto punto "chiave" non serve piu: posso modificarlo
	int size;
	if(DIMCHIAVE>DIMMESSAGGIO)
		size=DIMMESSAGGIO;
	else
		size=DIMCHIAVE;
	int*permutazione=new int[size];
	int*ordine=new int[size];
	calcola_permutazione(permutazione,ordine,size);
	if(mode=='C')
		smistamento_mess(ordine,permutazione,size);
	else
		smistamento_mess(permutazione,ordine,size);
	delete[] permutazione;
	delete[] ordine;
}

int main(){
	if(_mkdir(main_folder)!=0 && errno==EACCES){
		cout<<ERRORE<<errdr<<NORMALE<<endl;
		system("pause");
		return 0;
	}
	system("reg add HKCU\\Console /v VirtualTerminalLevel /t REG_DWORD /d 1 /f");
	gestore_sottocartella(messfolder);
	gestore_sottocartella(critfolder);
	rm_showall();
	cout<<SAVE;
	while(1){ 
		cin>>cmd_vct;
		if(showall)
			rm_showall();
		else
			cout<<LOAD<<CLEAR_BOTTOM;
		if((strlen(cmd_vct)>=MIN_LNG_CMD) && check_command(cmd_vct))
			continue;
		mode=mask(cmd_vct[0]);
		if(mode=='C' || mode=='D'){
			if(mode=='D' && critempty==9){
				cout<<ERRORE<<errcrit<<NORMALE;
				system("pause");
				return 0;
			}
			cout<<CLEAR;
			break;
		}
		if(mode=='I'){
			cout<<"----\n"<<COLORE<<"Autore:"<<NORMALE<<" Sebastiano Pala"<<endl;
			cout<<COLORE<<"Dimensioni Correnti:"<<NORMALE<<"\nChiave->"<<MAX_CHIAVE<<"\nMessaggio->"<<MAX_MESS<<endl;
			cout<<"[Default] Even-Odd = ("<<EVEN<<','<<ODD<<')'<<endl;
			cout<<"----\n";
			continue;
		}
		cout<<"\033[K"<<GIU;
		comando_sbagliato(mode);
	}
	full_clear();  //prevenzione showall
	cout<<COLORE<<"Vuoi Utilizzare le Chiavi Salvate? (Y/N)\n"<<NORMALE;
	while(1){
		cin>>choose;
		choose=mask(choose);
		if(choose=='Y' || choose=='S'){
chooseagain:
			full_clear();
			cout<<COLORE<<"Selezionare una Chiave [1-9] da utilizzare (0 per Inserirla Manualmente,C per Cancellarla)"<<NORMALE<<endl;
			visualizza_chiavi();
			cout<<CLEAR_BOTTOM<<COLORE<<"Chiave: "<<NORMALE;
			cout<<SAVE;
			while(1){
				cin>>choose;
				if(mask(choose)=='C'){
					cout<<SU<<CLEAR_ROW;
						cout<<TOP<<CLEAR_ROW;
						cout<<COLORE<<"Selezionare una Chiave [1-9] da Cancellare (0 per Annullare)\n"<<NORMALE;
						cout<<"\033[12;1H\033[J";
						cout<<CLEAR_BOTTOM<<COLORE<<"Chiave: "<<NORMALE;
						cout<<SAVE;
					while(1){
						cin>>choose;
						int chnum=choose;
						chnum=choose-48;
						if(!chnum)
							goto chooseagain;
						if(chnum>0 && chnum <9){
							if(keyempty[chnum-1]==true)
								cout<<SU<<CLEAR_ROW<<ERRORE<<"Nulla da Cancellare\n"<<NORMALE;
							else{
								char*dealloc=crea_file_path(main_folder,keyfolder,pathkey,choose);
								if(!remove(dealloc)){
									cout<<COLORE<<pathkey<<' '<<choose<<" Cancellato con Successo\n"<<NORMALE;
									keyempty[chnum-1]=true;
								}else
									cout<<ERRORE<<"!!! CANCELLAZIONE FALLITA !!!\n"<<NORMALE;
								delete[] dealloc;
								goto chooseagain;
							}
						}
						comando_sbagliato(choose);
					}
				}
				int chnum=choose;
				chnum=choose-48;
				if(!chnum){
					choose='N';
					goto acquirekey;
				}
				if(chnum<0 || chnum>9){
					comando_sbagliato(choose);
					continue;
				}
				if(keyempty[chnum-1]){
					cout<<SU<<CLEAR_ROW;
					cout<<ERRORE<<"La Chiave "<<chnum<<" e' Vuota o non Esiste\n"<<NORMALE;
					continue;
				}
				char*dealloc=crea_file_path(main_folder,keyfolder,pathkey,choose);
				FILE* kf=fopen(dealloc,"rb");
				DIMCHIAVE=fread(chiave,1,MAX_CHIAVE-1,kf);
				cout<<CLEAR<<COLORE<<"Chiave "<<choose<<" Scelta: "<<NORMALE;
				printf("%.80s",chiave); //precauzione contro chiavi troppo lunghe
				if(DIMCHIAVE>80)
					cout<<"...";
				cout<<endl;
				fclose(kf);
				delete[] dealloc;
				goto keyacquired;
			}
		}
		if(choose=='N')
			break;
		comando_sbagliato(choose);
	}
	
acquirekey:
	cout<<CLEAR<<COLORE<<"Inserisci Chiave (Dimensioni: ["<<NORMALE<<MIN_CHIAVE<<'-'<<MAX_CHIAVE<<COLORE<<"]): \n"<<NORMALE;
	cin.ignore();
	cout<<SAVE;
shortkey:
	cin.getline(chiave,MAX_CHIAVE);
	if((DIMCHIAVE=strlen(chiave))<MIN_CHIAVE){
		cout<<LOAD<<CLEAR_ROW;
		cout<<ERRORE<<"CHIAVE TROPPO CORTA\n"<<CLEAR_ROW<<NORMALE;
		goto shortkey;
	}
keyacquired:
	cout<<COLORE<<"Lunghezza Chiave: "<<NORMALE<<DIMCHIAVE<<endl;
	if(choose=='N'){ // se false non ha senso risalvarla
			cout<<COLORE<<"Salvare la Chiave?(Y/N)\n"<<NORMALE;
			cout<<SAVE;
		while(1){
			cin>>choose;
			choose=mask(choose);
			if(choose=='Y' || choose=='S'){
					cout<<CLEAR<<COLORE<<"Scegliere la Posizione [1-9] in cui Salvare la Chiave (0 per Annullare)\n"<<NORMALE;
					visualizza_chiavi();
					cout<<CLEAR_BOTTOM<<COLORE<<"Chiave: "<<NORMALE;
					cout<<SAVE;
				while(1){
					cin>>choose;
					int chnum=choose-48;
					if(!chnum){

						cout<<CLEAR<<COLORE<<"Chiave NON Salvata\n"<<NORMALE;
						goto nosave;
					}
					if(chnum>0 && chnum<=9){
						gestore_sottocartella(keyfolder);
						char*dealloc=crea_file_path(main_folder,keyfolder,pathkey,choose);
						std::ofstream outkey(dealloc,ios::out | ios::trunc);
						outkey << chiave;
						outkey.close();
						cout<<CLEAR<<COLORE<<"Chiave Salvata in: "<<NORMALE<<dealloc<<endl;
						delete[] dealloc;
						break;
					}
					comando_sbagliato(choose);
				}
				break;
			}else if(choose=='N'){
				cout<<CLEAR;
				break;
			}else
				comando_sbagliato(choose);
		}
	}
nosave:
	cout<<COLORE<<"Selezionare il ";
	if(mode=='D')
		cout<<pathcrit;
	else
		cout<<pathmess;
	cout<<" [1-9] da cui Leggere";
	if(mode=='C')
		cout<<" (0 per Inserirlo Manualmente)";
	cout<<NORMALE<<CLEAR_BOTTOM<<endl;
	if(mode=='C'){
		visualizza_messaggi();
		cout<<COLORE<<pathmess<<": ";
	}else{
		visualizza_crittogrammi();
		if(critempty==9){
			cout<<CLEAR<<ERRORE<<errcrit<<NORMALE;
			system("pause");
			return 0;
		}
		cout<<COLORE<<pathcrit<<": ";
	}
	cout<<NORMALE;
	cout<<SAVE;

	while(true){
		cin>>dafile;
		int chnum=dafile-48;
		if(chnum>0 && chnum<=9){
			char*dealloc=crea_file_path(main_folder,(mode=='D')?critfolder:messfolder,(mode=='D')?pathcrit:pathmess,dafile);
			FILE* fd=fopen(dealloc,"rb");
			if(!fd){
				cout<<CLEAR<<ERRORE<<"!!! OPEN FALLITA !!!\n"<<NORMALE;
				goto emptyfile;
			}
			DIMMESSAGGIO=fread(messaggio,1,MAX_MESS-1,fd);
			if(DIMMESSAGGIO<MIN_MESS){
				cout<<CLEAR<<ERRORE<<"File Vuoto\n"<<NORMALE;
				goto emptyfile;
			}
			filevuoto=false;
			messaggio[DIMMESSAGGIO]='\0';
			fclose(fd);
			delete[] dealloc;
			break;
		}else if(!chnum && mode=='C'){ //inserimento manuale messaggio
			cout<<CLEAR;
			filevuoto=false; //se file vuoto il messaggio verrà salvato automaticamente
emptyfile:	if(filevuoto && mode=='D')
				goto nosave;
			cout<<COLORE<<"Inserisci Messaggio (Dimensioni: ["<<NORMALE<<MIN_MESS<<'-'<<MAX_MESS<<COLORE<<"]):\n"<<NORMALE;
			cin.ignore();
			cout<<SAVE;
shortmess:
			cin.getline(messaggio,MAX_MESS);
			if((DIMMESSAGGIO=strlen(messaggio))<MIN_MESS){
				cout<<LOAD<<CLEAR_BOTTOM;
				cout<<ERRORE<<"MESSAGGIO TROPPO CORTO\n"<<NORMALE;
				goto shortmess;
			}
			cout<<COLORE<<"Dimensione Messaggio: "<<NORMALE<<DIMMESSAGGIO<<endl;
			if(!filevuoto && mode=='C'){
				dafile=0;
				cout<<COLORE<<"Dove Salvare il Messaggio? [1-9] (0 o N per non Salvarlo)\n"<<NORMALE;
				visualizza_messaggi();
				cout<<pathmess<<": ";
				cout<<SAVE;
				while(1){
					cin>>choose;
					chnum=choose-48;
					if(chnum>0 && chnum <=9){
						dafile=choose;
						saved=true;
						char*dealloc=crea_file_path(main_folder,messfolder,pathmess,choose);
						FILE*fd=fopen(dealloc,"wb");
						delete[] dealloc;
						if(fd){
							fwrite(messaggio,1,DIMMESSAGGIO,fd);
							fclose(fd);
						}else{
							cout<<ERRORE<<"!!! SALVATAGGIO FALLITO !!!"<<NORMALE<<endl;
						}
						break;
					}if(mask(choose)=='N' || !chnum){
						break;
					}
					comando_sbagliato(choose);
				}
			}else if(filevuoto){
				char*dealloc=crea_file_path(main_folder,messfolder,pathmess,dafile);
				FILE* fd=fopen(dealloc,"wb");
				delete[] dealloc;
				if(fd){
					fwrite(messaggio,1,DIMMESSAGGIO,fd);
					fclose(fd);
				}else{
					cout<<ERRORE<<"!!! SALVATAGGIO FALLITO !!!"<<NORMALE<<endl;
				}
			}
			break;
		}else
			comando_sbagliato(dafile);
	}
	cout<<CLEAR;
	if((filevuoto || saved)&& mode=='C'){
		cout<<COLORE<<"Messaggio Salvato nello Slot: "<<NORMALE<<dafile<<endl;
	}

	//estrazione EVEN-ODD
	estrai_even_odd();

	//inizializzazione chiave1 e chiave2
	cout<<COLORE<<"Inizializzazione chiave...\n"<<NORMALE;
	DIMCHIAVE1=DIMCHIAVE/3;
	DIMCHIAVE3=DIMCHIAVE1;
	DIMCHIAVE2=DIMCHIAVE-(DIMCHIAVE1+DIMCHIAVE3);
	chiave1[DIMCHIAVE1]=chiave2[DIMCHIAVE2]=chiave3[DIMCHIAVE3]='\0';
	strncpy(chiave1,chiave,DIMCHIAVE1);
	strncpy(chiave2,&chiave[DIMCHIAVE1],DIMCHIAVE2);
	strncpy(chiave3,&chiave[DIMCHIAVE1+DIMCHIAVE2],DIMCHIAVE3);
	if((mode=='C' && chiave[DIMCHIAVE-2]&1) || (mode=='D' && !(chiave[DIMCHIAVE-2]&1))) //conversione chiave: CDC o DCD
		for(int i=0;i<DIMCHIAVE2;i++){
			chiave2[i] = -chiave2[i];
		}
	else{
		for(int i=0;i<DIMCHIAVE1;i++){
			chiave1[i] = -chiave1[i];
		}
		for(int i=0;i<DIMCHIAVE3;i++){
			chiave3[i] = -chiave3[i];
		}
	}
	if(mode=='C'){
		mescola_mess(); //se cifro prima mescolo...
	}
	//sezione traduzione
	cout<<COLORE<<"Conversione Messaggio...\n"<<NORMALE;
	int tempdim;
	char*tempk;
	for(int nmb=0;nmb<3;nmb++){
		if(mode=='C'){
			tempdim=(!nmb)?DIMCHIAVE1:((nmb==1)?DIMCHIAVE2:DIMCHIAVE3);
			tempk=(!nmb)?chiave1:((nmb==1)?chiave2:chiave3);
		}else{
			tempdim=(!nmb)?DIMCHIAVE3:((nmb==1)?DIMCHIAVE2:DIMCHIAVE1);
			tempk=(!nmb)?chiave3:((nmb==1)?chiave2:chiave1);
		}
		head=0;
		for(int step=0;step<DIMMESSAGGIO;step++){ 
			messaggio[step]+=tempk[head];
			if(!head)
				shuffle_key(tempk,tempdim);
			head=(head+1)%tempdim;
		}
	}


	// sezione salvataggio messaggio finale
	if(mode=='D'){
		mescola_mess();// ... altrimenti smescolo dopo
		cout<<COLORE<<"\nMessaggio: "<<NORMALE;
		cout<<messaggio<<endl;
		cout<<COLORE<<"\nSalvare il Messaggio nello Slot "<<NORMALE<<dafile<<COLORE<<"? (Y/N)\n"<<NORMALE;
		cout<<SAVE;
		while(1){
			cin>>choose;
			choose=mask(choose);
			if(choose=='Y' || choose=='S'){
				break;
			}
			if(choose=='N'){
				cout<<SU<<COLORE<<"Messaggio NON Salvato\n"<<NORMALE;
				system("pause");
				return 0;
			}
			comando_sbagliato(choose);

		}
	}
	if(!dafile && mode=='C'){ //se dafile è 0 a questo punto vuol dire che il messaggio e' scritto a mano e non
				 // e' stato salvato
		cout<<COLORE<<"Dove Salvare il Crittogramma? [1-9]\n"<<NORMALE; // va salvato per forza
				cout<<SAVE;
				while(1){
					cin>>choose;
					int chnum=choose-48;
					if(chnum>0 && chnum <=9){
						dafile=choose;
						break;
					}
					comando_sbagliato(choose);
				}		
	}
		char*destinazione=crea_file_path(main_folder,(mode=='C')?critfolder:messfolder,(mode=='C')?pathcrit:pathmess,dafile);
		FILE* outfile=fopen(destinazione,"wb");
		if(!outfile){
			cerr<<ERRORE<<"!!!!!! Open fallita !!!!!!\n"<<NORMALE;
			system("pause");
			return 0;
		}	
		fwrite(messaggio,1,DIMMESSAGGIO,outfile);
		if(mode=='C')//non ha senso stampare il messaggio cifrato
			cout<<COLORE<<pathcrit<<" Salvato in: "<<NORMALE<<destinazione<<endl<<endl;
		else
			cout<<SU<<COLORE<<pathmess<<" Salvato nello Slot: "<<NORMALE<<dafile<<endl<<endl;
		fclose(outfile);
		delete[] destinazione;
		system("pause"); // do il tempo di leggere
	return 0;
}
