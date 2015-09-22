#include<stdio.h>
#include<stdlib.h>
#include <string.h>

main(){
	char nikos[]="nikos";
	int a=567,b;
	char value[256];
	char buffer[1024];

	printf("size of nikos is : %d\n",strlen(nikos));

	sprintf(buffer,"%s",nikos);

	sprintf(buffer+strlen(nikos),"%d",a);

	printf("buffer is : %s\n",buffer);	

	printf("size of buffer is : %d\n",strlen(buffer));

	//sscanf(buffer+sizeof(nikos),"%d",b);
	sprintf(value,"%d",buffer+sizeof(nikos));
	printf("value is %s\n",value);	



}
