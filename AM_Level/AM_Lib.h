#ifndef __AM_H__
#define __AM_H__

//error codes
#define AME_OK	0	//OK
#define AME_INDEXNO	-32	//The given indexNo already exists for some index
#define AME_FULLINDEXARRAY	-33	//The index array is full
//#define AME_FULLBLOCK	-34	//The block is full of records
#define AME_ATTRTYPEFAULT	-34	//Wrong attribute type given
#define AME_ENDOFINDEX	-35	//The array cannot be doubled any further, end reached
#define AME_RECNOTFOUND	-36	//The requested record was not found
#define AME_FULLSCANARRAY	-37//The open scans array is full
#define AME_WRONG_OP	-38//Wrong op number
#define AME_WRONGSCANDESC  -39//Wrong code for the open scans array
#define AME_INDEXSCANALREADYCLOSED -40 //The index scan has already terminated
#define AME_EOF	-41//Reached end of file


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


typedef struct amindex{//struct with information for each index

	int fileDesc;
	char *filename;//the name of the file we are indexing
	int indexNo;//sequential index number
	char attrType;//attribute type
	int attrLength;//attribute size in bytes
	int isfree;
	int recsize;//record size
	int maxrecs;//maximum number of records per bucket

}amindex;

typedef struct openscans{//struct for open scans

	int fileDesc;
	char attrType;
	int attrLength;
	int op;//comparison operator
	char *value;
	int bucket;//the bucket where the scan has stopped
	int recNo;//the last record that was scanned
	int isfree;
	
}openscans;

typedef unsigned int (*hash_function)(char*, unsigned int len);

unsigned int JSHash  (char* str, unsigned int len);


//global variables

int AM_errno;
amindex indexes[MAXINDEXES];
openscans scans[MAXSCANS];








#endif
