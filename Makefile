# $Id: Makefile,v 39.2 1995/04/25 02:04:18 mbayne Exp mbayne $

NAME       = Besch
INSTALLDIR = SYS:Games/Misc

CC      = SC
LD      = SLink
RM      = Delete
CP      = Copy
VERSION = 38

OBJS1 = resource.o card.o deck.o stack.o opponent.o board.o criterion.o move.o
OBJS2 = moveinfo.o menus.o main.o
OBJS  = $(OBJS1) $(OBJS2)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) LINK NODEBUG $(OBJS) PNAME $(NAME)
	$(RM) FORCE QUIET $(NAME).lnk

main.o: main.c
	$(CC) IGN=147 IGN=154 main.c

.c.o:
	$(CC) $*.c

clean:
	$(RM) FORCE QUIET $(OBJS) $(NAME)

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
	$(CP) $(NAME).info $(INSTALLDIR)

bumprev:
	BumpRev $(VERSION) $(NAME)

revision: bumprev
	$(RM) FORCE QUIET $(NAME)_rev.i
