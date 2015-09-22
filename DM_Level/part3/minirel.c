#include <stdio.h>
//#include "parser.tab.h"
#include "askisi.h"
////////////#include "UT_Lib.h"

int main(int argc, char* argv[]){
    char command[80];
    char* dbname;

    if(argc!=2){
       printf("Xrhsh: %s minirel \n",argv[0]);
       return(1);
    }
    dbname=argv[1];

    /* Arxikopoihsh programmatos kai anoigma sxesewn katalogou */
    //BF_Init();
    HF_Init();
    //AM_Init();

    if(chdir(dbname)<0){
        printf("Den yparxei h bash %s",dbname);
        exit(1);                    
    }
    if((relfileDesc=HF_OpenFile(RELCAT))<0){
        printf("Den einai dynato to anoigma tou RELCAT\n");
        exit(1);
    }
    if((attrfileDesc=HF_OpenFile(ATTRCAT))<0){
        printf("Den einai dynato to anoigma tou ATTRCAT\n");
        exit(1);
    }
    /* Klhsh ths synarthshs yyparse i opoia tha kalei automata tis synarthseis */
    while (yyparse()<0);

    /* Kleisimo programmatos kai kleisimo twn sxeswn katalogou an o xrhsths den */
    /* plhktrologhse thn entolh QUIT */
    if(HF_CloseFile(relfileDesc)!= HFE_OK){
            printf("Den einai dynato to kleisimo tou arxeiou RELCAT\n");
            exit(1);
    }
    if(HF_CloseFile(attrfileDesc)!= HFE_OK){
        printf("Den einai dynato to kleisimo tou arxeiou ATTRCAT\n");
        exit(1);
    }
    return(0);
}


