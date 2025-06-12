#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chessp.h" 
#include "starter.h"


int mmenu();//display the chess game main menu - asks user for choice 
int checkinput(int wmenu); //check if the input matches the chosen function's expectations

int main(){
    //system("color d");

    //implémentation boucle programme 

    int frstt = 0;  //first-time : est-ce la première itération de main 
    while(1){
        if(frstt){          //si ce n'est pas la première itération du programme alors : nous attendons 2,000ms que l'utilisateur puisse regarder le message de fin
            printf("\n\033[34mAppuyez sur ENTREE\033[0m\n");    //nous attendons PRESS_ENTER que l'utilisateur puisse regarder le message de fin...
            flusher();
            getchar();
            system("cls"); // ...puis nous cls-ons le terminal afin de le nettoyer et de relancer une partie sur de bonnes bases 
        }else{
            frstt=1;    //si c'est bien la première itération, il n'y a aucune raison ni d'attendre ni de nettoyer 
        }

        switch(mmenu()){
            case 0 : {
                printf("\nAu revoir !");
                return 0;
            }

            case 2 : {
                printf("\nReprendre une Partie >");
                setup(1,-1);    //mode is yet to be retrieved
                break;
            }

            case 11 : {
                printf("\nNouvelle Partie > Mode Conquete\n");
                //conqsetup(0);
                setup(0,0);
                break;
            }

            case 12 : {
                printf("\nNouvelle Partie > Mode Connecte\n");
                setup(0,1);
                break;
            }
        }
    }
}

int mmenu(){
    printf("\n\n\t\t\tJEU D'ECHECS \n\n\n1. Nouvelle Partie\t2. Reprendre une partie\t\t3. Quitter\n");
    switch(checkinput(0)){
        case 1 : {
            printf("\n\t\tNouvelle Partie\n\n\t1. Mode Conquete\t2. Mode Connecte\n");
            switch(checkinput(1)){
                case 1 : return 11; //11 because choice 1 then 1

                case 2 : return 12; //12 because choice 1 then 2
            }
        }
        case 2 : {
            return 2;
        }
        case 3 : {
            return 0;
        }
    }

}

int checkinput(int wmenu){  //wmenu = 0 : first menu ; =1 : 1.submenu 
    int userinput;
    switch(wmenu){
        case 0 : {
            printf("\noption choisie (1, 2 ou 3) : ");//scanf(" %d",&userinput);
            if(scanf(" %d",&userinput)!=1){flusher();}
            return (userinput<=3 && userinput>0) ? userinput:checkinput(wmenu); //check that the input is matching the given options ; otherwise repeat the question
        }
        case 1 : {
            printf("\n\t\tMode de jeu (1 ou 2) : ");//scanf(" %d",&userinput);
            if(scanf(" %d",&userinput)!=1){flusher();}
            return (userinput<=2 && userinput>0) ? userinput:checkinput(wmenu);
        }
        
    }
}

void flusher(){
    int ch;while((ch = getchar())!='\n' && ch != EOF);  //fonction usuelle pour vider le buffer en cas d'input du mauvais type 
}  

void fullfree(cell **board,int dim,hnd hands[2]){
    for(int i = 0 ;i<dim;i++){free(board[i]);}
    free(board);
    for(int i = 0;i<2;i++){
        free(hands[i].hlist);
        free(hands[i].len);
    }
}

//encodage / décodage fichier

void cipher(int mode,int len,int lastpl, cell **board,hnd hands[2]){  //fonction encodant le mode ; l'échiquier ; et les 2 mains -> on a ainsi une séquence display;lord;atkr;display;lord etc 
    FILE *csvf = fopen(".\\gamesave.csv","w");
    if(!csvf){printf("masterchiasse ! l'ouverture a échouée");return;}
    //première ligne : mode ; taille ; dernier joueur ;
    fprintf(csvf,"%d;%d;%d;\n",mode,len,lastpl);    //on garde le joueur qui a sauvegardé, car il n'a alors pas joué son tour                  
    //2e ligne : tout l'échiquier 
    for(int x=0;x<len;x++){
        for(int y=0;y<len;y++){
            fprintf(csvf,"%c;%d;",board[x][y].display,board[x][y].lord);
            //printf("done 2el l2");
            if(mode){
                for(int atkrp=0;atkrp<2;atkrp++){
                   for(int atkri=0;atkri<5;atkri++){
                        fprintf(csvf,"%d;",board[x][y].atkr[atkrp][atkri]);
                    }
                }
            }
        }
    }
    fprintf(csvf,"\n");
    //3e ligne : taille des mains  
    fprintf(csvf,"%d;%d;\n",hands[0].len[0],hands[1].len[0]);
    //4e & 5e lignes : les listes de pièces
    for(int plh=0;plh<2;plh++){
        for(int i=0;i<hands[plh].len[0];i++){
            fprintf(csvf,"%c;",hands[plh].hlist[i]);
        }
        fprintf(csvf,"\n");
    }
    fclose(csvf);
}

dcphr decipher(char *path){  //normalement toujours le même chemin ./gamesave.csv mais cette flexibilité nous servira pour le debug 
    FILE *csvf = fopen(path,"r");
    dcphr gbo;  //notre struct permettant de retourner toutes les variables en sortie de fonction 
    printf("\nis open\n");
    //ligne 1 : récupérer le mode ; la taille de l'échiquier ; le joueur qui reprend
    char *rl,*l1[3];
    rl = malloc(9 * sizeof(char));    //taille max : "1;12;1;\n\0" soit 9 caractères | précision maximale : peu recommandée mais plus intéressante pédagogiquement
    fgets(rl,sizeof(rl),csvf);     //rl : raw line 1 -> première ligne non traitée | *l1 : pointer de tableau pointant vers les 3 valeurs parsées
    l1[0] = strtok(rl,";");           //strtok : va jusqu'au premier ';' qu'il trouve -> pointe ce qui est avant 
    for(int i=1;i<3;i++){
        l1[i] = strtok(NULL,";");       //strtok : reprend miraculeusement d'où il en était jusqu'au prochain semicolon puis pointe la valeur survolée
    }//-> nous avons récupéré le mode de jeu (l1[0]) la taille de l'échiquier (l1[1]) et enfin le dernier joueur en date (l1[2])
    gbo.mode=atoi(l1[0]),gbo.len = atoi(l1[1]),gbo.pltp = atoi(l1[2]);  //nous retournerons nos variables dans tous les cas : autant les initialiser tout de suite
    free(rl);//for(int i=0;i<3;i++){free(l1[i]);};  //on nettoie rapidement nos pointers puisque plus utilisés

    printf("\nfirst line done\n");

    //ligne 2 : récupérer tout le tableau 
        //taille * taille cases -> 1char display + ; + 0/1/2 lord + ; + (mode 0/1) rien:2*0;0;0;0;0; -> \n\0 en bout de ligne
        //max mode 0 (conquête) : (taille * taille * 4) + 2 

    char *l2[(!gbo.mode)?(2*gbo.len*gbo.len):(gbo.len*gbo.len*12)];
    rl = malloc( (((gbo.mode)?(24):(4))*gbo.len*gbo.len + 2)*sizeof(char));
            //mode connecte : 2+2 + 2*5*2 (2 x 5x0 / 5x; )
    fgets(rl,(((gbo.mode)?(24):(4))*gbo.len*gbo.len + 2)*sizeof(char),csvf);
    l2[0] = strtok(rl,";");
    for(int i=1;i<((!gbo.mode)?(2*gbo.len*gbo.len):(gbo.len*gbo.len*12));i++){
        l2[i] = strtok(NULL,";");
    }
    gbo.chessboard = malloc(gbo.len*sizeof(cell*));for(int i=0;i<gbo.len;i++){gbo.chessboard[i] = malloc(gbo.len*sizeof(cell));} //nous définissons l'échiquier à retourner...

    //chessboard retrieval (banger)
    int li = 0;
    for(int i = 0;i<gbo.len;i++){
        for(int j=0;j<gbo.len;j++){
            gbo.chessboard[i][j].display = l2[li++][0]; //accessing ...[0] to get a char (not a char*)
            gbo.chessboard[i][j].lord = atoi(l2[li++]);
            if(gbo.mode){
                for(int ai=0;ai<2;ai++){
                    for(int aj=0;aj<5;aj++){
                        gbo.chessboard[i][j].atkr[ai][aj] = atoi(l2[li++]);
                    }
                }
            }
        }
    }
    //display_v2(gbo.chessboard,gbo.len);    //quick debug
    free(rl);

    printf("\n2nd line done\n");

    //ligne 3 : récupération des tailles des mains
    //char *l3[2]; il n'est même pas utile de créer une ligne, autant attribuer immédiatement les valeurs aux variables 
    rl = malloc(8*sizeof(char));    //xx;xx;\n\0 caracters
    if(fgets(rl,8*sizeof(char),csvf)==NULL){printf("you stoopid nigg...");}; 
    printf("\n%s\n",rl);   
    gbo.hands[0].len = malloc(sizeof(int));gbo.hands[1].len = malloc(sizeof(int));
    gbo.hands[0].len[0]= atoi(strtok(rl,";"));
    gbo.hands[1].len[0]= atoi(strtok(NULL,";"));
    printf("nigga whaaat");
    free(rl);

    printf("\n3rd line done\n");

    //ligne 4 & 5 : récupération du contenu des mains 
    for(int plh=0;plh<2;plh++){
        rl = malloc((gbo.hands[plh].len[0]*2 + 2)*sizeof(char));
        fgets(rl,(gbo.hands[plh].len[0]*2 + 2)*sizeof(char),csvf);
        gbo.hands[plh].hlist = malloc(gbo.hands[plh].len[0]*sizeof(char));
        gbo.hands[plh].hlist[0] = strtok(rl,";")[0];    //pour convertir les strings en char, nous appelons strtok(rl ;)[0] (le premier & dernier vrai caractère du string)
        for(int i = 1;i<gbo.hands[plh].len[0];i++){
            gbo.hands[plh].hlist[i] = strtok(NULL,";")[0];
        }
        free(rl);
    }
    fclose(csvf);
    return gbo;
}   

