#ifndef PARSER3
#define PARSER3

#define MAXNAME 40
#define MAXSTRINGLENGTH 255
#define FALSE 0
#define TRUE 1
#define RESTART -3
#pragma pack(1)

typedef struct {
	int number_of_index;
	int offset;
	int length;
	char type;
} NOI;

//function declarations
int yyparse(void);


//global variables
int relfd,attrfd,viewfd,viewattrfd;




typedef struct {
	char relname[MAXNAME];	/* table name */
	int relwidth;		/* record width of table in bytes */
	int attrcnt;		/* number of record fields */
	int indexcnt;		/* number of table indexes */
	} relDesc; 


typedef struct {
	char relname[MAXNAME];	/* table name */
	char attrname[MAXNAME];	/* field name of the table */
	int offset;		/* distance of field start from the beginning of the record in bytes */
	int attrlength;		/* field length in bytes */
	char attrtype;		/* field type ('i', 'f', or 'c' */
	int indexed;		/* TRUE if the field has an index */
	int indexno;		/* sequential number of the index if indexed=TRUE */
	} attrDesc; 

typedef struct {
	char viewname[MAXNAME];
	short type;
	char relname1[MAXNAME];
	char attrname1[MAXNAME];
	int op;
	char relname2[MAXNAME];
	char attrname2[MAXNAME];
	char value[MAXSTRINGLENGTH];
	int attrcnt;
} viewDesc;

typedef struct {
        char viewname[MAXNAME];
        char viewattrname[MAXNAME];
        char relname[MAXNAME];
        char relattrname[MAXNAME];
} viewAttrDesc;




#endif
