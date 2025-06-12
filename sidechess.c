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



/*void setup(int chornw,int mode){    //chornw : "charger ou nouvelle" -> 1/0 reprendre une partie ou en commencer une autre 
                                    //setup commun aux 2 modes de jeu : passe uniquement le mode (0 : conquête | 1 : connecte )
    switch(chornw){
        case 0:{    //nouvelle partie
            int size;   //taille voulue de l'échiquier
            do {
                printf("dimension de l'echiquier entre 6 & 12 (donnez le cote uniquement) : ");
                if(scanf("%d",&size)!=1){flusher();}    //vérifie que l'input donné est bien un entier
            }while(size<6 || size>12);
            //cell **chessboard = makeboard(size);    //définie un nouvel échiquier vierge
            //display(chessboard,size);   //debug 
            hnd *hands = malloc(2*sizeof(hnd));     //heap plutôt que stack : plus simple pour passer les arguments en tant que pointeurs (actualisation + simple)
            hands[0]=defhand();hands[1]=defhand();  // 2 mains complètes définies sous forme de pointers 
            gameplan(makeboard(size),size,0,hands,mode);    //appelle gameplan(...) qui lance la partie à proprement parler
        }
    }
    
}   --> fonction déplacée dans archchess.c 
*/





cell **makeboard(int side,int mode){ //échiquier carré de taille side x side 
//les cases devront avoir plusieurs propriétés : qui la possède ? qu'est-ce qui est affiché 
    int locmode=mode;   //variable locale pour limiter les tests processeurs 
    cell **chessboard = malloc(side * sizeof(cell *));
    for(int i = 0;i<side;i++){
        chessboard[i] = malloc(side * sizeof(cell));
    }
    for(int i=0;i<side;i++){                //attribution des valeurs de base des cases : 
        for(int j=0;j<side;j++){                
            chessboard[i][j].display= ' ';  //affichage vide 
            chessboard[i][j].lord = 0;      //case initialement neutre
            if(locmode){
                //chessboard[i][j].atkr = malloc(2*sizeof(int*));
                for(int atk=0;atk<2;atk++){
                    //chessboard[i][j].atkr[atk] = malloc(5*sizeof(int));
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
//cependant, les 2 joueurs doivent pouvoir différencier leurs pièces respectives 
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
            //printing
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
}

hnd defhand(){//définie la main de chaque joueur au commencement d'une partie | player 1 : pl = 0 / player 2 : pl = 1
    //p = pion ; r = tour ; n = cavalier ; b = fou ; q = dame ; k = roi
    char *hand = malloc(16*sizeof(char));
    for(int i = 0;i<8;i++){
        hand[i] = 'p';
    }
    for(int i = 0;i<2;i++){
        hand[8+i] = 'r';
        hand[10+i] = 'n';
        hand[12+i] = 'b';   
    }
    hand[14] = 'q';hand[15] = 'k';
    hnd fhand; 
    fhand.len = malloc(sizeof(int));
    fhand.hlist = hand;fhand.len[0] = 16;//définir fhand (type hnd) pour passer la liste & sa taille à main() qui rend le tout global (accessible partout)
                                    //.len[0] since POINTER 
                                    //nous noterons que puisque les mains sont définies par *hands[1] et [2] (en heap), le pointer sur la liste et la taille perd de son utilité.
                                    //il s'agit donc ici d'une amélioration possible qui nécessiterait néanmoins quelques ajustement
    
    /*                                    
    fflush(stdout); //flush du buffer de sortie : la ligne suivante ne s'imprime pas autrement pour des raisons mystiques 
    printf("defhand debug :\nlen : %d\nhand : ",fhand.len[0]);
    for(int i=0;i<fhand.len[0];i++){
        printf("%c",fhand.hlist[i]);
    }printf("\n");*/    //debug pour la fonction defhand 
    return fhand;
}

/*void gameplan(cell **chessboard,int len,int whpl,hnd *hands,int mode){ //boucle de jeu principale | varie en fonction du mode lancé
    //chessboard : échiquier | len : sa taille | whpl ("qui joue ?") : reprendre une partie : qui reprend ? (0/1) | *hands : les 2 mains dans un pointeurs (actualisation + facile) 
    int end=0;    //0 -> ça joue | 1 -> ça ne joue plus
    int skat = whpl;    //variable local : if(skat){...} sera optimisé et donc vérifié qu'une seule fois (normalement)

    void (*modeturn)(cell**,int,int,hnd);
    int (*modeover)(hnd*);
    switch(mode){
        case 0: {
            modeturn = conqturn;
            modeover = conqover;
            break;    
        }
        //remplacer la variable générique modeturn par conqturn / conturn pour ne pas avoir à tester %mode% à chaque itération
        //case 1: modeturn = conturn;
    }
    while(end==0){                  //boucle while lançant des boucles for de 2 itérations (j1 puis j2) jusqu'à la fin de la partie
                                    //while inutile en mode conquête mais pratique en mode connecte

        for(int player = 0;player<2;player++){
            if(skat){
                skat=0;
                continue;
            }//si le joueur 2 reprend : on passe un tour 

            display(chessboard,len);

            turnaction(player);//fonction balise : soit éteint le programme / soit le laisse poursuivre normalement 
            
            modeturn(chessboard,len,player,hands[player]); //soit conqturn soit conturn en fonction du %mode% passé initialement  
            
            end = modeover(hands);
        }
        
    }   //end bugué : ne s'arrête pas 
        //problème trouvé : hand.len appelé dans conqover -> est un pointeur | la valeur de len se trouve à .len[0] 
    display(chessboard,len);fflush(stdout); //le buffer n'était pas totalement imprimé que déjà winner() changeait la couleur du terminal : nous marquons donc une pause
    winner(chessboard,len);
} 
    --> déplacée dans archchess.c  
*/

/*int chpccheck(hnd crhand){      //chosen-piece check : vérifie si l'input est bien un char puis s'il est bien inclu dans la "main" (liste de pièce disponible)
    char chpc; //chosen piece    
    printf("choississez une piece : ");
    if(scanf(" %c",&chpc)!=1){flusher();}   //nettoie le buffer si l'entrée n'est pas un char
    for(int i=0;i<crhand.len[0];i++){
        if(chpc == crhand.hlist[i]){   
            return i;                   //si match : renvoie l'indice de la pièce dans la main 
        }
    }
    return chpccheck(crhand);           //sinon, récursivité pour redemander une pièce
}   --> déplacée dans conqchess
*/
void pop(hnd hand,int eleind){  //hand : la main | eleind : indice de l'élement à supprimer 
    //fisher yate's shuffle : pour supprimer un élément de notre liste, on l'intervertit avec le dernier élement puis on décrémente la taille de 1
    //ainsi, la valeur "supprimée" n'est plus accédée tandis que la dernière valeur est conservée car déplacée
    //inconvénient : les pièces se retrouvent en quelques tours totalement en désordre : boah pas si grave 
    char temp = hand.hlist[hand.len[0]-1];
    hand.hlist[hand.len[0]-1] = hand.hlist[eleind];
    hand.hlist[eleind] = temp;
    hand.len[0]-= 1;        //tout l'intérêt d'un pointeur : la taille de la main est actualisée partout et ce sans passement compliqué de variable
}

crd getpos(int blen){  //blen : taille échiquier | cplayer : joueur actuel (0 / 1) | pc : pièce jouée ce tour | mode : 0 = conquête / 1 = connecte
    crd pos;
    printf("entrez coordonnees (colonne - ligne)"); // /!\ stick to coordonates configuration col = x  line = y 
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
    }while(act<1 || act > 3);
    switch(act){
        case 1:return 0;

        case 2:{    //abandonner la partie
            printf("\n\033[31mLe joueur %d abandonne.\033[0m\n",crpl+1);
            //exit(0);
            return 2;
        }

        case 3:{
            cipher(mode,len,crpl,board,hands);
            printf("\nPartie Sauvegardee\n");
            return 2;
        }
    }
}

/*void winner(cell **chessboard,int blen){
    int scores[2] = {0,0},max;  //scores[0] pour le joueur 1 ; scores[1] pour le joueur 2
    for(int x=0;x<blen;x++){
        for(int y=0;y<blen;y++){
            scores[chessboard[x][y].lord - 1]++;
        }
    }
    if(scores[0]<scores[1]){max = 2;}else if(scores[0]>scores[1]){max=1;}
    else{
        printf("\nEgalite ! ");
        exit(0);
    }
    system("start /MIN .\\win.mp3");
    printf("\n\n\033[32mLe joueur %d remporte la partie avec %d cases conquises\033[0m\n\n\n",max,scores[max-1]);
    exit(0);
}       --> déplacée dans archchess.c
*/

int atkindex(char pc){
    switch(pc){
        case 'p':return 0;break;

        case 'n':return 1;break;

        case 'b':return 2;break;

        case 'r':return 3;break;

        case 'q':return 4;break;

        case 'k':return 5;break;
    }
}

void display_v2(cell **board,int dim){

    char bgcolors[3][8] = {"","\033[41m","\033[42m"};

    printf("\n");
    for(int k=0;k<dim;k++){
        printf("+---");
    }
    printf("+\n");
    for(int i=0;i<dim;i++){    //i : line 
        printf("|");
        for(int j=0;j<dim;j++){ //j : column    
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
//nous avons désormais besoin d'une fonction qui bypass une fonction en cours pour relancer une itération de main() dans mainchess.c