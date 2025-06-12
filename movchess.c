#include <stdio.h>
#include <stdlib.h>
#include "chessp.h"
#include "chesspcs.h"


//importance majeur de l'échiquier en pointer : actualisation globale automatique des cases (atroce sans cette fonctionnalité)

void pawn(cell **chessboard,int blen,crd pos,void (*dwset)(cell**,int,crd,int));
void rook(cell **cboard,int blen,crd pos,int own,void (*dwset)(cell**,int,crd,int));
void bishop(cell **cboard,int blen,crd pos,int own,void (*dwset)(cell**,int,crd,int));
void knight(cell **cboard,int blen,crd pos,void (*dwset)(cell**,int,crd,int));
void queen(cell **cboard,int blen,crd pos,void (*dwset)(cell**,int,crd,int));
void king(cell **cboard,int blen,crd pos);

void atkset(cell**,int,crd,int);



void takeover(cell **chessboard,int blen,crd lpcpos,void (*dwset)(cell**,int,crd,int)){ //lpcpos = position de la dernière pièce 

    /*void (*pcbeha)(cell**,int,int,crd);  //pcbeha = comportement de la pièce : une fonction par pièce car ayant des déplacements uniques 
                                        // on peut tout de même repomper les vérifications classiques (toute une diagonale / toute une ligne etc)
                                        // -> mauvaise idée : poser queen() = bishop();rook(); (seul "repompage" possible)
    pcbeha = chessboard[lpcpos.x][lpcpos.y].display;    //.display donne la pièce en question (p, r, n, b, q, k) : nous nommons nos fonctions en conséquence
    */  //no automated linking like in python : 'p' != p() | only solution : big switch 
    switch(chessboard[lpcpos.x][lpcpos.y].display){

        case 'p':   pawn(chessboard,blen,lpcpos,dwset);break; 
                                        //joueur actuel est donnée par .lord de la case lpcpos : nous passons la valeur graphique (1 ou 2) car c'est celle qui est ensuite passée en .lord 

        case 'r':   rook(chessboard,blen,lpcpos,3,dwset);break;   //3 for piece's id in hierarchy

        case 'n':   knight(chessboard,blen,lpcpos,dwset);break;

        case 'b':   bishop(chessboard,blen,lpcpos,2,dwset);break; //same as rook

        case 'q':   queen(chessboard,blen,lpcpos,dwset);break;

        case 'k':   king(chessboard,blen,lpcpos);break;
    }       

    //tkdebug(chessboard,blen);
}


//fonctions des pièces 

void pawn(cell **chessboard,int blen,crd pos,void (*dwset)(cell**,int,crd,int)){      
    int lr[2]= {-1,1};  //lr : gauche - droite : pour pouvoir "inverser" la direction (le pion est la seule pièce à changer de comportement en fonction du camp)
                        //stack plutôt que heap : cette liste ne sert qu'une fois dans la fonction et n'a pas besoin d'être dynamique

    int ud; //ud : haut - bas
    switch(chessboard[pos.x][pos.y].lord -1){     //- 1 : conversion valeur visuelle (1/2) -> valeur compiler (0/1)
        case 0 /*joueur 1*/:{
            ud = 1;             //les pions du joueur 1 attaquent "vers le haut" (ud positif)
            break;
        }
        case 1 /*joueur 2*/:{
            ud = -1;            //les pions du joueur 2 attaquent "vers le bas" (ud négatif)
            break;
        } 
    }
    
    //printf("\npawn() debug : ud = %d\n",ud);

    for(int i=0;i<2;i++){       //pour parcourir la liste lr (gauche / droite) -> on vérifie ainsi la gauche puis la droite de la pièce
        if(
            pos.x+(1*lr[i])<blen && pos.x+(1*lr[i])>=0  && //vérification pour la colonne : ne pas dépasser les dimensions du tableau
            pos.y+(1*ud)<blen &&    pos.y+(1*ud)>=0     && //idem pour la ligne : ne pas dépasser sur le bord 
            chessboard[pos.x + (1*lr[i])][pos.y + (1*ud)].display == ' '
        ){    //lr = -1 puis = 1 : pos.x + 1*(-1) -> colonne de gauche
            chessboard[pos.x + (1*lr[i])][pos.y + (1*ud)].lord = chessboard[pos.x][pos.y].lord; 
                        //x = colonne / y = ligne
            dwset(chessboard,chessboard[pos.x][pos.y].lord - 1,(crd){pos.x + (1*lr[i]),pos.y + (1*ud)},0);
                                    // -1 : pos visuelle to cpu (1/2 -> 0/1)
        }   

    }
    
} //pawn ok 

void rook(cell **cboard,int blen,crd pos,int own,void (*dwset)(cell**,int,crd,int)){
    int lrud[2] = {-1,1};   //cf pawn() : gauche / droite / haut / bas
                            //inutile de créer un coef par direction : le même pour gauche & bas / droite & haut  

    for(int dir=0;dir<2;dir++){
        int i=1,j=1;

        while(      //boucle pour la ligne -> hauteur y (ligne) fixe / abscisse x (colonne) variable  
            pos.x + (i*lrud[dir])<blen && pos.x + (i*lrud[dir])>=0 &&   //tant que x-i >= 0 et x+i < len  : tant que l'on reste dans les dimensions du tableau...
            cboard[pos.x + (i*lrud[dir])][pos.y].display == ' '         //... et que la case suivante est bien vide (une case occupée bloque la conquête et ce peu importe l'affiliation de la pièce)
        ){                                                                          
            cboard[pos.x +(i*lrud[dir])][pos.y].lord = cboard[pos.x][pos.y].lord;
            dwset(cboard,cboard[pos.x][pos.y].lord - 1,(crd){pos.x + (i*lrud[dir]),pos.y},own);
            i++;
        }

        while(      //boucle pour la colonne -> hauteur y (ligne) variable / abscisse x (colonne) fixe
            pos.y + (j*lrud[dir])<blen && pos.y + (j*lrud[dir])>=0 &&   //pareil que pour le premier while 
            cboard[pos.x][pos.y + (j*lrud[dir])].display == ' '
        ){
            cboard[pos.x][pos.y + (j*lrud[dir])].lord = cboard[pos.x][pos.y].lord;
            dwset(cboard,cboard[pos.x][pos.y].lord - 1,(crd){pos.x,pos.y + (j*lrud[dir])},own);
            j++;
        }


    }

}   //rook ok

void bishop(cell **cboard,int blen,crd pos,int own,void (*dwset)(cell**,int,crd,int)){
    int lrud[2] = {-1,1};

    for(int dlr=0;dlr<2;dlr++){     //dlr : "direction gauche droite"
        for(int dud=0;dud<2;dud++){ //dud : direction haut bas" 
                                                //besoin de 2 coefficients car déplacement biparamétrique (droite -> haut ; droite -> bas ; gauche -> haut ; gauche -> bas plutôt que droite ; gauche ; haut ; bas)
            int i=1;

            while(  //boucle vérifiant que la i-ème case bas-gauche / haut-gauche / bas-droite / haut-droite est libre
                pos.x + (i*lrud[dlr]) < blen && pos.x + (i*lrud[dlr]) >=0 && //colonne (x) gauche ou droite bien comprise entre 0 et blen

                pos.y + (i*lrud[dud]) < blen && pos.y + (i*lrud[dud]) >=0 && //ligne (y)  au-dessus/dessous bien comprise entre 0 et blen
                
                cboard[pos.x + (i*lrud[dlr])][pos.y + (i*lrud[dud])].display == ' '
                
            ){

                cboard[pos.x + (i*lrud[dlr])][pos.y + (i*lrud[dud])].lord = cboard[pos.x][pos.y].lord;
                dwset(cboard,cboard[pos.x][pos.y].lord - 1,(crd){pos.x + (i*lrud[dlr]),pos.y + (i*lrud[dud])},own);
                i++;

            }
        }
    }
}   //bishop ok

void knight(cell **cboard,int blen,crd pos,void (*dwset)(cell**,int,crd,int)){
    //déplacements plus compliqués : x+2 / y+1 | x+2 / y-1 | x+1 / y+2 | x+1 / y-2 | x-2 / y+1 | x-2 / y-1 | x-1 / y+2 | x-1 / y-2 
    //le but final est de créer des coefficients suffisamment ingénieux pour limiter le nombre de if(...){...} final 

    int lrud[2] = {-1,1},leap[2] = {1,2}; //leap : les déplacements x +/- 1 -> y +/- 2 & x +/- 2 -> y +/- 1 
    
    for(int dlr=0;dlr<2;dlr++){         //lr : x (colonne)  -> x + ? / x - ? 
        for(int lp=0;lp<2;lp++){        //lp : 1 ou 2       -> x dlr 1 / x dlr 2 avec dlr définissant le signe de l'opération  
            for(int dud=0;dud<2;dud++){ //ud : y (ligne)    -> x dlr 1 => y + 2 / y - 2 | x dlr 2 => y + 1 / y - 1 
                                //ainsi, nous couvrons dans les 2 premières boucles toutes les options de x et à chaque fois, le y + et y - correspondant 

                if(
                    pos.x + (leap[lp]*lrud[dlr]) < blen    && pos.x + (leap[lp]*lrud[dlr]) >= 0   &&  //vérifie si x+1 / x+2 / x-1 / x-2 demeure dans les dimensions
                        //leap[lp] : 1 ou 2
                    pos.y + (leap[1-lp]*lrud[dud]) < blen  && pos.y + (leap[1-lp]*lrud[dud]) >= 0 &&  //idem  avec y+2 / y
                        //leap[1-lp] (la valeur opposée de la liste) : 2 ou 1 
                    cboard[pos.x + (leap[lp]*lrud[dlr])][pos.y + (leap[1-lp]*lrud[dud])].display == ' ' 
                        //on vérifie à chaque fois que la case, si définie, est bien libre
                ){
                    cboard[pos.x + (leap[lp]*lrud[dlr])][pos.y + (leap[1-lp]*lrud[dud])].lord = cboard[pos.x][pos.y].lord;
                    dwset(cboard,cboard[pos.x][pos.y].lord - 1,(crd){pos.x + (leap[lp]*lrud[dlr]),pos.y + (leap[1-lp]*lrud[dud])},1);
                }
            }
        }
    }
}   //knight ok 

void queen(cell **cboard,int blen,crd pos,void (*dwset)(cell**,int,crd,int)){
    rook(cboard,blen,pos,4,dwset);
    bishop(cboard,blen,pos,4,dwset);
    //pour la Dame, on appelle successivement les fonctions "tour" & "fou" qui vont conquérir successivement les lignes / colonnes et les 4 diagonales 

    //mode connecte : implémentation de own pour conquérir les cases au nom de la dame et non du fou / de la tour 
}

void king(cell **cboard,int blen,crd pos){
    int lrudds[3] = {-1,0,1}; //gauche droite haut bas diagonales : modification mineure pour la clarté du code 
    
    for(int dx=0;dx<3;dx++){
        for(int dy=0;dy<3;dy++){
            if(
                pos.x + lrudds[dx] < blen && pos.x + lrudds[dx] >= 0 && //comme d'habitude -> vérifier que nous sommes dans les clous
                pos.y + lrudds[dy] < blen && pos.y + lrudds[dy] >= 0 && 
                cboard[pos.x + lrudds[dx]][pos.y + lrudds[dy]].display == ' ' //vérifier que la case attaquable est bien vide 
            ){
                cboard[pos.x + lrudds[dx]][pos.y + lrudds[dy]].lord = cboard[pos.x][pos.y].lord;
            }
        }    //de cette manière, nous testons pour rien la case x+0 y+0 mais cela nous économise un if de plus, ou du moins plusieurs lignes 
    }
}

void atkset(cell **board,int crpl,crd pos, int pcid){ //administre l'attribut .atkr de cell | pcid = "id de la pièce" pour l'attribut atkr
            //crpl : le joueur qui joue | pos : la position de la case évaluée 
    board[pos.x][pos.y].atkr[crpl][pcid] = 1;   //la case correspondante [joueur][pièce] est mise à 1 (= "une pièce %pcid% du joueur %crpl% attaque la case x y ")
}

void atkdecoy(cell **,int,crd,int){
    //does nothing, as in conqmode 
}

/*void atkres(cell **board,crd pos){  //réinitialise .atkr (à chaque changement de possession)
    for(int i=0;i<5;i++){
        board[pos.x][pos.y].atkr[i] = 0;
    }
} -> on ne réinitialise plus : 2 tableaux désormais (un par joueur)*/

void tkdebug(cell **board,int blen){
    for(int i=0;i<blen;i++){
        printf("\n");
        for(int j=0;j<blen;j++){
            printf("%d",board[j][blen-i-1].lord);
        }
    }
}

void atkrdispdebug(cell **board,int blen){
    printf("\n");
    for(int pl = 0 ; pl<2;pl++){
        for(int ln=0;ln<blen;ln++){
            for(int cl=0;cl<blen;cl++){
                printf("[");
                for(int i=0;i<5;i++){
                    printf("%d,",board[cl][blen-ln-1].atkr[pl][i]);
                }
                printf("],");
            }
            printf("\n");
        }
        printf("\n");
    }
}