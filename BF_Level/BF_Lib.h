
#ifndef __BF_H__ 
#define __BF_H__

#define BFE_OK 0            /* OK */
#define BFE_NOMEM -1        /* elleipsi mnimis */
#define BFE_NOBUF           -2      /* ellipsi xwrou endiamesisi mnimis */
#define BFE_BLOCKFIXED      -3      /* block idi "karfwmeno" sti mnimi */
#define BFE_BLOCKNOTINBUF           -4      /* block gia "ksekarfwma" den einai sti mnimi */
#define BFE_BLOCKINBUF      -5      /* block idi sti mnimi */
#define BFE_OS      -6      /* geniko sfalma Leitourgikou Sustimatos */
#define BFE_INCOMPLETEREAD          -7      /* atelis anagnwsi block */
#define BFE_INCOMPLETEWRITE         -8      /* ateles grapsimo se block */
#define BFE_INCOMPLETEHDRREAD       -9      /* atelis anagnwsi block-kefalidas */
#define BFE_INCOMPLETEHDRWRITE    -10       /* ateles grapsimo se block-kefalida */
#define BFE_INVALIDBLOCK          -11       /* mi egkiros anagnwristikos arithmos block */
#define BFE_FILEOPEN      -12       /* arxeio idi anoixto */
#define BFE_FTABFULL      -13       /* lista anoixtwn arxeiwn pliris */
#define BFE_FD    -14       /* mi egkuros anagnwristikos arithmos arxeiou */
#define BFE_EOF           -15       /* telos arxeiou */
#define BFE_BLOCKFREE     -16       /* block idi diathesimo */
#define BFE_BLOCKUNFIXED          -17       /* block idi "ksekarfwmeno" */
#define BFE_EXISTINGFILE        -18     /* to arxeio idi uparxei */
#define BFE_OPENFILE    -19     /* problima me anoigma arxeiou */
#define BFE_REMOVEFILE -20   /* problima me diagrafh arxeiou */
#define BFE_DESTROYERR -21  /* kodikos lathous stin destroyfile */
#define BFE_CLOSEFILE -22  /* problima me fclose */
#define BFE_PINNEDCLOSE -23  /* apopeira kleisimatos arxeiou eno exei pinned block sti mnimi */
#define BFE_NONVALIDBLOCK -24  /* de brethike egkiro block */
#define BFE_DIRTYFAULT -25  /* an to dirty den einai oute 0 oute 1 */
#define BFE_NOFREEBLOCK -26 /*de brethike eleuthero block(ola einai egkira)*/
#define BFE_ALREADY_DISPOSED -27 /*to block einai idi disposed*/


#define MAXOPENFILES 25
#define BF_BUFFER_SIZE 20
#define BF_BLOCK_SIZE 1024
#define NAMELENGTH 40

#define TRUE 1
#define FALSE 0



typedef struct openfile{

        char *filename;
        FILE *fp;
        int isfree;//logiki metabliti gia to an i thesi auti tou pinaka einai eleutheri
        int hd_index;//o arithmos tis thesis tou pinaka ton headers pou periexei to header tou arxeiou autou

}openfile;

typedef struct meminfo{
        int blockNum;//o arithmos tis thesis tou block sto arxeio
        long int lru_counter;
        int pinned_counter;//apo posous diaforetikous filedescs exei ginei to block pinned sti mnimi
        int dirty; // 0 an den exei allaksei, 1 an exei
        int isfree;//an einai keni i thesi
        char *filename; // to onoma tou arxeiou tou opoiou to block einai sti mnimi
        int *bytemap;   //enas pinakas 25 theseon,opou kathe thesi einai 0 i 1 analoga me to an i antistoixi klisi
                                        //ston pinaka anoixton arxeion exei kanei pin to block i oxi
}meminfo;

typedef struct header{
        char *headerblock;
        int counter;//posoi xrisimopoioun auto to header apo ton pinaka anoixton arxeion

}header;

//global metablites
openfile openfiles[MAXOPENFILES];//pinakas anoixton arxeion
meminfo memory[BF_BUFFER_SIZE];//pinakas me tis plirofories kathe block pou brisketai stin endiamesi mnimi
int BF_errno;
char *midmem;//endiamesi mnimi
header headers[MAXOPENFILES];//pinakas 25 theseon pou tha periexei ta headers se antistoixia me ton pinaka ton anoixton arxeion
long int lru;//kathe fora pou ena block tha xreiazetai stin endiamesi mnimi,tha auxanoume tin metabliti lru kata 1 kai tha tin apothikeuoume stin
	//antistoixi thesi ston pinaka meminfo


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

