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

//orismos synarthseon
int yyparse(void);


//global metablhtes
int relfd,attrfd,viewfd,viewattrfd;




typedef struct {
	char relname[MAXNAME];	/* όνομα πίνακα */
	int relwidth;		/* εύρος εγγραφής πίνακα σε bytes */
	int attrcnt;		/* αριθμός πεδίων εγγραφής */
	int indexcnt;		/* αριθμός ευρετηρίων πίνακα */
	} relDesc; 


typedef struct {
	char relname[MAXNAME];	/* όνομα πίνακα */
	char attrname[MAXNAME];	/* όνομα πεδίου του πίνακα */
	int offset;		/* απόσταση αρχής πεδίου από την αρχή της εγγραφής σε bytes */
	int attrlength;		/* μήκος πεδίου σε bytes */
	char attrtype;		/* τύπος πεδίου ('i', 'f', ή 'c' */
	int indexed;		/* TRUE αν το πεδίο έχει ευρετήριο */
	int indexno;		/* αύξον αριθμός του ευρετηρίου αν indexed=TRUE */
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
