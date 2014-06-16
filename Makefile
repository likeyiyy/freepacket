#Default Target.

ifeq ($(filter tile%,$(shell uname -m)),)


ifdef CROSS_COMPILING

ifdef TILERA_ROOT
$(error The 'TILERA_ROOT' enviroment variable is not set.)
else
CC = $(TILERA_ROOT)/bin/tile-gcc
MPIPE_CC = $(TILERA_ROOT)/bin/tile-mpipe-cc
endif

else
OBJS = display.o
CC = gcc
endif
else
CC = gcc
MPIPE_CC=mpipe-cc
endif

OPT = -Os
CFLAGS= -std=gnu99 -Wall -g 
OBJS += pool_manager.o node_queue.o checksum.o config.o \
	 parse.o packet_generator.o packet_parser.o session_queue.o   \
	 packet_manager.o 
LDFLAGS= -lncurses -lpthread 

OBJS += use_file.o cpuinfo.o meminfo.o hash.o \
		taskinfo.o top_list.o top_config.o ttop.o

.PHONY:clean all

EXECS=simulation 
	 
all:$(EXECS)

simulation:simulation.c $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f *.o $(EXECS)

