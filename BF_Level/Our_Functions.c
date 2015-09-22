#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "BF_Lib.h"

/////////////BITMAP FUNCTIONS/////////////////

//ta offset KAI sthn isavalid KAI sthn set diaxeirishs tou bitmap tha eimai apo 1 merxri 32 kai OXI apo 0 mexri 31

int isvalid(int bitmappart,int offset){
	
	
	if(((bitmappart & (1 << (31 - (offset - 1))))==0))
		return 0;	//an i AND praxi bgalei mono midenika,tote afou stin offset thesi eixame 1,sto bitmap tha exoume 0
	else
		return 1;
} 

void set(int* bitmappart,int offset,int what){
	if(what==1)
		 *bitmappart = *bitmappart | (1 << (31 - (offset - 1))); // apo 0 se 1 
	else
		*bitmappart = *bitmappart ^ (1 << (31 - (offset - 1))); 	// apo 1 se 0
}


//////////////////////////////////////////////////////////////////////////////////////

//////////////////////AUXILIARY FUNCTIONS///////////////////////////////////////////////

int flru(int thesi,int valid_block,int fileDesc,int dirty_value,char** blockBuf){

	int i;	
	FILE* fp;
	int min;
	
	if(thesi==-1){//politiki lru
		for(i=0;i<BF_BUFFER_SIZE;i++)//psaxnoume tin proti thesi gia tin opoia o pinned_counter einai 0,oste na ginei i arxikopoiisi gia tin euresi
					     //tou elaxistou lru_counter
			if(memory[i].pinned_counter==0){				
				min=memory[i].lru_counter;
				thesi=i;
				break;
			}

		if(thesi==-1){//an ola ta block tis endiamesis mnimis exoun pinned_counter!=0
			BF_errno=BFE_NOBUF;
			return BFE_NOBUF;
		}

		for(i=0;i<BF_BUFFER_SIZE;i++)//psaxnoume to ligotero prosfata xrisimopoiimeno block(to opoio den einai pinned)
			if(memory[i].lru_counter<min && memory[i].pinned_counter==0){
				min=memory[i].lru_counter;
				thesi=i;
			}
	}
	
	if(memory[thesi].dirty==1){//an to block einai bromismeno
		for(i=0;i<MAXOPENFILES;i++)
			if(strcmp(memory[thesi].filename,openfiles[i].filename)==0){
				fp=openfiles[i].fp;
				break;
			}	
		fseek(fp,(memory[thesi].blockNum+1)*BF_BLOCK_SIZE,SEEK_SET);//pame stin thesi pou brisketai to block sto arxeio(theoroume oti to blockNum pairnei times 1,2..)																
		fflush(fp);
		fwrite(midmem+thesi*BF_BLOCK_SIZE*sizeof(char),1,BF_BLOCK_SIZE,fp);//grapsimo tou block sto disko
		fflush(fp);
	}
	
	fflush(openfiles[fileDesc].fp);
	fseek(openfiles[fileDesc].fp,(valid_block+1)*BF_BLOCK_SIZE,SEEK_SET);//pame sti thesi tou arxeiou pou brisketai to block

	fread(midmem+thesi*BF_BLOCK_SIZE*sizeof(char),1,BF_BLOCK_SIZE,openfiles[fileDesc].fp);//to fernoume stin katallili thesi stin endiamesi mnimi
	fflush(openfiles[fileDesc].fp);

	//enimeronoume tin katallili thesi tou pinaka memory me tis plirofories tou block
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
