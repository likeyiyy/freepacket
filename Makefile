#Default Target.

ifeq ($(filter tile%,$(shell uname -m)),)


ifdef CROSS

ifdef TILERA_ROOT
CC = $(TILERA_ROOT)/bin/tile-gcc
MPIPE_CC = $(TILERA_ROOT)/bin/tile-mpipe-cc
LDFLAGS= -lpthread 
else
$(error The 'TILERA_ROOT' enviroment variable is not set.)
endif

else
LDFLAGS= -lncurses -lpthread -lrt 
OBJS = display.o
CC = gcc
endif

else
CC = gcc
MPIPE_CC=mpipe-cc
endif

OPT = -Os
CFLAGS= -std=gnu99 -Wall -g -O3
OBJS += pool_manager.o parser_queue.o checksum.o config.o \
	 parse.o packet_generator.o packet_parser.o manager_queue.o   \
	 packet_manager.o  hash.o


.PHONY:clean all

EXECS=simulation 
	 
all:$(EXECS)

simulation:simulation.c $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	git add .

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	git rm -f *.o $(EXECS)
	git add .

