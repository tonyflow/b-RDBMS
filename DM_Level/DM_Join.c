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
	int HFScanDesc;    //anagnristiko saroshs tou relname
	int check;
	int recId;
	int scanDescattr,scanDescrel,scanDescindex;
	int indexfd;       //anagnoristiko arxeiou eurethriou (open file)
	int rel1fd;         //anagnoristiko arxeiou relname (open file)
	int tempfd;        //anagnoristiko prosorinou arxeiou eisagoghs ton proballomenon apotelesmaton
	char record[1024];
	char *projectedrecord;// eggrafh pou periexei mono ta insteresting fields
	char** utcargs;    //pinakas mesa ston opoion tha ftiaksoume ta orismata ths UT_Create
						//kai tha ton perasoume san deutero orisma se auth
	NOI *INF;
	int recsize=0; //megethos kathe probllomenhs eggrafhs
	int i,j,k;
	int found;
	
	
	//periptosh opou h select kalestike me sxesh.pedio
	if(argc<6){
		printf("TOO FEW ARGUMENTS\n");
		return 1;
	}
	
	//an yparxei <WHERE condition> tha prepei na elegksoume an to onoma_sxeshs_epiloghs
	//einai idio me to onoma ths proballomenhs sxeshs
	if(strcmp(argv[3],argv[argc-4])){
		printf("SELECTION'S RELNAME DIFFERS FROM PROJETED RELATIONS' NAMES");
		return 1;
	}
	
	

	//SKANARISMA TOU RELCAT
	//elegxos an to relname yparxei sto RELCAT
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

	
	//efoson o parser katalabenei pote to erotima einai typou epiloghs (kai kalei thn DM_Select()) auto shmenei oti
	//san orismata sthn synarthsh DM_Select() mpenoune MONO idia onomata proballomenon sxeseon
	//kai emeis apla prepei na elegxoume an ta pedia ton proballomenon sxeseon einai ontos pedia 
	//ton sxeseon meso mias saroshs sto ATTRCAT
	
	//me thn "for" anatrexoume OLA ta pedia ton proballomenon sxeseon
	//kai gia kathe ena apo auta elegxoume an yparxei sto ATTRCAT me skanarisma autou
	
	
	//prepei na dhmiourgisoume ton pinaka pou tha perasoume san
	//orisma sthn UT_Create().. dld ton pinaka utcargs[][]
	
	//desmeush xorou gia ton pinaka
	utcargs=malloc(2*atoi(argv[2])*sizeof(char**));
	
	for(i=0;i<argc;i++)
		utcargs[i]=malloc(256*sizeof(char));
		
	
	sprintf(utcargs[0],"%s","create");
	sprintf(utcargs[1],"%s",argv[3]);
	
	//desmeush xorou gia ton pinaka ton INF
	
	INF=malloc(atoi(argv[2])*sizeof(NOI));
	
	j=2;
	k=0;
	
	for(i=4;i<argc-2;i=i+2){
		
		found=0;
	
		//SKANARISMA TOU ATTRCAT
		if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[3]))<0)
			HF_PrintError("Problem while opening file scan");
		
		check=HF_FindNextRec(scanDescattr,(char*)recattr);
			
		while(check!=HFE_EOF){
			//brikame to probalomeno pedio san pedio tou relname
			if(strcmp(recattr->attrname,argv[i])==0){
				recsize+=recattr->attrlength;
				found=1;
				
				///parallhla kataskeuazoume kai ton pinaka orismaton ths UT_Create - enarksh kataskeuhs
				sprintf(utcargs[j],"%s",recattr->attrname);
				j++;
				if(recattr->attrtype=='c')
					sprintf(utcargs[j],"%c%d",recattr->attrtype,recattr->attrlength);
				else
					sprintf(utcargs[j],"%c",recattr->attrtype);
				j++;	
				//telos kataskeuhs
				
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

	
	//////TELOS ELEGXOU ORISMATON
	
	//anoigma tou relname
	if((rel1fd=HF_OpenFile(argv[3]))<0)
		HF_PrintError("SELECT");
		
	//dhmiourgia prosorinou arxeiou eisagoghs ton probalomenon apotelesmaton

	//periptosh opou den mas exei dothei to onoma kapoiou arxeiou gia na baloume
	//ta apotelesmata ths select
	if(argv[1]==NULL){
		//dhmiourgia tou arxeiou
		if(UT_Create(2*atoi(argv[2])+2,utcargs)<0){
			printf("Error while creating temporary file\n");
			return -1;
		}
	
		//anoigma tou arxeiou
		if((tempfd=HF_OpenFile("temp"))<0)
			HF_PrintError("SELECT");

	
	}

					
	//periptosh opou mas exei dothei to onoma enos arxeiou gia na baloume ta apotelesmata
	else{
		//dhmiourgia tou arxeiou
		if(UT_Create(2*atoi(argv[2])+2,utcargs)<0){
			printf("Error while creating output file %s\n",argv[1]);
			return -1;
		}
		
		//anoigma tou arxeiou
		if((tempfd=HF_OpenFile(argv[1]))<0)
			HF_PrintError("SELECT");

	}
	
	
	///////ENARKSH SELECT
	
	//PROTH PERIPTOSH : EXOUME <WHERE>
	
	//elegxoume thn thesh tou pinaka argv[] opou apothikeuete o telesths sygkrishs
	//an o deikths se authn thn thesh einai diaforetikos tou NULL tote exoume WHERE
	//diaforetika den exoume
	
	if(argv[argc-2]!=NULL){
	
		//pame na doume an to pedio ths synthikis einai INDEXED
		//SKANARISMA TOU ATTRCAT
		if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[3]))<0)
			HF_PrintError("Problem while opening file scan");
		
		check=HF_FindNextRec(scanDescattr,(char*)recattr);
			
		while(check!=HFE_EOF){
			if(strcmp(recattr->attrname,argv[argc-3])==0){
				break;
				//bgenoume apo to while exontas kratisei sto recattr ta stoixeia tou 
				//pediou pou mas endiaferei, ta stoixeia dld tou pediou sygkrishs
			}
			
			check=HF_FindNextRec(scanDescattr,(char*)recattr);
		}
		
		if(HF_CloseFileScan(scanDescattr)<0)
			HF_PrintError("SELECT");

		//PROTH PERIPTOSH proths perisptoshs : to pedio tou WHERE einai eurethriasmeno
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
	
		//DEUTERH PERIPTOSH proths periptoshs : to pedio tou WHERE DEN einai eurethriasmeno
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
	
	
	//DEUTERH PERIPTOSH : DEN EXOUME <WHERE>
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
	
	
	//an mas exei dothei onoma gia output file tote den ektyponoume tpt
	//an den mas exei dothei onoma prepei na ektyposoume tis eggrafes pou
	//exoume balei sto "temp"
	
	if(argv[1]==NULL){
		//skanarisma tou "temp" xoris synthiki gia ektyposh olon ton eggrafon
		if((HFScanDesc=HF_OpenFileScan(tempfd,recsize,0,0,0,0,NULL))<0)
			HF_PrintError("SELECT");
			
		recId=HF_FindNextRec(HFScanDesc,record);
		
		while(recId!=HFE_EOF)
			printf("%s",record);
	}	                                                                            
	
	
	//kleisimo tou relname
	if(HF_CloseFile(rel1fd)<0)
		HF_PrintError("SELECT");
		
	//kleisimo kai katastrofh ton prosorinon arxeion
	if(argv[1]==NULL){
		//kleisimo
		if(HF_CloseFile(tempfd)<0)
			HF_PrintError("SELECT");
		
		//katastrofh
		if(UT_Destroy(NULL,"temp")<0){
			printf("Error while destroying temporary file in select\n");
			return -1;
		}
	}
	
	else{
		//kleisimo
		if(HF_CloseFile(tempfd)<0)
			HF_PrintError("SELECT");
		
		//katastrofh
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
	int HFScanDesc,HFScanDesc1,HFScanDesc2;    //anagnoristiko saroshs tou relname
	int check;
	int recId1,recId2;
	int scanDescattr,scanDescrel,scanDescrel2,scanDescindex,scanDescattr1,scanDescattr2;
	int indexfd;       //anagnoristiko arxeiou eurethriou (open file)
	int rel1fd,rel2fd;         //anagnoristiko arxeiou relname (open file)
	int tempfd;        //anagnoristiko prosorinou arxeiou eisagoghs ton proballomenon apotelesmaton
	char record[1024];
	char *projectedrecord1,*projectedrecord2,*joinedprojectedrecord,;// eggrafh pou periexei mono ta insteresting fields
	char** utcargs;    //pinakas mesa ston opoion tha ftiaksoume ta orismata ths UT_Create
						//kai tha ton perasoume san deutero orisma se auth
	NOI *INF;
	int recsize=0; //megethos kathe probllomenhs eggrafhs
	int i,j,k;
	int found=0;
	int diaforetiko;
	
	
	//SKANARISMA TOU RELCAT
	//elegxos an to relname1 yparxei sto RELCAT
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
	
	
	for(i=5;i<=argc-7;i=i+2){//psaxnoume to onoma tis deuteris sxesis
	
		if(strcmp(argv[3],argv[i])!=0){
			diaforetiko=i;
			break;
		}
	}
	
	//SKANARISMA TOU RELCAT
	//elegxos an to relname2 yparxei sto RELCAT
	
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
	
		//SKANARISMA TOU ATTRCAT gia na doume an ta pedia ton orismaton antistoixoun stis sxeseis pou dinontai
		if((scanDescattr=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[i-1]))<0)
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
					sprintf(utcargs[j],"%c%d",recattr.attrtype,recattr.attrlength);
				else
					sprintf(utcargs[j],"%c",recattr.attrtype);
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
			HF_PrintError("JOIN : Error while closing scan");

		
		if(found==0){
			printf("REQUESTED FIELD FOR PROJECTION NOT FOUND IN RELCAT... RETYPE SELECTION USING DIFFERENT ARGUMENTS\n");
			return 1;
		}
		
	}
//TELOS ELEGXOU ORISMATON//////////////////////////////////////////////	

	if(strcmp(argv[4],argv[diaforetiko+1])==0){ // an ta pedia exoun to idio onoma, prepei kapws na ta ksexwrisoume
		sprintf(argv[4],"%s1",argv[4]);
		sprintf(argv[diaforetiko+1],"%s2",argv[diaforetiko+1]);
	}

	//anoigma tou argv[3]
	if((rel1fd=HF_OpenFile(argv[3]))<0)
		HF_PrintError("JOIN");
		
	//dhmiourgia prosorinou arxeiou eisagoghs ton probalomenon apotelesmaton

	//periptosh opou den mas exei dothei to onoma kapoiou arxeiou gia na baloume
	//ta apotelesmata ths select
	if(argv[1]==NULL){
		//dhmiourgia tou arxeiou
		if(UT_Create(2*atoi(argv[2])+2,utcargs)<0){
			printf("Error while creating temporary file\n");
			return -1;
		}
	
		//anoigma tou arxeiou
		if((tempfd=HF_OpenFile("temp"))<0)
			HF_PrintError("JOIN");

	
	}

					
	//periptosh opou mas exei dothei to onoma enos arxeiou gia na baloume ta apotelesmata
	else{
		//dhmiourgia tou arxeiou
		if(UT_Create(2*atoi(argv[2])+2,utcargs)<0){
			printf("Error while creating output file %s\n",argv[1]);
			return -1;
		}
		
		//anoigma tou arxeiou
		if((tempfd=HF_OpenFile(argv[1]))<0)
			HF_PrintError("JOIN");

	}

	//TSEKAROUME AN TA PEDIA TON DUO SXESEON EINAI EURETIRIASMENA
	if((scanDescattr1=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[3]))<0)
			HF_PrintError("Problem while opening file scan");
		
		check=HF_FindNextRec(scanDescattr1,(char*)&recattr1);
			
		while(check!=HFE_EOF){
			if(strcmp(recattr1.attrname,argv[argc-5])==0){//an brei to onoma tou pediou zeuksis 1
				break;
				//bgenoume apo to while exontas kratisei sto recattr ta stoixeia tou 
				//pediou pou mas endiaferei, ta stoixeia dld tou pediou sygkrishs
			}
			
			check=HF_FindNextRec(scanDescattr1,(char*)&recattr1);
		}
		
		if(HF_CloseFileScan(scanDescattr1)<0)
			HF_PrintError("JOIN");

	
	
	

	if((scanDescattr2=HF_OpenFileScan(attrfd,sizeof(attrDesc),'c',MAXNAME,0,EQUAL,argv[diaforetiko]))<0)
		HF_PrintError("Problem while opening file scan");
		
	check=HF_FindNextRec(scanDescattr2,(char*)&recattr2);
			
	while(check!=HFE_EOF){
		if(strcmp(recattr2.attrname,argv[argc-3])==0){//an brei to onoma pediou zeuksis 2
			break;
			//bgenoume apo to while exontas kratisei sto recattr ta stoixeia tou 
			//pediou pou mas endiaferei, ta stoixeia dld tou pediou sygkrishs
		}
			
		check=HF_FindNextRec(scanDescattr2,(char*)&recattr2);
	}
		
	if(HF_CloseFileScan(scanDescattr2)<0)
		HF_PrintError("JOIN");


	
	
//TORA KSEKINANE OI PERIPTOSEIS TIS JOIN GIA TA EYRETIRIA////////////////////////////////////

	
	//Proti periptosi
	if(recattr2.indexed==TRUE){//to 2 ginetai esoteriko xoris na mas noiazei ti einai to 1
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
						if(recId2>0){//tora prepei sto arxeio na mpoun oi duo eggrafes(esoterikou kai exoterikou) kollita os mia eggrafi
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

						
				}//telos tou if(recId>0)
			
				recId1=HF_FindNextRec(HFScanDesc,record);
		}
		
			if(HF_CloseFileScan(HFScanDesc)<0)
				HF_PrintError("JOIN");		
	}

	//Deuteri periptosi
	else{//an to 2 den einai euretiriasmeno
		if(recattr1.indexed==TRUE){//ginetai to 1 esoteriko

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

						
					}//telos tou if(recId>0)
			
					recId2=HF_FindNextRec(HFScanDesc,record);
				}
		
				if(HF_CloseFileScan(HFScanDesc)<0)
					HF_PrintError("JOIN");				


		}	
		else{//to 1 den einai euretiriasmeno

			if((HFScanDesc2=HF_OpenFileScan(rel2fd,recrel2.relwidth,recattr2.attrtype,recattr2.attrlength,recattr2.offset,atoi(argv[argc-3]),NULL))<0)
					HF_PrintError("JOIN");

			recId2=HF_FindNextRec(HFScanDesc2,record);
				
			while(recId2!=HFE_EOF){
				if(recId2>0){
                        		if((HFScanDesc1=HF_OpenFileScan(rel1fd,recrel1.relwidth,recattr1.attrtype,recattr1.attrlength,recattr1.offset,atoi(argv[argc-5]),NULL))<0)
						HF_PrintError("JOIN");

					recId1=HF_FindNextRec(HFScanDesc1,record);

                        		while(recId1!=AME_EOF){
						if(recId1>0){//tora prepei sto arxeio na mpoun oi duo eggrafes(esoterikou kai exoterikou) kollita os mia eggrafi
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


				}//telos tou if(recId>0)

				recId2=HF_FindNextRec(HFScanDesc2,record);
			}//end tou else tou mikrou

			if(HF_CloseFileScan(HFScanDesc2)<0)
				HF_PrintError("JOIN");

		}
	}// end tou else tou megalou	
	


	return 0;
}







int main(){
	
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

