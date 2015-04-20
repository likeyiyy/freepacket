#Default Target.
ifeq ($(filter tile%,$(shell uname -m)),)
ifdef CROSS
#1.1交叉编译
ifdef TILERA_ROOT
CC = $(TILERA_ROOT)/bin/tile-gcc
MPIPE_CC = $(TILERA_ROOT)/bin/tile-mpipe-cc
LDFLAGS= -lpthread -lgxio -ltmc -lncurses
CFLAGS += -DTILERA_PLATFORM
OBJS += mpipe.o display.o
else
$(error The 'TILERA_ROOT' enviroment variable is not set.)
endif

else
#1.2编译X86平台上的
LDFLAGS= -lncurses -lpthread -lrt  -linac
OBJS = display.o
CFLAGS += -DINTEL_PLATFORM
CC = gcc
endif


#2 tilera平台编译tilera平台上的
else
CC = gcc
MPIPE_CC=mpipe-cc
LDFLAGS= -lpthread -lgxio -ltmc -lncurses
CFLAGS += -DTILERA_PLATFORM 
OBJS += mpipe.o display.o
endif

OPT = -Os
CFLAGS += -std=gnu99 -Wall 
OBJS += checksum.o config.o 						\
	 parse.o packet_generator.o packet_parser.o \
	 packet_manager.o  hash.o lossratio.o


.PHONY:clean all

EXECS=simulation 
	 
all:$(EXECS)

simulation:simulation.c $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -g 

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ -g


clean:
	rm -f *.o $(EXECS)

