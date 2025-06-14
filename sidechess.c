#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> //needed for toupper() function while defining a hand   
#include "chessp.h"
#include "starter.h"


//mettre ici les fonctions génériques utiles aussi bien en mode conquête que connecte
//pour la clarté du code : bouger setup() , gameplan() et winner() qui sont un peu plus que génériques 


void test1(){
    printf("hello world from side file");
}

cell **makeboard(int size,int mode);
hnd defhand();
int turnaction(int crpl,int mode,cell **board,int len,hnd hands[2]);
void winner(cell**,int);
void gameplan(cell **chessboard,int len,int whpl,hnd *hands,int mode);


cell **makeboard(int side,int mode){ //échiquier carré de taille side x side 
//les cases devront avoir plusieurs propriétés : qui la possède ? qu'est-ce qui est affiché ? quelles pièces l'attaquent ? 
    int locmode=mode;   //variable locale pour limiter les tests processeurs 
    cell **chessboard = malloc(side * sizeof(cell *));
    for(int i = 0;i<side;i++){
        chessboard[i] = malloc(side * sizeof(cell));
    }
    for(int i=0;i<side;i++){                //attribution des valeurs de base des cases : 
        for(int j=0;j<side;j++){                
            chessboard[i][j].display= ' ';  //affichage vide 
            chessboard[i][j].lord = 0;      //case initialement neutre
            if(locmode){            //si nous sommes en mode connecte, atkr[2][5] est rempli par la fonction 
                
                for(int atk=0;atk<2;atk++){
                
                    for(int k=0;k<5;k++){
                        chessboard[i][j].atkr[atk][k] = 0;
                    }
                }    
                    
            } 
        }
    }
        return chessboard;
}


//Au niveau du code, la différenciation entre joueur 1 & joueur 2 s'effectue dans cell.lord 
//cependant, les 2 joueurs doivent pouvoir différencier leurs pièces respectives à l'oeil
//pour ne pas tout péter, cette différenciation purement visuelle s'effectuera au sein de Display
void display(cell **chessboard,int dim){     //affiche l'échiquier dans sa configuration actuelle
    printf("\n");
    for(int k=0;k<dim;k++){
        printf("+---");
    }
    printf("+\n");
    for(int i=0;i<dim;i++){    //i : line 
        printf("|");
        for(int j=0;j<dim;j++){ //j : column    
            //si la case appartient au joueur 1 : on modifie l'affichage (toupper() ) sans éditer le contenu du tableau chessboard de sorte à ce qu'un pion demeure un pion indépendamment de son détenteur (pratique pour la conquête de case plus tard)
            char curchar = (chessboard[j][dim-i-1].lord==1) ? toupper(chessboard[j][dim-i-1].display):chessboard[j][dim-i-1].display ;
            //impression
            printf(" %c |",curchar);   //dim - i : imprime d'abord la ligne max --> ligne 0 (tableau vertical)
        }       //impeccable : ça marche parfaitement 
        printf(" %d\n",dim-i);
        for(int k=0;k<dim;k++){
            printf("+---");
        }printf("+\n"); 
    }
    for(int k=0;k<dim;k++){
        printf("  %d ",k+1);
    }
}   //cette première ébauche de display() n'affichait aucune couleur : il était alors compliqué de reconnaitre ses cases de celles de l'adversaire 

hnd defhand(){//définie la main de chaque joueur au commencement d'une partie | player 1 : pl = 0 / player 2 : pl = 1
    //p = pion ; r = tour ; n = cavalier ; b = fou ; q = dame ; k = roi
    char *hand = malloc(16*sizeof(char));
    for(int i = 0;i<8;i++){     //par la suite nous évitons d'écrire 16 hand[] = ...
        hand[i] = 'p';          
    }
    for(int i = 0;i<2;i++){     //petite méthode pour limiter le nombre de hand[] = ...
        hand[8+i] = 'r';
        hand[10+i] = 'n';
        hand[12+i] = 'b';   
    }
    hand[14] = 'q';hand[15] = 'k';
    hnd fhand; 
    fhand.len = malloc(sizeof(int));
    fhand.hlist = hand;fhand.len[0] = 16;//définir fhand (type hnd) pour passer la liste & sa taille à main() qui rend le tout global (accessible partout)
                                    //.len[0] car c'est un pointeur 
    return fhand;
}

void pop(hnd hand,int eleind){  //hand : la main | eleind : indice de l'élement à supprimer 
    //inspiré du fisher yate's shuffle : pour supprimer un élément de notre liste, on l'intervertit avec le dernier élement puis on décrémente la taille de 1
    //ainsi, la valeur "supprimée" n'est plus accédée tandis que la dernière valeur est conservée car déplacée
    //inconvénient : les pièces se retrouvent en quelques tours totalement en désordre : boah pas si grave 
    char temp = hand.hlist[hand.len[0]-1];
    hand.hlist[hand.len[0]-1] = hand.hlist[eleind]; 
    hand.hlist[eleind] = temp;
    hand.len[0]-= 1;        //tout l'intérêt d'un pointeur : la taille de la main est actualisée partout et ce sans passement compliqué de variable
}

crd getpos(int blen){  //blen : taille échiquier | cplayer : joueur actuel (0 / 1) | pc : pièce jouée ce tour | mode : 0 = conquête / 1 = connecte
    crd pos;
    printf("entrez coordonnees (colonne - ligne)"); // /!\ configuration des coordonnées col = x  line = y 
    printf("\ncolonne : ");
    if(scanf("%d",&pos.x)!=1){flusher();}
    printf("ligne : ");
    if(scanf("%d",&pos.y)!=1){flusher();}
    return (pos.x>=1 && pos.x<=blen && pos.y>=1 && pos.y<=blen) ? pos:getpos(blen);
}   // /!\ ne vérifie QUE la validité spatiale des coordonnées, pas si le coup est valide | renvoie coordonnées visuelles (1 -> max) : convertir en 0 -> max-1

int turnaction(int crpl,int mode,cell **board,int len,hnd hands[2]){   //demande simplement au joueur ce qu'il souhaite faire | 1. la fonction laisse passer | 2. la fonction arrête le programme | 3. la fonction enregistre la partie puis arrête le programme
    int act;
    do{
        printf("\nAu joueur %d de jouer\n1. Poser une piece\t2.Abandonner\t3.Sauvegarder et quitter\n\t\t\t",crpl+1);
        if(scanf("%d",&act)!=1){flusher();}
    }while(act<1 || act > 3);       //acquisition basique
    switch(act){
        case 1:return 0;    //si le joueur souhaite continuer, la fonction ne s'interpose pas (end demeure à 0)

        case 2:{    //abandonner la partie
            printf("\n\033[31mLe joueur %d abandonne.\033[0m\n",crpl+1);
            return 2;           //2 est un code de sortie pour la variable end afin de stopper prématurément une partie 
        }

        case 3:{
            cipher(mode,len,crpl,board,hands);  //si la demande est donnée, on appelle cipher() pour la sauvegarde puis nous quittons 
            printf("\nPartie Sauvegardee\n");
            return 2;
        }
    }
}


int atkindex(char pc){          //renvoie l'index hiérarchique d'une pièce donnée
    switch(pc){
        case 'p':return 0;break;

        case 'n':return 1;break;

        case 'b':return 2;break;

        case 'r':return 3;break;

        case 'q':return 4;break;

        case 'k':return 5;break;
    }
}

void display_v2(cell **board,int dim){      //reprend intégralement display() mais cette fois ci en ajoutant les couleurs en fonction de lord 

    char bgcolors[3][8] = {"","\033[41m","\033[42m"};   //tableau de nos balises ANSI propre à chaque couleur 

    printf("\n");
    for(int k=0;k<dim;k++){
        printf("+---");
    }
    printf("+\n");
    for(int i=0;i<dim;i++){    //i : ligne
        printf("|");
        for(int j=0;j<dim;j++){ //j : colonne    
            //si la case appartient au joueur 1 : on modifie l'affichage (toupper() ) sans éditer le contenu du tableau chessboard de sorte à ce qu'un pion demeure un pion indépendamment de son détenteur (pratique pour la conquête de case plus tard)
            char curchar = (board[j][dim-i-1].lord==1) ? toupper(board[j][dim-i-1].display):board[j][dim-i-1].display ;

            //printing
            printf("%s %c %s|",bgcolors[board[j][dim-i-1].lord],curchar,"\033[0m");   //dim - i : imprime d'abord la ligne max --> ligne 0 (tableau vertical)
                //escape code ANSI : nous mettons les cases conquises par le joueur 1 en rouge & celles du joueur 2 en vert

        }       //impeccable : ça marche parfaitement 
        printf(" %d\n",dim-i);
        for(int k=0;k<dim;k++){
            printf("+---");
        }printf("+\n"); 
    }
    for(int k=0;k<dim;k++){
        printf("  %d ",k+1);
    }
}

//Finalisation Programme 
