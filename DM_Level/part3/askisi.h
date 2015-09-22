#ifndef _ASKISI_H
#define _ASKISI_H

#include "AM_Lib.h"
#include "HF_Lib.h"
#include "BF_Lib.h"

#define RELCAT "relCat"
#define ATTRCAT "attrCat"
#define MAXNAME 25

typedef struct {
	char relname[MAXNAME]; /* onoma pinaka */
	int relwidth;		   /* euros eggrafhs pinaka se bytes */
	int attrcnt;		   /* arithnos pediwn eggrafhs */
	int indexcnt;		   /* arithmos eurethriwn pinaka */
	int reccnt;		       /* arithmos egrafwn ths sxeshs */
} relDesc;	
	
typedef struct {
	char relname[MAXNAME];	/* onoma pinaka */
	char attrname[2*MAXNAME+sizeof(char)];	/* onoma pediou tou pinaka */
	int offset;		        /* apostash arxhs pediou apo thn arxh ths eggrafhs se bytes */
	int attrlength;		    /* mhkos pediou se bytes */
	char attrtype;		    /* typos pediou ('i', 'f', h 'c') */
	int valuecnt;		    /* arithmos diaforetikwn timwn pediou */
	int indexed;		    /* TRUE an to pedio exei eurethrio */
	int indexno;		    /* aukswn arithmos tou eurethriou an indexed=TRUE */
	char min[4];            /* mikroterh timh tou pediou */ 
	char max[4];            /* megaluterh timh tou pediou */
} attrDesc;

int relfileDesc;
int attrfileDesc;

#endif
