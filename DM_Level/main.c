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

	//we need to check if the given relname (argv[1]) already exists in RELCAT
	//if it exists we return an error, otherwise we proceed to insert the appropriate record in RELCAT AND ATTRCAT
	//to check if a record named "relname" exists in RELCAT we start a scan with HF_OpenFileScan() on RELCAT
	
	if((scanDesc=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0){
		HF_PrintError("Problem while opening file scan");
		return -1;		
	}
	
	printf("initaited scan\n");

	//allocate memory for rec
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


	if(found==0){ //the record was not found so we proceed to update the files RELCAT and ATTRCAT
		
		//update the RELCAT file
		strcpy(newrelentry.relname,argv[1]);	

		newrelentry.relwidth=0;
		for(i=3;i<argc;i=i+2){        //skip the command name, the relation name and the attr#
			if(strcmp(argv[i],"'i'")==0)
				newrelentry.relwidth=newrelentry.relwidth+sizeof(int);
			if(strcmp(argv[i],"'f'")==0)
				newrelentry.relwidth=newrelentry.relwidth+sizeof(float);
			if(strpbrk(argv[i],"c")!=NULL){
				sscanf(argv[i],"'c%d'",&STRINGLENGTH);
				printf("%d\n",STRINGLENGTH);				
				newrelentry.relwidth=newrelentry.relwidth+STRINGLENGTH*sizeof(char);  //set relwidth appropriately;
			}
		}		
		newrelentry.attrcnt=((argc/2)-1);

		newattrentry=malloc(((argc/2)-1)*sizeof(attrDesc));  //allocate memory for the array of attributes to be written to ATTRCAT

		printf("%d\n",newrelentry.attrcnt);

		newrelentry.indexcnt=0;
		
		if(HF_InsertRec(relfd,(char*)(&newrelentry),sizeof(relDesc))<0){
			HF_PrintError("Problem with inserting record for RELCAT");
			exit(3);
		}
		//update the ATTRCAT file
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
				newattrentry[i].attrlength=STRINGLENGTH*sizeof(char);  //set relwidth appropriately;
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
		//after updating ATTRCAT and RELCAT we create the RELNAME file
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
	

	//search to find if the ATTRNAME of the RELNAME given as argument is INDEXED
	//to find this we open a scan on ATTRCAT with the given RELNAME as criterion
	//and in the retrieved record (that matches the given criteria) we check if its ATTRNAME
	//is the same as the given one and if its INDEXED field is "0", because only then can a NEW index
	//be created based on this specific field
	if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
		HF_PrintError("Problem while opening file scan");
	
	printf("%s\n",argv[1]);	

	check=HF_FindNextRec(scanDescattr,(char*)&recattr);
	while(check!=HFE_EOF){
		//instead of finding if the given ATTRNAME is INDEXED, it suffices to find if the given ATTRNAME
		//is NOT INDEXED.
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
		//open a second scan on RELCAT to find the relevant record and update it
		//the fields RELNAME,RELWIDTH and ATTRCNT of RELCAT as well as RELNAME,ATTRNAME,OFFSET,ATTRLENGTH and ATTRTYPE
		//of ATTRCAT remain the same...

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
	
		//update the sequential number of the newly created index
		recattr.indexno=(recrel.indexcnt)+1;
		
		//create the index
		if(AM_CreateIndex(argv[1],recrel.indexcnt,recattr.attrtype,recattr.attrlength)<0){
			AM_PrintError("Error while creating new index");
		}

		recrel.indexcnt++;

		//write the updated records to the file
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
	
	//SCAN RELCAT
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

	//SCAN ATTRCAT
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
	int HFScanDesc;    //scan descriptor for relname
	int check;
	int recId;
	int scanDescattr,scanDescrel,scanDescindex;
	int indexfd;       //index file descriptor (open file)
	int rel1fd;         //relname file descriptor (open file)
	int tempfd;        //temporary file descriptor for inserting projected results
	char *record;
	char *projectedrecord;// record containing only the interesting fields
	char** utcargs;    //array in which we build the arguments for UT_Create
						//and pass it as the second argument to that function

	char* utdargs[3];
	char buffer[]="TEMP";
	char* onomasxeshs;
	NOI *INF;
	int recsize=0; //size of each projected record
	int i,j,k;
	int found;
	int utcargc;   //the argc for UT_Create
	int output_file_exists=0;
	int start,finish;
	char telestes[]="><=";
	int telestis;
	int noo;
	int newoffset=0;   //offset in TEMP
	int numberint;
	float numberfloat;
	//case where select was called with relation.field
	if(argc<6){
		printf("TOO FEW ARGUMENTS\n");
		return 1;
	}
	
	//if there is a <WHERE condition> we need to check if the selection relation name
	//is the same as the projected relation name
	if(strpbrk(argv[argc-2],telestes)!=NULL && strcmp(argv[2],argv[argc-4])){
		printf("SELECTION'S RELNAME DIFFERS FROM PROJETED RELATIONS' NAMES\n");
		return 1;
	}
	
	if(strpbrk(argv[argc-2],telestes)!=NULL)
		finish=argc-5;
	else
		finish=argc-1;

	//SCAN RELCAT
	//check if relname exists in RELCAT
	if(atoi(argv[1])==0){   //if the int representation of argv[1] is 0 then we were given an output file	
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
	//since the parser determines when the query is a selection type (and calls DM_Select()) this means that
	//as arguments to DM_Select() ONLY identical projected relation names are passed
	//and we simply need to check if the projected relation fields are actually fields 
	//of the relations via a scan on ATTRCAT
	
	//with the "for" loop we iterate over ALL the projected relation fields
	//and for each one we check if it exists in ATTRCAT by scanning it
	
	
	//we need to create the array that we will pass as
	//argument to UT_Create().. i.e. the utcargs[][] array
	
	//allocate memory for the utcargs array
	utcargs=malloc(utcargc*sizeof(char*));
	
	for(i=0;i<utcargc;i++)
		utcargs[i]=malloc(256*sizeof(char));
		
	
	sprintf(utcargs[0],"%s","create");
	if(output_file_exists==1)
		sprintf(utcargs[1],"%s",argv[1]);
	else
		sprintf(utcargs[1],"%s",buffer);

	//allocate memory for the INF array
	
	INF=malloc(noo*sizeof(NOI));

	//allocate memory for the utdargs array

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
	
		//SCAN ATTRCAT
		if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,onomasxeshs))<0)
			HF_PrintError("Problem while opening file scan");
		
		check=HF_FindNextRec(scanDescattr,(char*)&recattr);
			
		while(check!=HFE_EOF){
			//found the projected field as a field of relname
			if(strcmp(recattr.attrname,argv[i])==0){
				recsize+=recattr.attrlength;
				found=1;
				
				///simultaneously we build the UT_Create arguments array - start of construction
				sprintf(utcargs[j],"%s",recattr.attrname);
				j++;
				if(recattr.attrtype=='c')
					sprintf(utcargs[j],"'%c%d'",recattr.attrtype,recattr.attrlength);
				else
					sprintf(utcargs[j],"'%c'",recattr.attrtype);
				j++;	
				//end of construction
				
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
	//allocate memory for the projected record
	//projectedrecord=malloc(recsize*sizeof(char));
	
	//////END OF ARGUMENT VALIDATION
	
	//open the relname file
	if((rel1fd=HF_OpenFile(onomasxeshs))<0)
		HF_PrintError("SELECT");
		
	//create temporary file for inserting projected results

	//case where no output file name was given to store
	//the results of the select
	if(output_file_exists==1){
		//create the file
		if(UT_create(utcargc,utcargs)<0){
			printf("Error while creating temporary file\n");
			return -1;
		}
	
		//open the file
		if((tempfd=HF_OpenFile(argv[1]))<0)
			HF_PrintError("SELECT");

	
	}

					
	//case where an output file name was given to store the results
	else{
		//create the file
		if(UT_create(utcargc,utcargs)<0){
			printf("Error while creating output file %s\n",argv[1]);
			return -1;
		}
		
		//open the file
		if((tempfd=HF_OpenFile(utcargs[1]))<0)
			HF_PrintError("SELECT");

	}
	
	
	///////BEGIN SELECT
	
	//FIRST CASE: WE HAVE <WHERE>
	
	//we check the position in argv[] where the comparison operator is stored
	//if the pointer at that position is different from NULL then we have WHERE
	//otherwise we do not
	
	if(strpbrk(argv[argc-2],telestes)!=NULL){
	
		//let's check if the condition field is INDEXED
		//SCAN ATTRCAT
		if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,onomasxeshs))<0)
			HF_PrintError("Problem while opening file scan");
		
		check=HF_FindNextRec(scanDescattr,(char*)&recattr);
			
		while(check!=HFE_EOF){
			if(strcmp(recattr.attrname,argv[argc-3])==0){
				break;
				//we exit the while having kept in recattr the details of the 
				//field we are interested in, i.e. the details of the comparison field
			}
			
			check=HF_FindNextRec(scanDescattr,(char*)&recattr);
		}
		
		if(HF_CloseFileScan(scanDescattr)<0)
			HF_PrintError("SELECT");

		//FIRST SUB-CASE of the first case: the WHERE field is indexed
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
	
		//SECOND SUB-CASE of the first case: the WHERE field is NOT indexed
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
	
	
	//SECOND CASE: WE DO NOT HAVE <WHERE>
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
	
	
	//if an output file name was given then we do not print anything
	//if no output file name was given we need to print the records that
	//we have stored in "temp"
	
	if(output_file_exists==0){
		//scan "temp" without condition to print all records
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
	
	
	//close relname
	if(HF_CloseFile(rel1fd)<0){
		HF_PrintError("SELECT");
		return -1;
	}
		
	//close and destroy the temporary files
	if(output_file_exists==0){
		//close
		if(HF_CloseFile(tempfd)<0){
			HF_PrintError("SELECT");
			return -1;
		}
		//destroy
		if(UT_destroy(2,utdargs)<0){
			printf("Error while destroying temporary file in select\n");
			return -1;
		}
	}
	
	else{
		//close
		if(HF_CloseFile(tempfd)<0){
			HF_PrintError("SELECT");
			return -1;
		}
		
		//if an output file was given... we simply do not destroy it 
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
	int HFScanDesc;    //scan descriptor for relname
	int check;
	int recId;
	int scanDescattr,scanDescrel,scanDescindex,scanDescindex2;
	int indexfd,indexfdtemp;       //index file descriptor (open file)
	int rel1fd;         //relname file descriptor (open file)
	int found=0;
	int telestis;
	char *record;
	
	//in delete we have 2 cases: either a condition is given and we need to delete 
	//the records that satisfy it, or no condition is given and we need to delete
	//ALL records from the RELNAME.
	
	//SCAN RELCAT
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
		
	//there is no need to check the operator since we have a B+ tree and all
	//operators are supported during the scan. we just need to check if argv[2]
	//and argv[3] are NULL to determine if we have a condition or not
	
	//we start with the case where there is no delete condition
	//recrel is not NULL meaning a record for relname was found in RELCAT
	//while argv[2] and argv[3] are NULL since no delete condition was given
	
	//allocate memory for the record
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

		
		
		//exiting the while, ALL records have been deleted from relname and the only thing left
		//is to check if any of the record fields were indexed, in which case we need to delete 
		//the records from that index as well
		
		if(recrel.indexcnt=0){
			//SCAN ATTRCAT
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
								//we use HF_GetThisRec to build the RECORD argument
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
			} //end of while(check!=HFFE_EOF)
			
			if(HF_CloseFileScan(scanDescattr)<0){
				HF_PrintError("DELETE : Error while closing scan");
				return -1;
			}

		}//end of if(recrel.indexcnt=0)
	}//end of if(recrel!=NULL && argv[2]==NULL && argv[3]==NULL)
	
	
	//SECOND CASE: relname was found in RELCAT but here we need to delete specific
	//records depending on the given condition
	
	if(found==1 && argv[2]!=NULL && argv[3]!=NULL){
		
		//since the TYPE of the field being scanned is not given to us, we need to
		//look up the ATTRCAT and find the attrname of the relname and its corresponding TYPE
		//we do this because HF_OpenFileScan takes attrtype as an argument.. which unfortunately is not given to us :(
		//SCAN ATTRCAT
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
		
		//exiting the while we have found the type of the field on which
		//the comparison operator is applied
		
		//open the heap file from which we delete records
		if((rel1fd=HF_OpenFile(argv[1]))<0){
			HF_PrintError("Error while trying to open file");
			return -1;
		}
		
		
		//exploit the index that may exist on the comparison field
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
				//delete the record from the heap
				if(HF_DeleteRec(rel1fd,recId,recrel.relwidth)<0){
					HF_PrintError("Error while deleting record");
					return -1;
				}
				
				//the next 2 commands perform the deletion of the record from the index
				if(HF_GetThisRec(indexfd,recId,record,recattr.attrlength)<0){
					HF_PrintError("DELETE : Error while fetching specified record");
					return -1;
				}
								
				if(AM_DeleteEntry(indexfd,recattr.attrtype,recattr.attrlength,record,recId)<0){
					AM_PrintError("DELETE");
					return -1;
				}
				
				//besides deleting the record from the index of the condition field
				//we also need to delete the corresponding records from ALL indexes of the indexed
				//fields of relname.... so we start a scan on ATTRCAT to see which
				//fields besides the condition field are indexed and delete the corresponding record
				//as in the previous commands
				//SCAN ATTRCAT
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
				//exiting the while we have deleted ALL records with record id recId
				//from ALL indexes of relname

				recId=AM_FindNextEntry(scanDescindex);
			}//end of while(recId!=AME_EOF)
		
			if(AM_CloseIndexScan(scanDescindex)<0){
				AM_PrintError("DELETE : Error while closing scan");
				return -1;
			}
						
			if(AM_CloseIndex(indexfd)<0){
				AM_PrintError("DELETE : Error while closing index");
				return -1;
			}
			
		} //end of if(recattr.indexed==TRUE)
		
		//begin scan with the given condition
		//there is no index on the given condition field
		else{
			
			if((HFScanDesc=HF_OpenFileScan(rel1fd,recrel.relwidth,recattr.attrtype,recattr.attrlength,recattr.offset,telestis,argv[4]))<0){
				HF_PrintError("Problem while opening file scan");
				return -1;
			}
			
			recId=HF_FindNextRec(HFScanDesc,record);
		
			while(recId!=HFE_EOF){
				if(recId>=0){
					//delete the record from the HEAP
					if(HF_DeleteRec(rel1fd,recId,recrel.relwidth)<0){
						HF_PrintError("Error while deleting record");
						return -1;
					}
					
					//SCAN ATTRCAT
					//we will search to find where the specific recId returned by
					// HF_FindNextEntry exists in indexes of OTHER fields
					
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
		
			} //end of while(recId!=HFE_EOF)
		
						
			if(HF_CloseFileScan(HFScanDesc)<0){
				HF_PrintError("DELETE : Error while closing scan");
				return -1;
			}
			//deleted ALL records from the heap

		
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

	int scanDescattr1,scanDescattr2; //scan descriptors for ATTRCAT
	int scanDescrel1,scanDescrel2,HFScanDesc; //scan descriptors for RELCAT
	int HFScanDesc1,HFScanDesc2; //scan descriptors for relname1 and relname2
	int check,check1,check2;
	int rel1fd,rel2fd; //file descriptors for relname1 and relname2
	int indexfd;
	int found=0;
	int recId;
	attrDesc recattr1;
	attrDesc recattr2;
	relDesc recrel1;
	relDesc recrel2;
	char *record1;
	char *record2;

	

	//relname1 CANNOT be the same as relname2
	if(strcmp(argv[1],argv[2])==0){
		printf("ERROR : EQUALITY OF RELNAMES");
		return 1;
	}


	//SCAN RELCAT FOR RELNAME 1
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


	//SCAN RELCAT FOR RELNAME 2
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

	//check for different number of fields
	if(recrel1.attrcnt!=recrel2.attrcnt){
		printf("ERROR IN ADD : DIFFERENT NUMBER OF FIELDS");
		return 1;
	}

	//allocate arrays for comparing records from the 2 files
	record1=malloc(recrel1.relwidth*sizeof(char));
	record2=malloc(recrel1.relwidth*sizeof(char));
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	//SCAN ATTRCAT
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
		//since the field comparison we perform is parallel, we also ensure the correct ORDER of fields
		//check the field name
		/*if(strcmp(recattr1.attrname,recattr2.attrname)){
			printf("ERROR IN ADD : INCOMPATIBLE NAME OF FIELDS");
			return 1;
		}*/
		
		//check the LENGTH of fields
		if(recattr1.attrlength!=recattr2.attrlength){
			printf("ERROR IN ADD : INCOMPATIBLE LENGTH OF FIELDS");
			return 1;
		}
		
		//check the TYPE of fields
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
		//open relname1 and relname2
		if((rel1fd=HF_OpenFile(argv[1]))<0){
			HF_PrintError("Error while attempting to open file");
			return -1;
		}
		
		if((rel2fd=HF_OpenFile(argv[2]))<0){
			HF_PrintError("Error while attempting to open file");
			return -1;
		}

		//open scan without condition to get ALL records of relname2
		if((HFScanDesc2=HF_OpenFileScan(rel2fd,recrel1.relwidth,0,0,0,0,NULL))<0){    //for record length we use either relwidth
											      //of relname1 or relname2 since they are equal
			HF_PrintError("Problem while opening file scan");
			return -1;
		}

		check2=HF_FindNextRec(HFScanDesc2,record2);

		while(check2!=HFE_EOF){
			//open scan with condition to find record2 in relname1 and delete it
			if((HFScanDesc1=HF_OpenFileScan(rel1fd,recrel1.relwidth,0,0,0,0,NULL))<0){
											      //for record length we use either relwidth
											      //of relname1 or relname2 since they are equal
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

		
	//with this subtract we delete records that are stored in the heap
	//and we do not exploit potential indexes on the fields of either relname
	//TODO: FIND A WAY TO EXPLOIT THEM FOR FASTER DELETION

	return 0;
}

int DM_insert(int argc,char* argv[]){
	
	int scanDescattr,scanDescrel;
	int check,notfound;
	int temprelfd,indexfd;
	int found=0;
	int recId;
	int offset=0; //helper variable for building the record
	int foundrelname=0;
	int *soa; //array where we keep the sequence of arguments (sequence of arguments)
	int numberint;
	float numberfloat;
	char *buffer;  //buffer in which we place the new record
	char value[256];  //buffer for getting the indexed value of the field
	NOI *types;
	attrDesc recattr;
	relDesc recrel;
	int i,j;
	
	//printArgs(argc,argv);
	for(i=0;i<argc;i++)
		printf("argv[%d]=%s\n",i,argv[i]);

	//during record insertion we need to ensure that its fields are the same as 
	//the fields of the RELNAME into which it will be inserted...

	//SCAN RELCAT
	//we will search to find if the first variable given as argument (i.e. ARGV[1])
	//exists in the RELCAT table
	
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

	//SCAN ATTRCAT
	if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
		HF_PrintError("Problem while opening file scan");
	
	check=HF_FindNextRec(scanDescattr,(char*)&recattr);

	//allocate memory for the types array
	types=malloc(recrel.attrcnt*sizeof(NOI));

	//allocate memory for the buffer where we will build the record
	buffer=malloc(recrel.relwidth*sizeof(char));
	
	//allocate memory for the sequence of arguments array = soa
	soa=malloc(recrel.attrcnt*sizeof(int));
	j=0;
	while(check!=HFE_EOF){

		if(check>=0)
			for(i=2;i<argc;i=i+2)
				if(strcmp(recattr.attrname,argv[i])==0 && (recattr.attrlength>=sizeof(argv[1]))/*check the type??*/){
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

	if(foundrelname==1 && found==recrel.attrcnt){     //RELNAME was found in RELCAT and found (which indicates HOW MANY identical
							//variables with the given arguments were found in ATTRCAT) equals the number
							//of fields of the RELNAME where the record will be inserted
		//build the record
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

		//next we need to check if any of the newly inserted record's fields is indexed
		//if so we need to find the index, open it, insert the record and close it
		if(recrel.indexcnt=!0){
			//open scan for
			//SCAN ATTRCAT
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
					
						} //end of if(strcmp(recattr.attrname,argv[i]) && recattr.indexed==TRUE)
					check=HF_FindNextRec(scanDescattr,(char*)&recattr);
			} //end of while(check!=HFE_EOF)

			if(HF_CloseFileScan(scanDescrel)<0)
				HF_PrintError("Error while closing scan");

		} //end of if(recrel.indexcnt=!0)
	} //end of if(recrel!=NULL && found==recrel.attrcnt)


	

	return 0;
}

int DM_add(int argc,char* argv[]){
	printArgs(argc,argv);

	//the INSERT operation can only be performed between relations that are absolutely identical in terms of record type
	//they contain (same type, order and length of fields)... the fields do not necessarily need to have the same name
	//so we start with this check

	int scanDescattr1,scanDescattr2; //scan descriptors for ATTRCAT
	int scanDescrel1,scanDescrel2,HFScanDesc; //scan descriptors for RELCAT
	int HFScanDesc1,HFScanDesc2; //scan descriptors for relname1 and relname2
	int check,check1,check2;
	int rel1fd,rel2fd; //file descriptors for relname1 and relname2
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
	
	//relname1 CANNOT be the same as relname2
	if(strcmp(argv[1],argv[2])==0){
		printf("ERROR : EQUALITY OF RELNAMES");
		return 1;
	}


	//SCAN RELCAT FOR RELNAME 1
	if((scanDescrel1=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
			HF_PrintError("Problem while opening file scan");

	check=HF_FindNextRec(scanDescrel1,(char*)&recrel1);

	while(check!=HFE_EOF){
		if(check>=0)
			break;
		check=HF_FindNextRec(scanDescrel1,(char*)&recrel1);
	}

	//allocate memory to hold the index numbers of relname1 to avoid second and
	//third scans of the same data

	mynoi=malloc(recrel1.attrcnt*sizeof(NOI));
	
	//SCAN RELCAT FOR RELNAME 2
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

	//check for different number of fields
	if(recrel1.attrcnt!=recrel2.attrcnt){
		printf("ERROR IN ADD : DIFFERENT NUMBER OF FIELDS");
		return 1;
	}

	//allocate memory for the record
	record=malloc(recrel1.relwidth*sizeof(char));
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	//SCAN ATTRCAT
	if((scanDescattr1=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[1]))<0)
		HF_PrintError("Problem while opening file scan");
	

	if((scanDescattr2=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[2]))<0)
		HF_PrintError("Problem while opening file scan");
	
	check1=HF_FindNextRec(scanDescattr1,(char*)&recattr1);
	check2=HF_FindNextRec(scanDescattr2,(char*)&recattr2);
	
	i=0;
	while(check1!=HFE_EOF && check2!=HFE_EOF){
		//since the field comparison we perform is parallel, we also ensure the correct ORDER of fields
		//check the field name
		/*if(strcmp(recattr1.attrname,recattr2.attrname)){
			printf("ERROR IN ADD : INCOMPATIBLE NAME OF FIELDS");
			return 1;
		}*/
		
		//check the LENGTH of fields
		if(recattr1.attrlength!=recattr2.attrlength){
			printf("ERROR IN ADD : INCOMPATIBLE LENGTH OF FIELDS");
			return 1;
		}
		
		//check the TYPE of fields
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

	if(found==recrel1.attrcnt){         //we passed the field validation, the command syntax is correct
					     //we proceed to insert records from relname1 into relname2
		

		//open relname1 and relname2
		if((rel1fd=HF_OpenFile(argv[1]))<0)
			HF_PrintError("Error while attempting to open file");
		
		if((rel2fd=HF_OpenFile(argv[2]))<0)
			HF_PrintError("Error while attempting to open file");
		
		//open scan without condition to get ALL records of relname2
		if((HFScanDesc2=HF_OpenFileScan(rel2fd,recrel1.relwidth,0,0,0,0,NULL))<0)    //for record length we use either relwidth
											      //of relname1 or relname2 since they are equal
			HF_PrintError("Problem while opening file scan");

		check=HF_FindNextRec(HFScanDesc2,record);
		
		while(check!=HFE_EOF){
			if(check>=0){	
				//insert the record from relname2 (=record) into relname1 (=rel1fd)
				if((recId=HF_InsertRec(rel1fd,record,recrel1.relwidth))<0)
					HF_PrintError("Problem while inserting one of MANY records");	

				//search to find if any of relname1's fields is indexed and if so
				//insert the corresponding record from relname2 into the corresponding index
			
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

	//open the files that contain information about the database
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

	/////////////////////////CLOSE FILES//////////////////////////////////////////////////////////////////////////////
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





