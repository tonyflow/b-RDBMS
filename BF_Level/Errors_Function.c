#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "BF_Lib.h"

void BF_PrintError(char *errString){
	
	printf("%s\n",errString);
	switch(BF_errno){
		case -1 : printf("No memory available\n");
			  break;
		case -2 : printf("No available space in middle memory\n");
			  break;
		case -3 : printf("Block already pinned on memory\n");
			  break;
		case -4 : printf("Block for unpinning not in memory\n");
			  break;
		case -5 : printf("Block already in memory\n");
			  break;
		case -6 : printf("General OS error\n");
			  break;
		case -7 : printf("Incomplete block read\n");
			  break;
		case -8 : printf("Incomplete block write\n");
			  break;
		case -9 : printf("Incomplete header block read\n");
			  break;
		case -10 : printf("Incomplete header block write\n");
			   break;
		case -11 : printf("Invalid block number\n");
			   break;
		case -12 : printf("File already open\n");
			   break;
		case -13 : printf("Full list of open files\n");
			   break;
		case -14 : printf("Invalid file descriptor\n");
			   break;
		case -15 : printf("End Of File\n");
			   break;
		case -16 : printf("Block already available\n");
			   break;
		case -17 : printf("Block already unpinned\n");
			   break;
		case -18 : printf("File already exists\n");
			   break;
		case -19 : printf("Problem while opening file\n");
			   break;
		case -20 : printf("Problem while deleting file\n");
			   break;
		case -21 : printf("Problem in BF_DestroyFile\n");
			   break;
		case -22 : printf("Problem with fclose\n");
			   break;
		case -23 : printf("Attempt to close file which contains pinned blocks\n");
			   break;
		case -24 : printf("No valid block found\n");
			   break;
		case -25 : printf("Dirty is neither 1 nor 0\n");
			   break;
		case -26 : printf("No free block found(all are valid)\n");
			   break;
		case -27 : printf("The block is already disposed\n");
			   break;
		default : printf("OK\n");
	}

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


