
#ifndef __BF_H__ 
#define __BF_H__

#define BFE_OK 0            /* OK */
#define BFE_NOMEM -1        /* lack of memory */
#define BFE_NOBUF           -2      /* lack of buffer memory space */
#define BFE_BLOCKFIXED      -3      /* block already "pinned" in memory */
#define BFE_BLOCKNOTINBUF           -4      /* block to be "unpinned" is not in memory */
#define BFE_BLOCKINBUF      -5      /* block already in memory */
#define BFE_OS      -6      /* general Operating System error */
#define BFE_INCOMPLETEREAD          -7      /* incomplete block read */
#define BFE_INCOMPLETEWRITE         -8      /* incomplete block write */
#define BFE_INCOMPLETEHDRREAD       -9      /* incomplete header block read */
#define BFE_INCOMPLETEHDRWRITE    -10       /* incomplete header block write */
#define BFE_INVALIDBLOCK          -11       /* invalid block identifier number */
#define BFE_FILEOPEN      -12       /* file already open */
#define BFE_FTABFULL      -13       /* open files list is full */
#define BFE_FD    -14       /* invalid file descriptor number */
#define BFE_EOF           -15       /* end of file */
#define BFE_BLOCKFREE     -16       /* block already available */
#define BFE_BLOCKUNFIXED          -17       /* block already "unpinned" */
#define BFE_EXISTINGFILE        -18     /* file already exists */
#define BFE_OPENFILE    -19     /* problem opening file */
#define BFE_REMOVEFILE -20   /* problem deleting file */
#define BFE_DESTROYERR -21  /* error code in destroyfile */
#define BFE_CLOSEFILE -22  /* problem with fclose */
#define BFE_PINNEDCLOSE -23  /* attempt to close file while it has pinned blocks in memory */
#define BFE_NONVALIDBLOCK -24  /* no valid block found */
#define BFE_DIRTYFAULT -25  /* dirty flag is neither 0 nor 1 */
#define BFE_NOFREEBLOCK -26 /*no free block found (all are valid)*/
#define BFE_ALREADY_DISPOSED -27 /*the block is already disposed*/


#define MAXOPENFILES 25
#define BF_BUFFER_SIZE 20
#define BF_BLOCK_SIZE 1024
#define NAMELENGTH 40

#define TRUE 1
#define FALSE 0



typedef struct openfile{

        char *filename;
        FILE *fp;
        int isfree;//boolean variable indicating whether this array position is free
        int hd_index;//the index in the headers array that contains this file's header

}openfile;

typedef struct meminfo{
        int blockNum;//the position number of the block in the file
        long int lru_counter;
        int pinned_counter;//how many different file descriptors have pinned this block in memory
        int dirty; // 0 if not modified, 1 if modified
        int isfree;//whether this position is empty
        char *filename; // the name of the file whose block is in memory
        int *bytemap;   //an array of 25 positions, where each position is 0 or 1 depending on whether the corresponding
                                        //entry in the open files array has pinned the block or not
}meminfo;

typedef struct header{
        char *headerblock;
        int counter;//how many entries in the open files array are using this header

}header;

//global variables
openfile openfiles[MAXOPENFILES];//open files array
meminfo memory[BF_BUFFER_SIZE];//array with information about each block in the buffer memory
int BF_errno;
char *midmem;//buffer memory
header headers[MAXOPENFILES];//array of 25 positions that will hold headers corresponding to the open files array
long int lru;//each time a block is needed in buffer memory, we increment the lru variable by 1 and store it in the
	//corresponding position in the meminfo array


void BF_Init();
int BF_CreateFile(char* fileName);
int BF_DestroyFile(char* fileName);
int BF_OpenFile(char *fileName);
int BF_CloseFile(int fileDesc);
int BF_GetFirstBlock( int fileDesc , int *blockNum , char **blockBuf);
int BF_GetNextBlock(int fileDesc , int *blockNum , char **blockBuf);
int BF_GetThisBlock(int fileDesc , int blockNum , char **blockBuf);
int BF_AllocBlock(int fileDesc , int *blockNum , char **blockBuf);
int BF_DisposeBlock(int fileDesc , int *blockNum);
int BF_UnpinBlock(int fileDesc , int blockNum , int dirty); 
void BF_PrintError(char* errString);

int isvalid(int bitmappart,int offset);
void set(int *bitmappart,int offset,int what);

int flru(int thesi,int valid_block,int fileDesc,int dirty_value,char** blockBuf);

#endif

