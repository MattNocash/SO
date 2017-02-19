
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

uarm_libs = /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o

all: mikaboo p1test

mikaboo: $(OBJ)
	$(LD) $(LDFLAGS) $(uarm_libs) -o $(patsubst %,$(ODIR)/%,mikaboo.elf) $(OBJ)
	elf2uarm -k $(patsubst %,$(ODIR)/%,mikaboo.elf)

p1test: $(OBJ)
	$(LD) $(LDFLAGS) $(uarm_libs) -o $(patsubst %,$(ODIR)/%,p1test.elf) $(OBJ)
	elf2uarm -k $(patsubst %,$(ODIR)/%,p1test.elf)

$(ODIR)/p1test.o: p1test.c $(uarm_heads) $(DEPS)
	$(CC) -o $@ p1test.c $(CFLAGS)

$(ODIR)/mikabooq.o: mikabooq.c $(DEPS)
	$(CC) -o $@ mikabooq.c $(CFLAGS)

.PHONY: clean

clean:
	rm -rf $(ODIR)/*o $(ODIR)/*elf

cleanall:
	rm -rf $(ODIR)/*o $(ODIR)/*elf $(ODIR)/*uarm
