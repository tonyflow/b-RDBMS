#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HF_Lib.h"
#include "AM_Lib.h"
#include "parser3.h"


int VW_CreateSelect(int argc, char* argv[]){

	int scanDesc,check,found=0;
	relDesc recrel;
	attrDesc recattr;
	int scanDescattr;
	char* rec;
	int utc,j;
	char** utcargs;    //pinakas mesa ston opoion tha ftiaksoume ta orismata ths UT_Create

	//elegxos gia to an dinetai akribos mia sxesi i parapano







	if((scanDesc=HF_OpenFileScan(viewfd,sizeof(viewDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
		HF_PrintError("Problem while opening file scan");

	check=HF_FindNextRec(scanDesc,rec);
	while(check!=HFE_EOF){

		if(check>0){
			found=1;			
			break;
		}
		check=HF_FindNextRec(scanDesc,rec);
	}

	if(HF_CloseFileScan(scanDesc)<0)
		HF_PrintError("Error while closing scan");
	
	if(found==0){//an den iparxei i opsi sto viewCat

		//desmeush xorou gia ton pinaka utcargs gia na enimerosoume ta relcat,attrcat me UT_create
		utcargs=malloc((2*atoi(argv[2])+2)*sizeof(char**));
	
		for(i=0;i<(2*atoi(argv[2])+2);i++)
			utcargs[i]=malloc(256*sizeof(char));
	
		sprintf(utcargs[0],"%s","create");
		sprintf(utcargs[1],"%s",argv[1]);
	
		for(i=2;i<(2*atoi(argv[2])+2);i++)//pername ston utcargs ta onomata ton pedion
			if(i%2==0)
				sprintf(utcargs[i],"%s",argv[i+2]);
			
					
		//skanaroume to attrCat gia na broume tous tipous ton pedion kai na tous perasoume ston utcargs	
		j=3;
		for(i=4;i<=(2*atoi(argv[2])+2);i=i+2){
		
		found=0;
	
		if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[3]))<0)
			HF_PrintError("Problem while opening file scan");
		
		check=HF_FindNextRec(scanDescattr,(char*)recattr);
			
		while(check!=HFE_EOF){
			//brikame to probalomeno pedio san pedio tou relname
			if(strcmp(recattr.attrname,argv[i])==0){
				recsize+=recattr.attrlength;
				found=1;
				
				if(recattr.attrtype=='c')
					sprintf(utcargs[j],"%c%d",recattr.attrtype,recattr.attrlength);
				else
					sprintf(utcargs[j],"%c",recattr.attrtype);
				j+=2;	
				//telos kataskeuhs
				
				break;
			}
			
			check=HF_FindNextRec(scanDescattr,(char*)&recattr);
		}
		
		if(HF_CloseFileScan(scanDescattr)<0)
			HF_PrintError("CreateSelect : Error while closing scan");

		
		if(found==0){
			printf("REQUESTED FIELD FOR PROJECTION NOT FOUND IN RELCAT... RETYPE SELECTION USING DIFFERENT ARGUMENTS\n");
			return 1;
		}
		

		if((utc=UT_create(2*atoi(argv[2])+2,utcargs))!=0){//dimiourgia tis sxesis gia na enimerosoume ta Relcat kai Attrcat
			printf("View could not be created\n");
			return -1;
		}
	}
	else{
		printf("Relationship already exists\n");
		return -1;
	}

	//enimerosi ton Viewcat,Viewattrcat






	return 0;	
	

}



int VW_CreateJoin(int argc, char* argv[]){




}






int VW_Destroy(int argc, char* argv[]){//argv[0]=destroy,argv[1]=onoma opsis i p1,argv[2]=NULL

	int scanDescview,scanDescview1,scanDescview2;
	int scanDescrel,scanDescrel1,scanDescrel2;
	int viewfd,relfd;
	int check,check1,check2,found=0,flag=0;
	relDesc recrel,recrel1,recrel2;
	attrDesc recattr;
	viewrelDesc viewrel,viewrel1,viewrel2;
	viewattrDesc viewattr;
	
	
	

	if(strcmp(argv[0],"destroyMview")==0){//an prokeitai gia opsi
		
		if((scanDescview=HF_OpenFileScan(viewfd,sizeof(viewDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
				HF_PrintError("Problem while opening file scan");

		check=HF_FindNextRec(scanDescview,(char*)&viewrel);
	
		while(check!=HFE_EOF){
			if(check>=0){
				found=1;
				break;
			}
		
			check=HF_FindNextRec(scanDescview,(char*)&viewrel);		
	
		}
	
		if(HF_CloseFileScan(scanDescview)<0)
			HF_PrintError("SELECT : Error while closing scan");
	
	
	
		if(found!=0){//an uparxei i opsi pou zitithike st viewCat
					 // pame na psaksoume an i opsi xrisimopoieitai ws vasi gia kapoia alli
					 
			//akolouthoun duo parallila skanarismata sto viewcat gia ta relname1,relname2
			
			if((scanDescview1=HF_OpenFileScan(viewfd,sizeof(viewDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
				HF_PrintError("Problem while opening file scan");
				
			if((scanDescview2=HF_OpenFileScan(viewfd,sizeof(viewDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
				HF_PrintError("Problem while opening file scan");
				
				
            check1=HF_FindNextRec(scanDescview1,(char*)&viewrel1);
            check2=HF_FindNextRec(scanDescview2,(char*)&viewrel2);

			while(check1!=HFE_EOF && check2!=HFE_EOF){
				if( strcmp(viewrel1.relname1,argv[1])==0 || strcmp(viewrel2.relname2,argv[1])==0){
					flag=1;
					printf("\nH zitoumeni opsi xrisimopoieitai ws vasi gia kapoia alli, opote den mporei na katasrtafei!!!\n");
					return -1;
				}

				check1=HF_FindNextRec(scanDescview1,(char*)&viewrel1);
				check2=HF_FindNextRec(scanDescview2,(char*)&viewrel2);

			}//end tou while

			if(HF_CloseFileScan(scanDescview1)<0)
				HF_PrintError("SELECT : Error while closing scan");
				
            if(HF_CloseFileScan(scanDescview2)<0)
				HF_PrintError("SELECT : Error while closing scan");
				
				
				
			if(flag==0){
				//edw tha prepei na ginei to destroy
				//diladi apla na kalesw tin DM_Delete me ta katallila orismata
				//i UT_destroy kaleitai eksw sto telos
			}

		
		}//end tou found!=0
		else{
			printf("\nH opsi pou zitithike den uparxei sto viewCat...\n");
			return -1;
		}
	
	}//end tou if gia opsi
	else if(strcmp(argv[0],"destroy")==0){ // an prokeitai gia prwtarxiko pinaka
	
		if((scanDescrel=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
			HF_PrintError("Problem while opening file scan");

		check=HF_FindNextRec(scanDescrel,(char*)&recrel);
	
		while(check!=HFE_EOF){
			if(check>=0){
				found=1;
				break;
			}
		
			check=HF_FindNextRec(scanDescrel,(char*)&recrel);		
	
		}
	
		if(HF_CloseFileScan(scanDescrel)<0)
			HF_PrintError("SELECT : Error while closing scan");
			
			
			
		if(found!=0){//an uparxei o prwtarxikos pinakas pou zitithike sto relCat
                     // pame na psaksoume an o prwtarxikos pinakas xrisimopoieitai ws vasi gia kapoia alli
                     
            //akolouthoun duo parallila skanarismata sto relcat gia ta relname1,relname2
            
            if((scanDescrel1=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
				HF_PrintError("Problem while opening file scan");
				
            if((scanDescrel2=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
				HF_PrintError("Problem while opening file scan");
				
            check1=HF_FindNextRec(scanDescrel1,(char*)&recrel1);
            check2=HF_FindNextRec(scanDescrel2,(char*)&recrel2);

			while(check1!=HFE_EOF && check2!=HFE_EOF){
				if( strcmp(recrel1.relname1,argv[1])==0 || strcmp(recrel2.relname2,argv[1])==0){
					flag=1;
					printf("\nH zitoumeni opsi xrisimopoieitai ws vasi gia kapoia alli, opote den mporei na katasrtafei!!!\n");
					return -1;
				}

				check1=HF_FindNextRec(scanDescview1,(char*)&recrel1);
				check2=HF_FindNextRec(scanDescview2,(char*)&recrel2);
			}// end tou while
			
			if(HF_CloseFileScan(scanDescrel1)<0)
				HF_PrintError("SELECT : Error while closing scan");

            if(HF_CloseFileScan(scanDescrel2)<0)
				HF_PrintError("SELECT : Error while closing scan");
				

		
		}//end tou found!=0
		else{
			printf("\nO prwtarxikos pinakas pou zitithike den uparxei sto relCat...\n");
			return -1;
		}
	
	
	
	}

	UT_destroy(argv[0],argv[1]); // katastrefoume tin opsi i ton prwtarxiko pinaka pou zitithike
	
	

}
