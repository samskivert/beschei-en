/* Prototypes for functions defined in
criterion.c
 */

extern long SuitMatrix[4][4];

extern int EmptyStackTried;

int NonEmptyTemp(Stack * );

int OwnNonEmptySource(Stack * );

int LegalSource(Stack * );

int LegalSourceUniq(Stack * );

int SuccSame(Stack * , Stack * );

int PredAlt(Stack * , Stack * );

int PredAltUniq(Stack * , Stack * );

int UnidirSame(Stack * , Stack * );

int ValidStack(Stack * );

int EndingMove(Stack * , Stack * );

