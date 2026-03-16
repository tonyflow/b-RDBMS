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
                printf("Usage: %s dbname \n",argv[0]);
                return(1);
        }

	    dbname=argv[1];
        sprintf(command,"mkdir %s",dbname);
        system(command);
        if(chdir(dbname)!=0){
                printf("Unable to create the database\n");
                return(2);
        }
        
        /* Create the relCat and attrCat tables using the HF_CreateFile functions */
        HF_Init();
        if(HF_CreateFile(RELCAT)!= HFE_OK){
            printf("Unable to create the RELCAT file\n");        
            exit(1); 
        }
        if(HF_CreateFile(ATTRCAT)!= HFE_OK){
            printf("Unable to create the ATTRCAT file\n");
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
            
        //Allocate memory for the pointers
        if((relDescptr=malloc(sizeof(relDesc)))==NULL){
            return -1;
        }
        if((attrDescptr=malloc(sizeof(attrDesc)))==NULL){
            return -1;
        }
        //Update the relcat table
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
                                         
        //Update the attrcat table for the relname field
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
        
        //Update the attrcat table for the relwidth field
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
        
        //Update the attrcat table for the attrcnt field
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
        
        //Update the attrcat table for the indexcnt field
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
        
        //Update the attrcat table for the reccnt field
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
        
        //Update the attrcat table for its fields
        //Field relname
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
        
        //Field attrname
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
        
        //Field offset
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
        
        //Field attrlength
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
        
        //Field attrtype
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
        
        //Field valuecnt
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
        
        //Field indexed
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
        
        //Field indexno
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
        
        //Field min
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
        
        //Field max
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
