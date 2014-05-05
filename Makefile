CC=gcc
CLFLAGS= -g -lpthread
OBJS=pool_manager.o manager_buffer.o

.PHONY:clean all

EXECS=pool_test manager_buffer_test

all:$(EXECS)

manager_buffer_test:manager_buffer_test.c $(OBJS)
	$(CC) $(CLFLAGS) -o $@ $^

pool_test:$(OBJS) pool_test.c
	$(CC) $(CLFLAGS) -o $@ $^

%.o:%.c
	$(CC) $(CLFLAGS) -c $< -o $@


clean:
	rm -f *.o $(EXECS)
