.SUFFIXES:

.SUFFIXES: .c .o

VPATH = src : include : build
CC = gcc
OBJLAGS =  -pthread -c -o
OBJDEPENDS = head.o
EXECFLAGS = -pthread -o

warehouse: $(OBJDEPENDS)
	$(CC) $(EXECFLAGS) exec/$@ src/$@.c  build/$(OBJDEPENDS)

.c.o:
	$(CC) $(OBJLAGS) build/$@ $<

clean:
	rm build/* exec/*