
IDIR = ./include
CC = arm-none-eabi-gcc
CFLAGS = -mcpu=arm7tdmi -c -Wall -I/usr/include/uarm -I/usr/include -I/usr/include/bits -I$(IDIR)
LD = arm-none-eabi-ld
LDFLAGS = -T

_DEPS = mikabooq.h const.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

uarm_heads	= include/listx.h /usr/include/uarm/libuarm.h /usr/include/uarm/uARMconst.h /usr/include/uarm/uARMtypes.h /usr/include/uarm/arch.h

ODIR = build

_OBJ = mikabooq.o p1test.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

uarm_libs = /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x -o mikaboo /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o

all: mikaboo

mikaboo: $(OBJ)
	$(LD) $(LDFLAGS) $(uarm_libs)
	elf2uarm -k mikaboo

p1test: $(OBJ)
	$(LD) $(LDFLAGS) $(uarm_libs)
	elf2uarm -k p1test

$(ODIR)/p1test.o: p1test.c $(uarm_heads) $(DEPS)
	$(CC) -o $@ p1test.c $(CFLAGS)

$(ODIR)/mikabooq.o: mikabooq.c $(DEPS)
	$(CC) -o $@ mikabooq.c $(CFLAGS)

.PHONY: clean

clean:
	rm -rf *o mikaboo

cleanall:
	rm -rf *o mikaboo mikaboo.core.uarm mikaboo.stab.uarm
