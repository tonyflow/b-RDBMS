#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BF_Lib.h"

#define MAXSCANS 20

#define HFE_OK				0	/* OK */


#define HFE_RECSIZE			-25 /* record size exceeds block capacity */
#define HFE_RECDOESNOTEXIST -26 /* record does not exist */
#define HFE_INVALIDREC		-27	/* invalid record identifier */
#define HFE_INVALIDATTRTYPE -28 /* invalid attribute type */
#define HFE_INVALIDOP		-29 /* invalid operator */
#define HFE_STABFULL		-30	/* file scan array full */
#define HFE_NOMEM			-31	/* insufficient memory */
#define HFE_INVALIDSD		-32	/* invalid scan desriptor */
#define HFE_EOF				-33	/* previous scan reached end of file */
#define HFE_BFLAYERERROR	-34	/* error in BF */
#define HFE_FD				-35	/* invalid file descriptor */
#define HFE_FILEEXISTS		-36	/* file already exists */

/* Ορισμοί τελεστών σύγκρισης για τις σαρώσεις */
#define EQUAL					1
#define LESS_THAN				2
#define GREATER_THAN			3
#define LESS_THAN_OR_EQUAL		4
#define GREATER_THAN_OR_EQUAL	5
#define NOT_EQUAL				6

/* Πρωτότυπα συναρτήσεων */
void HF_Init(void);
int HF_CreateFile(char *fileName);
int HF_DestroyFile(char *fileName);
int HF_OpenFile(char *fileName);
int HF_CloseFile(int fileDesc);
int HF_InsertRec(int fileDesc, char *record, int recordSize);
int HF_DeleteRec(int fileDesc, int recId, int recordSize);
int HF_GetFirstRec(int fileDesc, char *record, int recordSize);
int HF_GetNextRec(int fileDesc, int recId, char *record, int recordSize);
int HF_GetThisRec(int fileDesc, int recId, char *record, int recordSize);
int HF_OpenFileScan(int fileDesc, int recordSize, char attrType, int attrLength, int attrOffset, int op, char *value);
int HF_FindNextRec(int scanDesc, char *record);
int HF_CloseFileScan(int scanDesc);
void HF_PrintError(char *errString);
