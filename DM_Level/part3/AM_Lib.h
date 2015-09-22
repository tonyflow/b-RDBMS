void AM_Init(void);
int AM_CreateIndex(char *fileName, int indexNo, char attrType, int attrLength);
int AM_InsertEntry(int fileDesc, char attrType, int attrLength, char *value, int recId);
int AM_DeleteEntry(int fileDesc, char attrType, int attrLength, char *value, int recId);
int AM_OpenIndexScan(int fileDesc, char attrType, int attrLength, int op, char *value);
int AM_FindNextEntry(int scanDesc);
int AM_CloseIndexScan(int scanDesc);
void AM_PrintError(char *errString);

/*******************************************************/

#define MAXSCANS 20
#define MAX_TREE_HEIGHT 200
#define AME_OK 0
#define AME_BFERROR -1
#define AME_STACKFULL -2
#define AME_RECEXISTS -3
#define AME_NOMORESCANS -4
#define AME_INVALIDSCAN -5
#define AME_NORECS -6
#define AME_EOF -7
#define AME_NOSUCHREC -8

/*******************************************************/

struct B_scan
{
	int used;
	char attrType;
	int attrLength;
	int op;
	char *value;
	int fileDesc;
	int lastBlock;
	int lastRec;
};

/*******************************************************/

int AM_errno;
struct B_scan scans[MAXSCANS];
