#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <direct.h>

#include "askisi.h"
#include "HF_Lib.h"
#define RELCAT "relCat"
#define ATTRCAT "attrCat"
#define MAXNAME 25//////////////////////////////////////////

int main(int argc, char* argv[]){
        char command[80];
        char* dbname;
        relDesc *relDescptr;
        attrDesc *attrDescptr;
        char name[2*MAXNAME+sizeof(char)];

        if(argc!=2){
                printf("Xrhsh: %s dbname \n",argv[0]);
                return(1);
        }

	    dbname=argv[1];
        sprintf(command,"mkdir %s",dbname);
        system(command);
        if(chdir(dbname)!=0){
                printf("Den einai dynath h dhmiourgia ths bashs\n");
                return(2);
        }
        
        /* Dhmiourgia twn pinakwn relCat kai attrCat me xrhsh twn synarthsewn ÇF_CreateFile */
        HF_Init();
        if(HF_CreateFile(RELCAT)!= HFE_OK){
            printf("Den einai dynath h dhmiourgia tou arxeiou RELCAT\n");        
            exit(1); 
        }
        if(HF_CreateFile(ATTRCAT)!= HFE_OK){
            printf("Den einai dynath h dhmiourgia tou arxeiou ATTRCAT\n");
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
            
        //Desmeuw mnhmh gia tous deiktes
        if((relDescptr=malloc(sizeof(relDesc)))==NULL){
            return -1;
        }
        if((attrDescptr=malloc(sizeof(attrDesc)))==NULL){
            return -1;
        }
        //Emhmerwnw ton pinaka relcat
        strcpy(relDescptr->relname,"relcat");                        
        relDescptr->relwidth=sizeof(relDesc);
        relDescptr->attrcnt=5;
        relDescptr->indexcnt=0;
        relDescptr->reccnt=2;
        
        if(HF_InsertRec(relfileDesc,(char *)relDescptr,sizeof(relDesc))<0){
            return -1;
        }
        
        strcpy(relDescptr->relname,"attrcat");                        
        relDescptr->relwidth=sizeof(attrDesc);
        relDescptr->attrcnt=10;
        relDescptr->indexcnt=0;
        relDescptr->reccnt=15;
        
        if(HF_InsertRec(relfileDesc,(char *)relDescptr,sizeof(relDesc))<0){
            return -1;
        } 
        free(relDescptr);
                                         
        //Emhmerwnw ton pinaka attrcat gia to pedio relname
        strcpy(attrDescptr->relname,"relcat");
        sprintf(name,"%s.%s","relcat","relname");
        strcpy(attrDescptr->attrname,name);
        attrDescptr->offset=0;
        attrDescptr->attrlength=MAXNAME;
        attrDescptr->attrtype='c';
        attrDescptr->valuecnt=2;   /////////////////////////////////////////////////////////////////
        attrDescptr->indexed=0;
        attrDescptr->indexno=0;
        //attrDescptr->min=0;
        //attrDescptr->max=0;
        
        if(HF_InsertRec(attrfileDesc,(char *)attrDescptr,sizeof(attrDesc))<0){
            return -1;
        }
        
        //Emhmerwnw ton pinaka attrcat gia to pedio relwidth
        strcpy(attrDescptr->relname,"relcat");
        sprintf(name,"%s.%s","relcat","relwidth");
        strcpy(attrDescptr->attrname,name);
        attrDescptr->offset=MAXNAME;
        attrDescptr->attrlength=sizeof(int);
        attrDescptr->attrtype='i';
        attrDescptr->valuecnt=2;
        attrDescptr->indexed=0;
        attrDescptr->indexno=0;
        //attrDescptr->min=0;
        //attrDescptr->max=0;
        if(HF_InsertRec(attrfileDesc,(char *)attrDescptr,sizeof(attrDesc))<0){
            return -1;
        }
        
        //Emhmerwnw ton pinaka attrcat gia to pedio attrcnt
        strcpy(attrDescptr->relname,"relcat");
        sprintf(name,"%s.%s","relcat","attrcnt");
        strcpy(attrDescptr->attrname,name);
        attrDescptr->offset=MAXNAME+sizeof(int);
        attrDescptr->attrlength=sizeof(int);
        attrDescptr->attrtype='i';
        attrDescptr->valuecnt=2;
        attrDescptr->indexed=0;
        attrDescptr->indexno=0;
        //attrDescptr->min=0;
        //attrDescptr->max=0;
        if(HF_InsertRec(attrfileDesc,(char *)attrDescptr,sizeof(attrDesc))<0){
            return -1;
        }
        
        //Emhmerwnw ton pinaka attrcat gia to pedio indexcnt
        strcpy(attrDescptr->relname,"relcat");
        sprintf(name,"%s.%s","relcat","indexcnt");
        strcpy(attrDescptr->attrname,name);
        attrDescptr->offset=MAXNAME+2*sizeof(int);
        attrDescptr->attrlength=sizeof(int);
        attrDescptr->attrtype='i';
        attrDescptr->valuecnt=2;
        attrDescptr->indexed=0;
        attrDescptr->indexno=0;
        //attrDescptr->min=0;
        //attrDescptr->max=0;
        if(HF_InsertRec(attrfileDesc,(char *)attrDescptr,sizeof(attrDesc))<0){
            return -1;
        }
        
        //Emhmerwnw ton pinaka attrcat gia to pedio reccnt
        strcpy(attrDescptr->relname,"relcat");
        sprintf(name,"%s.%s","relcat","reccnt");
        strcpy(attrDescptr->attrname,name);
        attrDescptr->offset=MAXNAME+3*sizeof(int);
        attrDescptr->attrlength=sizeof(int);
        attrDescptr->attrtype='i';
        attrDescptr->valuecnt=2;
        attrDescptr->indexed=0;
        attrDescptr->indexno=0;
        //attrDescptr->min=0;
        //attrDescptr->max=0;
        if(HF_InsertRec(attrfileDesc,(char *)attrDescptr,sizeof(attrDesc))<0){
            return -1;
        }
        
        //Emhmerwnw ton pinaka attrcat gia ta pedia tou
        //Pedio relname
        strcpy(attrDescptr->relname,"attrcat");
        sprintf(name,"%s.%s","attrcat","relname");
        strcpy(attrDescptr->attrname,name);
        attrDescptr->offset=0;
        attrDescptr->attrlength=MAXNAME;
        attrDescptr->attrtype='c';
        attrDescptr->valuecnt=2;
        attrDescptr->indexed=0;
        attrDescptr->indexno=0;
        //attrDescptr->min=0;
        //attrDescptr->max=0;
        if(HF_InsertRec(attrfileDesc,(char *)attrDescptr,sizeof(attrDesc))<0){
            return -1;
        }
        
        //Pedio attrname
        strcpy(attrDescptr->relname,"attrcat");
        sprintf(name,"%s.%s","attrcat","attrname");
        strcpy(attrDescptr->attrname,name);
        attrDescptr->offset=MAXNAME;
        attrDescptr->attrlength=2*MAXNAME;
        attrDescptr->attrtype='c';
        attrDescptr->valuecnt=15;
        attrDescptr->indexed=0;
        attrDescptr->indexno=0;
        //attrDescptr->min=0;
        //attrDescptr->max=0;
        if(HF_InsertRec(attrfileDesc,(char *)attrDescptr,sizeof(attrDesc))<0){
            return -1;
        }
        
        //Pedio offset
        strcpy(attrDescptr->relname,"attrcat");
        sprintf(name,"%s.%s","attrcat","offset");
        strcpy(attrDescptr->attrname,name);
        attrDescptr->offset=3*MAXNAME;
        attrDescptr->attrlength=sizeof(int);
        attrDescptr->attrtype='i';
        attrDescptr->valuecnt=14;
        attrDescptr->indexed=0;
        attrDescptr->indexno=0;
        //attrDescptr->min=0;
        //attrDescptr->max=0;
        if(HF_InsertRec(attrfileDesc,(char *)attrDescptr,sizeof(attrDesc))<0){
            return -1;
        }
        
        //Pedio attrlength
        strcpy(attrDescptr->relname,"attrcat");
        sprintf(name,"%s.%s","attrcat","attrlength");
        strcpy(attrDescptr->attrname,name);
        attrDescptr->offset=3*MAXNAME+sizeof(int);
        attrDescptr->attrlength=sizeof(int);
        attrDescptr->attrtype='i';
        attrDescptr->valuecnt=4;
        attrDescptr->indexed=0;
        attrDescptr->indexno=0;
        //attrDescptr->min=0;
        //attrDescptr->max=0;
        if(HF_InsertRec(attrfileDesc,(char *)attrDescptr,sizeof(attrDesc))<0){
            return -1;
        }
        
        //Pedio attrtype
        strcpy(attrDescptr->relname,"attrcat");
        sprintf(name,"%s.%s","attrcat","attrtype");
        strcpy(attrDescptr->attrname,name);
        attrDescptr->offset=3*MAXNAME+2*sizeof(int);
        attrDescptr->attrlength=sizeof(char);
        attrDescptr->attrtype='c';
        attrDescptr->valuecnt=2;
        attrDescptr->indexed=0;
        attrDescptr->indexno=0;
        //attrDescptr->min=0;
        //attrDescptr->max=0;
        if(HF_InsertRec(attrfileDesc,(char *)attrDescptr,sizeof(attrDesc))<0){
            return -1;
        }
        
        //Pedio valuecnt
        strcpy(attrDescptr->relname,"attrcat");
        sprintf(name,"%s.%s","attrcat","valuecnt");
        strcpy(attrDescptr->attrname,name);
        attrDescptr->offset=3*MAXNAME+2*sizeof(int)+sizeof(char);
        attrDescptr->attrlength=sizeof(int);
        attrDescptr->attrtype='i';
        attrDescptr->valuecnt=6; 
        attrDescptr->indexed=0;
        attrDescptr->indexno=0;
        //attrDescptr->min=0;
        //attrDescptr->max=0;
        if(HF_InsertRec(attrfileDesc,(char *)attrDescptr,sizeof(attrDesc))<0){
            return -1;
        }
        
        //Pedio indexed
        strcpy(attrDescptr->relname,"attrcat");
        sprintf(name,"%s.%s","attrcat","indexed");
        strcpy(attrDescptr->attrname,name);
        attrDescptr->offset=3*MAXNAME+3*sizeof(int)+sizeof(char);
        attrDescptr->attrlength=sizeof(int);
        attrDescptr->attrtype='i';
        attrDescptr->valuecnt=1;
        attrDescptr->indexed=0;
        attrDescptr->indexno=0;
        //attrDescptr->min=0;
        //attrDescptr->max=0;
        if(HF_InsertRec(attrfileDesc,(char *)attrDescptr,sizeof(attrDesc))<0){
            return -1;
        }
        
        //Pedio indexno
        strcpy(attrDescptr->relname,"attrcat");
        sprintf(name,"%s.%s","attrcat","indexno");
        strcpy(attrDescptr->attrname,name);
        attrDescptr->offset=3*MAXNAME+4*sizeof(int)+sizeof(char);
        attrDescptr->attrlength=sizeof(int);
        attrDescptr->attrtype='i';
        attrDescptr->valuecnt=1;
        attrDescptr->indexed=0;
        attrDescptr->indexno=0;
        //attrDescptr->min=0;
        //attrDescptr->max=0;
        if(HF_InsertRec(attrfileDesc,(char *)attrDescptr,sizeof(attrDesc))<0){
            return -1;
        }
        
        //Pedio min
        strcpy(attrDescptr->relname,"attrcat");
        sprintf(name,"%s.%s","attrcat","indexno");
        strcpy(attrDescptr->attrname,name);
        attrDescptr->offset=3*MAXNAME+5*sizeof(int)+sizeof(char);
        attrDescptr->attrlength=sizeof(float);
        attrDescptr->attrtype='f';
        attrDescptr->valuecnt=1;
        attrDescptr->indexed=0;
        attrDescptr->indexno=0;
        //attrDescptr->min=0;
        //attrDescptr->max=0;
        if(HF_InsertRec(attrfileDesc,(char *)attrDescptr,sizeof(attrDesc))<0){
            return -1;
        }
        
        //Pedio max
        strcpy(attrDescptr->relname,"attrcat");
        sprintf(name,"%s.%s","attrcat","indexno");
        strcpy(attrDescptr->attrname,name);
        attrDescptr->offset=3*MAXNAME+5*sizeof(int)+sizeof(char)+sizeof(float);
        attrDescptr->attrlength=sizeof(float);
        attrDescptr->attrtype='f';
        attrDescptr->valuecnt=1;
        attrDescptr->indexed=0;
        attrDescptr->indexno=0;
        //attrDescptr->min=0;
        //attrDescptr->max=0;
        if(HF_InsertRec(attrfileDesc,(char *)attrDescptr,sizeof(attrDesc))<0){
            return -1;
        }
        
        return 0;
}
