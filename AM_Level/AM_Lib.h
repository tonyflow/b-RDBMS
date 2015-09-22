#ifndef __AM_H__
#define __AM_H__

//kodikoi lathon
#define AME_OK	0	//OK
#define AME_INDEXNO	-32	//To indexno pou dinetai iparxei idi gia kapoio euretirio
#define AME_FULLINDEXARRAY	-33	//O pinakas euretirion exei gemisei
//#define AME_FULLBLOCK	-34	//To block exei gemisei eggrafes
#define AME_ATTRTYPEFAULT	-34	//Dothike lathos typos pediou
#define AME_ENDOFINDEX	-35	//Den mporei na diplasiastei allo o pinakas ara telos
#define AME_RECNOTFOUND	-36	//De brethike i eggrafi pou zitithike
#define AME_FULLSCANARRAY	-37//O pinakas anoixton saroseon einai gematos
#define AME_WRONG_OP	-38//Lathos arithmos op
#define AME_WRONGSCANDESC  -39//lathos kwdikos gia ton pinaka anoiktwn sarwsewn
#define AME_INDEXSCANALREADYCLOSED -40 //to index scan exei idi termatisei
#define AME_EOF	-41//ftasame se end of file


#define MAXINDEXES 25
#define MAXSCANS 20
#define MAX_DEPTH 8
#define BITSOFCHAR 8
#define NAMELENGTH 40

#define EQUAL 1
#define LESS_THAN 2
#define GREATER_THAN 3
#define LESS_THAN_OR_EQUAL 4
#define GREATER_THAN_OR_EQUAL 5
#define NOT_EQUAL 6


typedef struct amindex{//domi me plirofories gia kathe euretirio

	int fileDesc;	
	char *filename;//to onoma tou arxeiou pou euretiriazoume
	int indexNo;//auxon arithmos euretiriou
	char attrType;//tipos pediou
	int attrLength;//megethos pediou se bytes
	int isfree;
	int recsize;//megethos eggrafis
	int maxrecs;//megistos arithmos eggrafon se kathe kado

}amindex;

typedef struct openscans{//domi gia tis anoixtes saroseis

	int fileDesc;
	char attrType;
	int attrLength;
	int op;//telestis sigkrisis
	char *value;
	int bucket;//o kados ston opoio exei stamatisei i sarosi
	int recNo;//i teleutaia eggrafi pou sarothike
	int isfree;
	
}openscans;

typedef unsigned int (*hash_function)(char*, unsigned int len);

unsigned int JSHash  (char* str, unsigned int len);


//global metablites

int AM_errno;
amindex indexes[MAXINDEXES];
openscans scans[MAXSCANS];








#endif
