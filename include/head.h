#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define LISTENQ  1024  /* second argument to listen() */

typedef struct sockaddr SA;

void unix_error(char *msg);
void dns_error(char *msg);

/* POSIX semaphore wrappers */
void p(sem_t *sem);
void v(sem_t *sem);

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen);

/* Client/server helper functions */
int open_clientfd(char *hostname, int portno);
int open_listenfd(int portno);

/*Client/server function*/
int Open_clientfd(char *hostname, int port);
int Open_listenfd(int port);

