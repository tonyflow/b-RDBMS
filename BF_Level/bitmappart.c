#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/* ---------------------- */

static void putbin(unsigned int i, FILE *f) {

if (i / 2) putbin(i / 2, f);
putc((i & 1) + '0', f);
} /* putbin */

/* ---------------------- */

//ta offset KAI sthn isavalid KAI sthn set diaxeirishs tou bitmap tha eimai apo 1 merxri 32 kai OXI apo 0 mexri 31

int isvalid(int bitmappart,int offset){
	
	putbin((bitmappart & (1 << (31 - (offset - 1)))),stdout);
	printf("\n");
	printf("offset is %d and (offset-1) is %d\n",offset,offset-1);
	
	if(((bitmappart & (1 << (31 - (offset - 1))))==0))
		return 0;	//an i AND praxi bgalei mono midenika,tote afou stin offset thesi eixame 1,sto bitmap tha exoume 0
	else
		return 1;
} 

void set(int* bitmappart,int offset,int what){
	if(what==1)
		 *bitmappart = *bitmappart | (1 << (31 - (offset - 1))); // apo 0 se 1 
	else
		*bitmappart = *bitmappart ^ (1 << (31 - (offset - 1))); 	// apo 1 se 0
}




main(){
	char* bitmap;
	int bitmappart;
	int i,offset;
	
	putbin(23,stdout);
	printf("\n");

	putbin(64,stdout);
	printf("\n");

	putbin(256,stdout);
	printf("\n");

	putbin(1024,stdout);
	printf("\n");


	bitmap=(char*)calloc(1024,sizeof(char));
	memcpy(&bitmappart,bitmap,sizeof(int));
	set(&bitmappart,3,1);
	putbin(bitmappart,stdout);
	printf("\n");
	set(&bitmappart,1,1);
	set(&bitmappart,5,1);
	set(&bitmappart,7,1);
	set(&bitmappart,8,1);
	set(&bitmappart,13,1);
	set(&bitmappart,14,1);
	set(&bitmappart,15,1);
	set(&bitmappart,25,1);
	set(&bitmappart,27,1);
	set(&bitmappart,28,1);
	set(&bitmappart,30,1);
	set(&bitmappart,31,1);
	set(&bitmappart,32,1);
	
	putbin(bitmappart,stdout);
	printf("\n");


	for(i=0;i<8;i++){
		printf("===============================================\n");
		printf("i+1 is %d\n",i+1);
		offset=i+1;
		if(isvalid(bitmappart,offset)==1)
			printf("Block %d is valid\n",i+1);
		else
			printf("Block %d is invalid\n",i+1);
	}
}

