#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "BF_Lib.h"
//Synarthseis HD epipedou
int HD_exist(char *filename){
    
    FILE *fp;
    
    if ((fp=(fopen(filename,"rb")))==NULL){
       BF_errno=-6;
       return BF_errno;//Den uparxei
    }   
    else{
         fclose(fp);
         BF_errno=0;
         return 1;//Yparxei
         }
}
int HD_create(char *filename)
{
    FILE *fp;
    
    if ((fp=(fopen(filename,"wb")))==NULL){
       BF_errno=-6;
       return BF_errno;//Apetixe i fopen
    }   
    else{
         fclose(fp);
         BF_errno=0;
         return 1;//Epityxia
   }
}
int HD_remove(char *filename){
    if(HD_exist(filename)<0){
         BF_errno=-18;
         return BF_errno;//To arxeio den iparxei
    }
    if(remove(filename)!=0){
         BF_errno=-6;
         return BF_errno;//Error kata tin diagrafi tou arxeiou
    }
    BF_errno=0;
    return 0;//Epityxia
}

//Synarthsh epeksergasias tou pinaka bitmap.Sygkekrimena bazei 1 sth thesh tou pinaka  pou antistoixei to block 
//tou arxeiou me arithmo num_of_block.Kanei dhladh valid to block. 
void make_valid(char *bitmap,int num_of_block){
      int i,j;
     
     i=num_of_block%8;//Briskoume th thesh tou pinaka (kathe thesh einai 1 byte) pou periexei to bit pou antistoixei sto block tou arxeiou
     j=num_of_block/8;//Mesa sto keli epilegw to katallhlo bit pou antistoixei sto block tou arxeiou
     bitmap[j]=bitmap[j]|m_table[i];//Me th bohtheia ths antistoixhs maskas kanw bitwise-or kai metatrepw to 0 se 1
}
//Synarthsh epeksergasias tou pinaka bitmap.Sygkekrimena bazei 0 sth thesh tou pinaka  pou antistoixei to block 
//tou arxeiou me arithmo num_of_block.Kanei dhladh invalid to block. 
void make_invalid(char *bitmap,int num_of_block){
     int i,j;
     
     i=num_of_block%8;//Briskoume th thesh tou pinaka (kathe thesh einai 1 byte) pou periexei to bit pou antistoixei sto block tou arxeiou
     j=num_of_block/8;//Mesa sto keli epilegw to katallhlo bit pou antistoixei sto block tou arxeiou
     bitmap[j]=bitmap[j]&~m_table[i];//Me th bohtheia ths antistoixhs maskas kanw bitwise-and kai metatrepw to 1 se 0
}
//Synarthsh epeksergasias tou pinaka bitmap.Sygkekrimena elegxei an h thesh pou antistoixei sto block tou arxeiou me 
//arithmo num_of_block einai valid h invalid
int is_valid(char *bitmap,int num_of_block){
     int i,j;
     
     i=num_of_block%8;//Briskoume th thesh tou pinaka (kathe thesh einai 1 byte) pou periexei to bit pou antistoixei sto block tou arxeiou
     j=num_of_block/8;//Mesa sto keli epilegw to katallhlo bit pou antistoixei sto block tou arxeiou
     if((bitmap[j]&m_table[i])!=0){ //An to apotelesma tou bitwise-and me thn katallhlh maska einai diaforo tou mhdenos tote
          return 1;//to block einai valid
     }
     return 0;//alliws to block einai invalid
}
void BF_Init(void){
    int i,j;
    m_table[0]=MASK0;
    m_table[1]=MASK1;
    m_table[2]=MASK2;
    m_table[3]=MASK3;
    m_table[4]=MASK4;
    m_table[5]=MASK5;
    m_table[6]=MASK6;
    m_table[7]=MASK7;
    Time=0;
    for(i=0;i<MAXOPENFILES;i++){
        of_table[i].isfree=0;
        of_table[i].fp=NULL;
        of_table[i].header_ptr=NULL;
        memset(of_table[i].pinned_table,0,BF_BUFFER_SIZE*4);
    }
    for(i=0;i<BF_BUFFER_SIZE;i++){
        buffer[i].isfree=0;
        memset(buffer[i].fname,'0',50);
        buffer[i].blockNum=0;
        buffer[i].isdirty=FALSE;
        buffer[i].ispinned=0;
        buffer[i].lastAccess=0;
        memset(buffer[i].data,0,BF_BLOCK_SIZE);
        memset(buffer[i].fpinned_table,0,MAXOPENFILES*4);
    }
}
int BF_CreateFile(char *filename){
    FILE *fp;
    header *headerptr;
    int i;
    
    //Elegxoi
    if (HD_exist(filename)>0){
       BF_errno=-19;
       return BF_errno;//error -> to arxeio yparxei hdh
     }
    if (HD_create(filename)<0){
       BF_errno=-6;
       return BF_errno;//error
    }
    if((fp=fopen(filename,"rb+"))==NULL){
       BF_errno=-6;
       return BF_errno;//error sto anoigma tou arxeiou
       }
    //Desmeush mnhmhs gia to header enos arxeiou
    if((headerptr=malloc(sizeof(header)))==NULL){
       BF_errno=-1;
       return BF_errno;//error -> h mnhmh einai gemati
       }
    //Antigrafh tou onomatos tou arxeiou sthn kefalida tou 
    strcpy(headerptr->fname,filename);
    //Arxikopoihsh stoixeiwn pou periexei to arxeio kefalida
    headerptr->num_of_blocks=0;
    for( i=0; i<970; i++ ){
         (headerptr->bitmaptable[i])=0;
    }
    //Metaferw to deikth tou arxeiou sthn arxh tou
    if((fseek(fp,0,SEEK_SET))!=0){
        BF_errno=-6;
        return BF_errno;//error sthn fseek
    }
    //Grafw sto arxeio thn kefalida tou
    fwrite(headerptr,BF_BLOCK_SIZE,1,fp);
    fclose(fp);//Kleinw to arxeio
    free(headerptr);//Eleutherwn th mnhmh pou eixa desmeusei gia to deikth
    BF_errno=0;
    return BF_errno;//Epityxia
}
int BF_OpenFile(char *filename){
    int i,j,freeNum=-1,samefileNum;
    //To arxeio yparxei ston pinaka anoiktwn arxeiwn   
    for(i=0;i<MAXOPENFILES;i++){
             if(of_table[i].isfree==0){//An h thesh einai adeia thn krataw 
                 freeNum=i;
             }
             else{//Alliws elegxw an to arxeio einai hdh anoikto se auth th thesh
               if(strcmp(of_table[i].header_ptr->fname,filename)==0){
                 BF_errno=-12;                                        
                 samefileNum=i;//Brhka th thesh tou anoiktou arxeiou ston pinaka anoiktwn arxeiwn
                 if(freeNum<0){//An den exw brei kenh thesh ws tote
                      for(j=i;j<MAXOPENFILES;j++){//Psaxnw mexri to telos tou pinaka mexri na brw thn prwth eleutherh thesh kai stamataw
                          if(of_table[j].isfree==0){
                                freeNum=j;
                                break;
                          }
                      }
                      if(freeNum==-1){
                           BF_errno=-13;
                           return BF_errno;//O pinakas anoiktwn arxeiwn einai gematos
                      }
                 //Se auto shmeio exw brei se poia thesh tou pinaka brisketai to anoikto arxeio 
                 //kathws kai mia kenh thesh ston pinaka.     
                 }//Antigrafw apo th mia thesh sthn allh ta stoixeia 
                 of_table[freeNum].isfree=1;
                 if((of_table[freeNum].fp=fopen(filename,"rb+"))==NULL){
                      BF_errno=-6;
                      return BF_errno;//error sto anoigma tou arxeiou
                 }
                 of_table[freeNum].header_ptr=of_table[samefileNum].header_ptr;
                 (of_table[freeNum].header_ptr->openedNum)++;
                 return freeNum;//Epityxia me to anoigma tou arxeiou -> Epistrefw to fileDesc
               }
             }
    }                       
    if(HD_exist(filename)<0){
       BF_errno=-18;
       return BF_errno;//Error -> To arxeio den yparxei
    }
    //To arxeio den yparxei ston pinaka anoiktwn arxeiwn
    for(i=0;i<MAXOPENFILES;i++){
                   if((of_table[i].isfree)==0){//Briskw thn prwth kenh thesh
                      //Desmeuw mnhmh gia tous deiktes , dinw timh sto deikth tou arxeiou kai kanw tis enhmerwseis pou xreiazontai
                      of_table[i].isfree=1;
                      of_table[i].fp=fopen(filename,"rb+");
                      of_table[i].header_ptr=malloc(sizeof(h_struct));
                      of_table[i].header_ptr->ofhead=malloc(sizeof(header));
                      of_table[i].header_ptr->openedNum=1;
                      strcpy(of_table[i].header_ptr->fname,filename);
                      if((fseek(of_table[i].fp,0,SEEK_SET))!=0){
                         BF_errno=-6;
                         return BF_errno;//Error sthn fseek
                      }
                      fread(of_table[i].header_ptr->ofhead,BF_BLOCK_SIZE,1,of_table[i].fp);
                      return i;//Epityxia me to grapsimo tou arxeiou ston pinaka -> Epistrefw to fileDesc
                   }
    }
    //An h timh ths meinei 0 dhladh an den mpei pote mesa sto parapanw if
    BF_errno=-13;
    return BF_errno;//Error -> O pinakas anoiktwn arxeiwn einai gematos
}
int BF_DestroyFile(char *filename){
    int i;
    
    if(HD_exist(filename)<0){
          BF_errno=-18;
       return BF_errno;//Error -> To arxeio den yparxei
    }
    for(i=0;i<MAXOPENFILES;i++){
        if(of_table[i].isfree==1 && strcmp(of_table[i].header_ptr->fname,filename)==0){
                 BF_errno=-12;
                 return BF_errno;//Error -> To arxeio einai anoikto
        }
    }//An den yparxei to arxeio ston pinaka twn anoiktwn arxeiwn tote to kanoume remove
    BF_errno=HD_remove(filename);
    return BF_errno;
}
int BF_CloseFile(int fileDesc){
    int i,j;
    i=fileDesc;
    
    if(of_table[i].isfree==0){
          BF_errno=-14;
          return BF_errno;//Error -> Lathos fileDesc
    }
    for(j=0;j<BF_BUFFER_SIZE;j++){
        if(of_table[i].pinned_table[j]!=0){
              BF_errno=-3;
              return BF_errno;//Error -> Kapoio block tou arxeiou einai karfitswmeno sth mnhmh
        }
    }
    //An to arxeio einai anoikto panw apo mia fores
    if(of_table[i].header_ptr->openedNum>1){
       (of_table[i].header_ptr->openedNum)--;
       of_table[i].header_ptr=NULL;
       of_table[i].isfree=0;
       fclose(of_table[i].fp);
       BF_errno=0;
       return BF_errno;//Epityxia 
    }//An kapoia blocks tou arxeiou einai dirty sth mnhmh ta grafoume sto disko prin kleisoume to arxeio
    for(j=0;j<BF_BUFFER_SIZE;j++){
        if(strcmp(buffer[j].fname,of_table[i].header_ptr->fname)==0){
           if(buffer[j].isdirty==TRUE){
              if((fseek(of_table[i].fp,(buffer[j].blockNum)*BF_BLOCK_SIZE,SEEK_SET))!=0){
               BF_errno=-6;
               return BF_errno;//lathos sthn fseek
              }
              fwrite(buffer[j].data,BF_BLOCK_SIZE,1,of_table[i].fp);
           }
           buffer[j].isfree=0;
           buffer[j].isdirty=FALSE;
        }
    }//An to arxeio einai anoikto mia mono fora
    free(of_table[i].header_ptr->ofhead);
    free(of_table[i].header_ptr);
    of_table[i].isfree=0;
    fclose(of_table[i].fp);
    BF_errno=0;
    return BF_errno;//Epityxia
}
int BF_GetThisBlock(int fileDesc,int blockNum,char **blockbuf){
    int i,j,fbn=-1,min;
    if(of_table[fileDesc].isfree==0){
       BF_errno=-14;
       return BF_errno;//Error -> Lathos fileDesc 
    }
    if(is_valid(of_table[fileDesc].header_ptr->ofhead->bitmaptable,blockNum)==0){
           BF_errno=-11;
           return BF_errno;;//Error -> To block me arithmo blockNum den einai valid 
    }
    //Koitaw an yparxei to block sthn endiamesh mnhmh,an yparxei kanw tis enhmerwseis pou xreiazontai
    for(i=0;i<BF_BUFFER_SIZE;i++){
       if((strcmp(buffer[i].fname,of_table[fileDesc].header_ptr->fname)==0)&& buffer[i].blockNum==(blockNum+1) && buffer[i].isfree==1 ){
          buffer[i].fpinned_table[fileDesc]+=1;
          buffer[i].ispinned++;
          buffer[i].lastAccess=++Time;
          for(j=0;j<BF_BUFFER_SIZE;j++){
              if(of_table[fileDesc].pinned_table[j]==0){
                 of_table[fileDesc].pinned_table[j]=blockNum+1;
                 break;          
              }
          }
          *blockbuf=buffer[i].data;  
          BF_errno=0;
          return BF_errno;//Epityxia
       }
    }
    //Den yparxei to block sthn endiamesh mnhmh
    //Briskw xwro sthn endiamesh mnhmh 
    for(j=0;j<BF_BUFFER_SIZE;j++){
        if(buffer[j].isfree==0){
           fbn=j;//Brhka thn prwth eleutherh thesh sth mnhmh
           break;                  
        }
    }//An den yparxei eleutherh thesh sth mnhmh xrhsimopoiw LRU
    //O LRU douleuei ws ekshs:Kathe fora pou ginetai mia energeia se ena block auksanetai h global metablhth time pou ekxwreitai sth
    //lastAccess.To block me th mikroterh timh lastAccess ,to opoio den einai karfwmeno sth mnhmh, einai to ypopshfio gia antikatastash
    if(fbn<0){
       min=10000000;
       for(j=0;j<BF_BUFFER_SIZE;j++){
           if(buffer[j].lastAccess<=min && buffer[j].ispinned==0){
              min=buffer[j].lastAccess;
              fbn=j;
           }
       }
       if(fbn<0){
          BF_errno=-2;
          return BF_errno;//Error -> Ola ta blocks einai pinned , den leitourgei o LRU
       }//Se periptwsh pou to block pros antikatastash einai disty enhmerwnw to periexomeno tou sto disko
       if(buffer[fbn].isdirty==TRUE){
          for(i=0;i<MAXOPENFILES;i++){
            if(strcmp(buffer[fbn].fname,of_table[i].header_ptr->fname)==0){                     
               break;
            }                
          }
          if((fseek(of_table[i].fp,(buffer[fbn].blockNum)*BF_BLOCK_SIZE,SEEK_SET))!=0){
              BF_errno=-6;
              return BF_errno;//Error sthn fseek
          }
          fwrite(buffer[fbn].data,BF_BLOCK_SIZE,1,of_table[i].fp);
       }
    }
    //Fernoume to block sth mnhmh,kanoume tis enhmerwseis
    buffer[fbn].isfree=1;
    strcpy(buffer[fbn].fname,of_table[fileDesc].header_ptr->fname);
    buffer[fbn].blockNum=blockNum+1;
    buffer[fbn].fpinned_table[fileDesc]+=1;
    buffer[fbn].isdirty=FALSE;
    buffer[fbn].ispinned++;
    buffer[fbn].lastAccess=++Time;
    if((fseek(of_table[fileDesc].fp,(blockNum+1)*BF_BLOCK_SIZE,SEEK_SET))!=0){
	   BF_errno=-6;
       return BF_errno;//Error stin fseek
    }
    fread(buffer[fbn].data,BF_BLOCK_SIZE,1,of_table[fileDesc].fp);
    if((fseek(of_table[fileDesc].fp,0,SEEK_SET))!=0){
       BF_errno=-6;
       return BF_errno;//Error stin fseek
    }
    //Enhmerwnw to pinned_table
    for(j=0;j<BF_BUFFER_SIZE;j++){
        if(of_table[fileDesc].pinned_table[j]==0){
           of_table[fileDesc].pinned_table[j]=blockNum+1;
           break;          
        }
    }
    *blockbuf=buffer[fbn].data;
    BF_errno=0;
    return BF_errno;//Epityxia
}
int BF_AllocBlock(int fileDesc,int *blockNum,char **blockbuf){
    int nobs,i,j,kenh_thesh=-1,fbn=-1,min;
    char data[BF_BLOCK_SIZE];
    if(of_table[fileDesc].isfree==0){
       BF_errno=-14;
       return BF_errno;//Error -> Lathos fileDesc 
    }
    nobs=of_table[fileDesc].header_ptr->ofhead->num_of_blocks;
    for(i=0;i<nobs;i++){
        if(is_valid(of_table[fileDesc].header_ptr->ofhead->bitmaptable,i)==0){
           kenh_thesh=i+1;
           break;//Brhka to prwto adeio block sth thesh i
        }
    }
    //Den yparxei adeio block sto arxeio
    //Dhmiourgw neo block sto telos tou arxeiou
    if(kenh_thesh<0){
       if((fseek(of_table[fileDesc].fp,0,SEEK_END))!=0){
           BF_errno=-6;
           return BF_errno;//Error stin fseek
       }
       memset(data,0,BF_BLOCK_SIZE);
       fwrite(data,BF_BLOCK_SIZE,1,of_table[fileDesc].fp);
       nobs++;
	   kenh_thesh=nobs;
    }
    for(j=0;j<BF_BUFFER_SIZE;j++){
        if(buffer[j].isfree==0){
           fbn=j;//Brhka thn prwth eleutherh thesh sth mnhmh
           break;                  
        }
    }
    //An den yparxei eleutherh thesh sth mnhmh tote xrhsimopoiw ton LRU
    if(fbn<0){
       min=10000000;
       for(j=0;j<BF_BUFFER_SIZE;j++){
           if(buffer[j].lastAccess<min && buffer[j].ispinned==0){
              min=buffer[j].lastAccess;
              fbn=j;
           }
       }
       if(fbn<0){
          BF_errno=-2;
          return BF_errno;;///Error -> Ola ta blocks einai pinned , den leitourgei o LRU
       }//Se periptwsh pou to block pros antikatastash einai disty enhmerwnw to periexomeno tou sto disko
       if(buffer[fbn].isdirty==TRUE){
          for(i=0;i<MAXOPENFILES;i++){
            if(strcmp(buffer[fbn].fname,of_table[i].header_ptr->fname)==0){                     
               break;
            }                
          }
          if((fseek(of_table[i].fp,(buffer[fbn].blockNum)*BF_BLOCK_SIZE,SEEK_SET))!=0){
              BF_errno=-6;
              return BF_errno;//Error sthn fseek
          }
          fwrite(buffer[fbn].data,BF_BLOCK_SIZE,1,of_table[i].fp);
       }
    }
    //Fernoume to block sth mnhmh,kanoume tis enhmerwseis
    buffer[fbn].isfree=1;
    strcpy(buffer[fbn].fname,of_table[fileDesc].header_ptr->fname);
    buffer[fbn].blockNum=kenh_thesh;
    buffer[fbn].fpinned_table[fileDesc]+=1;
    buffer[fbn].isdirty=FALSE;
    buffer[fbn].ispinned++;
    buffer[fbn].lastAccess=++Time;
    if((fseek(of_table[fileDesc].fp,kenh_thesh*BF_BLOCK_SIZE,SEEK_SET))!=0){
       BF_errno=-6;
       return BF_errno;//Error sthn fseek
    }
    fread(buffer[fbn].data,BF_BLOCK_SIZE,1,of_table[fileDesc].fp);
    of_table[fileDesc].header_ptr->ofhead->num_of_blocks=nobs;
    make_valid(of_table[fileDesc].header_ptr->ofhead->bitmaptable,kenh_thesh-1);
    if((fseek(of_table[fileDesc].fp,0,SEEK_SET))!=0){
	   BF_errno=-6;
       return BF_errno;//Error sthn fseek
    }
    fwrite(of_table[fileDesc].header_ptr->ofhead,BF_BLOCK_SIZE,1,of_table[fileDesc].fp);
    if((fseek(of_table[fileDesc].fp,0,SEEK_SET))!=0){
       BF_errno=-6;
       return BF_errno;//Error sthn fseek
    }//Enhmerwnw to pinned_table
    for(j=0;j<BF_BUFFER_SIZE;j++){
        if(of_table[fileDesc].pinned_table[j]==0){
           of_table[fileDesc].pinned_table[j]=kenh_thesh;
           break;          
        }
    }
    *blockNum=kenh_thesh-1;
    *blockbuf=buffer[fbn].data;
    BF_errno=0;
    return BF_errno;//Epityxia  
}
int BF_GetFirstBlock(int fileDesc,int *blockNum,char **blockbuf){
    int nobs,i,NumBlock=-1;
    if(of_table[fileDesc].isfree==0){
       BF_errno=-14;
       return BF_errno;//Error -> Lathos fileDesc 
    }
    nobs=of_table[fileDesc].header_ptr->ofhead->num_of_blocks;
    for(i=0;i<nobs;i++){
        if(is_valid(of_table[fileDesc].header_ptr->ofhead->bitmaptable,i)==1){
           NumBlock=i+1;
           break;//Brhka to prwto valid block sth thesh i+1
        }
    }
    //Den yparxei valid block sto arxeio
    if(NumBlock<0){
       BF_errno=-11;
       return BF_errno;
    }
    *blockNum=NumBlock-1;
    //Kalw thn BF_GetThisBlock me orisma to prwto valid block(an yparxei) pou exw brei
    return BF_GetThisBlock(fileDesc,NumBlock-1,blockbuf);    
}
int BF_GetNextBlock(int fileDesc,int *blockNum,char **blockbuf){
    int nobs,i,NumBlock=-1;
    if(of_table[fileDesc].isfree==0){
       BF_errno=-14;
       return BF_errno;//Error -> Lathos fileDesc 
    }
    nobs=of_table[fileDesc].header_ptr->ofhead->num_of_blocks;
    for(i=(*blockNum+1);i<nobs;i++){
        if(is_valid(of_table[fileDesc].header_ptr->ofhead->bitmaptable,i)==1){
           NumBlock=i+1;
           break;//Brhka to prwto valid block , meta to dosmeno block , sth thesh i+1
        }
    }
    //Den yparxei valid block sto arxeio meta to dosmeno block
    if(NumBlock<0){
       BF_errno=-11;
       return BF_errno;;
    }
    *blockNum=NumBlock-1;
    //Kalw thn BF_GetThisBlock me orisma to prwto valid block(an yparxei) , meta to dosmeno block , pou exw brei
    return BF_GetThisBlock(fileDesc,NumBlock-1,blockbuf);
}
int BF_DisposeBlock(int fileDesc,int *blockNum)
{
    int j;

    if(of_table[fileDesc].isfree==0){
       BF_errno=-14;
       return BF_errno;//Error -> Lathos fileDesc 
    }
    if(is_valid(of_table[fileDesc].header_ptr->ofhead->bitmaptable,(*blockNum))==0){
       BF_errno=-11;
       return BF_errno;//Error -> To block me arithmo blockNum den einai valid 
    }
    //Koitaw an to dosmeno block yparxei sth mnhmh
    for(j=0;j<BF_BUFFER_SIZE;j++){
       if((strcmp(buffer[j].fname,of_table[fileDesc].header_ptr->fname)==0)&& buffer[j].blockNum==(*blockNum+1) && buffer[j].isfree==1){
              if(buffer[j].ispinned!=0){
              BF_errno=-3;
              return BF_errno;//To block einai karfwmeno sthn endiamesh mnhmh
           }
           buffer[j].isfree=0;//Alliws adeiazw th thesh sth mnhmh
	       break;
       }
    }
    //Kanw 0 th thesh tou block sto bitmap kai enhmerwnw to header tou arxeiou
    make_invalid(of_table[fileDesc].header_ptr->ofhead->bitmaptable,*blockNum);
    if((fseek(of_table[fileDesc].fp,0,SEEK_SET))!=0){
            BF_errno=-6;
            return BF_errno;//Error stin fseek
    }
    fwrite(of_table[fileDesc].header_ptr->ofhead,BF_BLOCK_SIZE,1,of_table[fileDesc].fp);
    if((fseek(of_table[fileDesc].fp,0,SEEK_SET))!=0){
       BF_errno=-6;
       return BF_errno;//Error stin fseek
    }
    BF_errno=0;
    return BF_errno;//Epityxia
}
int BF_UnpinBlock(int fileDesc,int blockNum,int dirty){
    int j,thesh=-1;
    if(of_table[fileDesc].isfree==0){
       BF_errno=-14;
       return BF_errno;//Error -> Lathos fileDesc 
    }
    if(is_valid(of_table[fileDesc].header_ptr->ofhead->bitmaptable,blockNum)==0){
       BF_errno=-11;
       return BF_errno;//Error -> To block me arithmo blockNum den einai valid
    }
    for(j=0;j<BF_BUFFER_SIZE;j++){//Briskw apo poio arxeio proerxetai to dosmeno block
       if((strcmp(buffer[j].fname,of_table[fileDesc].header_ptr->fname)==0)&& buffer[j].blockNum==blockNum+1 && buffer[j].isfree==1){
           thesh=j;
           if(buffer[thesh].fpinned_table[fileDesc]==0){
              BF_errno=-14;
              return BF_errno;//To block den exei ginei pinned apo to arxeio me anagnwristiko arithmo fileDesc
           }
           else{//Alliws kanw tis katallhles enhmerwseis
              (buffer[thesh].fpinned_table[fileDesc])--;
              (buffer[thesh].ispinned)--;
              buffer[thesh].lastAccess=++Time;
              for(j=0;j<BF_BUFFER_SIZE;j++){//Sth thesh tou pinned_table pou briskotan to dosmeno block bazw 0 
                 if(of_table[fileDesc].pinned_table[j]==blockNum+1){
                    of_table[fileDesc].pinned_table[j]=0;          
                 }
              }//Enhmerwnw to isdirty symfwna me tous kanones pou isxyoun
              if(buffer[thesh].isdirty==FALSE){
                 buffer[thesh].isdirty=dirty;
              }
           }
           BF_errno=0;
           return BF_errno;//Epityxia
       }
    } 
    if(thesh<0){
       BF_errno=-4;
       return BF_errno;//Den uparxei to block sthn endiamesh mnhmh
    }
} 
void BF_PrintError(char *errString){

     
     printf("%s",errString);
    
     switch(BF_errno){
                      case 0: 
                           printf("OK\n");
                           break;
                      case -1:
                           printf("elleipsi mnhmhs\n");
                           break;
                      case -2: 
                           printf("elleipsi xwrou endiamesis mnhmhs\n");
                           break;
                      case -3: 
                           printf("block idi 'karfwmeno' sthn mnhmh\n");
                           break;
                      case -4: 
                           printf("block gia 'ksekarfwma' den einai sthn mnhmh\n");
                           break;
                      case -5: 
                           printf("bloch idi sthn mnhmh\n");
                           break;
                      case -6: 
                           printf("geniko sfalma Leitourgikou Systhmatos\n");
                           break;
                      case -7: 
                           printf("atelhs anagnwsi block\n");
                           break;
                      case -8: 
                           printf("ateles grapsimo se block\n");
                           break;
                      case -9: 
                           printf("atelhs anagnwsi block-kefalidas\n");
                           break;     
                      case -10: 
                           printf("ateles grapsimo se block-kefalida\n");
                           break;
                      case -11: 
                           printf("mh egkyros anagnwristikos arithmos block\n");
                           break;
                      case -12: 
                           printf("arxeio idi anoixto\n");
                           break;
                      case -13: 
                           printf("lista anoixtwn arxeiwn pliris\n");
                           break;
                      case -14: 
                           printf("mh egkyros anagnwristikos arithmos arxeiou\n");
                           break;
                      case -15: 
                           printf("telos arxeiou\n");
                           break;
                      case -16: 
                           printf("block idi diathesimo\n");
                           break;
                      case -17: 
                           printf("block idi 'ksekarfwmeno'\n");
                           break;
                      case -18:
                           printf("to arxeio den yparxei\n");
                           break;
                      case -19:
                           printf("to arxeio yparxei hdh\n");
                           break;    
                      }
}
