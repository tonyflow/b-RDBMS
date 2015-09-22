#ifndef _DM_LIB_H
#define _DM_LIB_H

#include "askisi.h"

int calc(char *);
int dm_mymemcmp(void *,void *,char,int);
int operand(char *);
float selectivity(attrDesc,int,void *);
int DM_insert(int,char **);
int DM_delete(int,char **);
int DM_select(int,char **);
int DM_join(int,char **);
int DM_pipeline(int,char **);
int DM_selectivejoin(int,char **);

#endif
