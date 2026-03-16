#ifndef HF_LIB_H
#define HF_LIB_H

#include "BF_Lib.h"

#define EQUAL 1
#define LESS_THAN 2
#define GREATER_THAN 3
#define LESS_THAN_OR_EQUAL 4
#define GREATER_THAN_OR_EQUAL 5
#define NOT_EQUAL 6

#define HFE_OK 0					/* OK */
#define HFE_NOMEM -1				/* Out of memory */
#define HFE_NOBUF -2				/* No buffer space available */
#define HFE_BLOCKFIXED -3			/* Block already "pinned" in buffer */
#define HFE_BLOCKNOTINBUF -4		/* Block not "pinned" and not in buffer */
#define HFE_BLOCKINBUF -5			/* Block already in buffer */
#define HFE_OS -6					/* Operating system error */
#define HFE_INCOMPLETEREAD -7		/* Incomplete block read */
#define HFE_INCOMPLETEWRITE -8		/* Incomplete block write */
#define HFE_INCOMPLETEHDRREAD -9	/* Incomplete header-block read */
#define HFE_INCOMPLETEHDRWRITE -10  /* Incomplete header-block write */
#define HFE_INVALIDBLOCK -11	    /* Invalid block number */
#define HFE_FILEOPEN -12		    /* File already open */
#define HFE_FTABFULL -13			/* File table full */
#define HFE_FD -14					/* Invalid file descriptor */
#define HFE_EOF -15				    /* End of file */
#define HFE_BLOCKFREE -16			/* Block not pinned */
#define HFE_BLOCKUNFIXED -17        /* Block already "unpinned" */
#define HFE_FILE_EXISTS -18		    /* File already exists */
#define HFE_FILE_NOT_EXISTS -19	    /* File does not exist */
#define HFE_FOPEN_ERROR  -20		/* Error opening the file */
#define HFE_FILE_CLOSED -21		    /* File already closed */
#define HFE_INVALID_FILEDESC -22    /* Invalid file descriptor */
#define HFE_MALLOC_ERROR -23        /* malloc error */
#define HFE_FWRITE_ERROR -24		/* fwrite error */
#define HFE_FCLOSE_ERROR -25		/* fclose error */
#define HFE_NULL_POINTER -26		/* Null pointer */
#define HFE_FSEEK_ERROR -27         /* fseek error */
#define HFE_FREAD_ERROR -28			/* fread error */
#define HFE_FTELL_ERROR -29         /* ftell error */
#define HFE_REALLOC_ERROR -30		/* realloc error */
#define HFE_TOO_BIG_FILENAME -31	/* Filename too long */
#define HFE_SCAN_IN_PROGRESS -32	/* Scan already in progress */
#define HFE_INSERTION_FAILED -33	/* Record insertion failed */
#define HFE_INVALIDRECORD -34		/* Invalid record */
#define HFE_STABFULL -35			/* Scan table full */
#define HFE_INVALID_SCANDESC -36	/* Invalid scan descriptor */
#define HFE_INVALID_OP -37			/* Invalid comparison operator */
#define HFE_INVALID_ATTRTYPE -38	/* Invalid attribute type */
#define HFE_GETFIRSTREC_FAILED -39	/* GetFirstRec operation failed */
#define HFE_GETNEXTREC_FAILED -40	/* GetNextRec operation failed */
#define HFE_INVALID_RECID -41		/* Invalid record ID */

/* Global error variable */
int HF_errno;

/* Function prototypes */
void HF_Init(void);
int HF_CreateFile(char *filename);
int HF_DestroyFile(char *filename);
int HF_OpenFile(char *filename);
int HF_CloseFile(int fileDesc);
int HF_InsertRec(int fileDesc,char *record,int recordSize);
int HF_DeleteRec(int fileDesc,int recId,int recordSize);
int HF_GetFirstRec(int fileDesc,char *record,int recordSize);
int HF_GetNextRec(int fileDesc,int recId,char *record,int recordSize);
int HF_GetThisRec(int fileDesc,int recId,char *record,int recordSize);
int HF_OpenFileScan(int fileDesc,int recordSize, char attrType,int attrLength,int attrOffset,int op,char *value);
int HF_FindNextRec(int scanDesc,char *record);
int HF_CloseFileScan(int scanDesc);
void HF_PrintError(char *errString);

#endif
