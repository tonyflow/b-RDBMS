#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "BF_Lib.h"
#include "AM_Lib.h"



int ReadHeader(int header_pos,char **blockBuf){

	int blockno;//the block number (pointer) stored at position header_pos
	int i;
	char *str;
	short int temp;

	//str=malloc(sizeof(int));

	if(header_pos==-1){//with argument -1, gets the total depth from the header (which is an int)
		str=malloc(sizeof(int));
		strncpy(str,*blockBuf,sizeof(int));
		memcpy(&blockno,str,sizeof(int));
		//memcpy(&blockno,*blockBuf,sizeof(int));
		//blockno=*((int*)(*blockBuf));
		//return blockno;
		//blockno=atoi(str);
		return blockno;
	}
	else if(header_pos==-2){//with argument -2, gets the local depth of the bucket (which is a short int)
		str=malloc(sizeof(short int));
		strncpy(str,*blockBuf,sizeof(short int));
		memcpy(&temp,str,sizeof(short int));
		//memcpy(&temp,*blockBuf,sizeof(short int));
		//temp=atoi(str);
		return temp;
	}
	else{//otherwise advances header_pos positions past the total depth to get the desired pointer
		str=malloc(sizeof(short int));	
		*blockBuf += sizeof(int);
		for(i=0;i<header_pos;i++)	
			*blockBuf += sizeof(short int);
		strncpy(str,*blockBuf,sizeof(short int));
		memcpy(&temp,str,sizeof(short int));
		//memcpy(&temp,*blockBuf,sizeof(short int));
		//temp=atoi(str);
		//now we return the pointer to the beginning
		for(i=0;i<header_pos;i++)	
			*blockBuf -= sizeof(short int);
		*blockBuf -= sizeof(int);
		return temp;
	}

	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WriteHeader(int header_pos,int blockNum,char **blockBuf){
	
	int i,help;
	short int temp;
	
	if(header_pos==-1){//with argument -1, writes the total depth
		help=blockNum;
		memcpy(*blockBuf,&help,sizeof(int));
	}
	else if(header_pos==-2){//for the local depth
		temp=(short int)blockNum;
		memcpy(*blockBuf,&temp,sizeof(short int));
	}
	else{//otherwise advances header_pos positions past the total depth to write the desired pointer
		temp=(short int)blockNum;
		*blockBuf += sizeof(int);
		for(i=0;i<header_pos;i++)	
			*blockBuf += sizeof(short int);
		memcpy(*blockBuf,&temp,sizeof(short int));
		//now we return the pointer to the beginning
		for(i=0;i<header_pos;i++)	
			*blockBuf -= sizeof(short int);
		*blockBuf -= sizeof(int);
		
	}


}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int is_valid(char hv,int offset){
	
	
	if(((hv & (1 << (7 - (offset - 1))))==0))
		return 0;	//if the AND operation yields only zeros, then since we had 1 at the offset position, the bitmap has 0
	else
		return 1;
} 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void set_bit(char* bitmappart,int offset,int what){
	if(what==1)
		 *bitmappart = *bitmappart | (1 << (7 - (offset - 1))); // from 0 to 1
	else
		*bitmappart = *bitmappart ^ (1 << (7 - (offset - 1))); 	// from 1 to 0
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int JSHash(char* str, unsigned int len)
{
   unsigned int hash = 1315423911;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash ^= ((hash << 5) + (*str) + (hash >> 2));
   }

   return hash;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AM_Init(){

	int i;

	BF_Init();
	
	for(i=0;i<MAXINDEXES;i++){
		indexes[i].isfree=0;
		indexes[i].indexNo=-1;
		indexes[i].filename=(char*)calloc(NAMELENGTH,sizeof(char));
		if(indexes[i].filename==NULL){
			printf("Calloc error\n");
			exit(1);
		}
	}
	
	for(i=0;i<MAXSCANS;i++)
		scans[i].isfree=0;
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int AM_CreateIndex(char* filename,int indexNo,char attrType,int attrLength){

	int fd,i,pos=-1;
	int blockNum;
	char *blockBuf;
	char name[NAMELENGTH];
	
	for(i=0;i<MAXINDEXES;i++)//check if the indexNo is unique or already exists for some index
		if(strcmp(indexes[i].filename,filename)==0 && indexes[i].indexNo==indexNo){
			AM_errno=AME_INDEXNO;
			return AM_errno;
		}
	
	for(i=0;i<MAXINDEXES;i++)//search for a free position in the index array
		if(indexes[i].isfree==0){
			pos=i;
			break;
		}
	
	if(pos==-1){//if there is no free position
		AM_errno=AME_FULLINDEXARRAY;
		return AM_errno;
	}
	
	//update the information
	strcpy(indexes[pos].filename,filename);
	indexes[pos].indexNo=indexNo;
	indexes[pos].attrType=attrType;
	indexes[pos].attrLength=attrLength;
	indexes[pos].isfree=1;
	indexes[pos].recsize=attrLength+sizeof(int);//attribute length + the size of the recId
	indexes[pos].maxrecs=floor(1022/(float)(indexes[pos].recsize+(float)(1/8)));//1024-local depth(2 bytes) / record size + 1 bit for bitmap - floor integer part
	
	sprintf(name,"%s.%d",filename,indexNo);//construct the composite name

	if((AM_errno=BF_CreateFile(name))<0)//create the index with the composite name
		return AM_errno;
		
	if((AM_errno=BF_OpenFile(name))<0)
		return AM_errno;
	else{
		fd=AM_errno;
		indexes[pos].fileDesc=fd;
		if((AM_errno=BF_AllocBlock(fd,&blockNum,&blockBuf))<0)//allocate the block that will be used for the AM header
			return AM_errno;
	}	

	//set the total depth, i.e. the first int of the AM header, to 0 and the first SHORT INT to -1
	WriteHeader(-1,0,&blockBuf);
	WriteHeader(0,-1,&blockBuf);
	
	if((AM_errno=BF_UnpinBlock(indexes[pos].fileDesc,blockNum,1))<0)
			return AM_errno;
	
	if((AM_errno=BF_CloseFile(indexes[pos].fileDesc))<0)
		return AM_errno;
	
	AM_errno=AME_OK;
	return AME_OK;
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int AM_DestroyIndex(char *filename,int indexNo){
	
	char name[NAMELENGTH];

	sprintf(name,"%s.%d",filename,indexNo);//construct the composite name

	if((AM_errno=BF_DestroyFile(name))<0)
		return AM_errno;

	AM_errno=AME_OK;
	return AM_errno;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int AM_OpenIndex(char *filename,int indexNo){

	int fd;
	char name[NAMELENGTH];

	sprintf(name,"%s.%d",filename,indexNo);//construct the composite name

	if((AM_errno=BF_OpenFile(name))<0)
		return AM_errno;
	else{
		fd=AM_errno;
		return fd;
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int AM_CloseIndex(int fileDesc){

	if((AM_errno=BF_CloseFile(fileDesc))<0)
		return AM_errno;
	
	AM_errno=AME_OK;
	return AME_OK;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int AM_InsertEntry(int fileDesc,char attrType,int attrLength,char *value,int recId){

	
	unsigned int hv;//hash value
	int i,j,r,distance,temp,header_pos=0,total_depth,found=0,pos=-1,rec_no=-1;
	short int local_depth,blockNum,temp_blockNum;
	char *blockBuf,*copyblockBuf,temp_value[attrLength],*finalblockBuf;
	char bitmappart,hashbits;
	int alloc_blockNum,times=0,power,temp_recId,k,l;


	

	if(fileDesc<0 || fileDesc>24){ // invalid file descriptor
                  
        	AM_errno=BFE_FD;
		return BFE_FD;
    	}

	for(i=0;i<MAXINDEXES;i++)
		if(fileDesc==indexes[i].fileDesc){
			pos=i;
			break;
		}
	//get the hash value depending on the attribute type
	if(attrType=='c' || attrType=='i' || attrType=='f')
		hv=JSHash(value,strlen(value));

	else{
		AM_errno=AME_ATTRTYPEFAULT;
		return AM_errno;
	}
		
	memcpy(&hashbits,&hv,sizeof(char));//get the first 8 bits of the integer

	if((AM_errno=BF_GetThisBlock(fileDesc,0,&blockBuf))<0)//get the AM header
		return AM_errno;

	total_depth=ReadHeader(-1,&blockBuf);//read the total depth from the header
	
	distance=total_depth;
	
	for(i=1;i<=total_depth;i++){	//read the total_depth most significant bits of the hash value and calculate the header position
		temp=is_valid(hashbits,i);	//where the desired pointer is located
		header_pos+=temp*pow(2,distance-1);
		distance--;
	}

	blockNum=ReadHeader(header_pos,&blockBuf);//the content of the header at position header_pos

	if((AM_errno=BF_UnpinBlock(fileDesc,0,1))<0)
			return AM_errno;

	if(blockNum==-1){//initially when no bucket has been created and we only have the header
		if((AM_errno=BF_AllocBlock(fileDesc,&alloc_blockNum,&blockBuf))<0)//allocate the first block-bucket
				return AM_errno;
		
		copyblockBuf=blockBuf;
		//now we insert the record
		WriteHeader(-2,0,&copyblockBuf);//set the local depth in the new block
		copyblockBuf+=sizeof(short int);//skip past the local depth
		memcpy(copyblockBuf,value,attrLength);//write the first field (value)
		copyblockBuf+=attrLength;
		memcpy(copyblockBuf,&recId,sizeof(int));//write the second field (recId)
		//set the first bit of the bitmap to 1
		blockBuf+=BF_BLOCK_SIZE*sizeof(char);//advance blockBuf to the end of the block to read the bitmap (right to left)
		memcpy(&bitmappart,(blockBuf-sizeof(char)),sizeof(char));//get the first character from the end
		set_bit(&bitmappart,8,1);//set the bit to 1
		memcpy((blockBuf-sizeof(char)),&bitmappart,sizeof(char));

		if((AM_errno=BF_UnpinBlock(fileDesc,alloc_blockNum,1))<0)
			return AM_errno;
		
		if((AM_errno=BF_GetThisBlock(fileDesc,0,&blockBuf))<0)//get the AM header
			return AM_errno;
		WriteHeader(0,1,&blockBuf);//put the blockNum pointer in the first position of the header (after the total depth)
		
		if((AM_errno=BF_UnpinBlock(fileDesc,0,1))<0)
			return AM_errno;

		AM_errno=AME_OK;
		return AME_OK;
		
	}

	if((AM_errno=BF_GetThisBlock(fileDesc,blockNum,&blockBuf))<0)//get the desired block/bucket
		return AM_errno;

	local_depth=ReadHeader(-2,&blockBuf);//read the local depth from the header
	
	copyblockBuf=blockBuf;//copy of blockBuf that we will move around

	copyblockBuf+=BF_BLOCK_SIZE*sizeof(char);//advance blockBuf to the end of the block to read the bitmap (right to left)
	
	for(i=0;i<=(indexes[pos].maxrecs/8);i++){//the bitmap has a size of maxrecs bits
		if(i!=(indexes[pos].maxrecs/8)){
			for(j=7;j>=0;j--){//read the bitmap character by character				
				memcpy(&bitmappart,(copyblockBuf-(i+1)*sizeof(char)),sizeof(char));
				if(is_valid(bitmappart,j+1)==0){   //since the offset must be from 1 to 8
					set_bit(&bitmappart,j+1,1);//set the bit to 1 since the record will be placed here
					memcpy((copyblockBuf-(i+1)*sizeof(char)),&bitmappart,sizeof(char));
					rec_no=i*8+(7-j);
					found=1;
					break;
				}
			}
			if(found==1)
				break;
		}
		else{
			for(j=0;j<=(indexes[pos].maxrecs%8);j++){//read the bitmap character by character				
				memcpy(&bitmappart,(copyblockBuf-(i+1)*sizeof(char)),sizeof(char));
				if(is_valid(bitmappart,8-j)==0){   //since the offset must be from 1 to 8
					set_bit(&bitmappart,8-j,1);//set the bit to 1 since the record will be placed here
					memcpy((copyblockBuf-(i+1)*sizeof(char)),&bitmappart,sizeof(char));					
					rec_no=i*8+j;
					found=1;
					break;
				}
			}
			if(found==1)
				break;
		}
	}
	
	
	/*if(rec_no==-1){//if no free position is found for the record
		AM_errno=AME_FULLBLOCK;
		return AME_FULLBLOCK;
	}*/	

	if(found==1){//if the block has space
		copyblockBuf-=BF_BLOCK_SIZE*sizeof(char);//move copyblockBuf back to the beginning of the block
		copyblockBuf+=sizeof(short int);//skip past the local depth
		copyblockBuf+=rec_no*indexes[pos].recsize;//advance past rec_no records
		memcpy(copyblockBuf,value,attrLength);//write the first field (value)
		copyblockBuf+=attrLength;
		memcpy(copyblockBuf,&recId,sizeof(int));//write the second field (recId)
		if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
			return AM_errno;
	}
	else{//if there is no space...
		if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
					return AM_errno;
		if(local_depth<total_depth){
			if((AM_errno=BF_AllocBlock(fileDesc,&alloc_blockNum,&blockBuf))<0)//allocate the new block-bucket
				return AM_errno;

			WriteHeader(-2,local_depth+1,&blockBuf);//set the local depth in the new block

			if((AM_errno=BF_GetThisBlock(fileDesc,blockNum,&blockBuf))<0)//get the block/bucket that the hash function directed us to
				return AM_errno;
		
			WriteHeader(-2,local_depth+1,&blockBuf);//change the local depth of this block as well

			if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
				return AM_errno;

			if((AM_errno=BF_GetThisBlock(fileDesc,0,&blockBuf))<0)//get the AM header
				return AM_errno;

			power=total_depth-local_depth;	//2^power tells us how many pointers point to the block, so we know how many to split
						//to point to the new block

			for(i=0;i<pow(2,total_depth);i++)//search the header
				if(ReadHeader(i,&blockBuf)==blockNum){//when we find the first position with pointer blockNum
					WriteHeader(i,alloc_blockNum,&blockBuf);//make the pointer point to the new block (i.e., split one pointer)
					times++;
					if(times==(pow(2,power)/2)){//we want to change 2^power/2 pointers so they now point to the new block
						times=0;
						break;
					}
				}

			if((AM_errno=BF_UnpinBlock(fileDesc,0,1))<0)
				return AM_errno;			
			
			//now we need to rehash all records of the old block + the current record
			if((AM_errno=BF_GetThisBlock(fileDesc,blockNum,&blockBuf))<0)//get the first of these two blocks
				return AM_errno;	
	
			copyblockBuf=blockBuf;//copy of blockBuf that we will move around
		

			copyblockBuf+=BF_BLOCK_SIZE*sizeof(char);//advance blockBuf to the end of the block to read the bitmap (right to left)
			finalblockBuf=copyblockBuf;

			for(i=0;i<=(indexes[pos].maxrecs/8);i++){//the bitmap has a size of maxrecs bits
				if(i!=(indexes[pos].maxrecs/8)){
					for(j=7;j>=0;j--){//read the bitmap character by character				
						memcpy(&bitmappart,(copyblockBuf-(i+1)*sizeof(char)),sizeof(char));
						if(is_valid(bitmappart,j+1)==1){   //since the offset must be from 1 to 8
							rec_no=i*8+(7-j);
							copyblockBuf-=BF_BLOCK_SIZE*sizeof(char);//move copyblockBuf back to the beginning of the block
							copyblockBuf+=sizeof(short int);//skip past the local depth
							copyblockBuf+=rec_no*indexes[pos].recsize;//advance past rec_no records
							memcpy(temp_value,copyblockBuf,attrLength);//read the first field (value)
							copyblockBuf+=attrLength;//advance to get the second field (recId)
							memcpy(&temp_recId,copyblockBuf,sizeof(int));//read the second field (recId)
							hv=JSHash(temp_value,attrLength);
							memcpy(&hashbits,&hv,sizeof(char));//get the first 8 bits of the integer
							distance=total_depth;
	
							header_pos=0;
							for(r=1;r<=total_depth;r++){	//read the total_depth most significant bits of the hash value and calculate the header position
								temp=is_valid(hashbits,r);	//where the desired pointer is located
								header_pos+=temp*pow(2,distance-1);
								distance--;
							}

							//if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
							//	return AM_errno;

							if((AM_errno=BF_GetThisBlock(fileDesc,0,&blockBuf))<0)//get the AM header
								return AM_errno;						
		
							temp_blockNum=ReadHeader(header_pos,&blockBuf);//see where the record should go
									
							if((AM_errno=BF_UnpinBlock(fileDesc,0,1))<0)
								return AM_errno;
							
							if(temp_blockNum!=blockNum){//if the record needs to move to a different block
								set_bit(&bitmappart,j+1,0);//set the bit to 0 for this record
								memcpy((finalblockBuf-(i+1)*sizeof(char)),&bitmappart,sizeof(char));
								if((AM_errno=BF_GetThisBlock(fileDesc,temp_blockNum,&blockBuf))<0)//get the other (new) block
									return AM_errno;
								copyblockBuf=blockBuf;
								copyblockBuf+=BF_BLOCK_SIZE*sizeof(char);//advance blockBuf to the end of the block to read the bitmap (right to left)

								for(k=0;k<=(indexes[pos].maxrecs/8);k++){//the bitmap has a size of maxrecs bits
									if(k!=(indexes[pos].maxrecs/8)){
										for(l=7;l>=0;l--){//read the bitmap character by character				
											memcpy(&bitmappart,(copyblockBuf-(k+1)*sizeof(char)),sizeof(char));
											if(is_valid(bitmappart,l+1)==0){   //since the offset must be from 1 to 8
												set_bit(&bitmappart,l+1,1);//set the bit to 1 since the record will be placed here
												memcpy((copyblockBuf-(k+1)*sizeof(char)),&bitmappart,sizeof(char));
												rec_no=k*8+(7-l);
												found=1;
												break;
											}
										}
										if(found==1)
											break;
									}
									else{
										for(l=0;l<=(indexes[pos].maxrecs%8);l++){//read the bitmap character by character				
											memcpy(&bitmappart,(copyblockBuf-(k+1)*sizeof(char)),sizeof(char));
											if(is_valid(bitmappart,8-l)==0){   //since the offset must be from 1 to 8
												set_bit(&bitmappart,8-l,1);//set the bit to 1 since the record will be placed here
												memcpy((copyblockBuf-(k+1)*sizeof(char)),&bitmappart,sizeof(char));					
												rec_no=k*8+l;
												found=1;
												break;
											}
										}
										if(found==1)
											break;
									}
								}//end of the for loop with k,l
								//write the record to the new block
								copyblockBuf-=BF_BLOCK_SIZE*sizeof(char);//move copyblockBuf back to the beginning of the block
								copyblockBuf+=sizeof(short int);//skip past the local depth
								copyblockBuf+=rec_no*indexes[pos].recsize;//advance past rec_no records
								memcpy(copyblockBuf,temp_value,attrLength);//write the first field (value)
								copyblockBuf+=attrLength;
								memcpy(copyblockBuf,&temp_recId,sizeof(int));//write the second field (recId)		
								if((AM_errno=BF_UnpinBlock(fileDesc,temp_blockNum,1))<0)
									return AM_errno;				
						
							}//endif
						}// if is_valid
					header_pos=0;
					}//for j
				}				
				else{//for the last value of i
					for(j=0;j<=(indexes[pos].maxrecs%8);j++){//read the bitmap character by character
						memcpy(&bitmappart,(copyblockBuf-(i+1)*sizeof(char)),sizeof(char));
						if(is_valid(bitmappart,8-j)==1){   //since the offset must be from 1 to 8
							rec_no=i*8+j;
							copyblockBuf-=BF_BLOCK_SIZE*sizeof(char);//move copyblockBuf back to the beginning of the block
							copyblockBuf+=sizeof(short int);//skip past the local depth
							copyblockBuf+=rec_no*indexes[pos].recsize;//advance past rec_no records
							memcpy(temp_value,copyblockBuf,attrLength);//read the first field (value)
							copyblockBuf+=attrLength;//advance to get the second field (recId)
							memcpy(&temp_recId,copyblockBuf,sizeof(int));//read the second field (recId)
							hv=JSHash(temp_value,attrLength);
							memcpy(&hashbits,&hv,sizeof(char));//get the first 8 bits of the integer
							distance=total_depth;
	
							for(r=1;r<=total_depth;r++){	//read the total_depth most significant bits of the hash value and calculate the header position
								temp=is_valid(hashbits,r);	//where the desired pointer is located
								header_pos+=temp*pow(2,distance-1);
								distance--;
							}

							if((AM_errno=BF_GetThisBlock(fileDesc,0,&blockBuf))<0)//get the AM header
								return AM_errno;						
		
							temp_blockNum=ReadHeader(header_pos,&blockBuf);//see where the record should go
	
							if((AM_errno=BF_UnpinBlock(fileDesc,0,1))<0)
								return AM_errno;
							
							if(temp_blockNum!=blockNum){//if the record needs to move to a different block
								set_bit(&bitmappart,8-j,0);//set the bit to 0 for this record
								memcpy((finalblockBuf-(i+1)*sizeof(char)),&bitmappart,sizeof(char));
								if((AM_errno=BF_GetThisBlock(fileDesc,temp_blockNum,&blockBuf))<0)//get the other (new) block
									return AM_errno;
								copyblockBuf=blockBuf;
								copyblockBuf+=BF_BLOCK_SIZE*sizeof(char);//advance blockBuf to the end of the block to read the bitmap (right to left)

								for(k=0;k<=(indexes[pos].maxrecs/8);k++){//the bitmap has a size of maxrecs bits
									if(k!=(indexes[pos].maxrecs/8)){
										for(l=7;l>=0;l--){//read the bitmap character by character				
											memcpy(&bitmappart,(copyblockBuf-(k+1)*sizeof(char)),sizeof(char));
											if(is_valid(bitmappart,l+1)==0){   //since the offset must be from 1 to 8
												set_bit(&bitmappart,l+1,1);//set the bit to 1 since the record will be placed here
												memcpy((copyblockBuf-(k+1)*sizeof(char)),&bitmappart,sizeof(char));
												rec_no=k*8+(7-l);
												found=1;
												break;
											}
										}
										if(found==1)
											break;
									}
									else{
										for(l=0;l<=(indexes[pos].maxrecs%8);l++){//read the bitmap character by character				
											memcpy(&bitmappart,(copyblockBuf-(k+1)*sizeof(char)),sizeof(char));
											if(is_valid(bitmappart,8-l)==0){   //since the offset must be from 1 to 8
												set_bit(&bitmappart,8-l,1);//set the bit to 1 since the record will be placed here	
												memcpy((copyblockBuf-(k+1)*sizeof(char)),&bitmappart,sizeof(char));				
												rec_no=k*8+l;
												found=1;
												break;
											}
										}
										if(found==1)
											break;
									}
								}//end of the for loop with k,l
								//write the record to the new block
								copyblockBuf-=BF_BLOCK_SIZE*sizeof(char);//move copyblockBuf back to the beginning of the block
								copyblockBuf+=sizeof(short int);//skip past the local depth
								copyblockBuf+=rec_no*indexes[pos].recsize;//advance past rec_no records
								memcpy(copyblockBuf,temp_value,attrLength);//write the first field (value)
								copyblockBuf+=attrLength;
								memcpy(copyblockBuf,&temp_recId,sizeof(int));//write the second field (recId)	
								if((AM_errno=BF_UnpinBlock(fileDesc,temp_blockNum,1))<0)
									return AM_errno;					
						
							}//endif
						}// if is_valid
					header_pos=0;
					}//for j of else

				}//else for the last i
			}//the main for loop
			if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
				return AM_errno;
			//now only the current record remains to see where it will go
			hv=JSHash(value,strlen(value));
			memcpy(&hashbits,&hv,sizeof(char));//get the first 8 bits of the integer
			distance=total_depth;
	
			for(i=1;i<=total_depth;i++){	//read the total_depth most significant bits of the hash value and calculate the header position
				temp=is_valid(hashbits,i);	//where the desired pointer is located
				header_pos+=temp*pow(2,distance-1);
				distance--;
			}
			blockNum=ReadHeader(header_pos,&blockBuf);//the content of the header at position header_pos
			if((AM_errno=BF_GetThisBlock(fileDesc,blockNum,&blockBuf))<0)//get the desired block/bucket
				return AM_errno;
			copyblockBuf=blockBuf;
			copyblockBuf+=BF_BLOCK_SIZE*sizeof(char);//advance blockBuf to the end of the block to read the bitmap (right to left)

			for(i=0;i<=(indexes[pos].maxrecs/8);i++){//the bitmap has a size of maxrecs bits
				if(i!=(indexes[pos].maxrecs/8)){
					for(j=7;j>=0;j--){//read the bitmap character by character				
						memcpy(&bitmappart,(copyblockBuf-(i+1)*sizeof(char)),sizeof(char));
						if(is_valid(bitmappart,j+1)==0){   //since the offset must be from 1 to 8
							set_bit(&bitmappart,j+1,1);//set the bit to 1 since the record will be placed here
							memcpy((copyblockBuf-(i+1)*sizeof(char)),&bitmappart,sizeof(char));
							rec_no=i*8+(7-j);
							found=1;
							break;
						}
					}
					if(found==1)
						break;
				}
				else{
					for(j=0;j<=(indexes[pos].maxrecs%8);j++){//read the bitmap character by character				
						memcpy(&bitmappart,(copyblockBuf-(i+1)*sizeof(char)),sizeof(char));
						if(is_valid(bitmappart,8-j)==0){   //since the offset must be from 1 to 8
							set_bit(&bitmappart,8-j,1);//set the bit to 1 since the record will be placed here					
							memcpy((copyblockBuf-(i+1)*sizeof(char)),&bitmappart,sizeof(char));
							rec_no=i*8+j;
							found=1;
							break;
						}
					}
					if(found==1)
						break;
				}
			}
			if(found==1){//if the block has space
				copyblockBuf-=BF_BLOCK_SIZE*sizeof(char);//move copyblockBuf back to the beginning of the block
				copyblockBuf+=sizeof(short int);//skip past the local depth
				copyblockBuf+=rec_no*indexes[pos].recsize;//advance past rec_no records
				memcpy(copyblockBuf,value,attrLength);//write the first field (value)
				copyblockBuf+=attrLength;
				memcpy(copyblockBuf,&recId,sizeof(int));//write the second field (recId)
				//if((AM_errno=BF_UnpinBlock(fileDesc,alloc_blockNum,1))<0)
				//	return AM_errno;
				if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
					return AM_errno;
			}
			else{//if the block still has no space
				//if((AM_errno=BF_UnpinBlock(fileDesc,alloc_blockNum,1))<0)
				//	return AM_errno;
				if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
					return AM_errno;
				if((AM_errno=AM_InsertEntry(fileDesc,attrType,attrLength,value,recId))<0)//recursion
					return AM_errno;
			}
		}//end if local<total
		else if(local_depth==total_depth && total_depth<MAX_DEPTH){
			if((AM_errno=BF_GetThisBlock(fileDesc,0,&blockBuf))<0)//get the AM header
				return AM_errno;
				
			WriteHeader(-1,total_depth+1,&blockBuf);//increase the total depth by one in the header
	
			for(i=pow(2,total_depth)-1;i>=0;i--){//doubling the pointer array in the header
				blockNum=ReadHeader(i,&blockBuf);
				WriteHeader(2*i+1,blockNum,&blockBuf);
				WriteHeader(2*i,blockNum,&blockBuf);
			}

			if((AM_errno=BF_UnpinBlock(fileDesc,0,1))<0)
				return AM_errno;
			//now we are in the case where local<total depth
			//blockNum=ReadHeader(-1,&blockBuf);//test!!!!!!!!!!!!!!!!!!!!!
			if((AM_errno=AM_InsertEntry(fileDesc,attrType,attrLength,value,recId))<0)//recursion
				return AM_errno;
		}//if local==total
		else{//the array cannot be doubled any further, so we stop
			AM_errno=AME_ENDOFINDEX;
			return AM_errno;
		}
	}
	
	AM_errno=AME_OK;
	return AME_OK;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int AM_DeleteEntry(int fileDesc,char attrType,int attrLength,char *value,int recId){

	unsigned int hv;//hash value
	int i,j,distance,temp,header_pos=0,total_depth,pos=-1,recNo=-1,thisrecId,mpike=0;
	short int blockNum;
	char *blockBuf,*copyblockBuf,thisvalue[attrLength];
	char bitmappart,hashbits;
	
	if(fileDesc<0 || fileDesc>24){ // invalid file descriptor
                  
        	AM_errno=BFE_FD;
		return BFE_FD;
    	}

	for(i=0;i<MAXINDEXES;i++)
		if(fileDesc==indexes[i].fileDesc){
			pos=i;
			break;
		}
	//get the hash value depending on the attribute type
	if(attrType=='c' || attrType=='i' || attrType=='f')
		hv=JSHash(value,strlen(value));

	else{
		AM_errno=AME_ATTRTYPEFAULT;
		return AM_errno;
	}
		
	memcpy(&hashbits,&hv,sizeof(char));//get the first 8 bits of the integer

	if((AM_errno=BF_GetThisBlock(fileDesc,0,&blockBuf))<0)//get the AM header
		return AM_errno;

	total_depth=ReadHeader(-1,&blockBuf);//read the total depth from the header
	
	distance=total_depth;
	
	for(i=1;i<=total_depth;i++){	//read the total_depth most significant bits of the hash value and calculate the header position
		temp=is_valid(hashbits,i);	//where the desired pointer is located
		header_pos+=temp*pow(2,distance-1);
		distance--;
	}

	blockNum=ReadHeader(header_pos,&blockBuf);//the content of the header at position header_pos

	if((AM_errno=BF_UnpinBlock(fileDesc,0,1))<0)
		return AM_errno;

	if((AM_errno=BF_GetThisBlock(fileDesc,blockNum,&blockBuf))<0)//get the desired block/bucket
		return AM_errno;

	copyblockBuf=blockBuf;

	copyblockBuf+=sizeof(short int);//skip past the local depth

	for(i=0;i<indexes[pos].maxrecs;i++){
		memcpy(thisvalue,copyblockBuf,attrLength);//get the first field
		if(strcmp(thisvalue,value)==0){//if the current value matches the one we are looking for
			mpike=1;
			copyblockBuf+=attrLength;
			memcpy(&thisrecId,copyblockBuf,sizeof(int));//get the second field (recId)
			copyblockBuf+=sizeof(int);
			if(thisrecId==recId){//if the second field also matches the one we are looking for
				recNo=i;
				break;
			}
		}
		if(mpike==0)
			copyblockBuf+=indexes[pos].recsize;//advance one record to get the next one
	}
	
	if(recNo!=-1){//if the record was indeed found
		blockBuf+=BF_BLOCK_SIZE*sizeof(char);//advance blockBuf (copyblockBuf has been moved enough) to the end of the block to read the bitmap (right to left)
		memcpy(&bitmappart,(blockBuf-((recNo/BITSOFCHAR)+1)*sizeof(char)),sizeof(char));//get the recNo/8 + 1 character from the end of the block
		set_bit(&bitmappart,BITSOFCHAR-(recNo%BITSOFCHAR),1);//set the bit to 0 at the appropriate position
		memcpy((blockBuf-((recNo/BITSOFCHAR)+1)*sizeof(char)),&bitmappart,sizeof(char));
		if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
			return AM_errno;		
		AM_errno=AME_OK;
		return AME_OK;	
	}
	else{//the record was not found
		AM_errno=AME_RECNOTFOUND;
		return AME_RECNOTFOUND;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int AM_OpenIndexScan(int fileDesc,char attrType,int attrLength,int op,char *value){

	int i,j,pos=-1,thesi,distance,temp,header_pos=0,total_depth,blockNum,thisrecId,maxblockNum;
	unsigned int hv;
	char *blockBuf,*copyblockBuf,hashbits,*thisvalue;
	
	for(i=0;i<MAXSCANS;i++)//search for a free position in the scans array
		if(scans[i].isfree==0){
			pos=i;
			scans[pos].isfree=1;
			break;
		}
	
	if(pos==-1){//if not found, return error
		AM_errno=AME_FULLSCANARRAY;
		return AME_FULLSCANARRAY;
	}
	
	for(i=0;i<MAXINDEXES;i++)
		if(fileDesc==indexes[i].fileDesc){
			thesi=i;
			break;
		}

	//update the struct members
	scans[pos].fileDesc=fileDesc;
	scans[pos].attrType=attrType;
	scans[pos].attrLength=attrLength;
	scans[pos].op=op;
	for(i=0;i<attrLength;i++)
		scans[pos].value='\0';
	strcpy(scans[pos].value,value);
	
	if((AM_errno=BF_GetThisBlock(fileDesc,0,&blockBuf))<0)//get the AM header
			return AM_errno;

	total_depth=ReadHeader(-1,&blockBuf);//read the total depth from the header
	
	/*temparray=(int*)calloc(pow(2,total_depth),sizeof(int));	//an array with 2^total_depth positions where we store
															//the distinct blockNums that exist in the header so we know how many
															//and which blocks we will scan

	for(i=0;i<pow(2,total_depth);i++)//initialize to -1 so we know which positions will not be filled at the end
		temparray[i]=-1;

	for(i=0;i<pow(2,total_depth);i++){//build the temparray as described above
		blockNum=ReadHeader(i,blockBuf);
		for(j=0;j<pow(2,total_depth);j++)
			if(blockNum==temparray[j]){
				exists=1;
				break;
			}
		if(exists==0){
			temparray[tempthesi]=blockNum;
			tempthesi++;
		}
		exists=0;
	}	*/
	
	//we want to find the largest blockNum in the header to scan up to that point
	maxblockNum=ReadHeader(0,&blockBuf);//initial value
	
	for(i=0;i<pow(2,total_depth);i++){
		blockNum=ReadHeader(i,&blockBuf);
		if(blockNum>maxblockNum)
			maxblockNum=blockNum;
	}
	
	if(op==EQUAL){
		//get the hash value depending on the attribute type
		if(attrType=='c' || attrType=='i' || attrType=='f')
			hv=JSHash(value,strlen(value));
		else{
			AM_errno=AME_ATTRTYPEFAULT;
			return AM_errno;
		}
		
		memcpy(&hashbits,&hv,sizeof(char));//get the first 8 bits of the integer
		
		distance=total_depth;
	
		for(i=1;i<=total_depth;i++){	//read the total_depth most significant bits of the hash value and calculate the header position
			temp=is_valid(hashbits,i);	//where the desired pointer is located
			header_pos+=temp*pow(2,distance-1);
			distance--;
		}

		blockNum=ReadHeader(header_pos,&blockBuf);//to see which block to search in

		if((AM_errno=BF_UnpinBlock(fileDesc,0,1))<0)
			return AM_errno;
	
		if((AM_errno=BF_GetThisBlock(fileDesc,blockNum,&blockBuf))<0)//get block blockNum where the records with this value are located
			return AM_errno;
		
		copyblockBuf=blockBuf;

		copyblockBuf+=sizeof(short int);//skip past the local depth
	
		for(i=0;i<indexes[thesi].maxrecs;i++){
			memcpy(thisvalue,copyblockBuf,attrLength);//get the first field
			if(strcmp(thisvalue,value)==0){//if the current value matches the one we are looking for
				copyblockBuf+=attrLength;
				memcpy(&thisrecId,copyblockBuf,sizeof(int));//get the second field (recId)
				//save the bucket and record where the scan will stop before the function returns
				scans[pos].bucket=blockNum;
				scans[pos].recNo=i;
				return thisrecId;
			}
			copyblockBuf+=indexes[thesi].recsize;//if the value does not match, move to the next record
		}
		if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
			return AM_errno;
	}//end if op=EQUAL
	
	else if(op==NOT_EQUAL){
		//get the hash value depending on the attribute type
		if(attrType=='c' || attrType=='i' || attrType=='f')
			hv=JSHash(value,strlen(value));
		else{
			AM_errno=AME_ATTRTYPEFAULT;
			return AM_errno;
		}
		
		memcpy(&hashbits,&hv,sizeof(char));//get the first 8 bits of the integer
		
		distance=total_depth;
	
		for(i=1;i<=total_depth;i++){	//read the total_depth most significant bits of the hash value and calculate the header position
			temp=is_valid(hashbits,i);	//where the desired pointer is located
			header_pos+=temp*pow(2,distance-1);
			distance--;
		}

		blockNum=ReadHeader(header_pos,&blockBuf);//to see which is the "dangerous" block
		
		//now we start scanning all block-buckets. If we are in the dangerous block (blockNum), we check that the value is not the same
		//as the argument's value, otherwise for any other block we return the first record we encounter
		for(i=0;i<=maxblockNum;i++){//for all existing buckets
			if((AM_errno=BF_GetThisBlock(fileDesc,i,&blockBuf))<0)
				return AM_errno;
		
			copyblockBuf=blockBuf;

			copyblockBuf+=sizeof(short int);//skip past the local depth
				
			if(i==blockNum){//if we are in the "dangerous" block
				for(j=0;j<indexes[thesi].maxrecs;j++){
					memcpy(thisvalue,copyblockBuf,attrLength);//get the first field
					if(strcmp(thisvalue,value)!=0){//if the current value is not the same as the one we are looking for
						copyblockBuf+=attrLength;
						memcpy(&thisrecId,copyblockBuf,sizeof(int));//get the second field (recId)
						//save the bucket and record where the scan will stop before the function returns
						scans[pos].bucket=blockNum;
						scans[pos].recNo=j;
						return thisrecId;
					}
					copyblockBuf+=indexes[thesi].recsize;//if the value does not match, move to the next record
				}
				if((AM_errno=BF_UnpinBlock(fileDesc,i,1))<0)
					return AM_errno;
			}
			else{//in any other block
				for(j=0;j<indexes[thesi].maxrecs;j++){
					copyblockBuf+=attrLength;
					memcpy(&thisrecId,copyblockBuf,sizeof(int));//get the second field (recId)
					//save the bucket and record where the scan will stop before the function returns
					scans[pos].bucket=i;
					scans[pos].recNo=j;
					if((AM_errno=BF_UnpinBlock(fileDesc,i,1))<0)
						return AM_errno;
					return thisrecId;						
				}
			}
		}
	
	}//end if op=NOT_EQUAL
		
	else{
		AM_errno=AME_WRONG_OP;
		return AME_WRONG_OP;
	}
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int AM_FindNextEntry(int scanDesc){

	int i,j,maxblockNum,blockNum,total_depth,thisrecId,distance,temp,header_pos=0,thesi;
	unsigned int hv;
	char *blockBuf,*copyblockBuf,*thisvalue,hashbits;
	
	if(scanDesc<0 || scanDesc>19){
		AM_errno=AME_WRONGSCANDESC;
        return AME_WRONGSCANDESC;
	}
	
	if(scans[scanDesc].isfree==0){//if the scan is not open for this scanDesc
		AM_errno=AME_INDEXSCANALREADYCLOSED;
		return AME_INDEXSCANALREADYCLOSED;
	}
	
	for(i=0;i<MAXINDEXES;i++)
		if(scans[scanDesc].fileDesc==indexes[i].fileDesc){
			thesi=i;
			break;
		}
	
	if((AM_errno=BF_GetThisBlock(scans[scanDesc].fileDesc,0,&blockBuf))<0)//get the AM header
			return AM_errno;
	
	total_depth=ReadHeader(-1,&blockBuf);//read the total depth from the header
	
	//we want to find the largest blockNum in the header to scan up to that point (we do not keep the old maxblockNum because it may
	//have changed since the previous call)
	maxblockNum=ReadHeader(0,&blockBuf);//initial value
	
	for(i=0;i<pow(2,total_depth);i++){
		blockNum=ReadHeader(i,&blockBuf);
		if(blockNum>maxblockNum)
			maxblockNum=blockNum;
	}
	
	if((AM_errno=BF_UnpinBlock(scans[scanDesc].fileDesc,0,1))<0)
		return AM_errno;
	
	if(scans[scanDesc].op==EQUAL){
			
		if((AM_errno=BF_GetThisBlock(scans[scanDesc].fileDesc,scans[scanDesc].bucket,&blockBuf))<0)//go to the bucket we have saved
			return AM_errno;
		
		copyblockBuf=blockBuf;

		copyblockBuf+=sizeof(short int);//skip past the local depth
	
		for(i=(scans[scanDesc].recNo)+1;i<indexes[thesi].maxrecs;i++){//continue from the next record
			memcpy(thisvalue,copyblockBuf,scans[scanDesc].attrLength);//get the first field
			if(strcmp(thisvalue,scans[scanDesc].value)==0){//if the current value matches the one we are looking for
				copyblockBuf+=scans[scanDesc].attrLength;
				memcpy(&thisrecId,copyblockBuf,sizeof(int));//get the second field (recId)
				//save the record where the scan will stop before the function returns
				scans[scanDesc].recNo=i;
				return thisrecId;
			}
			copyblockBuf+=indexes[thesi].recsize;//if the value does not match, move to the next record
		}
		if((AM_errno=BF_UnpinBlock(scans[scanDesc].fileDesc,scans[scanDesc].bucket,1))<0)
			return AM_errno;
	}//end if op=EQUAL

	if(scans[scanDesc].op=NOT_EQUAL){
		//get the hash value depending on the attribute type
		if(scans[scanDesc].attrType=='c' || scans[scanDesc].attrType=='i' || scans[scanDesc].attrType=='f')
			hv=JSHash(scans[scanDesc].value,strlen(scans[scanDesc].value));
		else{
			AM_errno=AME_ATTRTYPEFAULT;
			return AM_errno;
		}
		
		memcpy(&hashbits,&hv,sizeof(char));//get the first 8 bits of the integer
		
		distance=total_depth;
	
		for(i=1;i<=total_depth;i++){	//read the total_depth most significant bits of the hash value and calculate the header position
			temp=is_valid(hashbits,i);	//where the desired pointer is located
			header_pos+=temp*pow(2,distance-1);
			distance--;
		}

		blockNum=ReadHeader(header_pos,&blockBuf);//to see which is the "dangerous" block
		
		//now we start scanning all block-buckets. If we are in the dangerous block (blockNum), we check that the value is not the same
		//as the argument's value, otherwise for any other block we return the first record we encounter
		for(i=scans[scanDesc].bucket;i<=maxblockNum;i++){//for all existing buckets
			if((AM_errno=BF_GetThisBlock(scans[scanDesc].fileDesc,i,&blockBuf))<0)
				return AM_errno;
		
			if(i==scans[scanDesc].bucket){
			
				copyblockBuf=blockBuf;

				copyblockBuf+=sizeof(short int);//skip past the local depth
				
				if(i==blockNum){//if we are in the "dangerous" block
					for(j=(scans[scanDesc].recNo)+1;j<indexes[thesi].maxrecs;j++){
						memcpy(thisvalue,copyblockBuf,scans[scanDesc].attrLength);//get the first field
						if(strcmp(thisvalue,scans[scanDesc].value)!=0){//if the current value is not the same as the one we are looking for
							copyblockBuf+=scans[scanDesc].attrLength;
							memcpy(&thisrecId,copyblockBuf,sizeof(int));//get the second field (recId)
							//save the bucket and record where the scan will stop before the function returns
							scans[scanDesc].bucket=blockNum;
							scans[scanDesc].recNo=j;
							return thisrecId;
						}
						copyblockBuf+=indexes[thesi].recsize;//if the value does not match, move to the next record
					}
				}
				else{//in any other block
					for(j=(scans[scanDesc].recNo)+1;j<indexes[thesi].maxrecs;j++){
						copyblockBuf+=scans[scanDesc].attrLength;
						memcpy(&thisrecId,copyblockBuf,sizeof(int));//get the second field (recId)
						//save the bucket and record where the scan will stop before the function returns
						scans[scanDesc].bucket=i;
						scans[scanDesc].recNo=j;
						return thisrecId;						
					}
				}
				if((AM_errno=BF_UnpinBlock(scans[scanDesc].fileDesc,i,1))<0)
					return AM_errno;
			}//end if i=scans[scanDesc].bucket
			else{
				copyblockBuf=blockBuf;

				copyblockBuf+=sizeof(short int);//skip past the local depth
				
				if(i==blockNum){//if we are in the "dangerous" block
					for(j=0;j<indexes[thesi].maxrecs;j++){
						memcpy(thisvalue,copyblockBuf,scans[scanDesc].attrLength);//get the first field
						if(strcmp(thisvalue,scans[scanDesc].value)!=0){//if the current value is not the same as the one we are looking for
							copyblockBuf+=scans[scanDesc].attrLength;
							memcpy(&thisrecId,copyblockBuf,sizeof(int));//get the second field (recId)
							//save the bucket and record where the scan will stop before the function returns
							scans[scanDesc].bucket=blockNum;
							scans[scanDesc].recNo=j;
							return thisrecId;
						}
						copyblockBuf+=indexes[thesi].recsize;//if the value does not match, move to the next record
					}
				}
				else{//in any other block
					for(j=0;j<indexes[thesi].maxrecs;j++){
						copyblockBuf+=scans[scanDesc].attrLength;
						memcpy(&thisrecId,copyblockBuf,sizeof(int));//get the second field (recId)
						//save the bucket and record where the scan will stop before the function returns
						scans[scanDesc].bucket=i;
						scans[scanDesc].recNo=j;
						return thisrecId;						
					}
				}
				if((AM_errno=BF_UnpinBlock(scans[scanDesc].fileDesc,i,1))<0)
					return AM_errno;
			}//else
		}
	
	}//end if op=NOT_EQUAL

	
	
	AM_errno=AME_EOF;//if no record is found and the function does not return earlier
	return AME_EOF;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int AM_CloseIndexScan(int scanDesc){
   
    int i;
   
    if(scanDesc<0 || scanDesc>=MAXSCANS){
        AM_errno=AME_WRONGSCANDESC;
        return AME_WRONGSCANDESC;
    }
   
    for(i=0;i<MAXSCANS;i++)
        if(i==scanDesc){
            if(scans[i].isfree==1){
                scans[i].isfree=0;
                break;
            }
            else{
                AM_errno=AME_INDEXSCANALREADYCLOSED;
                return AME_INDEXSCANALREADYCLOSED;
            }
        }
   
    AM_errno=AME_OK;
    return AME_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AM_PrintError(char* errString){
    
    printf("%s\n",errString);
    
    if((AM_errno < 0) && (AM_errno >= (-31)))
        BF_PrintError(errString);
    else{
        switch(AM_errno){
        case -32 : printf("The indexNo has already been given\n");
              break;
        case -33 : printf("The array of the index is full\n");
              break;
        case -34 : printf("Wrong attrType\n");
              break;
        case -35 : printf("No more depth in the index\n");
              break;
        case -36 : printf("No record found\n");
              break;
        case -37 : printf("The array of the opened scans is full\n");
              break;
        case -38 : printf("Wrong code for op\n");
              break;
        case -39 : printf("Wrong code for the array of the index scans\n");
              break;
	case -40 : printf("The index scan has already terminated\n");
              break;
	case -41 : printf("End of file reached\n");
              break;
        default : printf("OK\n");
        }
    }
}




