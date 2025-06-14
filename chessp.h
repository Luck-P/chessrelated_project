#ifndef CHESSP_H
#define CHESSP_H

typedef struct{
    char display;   //ce qui apparaitra à l'écran : quelle pièce si pièce il y a
    int lord;       //le joueur possédant la case : 0 - neutre ; 1 - joueur 1 ; 2 - joueur 2
    int atkr[2][5];      //pièces attaquant la case 
    //tableau de 2 lignes ((0,0,0,0,0),(0,0,0,0,0)) : référence les pièces noires & blanches attaquant respectivement la case
    //nous switchons en stack car cell est toujours défini sous forme de pointer
}cell;              //type de variable pour chaque case de l'échiquier

typedef struct{     //struct définissant la composition d'une variable de type "main" (pièces disponibles du joueur)
    char *hlist;    //liste de toutes les pièces du joueur (forme [p,p,p,p,p,p,p,p,r,r,n,n,b,b,q,k])
    int *len;        //taille de la main (variable technique utile pour la gestion des pièces disponibles)
}hnd;       //hnd = hand mais nous aurons probablement envie d'utiliser "hand" pour nos noms de variable dans nos fonctions -> pas d'imprécision / de redondance 

typedef struct{ //struct de 2 int pour les coordonnées
    int x;
    int y;
}crd;


//mainchess.c
void flusher();
void fullfree(cell**,int,hnd[2]);


//conqchess.c
void conq1copy();
int cqpcheck(hnd crhand,cell**,int,int);
crd conqcheckpos(cell **cboard,int blen,crd pos,char pc,int crpl);
int conqover(hnd *hands);

//conchess.c
void con1copy();
int cnpcheck(hnd crhand,cell **board,int blen,int crpl);
crd concheckpos(cell **cboard,int blen,crd pos,char pc,int crpl);
int conover(hnd *hands);
void atkrdispdebug(cell**,int);

//sidechess.c       -> accessible à conqchess.c & conchess.c 
cell **makeboard(int side,int mode);               
void display(cell **chessboard,int dim);
hnd defhand();

void pop(hnd hand,int eleind);
crd getpos(int blen);
int turnaction(int crpl,int mode,cell **board,int len,hnd hands[2]);
int atkindex(char pc);
void display_v2(cell **board,int dim);



#endif