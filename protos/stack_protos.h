/* Prototypes for functions defined in
stack.c
 */

extern StackClass stackClasses[4];

void stackFree(Stack * );

void stackReset(Stack * );

void stackInit(Stack * , NewStack * , long );

Stack * stackAlloc(uchar );

long stackContainsPoint(Stack * , Point * );

void stackRender(Stack * , struct RastPort * , long );

void stackMakeTopCardFaceUp(Stack * , struct RastPort * );

Card stackRemoveCard(Stack * );

long stackCompare(Stack * , Stack * );

