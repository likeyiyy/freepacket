CC=gcc
CLFLAGS= -g -lpthread
OBJS=pool_manager.o

.PHONY:clean all
EXECS=pool_test
all:$(EXECS)

pool_test:$(OBJS) pool_test.c
	$(CC) $(CLFLAGS) -o $@ $^

%.o:%.c
	$(CC) $(CLFLAGS) -c $< -o $@


clean:
	rm -f *.o $(EXECS)
