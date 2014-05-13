CC=gcc
CLFLAGS= -g -lpthread 
OBJS=pool_manager.o node_queue.o checksum.o config.o \
	 parse.o packet_generator.o packet_parser.o session_queue.o   \
	 packet_session.o

.PHONY:clean all

EXECS=simulation checksum_test  config_test  generator_test  \
	  manager_buffer_test pool_test

all:$(EXECS)

simulation:simulation.c $(OBJS)
	$(CC) $(CLFLAGS) -o $@ $^

session_queue_test:session_queue_test.c $(OBJS)
	$(CC) $(CLFLAGS) -o $@ $^

generator_test:generator_test.c $(OBJS)
	$(CC) $(CLFLAGS) -o $@ $^

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
