/* Prototypes for functions defined in
move.c
 */

extern struct List FreeMoves;

void moveClassDestruct(void * );

int moveClassInit(void);

void moveReallyFree(Move * );

Move * moveAlloc(int , int );

