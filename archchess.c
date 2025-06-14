#include <stdio.h>
#include <stdlib.h>
#include "chessp.h"
#include "chesspcs.h"
#include "starter.h"

//nouveau fichier contenant les fonctions clés du programme (déplacées pour plus de clarté dans le code s)

void gameplan(cell**,int,int,hnd*,int);
void gameturn(cell**,int,int,hnd,crd (*)(cell**,int,crd,char,int),int (*)(hnd,cell**,int,int),void (*)(cell**,int,crd,int));
void winner(cell**,int);

void setup(int chornw,int mode){    //chornw : "charger ou nouvelle" -> 1/0 reprendre une partie ou en commencer une autre 
                                    //setup commun aux 2 modes de jeu : passe uniquement le mode (0 : conquête | 1 : connecte )
    switch(chornw){
        case 0:{    //nouvelle partie
            int size;   //taille voulue de l'échiquier
            do {
                printf("dimension de l'echiquier entre 6 & 12 (donnez le cote uniquement) : ");
                if(scanf("%d",&size)!=1){flusher();}    //vérifie que l'input donné est bien un entier
            }while(size<6 || size>12);
            
            hnd hands[2];                           //stack plutôt que heap car les attributs du struct hnd sont déjà des pointeurs
            hands[0]=defhand();hands[1]=defhand();  // 2 mains complètes définies sous forme de pointers 
            gameplan(makeboard(size,mode),size,0,hands,mode);    //appelle gameplan(...) qui lance la partie à proprement parler
            break;
        }

        case 1:{
            dcphr gbo = decipher(".\\gamesave.csv");    //nous récupérons toutes nos variables de jeu 
            gameplan(gbo.chessboard,gbo.len,gbo.pltp,gbo.hands,gbo.mode);   //on relance gameplan() avec ces variables récupérées
            break;
        }
    }
}

void gameplan(cell **chessboard,int len,int whpl,hnd *hands,int mode){ //boucle de jeu principale | varie en fonction du mode lancé
    //chessboard : échiquier | len : sa taille | whpl ("qui joue ?") : reprendre une partie : qui reprend ? (0/1) | *hands : les 2 mains dans un pointeurs (actualisation + facile) 
    int end=0;    //0 -> ça joue | 1 -> ça ne joue plus | 2 -> force stop , on relance une partie
    int skat = whpl;    //variable local : if(skat){...} sera optimisé et donc vérifié qu'une seule fois (normalement)

    //void (*modeturn)(cell**,int,int,hnd);
    int (*modeover)(hnd*);
    crd (*checkpos)(cell**,int,crd,char,int);   
    int (*cpcheck)(hnd,cell**,int,int);
    void (*modeatk)(cell**,int,crd,int);    
    switch(mode){
        case 0: {
            modeover = conqover;
            cpcheck = cqpcheck;
            checkpos = conqcheckpos;
            modeatk = atkdecoy;
            break;    
        }
        //remplacer les fonctions génériques par les fonctions du mode conquête / connecte pour éviter les duplications de code
        case 1: {
            modeover = conover;
            cpcheck = cnpcheck;
            checkpos = concheckpos; 
            modeatk = atkset;
            break;
        }
    }


    while(end==0){                  //boucle while lançant des boucles for de 2 itérations (j1 puis j2) jusqu'à la fin de la partie
                                    //while inutile en mode conquête mais pratique en mode connecte

        for(int player = 0;player<2 && end==0;player++){
            
            if(skat){       //skat étant la version locale de whpl, le test est optimisé par le compilateur
                skat=0;
                continue;
            }//si le joueur 2 reprend : on passe le premier tour du joueur 1 

        
            display_v2(chessboard,len);     //à chaque tour nous affichons l'échiquier 

            end = turnaction(player,mode,chessboard,len,hands);//fonction balise : soit éteint le programme / soit le laisse poursuivre normalement 
            if(end==2){fullfree(chessboard,len,hands);return;}

            gameturn(chessboard,len,player,hands[player],checkpos,cpcheck,modeatk); //soit conqturn soit conturn en fonction du %mode% passé initialement  
            
            end = modeover(hands);  //modeover() pour vérifier si la partie est finie
        }
        
    }  
    display_v2(chessboard,len);fflush(stdout); //le buffer n'était pas totalement imprimé que déjà winner() changeait la couleur du terminal : nous marquons donc une pause
    winner(chessboard,len);     //nous appelons la fonction de victoire
    fullfree(chessboard,len,hands); //à la fin d'une partie, nous nettoyons tous nos pointers pour éviter les fuites de mémoire
} 

void gameturn(cell **cboard,int len,int crpl,hnd cphand,crd (*checkpos)(cell**,int,crd,char,int),int (*cpcheck)(hnd,cell**,int,int),void (*modeatk)(cell**,int,crd,int)){// crpl : current player (joueur actuel) 

    
    printf("Vos pieces disponibles \033[90m(passer son tour e)\033[0m : ");
    for(int i=0;i<cphand.len[0];i++){   //.len[0] car il s'agit d'un pointeur 
        printf("%c ",cphand.hlist[i]);
    }printf("\n");

    //selection de la pièce à jouer

    int eleid = cpcheck(cphand,cboard,len,crpl);      //obtention de la position de la pièce choisie dans la liste de pièces disponibles 
    if(eleid == -1){printf("\nle joueur %d passe son tour\n",crpl);return;}
    char chpc = cphand.hlist[eleid];    //chpc = pièce choisie : le eleid-ème élément de cphand.hlist
    pop(cphand,eleid);
    
    //placement de la pièce choisie sur l'échiquier  

    crd pos = checkpos(cboard,len,getpos(len),chpc,crpl);    

    cboard[pos.x][pos.y].display = chpc;
    cboard[pos.x][pos.y].lord = crpl+1; //joueur 1 : crpl = 0 / lord = 1 | joueur 2 : crpl = 1 / lord = 2


    //conquête des cases menacées par la pièce posée (vif du sujet)

    takeover(cboard,len,pos,modeatk);          

}

void winner(cell **chessboard,int blen){
    int scores[2] = {0,0},max;  //scores[0] pour le joueur 1 ; scores[1] pour le joueur 2
    for(int x=0;x<blen;x++){
        for(int y=0;y<blen;y++){
            scores[chessboard[x][y].lord - 1]++;
        }
    }
    if(scores[0]<scores[1]){max = 2;}else if(scores[0]>scores[1]){max=1;}
    else{
        printf("\nEgalite ! ");
        return;
    }
    system("start /MIN .\\win.mp3");    //nous lancons un petit effet sonore de victoire
    printf("\n\n\033[32mLe joueur %d remporte la partie avec %d cases conquises\033[0m\n\n\n",max,scores[max-1]);
}

