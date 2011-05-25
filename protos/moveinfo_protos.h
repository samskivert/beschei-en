/* Prototypes for functions defined in
moveinfo.c
 */

extern struct List FreeMoveInfos;

extern long tmpStackIndexes[8];

void moveInfoClassDestruct(void * );

int moveInfoClassInit(void);

void moveInfoReallyFree(MoveInfo * );

void moveInfoFree(MoveInfo * );

MoveInfo * moveInfoAlloc(void);

int moveInfoContainsMove(MoveInfo * , Move * );

void moveInfoStrip(MoveInfo * );

void moveInfoInit(MoveInfo * );

void moveInfoApplyCriterion(MoveInfo * , SourceCriterionFunc , CriterionFunc );

void moveInfoSort(MoveInfo * );

MoveInfo * moveInfoCopyWithMove(MoveInfo * , Move * );

int moveInfoEmptyTempStacks(MoveInfo * );

