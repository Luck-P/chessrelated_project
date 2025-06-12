#include <stdio.h>
#include <stdlib.h>
#include "chessp.h"
#include "chesspcs.h"

crd checkpos(cell **board,int blen,crd pos);

void conq1copy(){
    printf("conqchess is called");
}

/*
void conqsetup(int chornw){ //charger ou nouvelle : souhaite-t-on débuter une nouvelle partie (0) ou en charger une ancienne (1)
    switch(chornw){
        case 0: {   //nouvel échiquier vierge 
            int size;   //taille voulue de l'échiquier
            do {
                printf("dimension de l'echiquier entre 6 & 12 (donnez le cote uniquement) : ");
                if(scanf("%d",&size)!=1){flusher();}    //vérifie que l'input donné est bien un entier
            }while(size<6 || size>12);
            cell **chessboard = makeboard(size);    //définie un nouvel échiquier vierge
            //display(chessboard,size);   //debug 
            hnd *hands = malloc(2*sizeof(hnd));     //heap plutôt que stack : plus simple pour passer les arguments en tant que pointeurs (actualisation + simple)
            hands[0]=defhand();hands[1]=defhand();  // 2 mains complètes définies sous forme de pointers 
            gameplan(chessboard,size,0,hands,0);    //appelle gameplan(...) qui lance la partie à proprement parler
            break;
        }

        case 1: {   //reprendre un échiquier antérieur

        }
    }

}   --> supprimé : retrospectivement inutile, la mise en place est la même pour conqsetup et consetup => mutualisation dans sidechess.c
*/

//possibilité de mutualiser ce code avec mode connecte : juste appeler conqturn() ou conturn() et cqwcheck() / cnwcheck()
/*void conqmode(cell **chessboard,int len,int whpl,hnd *hands){ 
    //chessboard : échiquier | len : sa taille | whpl ("qui joue ?") : reprendre une partie : qui reprend ? (0/1) | *hands : les 2 mains dans un pointeurs (actualisation + facile) 
    int end;    //0 -> ça joue | 1 -> ça ne joue plus
    while(end==0){                  //boucle while lançant des boucles for de 2 itérations (j1 puis j2) jusqu'à la fin de la partie
                                    //while inutile en mode conquête 
        for(int player = 0;player<2;player++){
            if(whpl == 1){whpl=0;continue;}//si le joueur 2 reprend : on passe un tour 
            turn(chessboard,len,player,hands[player]);
        }
    }
}*/

/*void conqturn(cell **cboard,int len,int crpl,hnd cphand){// crpl : current player (joueur actuel) 

    //display(cboard,len);

    //printf("\nAu joueur %d de jouer\nVos pieces disponibles : ",crpl+1); //crpl+1 : 0 -> joueur 1 | 1 -> joueur 2
    printf("Vos pieces disponibles : ");
    for(int i=0;i<cphand.len[0];i++){   //.len[0] since it's a pointer (remember /!\)
        printf("%c ",cphand.hlist[i]);
    }printf("\n");

    //selection de la pièce à jouer

    int eleid = chpccheck(cphand);      //obtention de la position de la pièce choisie dans la liste de pièces disponibles 
    char chpc = cphand.hlist[eleid];    //chpc = pièce choisie : le eleid-ème élément de cphand.hlist
    pop(cphand,eleid);
    
    //placement de la pièce choisie sur l'échiquier  

    crd pos = checkpos(cboard,len,getpos(len));    
    printf("pos : %d %d",pos.x,pos.y);

    cboard[pos.x][pos.y].display = chpc;
    cboard[pos.x][pos.y].lord = crpl+1; //joueur 1 : crpl = 0 / lord = 1 | joueur 2 : crpl = 1 / lord = 2


    //conquête des cases menacées par la pièce posée (vif du sujet)

    takeover(cboard,len,pos);          

    getchar();
}               --> supprimée car mutualisation possible : juste appeler le checkpos qui va bien ainsi que conqover 
    */
int cqpcheck(hnd crhand,cell **n1,int n2,int n3){      //chosen-piece check : vérifie si l'input est bien un char puis s'il est bien inclu dans la "main" (liste de pièce disponible)
                                                //comme dans conqcheckpos() : nous passons des variables inutiles, normalement sans incidence sur la cpu
                                                //certes moins propre, nous obtenons en contrepartie une fonction "tour de jeu" très générique et sans redondance
    char chpc; //chosen piece    
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
    //printf("\ndebug : conqover() called\nlen hand player 2 : %d",hands[1].len[0]);
    if(hands[1].len[0] == 0 && hands[0].len[0] == 0){ //on ne vérifie que hands[1] vu que le joueur 2 jouera toujours en dernier -> non
        printf("game ended");
        return 1;
    }
    return 0;
}
                
//01062025-0256pm : conqchess fonctionne, une partie entière est possible 