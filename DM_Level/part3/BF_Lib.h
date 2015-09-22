#ifndef _BF_LIB_H
#define _BF_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Errors
#define 	BFE_OK 	     0 	                   /* OK */
#define 	BFE_NOMEM 	    -1 	               /* έλλειψη μνήμης */
#define 	BFE_NOBUF 	    -2 	               /* έλλειψη χώρου ενδιάμεσης μνήμης */
#define 	BFE_BLOCKFIXED 	    -3 	           /* μπλοκ ήδη ``καρφωμένο'' στη μνήμη */
#define 	BFE_BLOCKNOTINBUF 	    -4 	       /* μπλοκ για ``ξεκάρφωμα'' δεν είναι στη μνήμη */
#define 	BFE_BLOCKINBUF 	    -5 	           /* μπλοκ ήδη στη μνήμη */
#define 	BFE_OS 	    -6 	                   /* γενικό σφάλμα Λειτουργικού Συστήματος */
#define 	BFE_INCOMPLETEREAD 	    -7 	       /* ατελής ανάγνωση μπλοκ */
#define 	BFE_INCOMPLETEWRITE 	    -8 	   /* ατελές γράψιμο σε μπλοκ */
#define 	BFE_INCOMPLETEHDREAD	    -9 	   /* ατελής ανάγνωση μπλοκ-κεφαλίδας */
#define 	BFE_INCOMPLETEHDWRITE 	  -10 	   /* ατελές γράψιμο σε μπλοκ-κεφαλίδα */
#define 	BFE_INVALIDBLOCK 	  -11 	       /* μη έγκυρος αναγνωριστικός αριθμός μπλοκ */
#define 	BFE_FILEOPEN 	  -12 	           /* αρχείο ήδη ανοιχτό */
#define 	BFE_FTABFULL 	  -13 	           /* λίστα ανοιχτών αρχείων πλήρης */
#define 	BFE_FD 	  -14 	                   /* μη έγκυρος αναγνωριστικός αριθμός αρχείου */
#define 	BFE_EOF 	  -15 	               /* τέλος αρχείου */
#define 	BFE_BLOCKFREE 	  -16 	           /* μπλοκ ήδη διαθέσιμο */
#define 	BFE_BLOCKUNFIXED 	  -17 	       /* μπλοκ ήδη ``ξεκαρφωμένο'' */
#define     BFE_FILEDOESNOTEXIST   -18         /* το αρχείο δεν υπάρχει */
#define     BFE_FILEALREADYEXISTS   -19        /* το αρχείο υπάρχει ήδη */

//Megethos Block
#define BF_BLOCK_SIZE 1024
#define MAXOPENFILES 25
#define BF_BUFFER_SIZE 20

//Maskes
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

//Global metablhtes
int BF_errno;//Metablhth pou apothikeuei ton kwdiko enos sfalmatos
static int Time=0; //Metablhth pou tha xrhsimopoihthei ston LRU
int m_table[8];//Pinakas maskwn pou tha xrhsimopoihthei se synarthseis epeksergasias tou bitmap

//Domh tou header tou arxeiou
struct header{
       char fname[50];//Onoma arxeiou
       int num_of_blocks;//Arithmos twn blocks tou arxeiou
       char bitmaptable[970];//Bitmap
};
typedef struct header header;

//Domh pou deixnei o header_ptr
struct h_struct{
       header *ofhead;//Deikths pros th domh header
       int openedNum;//Metablhth pou antistoixei sto poses fores exei anoixthei ena arxeio
       char fname[50];//Onoma tou anoiktou arxeiou 
};
typedef struct h_struct h_struct;

//Domh anoikto arxeio 
struct open_file{
       int isfree;//Metablhth pou deixnei an h thesh ston pinaka einai gemath h adeia
       FILE *fp;//Deikths sto arxeio
       h_struct *header_ptr;//Deikths pros th domh h_struct
       int pinned_table[BF_BUFFER_SIZE];//Pinakas pou periexei ta blocks tou arxeiou(tous arithmous tous) pou einai karfitswmena 
};                          //sthn endiamesh mnhmh

typedef struct open_file open_file;
open_file of_table[MAXOPENFILES];//Pinakas anoiktwn arxeiwn

//Domh block
struct block{
       int isfree;//Metablhth pou mas deixnei an h thesh ston pinaka twn blocks einai gemath h adeia
       char fname[50];//Onoma tou arxeiou apo to opoio proerxetai to block
       int blockNum;//Arithmos tou block sto arxeio apo to opoio proerxetai
       int fpinned_table[MAXOPENFILES];//Pinakas pou antistoixei ston pinaka anoiktwn arxeiwn kai pou kathe thesh tou periexei 
                             //ena metrhth pou deixnei poses fores exei karfitswthei to sygkekrimeno block apo kapoio arxeio
       int isdirty;//Metablhth pou mas deixnei an to block einai brwmismeno,dhladh an exei allaksei h oxi to periexomeno tou
       int ispinned;//Metablhth pou mas deixnei an to block einai karfitswmeno sthn endiamesh mnhmh
       int lastAccess;//Metablhth pou mas deixnei pote egine h teleutaia prospelash tou block sthn endiamesh mnhmh,xrhsimeuei ston LRU
       char data[BF_BLOCK_SIZE];//Pinakas pou periexei ta data tou block
};
typedef struct block block;
block buffer[BF_BUFFER_SIZE];//Pinakas twn blocks -> endiamesh mnhmh

//Dhlwseis synarthsewn
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
