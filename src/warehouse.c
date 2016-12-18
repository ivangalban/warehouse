#include "../include/warehouse.h"

int limit, total_slots_available, port;
product_package *product_types;
int product_types_size;

void check_init()
{
    printf("port------->%d\n",port );
    printf("limit------->%d\n",limit );
    printf("total_slots_available------->%d\n",total_slots_available );
    printf("product_types_size------->%d\n",product_types_size );


    for (int i = 0; i < product_types_size; ++i)
    {
        printf("type: %s ------------------> limit: %d \n", 
            product_types[i].type,product_types[i].limit);
    }
}
void init(int argc ,char **argv)
{ 
    //the number of arguments must be greater than 3
    if(argc<3)
    {
        unix_error(argv[0]);
    }

    //initializing values
    int index=2,tmp_limit=0;

    port=atoi(argv[1]);

    if(strchr(argv[2],':')==NULL)
    {
        limit=atoi(argv[2]);
        index=3;
    }

    //there are types
    if(index<argc)
    {
        product_types=(product_package*)malloc((argc-index)*sizeof(product_package));
        product_types_size=argc-index;
    }

    for (int i=0; index < argc; ++index,++i)
    {
        product_types[i].type=strtok(argv[index],":");
        if(strlen(product_types[i].type)>3)
        {
            unix_error("incorrect type name");
        }
        product_types[i].limit=atoi(strtok(NULL,":"));
        tmp_limit+=product_types[i].limit;
        product_types[i].slots_available=0;
    }

    if(limit==0)
        limit=tmp_limit;
    
}

int main(int argc, char **argv)
{
    int listenfd, *connfd, port;
    struct sockaddr_in clientaddr;
    init(argc,argv);
    check_init();
    listenfd = open_listenfd(port);
    socklen_t clientlen = sizeof(clientaddr);
    pthread_t tid;
    while (1) {

        connfd=malloc(sizeof(int));
        *connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        pthread_create(&tid,NULL,thread,connfd);
    }
    exit(0);
}

void *thread(void *vargp) 
{  
    int connfd = *((int *)vargp);
    pthread_detach(pthread_self()); 
    free(vargp);                  
   
    char buff[50];
    recv(client, buff, sizeof(buff), NULL); 
    if(strcmp("producer",buff)==0)
        producer_service(connfd);
    if(strcmp("consumer",buff))
        consumer_service(connfd);
    close(connfd);
    return NULL;
}

void producer_service(int connfd)
{

}

void consumer_service(int connfd)
{

}