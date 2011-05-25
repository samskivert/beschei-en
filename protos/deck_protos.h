/* Prototypes for functions defined in
deck.c
 */

void deckFree(Card * );

void deckInit(Card * );

Card * deckAlloc(long );

void deckShuffle(Card * , long );

void deckDeal(struct RastPort * );

