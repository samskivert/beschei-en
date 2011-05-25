/* Prototypes for functions defined in
opponent.c
 */

extern int tempStacks[9];

extern int skippedMoves;

extern int equivalentStacks;

extern int MoveHighest;

extern int EmptyStacks;

extern int Found;

extern int CurMainSource;

extern int CurAltSource;

extern int CurDiscard;

extern struct List * MoveSequence;

extern struct List Depths[20];

extern int DepthLens[20];

extern MoveInfo * reqMoveInfo;

extern int HypothesisDepth;

void opponentFreeVec(void * );

void opponentClassDestruct(void * );

int opponentClassInit(void);

void opponentPrintSequence(MoveInfo * );

void opponentPrintStacks(Stack * );

int opponentListLength(struct List * );

int opponentCheckWin(void);

int opponentNoPossibleFlips(struct RastPort * );

int opponentMakeRequiredMoves(struct RastPort * );

int opponentMakePossibleMoves(struct RastPort * );

void opponentApplyAnalysis(struct RastPort * );

void opponentStoreMoveSequence(MoveInfo * );

int opponentStatesEquivalent(MoveInfo * , MoveInfo * );

int opponentAlreadySeenMove(MoveInfo * , MoveInfo * , int );

void opponentAnalyzeHypothesis(MoveInfo * , MoveInfo * , int );

void opponentAnalyzeBoard(struct RastPort * );

void opponentMakeNextMove(struct RastPort * );

