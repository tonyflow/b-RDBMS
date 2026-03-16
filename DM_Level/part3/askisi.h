#ifndef _ASKISI_H
#define _ASKISI_H

#include "AM_Lib.h"
#include "HF_Lib.h"
#include "BF_Lib.h"

#define RELCAT "relCat"
#define ATTRCAT "attrCat"
#define MAXNAME 25

typedef struct {
	char relname[MAXNAME]; /* table name */
	int relwidth;		   /* record width in bytes */
	int attrcnt;		   /* number of fields per record */
	int indexcnt;		   /* number of indexes on the table */
	int reccnt;		       /* number of records in the relation */
} relDesc;	
	
typedef struct {
	char relname[MAXNAME];	/* table name */
	char attrname[2*MAXNAME+sizeof(char)];	/* field name of the table */
	int offset;		        /* distance of field start from the beginning of the record in bytes */
	int attrlength;		    /* field length in bytes */
	char attrtype;		    /* field type ('i', 'f', or 'c') */
	int valuecnt;		    /* number of distinct field values */
	int indexed;		    /* TRUE if the field has an index */
	int indexno;		    /* sequential number of the index if indexed=TRUE */
	char min[4];            /* minimum value of the field */
	char max[4];            /* maximum value of the field */
} attrDesc;

int relfileDesc;
int attrfileDesc;

#endif
