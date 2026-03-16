#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "BF_Lib.h"

/////////////BITMAP FUNCTIONS/////////////////

//the offsets in both isvalid and set for bitmap management will be from 1 to 32 and NOT from 0 to 31

int isvalid(int bitmappart,int offset){
	
	
	if(((bitmappart & (1 << (31 - (offset - 1))))==0))
		return 0;	//if the AND operation produces only zeros, then since we had 1 at the offset position, the bitmap has 0
	else
		return 1;
} 

void set(int* bitmappart,int offset,int what){
	if(what==1)
		 *bitmappart = *bitmappart | (1 << (31 - (offset - 1))); // from 0 to 1
	else
		*bitmappart = *bitmappart ^ (1 << (31 - (offset - 1))); 	// from 1 to 0
}


//////////////////////////////////////////////////////////////////////////////////////

//////////////////////AUXILIARY FUNCTIONS///////////////////////////////////////////////

int flru(int thesi,int valid_block,int fileDesc,int dirty_value,char** blockBuf){

	int i;	
	FILE* fp;
	int min;
	
	if(thesi==-1){//LRU policy
		for(i=0;i<BF_BUFFER_SIZE;i++)//search for the first position where pinned_counter is 0, to initialize the search
					     //for the minimum lru_counter
			if(memory[i].pinned_counter==0){				
				min=memory[i].lru_counter;
				thesi=i;
				break;
			}

		if(thesi==-1){//if all blocks in buffer memory have pinned_counter!=0
			BF_errno=BFE_NOBUF;
			return BFE_NOBUF;
		}

		for(i=0;i<BF_BUFFER_SIZE;i++)//search for the least recently used block (that is not pinned)
			if(memory[i].lru_counter<min && memory[i].pinned_counter==0){
				min=memory[i].lru_counter;
				thesi=i;
			}
	}
	
	if(memory[thesi].dirty==1){//if the block is dirty
		for(i=0;i<MAXOPENFILES;i++)
			if(strcmp(memory[thesi].filename,openfiles[i].filename)==0){
				fp=openfiles[i].fp;
				break;
			}	
		fseek(fp,(memory[thesi].blockNum+1)*BF_BLOCK_SIZE,SEEK_SET);//seek to the position of the block in the file (assuming blockNum takes values 1,2..)																
		fflush(fp);
		fwrite(midmem+thesi*BF_BLOCK_SIZE*sizeof(char),1,BF_BLOCK_SIZE,fp);//write the block to disk
		fflush(fp);
	}
	
	fflush(openfiles[fileDesc].fp);
	fseek(openfiles[fileDesc].fp,(valid_block+1)*BF_BLOCK_SIZE,SEEK_SET);//seek to the position of the block in the file

	fread(midmem+thesi*BF_BLOCK_SIZE*sizeof(char),1,BF_BLOCK_SIZE,openfiles[fileDesc].fp);//bring it to the appropriate position in buffer memory
	fflush(openfiles[fileDesc].fp);

	//update the appropriate position of the memory array with the block's information
	memory[thesi].blockNum=valid_block;
	lru++;
	memory[thesi].lru_counter=lru;
	memory[thesi].pinned_counter++;
	memory[thesi].dirty=dirty_value;
	memory[thesi].isfree=1;
	strcpy(memory[thesi].filename,openfiles[fileDesc].filename);
	memory[thesi].bytemap[fileDesc]=1;
	
	*blockBuf=midmem+thesi*BF_BLOCK_SIZE;

	return BFE_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
