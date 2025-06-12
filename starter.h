#ifndef STARTER_H
#define STARTER_H
#include "chessp.h"

typedef struct{
    int mode;
    int len;
    int pltp;
    cell **chessboard;
    hnd hands[2];
}dcphr;

//mainchess.c 
void cipher(int,int,int,cell**,hnd*);
dcphr decipher(char*);
//archchess.c
void setup(int,int);


#endif
