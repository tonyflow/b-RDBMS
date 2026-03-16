#include <stdio.h>
//#include "parser.tab.h"
#include "askisi.h"
////////////#include "UT_Lib.h"

int main(int argc, char* argv[]){
    char command[80];
    char* dbname;

    if(argc!=2){
       printf("Usage: %s minirel \n",argv[0]);
       return(1);
    }
    dbname=argv[1];

    /* Program initialization and opening of catalog relations */
    //BF_Init();
    HF_Init();
    //AM_Init();

    if(chdir(dbname)<0){
        printf("Database %s does not exist",dbname);
        exit(1);                    
    }
    if((relfileDesc=HF_OpenFile(RELCAT))<0){
        printf("Unable to open RELCAT\n");
        exit(1);
    }
    if((attrfileDesc=HF_OpenFile(ATTRCAT))<0){
        printf("Unable to open ATTRCAT\n");
        exit(1);
    }
    /* Call the yyparse function which will automatically call the functions */
    while (yyparse()<0);

    /* Close the program and close the catalog relations if the user did not */
    /* type the QUIT command */
    if(HF_CloseFile(relfileDesc)!= HFE_OK){
            printf("Unable to close the RELCAT file\n");
            exit(1);
    }
    if(HF_CloseFile(attrfileDesc)!= HFE_OK){
        printf("Unable to close the ATTRCAT file\n");
        exit(1);
    }
    return(0);
}


