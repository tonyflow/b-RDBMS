#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "BF_Lib.h"

void BF_Init(){

	int i,j;


	lru=0;
	midmem=(char*)malloc(BF_BUFFER_SIZE*BF_BLOCK_SIZE*sizeof(char));  // initialization of buffer memory
	if(midmem==NULL){
		printf("Calloc error\n");
		exit(1);
	}	


	//initialization of the openfiles array
	for(i=0;i<MAXOPENFILES;i++){
		openfiles[i].isfree=0;
		openfiles[i].filename=(char*)calloc(NAMELENGTH,sizeof(char));
		if(openfiles[i].filename==NULL){
		printf("Calloc error\n");
		exit(1);
	}

	}

	//initialization of the headers array
	for(i=0;i<MAXOPENFILES;i++){
		headers[i].headerblock=(char*)calloc(BF_BLOCK_SIZE,sizeof(char));
		if(headers[i].headerblock==NULL){
		printf("Calloc error\n");
		exit(1);
	}
		headers[i].counter=0;
	}

	//initialization of the memory array
	for(i=0;i<BF_BUFFER_SIZE;i++){
		memory[i].filename=(char*)calloc(NAMELENGTH,sizeof(char));
		if(memory[i].filename==NULL){
		printf("Calloc error\n");
		exit(1);
	}
		memory[i].lru_counter=0;
		memory[i].pinned_counter=0;
		memory[i].dirty=0;
		memory[i].isfree=0;
		memory[i].bytemap=(int*)calloc(MAXOPENFILES,sizeof(int));
		if(memory[i].bytemap==NULL){
		printf("Calloc error\n");
		exit(1);
	}
		for(j=0;j<MAXOPENFILES;j++)
			memory[i].bytemap[j]=0;
	}	

}


int BF_CreateFile(char *fileName){

	FILE *fp,*fpcheck;
	char *header;
	int i;

	fpcheck=fopen(fileName,"rb");
	if(fpcheck==NULL){	//the file does not exist, so we create it

		fp=fopen(fileName,"wb+");
		if(fp==NULL){
			BF_errno=-19;
			return BFE_OPENFILE;
		}

		header=(char*)calloc(BF_BLOCK_SIZE,sizeof(char));  //allocation and zeroing of the header bits
		if(header==NULL){
		printf("Calloc error\n");
		exit(1);
	}		
	
		fwrite(header,1,sizeof(header),fp);	//write the header to the file
		fflush(fp);
	
		fclose(fp);

		return BFE_OK;
	
	}
	else{	//the file already exists
		BF_errno=-18;
		return BFE_EXISTINGFILE;

	}
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int BF_DestroyFile(char *fileName){

	FILE *fpcheck;
	int i;

	fpcheck=fopen(fileName,"rb");
	if(fpcheck==NULL){	//the file does not exist, so we return an error code
		BF_errno=-19;
		return BFE_OPENFILE;
	}

	for(i=0;i<MAXOPENFILES;i++){//check if the file is open
		if(strcmp(openfiles[i].filename,fileName)==0 && openfiles[i].isfree==1){
			BF_errno=-21;
			return BFE_DESTROYERR;
		}
	}

	if(remove(fileName)){
		BF_errno=BFE_REMOVEFILE;
		return BFE_REMOVEFILE;	
	}
	else{
		BF_errno=BFE_OK;
		return BFE_OK;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////

int BF_OpenFile(char *fileName){

	FILE *fp;
	int i,fd,fullarray=0,isopen=0,thesi,found=0;

	for(i=0;i<MAXOPENFILES;i++){//check if the file is already open
		if(strcmp(openfiles[i].filename,fileName)==0){
			BF_errno=-12;
			isopen=1;
			break;
  		}
	}
	
		fp=fopen(fileName,"rb+");//open for update (read-write) of an existing file
		if(fp==NULL){
			BF_errno=-19;
			return BFE_OPENFILE;//problem opening file
		}

	for(i=0;i<MAXOPENFILES;i++)	//the first free position in the array is the file descriptor
		if(openfiles[i].isfree==0){
			fd=i;
			fullarray=1;
			break;
		}


	if(fullarray==0){          //check for full open files list, i.e. cannot open another file
		BF_errno=BFE_FTABFULL;
		return BFE_FTABFULL;
	}


	//now we update this position of the open files array with the appropriate information
	strcpy(openfiles[fd].filename,fileName);
	
	openfiles[fd].fp=fp;
		
	if(isopen==1){
		for(i=0;i<MAXOPENFILES;i++)
			if(strcmp(openfiles[i].filename,fileName)==0 && fd!=i){
				openfiles[fd].hd_index=openfiles[i].hd_index;//to avoid fetching the header from disk again, set hd_index to point to the
													//position in the header struct array where the already existing file is located
				break;
			}
	}
	
	openfiles[fd].isfree=1;
	

	if(isopen==0){//if this file is not in the open files array, we need to bring the header into memory
		fseek(openfiles[fd].fp,0,SEEK_SET);	//go to the beginning of the file (just to be safe)
		for(i=0;i<MAXOPENFILES;i++)
			if(headers[i].counter==0){
				thesi=i;
				break;
			}
		fflush(openfiles[fd].fp);
		fread(headers[thesi].headerblock,1,BF_BLOCK_SIZE,openfiles[fd].fp);//bring the header into memory
		fflush(openfiles[fd].fp);
		openfiles[fd].hd_index=thesi;//set the index to point to the header
	}

	headers[openfiles[fd].hd_index].counter++;   //increment the number of open files (counter) that use this INDEX header block
	return fd;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
int BF_CloseFile(int fileDesc){
	//to close the file, no block of it should be pinned in memory by this fileDesc. Also, if all blocks are unpinned, before
	//closing the file we must check which of them are dirty so we write them back to disk, and then close the file
	int i,j,pinned=0;
	
	
	if(fileDesc<0 || fileDesc>24){ // invalid file descriptor number

        	BF_errno=BFE_FD;
                return BFE_FD;
    	}

	for(i=0;i<BF_BUFFER_SIZE;i++)
		if(strcmp(openfiles[fileDesc].filename,memory[i].filename)==0)//if the file has a block in memory
			for(j=0;j<BF_BUFFER_SIZE;j++){
				if(memory[j].bytemap[fileDesc]==1){//if a block is pinned by this fileDesc
					pinned=1;
					break;
				}
			}
		
	if(pinned==0){//if there is no pinned block of the file in memory
		for(i=0;i<BF_BUFFER_SIZE;i++)
			if(strcmp(openfiles[fileDesc].filename,memory[i].filename)==0)
				if(memory[i].dirty==1){//if the block is dirty
					fseek(openfiles[fileDesc].fp,(memory[i].blockNum+1)*BF_BLOCK_SIZE,SEEK_SET);//seek to the position of the block in the file (assuming blockNum takes values 0,1,2..)																
					fwrite(midmem+i*BF_BLOCK_SIZE*sizeof(char),1,BF_BLOCK_SIZE,openfiles[fileDesc].fp);//write the block to disk
					fflush(openfiles[fileDesc].fp);
				}
				
		headers[openfiles[fileDesc].hd_index].counter--;
		if(headers[openfiles[fileDesc].hd_index].counter==0){//if no one is using the header, write it back to the file
			fseek(openfiles[fileDesc].fp,0,SEEK_SET);
			fwrite(headers[openfiles[fileDesc].hd_index].headerblock,1,BF_BLOCK_SIZE,openfiles[fileDesc].fp);
			fflush(openfiles[fileDesc].fp);
			for(i=0;i<BF_BUFFER_SIZE;i++)
				//if this is the last one closing the file
				if(strcmp(memory[i].filename,openfiles[fileDesc].filename)==0)
					memory[i].isfree=0;
		}				
			
		fflush(openfiles[fileDesc].fp);
		
		if(fclose(openfiles[fileDesc].fp)==0)//close the file and reinitialize the members of the open files array position
			openfiles[fileDesc].isfree=0;
		else{//error in fclose
			perror("\n\nerror in fp\n\n");
			BF_errno=BFE_CLOSEFILE;
			return BFE_CLOSEFILE;
		}
	}
	else{//pinned=1 so return an error about attempting to close while a block is pinned in memory
		BF_errno=BFE_PINNEDCLOSE;
		return BFE_PINNEDCLOSE;
	}

	BF_errno=BFE_OK;
        return BFE_OK;

}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


