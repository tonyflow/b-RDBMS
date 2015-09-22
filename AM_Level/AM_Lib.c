#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "BF_Lib.h"
#include "AM_Lib.h"



int ReadHeader(int header_pos,char **blockBuf){

	int blockno;//o arithmos tou block(deiktis) pou exei i thesi header_pos
	int i;
	char *str;
	short int temp;

	//str=malloc(sizeof(int));

	if(header_pos==-1){//me orisma -1 pairnei to total depth tou header pou einai int
		str=malloc(sizeof(int));
		strncpy(str,*blockBuf,sizeof(int));
		memcpy(&blockno,str,sizeof(int));
		//memcpy(&blockno,*blockBuf,sizeof(int));
		//blockno=*((int*)(*blockBuf));
		//return blockno;
		//blockno=atoi(str);
		return blockno;
	}
	else if(header_pos==-2){//me orisma -2 pairnei to local depth tou kadou pou einai short int
		str=malloc(sizeof(short int));
		strncpy(str,*blockBuf,sizeof(short int));
		memcpy(&temp,str,sizeof(short int));
		//memcpy(&temp,*blockBuf,sizeof(short int));
		//temp=atoi(str);
		return temp;
	}
	else{//allios proxoraei header_pos theseis apo to total depth gia na parei to deikti pou theloume
		str=malloc(sizeof(short int));	
		*blockBuf += sizeof(int);
		for(i=0;i<header_pos;i++)	
			*blockBuf += sizeof(short int);
		strncpy(str,*blockBuf,sizeof(short int));
		memcpy(&temp,str,sizeof(short int));
		//memcpy(&temp,*blockBuf,sizeof(short int));
		//temp=atoi(str);
		//tora epistrefoume ton deikti stin arxi
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
	
	if(header_pos==-1){//me orisma -1 grafei to total depth
		help=blockNum;
		memcpy(*blockBuf,&help,sizeof(int));
	}
	else if(header_pos==-2){//gia to local depth
		temp=(short int)blockNum;
		memcpy(*blockBuf,&temp,sizeof(short int));
	}
	else{//allios proxoraei header_pos theseis apo to total depth gia na grapsei to deikti pou theloume	
		temp=(short int)blockNum;
		*blockBuf += sizeof(int);
		for(i=0;i<header_pos;i++)	
			*blockBuf += sizeof(short int);
		memcpy(*blockBuf,&temp,sizeof(short int));
		//tora epistrefoume ton deikti stin arxi
		for(i=0;i<header_pos;i++)	
			*blockBuf -= sizeof(short int);
		*blockBuf -= sizeof(int);
		
	}


}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int is_valid(char hv,int offset){
	
	
	if(((hv & (1 << (7 - (offset - 1))))==0))
		return 0;	//an i AND praxi bgalei mono midenika,tote afou stin offset thesi eixame 1,sto bitmap tha exoume 0
	else
		return 1;
} 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void set_bit(char* bitmappart,int offset,int what){
	if(what==1)
		 *bitmappart = *bitmappart | (1 << (7 - (offset - 1))); // apo 0 se 1 
	else
		*bitmappart = *bitmappart ^ (1 << (7 - (offset - 1))); 	// apo 1 se 0
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
	
	for(i=0;i<MAXINDEXES;i++)//elegxoume an to indexNo einai monadiko i iparxei idi gia kapoio euretirio
		if(strcmp(indexes[i].filename,filename)==0 && indexes[i].indexNo==indexNo){
			AM_errno=AME_INDEXNO;
			return AM_errno;
		}
	
	for(i=0;i<MAXINDEXES;i++)//psaxnoume eleutheri thesi ston pinaka euretirion
		if(indexes[i].isfree==0){
			pos=i;
			break;
		}
	
	if(pos==-1){//an den iparxei eleutheri thesi
		AM_errno=AME_FULLINDEXARRAY;
		return AM_errno;
	}
	
	//enimeronoume tis plirofories
	strcpy(indexes[pos].filename,filename);
	indexes[pos].indexNo=indexNo;
	indexes[pos].attrType=attrType;
	indexes[pos].attrLength=attrLength;
	indexes[pos].isfree=1;
	indexes[pos].recsize=attrLength+sizeof(int);//mikos pediou+to megethos tou recId
	indexes[pos].maxrecs=floor(1022/(float)(indexes[pos].recsize+(float)(1/8)));//1024-local depth(2 bytes) / megethos eggrafis+1 bit gia bitmap -to kato akeraio meros
	
	sprintf(name,"%s.%d",filename,indexNo);//ftiaxnoume to sintheto onoma

	if((AM_errno=BF_CreateFile(name))<0)//dimiourgoume to euretirio me to sintheto onoma
		return AM_errno;
		
	if((AM_errno=BF_OpenFile(name))<0)
		return AM_errno;
	else{
		fd=AM_errno;
		indexes[pos].fileDesc=fd;
		if((AM_errno=BF_AllocBlock(fd,&blockNum,&blockBuf))<0)//desmeuoume to block pou tha xrisimopoiithei gia to AM header
			return AM_errno;
	}	

	//bazoume sto total depth dld ston proto int tou header tou AM thn timh 0 kai ston proto SHORT INT thn timh -1
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

	sprintf(name,"%s.%d",filename,indexNo);//ftiaxnoume to sintheto onoma

	if((AM_errno=BF_DestroyFile(name))<0)
		return AM_errno;

	AM_errno=AME_OK;
	return AM_errno;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int AM_OpenIndex(char *filename,int indexNo){

	int fd;
	char name[NAMELENGTH];

	sprintf(name,"%s.%d",filename,indexNo);//ftiaxnoume to sintheto onoma

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


	

	if(fileDesc<0 || fileDesc>24){ // mh egkyros anagnoristikos arithmos arxeiou
                  
        	AM_errno=BFE_FD;
		return BFE_FD;
    	}

	for(i=0;i<MAXINDEXES;i++)
		if(fileDesc==indexes[i].fileDesc){
			pos=i;
			break;
		}
	//pairnoume tin hash value analoga me ton tipo tou pediou
	if(attrType=='c' || attrType=='i' || attrType=='f')
		hv=JSHash(value,strlen(value));

	else{
		AM_errno=AME_ATTRTYPEFAULT;
		return AM_errno;
	}
		
	memcpy(&hashbits,&hv,sizeof(char));//pairnoume ta prota 8 bits tou akeraiou

	if((AM_errno=BF_GetThisBlock(fileDesc,0,&blockBuf))<0)//pairnoume to AM header
		return AM_errno;

	total_depth=ReadHeader(-1,&blockBuf);//diabazoume to oliko bathos apo to header
	
	distance=total_depth;
	
	for(i=1;i<=total_depth;i++){	//diabazoume ta total_depth simantikotera psifeia tou hash value kai ipologizoume ti thesi tou header
		temp=is_valid(hashbits,i);	//stin opoia einai o deiktis pou theloume
		header_pos+=temp*pow(2,distance-1);
		distance--;
	}

	blockNum=ReadHeader(header_pos,&blockBuf);//to periexomeno tis header_pos thesis tou header

	if((AM_errno=BF_UnpinBlock(fileDesc,0,1))<0)
			return AM_errno;

	if(blockNum==-1){//stin arxi pou den exei dimiourgithei kanenas kados kai exoume mono to header
		if((AM_errno=BF_AllocBlock(fileDesc,&alloc_blockNum,&blockBuf))<0)//desmeuoume to proto block-kado
				return AM_errno;
		
		copyblockBuf=blockBuf;
		//tora tha eisagoume tin eggrafi
		WriteHeader(-2,0,&copyblockBuf);//bazoume to topiko vathos sto kainourio block
		copyblockBuf+=sizeof(short int);//pername to local depth
		memcpy(copyblockBuf,value,attrLength);//grafoume to proto pedio(value)
		copyblockBuf+=attrLength;
		memcpy(copyblockBuf,&recId,sizeof(int));//grafoume to deutero pedio(recId)
		//pame na kanoume 1 to proto bit tou bitmap
		blockBuf+=BF_BLOCK_SIZE*sizeof(char);//proxorame to blockBuf sto telos tou block gia na diabasoume to bitmap(apo dexia pros ta aristera)
		memcpy(&bitmappart,(blockBuf-sizeof(char)),sizeof(char));//pairnoume ton proto apo to telos akeraio
		set_bit(&bitmappart,8,1);//kanoume 1 to bit
		memcpy((blockBuf-sizeof(char)),&bitmappart,sizeof(char));

		if((AM_errno=BF_UnpinBlock(fileDesc,alloc_blockNum,1))<0)
			return AM_errno;
		
		if((AM_errno=BF_GetThisBlock(fileDesc,0,&blockBuf))<0)//pairnoume to AM header
			return AM_errno;
		WriteHeader(0,1,&blockBuf);//bazoume to blockNum-deikti stin proti thesi tou header(meta to oliko bathos)
		
		if((AM_errno=BF_UnpinBlock(fileDesc,0,1))<0)
			return AM_errno;

		AM_errno=AME_OK;
		return AME_OK;
		
	}

	if((AM_errno=BF_GetThisBlock(fileDesc,blockNum,&blockBuf))<0)//pairnoume to block/kado pou theloume
		return AM_errno;

	local_depth=ReadHeader(-2,&blockBuf);//diabazoume to topiko bathos apo to header
	
	copyblockBuf=blockBuf;//antigrafo tou blockBuf pou tha to metakinoume

	copyblockBuf+=BF_BLOCK_SIZE*sizeof(char);//proxorame to blockBuf sto telos tou block gia na diabasoume to bitmap(apo dexia pros ta aristera)
	
	for(i=0;i<=(indexes[pos].maxrecs/8);i++){//to bitmap exei megethos maxrecs bits
		if(i!=(indexes[pos].maxrecs/8)){
			for(j=7;j>=0;j--){//diabazoume to bitmap ana xaraktira				
				memcpy(&bitmappart,(copyblockBuf-(i+1)*sizeof(char)),sizeof(char));
				if(is_valid(bitmappart,j+1)==0){   //epidi to offset prepei na einai apo 1 mexri 8
					set_bit(&bitmappart,j+1,1);//kanoume to bit 1 afou tha mpei i eggrafi
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
			for(j=0;j<=(indexes[pos].maxrecs%8);j++){//diabazoume to bitmap ana xaraktira				
				memcpy(&bitmappart,(copyblockBuf-(i+1)*sizeof(char)),sizeof(char));
				if(is_valid(bitmappart,8-j)==0){   //epeidi to offset prepei na einai apo 1 mexri 8
					set_bit(&bitmappart,8-j,1);//kanoume to bit 1 afou tha mpei i eggrafi
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
	
	
	/*if(rec_no==-1){//an den brethei eleutheri thesi gia na mpei i eggrafi
		AM_errno=AME_FULLBLOCK;
		return AME_FULLBLOCK;
	}*/	

	if(found==1){//an exei xoro to block
		copyblockBuf-=BF_BLOCK_SIZE*sizeof(char);//proxorame to copyblockBuf kai pali stin arxi tou block
		copyblockBuf+=sizeof(short int);//pername to local depth
		copyblockBuf+=rec_no*indexes[pos].recsize;//proxorame rec_no eggrafes
		memcpy(copyblockBuf,value,attrLength);//grafoume to proto pedio(value)
		copyblockBuf+=attrLength;
		memcpy(copyblockBuf,&recId,sizeof(int));//grafoume to deutero pedio(recId)
		if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
			return AM_errno;
	}
	else{//an den exei xoro...
		if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
					return AM_errno;
		if(local_depth<total_depth){
			if((AM_errno=BF_AllocBlock(fileDesc,&alloc_blockNum,&blockBuf))<0)//desmeuoume to neo block-kado
				return AM_errno;

			WriteHeader(-2,local_depth+1,&blockBuf);//bazoume to topiko vathos sto kainourio block

			if((AM_errno=BF_GetThisBlock(fileDesc,blockNum,&blockBuf))<0)//pairnoume to block/kado pou mas eixe paei i hash function
				return AM_errno;
		
			WriteHeader(-2,local_depth+1,&blockBuf);//allazoume to topiko vathos kai autou tou block

			if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
				return AM_errno;

			if((AM_errno=BF_GetThisBlock(fileDesc,0,&blockBuf))<0)//pairnoume to AM header
				return AM_errno;

			power=total_depth-local_depth;	//to 2^power mas deixnei posoi deiktes deixnoun sto block,oste na kseroume posous na spasoume
						//gia na deixnoun sto neo block		

			for(i=0;i<pow(2,total_depth);i++)//psaxnoume to header
				if(ReadHeader(i,&blockBuf)==blockNum){//otan broume tin proti thesi me deikti blockNum
					WriteHeader(i,alloc_blockNum,&blockBuf);//kanoume to deikti na deixnei sto neo block(spame diladi ton ena deikti)
					times++;
					if(times==(pow(2,power)/2)){//theloume na allaxoume 2^power/2 deiktes,oste autoi na deixnoun tora sto neo block
						times=0;
						break;
					}
				}

			if((AM_errno=BF_UnpinBlock(fileDesc,0,1))<0)
				return AM_errno;			
			
			//tora prepei na kanoume rehash oles tis eggrafes tou paliou block + tin trexousa eggrafi
			if((AM_errno=BF_GetThisBlock(fileDesc,blockNum,&blockBuf))<0)//pairnoume to proto apo ta duo auta block
				return AM_errno;	
	
			copyblockBuf=blockBuf;//antigrafo tou blockBuf pou tha to metakinoume
		

			copyblockBuf+=BF_BLOCK_SIZE*sizeof(char);//proxorame to blockBuf sto telos tou block gia na diabasoume to bitmap(apo dexia pros ta aristera)
			finalblockBuf=copyblockBuf;

			for(i=0;i<=(indexes[pos].maxrecs/8);i++){//to bitmap exei megethos maxrecs bits
				if(i!=(indexes[pos].maxrecs/8)){
					for(j=7;j>=0;j--){//diabazoume to bitmap ana xaraktira				
						memcpy(&bitmappart,(copyblockBuf-(i+1)*sizeof(char)),sizeof(char));
						if(is_valid(bitmappart,j+1)==1){   //epeidi to offset prepei na einai apo 1 mexri 8
							rec_no=i*8+(7-j);
							copyblockBuf-=BF_BLOCK_SIZE*sizeof(char);//proxorame to copyblockBuf kai pali stin arxi tou block
							copyblockBuf+=sizeof(short int);//pername to local depth
							copyblockBuf+=rec_no*indexes[pos].recsize;//proxorame rec_no eggrafes
							memcpy(temp_value,copyblockBuf,attrLength);//diabazoume to proto pedio(value)
							copyblockBuf+=attrLength;//proxorame gia na paroume to deutero pedio(recId)
							memcpy(&temp_recId,copyblockBuf,sizeof(int));//diabazoume to deutero pedio(recId)
							hv=JSHash(temp_value,attrLength);
							memcpy(&hashbits,&hv,sizeof(char));//pairnoume ta prota 8 bits tou akeraiou
							distance=total_depth;
	
							header_pos=0;
							for(r=1;r<=total_depth;r++){	//diabazoume ta total_depth simantikotera psifeia tou hash value kai ipologizoume ti thesi tou header
								temp=is_valid(hashbits,r);	//stin opoia einai o deiktis pou theloume
								header_pos+=temp*pow(2,distance-1);
								distance--;
							}

							//if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
							//	return AM_errno;

							if((AM_errno=BF_GetThisBlock(fileDesc,0,&blockBuf))<0)//pairnoume to AM header
								return AM_errno;						
		
							temp_blockNum=ReadHeader(header_pos,&blockBuf);//blepoume pou prepei na paei i eggrafi
									
							if((AM_errno=BF_UnpinBlock(fileDesc,0,1))<0)
								return AM_errno;
							
							if(temp_blockNum!=blockNum){//an i eggrafi prepei na allaxei block
								set_bit(&bitmappart,j+1,0);//kanoume 0 to bit gia autin tin eggrafi
								memcpy((finalblockBuf-(i+1)*sizeof(char)),&bitmappart,sizeof(char));
								if((AM_errno=BF_GetThisBlock(fileDesc,temp_blockNum,&blockBuf))<0)//pairnoume to allo(kainourio) block
									return AM_errno;
								copyblockBuf=blockBuf;
								copyblockBuf+=BF_BLOCK_SIZE*sizeof(char);//proxorame to blockBuf sto telos tou block gia na diabasoume to bitmap(apo dexia pros ta aristera)

								for(k=0;k<=(indexes[pos].maxrecs/8);k++){//to bitmap exei megethos maxrecs bits
									if(k!=(indexes[pos].maxrecs/8)){
										for(l=7;l>=0;l--){//diabazoume to bitmap ana xaraktira				
											memcpy(&bitmappart,(copyblockBuf-(k+1)*sizeof(char)),sizeof(char));
											if(is_valid(bitmappart,l+1)==0){   //epidi to offset prepei na einai apo 1 mexri 8
												set_bit(&bitmappart,l+1,1);//kanoume to bit 1 afou tha mpei i eggrafi
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
										for(l=0;l<=(indexes[pos].maxrecs%8);l++){//diabazoume to bitmap ana xaraktira				
											memcpy(&bitmappart,(copyblockBuf-(k+1)*sizeof(char)),sizeof(char));
											if(is_valid(bitmappart,8-l)==0){   //epeidi to offset prepei na einai apo 1 mexri 8
												set_bit(&bitmappart,8-l,1);//kanoume to bit 1 afou tha mpei i eggrafi
												memcpy((copyblockBuf-(k+1)*sizeof(char)),&bitmappart,sizeof(char));					
												rec_no=k*8+l;
												found=1;
												break;
											}
										}
										if(found==1)
											break;
									}
								}//kleinei to for me ta k,j
								//grafoume tin eggrafi sto kainourio block
								copyblockBuf-=BF_BLOCK_SIZE*sizeof(char);//proxorame to copyblockBuf kai pali stin arxi tou block
								copyblockBuf+=sizeof(short int);//pername to local depth
								copyblockBuf+=rec_no*indexes[pos].recsize;//proxorame rec_no eggrafes
								memcpy(copyblockBuf,temp_value,attrLength);//grafoume to proto pedio(value)
								copyblockBuf+=attrLength;
								memcpy(copyblockBuf,&temp_recId,sizeof(int));//grafoume to deutero pedio(recId)		
								if((AM_errno=BF_UnpinBlock(fileDesc,temp_blockNum,1))<0)
									return AM_errno;				
						
							}//endif
						}// if is_valid
					header_pos=0;
					}//for j
				}				
				else{//gia tin teleutaia timi tou i
					for(j=0;j<=(indexes[pos].maxrecs%8);j++){//diabazoume to bitmap ana xaraktira
						memcpy(&bitmappart,(copyblockBuf-(i+1)*sizeof(char)),sizeof(char));
						if(is_valid(bitmappart,8-j)==1){   //epeidi to offset prepei na einai apo 1 mexri 8
							rec_no=i*8+j;
							copyblockBuf-=BF_BLOCK_SIZE*sizeof(char);//proxorame to copyblockBuf kai pali stin arxi tou block
							copyblockBuf+=sizeof(short int);//pername to local depth
							copyblockBuf+=rec_no*indexes[pos].recsize;//proxorame rec_no eggrafes
							memcpy(temp_value,copyblockBuf,attrLength);//diabazoume to proto pedio(value)
							copyblockBuf+=attrLength;//proxorame gia na paroume to deutero pedio(recId)
							memcpy(&temp_recId,copyblockBuf,sizeof(int));//diabazoume to deutero pedio(recId)
							hv=JSHash(temp_value,attrLength);
							memcpy(&hashbits,&hv,sizeof(char));//pairnoume ta prota 8 bits tou akeraiou
							distance=total_depth;
	
							for(r=1;r<=total_depth;r++){	//diabazoume ta total_depth simantikotera psifeia tou hash value kai ipologizoume ti thesi tou header
								temp=is_valid(hashbits,r);	//stin opoia einai o deiktis pou theloume
								header_pos+=temp*pow(2,distance-1);
								distance--;
							}

							if((AM_errno=BF_GetThisBlock(fileDesc,0,&blockBuf))<0)//pairnoume to AM header
								return AM_errno;						
		
							temp_blockNum=ReadHeader(header_pos,&blockBuf);//blepoume pou prepei na paei i eggrafi
	
							if((AM_errno=BF_UnpinBlock(fileDesc,0,1))<0)
								return AM_errno;
							
							if(temp_blockNum!=blockNum){//an i eggrafi prepei na allaxei block
								set_bit(&bitmappart,8-j,0);//kanoume 0 to bit gia autin tin eggrafi	
								memcpy((finalblockBuf-(i+1)*sizeof(char)),&bitmappart,sizeof(char));
								if((AM_errno=BF_GetThisBlock(fileDesc,temp_blockNum,&blockBuf))<0)//pairnoume to allo(kainourio) block
									return AM_errno;
								copyblockBuf=blockBuf;
								copyblockBuf+=BF_BLOCK_SIZE*sizeof(char);//proxorame to blockBuf sto telos tou block gia na diabasoume to bitmap(apo dexia pros ta aristera)

								for(k=0;k<=(indexes[pos].maxrecs/8);k++){//to bitmap exei megethos maxrecs bits
									if(k!=(indexes[pos].maxrecs/8)){
										for(l=7;l>=0;l--){//diabazoume to bitmap ana xaraktira				
											memcpy(&bitmappart,(copyblockBuf-(k+1)*sizeof(char)),sizeof(char));
											if(is_valid(bitmappart,l+1)==0){   //epidi to offset prepei na einai apo 1 mexri 8
												set_bit(&bitmappart,l+1,1);//kanoume to bit 1 afou tha mpei i eggrafi
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
										for(l=0;l<=(indexes[pos].maxrecs%8);l++){//diabazoume to bitmap ana xaraktira				
											memcpy(&bitmappart,(copyblockBuf-(k+1)*sizeof(char)),sizeof(char));
											if(is_valid(bitmappart,8-l)==0){   //epeidi to offset prepei na einai apo 1 mexri 8
												set_bit(&bitmappart,8-l,1);//kanoume to bit 1 afou tha mpei i eggrafi	
												memcpy((copyblockBuf-(k+1)*sizeof(char)),&bitmappart,sizeof(char));				
												rec_no=k*8+l;
												found=1;
												break;
											}
										}
										if(found==1)
											break;
									}
								}//kleinei to for me ta k,j
								//grafoume tin eggrafi sto kainourio block
								copyblockBuf-=BF_BLOCK_SIZE*sizeof(char);//proxorame to copyblockBuf kai pali stin arxi tou block
								copyblockBuf+=sizeof(short int);//pername to local depth
								copyblockBuf+=rec_no*indexes[pos].recsize;//proxorame rec_no eggrafes
								memcpy(copyblockBuf,temp_value,attrLength);//grafoume to proto pedio(value)
								copyblockBuf+=attrLength;
								memcpy(copyblockBuf,&temp_recId,sizeof(int));//grafoume to deutero pedio(recId)	
								if((AM_errno=BF_UnpinBlock(fileDesc,temp_blockNum,1))<0)
									return AM_errno;					
						
							}//endif
						}// if is_valid
					header_pos=0;
					}//for j tou else
		
				}//else gia to teleutaio i
			}//to megalo for
			if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
				return AM_errno;
			//tora emeine mono i trexousa eggrafi na doume pou tha paei
			hv=JSHash(value,strlen(value));
			memcpy(&hashbits,&hv,sizeof(char));//pairnoume ta prota 8 bits tou akeraiou
			distance=total_depth;
	
			for(i=1;i<=total_depth;i++){	//diabazoume ta total_depth simantikotera psifeia tou hash value kai ipologizoume ti thesi tou header
				temp=is_valid(hashbits,i);	//stin opoia einai o deiktis pou theloume
				header_pos+=temp*pow(2,distance-1);
				distance--;
			}
			blockNum=ReadHeader(header_pos,&blockBuf);//to periexomeno tis header_pos thesis tou header
			if((AM_errno=BF_GetThisBlock(fileDesc,blockNum,&blockBuf))<0)//pairnoume to block/kado pou theloume
				return AM_errno;
			copyblockBuf=blockBuf;
			copyblockBuf+=BF_BLOCK_SIZE*sizeof(char);//proxorame to blockBuf sto telos tou block gia na diabasoume to bitmap(apo dexia pros ta aristera)

			for(i=0;i<=(indexes[pos].maxrecs/8);i++){//to bitmap exei megethos maxrecs bits
				if(i!=(indexes[pos].maxrecs/8)){
					for(j=7;j>=0;j--){//diabazoume to bitmap ana xaraktira				
						memcpy(&bitmappart,(copyblockBuf-(i+1)*sizeof(char)),sizeof(char));
						if(is_valid(bitmappart,j+1)==0){   //epidi to offset prepei na einai apo 1 mexri 8
							set_bit(&bitmappart,j+1,1);//kanoume to bit 1 afou tha mpei i eggrafi
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
					for(j=0;j<=(indexes[pos].maxrecs%8);j++){//diabazoume to bitmap ana xaraktira				
						memcpy(&bitmappart,(copyblockBuf-(i+1)*sizeof(char)),sizeof(char));
						if(is_valid(bitmappart,8-j)==0){   //epeidi to offset prepei na einai apo 1 mexri 8
							set_bit(&bitmappart,8-j,1);//kanoume to bit 1 afou tha mpei i eggrafi					
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
			if(found==1){//an exei xoro to block
				copyblockBuf-=BF_BLOCK_SIZE*sizeof(char);//proxorame to copyblockBuf kai pali stin arxi tou block
				copyblockBuf+=sizeof(short int);//pername to local depth
				copyblockBuf+=rec_no*indexes[pos].recsize;//proxorame rec_no eggrafes
				memcpy(copyblockBuf,value,attrLength);//grafoume to proto pedio(value)
				copyblockBuf+=attrLength;
				memcpy(copyblockBuf,&recId,sizeof(int));//grafoume to deutero pedio(recId)
				//if((AM_errno=BF_UnpinBlock(fileDesc,alloc_blockNum,1))<0)
				//	return AM_errno;
				if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
					return AM_errno;
			}
			else{//an kai pali den exei xoro to block
				//if((AM_errno=BF_UnpinBlock(fileDesc,alloc_blockNum,1))<0)
				//	return AM_errno;
				if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
					return AM_errno;
				if((AM_errno=AM_InsertEntry(fileDesc,attrType,attrLength,value,recId))<0)//anadromi
					return AM_errno;
			}
		}//end if local<total
		else if(local_depth==total_depth && total_depth<MAX_DEPTH){
			if((AM_errno=BF_GetThisBlock(fileDesc,0,&blockBuf))<0)//pairnoume to AM header
				return AM_errno;
				
			WriteHeader(-1,total_depth+1,&blockBuf);//auxanoume kata ena to oliko bathos sto header
	
			for(i=pow(2,total_depth)-1;i>=0;i--){//diplasiasmos tou pinaka deikton sto header
				blockNum=ReadHeader(i,&blockBuf);
				WriteHeader(2*i+1,blockNum,&blockBuf);
				WriteHeader(2*i,blockNum,&blockBuf);
			}

			if((AM_errno=BF_UnpinBlock(fileDesc,0,1))<0)
				return AM_errno;
			//tora eimaste pleon stin periptosi local<total depth
			//blockNum=ReadHeader(-1,&blockBuf);//dokimastiko!!!!!!!!!!!!!!!!!!!!!
			if((AM_errno=AM_InsertEntry(fileDesc,attrType,attrLength,value,recId))<0)//anadromi
				return AM_errno;
		}//if tou local==total
		else{//o pinakas den mporei na diplasiastei allo,opote telos
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
	
	if(fileDesc<0 || fileDesc>24){ // mh egkyros anagnoristikos arithmos arxeiou
                  
        	AM_errno=BFE_FD;
		return BFE_FD;
    	}

	for(i=0;i<MAXINDEXES;i++)
		if(fileDesc==indexes[i].fileDesc){
			pos=i;
			break;
		}
	//pairnoume tin hash value analoga me ton tipo tou pediou
	if(attrType=='c' || attrType=='i' || attrType=='f')
		hv=JSHash(value,strlen(value));

	else{
		AM_errno=AME_ATTRTYPEFAULT;
		return AM_errno;
	}
		
	memcpy(&hashbits,&hv,sizeof(char));//pairnoume ta prota 8 bits tou akeraiou

	if((AM_errno=BF_GetThisBlock(fileDesc,0,&blockBuf))<0)//pairnoume to AM header
		return AM_errno;

	total_depth=ReadHeader(-1,&blockBuf);//diabazoume to oliko bathos apo to header
	
	distance=total_depth;
	
	for(i=1;i<=total_depth;i++){	//diabazoume ta total_depth simantikotera psifeia tou hash value kai ipologizoume ti thesi tou header
		temp=is_valid(hashbits,i);	//stin opoia einai o deiktis pou theloume
		header_pos+=temp*pow(2,distance-1);
		distance--;
	}

	blockNum=ReadHeader(header_pos,&blockBuf);//to periexomeno tis header_pos thesis tou header

	if((AM_errno=BF_UnpinBlock(fileDesc,0,1))<0)
		return AM_errno;

	if((AM_errno=BF_GetThisBlock(fileDesc,blockNum,&blockBuf))<0)//pairnoume to block/kado pou theloume
		return AM_errno;

	copyblockBuf=blockBuf;

	copyblockBuf+=sizeof(short int);//pername to local depth

	for(i=0;i<indexes[pos].maxrecs;i++){
		memcpy(thisvalue,copyblockBuf,attrLength);//pernoume to proto pedio
		if(strcmp(thisvalue,value)==0){//an to trexon value einai idio me auto pou psaxnoume
			mpike=1;
			copyblockBuf+=attrLength;
			memcpy(&thisrecId,copyblockBuf,sizeof(int));//pernoume kai to deutero pedio(recId)
			copyblockBuf+=sizeof(int);
			if(thisrecId==recId){//an kai to deutero pedio einai idio me auto pou psaxnoume
				recNo=i;
				break;
			}
		}
		if(mpike==0)
			copyblockBuf+=indexes[pos].recsize;//proxorame mia eggrafi gia na paroume tin epomeni
	}
	
	if(recNo!=-1){//an ontos brethike i eggrafi
		blockBuf+=BF_BLOCK_SIZE*sizeof(char);//proxorame to blockBuf(to copyblockBuf exei metakinithei arketa) sto telos tou block gia na diabasoume to bitmap(apo dexia pros ta aristera)
		memcpy(&bitmappart,(blockBuf-((recNo/BITSOFCHAR)+1)*sizeof(char)),sizeof(char));//pernoume ton recNo/8 + 1 xaraktira apo to telos tou block
		set_bit(&bitmappart,BITSOFCHAR-(recNo%BITSOFCHAR),1);//kanoume to bit 0 stin katallili thesi
		memcpy((blockBuf-((recNo/BITSOFCHAR)+1)*sizeof(char)),&bitmappart,sizeof(char));
		if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
			return AM_errno;		
		AM_errno=AME_OK;
		return AME_OK;	
	}
	else{//i eggrafi de brethike
		AM_errno=AME_RECNOTFOUND;
		return AME_RECNOTFOUND;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int AM_OpenIndexScan(int fileDesc,char attrType,int attrLength,int op,char *value){

	int i,j,pos=-1,thesi,distance,temp,header_pos=0,total_depth,blockNum,thisrecId,maxblockNum;
	unsigned int hv;
	char *blockBuf,*copyblockBuf,hashbits,*thisvalue;
	
	for(i=0;i<MAXSCANS;i++)//psaxnoume eleutheri thesi ston pinaka saroseon
		if(scans[i].isfree==0){
			pos=i;
			scans[pos].isfree=1;
			break;
		}
	
	if(pos==-1){//an de brethei epistrefoume lathos
		AM_errno=AME_FULLSCANARRAY;
		return AME_FULLSCANARRAY;
	}
	
	for(i=0;i<MAXINDEXES;i++)
		if(fileDesc==indexes[i].fileDesc){
			thesi=i;
			break;
		}

	//enimerosi ton melon tou struct
	scans[pos].fileDesc=fileDesc;
	scans[pos].attrType=attrType;
	scans[pos].attrLength=attrLength;
	scans[pos].op=op;
	for(i=0;i<attrLength;i++)
		scans[pos].value='\0';
	strcpy(scans[pos].value,value);
	
	if((AM_errno=BF_GetThisBlock(fileDesc,0,&blockBuf))<0)//pairnoume to AM header
			return AM_errno;

	total_depth=ReadHeader(-1,&blockBuf);//diabazoume to oliko bathos apo to header
	
	/*temparray=(int*)calloc(pow(2,total_depth),sizeof(int));	//enas pinakas me 2^total_depth theseis ston opoio tha apothikeuoume
															//ta diaforetika blockNums pou uparxoun sto header oste na kseroume posa 
															//kai poia einai ta blocks pou tha sarosoume
	
	for(i=0;i<pow(2,total_depth);i++)//arxikopoioume se -1 gia na kseroume poies theseis sto telos den tha gemisoun
		temparray[i]=-1;
		
	for(i=0;i<pow(2,total_depth);i++){//ftiaxnoume ton temparray opos eipame parapano
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
	
	//theloume na broume to megalitero blockNum sto header gia na saronoume mexri ekei
	maxblockNum=ReadHeader(0,&blockBuf);//arxiki timi
	
	for(i=0;i<pow(2,total_depth);i++){
		blockNum=ReadHeader(i,&blockBuf);
		if(blockNum>maxblockNum)
			maxblockNum=blockNum;
	}
	
	if(op==EQUAL){
		//pairnoume tin hash value analoga me ton tipo tou pediou
		if(attrType=='c' || attrType=='i' || attrType=='f')
			hv=JSHash(value,strlen(value));
		else{
			AM_errno=AME_ATTRTYPEFAULT;
			return AM_errno;
		}
		
		memcpy(&hashbits,&hv,sizeof(char));//pairnoume ta prota 8 bits tou akeraiou
		
		distance=total_depth;
	
		for(i=1;i<=total_depth;i++){	//diabazoume ta total_depth simantikotera psifeia tou hash value kai ipologizoume ti thesi tou header
			temp=is_valid(hashbits,i);	//stin opoia einai o deiktis pou theloume
			header_pos+=temp*pow(2,distance-1);
			distance--;
		}

		blockNum=ReadHeader(header_pos,&blockBuf);//gia na doume se poio block tha psaksoume

		if((AM_errno=BF_UnpinBlock(fileDesc,0,1))<0)
			return AM_errno;
	
		if((AM_errno=BF_GetThisBlock(fileDesc,blockNum,&blockBuf))<0)//pairnoume to blockNum block sto opoio briskontai i eggrafes me value
			return AM_errno;
		
		copyblockBuf=blockBuf;

		copyblockBuf+=sizeof(short int);//pername to local depth
	
		for(i=0;i<indexes[thesi].maxrecs;i++){
			memcpy(thisvalue,copyblockBuf,attrLength);//pernoume to proto pedio
			if(strcmp(thisvalue,value)==0){//an to trexon value einai idio me auto pou psaxnoume
				copyblockBuf+=attrLength;
				memcpy(&thisrecId,copyblockBuf,sizeof(int));//pernoume kai to deutero pedio(recId)
				//kratame ton kado kai tin eggrafi stin opoia tha stamatisoume ti sarosi prin epistrepsei i sinartisi
				scans[pos].bucket=blockNum;
				scans[pos].recNo=i;
				return thisrecId;
			}
			copyblockBuf+=indexes[thesi].recsize;//an den einai idio to value,pame stin epomeni eggrafi
		}
		if((AM_errno=BF_UnpinBlock(fileDesc,blockNum,1))<0)
			return AM_errno;
	}//end if op=EQUAL
	
	else if(op==NOT_EQUAL){
		//pairnoume tin hash value analoga me ton tipo tou pediou
		if(attrType=='c' || attrType=='i' || attrType=='f')
			hv=JSHash(value,strlen(value));
		else{
			AM_errno=AME_ATTRTYPEFAULT;
			return AM_errno;
		}
		
		memcpy(&hashbits,&hv,sizeof(char));//pairnoume ta prota 8 bits tou akeraiou
		
		distance=total_depth;
	
		for(i=1;i<=total_depth;i++){	//diabazoume ta total_depth simantikotera psifeia tou hash value kai ipologizoume ti thesi tou header
			temp=is_valid(hashbits,i);	//stin opoia einai o deiktis pou theloume
			header_pos+=temp*pow(2,distance-1);
			distance--;
		}

		blockNum=ReadHeader(header_pos,&blockBuf);//gia na doume poio einai to epikindino block
		
		//tora ksekiname na saronoume ola ta block-kadous.An briskomaste sto epikindino block(blockNum),tote elegxoume to value na min einai idio
		//me auto tou orismatos,allios gia opoiodipote allo block epistrefoume tin proti eggrafi pou sinantame
		for(i=0;i<=maxblockNum;i++){//gia olous tous kadous pou iparxoun
			if((AM_errno=BF_GetThisBlock(fileDesc,i,&blockBuf))<0)
				return AM_errno;
		
			copyblockBuf=blockBuf;

			copyblockBuf+=sizeof(short int);//pername to local depth
				
			if(i==blockNum){//an eimaste sto "epikindino" block
				for(j=0;j<indexes[thesi].maxrecs;j++){
					memcpy(thisvalue,copyblockBuf,attrLength);//pernoume to proto pedio
					if(strcmp(thisvalue,value)!=0){//an to trexon value den einai idio me auto pou psaxnoume
						copyblockBuf+=attrLength;
						memcpy(&thisrecId,copyblockBuf,sizeof(int));//pernoume kai to deutero pedio(recId)
						//kratame ton kado kai tin eggrafi stin opoia tha stamatisoume ti sarosi prin epistrepsei i sinartisi
						scans[pos].bucket=blockNum;
						scans[pos].recNo=j;
						return thisrecId;
					}
					copyblockBuf+=indexes[thesi].recsize;//an den einai idio to value,pame stin epomeni eggrafi
				}
				if((AM_errno=BF_UnpinBlock(fileDesc,i,1))<0)
					return AM_errno;
			}
			else{//se opoiodipote allo block
				for(j=0;j<indexes[thesi].maxrecs;j++){
					copyblockBuf+=attrLength;
					memcpy(&thisrecId,copyblockBuf,sizeof(int));//pernoume kai to deutero pedio(recId)
					//kratame ton kado kai tin eggrafi stin opoia tha stamatisoume ti sarosi prin epistrepsei i sinartisi
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
	
	if(scans[scanDesc].isfree==0){//an den einai anoixti i sarosi gia auton ton scanDesc
		AM_errno=AME_INDEXSCANALREADYCLOSED;
		return AME_INDEXSCANALREADYCLOSED;
	}
	
	for(i=0;i<MAXINDEXES;i++)
		if(scans[scanDesc].fileDesc==indexes[i].fileDesc){
			thesi=i;
			break;
		}
	
	if((AM_errno=BF_GetThisBlock(scans[scanDesc].fileDesc,0,&blockBuf))<0)//pairnoume to AM header
			return AM_errno;
	
	total_depth=ReadHeader(-1,&blockBuf);//diabazoume to oliko bathos apo to header
	
	//theloume na broume to megalitero blockNum sto header gia na saronoume mexri ekei(den kratame kapou to palio maxblockNum giati mporei auto
	//na exei allaksei apo tin proigoumeni klisi)
	maxblockNum=ReadHeader(0,&blockBuf);//arxiki timi
	
	for(i=0;i<pow(2,total_depth);i++){
		blockNum=ReadHeader(i,&blockBuf);
		if(blockNum>maxblockNum)
			maxblockNum=blockNum;
	}
	
	if((AM_errno=BF_UnpinBlock(scans[scanDesc].fileDesc,0,1))<0)
		return AM_errno;
	
	if(scans[scanDesc].op==EQUAL){
			
		if((AM_errno=BF_GetThisBlock(scans[scanDesc].fileDesc,scans[scanDesc].bucket,&blockBuf))<0)//pigainoume ston kado pou exoume kratisei
			return AM_errno;
		
		copyblockBuf=blockBuf;

		copyblockBuf+=sizeof(short int);//pername to local depth
	
		for(i=(scans[scanDesc].recNo)+1;i<indexes[thesi].maxrecs;i++){//sinexizoume apo tin epomeni eggrafi
			memcpy(thisvalue,copyblockBuf,scans[scanDesc].attrLength);//pernoume to proto pedio
			if(strcmp(thisvalue,scans[scanDesc].value)==0){//an to trexon value einai idio me auto pou psaxnoume
				copyblockBuf+=scans[scanDesc].attrLength;
				memcpy(&thisrecId,copyblockBuf,sizeof(int));//pernoume kai to deutero pedio(recId)
				//kratame tin eggrafi stin opoia tha stamatisoume ti sarosi prin epistrepsei i sinartisi
				scans[scanDesc].recNo=i;
				return thisrecId;
			}
			copyblockBuf+=indexes[thesi].recsize;//an den einai idio to value,pame stin epomeni eggrafi
		}
		if((AM_errno=BF_UnpinBlock(scans[scanDesc].fileDesc,scans[scanDesc].bucket,1))<0)
			return AM_errno;
	}//end if op=EQUAL

	if(scans[scanDesc].op=NOT_EQUAL){
		//pairnoume tin hash value analoga me ton tipo tou pediou
		if(scans[scanDesc].attrType=='c' || scans[scanDesc].attrType=='i' || scans[scanDesc].attrType=='f')
			hv=JSHash(scans[scanDesc].value,strlen(scans[scanDesc].value));
		else{
			AM_errno=AME_ATTRTYPEFAULT;
			return AM_errno;
		}
		
		memcpy(&hashbits,&hv,sizeof(char));//pairnoume ta prota 8 bits tou akeraiou
		
		distance=total_depth;
	
		for(i=1;i<=total_depth;i++){	//diabazoume ta total_depth simantikotera psifeia tou hash value kai ipologizoume ti thesi tou header
			temp=is_valid(hashbits,i);	//stin opoia einai o deiktis pou theloume
			header_pos+=temp*pow(2,distance-1);
			distance--;
		}

		blockNum=ReadHeader(header_pos,&blockBuf);//gia na doume poio einai to epikindino block
		
		//tora ksekiname na saronoume ola ta block-kadous.An briskomaste sto epikindino block(blockNum),tote elegxoume to value na min einai idio
		//me auto tou orismatos,allios gia opoiodipote allo block epistrefoume tin proti eggrafi pou sinantame
		for(i=scans[scanDesc].bucket;i<=maxblockNum;i++){//gia olous tous kadous pou iparxoun
			if((AM_errno=BF_GetThisBlock(scans[scanDesc].fileDesc,i,&blockBuf))<0)
				return AM_errno;
		
			if(i==scans[scanDesc].bucket){
			
				copyblockBuf=blockBuf;

				copyblockBuf+=sizeof(short int);//pername to local depth
				
				if(i==blockNum){//an eimaste sto "epikindino" block
					for(j=(scans[scanDesc].recNo)+1;j<indexes[thesi].maxrecs;j++){
						memcpy(thisvalue,copyblockBuf,scans[scanDesc].attrLength);//pernoume to proto pedio
						if(strcmp(thisvalue,scans[scanDesc].value)!=0){//an to trexon value den einai idio me auto pou psaxnoume
							copyblockBuf+=scans[scanDesc].attrLength;
							memcpy(&thisrecId,copyblockBuf,sizeof(int));//pernoume kai to deutero pedio(recId)
							//kratame ton kado kai tin eggrafi stin opoia tha stamatisoume ti sarosi prin epistrepsei i sinartisi
							scans[scanDesc].bucket=blockNum;
							scans[scanDesc].recNo=j;
							return thisrecId;
						}
						copyblockBuf+=indexes[thesi].recsize;//an den einai idio to value,pame stin epomeni eggrafi
					}
				}
				else{//se opoiodipote allo block
					for(j=(scans[scanDesc].recNo)+1;j<indexes[thesi].maxrecs;j++){
						copyblockBuf+=scans[scanDesc].attrLength;
						memcpy(&thisrecId,copyblockBuf,sizeof(int));//pernoume kai to deutero pedio(recId)
						//kratame ton kado kai tin eggrafi stin opoia tha stamatisoume ti sarosi prin epistrepsei i sinartisi
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

				copyblockBuf+=sizeof(short int);//pername to local depth
				
				if(i==blockNum){//an eimaste sto "epikindino" block
					for(j=0;j<indexes[thesi].maxrecs;j++){
						memcpy(thisvalue,copyblockBuf,scans[scanDesc].attrLength);//pernoume to proto pedio
						if(strcmp(thisvalue,scans[scanDesc].value)!=0){//an to trexon value den einai idio me auto pou psaxnoume
							copyblockBuf+=scans[scanDesc].attrLength;
							memcpy(&thisrecId,copyblockBuf,sizeof(int));//pernoume kai to deutero pedio(recId)
							//kratame ton kado kai tin eggrafi stin opoia tha stamatisoume ti sarosi prin epistrepsei i sinartisi
							scans[scanDesc].bucket=blockNum;
							scans[scanDesc].recNo=j;
							return thisrecId;
						}
						copyblockBuf+=indexes[thesi].recsize;//an den einai idio to value,pame stin epomeni eggrafi
					}
				}
				else{//se opoiodipote allo block
					for(j=0;j<indexes[thesi].maxrecs;j++){
						copyblockBuf+=scans[scanDesc].attrLength;
						memcpy(&thisrecId,copyblockBuf,sizeof(int));//pernoume kai to deutero pedio(recId)
						//kratame ton kado kai tin eggrafi stin opoia tha stamatisoume ti sarosi prin epistrepsei i sinartisi
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

	
	
	AM_errno=AME_EOF;//an de brethei eggrafi kai den epistrepsei noritera i sinartisi
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




