#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cipher.h"
#define DIM_BUF 200
#define DIM_CHIAVE 50
#define NUM_CIFRARI 3
int main(){
    char buffer[DIM_BUF];
    char chiave[DIM_CHIAVE];
    int choice=0;
    char mode='C';
    bool result=false;
    printf("Selezionare il cifrario: (0 per uscire)\n");
    printf("1) Cifrario di Cesare -- la chiave deve essere un numero da 1 a 25\n");
    printf("2) Cifrario affine -- la chiave sono 2 numeri \"a\" (1-25 COPRIMO CON 26) e \"b\" (0-25), separati da uno SPAZIO\n");
    printf("3) Cifrario completo -- la chiave e' una stringa di caratteri convertita in una permutazione\n");
    while(true){
        printf("Scelta: ");
        fgets(buffer,DIM_BUF,stdin);
        choice=atoi(buffer);
        if(!choice)
            return 0;
        if(choice > NUM_CIFRARI)
            printf("Selezionare un numero nell'intervallo 1-%d!\n",NUM_CIFRARI);
        else
            break;
    }
    printf("Si vuole cifrare o decifrare? (C/D) (DEFAULT: Cifrare)\n");
    fgets(buffer,DIM_BUF,stdin);
    
    mode=buffer[0];
    mode&=0xDF;
    printf("Hai selezionato ");
    if(mode!='D'){
        mode='C';
        printf("Cifratura.\n");
    } else
        printf("Decifratura.\n");

    printf("Inserire il messaggio\nFrase: ");
    fgets(buffer,DIM_BUF,stdin);
    printf("Inserire la chiave\nChiave: ");
    fgets(chiave,DIM_CHIAVE,stdin);
    switch(choice){
        case 1: //cifrario di cesare
            result=cifra_cesare(buffer,atoi(chiave),mode);
        break;

        case 2: //cifrario affine
            result=cifra_affine(buffer,atoi(chiave),atoi(strchr(chiave,' ')+1),mode);
        break;

        case 3: //cifrario completo
            result=cifra_completo(buffer,chiave,mode);
        break;

        default:
        break;
    }
    if(result)
        printf("Operazione completata.\nMessaggio: %s\n",buffer);
    else
        printf("Operazione fallita. Verifica gli input\n");
    return 0;
}