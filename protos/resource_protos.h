/* Prototypes for functions defined in
resource.c
 */

extern struct List Resources;

void resourceClassDestruct(int );

void resourceClassInit(void);

int resourceDestroy(void * );

void resourceAdd(void * , FREEFUNC );

