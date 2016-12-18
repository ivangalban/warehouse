#git commit -m "" --author="Ivan Galban Smith <i.galban@lab.matcom.uh.cu>"

.SUFFIXES:

.SUFFIXES: .c .o

VPATH = src : include : build
CC = gcc
OBJLAGS =  -pthread -c -o
OBJDEPENDS = head.o
EXECFLAGS = -pthread -o

warehouse: $(OBJDEPENDS)
	$(CC) $(EXECFLAGS) exec/$@ src/$@.c  build/head.o

producer: $(OBJDEPENDS)
	$(CC) $(EXECFLAGS) exec/$@ src/$@.c  build/head.o	

.c.o:
	$(CC) $(OBJLAGS) build/$@ $<

clean:
	rm build/* exec/*