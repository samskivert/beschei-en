NAME = Besch
INSTALLDIR = SYS:Games

CC = SC
LD = SLink
RM = Delete
CP = Copy
VERSION = 38

OBJS = resource.o card.o deck.o stack.o opponent.o board.o criterion.o move.o moveinfo.o menus.o main.o

all: $(OBJS)
	$(CC) LINK NODEBUG $(OBJS) PNAME $(NAME)
	$(RM) FORCE QUIET $(NAME).lnk

main.o: main.c
	$(CC) IGN=147 IGN=154 main.c

.c.o:
	$(CC) $*.c

clean:
	$(RM) FORCE QUIET $(OBJS)

genprotos:
	$(CC) GPROTO GPFILE=protos/resource_protos.h resource.c
	$(CC) GPROTO GPFILE=protos/criterion_protos.h criterion.c
	$(CC) GPROTO GPFILE=protos/move_protos.h move.c
	$(CC) GPROTO GPFILE=protos/moveinfo_protos.h moveinfo.c
	$(CC) GPROTO GPFILE=protos/card_protos.h card.c
	$(CC) GPROTO GPFILE=protos/stack_protos.h stack.c
	$(CC) GPROTO GPFILE=protos/opponent_protos.h opponent.c
	$(CC) GPROTO GPFILE=protos/board_protos.h board.c
	$(CC) GPROTO GPFILE=protos/deck_protos.h deck.c
	$(CC) GPROTO GPFILE=protos/menus_protos.h menus.c
	$(CC) GPROTO GPFILE=protos/main_protos.h main.c

install: all
	$(CP) $(NAME) $(INSTALLDIR)

bumprev:
	BumpRev $(VERSION) $(NAME)

revision: bumprev
	$(RM) FORCE QUIET $(NAME)_rev.i
