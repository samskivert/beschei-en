/* Prototypes for functions defined in
main.c
 */

extern struct NewMenu BeschNewMenu[7];

extern long IDCMP_Flags;

extern long Width;

extern long Height;

extern long gameInProgress;

extern struct VisualInfo * VisInf;

extern struct Screen * Scr;

extern struct Window * Win;

extern struct Menu * Menus;

void appFreeVisualInfo(void * );

void appClearMenuStrip(void * );

void appFreeMenus(void * );

void appCloseWindow(void * );

void appCloseScreen(void * );

void appInfo(STRPTR );

void appExit(STRPTR );

void appStopGame(void);

void appStartGame(void);

int appHandleIDCMP(void);

void appInit(void);

void seedrng(void);

void main(int , char ** );

