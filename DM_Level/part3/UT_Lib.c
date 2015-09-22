#include <stdio.h>
#include "askisi.h"
#include "UT_Lib.h"

int UT_create(int argc,char* argv[]){
    int scanDesc,i,relwidth=0,offset=0;
    char name[2*MAXNAME+sizeof(char)];
    relDesc relmetadata,s;
    attrDesc attrmetadata;
    
    //Elegxos gia ortho argc 
    if(argc<4 || argc%2!=0){
       printf("Invalid argc\n");
       return -2;//Lathos argc          
    }    
    //Elegxos an uparxei h sxesh
    if((scanDesc=HF_OpenFileScan(relfileDesc,sizeof(relDesc),'c',MAXNAME,0,1,(char *)argv[1]))<0){
       HF_PrintError("");
       return -1;//Lathos sto anoigma ths sarwshs                                                                                
    }
    if(HF_FindNextRec(scanDesc,(char *)&s)>=0){
       HF_PrintError("Relation already exists : ");
       return -1;//H sxesh uparxei hdh                                            
    }
    if(HF_CloseFileScan(scanDesc)!=HFE_OK){
       HF_PrintError("");
       return -1;//Lathos sto kleisimo ths sarwshs                                            
    }
    //Den uparxei h sxesh opote th dhmiourgw
    if(HF_CreateFile(argv[1])!= HFE_OK){
       HF_PrintError("");
       return -1;//Lathos sthn dhmiourgia tou arxeiou
    }  
    //Enhmerwnw ta stoixeia tou RELCAT
    strcpy(relmetadata.relname,argv[1]);
    //Xrhsimopoioume bhma + wste na paroume ola ta pedia mexri argc-1
    for(i=3;i<argc;i+=2){
       if(strcmp(argv[i],"'i'")==0){          //to format tou pediou einai int
            relwidth+=sizeof(int);
       }
       else if(strcmp(argv[i],"'f'")==0){     //to format tou pediou einai float
            relwidth+=sizeof(float);
       }    
       else{
            relwidth+=atoi(argv[i]+2);        //to format tou pediou einai char
       }
    }
    relmetadata.relwidth=relwidth;
    relmetadata.attrcnt=(argc-2)/2;
    relmetadata.indexcnt=0;
    relmetadata.reccnt=0;
    //Eisagw thn eggrafh sto arxeio 
    if(HF_InsertRec(relfileDesc,(char *)&relmetadata,sizeof(relDesc))<0){
       HF_PrintError("");
       return -1;//Lathos sthn eisagwgh ths eggrafhs
    }
    //Enhmerwnw ta stoixeia tou ATTRCAT
    for(i=2;i<argc;i+=2){
       strcpy(attrmetadata.relname,relmetadata.relname);
       //To name tha einai relname.attrname        
       sprintf(name,"%s.%s",relmetadata.relname,argv[i]);
       strcpy(attrmetadata.attrname,name);
       attrmetadata.offset=offset;     
       attrmetadata.valuecnt=0;
       //Elegxw to format tou kathe pediou gia na ftiaxw to attrtype kai attrlength
       if(strcmp(argv[i+1],"'i'")==0){
            offset+=sizeof(int);
            attrmetadata.attrlength=sizeof(int);
            attrmetadata.attrtype='i';
       }
       else if(strcmp(argv[i+1],"'f'")==0){
            offset+=sizeof(float);
            attrmetadata.attrlength=sizeof(float);
            attrmetadata.attrtype='f';
       }    
       else{
            offset+=atoi(argv[i+1]+2);
            attrmetadata.attrlength=atoi(argv[i+1]+2);      
            attrmetadata.attrtype='c';
       }
       attrmetadata.indexed=0;
       attrmetadata.indexno=-1;
       //Eisagw thn eggrafh sto arxeio
       if(HF_InsertRec(attrfileDesc,(char *)&attrmetadata,sizeof(attrDesc))<0){
          HF_PrintError("");
          return -1;//Lathos sthn eisagwgh ths eggrafhs
       }
    }
    printf("Relation was successfully created\n");
    return 0;
}
int UT_buildindex(int argc,char* argv[]){
    int fileDesc,fileDesc1,scanDesc,scanDesc1,recId1,recId2;
    relDesc relmetadata;
    attrDesc attrmetadata;
    char name[2*MAXNAME+sizeof(char)],name1[MAXNAME+sizeof(int)+sizeof(char)],*s;
    
    //Elegxos gia ortho argc 
    if(argc!=3){
       printf("Invalid argc\n");
       return -2;//Lathos argc          
    }
    //Elegxos an uparxei h sxesh
    if((scanDesc=HF_OpenFileScan(relfileDesc,sizeof(relDesc),'c',MAXNAME,0,1,argv[1]))<0){
       HF_PrintError("");
       return -1;//Lathos sto anoigma ths sarwshs                                                                                
    }
    if((recId1=HF_FindNextRec(scanDesc,(char *)&relmetadata))<0){
       HF_PrintError("Invalid relation to build index : ");
       return -1;//H sxesh den uparxei                                            
    }
    //memcpy(relmetadata1,&relmetadata2,sizeof(relDesc));
    if(HF_CloseFileScan(scanDesc)!=HFE_OK){
       HF_PrintError("");
       return -1;//Lathos sto kleisimo ths sarwshs                                            
    }
    //Sbhnw thn eggrafh gia na perasw tis enhmerwseis
    if(HF_DeleteRec(relfileDesc,recId1,sizeof(relDesc))!=HFE_OK){
        HF_PrintError("");
        return -1;//Lathos sth diagrafh ths eggrafhs
    }
    //Elegxos an uparxei to pedio
    sprintf(name,"%s.%s",argv[1],argv[2]);
    if((scanDesc=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
       HF_PrintError("");
       return -1;//Lathos sto anoigma ths sarwshs                                                                                
    }
    if((recId2=HF_FindNextRec(scanDesc,(char *)&attrmetadata))<0){
       HF_PrintError("Invalid relation.attribute to build index : ");
       return -1;//To pedio den uparxei                                            
    }
    //Elegxos an yparxei hdh eurethrio sto pedio
    if(attrmetadata.indexed==1){
       printf("Index on this attribute already exists : ");
       return -4;//yparxei to eurethrio
    }
    if(HF_CloseFileScan(scanDesc)!=HFE_OK){
       HF_PrintError("");
       return -1;//Lathos sto kleisimo ths sarwshs                                            
    }
    //Sbhnw thn eggrafh gia na perasw tis enhmerwseis
    if(HF_DeleteRec(attrfileDesc,recId2,sizeof(attrDesc))!=HFE_OK){
       HF_PrintError("");
       return -1;//Lathos sth diagrafh ths eggrafhs
    }
    //Kanw tis aparaithtes enhmerwseis
    (attrmetadata.indexed)=1;
    (attrmetadata.indexno)=(relmetadata.indexcnt)+1;
    (relmetadata.indexcnt)+=1;
    //Eisagw ta nea dedomena sta arxeia
    if(HF_InsertRec(relfileDesc,(char *)&relmetadata,sizeof(relDesc))<0){
       HF_PrintError("");
       return -1;//Lathos sthn eisagwgh ths eggrafhs
    }
    if(HF_InsertRec(attrfileDesc,(char *)&attrmetadata,sizeof(attrDesc))<0){
       HF_PrintError(""); 
       return -1;//Lathos sthn eisagwgh ths eggrafhs
    }
    //Dhmiourgw to eurethrio
    if(AM_CreateIndex(argv[1],attrmetadata.indexno,attrmetadata.attrtype,attrmetadata.attrlength)!=AME_OK){
       AM_PrintError("");
       return -1;//Lathos sthn dhmiougia tou eurethriou
    }
    sprintf(name1,"%s.%d",argv[1],attrmetadata.indexno);
    //Afou dhmiourgw to eurethrio, anoigw auto kai to arxeio
    if((fileDesc=BF_OpenFile(name1))<0){
       BF_PrintError("");
       return -1;//Lathos sto anoigma tou arxeiou                    
    }
    if((fileDesc1=HF_OpenFile(argv[1]))<0){
       HF_PrintError("");
       return -1;//Lathos sto anoigma tou arxeiou                    
    }
    //Anoigw mia sarwsh me NULL gia na epistresei oles tis eggrafes
    if((scanDesc1=HF_OpenFileScan(fileDesc1,relmetadata.relwidth,'c',MAXNAME,0,1,NULL))<0){
       HF_PrintError("");
       return -1;//Lathos sto anoigma ths sarwshs                                                                                
    }
    if((s=malloc(relmetadata.relwidth))==NULL){
        return -3;
    }
    //Mexri na teleiwsoun oi eggrafes sto arxeio
    while((recId1=HF_FindNextRec(scanDesc1,s))>=0){
       //Tis eisagw sto eurethrio
       if(AM_InsertEntry(fileDesc,attrmetadata.attrtype,attrmetadata.attrlength,s+(attrmetadata.offset),recId1)!=AME_OK){
          AM_PrintError("");
          return -1;//Lathos sthn eisagwgh eggrafhs sto eurethrio                                                                                                                     
       }
    }
    //Kleinw ta arxeia kai thn sarwsh pou exw anoiksei
    if(BF_CloseFile(fileDesc)!=BFE_OK){
       BF_PrintError("");
       return -1;//Lathos sto kleisimo ths sarwshs                                            
    }
    if(HF_CloseFile(fileDesc1)!=HFE_OK){
       HF_PrintError("");
       return -1;//Lathos sto kleisimo ths sarwshs                                            
    }
    if(HF_CloseFileScan(scanDesc1)!=HFE_OK){
       HF_PrintError("");
       return -1;//Lathos sto kleisimo ths sarwshs                                            
    }
    printf("Index was successfully created\n");                       
    return 0;
}
int UT_destroy(int argc,char* argv[]){
    int scanDesc,recId1,recId2;
    relDesc relmetadata;
    attrDesc attrmetadata;
    char name[MAXNAME+sizeof(int)+sizeof(char)]; 
    //Elegxos gia ortho argc 
    if(argc!=2){
       return -2;//Lathos argc          
    } 
    //Elegxos an uparxei h sxesh
    if((scanDesc=HF_OpenFileScan(relfileDesc,sizeof(relDesc),'c',MAXNAME,0,1,argv[1]))<0){
       HF_PrintError("");
       return -1;//Lathos sto anoigma ths sarwshs                                                                                
    }
    if((recId1=HF_FindNextRec(scanDesc,(char *)&relmetadata))<0){
       HF_PrintError("Invalid relation to destroy : ");
       return -1;//H sxesh den uparxei                                            
    }
    if(HF_CloseFileScan(scanDesc)!=HFE_OK){
       HF_PrintError("");
       return -1;//Lathos sto kleisimo ths sarwshs                                            
    }
    //Sbhsimo eggrafhs
    if(HF_DeleteRec(relfileDesc,recId1,sizeof(relDesc))!=HFE_OK){
       HF_PrintError("");
       return -1;//Lathos sth diagrafh ths eggrafhs
    }
    do{
      //Anoigw mia sarwsh me value=relname(onoma sxeshs)
      if((scanDesc=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,0,1,argv[1]))<0){
         HF_PrintError("");
         return -1;//Lathos sto anoigma ths sarwshs                                                                                
      }
      if((recId2=HF_FindNextRec(scanDesc,(char *)&attrmetadata))<0){
         break;//To pedio den uparxei                                            
      }
      //An exei eurethrio
      if((attrmetadata.indexed)==1){
          sprintf(name,"%s.%d",attrmetadata.relname,attrmetadata.indexno);
          //Katastrefw to arxeio name
          if(BF_DestroyFile(name)!=BFE_OK){
             BF_PrintError("");
             return -1;//Lathos sthn katastrofh tou arxeiou                                 
          }
      }
      if(HF_CloseFileScan(scanDesc)!=HFE_OK){
         HF_PrintError("");
         return -1;//Lathos sto kleisimo ths sarwshs                                            
      }
      //Sbhnw thn eggrafh
      if(HF_DeleteRec(attrfileDesc,recId2,sizeof(attrDesc))!=HFE_OK){
         HF_PrintError("");
         return -1;//Lathos sth diagrafh ths eggrafhs
      }
    }while(1); //Tha synexistei mexri i FindNextRec na epistrepsei arnhtiko dld 
               //telos arxeiou
    //Katastrefw to arxeio
    if(HF_DestroyFile(argv[1])!=HFE_OK){
       HF_PrintError(""); 
       return -1;//Lathos sthn katastrofh tou arxeiou                                                  
    }
    printf("Relation and all indexes were successfully destroyed\n");
    return 0;//Ola ok!!!
}
int UT_quit(int argc,char* argv[]){
    //Kleinw ta arxeia 
    if(HF_CloseFile(relfileDesc)!=HFE_OK){
       HF_PrintError("");
       return -1;//Lathos sto kleisimo tou arxeiou                                           
    }
    if(HF_CloseFile(attrfileDesc)!=HFE_OK){
       HF_PrintError("");
       return -1;//Lathos sto kleisimo tou arxeiou                                           
    }
    exit(1);
}
