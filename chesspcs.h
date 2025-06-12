#ifndef CHESSPCS_H
#define CHESSPCS_H
#include "chessp.h"
//nouveau header : sidechess.c & mainchess.c n'ont pas besoin des fonctions de movchess.C

//movchess.c
void tkdebug(cell **board,int blen);
void takeover(cell **chessboard,int blen,crd lpcpos,void (*dwset)(cell**,int,crd,int));

void atkset(cell **board,int blen,crd pos,int pcid);
void atkdecoy(cell**,int,crd,int);

#endif