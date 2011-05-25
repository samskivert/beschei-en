/* Prototypes for functions defined in
card.c
 */

extern UWORD __chip SpadeData[7];

extern UWORD __chip HeartData[7];

extern UWORD __chip ClubData[7];

extern UWORD __chip DiamData[7];

extern struct Image SuitImages[4];

extern char * RankToStr[14];

extern char * SuitToStr[4];

void cardClassInit(struct RastPort * );

void cardRender(Card , struct RastPort * , long , long );

long cardCompareRank(Card , Card );

