#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser3.h"
#include "HF_Lib.h"
#include "AM_Lib.h"

char* interestingfields(char*,NOI*,int,int);  //noo=number of offsets
void printArgs(int,char**);
int UT_create(int,char**);
int UT_buildindex(int,char**);
int UT_destroy(int,char**);
int UT_quit(void);
int DM_select(int, char**);
int DM_join(int,char**);
int DM_delete(int,char**);
int DM_subtract(int,char**);
int DM_insert(int,char**);
int DM_add(int,char**);

int compare(char *telestis){

	if(strcmp(telestis,"=")==0)
		return 1;

	if(strcmp(telestis,"<")==0)
		return 2;

	if(strcmp(telestis,">")==0)
		return 3;

	if(strcmp(telestis,"<=")==0)
		return 4;

	if(strcmp(telestis,">=")==0)
		return 5;

	if(strcmp(telestis,"!=")==0)
		return 5;


}
char* interestingfields(char* buffer,NOI* inf,int noo,int recsize){  //noo=number of offsets

	int i;
	int masteroffset=0;
	int numberint;
	float numberfloat;
	char* pr;
	
	pr=malloc(recsize*sizeof(char));
	
	for(i=0;i<noo;i++){
		if(inf[i].type=='c'){
			memcpy((pr+masteroffset),buffer+inf[i].offset,inf[i].length*(sizeof(char)));
			masteroffset+=inf[i].length;
		}

		if(inf[i].type=='i'){
			memcpy(&numberint,buffer+inf[i].offset,sizeof(int));
			memcpy((pr+masteroffset),&numberint,sizeof(int));
			masteroffset+=sizeof(int);
		}
		if(inf[i].type=='f'){
			memcpy(&numberfloat,buffer+inf[i].offset,sizeof(float));
			memcpy((pr+masteroffset),&numberfloat,sizeof(float));
			masteroffset+=sizeof(float);
		}
	}
	
	return pr;
}

void printArgs(int argc, char **argv){
	int i;
	for(i=0;i<argc;++i)
		printf("argv[%d]=%s, ",i,argv[i]);
	printf("argv[%d]=%s\n\n",i,argv[i]);
	fflush(stdout);
}

int UT_create(int argc,char* argv[]){
	//printArgs(argc,argv);
	int i,j;
	int scanDesc,check,span,STRINGLENGTH,offsethelp;
	int found=0;
	relDesc newrelentry;
	attrDesc* newattrentry;
	char* rec=NULL;
	char* newrelname;
	char *start,*stringlength;
	char numbers[]="1234567890";

	//prepei na elegksoume an to dothen relname(argv[1]) yparxei HDH sto RELCAT
	//an yparxei epistrefoume lathos diaforetika proxorame sthn eisagogh katallhlhs eggrafhs sta RELCAT KAI ATTRCAT
	//gia na doume an yparxei eggrafh me onoma "relname" sto RELCAT ksekiname skanarisma me thn HF_OpenFileScan() sto RELCAT
	
	if((scanDesc=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0){
		HF_PrintError("Problem while opening file scan");
		return -1;		
	}
	
	printf("initaited scan\n");

	//desmeush xorou gia to rec
	rec=malloc(sizeof(relDesc));

	check=HF_FindNextRec(scanDesc,rec);

	while(check!=HFE_EOF){

		if(check<0){
			printf("Relname already exists\n");
			found=1;
		}

		check=HF_FindNextRec(scanDesc,rec);
	}

	if(HF_CloseFileScan(scanDesc)<0)
		HF_PrintError("Error while closing scan");
	
	
	printf("completed scan\n");

	printf("found is %d\n",found);


	if(found==0){ //h eggrafh den brethike opote proxorame sthn enhmerosh ton arxeio RELCAT kai ATTRCAT
		
		//enhmerosh tou arxeiou RELCAT
		strcpy(newrelentry.relname,argv[1]);	

		newrelentry.relwidth=0;
		for(i=3;i<argc;i=i+2){        //pername to onoma ths entolhs, to onoma ths sxeshs kai to attr#
			if(strcmp(argv[i],"'i'")==0)
				newrelentry.relwidth=newrelentry.relwidth+sizeof(int);
			if(strcmp(argv[i],"'f'")==0)
				newrelentry.relwidth=newrelentry.relwidth+sizeof(float);
			if(strpbrk(argv[i],"c")!=NULL){
				sscanf(argv[i],"'c%d'",&STRINGLENGTH);
				printf("%d\n",STRINGLENGTH);				
				newrelentry.relwidth=newrelentry.relwidth+STRINGLENGTH*sizeof(char);  //orizoume katallhla to relwidth;
			}
		}		
		newrelentry.attrcnt=((argc/2)-1);

		newattrentry=malloc(((argc/2)-1)*sizeof(attrDesc));  //desmeush xorou gia ton pinaka ton orismaton pou tha grapsoume sto ATTRCAT

		printf("%d\n",newrelentry.attrcnt);

		newrelentry.indexcnt=0;
		
		if(HF_InsertRec(relfd,(char*)(&newrelentry),sizeof(relDesc))<0){
			HF_PrintError("Problem with inserting record for RELCAT");
			exit(3);
		}
		//enhmerosh tou arxeiou ATTRCAT
		printf("%d\n",((argc/2)-1)*sizeof(attrDesc));
		
		j=2;
		offsethelp=0;
		for(i=0;i<((argc/2)-1);i++){
			strcpy(newattrentry[i].relname,argv[1]);
			strcpy(newattrentry[i].attrname,argv[j]);
			
			newattrentry[i].offset=offsethelp;

			if(strcmp(argv[j+1],"'i'")==0){
				newattrentry[i].attrlength=sizeof(int);
				newattrentry[i].attrtype='i';			
			}

			if(strcmp(argv[j+1],"'f'")==0){
				newattrentry[i].attrlength=sizeof(float);
				newattrentry[i].attrtype='f';
			}
			if(strpbrk(argv[j+1],"c")!=NULL){
				sscanf(argv[i],"'c%d'",&STRINGLENGTH);
				printf("%d\n",STRINGLENGTH);	
				newattrentry[i].attrlength=STRINGLENGTH*sizeof(char);  //orizoume katallhla to relwidth;
				newattrentry[i].attrtype='c';
			}
			
			offsethelp=offsethelp+newattrentry[i].attrlength;

			//sscanf(argv[j+1],"'c%c'",&newattrentry[i].attrtype);			
			//newattrentry[i].attrtype=*(argv[j+1]);
			
			newattrentry[i].indexed=FALSE;
			newattrentry[i].indexno=0;
			j+=2;
		}
		
		for(i=0;i<((argc/2)-1);i++)
			if(HF_InsertRec(attrfd,(char*)(&newattrentry[i]),sizeof(attrDesc))<0){
				HF_PrintError("Problem with inserting record for ATTRCAT");
				exit(4);
			}
		

		printf("relname is %s\n",argv[1]);
		//afou exei ginei h enhmerosh ton ATTRCAT kai RELCAT dhmiourgoume to RELNAME
		if(HF_CreateFile(argv[1])<0){
			HF_PrintError("Problem with HF_CreateFile of RELANAME");
			exit(1);
		}

		 
	}
	else
		printf("Relationship already exists\n");
	
	
	return 0;
}

int UT_buildindex(int argc,char* argv[]){
	printArgs(argc,argv);
	int scanDescattr,scanDescrel;
	int check;
	attrDesc recattr;
	relDesc recrel;
	int found;
	

	//psaxnoume na broume an to ATTRNAME tou RELNAME pou mas dothike san orisma einai INDEXED
	//gia na to broume auto anoigoume skanarisma sto ATTRCAT me to krithrio to dosmeno RELNAME
	//kai sthn eggrafh pou anaktate(kai plhrh ta dosmena krithria) psaxnoume na broume an to ATTRNAME ths
	//einai idio me to dosmeno kai an to INDEXED ths einai "0",giati mono etsi mporei na ftiaxtei NEO eurethrio
	//me bash to sygkekrimeno pedio
	if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
		HF_PrintError("Problem while opening file scan");
	
	printf("%s\n",argv[1]);	

	check=HF_FindNextRec(scanDescattr,(char*)&recattr);
	while(check!=HFE_EOF){
		//anti na broume an to ATTRNAME pou mas dinete einai INDEXED, arkei na broume an to ATTRNAME pou 
		//mas dinete DEN einai INDEXED.
		if(strcmp(recattr.attrname,argv[2])==0 && recattr.indexed==0){
			recattr.indexed=1;	
			if(HF_DeleteRec(attrfd,check,sizeof(attrDesc))<0)
				HF_PrintError("Error while deleting entry in UT_CreateIndex");
			found=1;
			break;
		}
		
		check=HF_FindNextRec(scanDescattr,(char*)&recattr);
	}
	
	if(HF_CloseFileScan(scanDescattr)<0)
		HF_PrintError("Error while closing scan");	

	if(found==1){
		//anoigoume deutero skanarisma sto RELCAT gia na broume thn sxetikh eggrafh kai na thn enhmerosoume
		//ta pedia RELNAME,RELWIDTH kai ATTRCNT tou RELCAT opos kai ta RELNAME,ATTRNAME,OFFSET,ATTRLENGTH kai ATTRTYPE
		//tou ATTRCAT paramenoun idia...

		if((scanDescrel=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
			HF_PrintError("Problem while opening file scan");

		check=HF_FindNextRec(scanDescrel,(char*)&recrel);

		while(check!=HFE_EOF){

			if(check>=0){
				if(HF_DeleteRec(relfd,check,sizeof(relDesc))<0)
					HF_PrintError("Error while deleting entry in UT_CreateIndex");
				break;
			}
			check=HF_FindNextRec(scanDescrel,(char*)&recrel);
		}
		
		if(HF_CloseFileScan(scanDescrel)<0)
			HF_PrintError("Error while closing scan");
	
		//enhmerosh tou auksontos arithmou tou neodhmiourgithentos eurethriou 
		recattr.indexno=(recrel.indexcnt)+1;
		
		//dhmiourgoume to eurethrio
		if(AM_CreateIndex(argv[1],recrel.indexcnt,recattr.attrtype,recattr.attrlength)<0){
			AM_PrintError("Error while creating new index");
		}

		recrel.indexcnt++;

		//egrafh ton enhmeromenon eggrafon sto arxeio
		if(HF_InsertRec(relfd,(char*)(&recrel),sizeof(relDesc))<0){
			HF_PrintError("Problem with inserting record for RELCAT");
			exit(3);
		}
        
		if(HF_InsertRec(attrfd,(char*)(&recattr),sizeof(attrDesc))<0){
			HF_PrintError("Problem with inserting record for ATTRCAT");
			exit(3);
		}

	}
	else
		printf("Index for the specified field already exists... \n");

	return 0;
}

int UT_destroy(int argc,char* argv[]){
	printArgs(argc,argv);

	int scanDescattr,scanDescrel;
	int check;
	attrDesc recattr;
	relDesc recrel;
	int i;
	
	//SKANARISMA TOU RELCAT	
	if((scanDescrel=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
			HF_PrintError("Problem while opening file scan");

	check=HF_FindNextRec(scanDescrel,(char*)&recrel);

	while(check!=HFE_EOF){

		if(check>=0){
			for(i=0;i<recrel.indexcnt;i++)
				if(AM_DestroyIndex(recrel.relname,i)<0)
					AM_PrintError("Error while deleting index");

			if(HF_DeleteRec(relfd,check,sizeof(relDesc))<0)
				HF_PrintError("Error while deleting entry in UT_CreateIndex");
			break;	
		}
		check=HF_FindNextRec(scanDescrel,(char*)&recrel);
	}

	if(HF_CloseFileScan(scanDescrel)<0)
		HF_PrintError("Error while closing scan");

	//SKANARISMA TOU ATTRCAT
	if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
		HF_PrintError("Problem while opening file scan");
	
	check=HF_FindNextRec(scanDescattr,(char*)&recattr);
	
	while(check!=HFE_EOF){

		if(check>=0)
			if(HF_DeleteRec(attrfd,check,sizeof(attrDesc))<0)
				HF_PrintError("Error while deleting entry in UT_CreateIndex");

		check=HF_FindNextRec(scanDescattr,(char*)&recattr);
	}

	if(HF_CloseFileScan(scanDescrel)<0)
		HF_PrintError("Error while closing scan");

	if(HF_DestroyFile(recrel.relname)<0)
		HF_PrintError("Error while deleting file with HF_DestroyFile");
	
	return 0;
}

int UT_quit(void){
	printf("QUIT\n");

	if(HF_CloseFile(relfd)<0){
		HF_PrintError("Problem while closing RELCAT");
		exit(8);
	}
	
	if(HF_CloseFile(attrfd)<0){
		HF_PrintError("Problem while closing ATTRCAT");
		exit(8);
	}

	if(HF_CloseFile(viewfd)<0){
		HF_PrintError("Problem while closing VIEWCAT");
		exit(8);
	}

	if(HF_CloseFile(viewattrfd)<0){
		HF_PrintError("Problem while closing VIEWATTRCAT");
		exit(8);
	}	

	exit(0);
	return 0;
}


int DM_select(int argc, char* argv[]){
	printArgs(argc,argv);
	
	relDesc recrel;
	attrDesc recattr;
	int HFScanDesc;    //anagnristiko saroshs tou relname
	int check;
	int recId;
	int scanDescattr,scanDescrel,scanDescindex;
	int indexfd;       //anagnoristiko arxeiou eurethriou (open file)
	int rel1fd;         //anagnoristiko arxeiou relname (open file)
	int tempfd;        //anagnoristiko prosorinou arxeiou eisagoghs ton proballomenon apotelesmaton
	char *record;
	char *projectedrecord;// eggrafh pou periexei mono ta insteresting fields
	char** utcargs;    //pinakas mesa ston opoion tha ftiaksoume ta orismata ths UT_Create
						//kai tha ton perasoume san deutero orisma se auth

	char* utdargs[3];
	char buffer[]="TEMP";
	char* onomasxeshs;
	NOI *INF;
	int recsize=0; //megethos kathe probllomenhs eggrafhs
	int i,j,k;
	int found;
	int utcargc;   //to argc ths UT_Create
	int output_file_exists=0;
	int start,finish;
	char telestes[]="><=";
	int telestis;
	int noo;
	int newoffset=0;   //offset in TEMP
	int numberint;
	float numberfloat;
	//periptosh opou h select kalestike me sxesh.pedio
	if(argc<6){
		printf("TOO FEW ARGUMENTS\n");
		return 1;
	}
	
	//an yparxei <WHERE condition> tha prepei na elegksoume an to onoma_sxeshs_epiloghs
	//einai idio me to onoma ths proballomenhs sxeshs
	if(strpbrk(argv[argc-2],telestes)!=NULL && strcmp(argv[2],argv[argc-4])){
		printf("SELECTION'S RELNAME DIFFERS FROM PROJETED RELATIONS' NAMES\n");
		return 1;
	}
	
	if(strpbrk(argv[argc-2],telestes)!=NULL)
		finish=argc-5;
	else
		finish=argc-1;

	//SKANARISMA TOU RELCAT
	//elegxos an to relname yparxei sto RELCAT
	if(atoi(argv[1])==0){   //an h anaparastash se int tou argv[1] einai 0 tote mas exei dothei output file	
		onomasxeshs=argv[3];
		utcargc=2*atoi(argv[2])+2;
		noo=atoi(argv[2]);
		output_file_exists=1;
		start=4;
	}
	else{
		onomasxeshs=argv[2];
		utcargc=2*atoi(argv[1])+2;
		noo=atoi(argv[1]);
		start=3;
	}

	if((scanDescrel=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,onomasxeshs))<0){
		HF_PrintError("Problem while opening file scan");
		return -1;
	}

	check=HF_FindNextRec(scanDescrel,(char*)&recrel);
	
	while(check!=HFE_EOF){
		if(check>=0)
			break;
		
		check=HF_FindNextRec(scanDescrel,(char*)&recrel);		
	
	}
	
	if(HF_CloseFileScan(scanDescrel)<0)
		HF_PrintError("SELECT : Error while closing scan");

	record=malloc(recrel.relwidth*sizeof(char));
	//efoson o parser katalabenei pote to erotima einai typou epiloghs (kai kalei thn DM_Select()) auto shmenei oti
	//san orismata sthn synarthsh DM_Select() mpenoune MONO idia onomata proballomenon sxeseon
	//kai emeis apla prepei na elegxoume an ta pedia ton proballomenon sxeseon einai ontos pedia 
	//ton sxeseon meso mias saroshs sto ATTRCAT
	
	//me thn "for" anatrexoume OLA ta pedia ton proballomenon sxeseon
	//kai gia kathe ena apo auta elegxoume an yparxei sto ATTRCAT me skanarisma autou
	
	
	//prepei na dhmiourgisoume ton pinaka pou tha perasoume san
	//orisma sthn UT_Create().. dld ton pinaka utcargs[][]
	
	//desmeush xorou gia ton pinaka utcargs
	utcargs=malloc(utcargc*sizeof(char*));
	
	for(i=0;i<utcargc;i++)
		utcargs[i]=malloc(256*sizeof(char));
		
	
	sprintf(utcargs[0],"%s","create");
	if(output_file_exists==1)
		sprintf(utcargs[1],"%s",argv[1]);
	else
		sprintf(utcargs[1],"%s",buffer);

	//desmeush xorou gia ton pinaka ton INF
	
	INF=malloc(noo*sizeof(NOI));

	//desmeush xorou gia ton pinaka utdargs

	for(i=0;i<3;i++)
		utdargs[i]=malloc(256*sizeof(char));

	sprintf(utdargs[0],"%s","destroy");
	if(output_file_exists==1)
		sprintf(utcargs[1],"%s",argv[1]);
	else
		sprintf(utcargs[1],"%s",buffer);	
	
	j=2;
	k=0;
	
	for(i=start;i<=finish;i=i+2){
		
		found=0;
	
		//SKANARISMA TOU ATTRCAT
		if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,onomasxeshs))<0)
			HF_PrintError("Problem while opening file scan");
		
		check=HF_FindNextRec(scanDescattr,(char*)&recattr);
			
		while(check!=HFE_EOF){
			//brikame to probalomeno pedio san pedio tou relname
			if(strcmp(recattr.attrname,argv[i])==0){
				recsize+=recattr.attrlength;
				found=1;
				
				///parallhla kataskeuazoume kai ton pinaka orismaton ths UT_Create - enarksh kataskeuhs
				sprintf(utcargs[j],"%s",recattr.attrname);
				j++;
				if(recattr.attrtype=='c')
					sprintf(utcargs[j],"'%c%d'",recattr.attrtype,recattr.attrlength);
				else
					sprintf(utcargs[j],"'%c'",recattr.attrtype);
				j++;	
				//telos kataskeuhs
				
				INF[k].offset=recattr.offset;
				INF[k].type=recattr.attrtype;
				if(recattr.attrtype=='c')
					INF[k].length=recattr.attrlength;
					
				k++;
				
				break;
			}
			
			check=HF_FindNextRec(scanDescattr,(char*)&recattr);
		}
		
		if(HF_CloseFileScan(scanDescattr)<0)
			HF_PrintError("DELETE : Error while closing scan");

		
		if(found==0){
			printf("REQUESTED FIELD FOR PROJECTION NOT FOUND IN RELCAT... RETYPE SELECTION USING DIFFERENT ARGUMENTS\n");
			return 1;
		}
		
	}
	//desmeush xorou gia to projected record
	//projectedrecord=malloc(recsize*sizeof(char));
	
	//////TELOS ELEGXOU ORISMATON
	
	//anoigma tou relname
	if((rel1fd=HF_OpenFile(onomasxeshs))<0)
		HF_PrintError("SELECT");
		
	//dhmiourgia prosorinou arxeiou eisagoghs ton probalomenon apotelesmaton

	//periptosh opou den mas exei dothei to onoma kapoiou arxeiou gia na baloume
	//ta apotelesmata ths select
	if(output_file_exists==1){
		//dhmiourgia tou arxeiou
		if(UT_create(utcargc,utcargs)<0){
			printf("Error while creating temporary file\n");
			return -1;
		}
	
		//anoigma tou arxeiou
		if((tempfd=HF_OpenFile(argv[1]))<0)
			HF_PrintError("SELECT");

	
	}

					
	//periptosh opou mas exei dothei to onoma enos arxeiou gia na baloume ta apotelesmata
	else{
		//dhmiourgia tou arxeiou
		if(UT_create(utcargc,utcargs)<0){
			printf("Error while creating output file %s\n",argv[1]);
			return -1;
		}
		
		//anoigma tou arxeiou
		if((tempfd=HF_OpenFile(utcargs[1]))<0)
			HF_PrintError("SELECT");

	}
	
	
	///////ENARKSH SELECT
	
	//PROTH PERIPTOSH : EXOUME <WHERE>
	
	//elegxoume thn thesh tou pinaka argv[] opou apothikeuete o telesths sygkrishs
	//an o deikths se authn thn thesh einai diaforetikos tou NULL tote exoume WHERE
	//diaforetika den exoume
	
	if(strpbrk(argv[argc-2],telestes)!=NULL){
	
		//pame na doume an to pedio ths synthikis einai INDEXED
		//SKANARISMA TOU ATTRCAT
		if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,onomasxeshs))<0)
			HF_PrintError("Problem while opening file scan");
		
		check=HF_FindNextRec(scanDescattr,(char*)&recattr);
			
		while(check!=HFE_EOF){
			if(strcmp(recattr.attrname,argv[argc-3])==0){
				break;
				//bgenoume apo to while exontas kratisei sto recattr ta stoixeia tou 
				//pediou pou mas endiaferei, ta stoixeia dld tou pediou sygkrishs
			}
			
			check=HF_FindNextRec(scanDescattr,(char*)&recattr);
		}
		
		if(HF_CloseFileScan(scanDescattr)<0)
			HF_PrintError("SELECT");

		//PROTH PERIPTOSH proths perisptoshs : to pedio tou WHERE einai eurethriasmeno
		if(recattr.indexed==TRUE){
			if((indexfd=AM_OpenIndex(onomasxeshs,recattr.indexno))<0)
				AM_PrintError("SELECT");
			
			telestis=compare(argv[argc-2]);
			if((scanDescindex=AM_OpenIndexScan(indexfd,recattr.attrtype,recattr.attrlength,telestis,argv[argc-1]))<0)
				AM_PrintError("SELECT");

			recId=AM_FindNextEntry(scanDescindex);
			
			while(recId!=AME_EOF){
				if(recId>=0){
					if(HF_GetThisRec(rel1fd,recId,record,recrel.relwidth)<0)
						HF_PrintError("SELECT");
						
					projectedrecord=interestingfields(record,INF,noo,recsize);
					
					if(HF_InsertRec(tempfd,projectedrecord,recsize)<0)
						HF_PrintError("SELECT");
				}
				recId=AM_FindNextEntry(scanDescindex);
			}
			
			if(AM_CloseIndexScan(scanDescindex)<0)
				AM_PrintError("SELECT");
		}
	
		//DEUTERH PERIPTOSH proths periptoshs : to pedio tou WHERE DEN einai eurethriasmeno
		else{
			telestis=compare(argv[argc-2]);
			if((HFScanDesc=HF_OpenFileScan(rel1fd,recrel.relwidth,recattr.attrtype,recattr.attrlength,recattr.offset,telestis,argv[argc-1]))<0)
				HF_PrintError("SELECT");
				
			recId=HF_FindNextRec(HFScanDesc,record);
			
			while(recId!=HFE_EOF){
				if(recId>=0){
					projectedrecord=interestingfields(record,INF,noo,recsize);
					
					if(HF_InsertRec(tempfd,projectedrecord,recsize)<0)
						HF_PrintError("SELECT");
				}
				recId=HF_FindNextRec(HFScanDesc,record);
			}
			
			if(HF_CloseFileScan(HFScanDesc)<0)
				HF_PrintError("SELECT");
		}
	}
	
	
	//DEUTERH PERIPTOSH : DEN EXOUME <WHERE>
	else{
		if((HFScanDesc=HF_OpenFileScan(rel1fd,recrel.relwidth,0,0,0,0,NULL))<0)
			HF_PrintError("SELECT");
				
		recId=HF_FindNextRec(HFScanDesc,record);
			
		while(recId!=HFE_EOF){
			if(recId>=0){
				projectedrecord=interestingfields(record,INF,noo,recsize);
	
				if(HF_InsertRec(tempfd,projectedrecord,recsize)<0){
					HF_PrintError("SELECT");
					return -1;
				}
			}
			
			recId=HF_FindNextRec(HFScanDesc,record);
		}
		
		if(HF_CloseFileScan(HFScanDesc)<0){
			HF_PrintError("SELECT");
			return -1;
		}
	}
	
	
	//an mas exei dothei onoma gia output file tote den ektyponoume tpt
	//an den mas exei dothei onoma prepei na ektyposoume tis eggrafes pou
	//exoume balei sto "temp"
	
	if(output_file_exists==0){
		//skanarisma tou "temp" xoris synthiki gia ektyposh olon ton eggrafon
		if((HFScanDesc=HF_OpenFileScan(tempfd,recsize,0,0,0,0,NULL))<0){
			HF_PrintError("SELECT");
			return -1;
		}

		recId=HF_FindNextRec(HFScanDesc,record);
		
		while(recId!=HFE_EOF){
			newoffset=0;
			for(i=0;i<noo;i++){
				if(INF[i].type=='c'){
					printf("%s",record);
					newoffset+=INF[i].length;
				}
				
				if(INF[i].type=='i'){
					memcpy(&numberint,record+newoffset,sizeof(int));
					printf("%d",numberint);
					newoffset+=sizeof(int);
				}
				if(INF[i].type=='f'){
					memcpy(&numberfloat,record+newoffset,sizeof(float));
					printf("%f",record);
					newoffset+=sizeof(float);
				}
				
				printf("    ");
			}
			recId=HF_FindNextRec(HFScanDesc,record);
			printf("\n");
		}

		if(HF_CloseFileScan(HFScanDesc)<0){
			HF_PrintError("SELECT");
			return -1;
		}
	}	                                                                            
	
	
	//kleisimo tou relname
	if(HF_CloseFile(rel1fd)<0){
		HF_PrintError("SELECT");
		return -1;
	}
		
	//kleisimo kai katastrofh ton prosorinon arxeion
	if(output_file_exists==0){
		//kleisimo
		if(HF_CloseFile(tempfd)<0){
			HF_PrintError("SELECT");
			return -1;
		}
		//katastrofh
		if(UT_destroy(2,utdargs)<0){
			printf("Error while destroying temporary file in select\n");
			return -1;
		}
	}
	
	else{
		//kleisimo
		if(HF_CloseFile(tempfd)<0){
			HF_PrintError("SELECT");
			return -1;
		}
		
		//an mas exei dothei output file... apla den to katastrefoume 
	}

	
	return 0;
}

int DM_join(int argc, char* argv[]){
	printArgs(argc,argv);
	return 0;
}

int DM_delete(int argc, char* argv[]){
	printArgs(argc,argv);
	
	//argv[1]=relaname
	//argv[2]=attrname
	//argv[3]=operator
	//argv[4]=value
	
	relDesc recrel;
	attrDesc recattr;
	int HFScanDesc;    //anagnristiko saroshs tou relname
	int check;
	int recId;
	int scanDescattr,scanDescrel,scanDescindex,scanDescindex2;
	int indexfd,indexfdtemp;       //anagnoristikou arxeiou eurethriou (open file)
	int rel1fd;         //anagnoristiko arxeiou relname (open file)
	int found=0;
	int telestis;
	char *record;
	
	//sthn delete tha exoume 2 periptoseis : eite tha mas dinete mia synthiki kai tha prepei na diagrafoun 
	//oi eggrafes pou thn ikanopoioun eite den tha mas dinete kamia synthiki kai tha prepei na diagrapsoume
	//OLES tis eggrafes apo to RELNAME.
	
	//SKANARISMA TOU RELCAT
	if((scanDescrel=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0){
		HF_PrintError("Problem while opening file scan");
		return -1;
	}

	check=HF_FindNextRec(scanDescrel,(char*)&recrel);
	
	while(check!=HFE_EOF){
		if(check>=0){
			found=1;
			break;
		}

		check=HF_FindNextRec(scanDescrel,(char*)&recrel);		
	
	}
	
	if(HF_CloseFileScan(scanDescrel)<0){
		HF_PrintError("DELETE : Error while closing scan");
		return -1;
	}
		
	//den xreiazete na yparxei elegxos gia ton telesth mias kai epidi exoume b+ dentro oloi
	//oi telestes yposthrizonte kata thn sarosh. apla tha prepei na elegksoume an ta argv[2]
	//kai argv[3] einai NULL gia na doume an exoume h oxi synthiki
	
	//ksekiname me thn periptosh opou den exoume synthiki diagrafhs
	//to recrel einai diaforo tou NULL dld exei brethei eggrafh gia to relname sto RELCAT
	//eno ta argv[2] kai argv[3] einai NULL mias kai den exei dothei synthiki diagrafhs
	
	//desmeush xorou gai to record
	record=malloc(recrel.relwidth*sizeof(char));

	telestis=compare(argv[3]);

	if(found==1 && argv[2]==NULL && argv[3]==NULL){
	
		if((rel1fd=HF_OpenFile(argv[1]))<0){
			HF_PrintError("Error while trying to open file");
			return -1;
		}
			
		if((HFScanDesc=HF_OpenFileScan(rel1fd,recrel.relwidth,0,0,0,0,NULL))<0){
			HF_PrintError("Problem while opening file scan");
			return -1;
		}
		
		check=HF_FindNextRec(HFScanDesc,record);
			
		while(check!=HFE_EOF){
			if(check>=0)
				if(HF_DeleteRec(rel1fd,check,recrel.relwidth)<0){
					HF_PrintError("Error while deleting record");
					return -1;
				}
			check=HF_FindNextRec(HFScanDesc,record);
		}
		
		if(HF_CloseFileScan(HFScanDesc)<0){
			HF_PrintError("DELETE : Error while closing scan");
			return -1;
		}

		
		
		//bgenontas apo to while exoun diagrafei OLES oi eggrafes apo to relname kai to mono pou mas menei
		//einai na doume an kapoio apo ta pedia ton eggrafon htan eurethriasmeno opote prepei na diagrapsume 
		//tis eggrafes kai apo auto to eurethrio
		
		if(recrel.indexcnt=0){
			//SKANARISMA TOU ATTRCAT
			if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0){
				HF_PrintError("Problem while opening file scan");
				return -1;
			}
			
			check=HF_FindNextRec(scanDescattr,(char*)&recattr);
			
			while(check!=HFE_EOF){
				if(check>=0)
					if(recattr.indexed==TRUE){
					
						if((indexfd=AM_OpenIndex(argv[1],recattr.indexno))<0){
							AM_PrintError("DELETE : Error while opening scan");
							return -1;
						}
						
						if((scanDescindex=AM_OpenIndexScan(indexfd,0,0,0,NULL))<0){
							AM_PrintError("DELETE : Error while opening scan");
							return -1;
						}
							
						recId=AM_FindNextEntry(scanDescindex);
						
						while(recId!=AME_EOF){
							if(recId>=0){
								//xrhsimopoioume thn HF_GetThisRec gia na ftiaksoume to orisma RECORD
								//the AM_DeleteEntry
								if(HF_GetThisRec(indexfd,recId,record,recattr.attrlength)<0){
									HF_PrintError("DELETE : Error while fetching specified record");
									return -1;
								}
								
								if(AM_DeleteEntry(indexfd,recattr.attrtype,recattr.attrlength,record,recId)<0){
									AM_PrintError("DELETE");
									return -1;
								}
							}
							recId=AM_FindNextEntry(scanDescindex);
						}
							
						if(AM_CloseIndexScan(scanDescindex)<0){
							AM_PrintError("DELETE : Error while closing scan");
							return -1;
						}
						
						if(AM_CloseIndex(indexfd)<0){
							AM_PrintError("DELETE : Error while closing index");
							return -1;
						}
					}
					
				check=HF_FindNextRec(HFScanDesc,record);
			} //kleisimo tou while(check!=HFFE_EOF)
			
			if(HF_CloseFileScan(scanDescattr)<0){
				HF_PrintError("DELETE : Error while closing scan");
				return -1;
			}

		}//kleisimo tou if(recrel.indexcnt=0)
	}//kleisimo tou if(recrel!=NULL && argv[2]==NULL && argv[3]==NULL)
	
	
	//DEUTERH PERIPTOSH : to relname exei brethei sto RELCAT mono pou edo prepei na diagrpasoume sygkekrimenes
	//eggrafes analoga me thn dothisa synthiki
	
	if(found==1 && argv[2]!=NULL && argv[3]!=NULL){
		
		//epidi den mas dinete o TYPOS tou pediou pano sto opoio ginete h sarosh tha prepei na
		//anatreksoume sto ATTRCAT kai na broume to attrname tou relname kai to antistoixo TYPO tou
		//auto to kanoume gt h HF_OpenFileScan pernei san orisma to attrtype.. to opoio dystyxos den mas dinete :(
		//SKANARISMA TOU ATTRCAT
		if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0){
			HF_PrintError("Problem while opening file scan");
			return -1;
		}
		
		check=HF_FindNextRec(scanDescattr,(char*)&recattr);
		
		while(check!=HFE_EOF){
			if(strcmp(recattr.attrname,argv[2])==0)
				break;
			
			else
				check=HF_FindNextRec(scanDescattr,(char*)&recattr);
		}

		if(HF_CloseFileScan(scanDescattr)<0){
			HF_PrintError("DELETE : Error while closing scan");
			return -1;
		}
		
		//bgenontas apo to while exoume brei ton typo tou pediou pano sto opoio efarmozete
		//o telesths sygkrishs
		
		//anoigma tou arxeiou sorou apo opou diagrafoume eggrafes
		if((rel1fd=HF_OpenFile(argv[1]))<0){
			HF_PrintError("Error while trying to open file");
			return -1;
		}
		
		
		//ekmetaleush tou eurethriou pou tyxon yparxei sto pedio sygkrishs
		if(recattr.indexed==TRUE){
			if((indexfd=AM_OpenIndex(argv[1],recattr.indexno))<0){
				AM_PrintError("DELETE : Error while opening scan");
				return -1;
			}
			
			if((scanDescindex=AM_OpenIndexScan(indexfd,recattr.attrtype,recattr.attrlength,telestis,argv[4]))<0){
				AM_PrintError("DELETE : Error while opening scan");
				return -1;
			}
			
			recId=AM_FindNextEntry(scanDescindex);
			
			while(recId!=AME_EOF){
				//diagrafh tou record apo ton soro
				if(HF_DeleteRec(rel1fd,recId,recrel.relwidth)<0){
					HF_PrintError("Error while deleting record");
					return -1;
				}
				
				//oi 2 epomenes entoles pragmatopoioun thn diagrafh ths eggrafhs apo to eurethrio
				if(HF_GetThisRec(indexfd,recId,record,recattr.attrlength)<0){
					HF_PrintError("DELETE : Error while fetching specified record");
					return -1;
				}
								
				if(AM_DeleteEntry(indexfd,recattr.attrtype,recattr.attrlength,record,recId)<0){
					AM_PrintError("DELETE");
					return -1;
				}
				
				//ektos omos apo thn diagrafh ths eggrafhs apo to eurethriou tou pediou ths synthikis
				//prepei na diagrapsoume kai tis antistoixes eggrafes apo OLA ta eurethria ton eurethriasmenon
				//pedion tou relname.... opote tha ksekinisoume ena skanarisma sto ATTRCAT gia na doume poia
				//pedia ektos apo auto ths synthikis einai eurethrismena kai tha sbinoume thn antistoixh eggrafh
				//opos kai stis proigoumenes entoles
				//SKANARISMA TOU ATTRCAT
				if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0){
					HF_PrintError("Problem while opening file scan");
					return -1;
				}
				
				check=HF_FindNextRec(scanDescattr,(char*)&recattr);
				
				while(check!=HFE_EOF){
					if(check>=0)
						if(strcmp(recattr.attrname,argv[2])!=0 && recattr.indexed==TRUE){
							
							if((indexfdtemp=AM_OpenIndex(argv[1],recattr.indexno))<0){
								AM_PrintError("DELETE : Error while opening scan");
								return -1;
							}
							
							if(HF_GetThisRec(indexfdtemp,recId,record,recattr.attrlength)<0){
								HF_PrintError("DELETE : Error while fetching specified record");
								return -1;
							}
							
							if(AM_DeleteEntry(indexfdtemp,recattr.attrtype,recattr.attrlength,record,recId)<0){
								AM_PrintError("DELETE");
								return -1;
							}
							
							if(AM_CloseIndex(indexfdtemp)<0){
								AM_PrintError("DELETE : Error while closing index");
								return -1;
							}
							
						}
				

					check=HF_FindNextRec(scanDescattr,(char*)&recattr);

				}
				//bgenontas apo to while exoume diagrapsei OLES tis eggrafes me anagnoristiko eggrafhs recId
				//apo OLA ta eurethria tou relname

				recId=AM_FindNextEntry(scanDescindex);
			}//kleisimo tou while(recId!=AME_EOF)
		
			if(AM_CloseIndexScan(scanDescindex)<0){
				AM_PrintError("DELETE : Error while closing scan");
				return -1;
			}
						
			if(AM_CloseIndex(indexfd)<0){
				AM_PrintError("DELETE : Error while closing index");
				return -1;
			}
			
		} //kleisimo tou if(recattr.indexed==TRUE)
		
		//enarksh skanarismatos me thn dosmenh synthiki
		//den yparxei eurethrio pano sto dosmeno pedio ths synthikis
		else{
			
			if((HFScanDesc=HF_OpenFileScan(rel1fd,recrel.relwidth,recattr.attrtype,recattr.attrlength,recattr.offset,telestis,argv[4]))<0){
				HF_PrintError("Problem while opening file scan");
				return -1;
			}
			
			recId=HF_FindNextRec(HFScanDesc,record);
		
			while(recId!=HFE_EOF){
				if(recId>=0){
					//diagrafh ths eggrafhs apo ton SORO
					if(HF_DeleteRec(rel1fd,recId,recrel.relwidth)<0){
						HF_PrintError("Error while deleting record");
						return -1;
					}
					
					//SKANARISMA TOU ATTRCAT
					//tha psaksoume na broume pou yparxei to sygkekrimeno recId pou mas edose san return value h
					// HF_FindNextEntry mesa se eurethria ALLON pedion
					
					if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0){
						HF_PrintError("Problem while opening file scan");
						return -1;
					}
					
					check=HF_FindNextRec(scanDescattr,(char*)&recattr);
				
					while(check!=HFE_EOF){
						if(strcmp(recattr.attrname,argv[2])!=0 && recattr.indexed==TRUE){
							if((indexfd=AM_OpenIndex(argv[1],recattr.indexno))<0){
								AM_PrintError("DELETE : Error while opening scan");
								return -1;
							}
							
							if(HF_GetThisRec(indexfd,recId,record,recattr.attrlength)<0){
								HF_PrintError("DELETE : Error while fetching specified record");	
								return -1;
							}
							
							if(AM_DeleteEntry(indexfd,recattr.attrtype,recattr.attrlength,record,recId)<0){
								AM_PrintError("DELETE : Error while deleting entry from index");
								return -1;
							}
							
							if(AM_CloseIndex(indexfd)<0){
								AM_PrintError("DELETE : Error while closing index");
								return -1;
							}
							
						}
				
						check=HF_FindNextRec(scanDescattr,(char*)&recattr);

					}
				}
				recId=HF_FindNextRec(HFScanDesc,record);
		
			} //kleisimo tou while(recId!=HFE_EOF)
		
						
			if(HF_CloseFileScan(HFScanDesc)<0){
				HF_PrintError("DELETE : Error while closing scan");
				return -1;
			}
			//diagrapsame OLES tis eggrafes apo ton soro

		
			if(HF_CloseFile(rel1fd)<0){
				HF_PrintError("DELETE : Error while closing file");
				return -1;
			}
		}
	}
	
	else
		printf("wrong use of command delete.. read instructions and try again...\n");
	
	
	
	
	return 0;
}

int DM_subtract(int argc,char* argv[]){
	printArgs(argc,argv);

	int scanDescattr1,scanDescattr2; //anagnoristika saroseon gia ATTRCAT
	int scanDescrel1,scanDescrel2,HFScanDesc; //anagnoristika saroseon gia RELCAT
	int HFScanDesc1,HFScanDesc2; //anagnoristika saroseon gia relname1 kai relname 2
	int check,check1,check2;
	int rel1fd,rel2fd; //anagnoristika arxeion gia ta relname1 kai relname2
	int indexfd;
	int found=0;
	int recId;
	attrDesc recattr1;
	attrDesc recattr2;
	relDesc recrel1;
	relDesc recrel2;
	char *record1;
	char *record2;

	

	//to relname1 DEN mporei na einai idio me to relname2
	if(strcmp(argv[1],argv[2])==0){
		printf("ERROR : EQUALITY OF RELNAMES");
		return 1;
	}


	//SKANARISMA TOU RELCAT	GIA TO RELNAME 1
	if((scanDescrel1=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0){
			HF_PrintError("Problem while opening file scan");
			return -1;
	}

	check=HF_FindNextRec(scanDescrel1,(char*)&recrel1);

	while(check!=HFE_EOF){
		if(check>=0)
			break;
		check=HF_FindNextRec(scanDescrel1,(char*)&recrel1);
	}


	//SKANARISMA TOU RELCAT	GIA TO RELNAME 2
	if((scanDescrel2=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,argv[2]))<0){
			HF_PrintError("Problem while opening file scan");
			return -1;
	}

	check=HF_FindNextRec(scanDescrel2,(char*)&recrel2);

	while(check!=HFE_EOF){
		if(check>=0)
			break;
		check=HF_FindNextRec(scanDescrel2,(char*)&recrel2);
	}
	
	if(HF_CloseFileScan(scanDescrel1)<0){
		HF_PrintError("Error while closing scan");
		return -1;
	}

	if(HF_CloseFileScan(scanDescrel2)<0){
		HF_PrintError("Error while closing scan");
		return -1;
	}

	//elegxos gia diaforetiko arithmo orismaton
	if(recrel1.attrcnt!=recrel2.attrcnt){
		printf("ERROR IN ADD : DIFFERENT NUMBER OF FIELDS");
		return 1;
	}

	//desmeush pinakon gia thn syggrish ton eggrafon apo ta 2 arxeia
	record1=malloc(recrel1.relwidth*sizeof(char));
	record2=malloc(recrel1.relwidth*sizeof(char));
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	//SKANARISMA TOU ATTRCAT
	if((scanDescattr1=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0){
		HF_PrintError("Problem while opening file scan");
		return -1;
	}

	if((scanDescattr2=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[2]))<0){
		HF_PrintError("Problem while opening file scan");
		return -1;
	}
	
	check1=HF_FindNextRec(scanDescattr1,(char*)&recattr1);
	check2=HF_FindNextRec(scanDescattr2,(char*)&recattr2);
	
	while(check1!=HFE_EOF && check2!=HFE_EOF){
		//efoson h sygkrish ton pou pedion pou kanoume einai parallhlh eksasfalizoume kai thn sosth SEIRA ton pedion
		//elegxos gia to onoma tou pediou
		/*if(strcmp(recattr1.attrname,recattr2.attrname)){
			printf("ERROR IN ADD : INCOMPATIBLE NAME OF FIELDS");
			return 1;
		}*/
		
		//elegxos gia to mhkos (LENGTH) ton pedion
		if(recattr1.attrlength!=recattr2.attrlength){
			printf("ERROR IN ADD : INCOMPATIBLE LENGTH OF FIELDS");
			return 1;
		}
		
		//elegxos gia ton TYPO ton pedion
		if(strcmp(&recattr1.attrtype,&recattr2.attrtype)){
			printf("ERROR IN ADD : INCOMPATIBLE TYPE OF FIELDS");
			return 1;
		}		
				
		found++;
		
		/*if(recattr1.indexed==TRUE){
			mynoi[i].number_of_index=recattr1.indexno;
			mynoi[i].offset=recattr1.offset;
			mynoi[i].type=recattr1.attrtype;
			if(mynoi[i].type!='i' && mynoi[i].type!='f')
				mynoi[i].length=recattr1.attrlength;
		}
	
		else
			mynoi[i].number_of_index=0;*/

		
		check1=HF_FindNextRec(scanDescattr1,(char*)&recattr1);
		check2=HF_FindNextRec(scanDescattr2,(char*)&recattr2);	
	}
	

	if(HF_CloseFileScan(scanDescattr1)<0){
		HF_PrintError("Error while closing scan");
		return -1;
	}
	
	if(HF_CloseFileScan(scanDescattr2)<0){
		HF_PrintError("Error while closing scan");
		return -1;
	}



	if(found==recrel1.attrcnt){
		//anoigma ton relname1 kai relname2
		if((rel1fd=HF_OpenFile(argv[1]))<0){
			HF_PrintError("Error while attempting to open file");
			return -1;
		}
		
		if((rel2fd=HF_OpenFile(argv[2]))<0){
			HF_PrintError("Error while attempting to open file");
			return -1;
		}

		//anoigma skanarismatos xoris synthiki gia na paroume OLES tis eggrafes tou relname2
		if((HFScanDesc2=HF_OpenFileScan(rel2fd,recrel1.relwidth,0,0,0,0,NULL))<0){    //gia mikos eggrafhs vazoume ena apo relwidth
											      //eite tou relname1 eite tou relname2 afou einai isa
			HF_PrintError("Problem while opening file scan");
			return -1;
		}

		check2=HF_FindNextRec(HFScanDesc2,record2);

		while(check2!=HFE_EOF){
			//anoigma skanarismatos me synthiki gia na broume thn eggrafh record2 sto relname1 kai na thn diagrapsoume
			if((HFScanDesc1=HF_OpenFileScan(rel1fd,recrel1.relwidth,0,0,0,0,NULL))<0){
											      //gia mikos eggrafhs vazoume ena apo relwidth
											      //eite tou relname1 eite tou relname2 afou einai isa
				HF_PrintError("Problem while opening file scan");
				return -1;
			}

			check1=HF_FindNextRec(HFScanDesc1,record1);
			while(check1!=HFE_EOF){
				if(check1>=0 && strcmp(record1,record2)==0){
					if(HF_DeleteRec(rel1fd,check1,recrel1.relwidth)<0){
						HF_PrintError("Error while trying to delete record with HF");
						return -1;
					}
					break;
				}
			
			check1=HF_FindNextRec(HFScanDesc1,record1);			
			}

			if(HF_CloseFileScan(HFScanDesc1)<0){
				HF_PrintError("SUBSTRACT");
				return -1;					
			}
		}
		if(HF_CloseFileScan(HFScanDesc2)<0){
			HF_PrintError("SUBSTRACT");
			return -1;
		}

	}
	else
		printf("INVALID ARGUMENTS IN COMMAND LINE PROMPT...READ INSTRUCTIONS AND TRY AGAIN");

		
	//me authn thn substract diagrafoume eggrafes pou einai topothetimenes se soro
	//kai den ekmetaleiomaste tyxon eurethria gia ta pedia tou enos h tou allou relname
	//RPEPEI NA BROUME TROPO GIA NA TA EKMETALEUTOUME KAI NA GINETE PIO GRIGORA H DIAGRAFH

	return 0;
}

int DM_insert(int argc,char* argv[]){
	
	int scanDescattr,scanDescrel;
	int check,notfound;
	int temprelfd,indexfd;
	int found=0;
	int recId;
	int offset=0; //boithitikh metablith gia thn dhmiourgia ths eggrafhs
	int foundrelname=0;
	int *soa; //pinakas opou kratame thn seira diadoxhs ton orismaton (sequence of arguments)
	int numberint;
	float numberfloat;
	char *buffer;  //buffer mesa sto opoio bazoume thn nea eggrafh
	char value[256];  //buffer gia na pernoume thn eurethriasmenh timh tou pediou
	NOI *types;
	attrDesc recattr;
	relDesc recrel;
	int i,j;
	
	//printArgs(argc,argv);
	for(i=0;i<argc;i++)
		printf("argv[%d]=%s\n",i,argv[i]);

	//kata thn eisagogh ths eggrafhs tha prepei na diasfalisoume pos ta pedia auths einai idia me 
	//ta pedia tou RELNAME sto opoio tha eisaxthei...

	//SKANARISMA TOU RELCAT	
	//tha psaksoume na broume an h proth metablhth pou dothike san orisma (dld to ARGV[1])
	//yparxei ston pinaka RELCAT
	
	if((scanDescrel=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
			HF_PrintError("Problem while opening file scan");

	check=HF_FindNextRec(scanDescrel,(char*)&recrel);

	while(check!=HFE_EOF){

		if(check>=0){
			foundrelname=1;
			break;
		}
		check=HF_FindNextRec(scanDescrel,(char*)&recrel);
	}

	if(HF_CloseFileScan(scanDescrel)<0)
		HF_PrintError("Error while closing scan");

	//SKANARISMA TOU ATTRCAT
	if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
		HF_PrintError("Problem while opening file scan");
	
	check=HF_FindNextRec(scanDescattr,(char*)&recattr);

	//dsmeush xorou gia ton pinaka types
	types=malloc(recrel.attrcnt*sizeof(NOI));

	//desmeush xorou gia ton buffer opou tha ftiaksoume thn eggrafh
	buffer=malloc(recrel.relwidth*sizeof(char));
	
	//desmeush xorou gia ton pinaka sequence of arguments = soa
	soa=malloc(recrel.attrcnt*sizeof(int));
	j=0;
	while(check!=HFE_EOF){

		if(check>=0)
			for(i=2;i<argc;i=i+2)
				if(strcmp(recattr.attrname,argv[i])==0 && (recattr.attrlength>=sizeof(argv[1]))/*elegxos gia ton typo??*/){
					found++;
					types[j].type=recattr.attrtype;
					types[j].length=recattr.attrlength;					
					soa[j]=i;
					j++;
			}
					  			
		check=HF_FindNextRec(scanDescattr,(char*)&recattr);
	}	

	if(HF_CloseFileScan(scanDescattr)<0)
		HF_PrintError("Error while closing scan");	

	if(foundrelname==1 && found==recrel.attrcnt){     //to RELNAME brethike sto RELCAT kai to found(pou deixnei to POSES idies meta
							//blites me ta dosmena orismata brethikan sto ATTRCAT) einai iso me ton arithmo
							//ton pedion tou RELNAME pou tha mpei to record
		//kataskeuh ths eggrafhs
		//j=3;
		for(i=0;i<recrel.attrcnt;i++){
			if(types[i].type=='i'){
				numberint=atoi(argv[soa[i]+1]);
				memcpy(buffer+offset,(char*)&numberint,sizeof(int));
			}

			if(types[i].type=='f'){
				numberfloat=atof(argv[soa[i]+1]);
				memcpy(buffer+offset,(char*)&numberfloat,sizeof(float));
			}

			if(types[i].type=='c')
				memcpy(buffer+offset,argv[soa[i]+1],types[i].length);	
			
			offset+=types[i].length;
		}
		
		if((temprelfd=HF_OpenFile(argv[1]))<0)
			HF_PrintError("Error while opening file for inserting new record");
		
		if((recId=HF_InsertRec(temprelfd,buffer,recrel.relwidth))<0)
			HF_PrintError("Error while insering new single record");
	
		if(HF_CloseFile(temprelfd)<0)
			HF_PrintError("Error while attempting to close file");

		//sthn synexeia tha prepei na elegksoume an kapoio apo ta pedia ths neoeisaxthisas eggrafhs einai euretiriasmeno
		//an einai tha prepei na broume to eurethrio,na to anoiksoume, na eisagoume thn eggrafh kai na to klisoume
		if(recrel.indexcnt=!0){
			//anoigma skanarismatos gia to
			//SKANARISMA TOU ATTRCAT
			if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
				HF_PrintError("Problem while opening file scan");
	
			check=HF_FindNextRec(scanDescattr,(char*)&recattr);
		
			while(check!=HFE_EOF){

				if(check>=0)
					for(i=2;i<argc;i=i+2)
						if(strcmp(recattr.attrname,argv[i]) && recattr.indexed==TRUE){
							if((indexfd=AM_OpenIndex(argv[1],recattr.indexno))<0)
								AM_PrintError("Error while attempting to open index");
							if(recattr.attrtype=='c')
								sprintf(value,"%s",argv[i+1]);
							
							if(recattr.attrtype=='i')
								sprintf(value,"%d",atoi(argv[i+1]));

							if(recattr.attrtype=='f')
								sprintf(value,"%f",atof(argv[i+1]));


							if(AM_InsertEntry(indexfd,recattr.attrtype,recattr.attrlength,value,recId)<0)
								AM_PrintError("Error while attempting to insert entry in index");
							
							if(AM_CloseIndex(indexfd)<0)
								AM_PrintError("Error while attempting to close index");
					
						} //kleisimo ths if(strcmp(recattr.attrname,argv[i]) && recattr.indexed==TRUE)
					check=HF_FindNextRec(scanDescattr,(char*)&recattr);
			} //kleisimo ths while(check!=HFE_EOF)

			if(HF_CloseFileScan(scanDescrel)<0)
				HF_PrintError("Error while closing scan");

		} //kleisimo ths if(recrel.indexcnt=!0)
	} //kleisimo ths if(recrel!=NULL && found==recrel.attrcnt)


	

	return 0;
}

int DM_add(int argc,char* argv[]){
	printArgs(argc,argv);

	//h praksh INSERT mporei na ginete mono metaksy sxeseon pou einai apolyta idies os pros ton typo ton eggrafon
	//pou periexoun(idios typos,seira kai mikos pedion)... ta pedia den einai aparetita na exoun to idio onoma
	//opote ksekiname me auton ton elegxo

	int scanDescattr1,scanDescattr2; //anagnoristika saroseon gia ATTRCAT
	int scanDescrel1,scanDescrel2,HFScanDesc; //anagnoristika saroseon gia RELCAT
	int HFScanDesc1,HFScanDesc2; //anagnoristika saroseon gia relname1 kai relname 2
	int check,check1,check2;
	int rel1fd,rel2fd; //anagnoristika arxeion gia ta relname1 kai relname2
	int indexfd;
	int found=0;
	int recId;
	attrDesc recattr1;
	attrDesc recattr2;
	relDesc recrel1;
	relDesc recrel2;
	char *record;
	char value[256];
	NOI *mynoi;
	int i;
	
	//to relname1 DEN mporei na einai idio me to relname2
	if(strcmp(argv[1],argv[2])==0){
		printf("ERROR : EQUALITY OF RELNAMES");
		return 1;
	}


	//SKANARISMA TOU RELCAT	GIA TO RELNAME 1
	if((scanDescrel1=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
			HF_PrintError("Problem while opening file scan");

	check=HF_FindNextRec(scanDescrel1,(char*)&recrel1);

	while(check!=HFE_EOF){
		if(check>=0)
			break;
		check=HF_FindNextRec(scanDescrel1,(char*)&recrel1);
	}

	//desmeush xorou pou tha krataei ton arithmo ton eurethrion tou relname1 gia na apofygoume deutera kai
	//trita skanarismata idion pramgaton

	mynoi=malloc(recrel1.attrcnt*sizeof(NOI));
	
	//SKANARISMA TOU RELCAT	GIA TO RELNAME 2
	if((scanDescrel2=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,argv[2]))<0)
			HF_PrintError("Problem while opening file scan");

	check=HF_FindNextRec(scanDescrel2,(char*)&recrel2);

	while(check!=HFE_EOF){
		if(check>=0)
			break;
		check=HF_FindNextRec(scanDescrel2,(char*)&recrel2);
	}
	
	if(HF_CloseFileScan(scanDescrel1)<0)
		HF_PrintError("Error while closing scan");

	if(HF_CloseFileScan(scanDescrel2)<0)
		HF_PrintError("Error while closing scan");

	//elegxos gia diaforetiko arithmo orismaton
	if(recrel1.attrcnt!=recrel2.attrcnt){
		printf("ERROR IN ADD : DIFFERENT NUMBER OF FIELDS");
		return 1;
	}

	//desmeush xorou gia thn eggrafh
	record=malloc(recrel1.relwidth*sizeof(char));
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	//SKANARISMA TOU ATTRCAT
	if((scanDescattr1=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
		HF_PrintError("Problem while opening file scan");
	

	if((scanDescattr2=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[2]))<0)
		HF_PrintError("Problem while opening file scan");
	
	check1=HF_FindNextRec(scanDescattr1,(char*)&recattr1);
	check2=HF_FindNextRec(scanDescattr2,(char*)&recattr2);
	
	i=0;
	while(check1!=HFE_EOF && check2!=HFE_EOF){
		//efoson h sygkrish ton pou pedion pou kanoume einai parallhlh eksasfalizoume kai thn sosth SEIRA ton pedion
		//elegxos gia to onoma tou pediou
		/*if(strcmp(recattr1.attrname,recattr2.attrname)){
			printf("ERROR IN ADD : INCOMPATIBLE NAME OF FIELDS");
			return 1;
		}*/
		
		//elegxos gia to mhkos (LENGTH) ton pedion
		if(recattr1.attrlength!=recattr2.attrlength){
			printf("ERROR IN ADD : INCOMPATIBLE LENGTH OF FIELDS");
			return 1;
		}
		
		//elegxos gia ton TYPO ton pedion
		if(strcmp(&recattr1.attrtype,&recattr2.attrtype)){
			printf("ERROR IN ADD : INCOMPATIBLE TYPE OF FIELDS");
			return 1;
		}		
				
		found++;
		
		if(recattr1.indexed==TRUE){
			mynoi[i].number_of_index=recattr1.indexno;
			mynoi[i].offset=recattr1.offset;
			mynoi[i].type=recattr1.attrtype;
			if(mynoi[i].type!='i' && mynoi[i].type!='f')
				mynoi[i].length=recattr1.attrlength;
		}
	
		else
			mynoi[i].number_of_index=0;

		i++;
		
		check1=HF_FindNextRec(scanDescattr1,(char*)&recattr1);
		check2=HF_FindNextRec(scanDescattr2,(char*)&recattr2);	
	}
	

	if(HF_CloseFileScan(scanDescattr1)<0)
		HF_PrintError("Error while closing scan");

	if(HF_CloseFileScan(scanDescattr2)<0)
		HF_PrintError("Error while closing scan");

	if(found==recrel1.attrcnt){         //perasame ton elegxo ton pedion , h syntaksh ths entolhs einai sosth
					     //proxorame sthn eisagogh ton eggrafon tou relname1 sthn relname2
		

		//anoigma ton relname1 kai relname2
		if((rel1fd=HF_OpenFile(argv[1]))<0)
			HF_PrintError("Error while attempting to open file");
		
		if((rel2fd=HF_OpenFile(argv[2]))<0)
			HF_PrintError("Error while attempting to open file");
		
		//anoigma skanarismatos xoris synthiki gia na paroume OLES tis eggrafes tou relname2
		if((HFScanDesc2=HF_OpenFileScan(rel2fd,recrel1.relwidth,0,0,0,0,NULL))<0)    //gia mikos eggrafhs vazoume ena apo relwidth
											      //eite tou relname1 eite tou relname2 afou einai isa
			HF_PrintError("Problem while opening file scan");

		check=HF_FindNextRec(HFScanDesc2,record);
		
		while(check!=HFE_EOF){
			if(check>=0){	
				//eisagoume thn eggrafh tou relname2(=record) sto relname1(=rel1fd)
				if((recId=HF_InsertRec(rel1fd,record,recrel1.relwidth))<0)
					HF_PrintError("Problem while inserting one of MANY records");	

				//psaxnoume na broume an kapoio apo ta pedia tou relname1 einai eurethriasmeno kai an einai
				//tote eisagoume thn antistoixh eggrafh tou relname2 sto antisoixo eurethrio
			
				for(i=0;i<recrel1.indexcnt;i++)
					if(mynoi[i].number_of_index!=0){
						if((indexfd=AM_OpenIndex(argv[1],mynoi[i].number_of_index))<0)
							AM_PrintError("Error while trying to open index");
						strncpy(value,record+mynoi[i].offset,mynoi[i].length);

			
						if(mynoi[i].type=='i')
							sprintf(value,"%d",atoi(value));
							
						if(mynoi[i].type=='f')
							sprintf(value,"%f",atof(value));

						if(AM_InsertEntry(indexfd,mynoi[i].type,mynoi[i].length,value,recId)<0)
							AM_PrintError("Error while inserting entry in index");

						if(AM_CloseIndex(indexfd)<0)
							AM_PrintError("Error while closing index");
					}
			}

			check=HF_FindNextRec(HFScanDesc2,record);
		}


		if(HF_CloseFileScan(HFScanDesc2)<0)
			HF_PrintError("Error while closing scan");


		if(HF_CloseFile(rel1fd)<0)
			HF_PrintError("Error while closing file");
		
		if(HF_CloseFile(rel2fd)<0)
			HF_PrintError("Error while closing file");


	}

	else
		printf("INVALID ARGUMENTS IN COMMAND LINE PROMPT...READ INSTRUCTIONS AND TRY AGAIN");

	



	return 0;
}


int main(int argc,char* argv[]){
	
	char* dbname;
        char command[80];

	dbname=argv[1];
        sprintf(command,"chdir %s",dbname);
	if(chdir(dbname)){
                printf("Not possible to create database");
                return(2);
        }


	HF_Init();
	AM_Init();	

	//anoigma ton arxeion pou emperiexoun plirofories gia thn bash
	if((relfd=HF_OpenFile("RELCAT"))<0){
		HF_PrintError("Problem with RELCAT's HF_OpenFile");
		exit(2);
	}
	
	if((attrfd=HF_OpenFile("ATTRCAT"))<0){
		HF_PrintError("Problem with ATTRCAT's HF_OpenFile");
		exit(2);
	}

	if((viewfd=HF_OpenFile("VIEWCAT"))<0){
		HF_PrintError("Problem with VEIWCAT's HF_OpenFile");
		exit(2);
	}

	if((viewattrfd=HF_OpenFile("VIEWATTRCAT"))<0){
		HF_PrintError("Problem with VIEWATTRCAT's HF_OpenFile");
		exit(2);
	}
		
	while(yyparse() == RESTART);

	/////////////////////////KLEISIMO TON ARXEION//////////////////////////////////////////////////////////////////////////////
	if(HF_CloseFile(relfd)<0){
		HF_PrintError("Problem while closing RELCAT");
		exit(8);
	}
	
	if(HF_CloseFile(attrfd)<0){
		HF_PrintError("Problem while closing ATTRCAT");
		exit(8);
	}

	if(HF_CloseFile(viewfd)<0){
		HF_PrintError("Problem while closing VIEWCAT");
		exit(8);
	}

	if(HF_CloseFile(viewattrfd)<0){
		HF_PrintError("Problem while closing VIEWATTRCAT");
		exit(8);
	}	

	return 0;
}





