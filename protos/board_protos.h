/* Prototypes for functions defined in
board.c
 */

extern NewStack InitStacks[22];

extern MoveInfo * reqTempMoves;

extern MoveInfo * reqSourceMoves;

extern MoveInfo * legalMoves;

extern Card * Deck1;

extern Card * Deck2;

extern Stack Stacks[22];

extern Stack * MouseDown;

extern Stack * MouseUp;

extern Stack * Discard[2];

extern Stack * MainSource[2];

extern Stack * AltSource[2];

extern Move currentMove;

extern long PlNo;

extern Point offsetPoint;

extern Point storePoint;

extern struct Layer * cardLayer;

extern Card windowCard;

void boardFree(void);

void boardDeleteLayer(struct Layer * );

void boardClassInit(struct Window * );

int boardClassReset(struct Window * );

void boardRender(struct Window * );

Stack * boardStackFromPoint(Point * );

void boardIllegalMove(struct RastPort * , Move * );

long boardHandleMouseDown(Point * , struct RastPort * );

void boardHandleMouseUp(Point * , struct RastPort * );

void boardHandleMouseMove(Point * , struct RastPort * );

void boardPrepareForMove(void);

void boardSwitchPlayer(struct RastPort * );

void boardApplyMove(Move * , struct RastPort * );

void boardRenderMax(struct RastPort * , LONG , LONG );

void boardRenderIndicator(struct RastPort * , LONG , LONG );

