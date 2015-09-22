#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "BF_Lib.h"

void BF_Init(){

	int i,j;


	lru=0;
	midmem=(char*)malloc(BF_BUFFER_SIZE*BF_BLOCK_SIZE*sizeof(char));  // arxikopoihsi tis endiamesis mnimis
	if(midmem==NULL){
		printf("Calloc error\n");
		exit(1);
	}	


	//arxikopoisi tou pinaka openfiles
	for(i=0;i<MAXOPENFILES;i++){
		openfiles[i].isfree=0;
		openfiles[i].filename=(char*)calloc(NAMELENGTH,sizeof(char));
		if(openfiles[i].filename==NULL){
		printf("Calloc error\n");
		exit(1);
	}

	}

	//arxikopoiisi tou pinaka me ta headers	
	for(i=0;i<MAXOPENFILES;i++){
		headers[i].headerblock=(char*)calloc(BF_BLOCK_SIZE,sizeof(char));
		if(headers[i].headerblock==NULL){
		printf("Calloc error\n");
		exit(1);
	}
		headers[i].counter=0;
	}

	//arxikopoisi tou pinaka memory
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
	if(fpcheck==NULL){	//tote to arxeio den iparxei ara to dimiourgoume emeis

		fp=fopen(fileName,"wb+");
		if(fp==NULL){
			BF_errno=-19;
			return BFE_OPENFILE;
		}

		header=(char*)calloc(BF_BLOCK_SIZE,sizeof(char));  //desmeush kai mhdenismos ton bits tou header
		if(header==NULL){
		printf("Calloc error\n");
		exit(1);
	}		
	
		fwrite(header,1,sizeof(header),fp);	//grapsimo tou header sto arxeio
		fflush(fp);
	
		fclose(fp);

		return BFE_OK;
	
	}
	else{	//to arxeio iparxei idi
		BF_errno=-18;
		return BFE_EXISTINGFILE;

	}
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int BF_DestroyFile(char *fileName){

	FILE *fpcheck;
	int i;

	fpcheck=fopen(fileName,"rb");
	if(fpcheck==NULL){	//tote to arxeio den iparxei ara epistrefoume kodiko lathous
		BF_errno=-19;
		return BFE_OPENFILE;
	}

	for(i=0;i<MAXOPENFILES;i++){//elegxoume an to arxeio einai anoixto
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

	for(i=0;i<MAXOPENFILES;i++){//elegxoume an to arxeio einai anoixto
		if(strcmp(openfiles[i].filename,fileName)==0){
			BF_errno=-12;
			isopen=1;
			break;
  		}
	}
	
		fp=fopen(fileName,"rb+");//anoigma gia update diabasma-grapsimo enos iparxontos arxeiou
		if(fp==NULL){
			BF_errno=-19;
			return BFE_OPENFILE;//problima me anoigma arxeiou
		}

	for(i=0;i<MAXOPENFILES;i++)	//i proti eleutheri thesi tou pinaka einai o filedes tou arxeiou
		if(openfiles[i].isfree==0){
			fd=i;
			fullarray=1;
			break;
		}


	if(fullarray==0){          //elegxos gia pliri lista anoixton arxeion,dld den mporo na anoikso allo
		BF_errno=BFE_FTABFULL;
		return BFE_FTABFULL;
	}


	//tora tha enimerosoume tin thesi auti tou pinaka anoixton arxeion me tis katalliles plirofories
	strcpy(openfiles[fd].filename,fileName);
	
	openfiles[fd].fp=fp;
		
	if(isopen==1){
		for(i=0;i<MAXOPENFILES;i++)
			if(strcmp(openfiles[i].filename,fileName)==0 && fd!=i){
				openfiles[fd].hd_index=openfiles[i].hd_index;//gia na min ksanafernoume to header apo to disko,kanoume to hd_index na deixnei sth
													//thesh tou pinaka domhs header pou briskete to hdh yparxon arxeio
				break;
			}
	}
	
	openfiles[fd].isfree=1;
	

	if(isopen==0){//an to arxeio auto den iparxei ston pinaka anoikton arxeion tote prepei na feroume to header sti mnimi
		fseek(openfiles[fd].fp,0,SEEK_SET);	//pame stin arxi tou arxeiou(apla gia sigouria)
		for(i=0;i<MAXOPENFILES;i++)
			if(headers[i].counter==0){
				thesi=i;
				break;
			}
		fflush(openfiles[fd].fp);
		fread(headers[thesi].headerblock,1,BF_BLOCK_SIZE,openfiles[fd].fp);//fernoume to header sti mnimi
		fflush(openfiles[fd].fp);
		openfiles[fd].hd_index=thesi;//bazoume to deikti na deixnei sto header
	}

	headers[openfiles[fd].hd_index].counter++;   //auksanoume ton arithmo ton anoixton arxeion (counter) pou xrhsimopoioun to INDEX header block
	return fd;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
int BF_CloseFile(int fileDesc){
	//gia na kleisei to arxeio prepei na min iparxei kapoio block tou pinned sti mmimi apo ton sigkekrimeno fileDesc.Episis,an ola ta block einai unpinned,tote prin
	//kleisoume to arxeio prepei na tsekaroume poia apo auta einai dirty oste na ta grapsoume piso sto disko kai meta na kleisoume to arxeio
	int i,j,pinned=0;
	
	
	if(fileDesc<0 || fileDesc>24){ // mh egkyros anagnoristikos arithmos arxeiou
                  
        	BF_errno=BFE_FD;
                return BFE_FD;
    	}

	for(i=0;i<BF_BUFFER_SIZE;i++)
		if(strcmp(openfiles[fileDesc].filename,memory[i].filename)==0)//an to arxeio exei kapoio block sti mnimi
			for(j=0;j<BF_BUFFER_SIZE;j++){
				if(memory[j].bytemap[fileDesc]==1){//an kapoio block einai pinned apo ton fileDesc	
					pinned=1;
					break;
				}
			}
		
	if(pinned==0){//an den iparxei pinned block tou arxeiou sti mnimi
		for(i=0;i<BF_BUFFER_SIZE;i++)
			if(strcmp(openfiles[fileDesc].filename,memory[i].filename)==0)
				if(memory[i].dirty==1){//an to block einai bromismeno
					fseek(openfiles[fileDesc].fp,(memory[i].blockNum+1)*BF_BLOCK_SIZE,SEEK_SET);//pame stin thesi pou brisketai to block sto arxeio(theoroume oti to blockNum pairnei times 0,1,2..)																
					fwrite(midmem+i*BF_BLOCK_SIZE*sizeof(char),1,BF_BLOCK_SIZE,openfiles[fileDesc].fp);//grapsimo tou block sto disko
					fflush(openfiles[fileDesc].fp);
				}
				
		headers[openfiles[fileDesc].hd_index].counter--;
		if(headers[openfiles[fileDesc].hd_index].counter==0){//an kanenas den xrisimopoiei to header,to grafoume piso sto arxeio
			fseek(openfiles[fileDesc].fp,0,SEEK_SET);
			fwrite(headers[openfiles[fileDesc].hd_index].headerblock,1,BF_BLOCK_SIZE,openfiles[fileDesc].fp);
			fflush(openfiles[fileDesc].fp);
			for(i=0;i<BF_BUFFER_SIZE;i++)
				//an einai o teleuteos pou klinei to arxeio
				if(strcmp(memory[i].filename,openfiles[fileDesc].filename)==0)
					memory[i].isfree=0;
		}				
			
		fflush(openfiles[fileDesc].fp);
		
		if(fclose(openfiles[fileDesc].fp)==0)//kleisimo tou arxeiou kai epanarxikopoihsh ton melon ths theshs tou pinaka anoixton arxeion
			openfiles[fileDesc].isfree=0;
		else{//sfalma stin fclose
			perror("\n\nerror in fp\n\n");
			BF_errno=BFE_CLOSEFILE;
			return BFE_CLOSEFILE;
		}
	}
	else{//pinned=1 ara epistrofi lathous sxetiko me apopeira kleisimatos eno kapoio block einai karfomeno sti mnimi
		BF_errno=BFE_PINNEDCLOSE;
		return BFE_PINNEDCLOSE;
	}

	BF_errno=BFE_OK;
        return BFE_OK;

}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


