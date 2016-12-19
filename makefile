#git commit -m "" --author="Ivan Galban Smith <i.galban@lab.matcom.uh.cu>"

.SUFFIXES:

.SUFFIXES: .c .o

VPATH = src : include : build
CC = gcc
OBJLAGS =  -pthread -c -o
OBJDEPENDS = head.o
EXECFLAGS = -pthread -o

all: warehouse producer consumer


warehouse: $(OBJDEPENDS)
	$(CC) $(EXECFLAGS) bin/$@ src/$@.c  build/head.o

producer: $(OBJDEPENDS)
	$(CC) $(EXECFLAGS) bin/$@ src/$@.c  build/head.o	

consumer: $(OBJDEPENDS)
	$(CC) $(EXECFLAGS) bin/$@ src/$@.c  build/head.o	

.c.o:
	$(CC) $(OBJLAGS) build/$@ $<

clean:
	rm build/* bin/*