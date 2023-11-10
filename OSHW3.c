#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define DRAMMAX 20
#define PFMAX 40
#define MAXPID 9
#define MAXREQ 5

char frame[DRAMMAX+1];
char swapFile[PFMAX +1];
char swapmask[PFMAX + 1];
char memmask[DRAMMAX + 1];
char msg[100];

void show();
int memfree();
void demandnew();
void demandfromswap();
void freeframe();
void freeswap();
int findemptyframe();
void swapout(int which);
int choosedvictim();
int findemptorevict();


int findemptyorvict();

void freeframe(){
    int which = (int) (DRAMMAX * (rand() / (RAND_MAX + 1.0))) + 1;
    if(frame[which] == '_')
        return;


    printf("PID%d free's frame m = %d\n", frame[which], which);

    memmask[which] = frame[which];
    frame[which] = '_';
    show();
}

void freeswap(){
    int which = (int) (PFMAX * (rand() / (RAND_MAX + 1.0))) + 1;
    if(swapFile[which] == '_')
        return;

    printf("PID%d free's swap m = %d\n", swapFile[which], which);

    swapmask[which] = swapFile[which];
    swapFile[which] = '_';
    show();
}

int memfree(){
    int count = 0;

    for (int i=1; i<=DRAMMAX; i++){
        if (frame[i] == '_'){
            count++;
        }
    }
    return count;
}

void demandfromswap(){
    int pwhere = (int) (DRAMMAX * (rand() / (RAND_MAX + 1.0))) + 1;

    int pid = swapFile[pwhere];

    int dwhere;

    if(pid == '_')
        return;

    char* msg = "PID%d DEMANDS FROM SWAP AT d=%d\n";

    printf(msg, pid, pwhere);

    swapmask[pwhere] = pid;

    // swap in
    dwhere = findemptyorvict();
    memmask[dwhere] = pid;
    show();
}

void demandnew(){

    int pid = (int) (MAXPID * (rand() / (RAND_MAX + 1.0))) + 1;

    int amt = (int) (MAXREQ * (rand() / (RAND_MAX + 1.0))) + 1;

    char* msg = "PID%d demands %d frames from swap\n";

    printf(msg, pid + '0', amt);

    for(int j = 1; j <= amt; j++){

        int where = (int) (PFMAX * (rand() / (RAND_MAX + 1.0))) + 1;

        frame[where] = pid;
    }

    show();
}

int findemptyorvict() {
    int k;
    int dwhere;

    for(k = 1; k <= DRAMMAX; k++){
        if(frame[k] == '_'){
            return k;
        }
    }

    dwhere = choosedvictim();
    swapout(dwhere);
    return dwhere;
}

void swapout(int dwhere){
    int k, foundfree = 0, pwhere;

    if(!dwhere){
        return;
    }

    for(k = 1; k <= PFMAX; k++){
        if(swapFile[k] == '_') {
            pwhere = k;
            swapmask[pwhere] = frame[pwhere];
            swapFile[pwhere] = frame[dwhere];
            frame[dwhere] = '_';
            foundfree = 1;
            break;
        }
    }

    if(rand() < RAND_MAX * .8){
        printf("PID%d SWAPOUT CLEAN ", swapFile[pwhere], pwhere);
    }
    else {
        printf("PID%d SWAPOUT DIRTY ", swapFile[pwhere], pwhere);
    }
    show();

    if(!foundfree){
        printf("SWAP FILE FULL\n");
        return;
    }
}

int choosedvictim(){
    int k;

    for(int k = 0; k <= DRAMMAX; k++){
        if(frame[k] != '_'){
            return k;
        }
    }
    return 0;
}

void show(){
    int i, k;

    printf("     ");

    for (i = 1; i <= DRAMMAX; i++){

        if(memmask[i]){
            printf("*");
        }
        else {
            printf(" ");
        }

    }

    printf("\n DRAM: ");

    for (i = 1; i <= DRAMMAX; i++){
        printf("%c", frame[i]);
    }

    printf("\n DISK: ");

    for (i = 1; i <= PFMAX; i++){
        printf("%c", swapFile[i]);
    }

    for (i = 1; i <= PFMAX; i++){
        if(swapmask[i]){
            printf("*");
        }
        else {
            printf(" ");
        }
    }

    printf("\n");
    printf(msg);
    printf("\n");

    // for k in swapmask delete swapmask[k]
    memset(swapmask, 0, sizeof(swapmask));
    memset(memmask, 0, sizeof(memmask));

    sleep(msg[0] == 'S' && msg[1] == 'C' ? 4 : 1);
    
}

int main() {

    int i, e, pid, pwhere, dwhich, which, j;

    srand(time(NULL));
    for(i = 1; i < DRAMMAX; i++){
        frame[i] =  '_';
    }

    for(i = 1; i < PFMAX; i++){
        swapFile[i] =  '_';
    }

    for(e = 1; e < 10000; e++){

        /*
        if (rand() < RAND_MAX * .5) demandnew();
        if (rand() < RAND_MAX * .5) demandfromswap();
        if (rand() < RAND_MAX * .5) demandfromswap();
        if (rand() < RAND_MAX * .5) demandfromswap();
        if (rand() < RAND_MAX * .5) demandfromswap();
        if (rand() < RAND_MAX * .5) demandfromswap();
        if (rand() < RAND_MAX * .5) demandfromswap();
        if (rand() < RAND_MAX * .8) freeframe();
        if (rand() < RAND_MAX * .8) freeframe();
        if (rand() < RAND_MAX * .8) freeframe();
        if (rand() < RAND_MAX * .8) freeswap();
        if (rand() < RAND_MAX * .8) freeswap();
        if (rand() < RAND_MAX * .8) freeswap();
        if (rand() < RAND_MAX * .8) freeswap();
        if (rand() < RAND_MAX * .8) freeswap();

        if (memfree() < .5*DRAMMAX) if (rand() < RAND_MAX * .8) swapout(choosedvictim());
        if (memfree() < .5*DRAMMAX) if (rand() < RAND_MAX * .8) swapout(choosedvictim());
        if (memfree() < .5*DRAMMAX) if (rand() < RAND_MAX * .8) swapout(choosedvictim());
        if (memfree() < .5*DRAMMAX) if (rand() < RAND_MAX * .8) swapout(choosedvictim());
        if (memfree() < .5*DRAMMAX) if (rand() < RAND_MAX * .8) swapout(choosedvictim());
         */




    }

    printf("Hello world");

    return 0;
}
