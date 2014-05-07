CC=gcc
CLFLAGS= -g -lpthread
OBJS=pool_manager.o node_buffer.o checksum.o config.o packet_parse.o

.PHONY:clean all

EXECS=pool_test manager_buffer_test checksum_test config_test

all:$(EXECS)

config_test:config_test.c $(OBJS)
	$(CC) $(CLFLAGS) -o $@ $^


checksum_test:checksum_test.c $(OBJS)
	$(CC) $(CLFLAGS) -o $@ $^

manager_buffer_test:manager_buffer_test.c $(OBJS)
	$(CC) $(CLFLAGS) -o $@ $^

pool_test:$(OBJS) pool_test.c
	$(CC) $(CLFLAGS) -o $@ $^

%.o:%.c
	$(CC) $(CLFLAGS) -c $< -o $@


clean:
	rm -f *.o $(EXECS)
