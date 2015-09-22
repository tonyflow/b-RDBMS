#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "BF_Lib.h"

int BF_GetFirstBlock(int fileDesc,int *blockNum,char **blockBuf){

	int i,j,thesi=-1,min,valid_block=-1,blockinmem=0;
	int bitmappart,found=0;
	FILE *fp;

	if(fileDesc<0 || fileDesc>24){ // mh egkyros anagnoristikos arithmos arxeiou
                  
        	BF_errno=BFE_FD;
                return BFE_FD;
    	}

	
	//theloume na diabasoume to bitmap gia na broume to proto egkiro block
	
	for(i=0;i<256;i++){//256 akeraioi se ena header
		for(j=0;j<32;j++){//32 bits se kathe akeraio				
			memcpy(&bitmappart,(headers[openfiles[fileDesc].hd_index].headerblock+i*sizeof(int)),sizeof(int));
			if(isvalid(bitmappart,j+1)==1){   //epidi to offset prepei na einai apo 1 mexri 32
				valid_block=i*32+j;
				found=1;
				break;
			}
		}
		if(found==1)
			break;
	}
	
	
	if(valid_block==-1){//an den brethei egkiro block
		BF_errno=BFE_NONVALIDBLOCK;
		return BFE_NONVALIDBLOCK;
	}

	*blockNum=valid_block;

	for(i=0;i<BF_BUFFER_SIZE;i++){//an to block einai idi sti mnimi
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

    
	if(fileDesc<0 || fileDesc>24){ // mh egkyros anagnoristikos arithmos arxeiou
                  
        	BF_errno=BFE_FD;
                return BFE_FD;
    	}


	/*if(openfiles[fileDesc].numofblocks==*blockNum){//an to arxeio den exei kanena block
		BF_errno=BFE_EOF;
		return BFE_EOF;

	}*/


	first=(*blockNum+1) / 32;
	f_offset=(*blockNum+1) % 32;

	for(i=first;i<256;i++){
		if(i==first) // gia ton proto akeraio koitame apo ti thesi tou blockNum kai meta
			for(j=f_offset;j<32;j++){
				memcpy(&bitmappart,(headers[openfiles[fileDesc].hd_index].headerblock+i*sizeof(int)),sizeof(int));
				if(isvalid(bitmappart,j+1)==1){
					valid_block=i*32+j;
					found=1;
					break;
				}
			}
		else//gia tous epomenous akeraious koitame apo to proto tous bit
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

    
	if(fileDesc<0 || fileDesc>24){ // na mpei pantou
                  
        	BF_errno=BFE_FD;
                return BFE_FD;
    	}


    
	//theoroume to blocknum oti pairnei times 0,1,2...
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

	
	if(fileDesc<0 || fileDesc>24){ // na mpei pantou
                  
        	BF_errno=BFE_FD;
                return BFE_FD;
    	}

	for(i=0;i<BF_BUFFER_SIZE;i++)
		if(strcmp(openfiles[fileDesc].filename,memory[i].filename)==0)//an ontos iparxei sti mnimi to block auto
			if(memory[i].blockNum==blockNum){
				thesi=i;
				break;
			}
	
	if(thesi==-1){
		BF_errno=BFE_BLOCKNOTINBUF;
		return BFE_BLOCKNOTINBUF;
	}

	if(memory[thesi].bytemap[fileDesc]==1){//an einai pinned
		memory[thesi].pinned_counter--;
		memory[thesi].bytemap[fileDesc]=0;
	}
	else{//block idi ksekarfomeno
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

	//theloume na diabasoume to bitmap gia na broume to proto egkiro block
	
	for(i=0;i<256;i++){//256 akeraioi se ena header
		for(j=0;j<32;j++){//32 bits se kathe akeraio				
			memcpy(&bitmappart,(headers[openfiles[fileDesc].hd_index].headerblock+i*sizeof(int)),sizeof(int));
			if(isvalid(bitmappart,j+1)==0){   //epidi to offset prepei na einai apo 1 mexri 32
				set(&bitmappart,j+1,1);   //allazoume thn timh tou bit tou bitmappart apo 0 se 1
				free_block=i*32+j;
				memcpy((headers[openfiles[fileDesc].hd_index].headerblock+i*sizeof(int)),&bitmappart,sizeof(int));
				found=1;				
				break;
			}
		}
		if(found==1)
			break;
	}
	
	if(free_block==-1){//an den brethei elethero block
		BF_errno=BFE_NOFREEBLOCK;
		return BFE_NOFREEBLOCK;
	}

	*blockNum=free_block;

	fseek(openfiles[fileDesc].fp,(free_block+1)*BF_BLOCK_SIZE,SEEK_SET);//pame stin thesi pou prepei na baloume to neo block sto arxeio 																
	fwrite(newblock,1,BF_BLOCK_SIZE,openfiles[fileDesc].fp);//grapsimo tou neou block sto disko
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

	for(i=0;i<BF_BUFFER_SIZE;i++){//an einai pinned apo kapoion sti mnimi
		if(memory[i].blockNum==(*blockNum) && strcmp(memory[i].filename,openfiles[fileDesc].filename)==0 && memory[i].pinned_counter!=0){
			BF_errno=BFE_BLOCKFIXED;
			return BFE_BLOCKFIXED;
		}
	}	

	//theoroume to blocknum oti pairnei times 0,1,2...
	i=(*blockNum) / 32; // int
	j=(*blockNum) % 32; // offset

	memcpy(&bitmappart,(headers[openfiles[fileDesc].hd_index].headerblock+i*sizeof(int)),sizeof(int));
	if(isvalid(bitmappart,j+1)==1){
		set(&bitmappart,j+1,0);   //allazoume thn timh tou bit tou bitmappart apo 1 se 0
		memcpy((headers[openfiles[fileDesc].hd_index].headerblock+i*sizeof(int)),&bitmappart,sizeof(int));//enimeronoume to header
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

