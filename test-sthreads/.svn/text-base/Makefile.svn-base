PROGRAMS = test-create test-mutex test-join test-sleep test-time-slice test-dump test-sleep-dump escalonamento-01 escalonamento-02 escalonamento-03 test-crash 01mutex01 02mutex02 03mutex03 04mutex04 05mutex05 06monitor01 07monitor02 08monitor03 09monitor04 10monitor05 11escalonador01 12escalonador02 13escalonador03 escalonamento01 escalonamento02 escalonamento03 escalonamento04 escalonamento05 escalonamento06 escalonamento07 escalonamento08 escalonamento09 escalonamento10


INCLUDES = -I. -I$(srcdir) -I../include -I ../include
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(CFLAGS)
CC = gcc
CFLAGS = -g -O0 -Wall -m32 -pthread 
DEFS = -DHAVE_CONFIG_H
LIBSTHREAD = ../sthread_lib/libsthread.a

all: $(PROGRAMS)

%:	%.c
	$(COMPILE) ../sthread_lib/sthread_start.o -o $@ $< $(LIBSTHREAD)

clean: clean-PROGRAMS
	rm -f *.o

clean-PROGRAMS:
	@list='$(PROGRAMS)'; for p in $$list; do \
	  echo " rm -f $$p $$p.o"; \
	  rm -f $$p $$p.o ; \
	done


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
