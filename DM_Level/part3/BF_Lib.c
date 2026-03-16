#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "BF_Lib.h"
//HD level functions
int HD_exist(char *filename){
    
    FILE *fp;
    
    if ((fp=(fopen(filename,"rb")))==NULL){
       BF_errno=-6;
       return BF_errno;//Does not exist
    }
    else{
         fclose(fp);
         BF_errno=0;
         return 1;//Exists
         }
}
int HD_create(char *filename)
{
    FILE *fp;
    
    if ((fp=(fopen(filename,"wb")))==NULL){
       BF_errno=-6;
       return BF_errno;//fopen failed
    }
    else{
         fclose(fp);
         BF_errno=0;
         return 1;//Success
   }
}
int HD_remove(char *filename){
    if(HD_exist(filename)<0){
         BF_errno=-18;
         return BF_errno;//The file does not exist
    }
    if(remove(filename)!=0){
         BF_errno=-6;
         return BF_errno;//Error during file deletion
    }
    BF_errno=0;
    return 0;//Success
}

//Bitmap manipulation function. Sets to 1 the position in the table corresponding to the block
//of the file with number num_of_block. In other words, marks the block as valid.
void make_valid(char *bitmap,int num_of_block){
      int i,j;

     i=num_of_block%8;//Find the table position (each position is 1 byte) containing the bit corresponding to the file block
     j=num_of_block/8;//Within the cell, select the appropriate bit corresponding to the file block
     bitmap[j]=bitmap[j]|m_table[i];//Using the corresponding mask, perform bitwise-or to change 0 to 1
}
//Bitmap manipulation function. Sets to 0 the position in the table corresponding to the block
//of the file with number num_of_block. In other words, marks the block as invalid.
void make_invalid(char *bitmap,int num_of_block){
     int i,j;

     i=num_of_block%8;//Find the table position (each position is 1 byte) containing the bit corresponding to the file block
     j=num_of_block/8;//Within the cell, select the appropriate bit corresponding to the file block
     bitmap[j]=bitmap[j]&~m_table[i];//Using the corresponding mask, perform bitwise-and to change 1 to 0
}
//Bitmap manipulation function. Checks whether the position corresponding to the block of the file with
//number num_of_block is valid or invalid.
int is_valid(char *bitmap,int num_of_block){
     int i,j;

     i=num_of_block%8;//Find the table position (each position is 1 byte) containing the bit corresponding to the file block
     j=num_of_block/8;//Within the cell, select the appropriate bit corresponding to the file block
     if((bitmap[j]&m_table[i])!=0){ //If the result of bitwise-and with the appropriate mask is non-zero then
          return 1;//the block is valid
     }
     return 0;//otherwise the block is invalid
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
    
    //Checks
    if (HD_exist(filename)>0){
       BF_errno=-19;
       return BF_errno;//error -> the file already exists
     }
    if (HD_create(filename)<0){
       BF_errno=-6;
       return BF_errno;//error
    }
    if((fp=fopen(filename,"rb+"))==NULL){
       BF_errno=-6;
       return BF_errno;//error opening the file
       }
    //Memory allocation for the file header
    if((headerptr=malloc(sizeof(header)))==NULL){
       BF_errno=-1;
       return BF_errno;//error -> memory is full
       }
    //Copy the file name to its header
    strcpy(headerptr->fname,filename);
    //Initialize the elements contained in the file header
    headerptr->num_of_blocks=0;
    for( i=0; i<970; i++ ){
         (headerptr->bitmaptable[i])=0;
    }
    //Move the file pointer to the beginning
    if((fseek(fp,0,SEEK_SET))!=0){
        BF_errno=-6;
        return BF_errno;//error in fseek
    }
    //Write the header to the file
    fwrite(headerptr,BF_BLOCK_SIZE,1,fp);
    fclose(fp);//Close the file
    free(headerptr);//Free the memory allocated for the pointer
    BF_errno=0;
    return BF_errno;//Success
}
int BF_OpenFile(char *filename){
    int i,j,freeNum=-1,samefileNum;
    //The file exists in the open files table
    for(i=0;i<MAXOPENFILES;i++){
             if(of_table[i].isfree==0){//If the slot is empty, keep it
                 freeNum=i;
             }
             else{//Otherwise check if the file is already open in this slot
               if(strcmp(of_table[i].header_ptr->fname,filename)==0){
                 BF_errno=-12;
                 samefileNum=i;//Found the position of the open file in the open files table
                 if(freeNum<0){//If no empty slot has been found so far
                      for(j=i;j<MAXOPENFILES;j++){//Search until the end of the table to find the first free slot and stop
                          if(of_table[j].isfree==0){
                                freeNum=j;
                                break;
                          }
                      }
                      if(freeNum==-1){
                           BF_errno=-13;
                           return BF_errno;//The open files table is full
                      }
                 //At this point we have found the position in the table where the open file is located
                 //as well as an empty slot in the table.
                 }//Copy the data from one slot to the other
                 of_table[freeNum].isfree=1;
                 if((of_table[freeNum].fp=fopen(filename,"rb+"))==NULL){
                      BF_errno=-6;
                      return BF_errno;//error opening the file
                 }
                 of_table[freeNum].header_ptr=of_table[samefileNum].header_ptr;
                 (of_table[freeNum].header_ptr->openedNum)++;
                 return freeNum;//Success opening the file -> return the fileDesc
               }
             }
    }                       
    if(HD_exist(filename)<0){
       BF_errno=-18;
       return BF_errno;//Error -> The file does not exist
    }
    //The file does not exist in the open files table
    for(i=0;i<MAXOPENFILES;i++){
                   if((of_table[i].isfree)==0){//Find the first empty slot
                      //Allocate memory for the pointers, assign the file pointer value and perform necessary updates
                      of_table[i].isfree=1;
                      of_table[i].fp=fopen(filename,"rb+");
                      of_table[i].header_ptr=malloc(sizeof(h_struct));
                      of_table[i].header_ptr->ofhead=malloc(sizeof(header));
                      of_table[i].header_ptr->openedNum=1;
                      strcpy(of_table[i].header_ptr->fname,filename);
                      if((fseek(of_table[i].fp,0,SEEK_SET))!=0){
                         BF_errno=-6;
                         return BF_errno;//Error in fseek
                      }
                      fread(of_table[i].header_ptr->ofhead,BF_BLOCK_SIZE,1,of_table[i].fp);
                      return i;//Success writing the file to the table -> return the fileDesc
                   }
    }
    //If the value remains 0, meaning it never entered the above if
    BF_errno=-13;
    return BF_errno;//Error -> The open files table is full
}
int BF_DestroyFile(char *filename){
    int i;
    
    if(HD_exist(filename)<0){
          BF_errno=-18;
       return BF_errno;//Error -> The file does not exist
    }
    for(i=0;i<MAXOPENFILES;i++){
        if(of_table[i].isfree==1 && strcmp(of_table[i].header_ptr->fname,filename)==0){
                 BF_errno=-12;
                 return BF_errno;//Error -> The file is open
        }
    }//If the file does not exist in the open files table then we remove it
    BF_errno=HD_remove(filename);
    return BF_errno;
}
int BF_CloseFile(int fileDesc){
    int i,j;
    i=fileDesc;
    
    if(of_table[i].isfree==0){
          BF_errno=-14;
          return BF_errno;//Error -> Invalid fileDesc
    }
    for(j=0;j<BF_BUFFER_SIZE;j++){
        if(of_table[i].pinned_table[j]!=0){
              BF_errno=-3;
              return BF_errno;//Error -> A block of the file is pinned in memory
        }
    }
    //If the file is open more than once
    if(of_table[i].header_ptr->openedNum>1){
       (of_table[i].header_ptr->openedNum)--;
       of_table[i].header_ptr=NULL;
       of_table[i].isfree=0;
       fclose(of_table[i].fp);
       BF_errno=0;
       return BF_errno;//Success
    }//If some blocks of the file are dirty in memory, write them to disk before closing the file
    for(j=0;j<BF_BUFFER_SIZE;j++){
        if(strcmp(buffer[j].fname,of_table[i].header_ptr->fname)==0){
           if(buffer[j].isdirty==TRUE){
              if((fseek(of_table[i].fp,(buffer[j].blockNum)*BF_BLOCK_SIZE,SEEK_SET))!=0){
               BF_errno=-6;
               return BF_errno;//error in fseek
              }
              fwrite(buffer[j].data,BF_BLOCK_SIZE,1,of_table[i].fp);
           }
           buffer[j].isfree=0;
           buffer[j].isdirty=FALSE;
        }
    }//If the file is open only once
    free(of_table[i].header_ptr->ofhead);
    free(of_table[i].header_ptr);
    of_table[i].isfree=0;
    fclose(of_table[i].fp);
    BF_errno=0;
    return BF_errno;//Success
}
int BF_GetThisBlock(int fileDesc,int blockNum,char **blockbuf){
    int i,j,fbn=-1,min;
    if(of_table[fileDesc].isfree==0){
       BF_errno=-14;
       return BF_errno;//Error -> Invalid fileDesc
    }
    if(is_valid(of_table[fileDesc].header_ptr->ofhead->bitmaptable,blockNum)==0){
           BF_errno=-11;
           return BF_errno;;//Error -> The block with number blockNum is not valid
    }
    //Check if the block exists in the buffer; if it does, perform the necessary updates
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
          return BF_errno;//Success
       }
    }
    //The block does not exist in the buffer
    //Find space in the buffer
    for(j=0;j<BF_BUFFER_SIZE;j++){
        if(buffer[j].isfree==0){
           fbn=j;//Found the first free slot in the buffer
           break;
        }
    }//If there is no free slot in the buffer, use LRU
    //LRU works as follows: Each time an operation is performed on a block, the global variable Time is incremented and assigned to
    //lastAccess. The block with the smallest lastAccess value, which is not pinned in memory, is the candidate for replacement
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
          return BF_errno;//Error -> All blocks are pinned, LRU cannot work
       }//If the block to be replaced is dirty, write its contents to disk
       if(buffer[fbn].isdirty==TRUE){
          for(i=0;i<MAXOPENFILES;i++){
            if(strcmp(buffer[fbn].fname,of_table[i].header_ptr->fname)==0){
               break;
            }
          }
          if((fseek(of_table[i].fp,(buffer[fbn].blockNum)*BF_BLOCK_SIZE,SEEK_SET))!=0){
              BF_errno=-6;
              return BF_errno;//Error in fseek
          }
          fwrite(buffer[fbn].data,BF_BLOCK_SIZE,1,of_table[i].fp);
       }
    }
    //Bring the block into memory, perform updates
    buffer[fbn].isfree=1;
    strcpy(buffer[fbn].fname,of_table[fileDesc].header_ptr->fname);
    buffer[fbn].blockNum=blockNum+1;
    buffer[fbn].fpinned_table[fileDesc]+=1;
    buffer[fbn].isdirty=FALSE;
    buffer[fbn].ispinned++;
    buffer[fbn].lastAccess=++Time;
    if((fseek(of_table[fileDesc].fp,(blockNum+1)*BF_BLOCK_SIZE,SEEK_SET))!=0){
	   BF_errno=-6;
       return BF_errno;//Error in fseek
    }
    fread(buffer[fbn].data,BF_BLOCK_SIZE,1,of_table[fileDesc].fp);
    if((fseek(of_table[fileDesc].fp,0,SEEK_SET))!=0){
       BF_errno=-6;
       return BF_errno;//Error in fseek
    }
    //Update the pinned_table
    for(j=0;j<BF_BUFFER_SIZE;j++){
        if(of_table[fileDesc].pinned_table[j]==0){
           of_table[fileDesc].pinned_table[j]=blockNum+1;
           break;          
        }
    }
    *blockbuf=buffer[fbn].data;
    BF_errno=0;
    return BF_errno;//Success
}
int BF_AllocBlock(int fileDesc,int *blockNum,char **blockbuf){
    int nobs,i,j,kenh_thesh=-1,fbn=-1,min;
    char data[BF_BLOCK_SIZE];
    if(of_table[fileDesc].isfree==0){
       BF_errno=-14;
       return BF_errno;//Error -> Invalid fileDesc
    }
    nobs=of_table[fileDesc].header_ptr->ofhead->num_of_blocks;
    for(i=0;i<nobs;i++){
        if(is_valid(of_table[fileDesc].header_ptr->ofhead->bitmaptable,i)==0){
           kenh_thesh=i+1;
           break;//Found the first empty block at position i
        }
    }
    //There is no empty block in the file
    //Create a new block at the end of the file
    if(kenh_thesh<0){
       if((fseek(of_table[fileDesc].fp,0,SEEK_END))!=0){
           BF_errno=-6;
           return BF_errno;//Error in fseek
       }
       memset(data,0,BF_BLOCK_SIZE);
       fwrite(data,BF_BLOCK_SIZE,1,of_table[fileDesc].fp);
       nobs++;
	   kenh_thesh=nobs;
    }
    for(j=0;j<BF_BUFFER_SIZE;j++){
        if(buffer[j].isfree==0){
           fbn=j;//Found the first free slot in the buffer
           break;
        }
    }
    //If there is no free slot in the buffer then use LRU
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
          return BF_errno;;///Error -> All blocks are pinned, LRU cannot work
       }//If the block to be replaced is dirty, write its contents to disk
       if(buffer[fbn].isdirty==TRUE){
          for(i=0;i<MAXOPENFILES;i++){
            if(strcmp(buffer[fbn].fname,of_table[i].header_ptr->fname)==0){                     
               break;
            }                
          }
          if((fseek(of_table[i].fp,(buffer[fbn].blockNum)*BF_BLOCK_SIZE,SEEK_SET))!=0){
              BF_errno=-6;
              return BF_errno;//Error in fseek
          }
          fwrite(buffer[fbn].data,BF_BLOCK_SIZE,1,of_table[i].fp);
       }
    }
    //Bring the block into memory, perform updates
    buffer[fbn].isfree=1;
    strcpy(buffer[fbn].fname,of_table[fileDesc].header_ptr->fname);
    buffer[fbn].blockNum=kenh_thesh;
    buffer[fbn].fpinned_table[fileDesc]+=1;
    buffer[fbn].isdirty=FALSE;
    buffer[fbn].ispinned++;
    buffer[fbn].lastAccess=++Time;
    if((fseek(of_table[fileDesc].fp,kenh_thesh*BF_BLOCK_SIZE,SEEK_SET))!=0){
       BF_errno=-6;
       return BF_errno;//Error in fseek
    }
    fread(buffer[fbn].data,BF_BLOCK_SIZE,1,of_table[fileDesc].fp);
    of_table[fileDesc].header_ptr->ofhead->num_of_blocks=nobs;
    make_valid(of_table[fileDesc].header_ptr->ofhead->bitmaptable,kenh_thesh-1);
    if((fseek(of_table[fileDesc].fp,0,SEEK_SET))!=0){
	   BF_errno=-6;
       return BF_errno;//Error in fseek
    }
    fwrite(of_table[fileDesc].header_ptr->ofhead,BF_BLOCK_SIZE,1,of_table[fileDesc].fp);
    if((fseek(of_table[fileDesc].fp,0,SEEK_SET))!=0){
       BF_errno=-6;
       return BF_errno;//Error in fseek
    }//Update the pinned_table
    for(j=0;j<BF_BUFFER_SIZE;j++){
        if(of_table[fileDesc].pinned_table[j]==0){
           of_table[fileDesc].pinned_table[j]=kenh_thesh;
           break;          
        }
    }
    *blockNum=kenh_thesh-1;
    *blockbuf=buffer[fbn].data;
    BF_errno=0;
    return BF_errno;//Success  
}
int BF_GetFirstBlock(int fileDesc,int *blockNum,char **blockbuf){
    int nobs,i,NumBlock=-1;
    if(of_table[fileDesc].isfree==0){
       BF_errno=-14;
       return BF_errno;//Error -> Invalid fileDesc
    }
    nobs=of_table[fileDesc].header_ptr->ofhead->num_of_blocks;
    for(i=0;i<nobs;i++){
        if(is_valid(of_table[fileDesc].header_ptr->ofhead->bitmaptable,i)==1){
           NumBlock=i+1;
           break;//Found the first valid block at position i+1
        }
    }
    //There is no valid block in the file
    if(NumBlock<0){
       BF_errno=-11;
       return BF_errno;
    }
    *blockNum=NumBlock-1;
    //Call BF_GetThisBlock with the first valid block (if it exists) that we found
    return BF_GetThisBlock(fileDesc,NumBlock-1,blockbuf);    
}
int BF_GetNextBlock(int fileDesc,int *blockNum,char **blockbuf){
    int nobs,i,NumBlock=-1;
    if(of_table[fileDesc].isfree==0){
       BF_errno=-14;
       return BF_errno;//Error -> Invalid fileDesc
    }
    nobs=of_table[fileDesc].header_ptr->ofhead->num_of_blocks;
    for(i=(*blockNum+1);i<nobs;i++){
        if(is_valid(of_table[fileDesc].header_ptr->ofhead->bitmaptable,i)==1){
           NumBlock=i+1;
           break;//Found the first valid block, after the given block, at position i+1
        }
    }
    //There is no valid block in the file after the given block
    if(NumBlock<0){
       BF_errno=-11;
       return BF_errno;;
    }
    *blockNum=NumBlock-1;
    //Call BF_GetThisBlock with the first valid block (if it exists), after the given block, that we found
    return BF_GetThisBlock(fileDesc,NumBlock-1,blockbuf);
}
int BF_DisposeBlock(int fileDesc,int *blockNum)
{
    int j;

    if(of_table[fileDesc].isfree==0){
       BF_errno=-14;
       return BF_errno;//Error -> Invalid fileDesc
    }
    if(is_valid(of_table[fileDesc].header_ptr->ofhead->bitmaptable,(*blockNum))==0){
       BF_errno=-11;
       return BF_errno;//Error -> The block with number blockNum is not valid
    }
    //Check if the given block exists in the buffer
    for(j=0;j<BF_BUFFER_SIZE;j++){
       if((strcmp(buffer[j].fname,of_table[fileDesc].header_ptr->fname)==0)&& buffer[j].blockNum==(*blockNum+1) && buffer[j].isfree==1){
              if(buffer[j].ispinned!=0){
              BF_errno=-3;
              return BF_errno;//The block is pinned in the buffer
           }
           buffer[j].isfree=0;//Otherwise free the slot in the buffer
	       break;
       }
    }
    //Set the block's bitmap position to 0 and update the file header
    make_invalid(of_table[fileDesc].header_ptr->ofhead->bitmaptable,*blockNum);
    if((fseek(of_table[fileDesc].fp,0,SEEK_SET))!=0){
            BF_errno=-6;
            return BF_errno;//Error in fseek
    }
    fwrite(of_table[fileDesc].header_ptr->ofhead,BF_BLOCK_SIZE,1,of_table[fileDesc].fp);
    if((fseek(of_table[fileDesc].fp,0,SEEK_SET))!=0){
       BF_errno=-6;
       return BF_errno;//Error in fseek
    }
    BF_errno=0;
    return BF_errno;//Success
}
int BF_UnpinBlock(int fileDesc,int blockNum,int dirty){
    int j,thesh=-1;
    if(of_table[fileDesc].isfree==0){
       BF_errno=-14;
       return BF_errno;//Error -> Invalid fileDesc
    }
    if(is_valid(of_table[fileDesc].header_ptr->ofhead->bitmaptable,blockNum)==0){
       BF_errno=-11;
       return BF_errno;//Error -> The block with number blockNum is not valid
    }
    for(j=0;j<BF_BUFFER_SIZE;j++){//Find which file the given block belongs to
       if((strcmp(buffer[j].fname,of_table[fileDesc].header_ptr->fname)==0)&& buffer[j].blockNum==blockNum+1 && buffer[j].isfree==1){
           thesh=j;
           if(buffer[thesh].fpinned_table[fileDesc]==0){
              BF_errno=-14;
              return BF_errno;//The block has not been pinned by the file with identifier fileDesc
           }
           else{//Otherwise perform the appropriate updates
              (buffer[thesh].fpinned_table[fileDesc])--;
              (buffer[thesh].ispinned)--;
              buffer[thesh].lastAccess=++Time;
              for(j=0;j<BF_BUFFER_SIZE;j++){//Set the pinned_table position where the given block was to 0
                 if(of_table[fileDesc].pinned_table[j]==blockNum+1){
                    of_table[fileDesc].pinned_table[j]=0;          
                 }
              }//Update isdirty according to the applicable rules
              if(buffer[thesh].isdirty==FALSE){
                 buffer[thesh].isdirty=dirty;
              }
           }
           BF_errno=0;
           return BF_errno;//Success
       }
    } 
    if(thesh<0){
       BF_errno=-4;
       return BF_errno;//The block does not exist in the buffer
    }
} 
void BF_PrintError(char *errString){

     
     printf("%s",errString);
    
     switch(BF_errno){
                      case 0: 
                           printf("OK\n");
                           break;
                      case -1:
                           printf("out of memory\n");
                           break;
                      case -2:
                           printf("out of buffer space\n");
                           break;
                      case -3:
                           printf("block already pinned in memory\n");
                           break;
                      case -4:
                           printf("block to unpin is not in memory\n");
                           break;
                      case -5:
                           printf("block already in memory\n");
                           break;
                      case -6:
                           printf("general OS error\n");
                           break;
                      case -7:
                           printf("incomplete block read\n");
                           break;
                      case -8:
                           printf("incomplete block write\n");
                           break;
                      case -9:
                           printf("incomplete header-block read\n");
                           break;
                      case -10:
                           printf("incomplete header-block write\n");
                           break;
                      case -11:
                           printf("invalid block identifier\n");
                           break;
                      case -12:
                           printf("file already open\n");
                           break;
                      case -13:
                           printf("open file list is full\n");
                           break;
                      case -14:
                           printf("invalid file identifier\n");
                           break;
                      case -15:
                           printf("end of file\n");
                           break;
                      case -16:
                           printf("block already available\n");
                           break;
                      case -17:
                           printf("block already unpinned\n");
                           break;
                      case -18:
                           printf("file does not exist\n");
                           break;
                      case -19:
                           printf("file already exists\n");
                           break;    
                      }
}
