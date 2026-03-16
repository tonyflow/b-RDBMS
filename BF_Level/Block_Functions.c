#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "BF_Lib.h"

int BF_GetFirstBlock(int fileDesc,int *blockNum,char **blockBuf){

	int i,j,thesi=-1,min,valid_block=-1,blockinmem=0;
	int bitmappart,found=0;
	FILE *fp;

	if(fileDesc<0 || fileDesc>24){ // invalid file descriptor number
                  
        	BF_errno=BFE_FD;
                return BFE_FD;
    	}

	
	//we want to read the bitmap to find the first valid block

	for(i=0;i<256;i++){//256 integers in a header
		for(j=0;j<32;j++){//32 bits in each integer
			memcpy(&bitmappart,(headers[openfiles[fileDesc].hd_index].headerblock+i*sizeof(int)),sizeof(int));
			if(isvalid(bitmappart,j+1)==1){   //because the offset must be from 1 to 32
				valid_block=i*32+j;
				found=1;
				break;
			}
		}
		if(found==1)
			break;
	}
	
	
	if(valid_block==-1){//if no valid block is found
		BF_errno=BFE_NONVALIDBLOCK;
		return BFE_NONVALIDBLOCK;
	}

	*blockNum=valid_block;

	for(i=0;i<BF_BUFFER_SIZE;i++){//if the block is already in memory
		if(memory[i].blockNum==valid_block && strcmp(memory[i].filename,openfiles[fileDesc].filename)==0){
			BF_errno=BFE_BLOCKINBUF;
			lru++;
			memory[i].lru_counter=lru;
			if(memory[i].bytemap[fileDesc]==0){
				memory[i].pinned_counter++;
				memory[i].bytemap[fileDesc]=1;
			}
			memory[i].dirty=0;
			memory[i].isfree=1;
			*blockBuf=midmem+i*BF_BLOCK_SIZE;
			blockinmem=1;
			break;
			//return BFE_BLOCKINBUF;
		}
	}
	
	if(blockinmem==0){
		for(i=0;i<BF_BUFFER_SIZE;i++)
			if(memory[i].isfree==0){
				thesi=i;
				break;
			}
		
		if(flru(thesi,valid_block,fileDesc,0,blockBuf)<0)
			return BFE_NOBUF;
	}

	BF_errno=BFE_OK;
	return BFE_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int BF_GetNextBlock(int fileDesc,int *blockNum,char **blockBuf){
	
	int i,j,thesi=-1,min,valid_block=-1,first,f_offset;
	int bitmappart,found=0,blockinmem=0;
	FILE *fp;

    
	if(fileDesc<0 || fileDesc>24){ // invalid file descriptor number
                  
        	BF_errno=BFE_FD;
                return BFE_FD;
    	}


	/*if(openfiles[fileDesc].numofblocks==*blockNum){//if the file has no blocks
		BF_errno=BFE_EOF;
		return BFE_EOF;

	}*/


	first=(*blockNum+1) / 32;
	f_offset=(*blockNum+1) % 32;

	for(i=first;i<256;i++){
		if(i==first) // for the first integer, we look from the position of blockNum onwards
			for(j=f_offset;j<32;j++){
				memcpy(&bitmappart,(headers[openfiles[fileDesc].hd_index].headerblock+i*sizeof(int)),sizeof(int));
				if(isvalid(bitmappart,j+1)==1){
					valid_block=i*32+j;
					found=1;
					break;
				}
			}
		else//for the subsequent integers, we look from their first bit
			for(j=0;j<32;j++){
				memcpy(&bitmappart,(headers[openfiles[fileDesc].hd_index].headerblock+i*sizeof(int)),sizeof(int));
				if(isvalid(bitmappart,j+1)==1){
					valid_block=i*32+j;
					found=1;
					break;
				}
			}
		if(found==1)
			break;
	}

	if (valid_block==-1){
        	BF_errno=BFE_INVALIDBLOCK;
        	return BFE_INVALIDBLOCK;
     	}

	*blockNum=valid_block;


	for(i=0;i<BF_BUFFER_SIZE;i++){
		if(memory[i].blockNum==valid_block && strcmp(memory[i].filename,openfiles[fileDesc].filename)==0){
			BF_errno=BFE_BLOCKINBUF;
			lru++;
			memory[i].lru_counter=lru;
			if(memory[i].bytemap[fileDesc]==0){
				memory[i].pinned_counter++;
				memory[i].bytemap[fileDesc]=1;
			}
			memory[i].dirty=0;
			memory[i].isfree=1;
			*blockBuf=midmem+i*BF_BLOCK_SIZE;
			blockinmem=1;
			break;
			//return BFE_BLOCKINBUF;
		}
	}

	if(blockinmem==0){
		for(i=0;i<BF_BUFFER_SIZE;i++)
			if(memory[i].isfree==0){
				thesi=i;
				break;
			}
		
		if(flru(thesi,valid_block,fileDesc,0,blockBuf)<0)
			return BFE_NOBUF;
	}
	
	BF_errno=BFE_OK;
	return BFE_OK;





}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int BF_GetThisBlock(int fileDesc,int blockNum,char **blockBuf){
    
	int i,j,thesi=-1,min,valid_block=-1;
	int bitmappart,blockinmem=0;
	FILE *fp;

    
	if(fileDesc<0 || fileDesc>24){ // should be added everywhere
                  
        	BF_errno=BFE_FD;
                return BFE_FD;
    	}


    
	//we assume blockNum takes values 0,1,2...
	i=(blockNum) / 32; // int
	j=(blockNum) % 32; // offset

	memcpy(&bitmappart,(headers[openfiles[fileDesc].hd_index].headerblock+i*sizeof(int)),sizeof(int));
     
	if(isvalid(bitmappart,j+1)==1)
		valid_block=i*32+j;
 
    
    
	if (valid_block==-1){
		BF_errno=BFE_INVALIDBLOCK;
		return BFE_INVALIDBLOCK;
	}
    
	for(i=0;i<BF_BUFFER_SIZE;i++){
		if(memory[i].blockNum==valid_block && strcmp(memory[i].filename,openfiles[fileDesc].filename)==0){
			BF_errno=BFE_BLOCKINBUF;
			lru++;
			memory[i].lru_counter=lru;
			if(memory[i].bytemap[fileDesc]==0){
				memory[i].pinned_counter++;
				memory[i].bytemap[fileDesc]=1;
			}
			memory[i].dirty=0;
			memory[i].isfree=1;
			*blockBuf=midmem+i*BF_BLOCK_SIZE;
			blockinmem=1;
			break;
			//return BFE_BLOCKINBUF;
		}
	}

	if(blockinmem==0){
		for(i=0;i<BF_BUFFER_SIZE;i++)
			if(memory[i].isfree==0){
				thesi=i;
				break;
			}
		
		if(flru(thesi,valid_block,fileDesc,0,blockBuf)<0)
			return BFE_NOBUF;
	}
	

	BF_errno=BFE_OK;
	return BFE_OK;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int BF_UnpinBlock(int fileDesc, int blockNum, int dirty){

	int i,thesi=-1;	

	
	if(fileDesc<0 || fileDesc>24){ // should be added everywhere
                  
        	BF_errno=BFE_FD;
                return BFE_FD;
    	}

	for(i=0;i<BF_BUFFER_SIZE;i++)
		if(strcmp(openfiles[fileDesc].filename,memory[i].filename)==0)//if this block actually exists in memory
			if(memory[i].blockNum==blockNum){
				thesi=i;
				break;
			}
	
	if(thesi==-1){
		BF_errno=BFE_BLOCKNOTINBUF;
		return BFE_BLOCKNOTINBUF;
	}

	if(memory[thesi].bytemap[fileDesc]==1){//if it is pinned
		memory[thesi].pinned_counter--;
		memory[thesi].bytemap[fileDesc]=0;
	}
	else{//block already unpinned
		BF_errno=BFE_BLOCKUNFIXED; 
		return BFE_BLOCKUNFIXED;
	}
	
	if(dirty==1)	
		memory[thesi].dirty=1;


	if(dirty!=0 && dirty!=1){
		BF_errno=BFE_DIRTYFAULT;
		return BFE_DIRTYFAULT;
	}
	
	BF_errno=BFE_OK;
	return BFE_OK;


}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int BF_AllocBlock(int fileDesc,int *blockNum,char **blockBuf){

	int i,j,min,bitmappart,free_block=-1,thesi=-1,found=0;
	char newblock[BF_BLOCK_SIZE];	
	FILE *fp;
	
	if(fileDesc<0 || fileDesc>24){
                  
        	BF_errno=BFE_FD;
                return BFE_FD;
    	}
	
//	newblock=(char*)calloc(BF_BLOCK_SIZE,sizeof(char));
	
	for( i = 0 ; i < 1024 ; i ++ ){
		newblock[ i ] = '\0';
	} 

	//we want to read the bitmap to find the first free block

	for(i=0;i<256;i++){//256 integers in a header
		for(j=0;j<32;j++){//32 bits in each integer
			memcpy(&bitmappart,(headers[openfiles[fileDesc].hd_index].headerblock+i*sizeof(int)),sizeof(int));
			if(isvalid(bitmappart,j+1)==0){   //because the offset must be from 1 to 32
				set(&bitmappart,j+1,1);   //change the bit value in bitmappart from 0 to 1
				free_block=i*32+j;
				memcpy((headers[openfiles[fileDesc].hd_index].headerblock+i*sizeof(int)),&bitmappart,sizeof(int));
				found=1;				
				break;
			}
		}
		if(found==1)
			break;
	}
	
	if(free_block==-1){//if no free block is found
		BF_errno=BFE_NOFREEBLOCK;
		return BFE_NOFREEBLOCK;
	}

	*blockNum=free_block;

	fseek(openfiles[fileDesc].fp,(free_block+1)*BF_BLOCK_SIZE,SEEK_SET);//seek to the position where we need to place the new block in the file 																
	fwrite(newblock,1,BF_BLOCK_SIZE,openfiles[fileDesc].fp);//write the new block to disk
	fflush(openfiles[fileDesc].fp);

	for(i=0;i<BF_BUFFER_SIZE;i++)
		if(memory[i].isfree==0){
			thesi=i;
			break;
		}
	
	if(flru(thesi,free_block,fileDesc,1,blockBuf)<0)
		return BFE_NOBUF;
//	free(newblock);

	BF_errno=BFE_OK;
	return BFE_OK;



}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int BF_DisposeBlock(int fileDesc,int *blockNum){

	int i,j,bitmappart;

	if(fileDesc<0 || fileDesc>24){
                  
        	BF_errno=BFE_FD;
                return BFE_FD;
    	}	

	for(i=0;i<BF_BUFFER_SIZE;i++){//if it is pinned by someone in memory
		if(memory[i].blockNum==(*blockNum) && strcmp(memory[i].filename,openfiles[fileDesc].filename)==0 && memory[i].pinned_counter!=0){
			BF_errno=BFE_BLOCKFIXED;
			return BFE_BLOCKFIXED;
		}
	}	

	//we assume blockNum takes values 0,1,2...
	i=(*blockNum) / 32; // int
	j=(*blockNum) % 32; // offset

	memcpy(&bitmappart,(headers[openfiles[fileDesc].hd_index].headerblock+i*sizeof(int)),sizeof(int));
	if(isvalid(bitmappart,j+1)==1){
		set(&bitmappart,j+1,0);   //change the bit value in bitmappart from 1 to 0
		memcpy((headers[openfiles[fileDesc].hd_index].headerblock+i*sizeof(int)),&bitmappart,sizeof(int));//update the header
		for(i=0;i<BF_BUFFER_SIZE;i++)
			if(strcmp(openfiles[fileDesc].filename,memory[i].filename)==0 && memory[i].blockNum==*blockNum)
				memory[i].isfree=0;
	}
	else{
		BF_errno=BFE_ALREADY_DISPOSED;
		return BFE_ALREADY_DISPOSED;
	}
	
	
	BF_errno=BFE_OK;
	return BFE_OK;


}

