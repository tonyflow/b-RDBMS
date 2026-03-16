#include <stdio.h>
#include "askisi.h"
#include "UT_Lib.h"

int UT_create(int argc,char* argv[]){
    int scanDesc,i,relwidth=0,offset=0;
    char name[2*MAXNAME+sizeof(char)];
    relDesc relmetadata,s;
    attrDesc attrmetadata;
    
    //Check for correct argc
    if(argc<4 || argc%2!=0){
       printf("Invalid argc\n");
       return -2;//Invalid argc          
    }    
    //Check if the relation exists
    if((scanDesc=HF_OpenFileScan(relfileDesc,sizeof(relDesc),'c',MAXNAME,0,1,(char *)argv[1]))<0){
       HF_PrintError("");
       return -1;//Error opening the scan                                                                                
    }
    if(HF_FindNextRec(scanDesc,(char *)&s)>=0){
       HF_PrintError("Relation already exists : ");
       return -1;//The relation already exists                                            
    }
    if(HF_CloseFileScan(scanDesc)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan                                            
    }
    //The relation does not exist so we create it
    if(HF_CreateFile(argv[1])!= HFE_OK){
       HF_PrintError("");
       return -1;//Error creating the file
    }  
    //Update the RELCAT data
    strcpy(relmetadata.relname,argv[1]);
    //Use step +2 to get all fields up to argc-1
    for(i=3;i<argc;i+=2){
       if(strcmp(argv[i],"'i'")==0){          //the field format is int
            relwidth+=sizeof(int);
       }
       else if(strcmp(argv[i],"'f'")==0){     //the field format is float
            relwidth+=sizeof(float);
       }    
       else{
            relwidth+=atoi(argv[i]+2);        //the field format is char
       }
    }
    relmetadata.relwidth=relwidth;
    relmetadata.attrcnt=(argc-2)/2;
    relmetadata.indexcnt=0;
    relmetadata.reccnt=0;
    //Insert the record into the file
    if(HF_InsertRec(relfileDesc,(char *)&relmetadata,sizeof(relDesc))<0){
       HF_PrintError("");
       return -1;//Error inserting the record
    }
    //Update the ATTRCAT data
    for(i=2;i<argc;i+=2){
       strcpy(attrmetadata.relname,relmetadata.relname);
       //The name will be relname.attrname        
       sprintf(name,"%s.%s",relmetadata.relname,argv[i]);
       strcpy(attrmetadata.attrname,name);
       attrmetadata.offset=offset;     
       attrmetadata.valuecnt=0;
       //Check the format of each field to build attrtype and attrlength
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
       //Insert the record into the file
       if(HF_InsertRec(attrfileDesc,(char *)&attrmetadata,sizeof(attrDesc))<0){
          HF_PrintError("");
          return -1;//Error inserting the record
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
    
    //Check for correct argc
    if(argc!=3){
       printf("Invalid argc\n");
       return -2;//Invalid argc          
    }
    //Check if the relation exists
    if((scanDesc=HF_OpenFileScan(relfileDesc,sizeof(relDesc),'c',MAXNAME,0,1,argv[1]))<0){
       HF_PrintError("");
       return -1;//Error opening the scan                                                                                
    }
    if((recId1=HF_FindNextRec(scanDesc,(char *)&relmetadata))<0){
       HF_PrintError("Invalid relation to build index : ");
       return -1;//The relation does not exist                                            
    }
    //memcpy(relmetadata1,&relmetadata2,sizeof(relDesc));
    if(HF_CloseFileScan(scanDesc)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan                                            
    }
    //Delete the record to apply the updates
    if(HF_DeleteRec(relfileDesc,recId1,sizeof(relDesc))!=HFE_OK){
        HF_PrintError("");
        return -1;//Error deleting the record
    }
    //Check if the field exists
    sprintf(name,"%s.%s",argv[1],argv[2]);
    if((scanDesc=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
       HF_PrintError("");
       return -1;//Error opening the scan                                                                                
    }
    if((recId2=HF_FindNextRec(scanDesc,(char *)&attrmetadata))<0){
       HF_PrintError("Invalid relation.attribute to build index : ");
       return -1;//The field does not exist                                            
    }
    //Check if an index already exists on the field
    if(attrmetadata.indexed==1){
       printf("Index on this attribute already exists : ");
       return -4;//the index exists
    }
    if(HF_CloseFileScan(scanDesc)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan                                            
    }
    //Delete the record to apply the updates
    if(HF_DeleteRec(attrfileDesc,recId2,sizeof(attrDesc))!=HFE_OK){
       HF_PrintError("");
       return -1;//Error deleting the record
    }
    //Perform the necessary updates
    (attrmetadata.indexed)=1;
    (attrmetadata.indexno)=(relmetadata.indexcnt)+1;
    (relmetadata.indexcnt)+=1;
    //Insert the new data into the files
    if(HF_InsertRec(relfileDesc,(char *)&relmetadata,sizeof(relDesc))<0){
       HF_PrintError("");
       return -1;//Error inserting the record
    }
    if(HF_InsertRec(attrfileDesc,(char *)&attrmetadata,sizeof(attrDesc))<0){
       HF_PrintError(""); 
       return -1;//Error inserting the record
    }
    //Create the index
    if(AM_CreateIndex(argv[1],attrmetadata.indexno,attrmetadata.attrtype,attrmetadata.attrlength)!=AME_OK){
       AM_PrintError("");
       return -1;//Error creating the index
    }
    sprintf(name1,"%s.%d",argv[1],attrmetadata.indexno);
    //After creating the index, open it and the file
    if((fileDesc=BF_OpenFile(name1))<0){
       BF_PrintError("");
       return -1;//Error opening the file                    
    }
    if((fileDesc1=HF_OpenFile(argv[1]))<0){
       HF_PrintError("");
       return -1;//Error opening the file                    
    }
    //Open a scan with NULL to return all records
    if((scanDesc1=HF_OpenFileScan(fileDesc1,relmetadata.relwidth,'c',MAXNAME,0,1,NULL))<0){
       HF_PrintError("");
       return -1;//Error opening the scan                                                                                
    }
    if((s=malloc(relmetadata.relwidth))==NULL){
        return -3;
    }
    //Until the file records are exhausted
    while((recId1=HF_FindNextRec(scanDesc1,s))>=0){
       //Insert them into the index
       if(AM_InsertEntry(fileDesc,attrmetadata.attrtype,attrmetadata.attrlength,s+(attrmetadata.offset),recId1)!=AME_OK){
          AM_PrintError("");
          return -1;//Error inserting the record into the index                                                                                                                     
       }
    }
    //Close the files and the scan that we opened
    if(BF_CloseFile(fileDesc)!=BFE_OK){
       BF_PrintError("");
       return -1;//Error closing the scan                                            
    }
    if(HF_CloseFile(fileDesc1)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan                                            
    }
    if(HF_CloseFileScan(scanDesc1)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan                                            
    }
    printf("Index was successfully created\n");                       
    return 0;
}
int UT_destroy(int argc,char* argv[]){
    int scanDesc,recId1,recId2;
    relDesc relmetadata;
    attrDesc attrmetadata;
    char name[MAXNAME+sizeof(int)+sizeof(char)]; 
    //Check for correct argc
    if(argc!=2){
       return -2;//Invalid argc          
    } 
    //Check if the relation exists
    if((scanDesc=HF_OpenFileScan(relfileDesc,sizeof(relDesc),'c',MAXNAME,0,1,argv[1]))<0){
       HF_PrintError("");
       return -1;//Error opening the scan                                                                                
    }
    if((recId1=HF_FindNextRec(scanDesc,(char *)&relmetadata))<0){
       HF_PrintError("Invalid relation to destroy : ");
       return -1;//The relation does not exist                                            
    }
    if(HF_CloseFileScan(scanDesc)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan                                            
    }
    //Delete the record
    if(HF_DeleteRec(relfileDesc,recId1,sizeof(relDesc))!=HFE_OK){
       HF_PrintError("");
       return -1;//Error deleting the record
    }
    do{
      //Open a scan with value=relname (relation name)
      if((scanDesc=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,0,1,argv[1]))<0){
         HF_PrintError("");
         return -1;//Error opening the scan                                                                                
      }
      if((recId2=HF_FindNextRec(scanDesc,(char *)&attrmetadata))<0){
         break;//The field does not exist                                            
      }
      //If the field has an index
      if((attrmetadata.indexed)==1){
          sprintf(name,"%s.%d",attrmetadata.relname,attrmetadata.indexno);
          //Destroy the file name
          if(BF_DestroyFile(name)!=BFE_OK){
             BF_PrintError("");
             return -1;//Error destroying the file                                 
          }
      }
      if(HF_CloseFileScan(scanDesc)!=HFE_OK){
         HF_PrintError("");
         return -1;//Error closing the scan                                            
      }
      //Delete the record
      if(HF_DeleteRec(attrfileDesc,recId2,sizeof(attrDesc))!=HFE_OK){
         HF_PrintError("");
         return -1;//Error deleting the record
      }
    }while(1); //Will continue until FindNextRec returns negative i.e.
               //end of file
    //Destroy the file
    if(HF_DestroyFile(argv[1])!=HFE_OK){
       HF_PrintError("");
       return -1;//Error destroying the file                                                  
    }
    printf("Relation and all indexes were successfully destroyed\n");
    return 0;//All ok!!!
}
int UT_quit(int argc,char* argv[]){
    //Close the files
    if(HF_CloseFile(relfileDesc)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the file                                           
    }
    if(HF_CloseFile(attrfileDesc)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the file                                           
    }
    exit(1);
}
