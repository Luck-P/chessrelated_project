#include <stdio.h>
#include <stdlib.h>
#include "chessp.h"
#include "chesspcs.h"

//fichier "miroir" de conqchess.c, mais cette fois ci avec les fonctions du mode connecte

int kisin(hnd hand);

void con1copy(){
    printf("conchess.c is called");
}

int cnpcheck(hnd crhand,cell **board,int blen,int crpl){ //lorsque l'utilisateur choisit une pièce, il faut déjà vérifier qu'il ait le droit 
                //solution : vérifier qu'il existe au moins 1 case compatible avec la pièce choisie
    char chpc; //chosen piece    
    printf("choississez une piece : ");
    if(scanf(" %c",&chpc)!=1){flusher();}   //nettoie le buffer si l'entrée n'est pas un char
    if(chpc == 'e'){return -1;}
    for(int i = 0;i<crhand.len[0];i++){
        if(crhand.hlist[i] == chpc){        //si on trouve une pièce valide dans la main ...
            //printf("\ndebug 1 : %c in list\n",chpc);
            int h = atkindex(chpc);         //h = hiérarchie 
            for(int x = 0;x<blen;x++){      //on parcourt chaque case de l'échiquier
                for(int y=0;y<blen;y++){
                    if(chpc == 'p' || (board[x][y].display == ' ' && board[x][y].atkr[crpl][h-1] && board[x][y].lord == crpl+1)){  //si la case parcourue est attaquée par la pièce précédente hiérarchiquement  
                        //printf("\ndebug 2 : returning\ncell : (%d,%d)\nnatkr : %d\nchpc : %c\n",x,y,board[x][y].atkr[crpl][h-1],chpc);
                        return i;               //on renvoie l'indice de la pièce dans hlist 
                    }/*else{  //debug process (to dispose of)
                        printf("\ndebug 2.1 :\nx - y : %d - %d \n h-1 = %d\natkr = %d\n",x,y,h-1,board[x][y].atkr[crpl][h-1]);
                    }*/
                }
            }
        }
    }
    return cnpcheck(crhand,board,blen,crpl); //si l'une des conditions n'est pas respectée, récursivité 
}


crd concheckpos(cell **cboard,int blen,crd pos,char pc,int crpl){ 
    pos.x --;pos.y--;   //conversion coord visuelles en coord compiler 
    int i = atkindex(pc);                                                                                                  //crpl + 1 : cpu to graphic
    return (cboard[pos.x][pos.y].display == ' ' && (pc=='p' || (cboard[pos.x][pos.y].atkr[crpl][i-1] == 1 && cboard[pos.x][pos.y].lord == crpl+1))) ? pos:concheckpos(cboard,blen,getpos(blen),pc,crpl);
            //si la case est vide et que soit pc = pion, soit la case est attaquée par la pièce hiérarchiquement précedente : on return pos qui est valide 
}

int conover(hnd *hands){
    //printf("\nkisin p1 : %d\nkisin p2 : %d\nconover : %d",kisin(hands[0]),kisin(hands[1]),(kisin(hands[0]) && kisin(hands[1])) ? 0:1);
    return (kisin(hands[0]) && kisin(hands[1])) ? 0:1;
}

int kisin(hnd hand){    //petite sous-fonction facilitant un peu conover()
    for(int i=0;i<hand.len[0];i++){
        if(hand.hlist[i]== 'k'){
            return 1;
        }
    }
    return 0;
}