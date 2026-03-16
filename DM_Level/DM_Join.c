#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser3.h"
#include "HF_Lib.h"
#include "AM_Lib.h"


int DM_select(int argc, char* argv[]){
	printArgs(argc,argv);
	
	relDesc *recrel;
	attrDesc *recattr;
	int HFScanDesc;    //scan descriptor for relname
	int check;
	int recId;
	int scanDescattr,scanDescrel,scanDescindex;
	int indexfd;       //index file descriptor (open file)
	int rel1fd;         //relname file descriptor (open file)
	int tempfd;        //temporary file descriptor for inserting projected results
	char record[1024];
	char *projectedrecord;// record containing only the interesting fields
	char** utcargs;    //array in which we build the arguments for UT_Create
						//and pass it as the second argument to that function
	NOI *INF;
	int recsize=0; //size of each projected record
	int i,j,k;
	int found;
	
	
	//case where select was called with relation.field
	if(argc<6){
		printf("TOO FEW ARGUMENTS\n");
		return 1;
	}
	
	//if there is a <WHERE condition> we need to check if the selection relation name
	//is the same as the projected relation name
	if(strcmp(argv[3],argv[argc-4])){
		printf("SELECTION'S RELNAME DIFFERS FROM PROJETED RELATIONS' NAMES");
		return 1;
	}
	
	

	//SCAN RELCAT
	//check if relname exists in RELCAT
	if((scanDescrel=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,argv[3]))<0)
			HF_PrintError("Problem while opening file scan");

	check=HF_FindNextRec(scanDescrel,(char*)recrel);
	
	while(check!=HFE_EOF){
		if(check>0)
			break;
		
		check=HF_FindNextRec(scanDescrel,(char*)recrel);		
	
	}
	
	if(HF_CloseFileScan(scanDescrel)<0)
		HF_PrintError("SELECT : Error while closing scan");

	
	//since the parser determines when the query is a selection type (and calls DM_Select()) this means that
	//as arguments to DM_Select() ONLY identical projected relation names are passed
	//and we simply need to check if the projected relation fields are actually fields 
	//of the relations via a scan on ATTRCAT
	
	//with the "for" loop we iterate over ALL the projected relation fields
	//and for each one we check if it exists in ATTRCAT by scanning it
	
	
	//we need to create the array that we will pass as
	//argument to UT_Create().. i.e. the utcargs[][] array
	
	//allocate memory for the array
	utcargs=malloc(2*atoi(argv[2])*sizeof(char**));
	
	for(i=0;i<argc;i++)
		utcargs[i]=malloc(256*sizeof(char));
		
	
	sprintf(utcargs[0],"%s","create");
	sprintf(utcargs[1],"%s",argv[3]);
	
	//allocate memory for the INF array
	
	INF=malloc(atoi(argv[2])*sizeof(NOI));
	
	j=2;
	k=0;
	
	for(i=4;i<argc-2;i=i+2){
		
		found=0;
	
		//SCAN ATTRCAT
		if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[3]))<0)
			HF_PrintError("Problem while opening file scan");
		
		check=HF_FindNextRec(scanDescattr,(char*)recattr);
			
		while(check!=HFE_EOF){
			//found the projected field as a field of relname
			if(strcmp(recattr->attrname,argv[i])==0){
				recsize+=recattr->attrlength;
				found=1;
				
				///simultaneously we build the UT_Create arguments array - start of construction
				sprintf(utcargs[j],"%s",recattr->attrname);
				j++;
				if(recattr->attrtype=='c')
					sprintf(utcargs[j],"%c%d",recattr->attrtype,recattr->attrlength);
				else
					sprintf(utcargs[j],"%c",recattr->attrtype);
				j++;	
				//end of construction
				
				INF[k].offset=recattr->offset;
				INF[k].type=recattr->attrtype;
				if(recattr->attrtype=='c')
					INF[k].length=recattr->attrlength;
					
				k++;
				
				break;
			}
			
			check=HF_FindNextRec(scanDescattr,(char*)recattr);
		}
		
		if(HF_CloseFileScan(scanDescattr)<0)
			HF_PrintError("DELETE : Error while closing scan");

		
		if(found==0){
			printf("REQUESTED FIELD FOR PROJECTION NOT FOUND IN RELCAT... RETYPE SELECTION USING DIFFERENT ARGUMENTS\n");
			return 1;
		}
		
	}

	
	//////END OF ARGUMENT VALIDATION
	
	//open the relname file
	if((rel1fd=HF_OpenFile(argv[3]))<0)
		HF_PrintError("SELECT");
		
	//create temporary file for inserting projected results

	//case where no output file name was given to store
	//the results of the select
	if(argv[1]==NULL){
		//create the file
		if(UT_Create(2*atoi(argv[2])+2,utcargs)<0){
			printf("Error while creating temporary file\n");
			return -1;
		}
	
		//open the file
		if((tempfd=HF_OpenFile("temp"))<0)
			HF_PrintError("SELECT");

	
	}

					
	//case where an output file name was given to store the results
	else{
		//create the file
		if(UT_Create(2*atoi(argv[2])+2,utcargs)<0){
			printf("Error while creating output file %s\n",argv[1]);
			return -1;
		}
		
		//open the file
		if((tempfd=HF_OpenFile(argv[1]))<0)
			HF_PrintError("SELECT");

	}
	
	
	///////BEGIN SELECT
	
	//FIRST CASE: WE HAVE <WHERE>
	
	//we check the position in argv[] where the comparison operator is stored
	//if the pointer at that position is different from NULL then we have WHERE
	//otherwise we do not
	
	if(argv[argc-2]!=NULL){
	
		//let's check if the condition field is INDEXED
		//SCAN ATTRCAT
		if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[3]))<0)
			HF_PrintError("Problem while opening file scan");
		
		check=HF_FindNextRec(scanDescattr,(char*)recattr);
			
		while(check!=HFE_EOF){
			if(strcmp(recattr->attrname,argv[argc-3])==0){
				break;
				//we exit the while having kept in recattr the details of the 
				//field we are interested in, i.e. the details of the comparison field
			}
			
			check=HF_FindNextRec(scanDescattr,(char*)recattr);
		}
		
		if(HF_CloseFileScan(scanDescattr)<0)
			HF_PrintError("SELECT");

		//FIRST SUB-CASE of the first case: the WHERE field is indexed
		if(recattr->indexed==TRUE){
			if((indexfd=AM_OpenIndex(argv[3],recattr->indexno))<0)
				AM_PrintError("SELECT");
				
			if((scanDescindex=AM_OpenIndexScan(indexfd,recattr->attrtype,recattr->attrlength,atoi(argv[argc-2]),argv[argc-1]))<0)
				AM_PrintError("SELECT");

			recId=AM_FindNextEntry(scanDescindex);
			
			while(recId!=AME_EOF){
				if(recId>0){
					if(HF_GetThisRec(rel1fd,recId,record,recrel->relwidth)<0)
						HF_PrintError("SELECT");
						
					interestingfields(&projectedrecord,record,INF,atoi(argv[2]));
					
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
			if((HFScanDesc=HF_OpenFileScan(rel1fd,recrel->relwidth,recattr->attrtype,recattr->attrlength,recattr->offset,atoi(argv[argc-2]),argv[argc-1]))<0)
				HF_PrintError("SELECT");
				
			recId=HF_FindNextRec(HFScanDesc,record);
			
			while(recId!=HFE_EOF){
				if(recId>0){
					interestingfields(&projectedrecord,record,INF,atoi(argv[2]));
					
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
		if((HFScanDesc=HF_OpenFileScan(rel1fd,recrel->relwidth,0,0,0,0,NULL))<0)
			HF_PrintError("SELECT");
				
		recId=HF_FindNextRec(HFScanDesc,record);
			
		while(recId!=HFE_EOF){
			if(recId>0){
				interestingfields(&projectedrecord,record,INF,atoi(argv[2]));
					
				if(HF_InsertRec(tempfd,projectedrecord,recsize)<0)
					HF_PrintError("SELECT");
			}
			
			recId=HF_FindNextRec(HFScanDesc,record);
		}
		
		if(HF_CloseFileScan(HFScanDesc)<0)
			HF_PrintError("SELECT");
	}
	
	
	//if an output file name was given then we do not print anything
	//if no output file name was given we need to print the records that
	//we have stored in "temp"
	
	if(argv[1]==NULL){
		//scan "temp" without condition to print all records
		if((HFScanDesc=HF_OpenFileScan(tempfd,recsize,0,0,0,0,NULL))<0)
			HF_PrintError("SELECT");
			
		recId=HF_FindNextRec(HFScanDesc,record);
		
		while(recId!=HFE_EOF)
			printf("%s",record);
	}	                                                                            
	
	
	//close relname
	if(HF_CloseFile(rel1fd)<0)
		HF_PrintError("SELECT");
		
	//close and destroy the temporary files
	if(argv[1]==NULL){
		//close
		if(HF_CloseFile(tempfd)<0)
			HF_PrintError("SELECT");
		
		//destroy
		if(UT_Destroy(NULL,"temp")<0){
			printf("Error while destroying temporary file in select\n");
			return -1;
		}
	}
	
	else{
		//close
		if(HF_CloseFile(tempfd)<0)
			HF_PrintError("SELECT");
		
		//destroy
		if(UT_Destroy(NULL,argv[1])<0){
			printf("Error while destroying output file %s in select\n",argv[1]);
			return -1;
		}
	}

	
	return 0;
}


int DM_join(int argc, char* argv[]){
	printArgs(argc,argv);
	
	relDesc recrel1,recrel2;
	attrDesc recattr,recattr1,recattr2;
	int HFScanDesc,HFScanDesc1,HFScanDesc2;    //scan descriptor for relname
	int check;
	int recId1,recId2;
	int scanDescattr,scanDescrel,scanDescrel2,scanDescindex,scanDescattr1,scanDescattr2;
	int indexfd;       //index file descriptor (open file)
	int rel1fd,rel2fd;         //relname file descriptor (open file)
	int tempfd;        //temporary file descriptor for inserting projected results
	char record[1024];
	char *projectedrecord1,*projectedrecord2,*joinedprojectedrecord,;// record containing only the interesting fields
	char** utcargs;    //array in which we build the arguments for UT_Create
						//and pass it as the second argument to that function
	NOI *INF;
	int recsize=0; //size of each projected record
	int i,j,k;
	int found=0;
	int diaforetiko;
	
	
	//SCAN RELCAT
	//check if relname1 exists in RELCAT
	if((scanDescrel=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,argv[3]))<0)
		HF_PrintError("Problem while opening file scan");
		
	check=HF_FindNextRec(scanDescrel,(char*)&recrel1);
	
	while(check!=HFE_EOF){
		if(check>0){
			found=1;
			break;
		}
		check=HF_FindNextRec(scanDescrel,(char*)&recrel1);		
	
	}
	
	if(HF_CloseFileScan(scanDescrel)<0)
		HF_PrintError("JOIN : Error while closing scan");

	if(found==0)
		return -1;
	
	
	for(i=5;i<=argc-7;i=i+2){//search for the name of the second relation
	
		if(strcmp(argv[3],argv[i])!=0){
			diaforetiko=i;
			break;
		}
	}
	
	//SCAN RELCAT
	//check if relname2 exists in RELCAT
	
	if((scanDescrel=HF_OpenFileScan(relfd,sizeof(relDesc),'c',MAXNAME,0,EQUAL,argv[diaforetiko]))<0)	
		HF_PrintError("Problem while opening file scan");
		
	check=HF_FindNextRec(scanDescrel,(char*)&recrel2);
	
	found=0;
		
	while(check!=HFE_EOF){
		if(check>0){
			found=1;	
			break;
		}
			
		check=HF_FindNextRec(scanDescrel,(char*)&recrel2);		
	
	}
		
	if(HF_CloseFileScan(scanDescrel)<0)
		HF_PrintError("JOIN : Error while closing scan");
	
	if(found==0)
		return -1;
		
	

	for(i=4;i<argc-2;i=i+2){
		
		found=0;
	
		//SCAN ATTRCAT to check if the argument fields correspond to the given relations
		if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[i-1]))<0)
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
					sprintf(utcargs[j],"%c%d",recattr.attrtype,recattr.attrlength);
				else
					sprintf(utcargs[j],"%c",recattr.attrtype);
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
			HF_PrintError("JOIN : Error while closing scan");

		
		if(found==0){
			printf("REQUESTED FIELD FOR PROJECTION NOT FOUND IN RELCAT... RETYPE SELECTION USING DIFFERENT ARGUMENTS\n");
			return 1;
		}
		
	}
//END OF ARGUMENT VALIDATION//////////////////////////////////////////////	

	if(strcmp(argv[4],argv[diaforetiko+1])==0){ // if the fields have the same name, we need to differentiate them somehow
		sprintf(argv[4],"%s1",argv[4]);
		sprintf(argv[diaforetiko+1],"%s2",argv[diaforetiko+1]);
	}

	//open argv[3]
	if((rel1fd=HF_OpenFile(argv[3]))<0)
		HF_PrintError("JOIN");
		
	//create temporary file for inserting projected results

	//case where no output file name was given to store
	//the results of the select
	if(argv[1]==NULL){
		//create the file
		if(UT_Create(2*atoi(argv[2])+2,utcargs)<0){
			printf("Error while creating temporary file\n");
			return -1;
		}
	
		//open the file
		if((tempfd=HF_OpenFile("temp"))<0)
			HF_PrintError("JOIN");

	
	}

					
	//case where an output file name was given to store the results
	else{
		//create the file
		if(UT_Create(2*atoi(argv[2])+2,utcargs)<0){
			printf("Error while creating output file %s\n",argv[1]);
			return -1;
		}
		
		//open the file
		if((tempfd=HF_OpenFile(argv[1]))<0)
			HF_PrintError("JOIN");

	}

	//CHECK IF THE FIELDS OF THE TWO RELATIONS ARE INDEXED
	if((scanDescattr1=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[3]))<0)
			HF_PrintError("Problem while opening file scan");
		
		check=HF_FindNextRec(scanDescattr1,(char*)&recattr1);
			
		while(check!=HFE_EOF){
			if(strcmp(recattr1.attrname,argv[argc-5])==0){//if it finds the name of join field 1
				break;
				//we exit the while having kept in recattr the details of the 
				//field we are interested in, i.e. the details of the comparison field
			}
			
			check=HF_FindNextRec(scanDescattr1,(char*)&recattr1);
		}
		
		if(HF_CloseFileScan(scanDescattr1)<0)
			HF_PrintError("JOIN");

	
	
	

	if((scanDescattr2=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[diaforetiko]))<0)
		HF_PrintError("Problem while opening file scan");
		
	check=HF_FindNextRec(scanDescattr2,(char*)&recattr2);
			
	while(check!=HFE_EOF){
		if(strcmp(recattr2.attrname,argv[argc-3])==0){//if it finds the name of join field 2
			break;
			//we exit the while having kept in recattr the details of the 
			//field we are interested in, i.e. the details of the comparison field
		}
			
		check=HF_FindNextRec(scanDescattr2,(char*)&recattr2);
	}
		
	if(HF_CloseFileScan(scanDescattr2)<0)
		HF_PrintError("JOIN");


	
	
//NOW THE JOIN CASES FOR INDEXES BEGIN////////////////////////////////////

	
	//First case
	if(recattr2.indexed==TRUE){//relation 2 becomes the inner, regardless of what relation 1 is
		if((HFScanDesc=HF_OpenFileScan(rel1fd,recrel1.relwidth,recattr1.attrtype,recattr1.attrlength,recattr1.offset,atoi(argv[argc-5]),NULL)<0)
			HF_PrintError("JOIN");
				
		recId1=HF_FindNextRec(HFScanDesc,record);

		while(recId1!=HFE_EOF){
				if(recId1>0){
					
					if((indexfd=AM_OpenIndex(argv[diaforetiko],recattr2.indexno))<0)
						AM_PrintError("JOIN");
				
					if((scanDescindex=AM_OpenIndexScan(indexfd,recattr2.attrtype,recattr2.attrlength,atoi(argv[argc-3]),record))<0)
						AM_PrintError("JOIN");

					recId2=AM_FindNextEntry(scanDescindex);
			
					while(recId2!=AME_EOF){
						if(recId2>0){//now the two records (inner and outer) must be placed in the file concatenated as one record
							if(HF_GetThisRec(rel1fd,recId1,record,recrel1.relwidth)<0)
									HF_PrintError("JOIN");
								
							interestingfields(&projectedrecord1,record,INF,atoi(argv[2]));

							if(HF_GetThisRec(rel2fd,recId2,record,recrel2.relwidth)<0)
								HF_PrintError("JOIN");

							interestingfields(&projectedrecord2,record,INF,atoi(argv[2]));

							sprintf(joinedprojectedrecord,"%s%s",projectedrecord1,projectedrecord2);
					
							if(HF_InsertRec(tempfd,joinedprojectedrecord,recsize)<0)
								HF_PrintError("JOIN");
						}
						recId2=AM_FindNextEntry(scanDescindex);
					}
			
					if(AM_CloseIndexScan(scanDescindex)<0)
						AM_PrintError("JOIN");								

						
				}//end of if(recId>0)
			
				recId1=HF_FindNextRec(HFScanDesc,record);
		}
		
			if(HF_CloseFileScan(HFScanDesc)<0)
				HF_PrintError("JOIN");		
	}

	//Second case
	else{//if relation 2 is not indexed
		if(recattr1.indexed==TRUE){//relation 1 becomes the inner

			if((HFScanDesc=HF_OpenFileScan(rel2fd,recrel2.relwidth,recattr2.attrtype,recattr2.attrlength,recattr2.offset,atoi(argv[argc-3]),NULL))<0)
					HF_PrintError("JOIN");
				
				recId2=HF_FindNextRec(HFScanDesc,record);
						
				while(recId2!=HFE_EOF){
					if(recId2>0){
						//interestingfields(&projectedrecord,record,INF,atoi(argv[2]));
					
						if((indexfd=AM_OpenIndex(argv[3],recattr2.indexno))<0)
							AM_PrintError("JOIN");
				
						if((scanDescindex=AM_OpenIndexScan(indexfd,recattr1.attrtype,recattr1.attrlength,atoi(argv[argc-5]),record))<0)
							AM_PrintError("JOIN");

						recId1=AM_FindNextEntry(scanDescindex);
			
						while(recId1!=AME_EOF){
							if(recId1>0){
								if(HF_GetThisRec(rel2fd,recId2,record,recrel2.relwidth)<0)
									HF_PrintError("JOIN");
								
								interestingfields(&projectedrecord2,record,INF,atoi(argv[2]));

								if(HF_GetThisRec(rel1fd,recId1,record,recrel1.relwidth)<0)
									HF_PrintError("JOIN");

								interestingfields(&projectedrecord1,record,INF,atoi(argv[2]));

								sprintf(joinedprojectedrecord,"%s%s",projectedrecord2,projectedrecord1);
					
								if(HF_InsertRec(tempfd,joinedprojectedrecord,recsize)<0)
									HF_PrintError("JOIN");
							}
							recId1=AM_FindNextEntry(scanDescindex);
						}
			
						if(AM_CloseIndexScan(scanDescindex)<0)
							AM_PrintError("JOIN");

						
					}//end of if(recId>0)
			
					recId2=HF_FindNextRec(HFScanDesc,record);
				}
		
				if(HF_CloseFileScan(HFScanDesc)<0)
					HF_PrintError("JOIN");				


		}	
		else{//relation 1 is not indexed

			if((HFScanDesc2=HF_OpenFileScan(rel2fd,recrel2.relwidth,recattr2.attrtype,recattr2.attrlength,recattr2.offset,atoi(argv[argc-3]),NULL))<0)
					HF_PrintError("JOIN");

			recId2=HF_FindNextRec(HFScanDesc2,record);
				
			while(recId2!=HFE_EOF){
				if(recId2>0){
                        		if((HFScanDesc1=HF_OpenFileScan(rel1fd,recrel1.relwidth,recattr1.attrtype,recattr1.attrlength,recattr1.offset,atoi(argv[argc-5]),NULL))<0)
						HF_PrintError("JOIN");

					recId1=HF_FindNextRec(HFScanDesc1,record);

                        		while(recId1!=AME_EOF){
						if(recId1>0){//now the two records (inner and outer) must be placed in the file concatenated as one record
							if(HF_GetThisRec(rel2fd,recId2,record,recrel2.relwidth)<0)
									HF_PrintError("JOIN");
								
							interestingfields(&projectedrecord2,record,INF,atoi(argv[2]));

							if(HF_GetThisRec(rel1fd,recId1,record,recrel1.relwidth)<0)
								HF_PrintError("JOIN");

							interestingfields(&projectedrecord1,record,INF,atoi(argv[2]));

							sprintf(joinedprojectedrecord,"%s%s",projectedrecord2,projectedrecord1);
					
							if(HF_InsertRec(tempfd,joinedprojectedrecord,recsize)<0)
								HF_PrintError("JOIN");
						}
						recId1=HF_FindNextRec(HFScanDesc1,record);
					}

					if(HF_CloseFileScan(HFScanDesc1)<0)
						HF_PrintError("JOIN");


				}//end of if(recId>0)

				recId2=HF_FindNextRec(HFScanDesc2,record);
			}//end of inner else

			if(HF_CloseFileScan(HFScanDesc2)<0)
				HF_PrintError("JOIN");

		}
	}// end of outer else	
	


	return 0;
}







int main(){
	
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

