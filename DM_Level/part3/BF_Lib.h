#ifndef _BF_LIB_H
#define _BF_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Errors
#define 	BFE_OK 	     0 	                   /* OK */
#define 	BFE_NOMEM 	    -1 	               /* out of memory */
#define 	BFE_NOBUF 	    -2 	               /* out of buffer space */
#define 	BFE_BLOCKFIXED 	    -3 	           /* block already pinned in memory */
#define 	BFE_BLOCKNOTINBUF 	    -4 	       /* block to unpin is not in memory */
#define 	BFE_BLOCKINBUF 	    -5 	           /* block already in memory */
#define 	BFE_OS 	    -6 	                   /* general OS error */
#define 	BFE_INCOMPLETEREAD 	    -7 	       /* incomplete block read */
#define 	BFE_INCOMPLETEWRITE 	    -8 	   /* incomplete block write */
#define 	BFE_INCOMPLETEHDREAD	    -9 	   /* incomplete header-block read */
#define 	BFE_INCOMPLETEHDWRITE 	  -10 	   /* incomplete header-block write */
#define 	BFE_INVALIDBLOCK 	  -11 	       /* invalid block identifier */
#define 	BFE_FILEOPEN 	  -12 	           /* file already open */
#define 	BFE_FTABFULL 	  -13 	           /* open file list is full */
#define 	BFE_FD 	  -14 	                   /* invalid file identifier */
#define 	BFE_EOF 	  -15 	               /* end of file */
#define 	BFE_BLOCKFREE 	  -16 	           /* block already available */
#define 	BFE_BLOCKUNFIXED 	  -17 	       /* block already unpinned */
#define     BFE_FILEDOESNOTEXIST   -18         /* file does not exist */
#define     BFE_FILEALREADYEXISTS   -19        /* file already exists */

//Block size
#define BF_BLOCK_SIZE 1024
#define MAXOPENFILES 25
#define BF_BUFFER_SIZE 20

//Masks
#define MASK0 128
#define MASK1 64
#define MASK2 32
#define MASK3 16
#define MASK4 8
#define MASK5 4
#define MASK6 2
#define MASK7 1

#define TRUE 1
#define FALSE 0

//Global variables
int BF_errno;//Variable that stores the error code
static int Time=0; //Variable used for the LRU algorithm
int m_table[8];//Mask table used in bitmap manipulation functions

//File header structure
struct header{
       char fname[50];//File name
       int num_of_blocks;//Number of blocks in the file
       char bitmaptable[970];//Bitmap
};
typedef struct header header;

//Structure pointed to by header_ptr
struct h_struct{
       header *ofhead;//Pointer to the header structure
       int openedNum;//Variable indicating how many times a file has been opened
       char fname[50];//Name of the open file
};
typedef struct h_struct h_struct;

//Open file structure
struct open_file{
       int isfree;//Variable indicating whether the table slot is occupied or empty
       FILE *fp;//File pointer
       h_struct *header_ptr;//Pointer to the h_struct structure
       int pinned_table[BF_BUFFER_SIZE];//Table containing the block numbers of this file that are pinned
};                          //in the buffer

typedef struct open_file open_file;
open_file of_table[MAXOPENFILES];//Open files table

//Block structure
struct block{
       int isfree;//Variable indicating whether the block table slot is occupied or empty
       char fname[50];//Name of the file this block belongs to
       int blockNum;//Block number in the file this block belongs to
       int fpinned_table[MAXOPENFILES];//Table corresponding to the open files table, where each slot contains
                             //a counter showing how many times this block has been pinned by a given file
       int isdirty;//Variable indicating whether the block is dirty, i.e. whether its contents have changed
       int ispinned;//Variable indicating whether the block is pinned in the buffer
       int lastAccess;//Variable indicating when the last access to this block occurred in the buffer, used for LRU
       char data[BF_BLOCK_SIZE];//Array containing the block data
};
typedef struct block block;
block buffer[BF_BUFFER_SIZE];//Block table -> buffer memory

//Function declarations
int HD_exist(char *filename);
int HD_create(char *filename);
int HD_remove(char *filename);
void make_valid(char *bitmap,int num_of_block);
void make_invalid(char *bitmap,int num_of_block);
int is_valid(char *bitmap,int num_of_block);
void BF_Init(void);
int BF_CreateFile(char *filename);
int BF_OpenFile(char *filename);
int BF_CloseFile(int fileDesc);
int BF_DestroyFile(char *filename);
int BF_PinBlock(int fileDesc,int blocknum,char **blockbuf);
int BF_AllocBlock(int fileDesc,int *blocknum,char **blockbuf);
int BF_DisposeBlock(int fileDesc,int *blocknum);
int BF_UnpinBlock(int fileDesc,int blocknum,int dirty);
int BF_GetThisBlock(int fileDesc,int blocknum,char **blockbuf);
int BF_GetNextBlock(int fileDesc,int *blocknum,char **blockbuf);   
int BF_GetFirstBlock(int fileDesc,int *blocknum,char **blockbuf);
void BF_PrintError(char *errstring);

#endif
