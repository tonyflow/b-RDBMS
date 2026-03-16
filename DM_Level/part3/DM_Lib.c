#include <stdio.h>
#include "askisi.h"
#include "DM_Lib.h"
#include <math.h>

//Function used when an attribute is not a numeric type but a string.
//We take the first 3 characters and compute their "arithmetic" value as indicated
//by the specifications using base 256.
int calc(char *s){
    int mhkos=0,i,j=0,result,k=2; 
    
    if((mhkos=strlen(s))>=3){
        mhkos=3;                   
    }
    for(i=mhkos-1;i>=0;i--){
        result=pow((int)(*(s+j)),k);                            
        j++;
        k--;                    
    }
    return result;
}
//Function that performs comparisons between two values depending on their type (int, float, char).
//Used because values are passed as arguments to functions via a pointer to char.
//The flag is used in the case where for 'c' the calc function should be used.
int dm_mymemcmp(void *value1,void *value2,char attrType,int flag){
        switch(attrType){
                         case 'i':
                              if(*(int *)value1<*(int *)value2){
                                   return -1;
                              }
                              else if(*(int *)value1>*(int *)value2){
                                   return 1;
                              }
                              else{
                                   return 0;
                              }
                         case 'f':
                              if(*(float *)value1<*(float *)value2){
                                   return -1;
                              }
                              else if(*(float *)value1>*(float *)value2){
                                   return 1;
                              }
                              else{
                                   return 0;
                              }
                         case 'c':
                              if(flag==1){
                                 if(calc(value1)<calc(value2)){
                                    return -1;
                                 }
                                 else if(calc(value1)>calc(value2)){
                                    return 1;
                                 }
                                 else{
                                    return 0;
                                 }
                              }
                              else{
                                 return strcmp(value1,value2);     
                              }
        }
}
//Function that takes the operand and returns a number 1-6 as its representation
int operand(char *s){
    if(strcmp(s,"=")==0){
        return 1;
    }
    if(strcmp(s,"<")==0){
        return 2;
    }
    if(strcmp(s,">")==0){
        return 3;
    }
    if(strcmp(s,"<=")==0){
        return 4;
    }
    if(strcmp(s,">=")==0){
        return 5;
    }
    if(strcmp(s,"!=")==0){
        return 6;
    }
    return -1;
}
//Selectivity function that computes the relative size of the answer to a selection query.
//That is, the number of records, which varies depending on the operator.
float selectivity(attrDesc aD,int op,void *val){ 
    switch(op){
               case 1:
                    return 1/(aD.valuecnt);
               case 2: case 4:
                    if(strcmp(&(aD.attrtype),"i")==0){
                         return (atoi(val)-(*(int *)(aD.min)))/((*(int *)(aD.max))-(*(int *)(aD.min)));
                    }
                    if(strcmp(&(aD.attrtype),"c")==0){
                         return (*(int *)val-(*(int *)(aD.min)))/((*(int *)(aD.max))-(*(int *)(aD.min)));
                    }
                    if(strcmp(&(aD.attrtype),"f")==0){
                         return (atof(val)-(*(float *)(aD.min)))/((*(float *)(aD.max))-(*(float *)(aD.min)));        
                    }
               case 3: case 5:
                    if(strcmp(&(aD.attrtype),"i")==0){
                         return ((*(int *)(aD.max))-atoi(val))/((*(int *)(aD.max))-(*(int *)(aD.min)));
                    }
                    if(strcmp(&(aD.attrtype),"c")==0){
                         return ((*(int *)(aD.max))-*(int *)val)/((*(int *)(aD.max))-(*(int *)(aD.min)));
                    }
                    if(strcmp(&(aD.attrtype),"f")==0){
                         return ((*(float *)(aD.min))-atof(val))/((*(float *)(aD.max))-(*(float *)(aD.min)));        
                    }
               case 6:
                    return 1-(1/(aD.valuecnt));
    }        
}
int DM_insert(int argc,char* argv[]){
    int scanDesc,i,fd,fd2,sd,flagindex,flagmin,flagmax,fileD,RecId,recId,timh,pedioint;
    float pediofloat;
    char name[2*MAXNAME+sizeof(char)],name1[MAXNAME+sizeof(int)+sizeof(char)],*buffer;
    relDesc rD;
    attrDesc aD;
    
    //Check for correct argc
    if(argc<4 || argc%2!=0){
       printf("Invalid argc\n");
       return -1;//Invalid argc          
    }    
    //Check if the relation exists
    if((scanDesc=HF_OpenFileScan(relfileDesc,sizeof(relDesc),'c',MAXNAME,0,1,argv[1]))<0){
       HF_PrintError("");
       return -1;                                                                                
    }
    if((recId=HF_FindNextRec(scanDesc,(char *)&rD))<0){
       HF_PrintError("Invalid relation to insert : ");
       return -1;//The relation does not exist                                           
    }
    if(HF_CloseFileScan(scanDesc)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan                                            
    }
    //Memory allocation
    if((buffer=malloc(rD.relwidth))==NULL){
       return -1;
    }
    //Refresh
    //Delete the record
    if(HF_DeleteRec(relfileDesc,recId,sizeof(relDesc))!=HFE_OK){
       HF_PrintError("");
       return -1;//Error deleting the record
    }
    //Perform the necessary updates
    (rD.reccnt)+=1;      //increment the record count
    //Insert the new data into the files
    if(HF_InsertRec(relfileDesc,(char *)&rD,sizeof(relDesc))<0){
      HF_PrintError(""); 
      return -1;
    }
    //Check the correctness of the fields
    for(i=2;i<argc;i+=2){
        sprintf(name,"%s.%s",argv[1],argv[i]);
        if((scanDesc=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
           HF_PrintError("");
           return -1;                                                                                
        }
        if(HF_FindNextRec(scanDesc,(char *)&aD)<0){
           HF_PrintError("Invalid relation.attribute to insert : ");
           return -1;//The field does not exist                                            
        }
        //Depending on the record type, insert it into a buffer
        if(aD.attrtype=='c'){
           memcpy(buffer+(aD.offset),argv[i+1],aD.attrlength);
        }
        if(aD.attrtype=='i'){
           pedioint=atoi(argv[i+1]);  
           memcpy(buffer+(aD.offset),&pedioint,aD.attrlength);
        }
        if(aD.attrtype=='f'){
           pediofloat=atof(argv[i+1]);  
           memcpy(buffer+(aD.offset),&pediofloat,aD.attrlength);
        }
        if(HF_CloseFileScan(scanDesc)!=HFE_OK){
           HF_PrintError("");
           return -1;//Error closing the scan                                            
        }        
    }//Open the relation
    if((fileD=HF_OpenFile(argv[1]))<0){
       HF_PrintError(""); 
       return -1;//Error opening the file
    }//Insert the record by inserting the buffer
    if((RecId=HF_InsertRec(fileD,buffer,rD.relwidth))<0){
       HF_PrintError(""); 
       return -1;//Error inserting the record
    }
    if(HF_CloseFile(fileD)!=HFE_OK){
       HF_PrintError(""); 
       return -1;//Error closing
    }
    for(i=2;i<argc;i+=2){
        flagmin=0;
        flagmax=0;
        flagindex=0;
        sprintf(name,"%s.%s",argv[1],argv[i]);
        //Check if the relation.field exists
        if((scanDesc=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
           HF_PrintError("");
           return -1;//Error opening the scan                                                                                
        }
        if((recId=HF_FindNextRec(scanDesc,(char *)&aD))<0){
           HF_PrintError("Invalid relation.attribute to insert : ");
           return -1;//The field does not exist                                            
        }
        if(HF_CloseFileScan(scanDesc)!=HFE_OK){
           HF_PrintError("");
           return -1;//Error closing the scan                                            
        }
        //Check for min and max
        //Update min and max with the first record to be inserted
        if(aD.valuecnt==0){
           flagmin=1;
           flagmax=1;
        }
        //compare record with min, if it is smaller then min must be updated
        else if(dm_mymemcmp(buffer+(aD.offset),aD.min,aD.attrtype,1)==-1){
           flagmin=1;
        }
        //compare record with max, if it is larger then max must be updated
        else if(dm_mymemcmp(buffer+(aD.offset),aD.max,aD.attrtype,1)==1){
           flagmax=1;
        }
        //If the field has an index
        if((aD.indexed)==1){
           sprintf(name1,"%s.%d",argv[1],aD.indexno);
           //Open the indexed file
           if((fd2=BF_OpenFile(name1))<0){
              BF_PrintError(""); 
              return -1;//Error opening the file
           }
           //Check if the value exists
           if((sd=AM_OpenIndexScan(fd2,aD.attrtype,aD.attrlength,1,buffer+(aD.offset)))<0){
              AM_PrintError(""); 
              return -1;//Error opening the scan
           }
           if(AM_FindNextEntry(sd)>=0){
                 flagindex=1;
           }
           //Insert the record into the index
           if((AM_InsertEntry(fd2,aD.attrtype,aD.attrlength,buffer+(aD.offset),RecId))!=AME_OK){
               AM_PrintError("");
               return -1;//Error inserting the record into the index
           }
           if(AM_CloseIndexScan(sd)!=AME_OK){
              AM_PrintError("");   
              return -1;//Error closing the scan                                            
           }
           if(BF_CloseFile(fd2)!=BFE_OK){
              BF_PrintError("");
              return -1;//Error closing the file
           }
        }
        //If there is no identical value or it is smaller than min or larger than max
        if( flagindex==0 || flagmin==1 || flagmax==1 ){
            if(HF_DeleteRec(attrfileDesc,recId,sizeof(attrDesc))!=HFE_OK){
               HF_PrintError("");
               return -1;//Error deleting the record
            }
            //Perform the necessary updates
            if(flagindex==0){           //If the field is indexed, increment valuecnt
               (aD.valuecnt)+=1;
            }
            if(flagmin==1){             //Prepei na enhmerwsw to min
               if(strcmp(&(aD.attrtype),"i")==0 || strcmp(&(aD.attrtype),"f")==0){            
                  memcpy(aD.min,buffer+(aD.offset),aD.attrlength);
               }
               //use calc if we have a string
               else{
                  timh=calc(argv[i+1]);  
                  memcpy(aD.min,&timh,sizeof(int));
               }
            }
            if(flagmax==1){              //Prepei na enhmerwsw to max
               if(strcmp(&(aD.attrtype),"i")==0 || strcmp(&(aD.attrtype),"f")==0){            
                  memcpy(aD.max,buffer+(aD.offset),aD.attrlength);
               }
               //use calc if we have a string
               else{
                  timh=calc(argv[i+1]);
                  memcpy(aD.max,&timh,sizeof(int));
               }
            }            
            //Insert the new field data into the fields table
            if(HF_InsertRec(attrfileDesc,(char *)&aD,sizeof(attrDesc))<0){
               HF_PrintError("");
               return -1;
            }
        }
    }
    //Free memory
    free(buffer);
    return 0;
}
int DM_delete(int argc,char* argv[]){
    int scanDesc1,scanDesc2,scanDesc3,recId1,recId2,recId3,fileDesc,fileDesc1,i,k,pedioint,sunolo=0;    
    char name[2*MAXNAME+sizeof(char)],name1[MAXNAME+sizeof(int)+sizeof(char)],*s,*timi;
    float pediofloat;
    relDesc rD;
    attrDesc aD;
    
    //Check for correct argc
    if(argc!=2 && argc!=5){
       printf("Invalid argc\n");
       return -1;//Invalid number of arguments           
    }
    //Check if the relation exists
    if((scanDesc1=HF_OpenFileScan(relfileDesc,sizeof(relDesc),'c',MAXNAME,0,1,argv[1]))<0){
       HF_PrintError("");
       return -1;                                                                                
    }
    if((recId1=HF_FindNextRec(scanDesc1,(char *)&rD))<0){
       HF_PrintError("Invalid relation to delete : ");
       return -1;//The relation does not exist                                           
    }
    if(HF_CloseFileScan(scanDesc1)!=HFE_OK){
      HF_PrintError("");
      return -1;//Error closing the scan                                            
    }
    //Memory allocation
    if((s=malloc(rD.relwidth))==NULL){
       return -1;
    }
    //Refresh
    //Delete the record
    if(HF_DeleteRec(relfileDesc,recId1,sizeof(relDesc))!=HFE_OK){
       HF_PrintError("");
       return -1;//Error deleting the record
    }
    //No where clause -> simple case
    if(argc==2){
        //Open the file with the relation name
        if((fileDesc=HF_OpenFile(argv[1]))<0){
           HF_PrintError("");
           return -1;//Error opening the relation
        }
        while(1){
          //Open a scan with NULL
          if((scanDesc3=HF_OpenFileScan(fileDesc,rD.relwidth,aD.attrtype,aD.attrlength,aD.offset,1,NULL))<0){
              HF_PrintError("");
              return -1;//Error opening the scan                                                                                
          }                        
          if((recId3=HF_FindNextRec(scanDesc3,s))<0){
             if(HF_CloseFileScan(scanDesc3)!=HFE_OK){
                HF_PrintError("");
                return -1;//Error closing the scan                                            
             }
             break;//an teleiwsoun oi eggrafes pou epistrefei i FindNextRec
          }
          if(HF_CloseFileScan(scanDesc3)!=HFE_OK){
             HF_PrintError("");
             return -1;//Error closing the scan                                            
          }          
          if(HF_DeleteRec(fileDesc,recId3,rD.relwidth)!=HFE_OK){
             HF_PrintError("");
             return -1;//Error deleting the record
          }
          //For all fields of the relation                                        
          for(i=0;i<(rD.attrcnt);i++){
             if((scanDesc2=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,0,1,argv[1]))<0){
                HF_PrintError("");
                return -1;//Error opening the scan                                                                                
             }
             //Since HF does not support scan and simultaneous deletion, we use i as an
             //indicator of which record we had reached
             k=0;
             while(k<=i){                             
                if((recId2=HF_FindNextRec(scanDesc2,(char *)&aD))<0){
                   HF_PrintError("");
                   return -1;//The field does not exist                                            
                }
		        k++;
             }
             if(HF_CloseFileScan(scanDesc2)!=HFE_OK){
                HF_PrintError("");
                return -1;//Error closing the scan                                            
             }
             //If the field has no index            
             if((aD.indexed)==0){
                (aD.valuecnt)--;
                if(HF_DeleteRec(attrfileDesc,recId2,sizeof(attrDesc))!=HFE_OK){
                   HF_PrintError("");
                   return -1;//Error deleting the record
                }
                if(HF_InsertRec(attrfileDesc,(char *)&aD,sizeof(attrDesc))<0){
                   HF_PrintError("");
                   return -1;
                }
             }
             //If the field has an index                         
             if((aD.indexed)==1){
                sprintf(name1,"%s.%d",argv[1],aD.indexno);
                if((fileDesc1=BF_OpenFile(name1))<0){
                   BF_PrintError("");
                   return -1;//Error opening the file
                }
                if(AM_DeleteEntry(fileDesc1,aD.attrtype,aD.attrlength,s+(aD.offset),recId3)!=AME_OK){
                   AM_PrintError("");
                   return -1;//Error deleting the record from the index                                                                                    
                }                      
                if(BF_CloseFile(fileDesc1)!=BFE_OK){
                   BF_PrintError("");
                   return -1;//Error closing the file
                }                   
             }
          }
          (rD.reccnt)--;//Decrement the record count
          sunolo++;                                                      
        }
        if(HF_CloseFile(fileDesc)!=HFE_OK){
           HF_PrintError("");
           return -1;//Error opening the relation
        }
        //Update the record
        if(HF_InsertRec(relfileDesc,(char *)&rD,sizeof(relDesc))<0){
           HF_PrintError("");
           return -1;//Error inserting the record
        }
    }
    //Have where clause -> complex case
    else{//Check if the given field exists
        sprintf(name,"%s.%s",argv[1],argv[2]);
        if((scanDesc2=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
           HF_PrintError("");
           return -1;//Error opening the scan                                                                                
        }
        if((recId2=HF_FindNextRec(scanDesc2,(char *)&aD))<0){
           HF_PrintError("Invalid relation.attribute to delete : ");
           return -1;//The field does not exist                                            
        }
        if(HF_CloseFileScan(scanDesc2)!=HFE_OK){
           HF_PrintError("");
           return -1;//Error closing the scan                                            
        }
        //If the field has no index            
        if((aD.indexed)==0){
           (aD.valuecnt)--;
           if(HF_DeleteRec(attrfileDesc,recId2,sizeof(attrDesc))!=HFE_OK){
              HF_PrintError("");
              return -1;//Error deleting the record
           }
           if(HF_InsertRec(attrfileDesc,(char *)&aD,sizeof(attrDesc))<0){
              HF_PrintError("");
              return -1;//Error inserting the record
           }
        }
        if((fileDesc=HF_OpenFile(argv[1]))<0){
           HF_PrintError("");
           return -1;//Error opening the relation
        }
        //Depending on the format of each field, get its value for the scan
        if(aD.attrtype=='c'){
           timi=malloc(aD.attrlength);
           memcpy(timi,argv[4],aD.attrlength);
        }
        if(aD.attrtype=='i'){
           timi=malloc(aD.attrlength);
           pedioint=atoi(argv[4]);
           memcpy(timi,&pedioint,aD.attrlength);
        }
        if(aD.attrtype=='f'){
           timi=malloc(aD.attrlength);
           pediofloat=atoi(argv[4]);
           memcpy(timi,&pediofloat,aD.attrlength);
        }
        while(1){
          if((scanDesc3=HF_OpenFileScan(fileDesc,rD.relwidth,aD.attrtype,aD.attrlength,aD.offset,operand(argv[3]),timi))<0){
             HF_PrintError("");
             return -1;//Error opening the scan                                                                                
          }
          if((recId3=HF_FindNextRec(scanDesc3,s))<0){
              if(HF_CloseFileScan(scanDesc3)!=HFE_OK){
                 HF_PrintError("");
                 return -1;//Error closing the scan                                            
              }
              break;
          }
          if(HF_CloseFileScan(scanDesc3)!=HFE_OK){
             HF_PrintError("");
             return -1;//Error closing the scan                                            
          }
          //Delete the record
          if(HF_DeleteRec(fileDesc,recId3,rD.relwidth)!=HFE_OK){
             HF_PrintError("");
             return -1;//Error deleting the record
          }
          //If the field has an index
          if((aD.indexed)==1){
             sprintf(name1,"%s.%d",argv[1],aD.indexno);
             if((fileDesc1=BF_OpenFile(name1))<0){
                BF_PrintError(""); 
                return -1;
             }
             //Delete the record from the index
             if(AM_DeleteEntry(fileDesc1,aD.attrtype,aD.attrlength,s+(aD.offset),recId3)!=AME_OK){
                AM_PrintError("");
                return -1;                                                                                    
             }                      
             if(BF_CloseFile(fileDesc1)!=BFE_OK){
                BF_PrintError(""); 
                return -1;
             }                   
          }
          (rD.reccnt)--;//Decrement the record count
          sunolo++;                                                    
        }
        if(HF_CloseFile(fileDesc)!=HFE_OK){
           HF_PrintError("");
           return -1;//Error opening the relation
        }
        //Update the record
        if(HF_InsertRec(relfileDesc,(char *)&rD,sizeof(relDesc))<0){
           HF_PrintError("");
           return -1;//Error inserting the record
        }
        free(timi); 
    }
    //Free memory
    free(s);
    if(sunolo==1){
       printf("\n%d rec was successfully deleted\n",sunolo);              
    }
    else{
        printf("\n%d recs were successfully deleted\n",sunolo);
    }
    return 0;
}
int DM_select(int argc,char* argv[]){
    int scanDesc,scanDesc1,scanDesc2,recId,recId1,recId2,N,i,j,fileDesc,fileDesc1,intofileDesc,intoargc,intoscanDesc,k;
    int intoflag=0,whereflag=0,attrlength,offset,op,argument=0,indexed=0,shift=0,pedioint,sunolo=0,insertargc;
    float pediofloat;
    char *s,name[2*MAXNAME+2*sizeof(char)],**intoargv,attrtype,**insertargv;
    relDesc rD;
    attrDesc aD;
    void *value;
    
    //Check if a temporary table exists
    if(argc%2!=0){
       shift=0;
       intoflag=1;
       //N=number of projected attributes
       N=atoi(argv[2]);
       for(i=3;i<2*N+3;i+=2){
        if(strcmp(argv[3],argv[i])!=0){
           printf("Relation to project differs from the others\n");
           return -1;
         }
       }
       //Build the arguments that UT_Create will receive
       intoargc=2+2*N;
       if((intoargv=malloc((intoargc+1)*sizeof(char *)))==NULL){
           return -1;
       }
       for(j=0;j<=(intoargc-1);j++){//
           if((intoargv[j]=malloc(MAXNAME*sizeof(char)))==NULL){
               return -1;                                            
           }                        
       }
       strcpy(intoargv[0],"create");
       strcpy(intoargv[1],argv[1]);
       for(k=2;k<=N*2;k+=2){
          sprintf(name,"%s.%s",argv[k+1],argv[k+2]);                             
          if((intoscanDesc=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
             HF_PrintError("");
             return -1;                                                                                
          }                             
          if(HF_FindNextRec(intoscanDesc,(char *)&aD)<0){
             HF_PrintError("Invalid relation.attribute to project (into) : ");
             return -1;//The field does not exist                                            
          }
          sscanf(aD.attrname,"%*[^'.'].%s",intoargv[k]);
          if(aD.attrtype=='c'){
             sprintf(intoargv[k+1],"%s%c%d%s","'",(aD.attrtype),(aD.attrlength),"'");
          }  
          else{
             sprintf(intoargv[k+1],"%s%c%s","'",(aD.attrtype),"'");
          } 
          if(HF_CloseFileScan(intoscanDesc)!=HFE_OK){
             HF_PrintError("");
             return -1;//Error closing the scan                                            
          }
       }
       intoargv[intoargc]=NULL;
       //Create the temporary table with the appropriate arguments we have provided
       if(UT_create(intoargc,intoargv)<0){
         printf("Temporary table couldn't be created\n");
         return -1;
       }
       insertargc=2+2*N;
       if((insertargv=malloc((insertargc+1)*sizeof(char *)))==NULL){
           return -1;
       }
       for(j=0;j<=(insertargc-1);j++){//
           if((insertargv[j]=malloc(MAXNAME*sizeof(char)))==NULL){
               return -1;                                            
           }                        
       }
       strcpy(insertargv[0],"insert");
       strcpy(insertargv[1],argv[1]);
       insertargv[insertargc]=NULL;
       
    }
    else{                  //no temporary table
       shift=-1;
       //N=number of projected attributes
       N=atoi(argv[1]);
       for(i=3+shift;i<2*N+shift+3;i+=2){
        if(strcmp(argv[3+shift],argv[i])!=0){
           printf("Relation to project differs from the others\n");
           return -1;
        }
       }
    }
    //Check arguments
    if(N*2+3+shift!=argc){//
       if(N*2+3+shift+4!=argc){//
          printf("Invalid argc\n");
          return -1;//invalid arguments
       }
       else{
            whereflag=1; //we also have a where clause
            if(strcmp(argv[3+shift],argv[argc-4])!=0){
               printf("Relation to select differs from the one to project\n");
               return -1;                                          
            }
       }
    }
    
    //Check if the relation exists
    if((scanDesc1=HF_OpenFileScan(relfileDesc,sizeof(relDesc),'c',MAXNAME,0,1,argv[3+shift]))<0){
       HF_PrintError("");
       return -1;                                                                                
    }
    if((recId1=HF_FindNextRec(scanDesc1,(char *)&rD))<0){
       HF_PrintError("Invalid relation to project : ");
       return -1;//The relation does not exist                                           
    }
    if(HF_CloseFileScan(scanDesc1)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan  
    }
    //Check if the given relation.fields exist
    for(i=3+shift;i<2*N+3+shift;i+=2){
        sprintf(name,"%s.%s",argv[i],argv[i+1]);
        if((scanDesc2=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
          HF_PrintError(" ");
          return -1;                                                                                
        }
        if((recId2=HF_FindNextRec(scanDesc2,(char *)&aD))<0){
          HF_PrintError("Invalid relation.attribute to project : ");
          return -1;//The field does not exist                                            
        }
        if(HF_CloseFileScan(scanDesc2)!=HFE_OK){
          HF_PrintError("");
          return -1;//Error closing the scan                                            
        }
    }
    //An den exw where
    if(whereflag==0){
      argument=argc;
      //Open the relation
      if((fileDesc=HF_OpenFile(argv[3+shift]))<0){
        HF_PrintError("");
        return -1;//Error opening the relation
      }
      //Open scan with null to return all records of the relation specified in TARGET_LIST
      if((scanDesc2=HF_OpenFileScan(fileDesc,rD.relwidth,'c',MAXNAME,MAXNAME,1,NULL))<0){
        HF_PrintError("");
        return -1;                                                                                
      }
    }//If we have a where clause
    else{
      argument=argc-4;
      //Check if the selection field exists
      sprintf(name,"%s.%s",argv[argc-4],argv[argc-3]);//
      if((scanDesc2=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
          HF_PrintError("");
          return -1;                                                                                
      }
      if((recId2=HF_FindNextRec(scanDesc2,(char *)&aD))<0){
          HF_PrintError("Invalid relation.attribute to select with condition : ");
          return -1;//The field does not exist                                            
      }
      memcpy(&attrlength,&(aD.attrlength),sizeof(int));
      memcpy(&attrtype,&(aD.attrtype),sizeof(char));
      memcpy(&offset,&(aD.offset),sizeof(int));
      op=operand(argv[argc-2]);
      memcpy(&indexed,&(aD.indexed),sizeof(int));
      if((value=malloc(attrlength))==NULL){
        return -1;                                     
      } 
      if(aD.attrtype=='c'){
           memcpy(value,argv[argc-1],aD.attrlength);
      }
      if(aD.attrtype=='i'){
           pedioint=atoi(argv[argc-1]);  
           memcpy(value,&pedioint,aD.attrlength);
      }
      if(aD.attrtype=='f'){
           pediofloat=atof(argv[argc-1]);  
           memcpy(value,&pediofloat,aD.attrlength);
      }    
      if(HF_CloseFileScan(scanDesc2)!=HFE_OK){
          HF_PrintError("");
          return -1;//Error closing the scan                                            
      }
      //Open scan with appropriate arguments to return all records of the relation specified in TARGET_LIST
      //that satisfy the condition
      //If the relation.field has no index
      if(indexed==0){
         //Open the selection relation
         if((fileDesc=HF_OpenFile(argv[argc-4]))<0){//
            HF_PrintError("");
            return -1;//Error opening the relation
         }            
         if((scanDesc2=HF_OpenFileScan(fileDesc,rD.relwidth,attrtype,attrlength,offset,op,value))<0){
            HF_PrintError("");
            return -1;                                                                                
         }
      }
      else{//If the relation.field has an index
         //Open the selection relation
         if((fileDesc=HF_OpenFile(argv[argc-4]))<0){//
            HF_PrintError("");
            return -1;//Error opening the relation
         }
         sprintf(name,"%s.%d",argv[argc-4],aD.indexno);
         if((fileDesc1=BF_OpenFile(name))<0){//
              BF_PrintError("");
              return -1;//Error opening the relation
         }
         if((scanDesc2=AM_OpenIndexScan(fileDesc1,attrtype,attrlength,op,value))<0){
              AM_PrintError("");
              return -1;
         }  
      }      
    }
    if((s=malloc(rD.relwidth))==NULL){
       return -1;
    }//Print output
    printf("\nSELECT\n");
    for(i=0;i<N;i++){
       printf("____________________________");
    }
    printf("\n");
    for(i=3+shift;i<argument;i+=2){
        sprintf(name,"%s.%s",argv[i],argv[i+1]);
        printf("|%-27s",name);             
    }
    printf("|\n");  
    while(1){
      
      if(whereflag==0 || indexed==0){
         if(HF_FindNextRec(scanDesc2,s)<0){
            break;
         }
      }
      else{
         if((recId=AM_FindNextEntry(scanDesc2))==AME_EOF){
            break;
         }
         if(HF_GetThisRec(fileDesc,recId,s,rD.relwidth)!=HFE_OK){
            HF_PrintError("");
            return -1;                                                        
         }
      }
      sunolo++;                                               
      for(i=3+shift;i<argument;i+=2){
        sprintf(name,"%s.%s",argv[i],argv[i+1]);
        if((scanDesc=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
          HF_PrintError("");
          return -1;                                                                                
        }
        if((recId2=HF_FindNextRec(scanDesc,(char *)&aD))<0){
          HF_PrintError("");
          return -1;//The field does not exist                                            
        }
        if(i==3+shift){
          printf("|");
        }
        switch(aD.attrtype){
                   case 'i':
                        printf("%-27d|",*(int *)(s+(aD.offset)));
                        if(intoflag==1){
                            strcpy(insertargv[i-1],argv[i+1]);
                            sprintf(insertargv[i],"%d",*(int *)(s+(aD.offset)));
                        }
                        break;
                   case 'f':
                        printf("%-27f|",*(float *)(s+(aD.offset)));
                        if(intoflag==1){
                            strcpy(insertargv[i-1],argv[i+1]);
                            sprintf(insertargv[i],"%f",*(float *)(s+(aD.offset)));
                        }
                        break;
                   case 'c':
                        printf("%-27s|",s+(aD.offset));
                        if(intoflag==1){
                            strcpy(insertargv[i-1],argv[i+1]);
                            strcpy(insertargv[i],s+(aD.offset));
                        }
                        break;
        }
        if(HF_CloseFileScan(scanDesc)!=HFE_OK){
          HF_PrintError("");
          return -1;//Error closing the scan                                            
        }
      }
      printf("\n");
      //If we have a temporary table (INTO)
      if(intoflag==1){
        if(DM_insert(insertargc,insertargv)<0){
           return -1;
        }
      }
    }//while
    printf("|");
    for(i=0;i<N;i++){
       printf("___________________________|");
    }
    if(sunolo==1){
       printf("\n%d rec was successfully selected\n",sunolo);              
    }
    else{
       printf("\n%d recs were successfully selected\n",sunolo);
    }
    if(whereflag==0){
      if(HF_CloseFileScan(scanDesc2)!=HFE_OK){
        HF_PrintError("");
        return -1;                                                                                
      }
    }
    else{//An exw where
      if(indexed==0){
        if(HF_CloseFileScan(scanDesc2)!=HFE_OK){
          HF_PrintError("");
          return -1;//Error closing the scan                                            
        }
      }
      else{
        if(AM_CloseIndexScan(scanDesc2)!=AME_OK){
          AM_PrintError("");
          return -1;//Error closing the scan                                            
        }                                               //Bug in the AM module: when opening a scan with < or <= in AM, it probably does not
        /*if(BF_CloseFile(fileDesc1)!=BFE_OK){          //unpin some block and cannot close the file.
          BF_PrintError("");                            //The BF_CloseFile call fails with message block pinned in memory.
          return -11;//Error opening the relation
        }*/
      }
    }
    if(HF_CloseFile(fileDesc)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error opening the relation
    }
    if(whereflag==1){
      free(value);                 
    }
    if(intoflag==1){
       for(j=0;j<intoargc;j++){
         free(intoargv[j]);                    
       }
       free(intoargv);
       for(j=0;j<insertargc;j++){
         free(insertargv[j]);                    
       }
       free(insertargv);
    }
    return 0;
}
int DM_join(int argc,char* argv[]){
    int scanDesc,scanDesc1,scanDesc2,scanDesc3,scanDesc4,scanDesc5,scanDesc6,N,i,j,k,fileDesc1,fileDesc2;
    int intoflag=0,attrlength,offset,op,Rrelwidth,Srelwidth,Slength,Soffset,Roffset,intofileDesc,intoargc,intoscanDesc;
    int fD1,fD2,recId1,recId2,shift=0,Rindexno,Sindexno,sunolo=0,insertargc; 
    relDesc rD1,rD2;
    attrDesc aD1,aD2,aD;
    char *s,*s1,*s2,name[2*MAXNAME+sizeof(char)],Rname[MAXNAME],Sname[MAXNAME],Stype,**intoargv,**insertargv,attrtype;
    
    //Check if a temporary table exists
    if(argc%2==0){
       shift=0;
       intoflag=1;
       //N=number of projected attributes
       N=atoi(argv[2]);
       for(i=3;i<2*N+3;i+=2){
           if(strcmp(argv[i],argv[argc-5])!=0 && strcmp(argv[i],argv[argc-2])!=0){
               printf("Relation to join differs from the ones to project\n");
               return -1;
           }
       }
       //Build the arguments that UT_Create will receive
       intoargc=2+2*N;
       if((intoargv=malloc((intoargc+1)*sizeof(char *)))==NULL){
           return -1;
       }
       for(j=0;j<=(intoargc-1);j++){//
           if((intoargv[j]=malloc(MAXNAME*sizeof(char)))==NULL){
               return -1;                                            
           }                        
       }
       strcpy(intoargv[0],"create");
       strcpy(intoargv[1],argv[1]);
       for(k=2;k<=N*2;k+=2){
          sprintf(name,"%s.%s",argv[k+1],argv[k+2]);                             
          if((intoscanDesc=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
              HF_PrintError("");
              return -1;                                                                                
          }                             
          if(HF_FindNextRec(intoscanDesc,(char *)&aD)<0){
             HF_PrintError("Invalid relation.attribute to project (into) : ");
             return -1;//The field does not exist                                            
          }
          sscanf(aD.attrname,"%*[^'.'].%s",intoargv[k]);
          if(aD.attrtype=='c'){
             sprintf(intoargv[k+1],"%s%c%d%s","'",aD.attrtype,aD.attrlength,"'");
          }  
          else{
             sprintf(intoargv[k+1],"%s%c%s","'",aD.attrtype,"'");
          }  
          if(HF_CloseFileScan(intoscanDesc)!=HFE_OK){
             HF_PrintError("");
             return -1;//Error closing the scan                                            
          }
       }
       intoargv[intoargc]=NULL;
       //Create the temporary table with the appropriate arguments we have provided
       if(UT_create(intoargc,intoargv)<0){
         printf("Temporary table couldn't be created\n");
         return -1;
       }
       insertargc=2+2*N;
       if((insertargv=malloc((insertargc+1)*sizeof(char *)))==NULL){
           return -1;
       }
       for(j=0;j<=(insertargc-1);j++){//
           if((insertargv[j]=malloc(MAXNAME*sizeof(char)))==NULL){
               return -1;                                            
           }                        
       }
       strcpy(insertargv[0],"insert");
       strcpy(insertargv[1],argv[1]);
       insertargv[insertargc]=NULL;
    }
    else{                  //no temporary table
       shift=-1;
       //N=number of projected attributes
       N=atoi(argv[1]);
       for(i=3+shift;i<2*N+3+shift;i+=2){
           if(strcmp(argv[i],argv[argc-5])!=0 && strcmp(argv[i],argv[argc-2])!=0){
               printf("Relation to join differs from the ones to project\n");
               return -1;
           }
       }
    }
    //Check arguments
    if(N*2+3+shift+5!=argc){
       printf("Invalid argc\n");
       return -1;//invalid arguments
    }//Check if the given relation.fields exist
    for(i=3+shift;i<2*N+3+shift;i+=2){
        sprintf(name,"%s.%s",argv[i],argv[i+1]);
        if((scanDesc=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
            HF_PrintError("");
            return -1;                                                                                
        }
        if(HF_FindNextRec(scanDesc,(char *)&aD)<0){
           HF_PrintError("Invalid relation.attribute to project : ");
           return -1;//The field does not exist                                            
        }
        if(HF_CloseFileScan(scanDesc)!=HFE_OK){
           HF_PrintError("");
           return -1;//Error closing the scan                                            
        }
    }
    //Check if the relations to be joined exist
    if((scanDesc1=HF_OpenFileScan(relfileDesc,sizeof(relDesc),'c',MAXNAME,0,1,argv[argc-5]))<0){//
        HF_PrintError("");
        return -1;                                                                                
    }
    if(HF_FindNextRec(scanDesc1,(char *)&rD1)<0){
       HF_PrintError("Invalid relation to join : ");
       return -1;//The relation does not exist                                           
    }
    if(HF_CloseFileScan(scanDesc1)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan                                            
    }
    if((scanDesc2=HF_OpenFileScan(relfileDesc,sizeof(relDesc),'c',MAXNAME,0,1,argv[argc-2]))<0){//
        HF_PrintError("");
        return -1;                                                                                
    }
    if(HF_FindNextRec(scanDesc2,(char *)&rD2)<0){
       HF_PrintError("Invalid relation to join : ");
       return -1;//The relation does not exist                                           
    }
    if(HF_CloseFileScan(scanDesc2)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan                                            
    }
    //Check if the relation.fields to be joined exist
    //First relation.field
    sprintf(name,"%s.%s",argv[argc-5],argv[argc-4]);//
    if((scanDesc3=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
        HF_PrintError("");
        return -1;                                                                                
    }
    if(HF_FindNextRec(scanDesc3,(char *)&aD1)<0){
       HF_PrintError("Invalid relation.attribute to join : ");
       HF_PrintError("");
       return -1;//The field does not exist                                            
    }
    if(HF_CloseFileScan(scanDesc3)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan                                            
    }
    //Second relation.field
    sprintf(name,"%s.%s",argv[argc-2],argv[argc-1]);//
    if((scanDesc4=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
        HF_PrintError("");
        return -1;                                                                                
    }
    if(HF_FindNextRec(scanDesc4,(char *)&aD2)<0){
       HF_PrintError("Invalid relation.attribute to join : ");
       return -1;//The field does not exist                                            
    }
    if(HF_CloseFileScan(scanDesc4)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan                                            
    }
    //If neither relation.field has an index or if both relation.fields have indexes
    if(( (aD1.indexed)==0 && (aD2.indexed)==0 ) || ( (aD1.indexed)==1 && (aD2.indexed)==1 )){
      //Based on reccnt, choose which relation will be outer and which inner
      if((rD1.reccnt)-(rD2.reccnt)<=0){
        strcpy(Rname,argv[argc-5]);//       
        strcpy(Sname,argv[argc-2]);//
        Rrelwidth=rD1.relwidth;
        Rindexno=aD1.indexno;
        Roffset=aD1.offset;
        Srelwidth=rD2.relwidth;
        Sindexno=aD2.indexno;
        Stype=aD2.attrtype;
        Slength=aD2.attrlength;
        Soffset=aD2.offset;
        op=operand(argv[argc-3]);
      }
      else{
        strcpy(Rname,argv[argc-2]);//
        strcpy(Sname,argv[argc-5]);//
        Rrelwidth=rD2.relwidth;
        Rindexno=aD2.indexno;
        Roffset=aD2.offset;
        Sindexno=aD1.indexno;
        Srelwidth=rD1.relwidth;
        Stype=aD1.attrtype;
        Slength=aD1.attrlength;
        Soffset=aD1.offset;
        op=operand(argv[argc-3]);
      }
    }//Otherwise the only relation that has an index becomes the inner one
    else{
       if(aD2.indexed==1){ 
        strcpy(Rname,argv[argc-5]);//       
        strcpy(Sname,argv[argc-2]);//
        Rrelwidth=rD1.relwidth;
        Rindexno=aD1.indexno;
        Roffset=aD1.offset;
        Sindexno=aD2.indexno;
        Srelwidth=rD2.relwidth;
        Stype=aD2.attrtype;
        Slength=aD2.attrlength;
        Soffset=aD2.offset;
        op=operand(argv[argc-3]);
       } 
       else{
        strcpy(Rname,argv[argc-2]);//
        strcpy(Sname,argv[argc-5]);//
        Rrelwidth=rD2.relwidth;
        Rindexno=aD2.indexno;
        Roffset=aD2.offset;
        Sindexno=aD1.indexno;
        Srelwidth=rD1.relwidth;
        Stype=aD1.attrtype;
        Slength=aD1.attrlength;
        Soffset=aD1.offset;
        op=operand(argv[argc-3]);
       }  
    }//Open the relations
    if((fileDesc1=HF_OpenFile(Rname))<0){
        HF_PrintError("");
        return -1;//Error opening the relation
    }
    if((fileDesc2=HF_OpenFile(Sname))<0){
        HF_PrintError("");
        return -1;//Error opening the relation
    }
    if((aD1.indexed)==1 && (aD2.indexed)==1){
       sprintf(name,"%s.%d",Rname,Rindexno);
       if((fD1=BF_OpenFile(name))<0){
           BF_PrintError("");
           return -1;//Error opening the relation
       }
       sprintf(name,"%s.%d",Sname,Sindexno);
       if((fD2=BF_OpenFile(name))<0){
           BF_PrintError("");
           return -1;//Error opening the relation
       }                  
    }
    else if(( (aD1.indexed)==1 && (aD2.indexed)==0 ) || ( (aD1.indexed)==0 && (aD2.indexed)==1 )){
       sprintf(name,"%s.%d",Sname,Sindexno);
       if((fD1=BF_OpenFile(name))<0){
           BF_PrintError("");
           return -1;//Error opening the relation
       }  
    }
    //I have no index or I have 2 indexes
    if(((aD1.indexed)==0 && (aD2.indexed)==0) || ( (aD1.indexed)==1 && (aD2.indexed)==0 ) || ( (aD1.indexed)==0 && (aD2.indexed)==1 )){
         if((scanDesc5=HF_OpenFileScan(fileDesc1,Rrelwidth,'c',MAXNAME,MAXNAME,1,NULL))<0){
             HF_PrintError("");
             return -1;                                                                                
         }
    }
    else{         
        if((scanDesc5=AM_OpenIndexScan(fD1,'c',Rrelwidth,1,NULL))<0){
            AM_PrintError("");
            return -1;                                                                                
        } 
    }
    if((s1=malloc(Rrelwidth))==NULL){
        return -1;
    }
    if((s2=malloc(Srelwidth))==NULL){
        return -1;
    }
    printf("\n\nJOIN\n");
    for(i=0;i<N;i++){
        printf("____________________________");
    }
    printf("\n");
    for(i=3+shift;i<argc-5;i+=2){
        sprintf(name,"%s.%s",argv[i],argv[i+1]);
        printf("|%-27s",name);   
    }
    printf("|\n");
    //Nested loops algorithm
    while(1){
          if((aD1.indexed)==0 && (aD2.indexed)==0){
            if(HF_FindNextRec(scanDesc5,s1)<0){ 
               break;                                       
            }    
            if((scanDesc6=HF_OpenFileScan(fileDesc2,Srelwidth,Stype,Slength,Soffset,op,s1+Roffset))<0){
                HF_PrintError("");
                return -1;                                                                                
            }
          }
          else if((aD1.indexed)==1 && (aD2.indexed)==1){
               if((recId1=AM_FindNextEntry(scanDesc5))<0){
                   break;                                       
               }
               if(HF_GetThisRec(fileDesc1,recId1,s1,Rrelwidth)!=HFE_OK){
                  HF_PrintError("");
                  return -1;
               }
               if((scanDesc6=AM_OpenIndexScan(fD2,Stype,Slength,op,s1+Roffset))<0){
                   AM_PrintError("");
                   return -1;                                                                                
               } 
          }
          else{
              if((recId1=HF_FindNextRec(scanDesc5,s1))<0){
                  break;                                       
              }
              if((scanDesc6=AM_OpenIndexScan(fD1,Stype,Slength,op,s1+Roffset))<0){
                  AM_PrintError("");
                  return -1;                                                                                
              }
          }        
          while(1){
              if((aD1.indexed)==0 && (aD2.indexed)==0){
                 if(HF_FindNextRec(scanDesc6,s2)<0){
                    break;                                           
                 }
              }
              else{
                if((recId2=AM_FindNextEntry(scanDesc6))<0){
                  break;                                       
                }
                if(HF_GetThisRec(fileDesc2,recId2,s2,Srelwidth)!=HFE_OK){
                   HF_PrintError("");
                   return -1;
                }  
              }
              sunolo++;       
              for(i=3+shift;i<argc-5;i+=2){
                 if(strcmp(argv[i],Rname)==0){
                      s=s1;
                 }
                 else{
                      s=s2;
                 }                   
                 sprintf(name,"%s.%s",argv[i],argv[i+1]);
                 if((scanDesc=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
                     HF_PrintError("");
                     return -1;                                                                                
                 }
                 if(HF_FindNextRec(scanDesc,(char *)&aD)<0){
                    HF_PrintError("");
                    return -1;//The field does not exist                                            
                 }
                 if(i==3+shift){
                    printf("|");
                 }
                 switch(aD.attrtype){
                   case 'i':
                        printf("%-27d|",*(int *)(s+(aD.offset)));
                        if(intoflag==1){
                           strcpy(insertargv[i-1],argv[i+1]);
                           sprintf(insertargv[i],"%d",*(int *)(s+(aD.offset)));
                        }
                        break;
                   case 'f':
                        printf("%-27f|",*(float *)(s+(aD.offset)));
                        if(intoflag==1){
                           strcpy(insertargv[i-1],argv[i+1]);
                           sprintf(insertargv[i],"%f",*(float *)(s+(aD.offset)));
                        }
                        break;
                   case 'c':
                        printf("%-27s|",s+(aD.offset));
                        if(intoflag==1){
                           strcpy(insertargv[i-1],argv[i+1]);
                           strcpy(insertargv[i],s+(aD.offset));
                        }
                        break;
                 }
                 if(HF_CloseFileScan(scanDesc)!=HFE_OK){
                    HF_PrintError("");
                    return -1;//Error closing the scan                                            
                 }   
              }
              printf("\n");      
              //If we have a temporary table (INTO)
              if(intoflag==1){
                 if(DM_insert(insertargc,insertargv)<0){
                        return -1;                                      
                 }
              }                                                            
          }
          if((aD1.indexed)==0 && (aD2.indexed)==0){
              if(HF_CloseFileScan(scanDesc6)!=HFE_OK){
                 HF_PrintError("");
                 return -1;//Error closing the scan                                            
              }
          }
          else{
              if(AM_CloseIndexScan(scanDesc6)!=AME_OK){
                 AM_PrintError("");
                 return -1;//Error closing the scan                                            
              }
          }            
    }
    printf("|");
    for(i=0;i<N;i++){
        printf("___________________________|");
    }
    if(sunolo==1){
       printf("\n%d rec was successfully joined\n",sunolo);              
    }
    else{
        printf("\n%d recs were successfully joined\n",sunolo);
    }//Close scans and relations
    if(((aD1.indexed)==0 && (aD2.indexed)==0) || ( (aD1.indexed)==1 && (aD2.indexed)==0 ) || ( (aD1.indexed)==0 && (aD2.indexed)==1 )){
         if(HF_CloseFileScan(scanDesc5)!=HFE_OK){
            HF_PrintError("");
            return -1;//Error closing the scan                                            
         }
         if(( (aD1.indexed)==1 && (aD2.indexed)==0 ) || ( (aD1.indexed)==0 && (aD2.indexed)==1 )){
            if(BF_CloseFile(fD1)!=BFE_OK){
               BF_PrintError("");
               return -1;//Error closing the scan                                            
            }      
         }
    }
    else{
         if(AM_CloseIndexScan(scanDesc5)!=AME_OK){
            AM_PrintError("");
            return -1;//Error closing the scan                                            
         } 
         if(BF_CloseFile(fD1)!=BFE_OK){
            BF_PrintError("");
            return -1;//Error closing the scan                                            
         }
         if(BF_CloseFile(fD2)!=BFE_OK){
            BF_PrintError("");
            return -1;//Error closing the scan                                            
         }
    }
    if(HF_CloseFile(fileDesc1)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan                                            
    }
    if(HF_CloseFile(fileDesc2)!=HFE_OK){
       HF_PrintError("");   
       return -1;//Error closing the scan                                            
    }
    free(s1);
    free(s2);                  
    if(intoflag==1){
       for(j=0;j<intoargc;j++){
         free(intoargv[j]);                    
       }
       free(intoargv);
       for(j=0;j<insertargc;j++){
           free(insertargv[j]);                    
       }
       free(insertargv);
    }
    return 0;
}
int DM_selectivejoin(int argc,char *argv[]){
    int shift,intoflag,k,j,i,N,intoscanDesc,scanDesc,scanDesc1,scanDesc2,scanDesc3,scanDesc4,scanDesc5,fileDesc,op2,selvalue;
    int selectargc,joinargc,pipeargc,selscanDesc,destargc;
    char name[2*MAXNAME+sizeof(char)],**selectargv,**joinargv,**pipeargv,**destargv,temp[MAXNAME+4*sizeof(char)],attrtype;
    attrDesc aD1,aD2,aD3,aD;
    relDesc rD1,rD2,rD;
    void *value;
    //Check if a temporary table exists
    if(argc%2==0){
       shift=0;
       intoflag=1;
       //N=number of projected attributes
       N=atoi(argv[2]);
    }
    else{//no temporary table
       shift=-1;
       //N=number of projected attributesm
       N=atoi(argv[1]);
    }
    //Check arguments
    if(N*2+3+shift+9!=argc){//
       printf("Invalid argc\n");
       return -1;//invalid arguments
    }
    
    //Check if the given relation.fields exist
    for(i=3+shift;i<2*N+3+shift;i+=2){
        sprintf(name,"%s.%s",argv[i],argv[i+1]);
        if((scanDesc=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
          HF_PrintError("");
          return -1;                                                                                
        }
        if(HF_FindNextRec(scanDesc,(char *)&aD1)<0){
         HF_PrintError("Invalid relation.attribute to project : ");
          return -1;//The field does not exist                                            
        }
        if(HF_CloseFileScan(scanDesc)!=HFE_OK){
          HF_PrintError("");
          return -1;//Error closing the scan                                            
        }
    }
    //Check if the selection relation exists
    if((scanDesc=HF_OpenFileScan(relfileDesc,sizeof(relDesc),'c',MAXNAME,0,1,argv[argc-9]))<0){
       HF_PrintError("");
       return -1;                                                                                
    }
    if(HF_FindNextRec(scanDesc,(char *)&rD)<0){
       HF_PrintError("Invalid relation to select with condition : ");
       return -1;//The selection relation does not exist                                           
    }
    if(HF_CloseFileScan(scanDesc)!=HFE_OK){
      HF_PrintError("");
      return -1;//Error closing the scan  
    }
    //Check if the selection relation.field exists
    sprintf(name,"%s.%s",argv[argc-9],argv[argc-8]);//
    if((scanDesc5=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
        HF_PrintError("");
        return -1;                                                                                
    }
    if(HF_FindNextRec(scanDesc5,(char *)&aD3)<0){
        HF_PrintError("Invalid relation.attribute to select with condition : ");
        return -1;//The field does not exist                                            
    }
    if(HF_CloseFileScan(scanDesc5)!=HFE_OK){
      HF_PrintError("");
      return -1;//Error closing the scan  
    }
    //Check if the relations to be joined exist
    //Relation 1
    if((scanDesc1=HF_OpenFileScan(relfileDesc,sizeof(relDesc),'c',MAXNAME,0,1,argv[argc-5]))<0){
       HF_PrintError("");
       return -1;                                                                                
    }
    if(HF_FindNextRec(scanDesc1,(char *)&rD1)<0){
       HF_PrintError("Invalid relation to join : ");
       return -1;//The relation does not exist                                           
    }
    if(HF_CloseFileScan(scanDesc1)!=HFE_OK){
      HF_PrintError("");
      return -1;//Error closing the scan  
    }
    //Relation 2
    if((scanDesc2=HF_OpenFileScan(relfileDesc,sizeof(relDesc),'c',MAXNAME,0,1,argv[argc-2]))<0){
       HF_PrintError("");
       return -1;                                                                                
    }
    if(HF_FindNextRec(scanDesc2,(char *)&rD2)<0){
       HF_PrintError("Invalid relation to join : ");
       return -1;//The relation does not exist                                           
    }
    if(HF_CloseFileScan(scanDesc2)!=HFE_OK){
      HF_PrintError("");
      return -1;//Error closing the scan  
    }
    //Check if the relation.fields to be joined exist
    //First relation.field
    sprintf(name,"%s.%s",argv[argc-5],argv[argc-4]);//
    if((scanDesc3=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
        HF_PrintError("");
        return -1;                                                                                
    }
    if(HF_FindNextRec(scanDesc3,(char *)&aD1)<0){
        HF_PrintError("Invalid relation.attribute to join : ");
        return -1;//The field does not exist                                            
    }
    if(HF_CloseFileScan(scanDesc3)!=HFE_OK){
      HF_PrintError("");
      return -1;//Error closing the scan  
    }//Second relation.field
    sprintf(name,"%s.%s",argv[argc-2],argv[argc-1]);//
    if((scanDesc4=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
        HF_PrintError("");
        return -1;                                                                                
    }
    if(HF_FindNextRec(scanDesc4,(char *)&aD2)<0){
        HF_PrintError("Invalid relation.attribute to join : ");
        return -1;//The field does not exist                                            
    }
    if(HF_CloseFileScan(scanDesc4)!=HFE_OK){
      HF_PrintError("");
      return -1;//Error closing the scan  
    }//Check if the first join relation is the same as the selection relation
    if(strcmp(argv[argc-9],argv[argc-5])!=0){
        printf("Relation to select differs from the one to join");
        return -1;//Invalid arguments, the selection relation is not the same as the first join relation                                         
    }
    //End of checks
    if((value=malloc(aD3.attrlength))==NULL){
        return -1;                                    
    } 
    op2=operand(argv[argc-3]);
    memcpy(value,argv[argc-6],aD3.attrlength);
    //Calculate the selectivity
    selvalue=(rD.reccnt)*selectivity(aD3,op2,value);
    //Cases where select and join will be called
    if((aD1.indexed==1 && aD2.indexed==0) || (aD1.indexed==1 && aD2.indexed==1 && selvalue>(rD2.reccnt)) ||
       (aD1.indexed==0 && aD2.indexed==0 && selvalue>(rD2.reccnt))){
       //Build the arguments for select
       selectargc=3+2*(rD.attrcnt)+4;
       if((selectargv=malloc((selectargc+1)*sizeof(char *)))==NULL){
           return -1;
       }
       for(j=0;j<=(selectargc-1);j++){//
           if((selectargv[j]=malloc(MAXNAME*sizeof(char)))==NULL){
               return -1;                                            
           }                        
       }
       strcpy(selectargv[0],"select");
       sprintf(temp,"%s%s",argv[argc-9],"new");
       strcpy(selectargv[1],temp);
       sprintf(selectargv[2],"%d",(rD.attrcnt));

       if((selscanDesc=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,0,1,argv[argc-9]))<0){
           HF_PrintError("");
           return -1;                                                                                
       }
       for(k=0;k<2*(rD.attrcnt);k+=2){
          if(HF_FindNextRec(selscanDesc,(char *)&aD)<0){
             HF_PrintError("");
             return -1;//The field does not exist                                            
          }
          sscanf(aD.attrname,"%[^'.'].%s",selectargv[k+3],selectargv[k+4]);
       }
       if(HF_CloseFileScan(selscanDesc)!=HFE_OK){
          HF_PrintError("");
          return -1;//Error closing the scan                                            
       }
       strcpy(selectargv[selectargc-4],argv[argc-9]);
       strcpy(selectargv[selectargc-3],argv[argc-8]);
       strcpy(selectargv[selectargc-2],argv[argc-7]);
       strcpy(selectargv[selectargc-1],argv[argc-6]);
       selectargv[selectargc]=NULL;
               
       if(DM_select(selectargc,selectargv)<0){
          return -1;
       }
       
       //Build the arguments for join
       joinargc=3+shift+2*N+5;
       if((joinargv=malloc((joinargc+1)*sizeof(char *)))==NULL){
           return -1;
       }
       for(j=0;j<=(joinargc-1);j++){//
           if((joinargv[j]=malloc(MAXNAME*sizeof(char)))==NULL){
               return -1;                                            
           }                        
       }
       strcpy(joinargv[0],"join");
       if(intoflag==1){
          strcpy(joinargv[1],argv[1]);
       }
       strcpy(joinargv[2+shift],argv[2+shift]);
       for(i=3+shift;i<=argc-10;i+=2){
           if(strcmp(argv[i],argv[argc-9])==0){
              strcpy(joinargv[i],selectargv[1]);   
           }
           else{
              strcpy(joinargv[i],argv[i]);
           }
           strcpy(joinargv[i+1],argv[i+1]);                             
       } 
       
       strcpy(joinargv[joinargc-5],selectargv[1]);
       for(i=4;i>=1;i--){
           strcpy(joinargv[joinargc-i],argv[argc-i]);
       }
       joinargv[joinargc]=NULL;
       if(DM_join(joinargc,joinargv)<0){
         return -1;
       }
       //Free memory
       for(j=0;j<joinargc;j++){
         free(joinargv[j]);                    
       }
       free(joinargv);
       //Free memory
       for(j=0;j<selectargc;j++){
         free(selectargv[j]);                    
       }
       free(selectargv);
       //Destroy the temporary table from select
       destargc=2;
       if((destargv=malloc((destargc+1)*sizeof(char *)))==NULL){
           return -1;
       }
       for(j=0;j<=(destargc-1);j++){//
           if((destargv[j]=malloc(MAXNAME*sizeof(char)))==NULL){
               return -1;                                            
           }                        
       }
       strcpy(destargv[0],"destroy");       
       strcpy(destargv[1],temp);
       destargv[destargc]=NULL;
       if(UT_destroy(destargc,destargv)<0){
            return -1;                               
       }
       for(j=0;j<destargc;j++){
         free(destargv[j]);                    
       }
       free(destargv);                               
    }//Case where the pipeline will be called
    else{
       //Build the arguments for pipeline
       pipeargc=argc;
       if((pipeargv=malloc((pipeargc+1)*sizeof(char *)))==NULL){
           return -1;
       }
       for(j=0;j<=(pipeargc-1);j++){//
           if((pipeargv[j]=malloc(MAXNAME*sizeof(char)))==NULL){
               return -1;                                            
           }                        
       }
       strcpy(pipeargv[0],"pipeline");
       for(i=1;i<argc;i++){
           strcpy(pipeargv[i],argv[i]);
       }
       pipeargv[pipeargc]=NULL;
       if(DM_pipeline(pipeargc,pipeargv)<0){
         return -1;
       }
       //Free memory
       for(j=0;j<pipeargc;j++){
         free(pipeargv[j]);                    
       }
       free(pipeargv);
    }
    free(value);     
    return 0;
}
int DM_pipeline(int argc,char *argv[]){
    int shift,intoargc,intoflag,k,j,i,N,intoscanDesc,scanDesc,scanDesc1,scanDesc2,scanDesc3,scanDesc4,scanDesc5,scanDesc6,scanDesc7,scanDesc8,scanDesc9;
    int fileDesc,fileDesc1,fileDesc2,fileDesc4,intofileDesc,flag=0,attrlength,capacity,op,op2,indexed,offset=0,counter=0,recId,recId1;
    int joinflag,pedioint,a,sunolo=0,insertargc;
    char *s,*s2,buffer[BF_BLOCK_SIZE],name[2*MAXNAME+sizeof(char)],Rname[MAXNAME],Sname[MAXNAME],**intoargv,temp[5],attrtype,*value;
    char **insertargv;
    attrDesc aD1,aD2,aD3,aD4,aD;
    relDesc rD1,rD2,rD;
    float pediofloat;
    //Check if a temporary table exists
    if(argc%2==0){
       shift=0;
       intoflag=1;
       //N=number of projected attributes
       N=atoi(argv[2]);
       for(i=3;i<2*N+3;i+=2){
           if(strcmp(argv[i],argv[argc-5])!=0 && strcmp(argv[i],argv[argc-2])!=0){
               printf("Relation to join differs from the ones to project (pipeline)\n");
               return -1;
           }
       }
       //Build the arguments that UT_Create will receive
       intoargc=2+2*N;
       if((intoargv=malloc((intoargc+1)*sizeof(char *)))==NULL){
           return -1;
       }
       for(j=0;j<=(intoargc-1);j++){//
           if((intoargv[j]=malloc(MAXNAME*sizeof(char)))==NULL){
               return -1;                                            
           }                        
       }
       strcpy(intoargv[0],"create");
       strcpy(intoargv[1],argv[1]);
       for(k=2;k<=N*2;k+=2){
          sprintf(name,"%s.%s",argv[k+1],argv[k+2]);                             
          if((intoscanDesc=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
              HF_PrintError("");
              return -1;                                                                                
          }                             
          if(HF_FindNextRec(intoscanDesc,(char *)&aD)<0){
             HF_PrintError("Invalid relation.attribute to project (into) : ");
             return -1;//The field does not exist                                            
          }
          sscanf(aD.attrname,"%*[^'.'].%s",intoargv[k]);
          if(aD.attrtype=='c'){
             sprintf(intoargv[k+1],"%s%c%d%s","'",aD.attrtype,aD.attrlength,"'");
          }  
          else{
             sprintf(intoargv[k+1],"%s%c%s","'",aD.attrtype,"'");
          }  
          if(HF_CloseFileScan(intoscanDesc)!=HFE_OK){
             HF_PrintError("");
             return -1;//Error closing the scan                                            
          }
       }
       intoargv[intoargc]=NULL;
       //Create the temporary table with the appropriate arguments we have provided
       if(UT_create(intoargc,intoargv)<0){
          printf("Temporary table couldn't be created\n");
          return -1;
       }
       insertargc=2+2*N;
       if((insertargv=malloc((insertargc+1)*sizeof(char *)))==NULL){
           return -1;
       }
       for(j=0;j<=(insertargc-1);j++){//
           if((insertargv[j]=malloc(MAXNAME*sizeof(char)))==NULL){
               return -1;                                            
           }                        
       }
       strcpy(insertargv[0],"insert");
       strcpy(insertargv[1],argv[1]);
       insertargv[insertargc]=NULL;
    }
    else{                  //no temporary table
       shift=-1;
       //N=number of projected attributes
       N=atoi(argv[1]);
       for(i=3+shift;i<2*N+3+shift;i+=2){
           if(strcmp(argv[i],argv[argc-5])!=0 && strcmp(argv[i],argv[argc-2])!=0){
               printf("Relation to join differs from the ones to project (pipeline)\n");
               return -1;
           }
       }
    }
    //Check arguments
    if(N*2+3+shift+9!=argc){//
       printf("Invalid argc\n");
       return -1;//invalid arguments
    }
    //Check if the given relation.fields exist
    for(i=3+shift;i<2*N+3+shift;i+=2){
        sprintf(name,"%s.%s",argv[i],argv[i+1]);
        if((scanDesc=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
            HF_PrintError("");
            return -1;                                                                                
        }
        if(HF_FindNextRec(scanDesc,(char *)&aD1)<0){
           HF_PrintError("Invalid relation.attribute to project : ");
           return -1;//The field does not exist                                            
        }
        if(HF_CloseFileScan(scanDesc)!=HFE_OK){
           HF_PrintError("");
           return -1;//Error closing the scan                                            
        }
    }
    //Check if the selection relation exists
    if((scanDesc=HF_OpenFileScan(relfileDesc,sizeof(relDesc),'c',MAXNAME,0,1,argv[argc-9]))<0){
       HF_PrintError("");
       return -1;                                                                                
    }
    if(HF_FindNextRec(scanDesc,(char *)&rD)<0){
       HF_PrintError("Invalid relation to select : ");                              
       return -1;//The selection relation does not exist                                           
    }
    if(HF_CloseFileScan(scanDesc)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan  
    }
    //Calculate block capacity in records
    capacity=BF_BLOCK_SIZE/(rD.relwidth);
    //Check if the selection relation.field exists
    sprintf(name,"%s.%s",argv[argc-9],argv[argc-8]);//
    if((scanDesc5=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
        HF_PrintError("");
        return -1;                                                                                
    }
    if(HF_FindNextRec(scanDesc5,(char *)&aD3)<0){
        HF_PrintError("Invalid relation.attribute to select with condition : ");
        return -1;//The field does not exist                                            
    }
    if(HF_CloseFileScan(scanDesc5)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan  
    }
    //Check if the relations to be joined exist
    //Relation 1
    if((scanDesc1=HF_OpenFileScan(relfileDesc,sizeof(relDesc),'c',MAXNAME,0,1,argv[argc-5]))<0){
       HF_PrintError("");
       return -1;                                                                                
    }
    if(HF_FindNextRec(scanDesc1,(char *)&rD1)<0){
       HF_PrintError("Invalid relation to join : ");
       return -1;//The relation does not exist                                           
    }
    if(HF_CloseFileScan(scanDesc1)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan  
    }
    //Relation 2
    if((scanDesc2=HF_OpenFileScan(relfileDesc,sizeof(relDesc),'c',MAXNAME,0,1,argv[argc-2]))<0){
       HF_PrintError("");
       return -1;                                                                                
    }
    if(HF_FindNextRec(scanDesc2,(char *)&rD2)<0){
       HF_PrintError("Invalid relation to join : ");
       return -1;//The relation does not exist                                           
    }
    if(HF_CloseFileScan(scanDesc2)!=HFE_OK){
      HF_PrintError("");
      return -1;//Error closing the scan  
    }
    //Check if the relation.fields to be joined exist
    //First relation.field
    sprintf(name,"%s.%s",argv[argc-5],argv[argc-4]);//
    if((scanDesc3=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
        HF_PrintError("");
        return -1;                                                                                
    }
    if(HF_FindNextRec(scanDesc3,(char *)&aD1)<0){
       HF_PrintError("Invalid relation.attribute to join : ");
       return -1;//The field does not exist                                            
    }
    if(HF_CloseFileScan(scanDesc3)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan  
    }
    //Second relation.field
    sprintf(name,"%s.%s",argv[argc-2],argv[argc-1]);//
    if((scanDesc4=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
        HF_PrintError("");
        return -1;                                                                                
    }
    if(HF_FindNextRec(scanDesc4,(char *)&aD2)<0){
       HF_PrintError("Invalid relation.attribute to join : ");
       return -1;//The field does not exist                                            
    }
    if(HF_CloseFileScan(scanDesc4)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error closing the scan  
    }//Check if the first join relation is the same as the selection relation
    if(strcmp(argv[argc-9],argv[argc-5])!=0){
       printf("Relation to select differs from the one to join");
       return -1;//Invalid arguments, the selection relation is not the same as the first join relation                                         
    }
    //End of checks
    
    //We will perform select - Fill the block
    //Open the relation
    if((fileDesc=HF_OpenFile(argv[argc-9]))<0){
       HF_PrintError("");
       return -1;//Error opening the relation
    }
    
    memcpy(&attrlength,&(aD3.attrlength),sizeof(int));
    memcpy(&attrtype,&(aD3.attrtype),sizeof(char));
    memcpy(&offset,&(aD3.offset),sizeof(int));
    op=operand(argv[argc-7]);
    memcpy(&indexed,&(aD3.indexed),sizeof(int));
    if((value=malloc(attrlength))==NULL){
        return -1;                                    
    }
    if(aD3.attrtype=='c'){
       memcpy(value,argv[argc-6],aD3.attrlength);
    }
    if(aD3.attrtype=='i'){
       pedioint=atoi(argv[argc-6]);  
       memcpy(value,&pedioint,aD3.attrlength);
    }
    if(aD3.attrtype=='f'){
       pediofloat=atof(argv[argc-6]);  
       memcpy(value,&pediofloat,aD3.attrlength);
    } 
    op2=operand(argv[argc-3]);
    
    //Open scan with appropriate arguments to return all records of the relation specified in TARGET_LIST
    //that satisfy the condition
    //If the relation.field has no index
    if(indexed==0){
       if((scanDesc6=HF_OpenFileScan(fileDesc,rD.relwidth,attrtype,attrlength,offset,op,value))<0){
           HF_PrintError("");
           return -1;                                                                                
       }
    }
    else{//If the relation.field has an index
         //Open the selection relation
         sprintf(name,"%s.%d",argv[argc-9],aD3.indexno);
         if((fileDesc1=BF_OpenFile(name))<0){//
             BF_PrintError("");
             return -1;//Error opening the relation
         }  
         if((scanDesc6=AM_OpenIndexScan(fileDesc1,attrtype,attrlength,op,value))<0){
             AM_PrintError("");
             return -1;
         }  
    }
    offset=0;
    if(aD2.indexed==0){
      if((fileDesc2=HF_OpenFile(argv[argc-2]))<0){
          HF_PrintError("");
          return -1;//Error opening the relation
      }      
    }
    else{
       sprintf(name,"%s.%d",argv[argc-2],aD2.indexno);  
       if((fileDesc2=BF_OpenFile(name))<0){
           BF_PrintError("");
           return -1;//Error opening the relation
       }
       if((fileDesc4=HF_OpenFile(argv[argc-2]))<0){
           HF_PrintError("");
           return -1;//Error opening the relation
      }  
    }
    if((s2=malloc(rD2.relwidth))==NULL){
        return -1;
    }
    printf("\n\nPIPELINE\n");
    for(i=0;i<N;i++){
        printf("____________________________");
    }
    printf("\n");
    for(i=3+shift;i<argc-9;i+=2){
        sprintf(name,"%s.%s",argv[i],argv[i+1]);
        printf("|%-27s",name);   
    }
    printf("|\n");
    //Nested loops algorithm
    while(1){
      if((s=malloc(rD.relwidth))==NULL){
          return -1;
      }
      //if the selection relation has no index
      if(indexed==0){
         if(HF_FindNextRec(scanDesc6,s)<0){
            flag=1;
         }
         else{
             memcpy(buffer+offset,s,rD.relwidth);
             offset+=(rD.relwidth);
         }  
      }
      //if the selection relation has an index
      else{
         if((recId=AM_FindNextEntry(scanDesc6))<0){
            flag=1;
         }
         else{
            if(HF_GetThisRec(fileDesc,recId,s,rD.relwidth)!=HFE_OK){
               HF_PrintError("");
               return -1;//The index does not match the heap file!                                                          
            }
            memcpy(buffer+offset,s,rD.relwidth);
            offset+=(rD.relwidth);
         }
      }
      free(s);
      //the block is full or the file records are exhausted
      if(offset+(rD.relwidth)>BF_BLOCK_SIZE || flag==1){
         //if the file records are exhausted, adjust the capacity
         if(flag==1){
            capacity=offset/(rD.relwidth);            
         }
         offset=0;
         //if the inner join relation has an index
         if(aD2.indexed==1){
            counter=0;
            while(counter<capacity){                                
                if((scanDesc8=AM_OpenIndexScan(fileDesc2,aD2.attrtype,aD2.attrlength,op2,buffer+offset+(aD1.offset)))<0){
                    AM_PrintError("");
                    return -1;                                                                                
                }
                while(1){
                  if((recId1=AM_FindNextEntry(scanDesc8))==AME_EOF){
                      break;                                      
                  }
                  if(HF_GetThisRec(fileDesc4,recId1,s2,rD2.relwidth)!=HFE_OK){
                     HF_PrintError("");
                     return -1;                                                             
                  }
                  sunolo++;
                  for(i=3+shift;i<argc-9;i+=2){
                    if(strcmp(argv[i],argv[argc-9])==0){
                       s=buffer+offset;                                       
                    }
                    else{
                       s=s2;
                    }                
                    sprintf(name,"%s.%s",argv[i],argv[i+1]);
                    if((scanDesc9=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
                        HF_PrintError("");
                        return -1;                                                                                
                    }
                    if(HF_FindNextRec(scanDesc9,(char *)&aD4)<0){
                       HF_PrintError("");
                       return -1;//The field does not exist                                            
                    }
                    if(i==3+shift){
                       printf("|");
                    }
                    switch(aD4.attrtype){
                      case 'i':
                        printf("%-27d|",*(int *)(s+(aD4.offset)));
                        if(intoflag==1){
                           strcpy(insertargv[i-1],argv[i+1]);
                           sprintf(insertargv[i],"%d",*(int *)(s+(aD4.offset)));
                        }
                        break;
                      case 'f':
                        printf("%-27f|",*(float *)(s+(aD4.offset)));
                        if(intoflag==1){
                           strcpy(insertargv[i-1],argv[i+1]);
                           sprintf(insertargv[i],"%f",*(float *)(s+(aD4.offset)));
                        }
                        break;
                      case 'c':
                        printf("%-27s|",s+(aD4.offset));
                        if(intoflag==1){
                           strcpy(insertargv[i-1],argv[i+1]);
                           strcpy(insertargv[i],s+(aD4.offset));
                        }
                        break;
                    }
                    if(HF_CloseFileScan(scanDesc9)!=HFE_OK){
                       HF_PrintError("");
                       return -1;//Error closing the scan                                            
                    }
                  }//If we have a temporary table (INTO)
                  if(intoflag==1){
                     if(DM_insert(insertargc,insertargv)<0){
                        return -1;                                      
                     }
                  }
                  printf("\n");
                }
                counter++;
                offset+=(rD.relwidth);                     
                if(AM_CloseIndexScan(scanDesc8)!=AME_OK){
                   AM_PrintError("");
                   return -1;//Error closing the scan                                            
                }
            }                
         }                               
         //if the inner join relation has no index
         else{
              counter=0;
              if((scanDesc7=HF_OpenFileScan(fileDesc2,rD2.relwidth,'c',MAXNAME,MAXNAME,1,NULL))<0){
                  HF_PrintError("");
                  return -1;                                                                                
              }
              while(HF_FindNextRec(scanDesc7,s2)>=0){
                    offset=0;
                    counter=0;
                    while(counter<capacity){
                      joinflag=0;
                      switch(op2){
                        case 1:            
                             if(dm_mymemcmp(buffer+offset+(aD1.offset),s2+(aD2.offset),aD2.attrtype,0)==0){                     
                                joinflag=1;
                             }
                             break;
                        case 2:
                             if(dm_mymemcmp(buffer+offset+(aD1.offset),s2+(aD2.offset),aD2.attrtype,0)<0){                     
                                joinflag=1;
                             }
                             break;
                        case 3:
                             if(dm_mymemcmp(buffer+offset+(aD1.offset),s2+(aD2.offset),aD2.attrtype,0)>0){                     
                                joinflag=1;
                             }
                             break;
                        case 4:
                             if(dm_mymemcmp(buffer+offset+(aD1.offset),s2+(aD2.offset),aD2.attrtype,0)<=0){                     
                                joinflag=1;
                             }
                             break;
                        case 5:
                             if(dm_mymemcmp(buffer+offset+(aD1.offset),s2+(aD2.offset),aD2.attrtype,0)>=0){                     
                                joinflag=1;
                             }
                             break;
                        case 6:
                             if(dm_mymemcmp(buffer+offset+(aD1.offset),s2+(aD2.offset),aD2.attrtype,0)!=0){                     
                                joinflag=1;
                             }
                             break;
                      }                     
                      if(joinflag==1){
                        sunolo++;
                        for(i=3+shift;i<argc-9;i+=2){
                         if(strcmp(argv[i],argv[argc-9])==0){
                            s=buffer+offset;                                       
                         }
                         else{
                            s=s2;
                         }                
                         sprintf(name,"%s.%s",argv[i],argv[i+1]);
                         if((scanDesc9=HF_OpenFileScan(attrfileDesc,sizeof(attrDesc),'c',MAXNAME,MAXNAME,1,name))<0){
                             HF_PrintError("");
                             return -1;                                                                                
                         }
                         if(HF_FindNextRec(scanDesc9,(char *)&aD4)<0){
                            HF_PrintError("");
                            return -1;//The field does not exist                                            
                         }
                         if(i==3+shift){
                            printf("|");
                         }
                         switch(aD4.attrtype){
                           case 'i':
                             printf("%-27d|",*(int *)(s+(aD4.offset)));
                             if(intoflag==1){
                                strcpy(insertargv[i-1],argv[i+1]);
                                sprintf(insertargv[i],"%d",*(int *)(s+(aD4.offset)));
                             }
                             break;
                           case 'f':
                             printf("-27%f|",*(float *)(s+(aD4.offset)));
                             if(intoflag==1){
                                strcpy(insertargv[i-1],argv[i+1]);
                                sprintf(insertargv[i],"%f",*(float *)(s+(aD4.offset)));
                             }
                             break;
                           case 'c':
                             printf("%-27s|",s+(aD4.offset));
                             if(intoflag==1){
                                strcpy(insertargv[i-1],argv[i+1]);
                                strcpy(insertargv[i],s+(aD4.offset));
                             }
                             break;
                         }
                         if(HF_CloseFileScan(scanDesc9)!=HFE_OK){
                            HF_PrintError("");
                            return -1;//Error closing the scan                                            
                         }
                        }//If we have a temporary table (INTO)
                        if(intoflag==1){
                           if(DM_insert(insertargc,insertargv)<0){
                              return -1;                                      
                           }
                        }
                      }
                     counter++;
                     offset+=(rD.relwidth);
                     if(joinflag==1){
                       printf("\n");
                     }
                    }                                        
              }        
              if(HF_CloseFileScan(scanDesc7)!=HFE_OK){
                 HF_PrintError("");
                 return -1;//Error closing the scan                                            
              }       
         }
         offset=0;
         if(flag==1){
            break;
         }                                  
      }
    }
    printf("|");
    for(i=0;i<N;i++){
      printf("___________________________|");
    }
    if(sunolo==1){
       printf("\n%d rec was successfully selected and joined with function pipeline\n",sunolo);              
    }
    else{
        printf("\n%d recs were successfully selected and joined with function pipeline\n",sunolo);
    }
    free(s2);
    //Close scans and relations
    if(aD2.indexed==0){   
       if(HF_CloseFile(fileDesc2)!=HFE_OK){
          HF_PrintError("");
          return -1;//Error closing the scan                                            
       }
    }
    else{
       if(BF_CloseFile(fileDesc2)!=BFE_OK){
          BF_PrintError("");
          return -1;//Error closing the scan                                            
       } 
       if(HF_CloseFile(fileDesc4)!=HFE_OK){
          HF_PrintError("");
          return -1;//Error opening the relation
       }  
    }
    if(indexed==0){   
       if(HF_CloseFileScan(scanDesc6)!=HFE_OK){
          HF_PrintError("");
          return -1;//Error closing the scan                                            
       }
    }
    else{
       if(AM_CloseIndexScan(scanDesc6)!=AME_OK){
          AM_PrintError("");
          return -1;//Error closing the scan                                            
       } 
       if(BF_CloseFile(fileDesc1)!=BFE_OK){
          BF_PrintError("");
          return -1;//Error opening the relation
       }  
    }
    if(HF_CloseFile(fileDesc)!=HFE_OK){
       HF_PrintError("");
       return -1;//Error opening the relation
    }
    free(value);                
    if(intoflag==1){
       for(j=0;j<intoargc;j++){
         free(intoargv[j]);                    
       }
       free(intoargv);
    }
    return 0;
}
//THE END
