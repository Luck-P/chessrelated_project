#include <stdio.h>
#include <stdlib.h>
#include "chessp.h"
#include "chesspcs.h"

crd checkpos(cell **board,int blen,crd pos);

void conq1copy(){
    printf("conqchess is called");
}

int cqpcheck(hnd crhand,cell **n1,int n2,int n3){      //chosen-piece check : vérifie si l'input est bien un char puis s'il est bien inclu dans la "main" (liste de pièce disponible)
                                                //comme dans conqcheckpos() : nous passons des variables inutiles, normalement sans incidence sur la cpu
                                                //certes moins propre, nous obtenons en contrepartie une fonction "tour de jeu" très générique et sans redondance
    char chpc; //chosen piece    -> pièce choisie
    printf("choississez une piece : ");
    if(scanf(" %c",&chpc)!=1){flusher();}   //nettoie le buffer si l'entrée n'est pas un char
    if(chpc == 'e'){return -1;}
    for(int i=0;i<crhand.len[0];i++){
        if(chpc == crhand.hlist[i]){   
            return i;                   //si match : renvoie l'indice de la pièce dans la main 
        }
    }
    return cqpcheck(crhand,n1,n2,n3);           //sinon, récursivité pour redemander une pièce
}



crd conqcheckpos(cell **board,int blen,crd pos,char pc,int crpl){    //vérifie que la case choisie est valide -> en l'occurence seulement si case vide (+complexe en mode connecte)
                                                //pc est passé mais n'est pas utilisée : pas très propre mais sans conséquence en terme de compilation, cet input nous sert pour le mode connecte
                                                //idem pour crpl
    pos.x -=1;pos.y-=1;     //conversion des coord "visuelles" en coord compiler 
    return (board[pos.x][pos.y].display == ' ') ? pos:conqcheckpos(board,blen,getpos(blen),pc,crpl);
}

 //trouver une méthode versatile pour conqover() et conover() (en terme de variable )
 //solution : passer uniquement les mains et vérifier soit len = 0 ; soit k !€ hlist 
int conqover(hnd *hands){
    
    if(hands[1].len[0] == 0 && hands[0].len[0] == 0){ //on ne vérifie que hands[1] vu que le joueur 2 jouera toujours en dernier -> non
        printf("game ended");
        return 1;
    }
    return 0;
}
                
//01062025-0256pm : conqchess fonctionne, une partie entière est possible 