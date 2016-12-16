#include "../include/head.h"
#include "../include/warehouse.h"

int limit, total_amount, port;
product_box *product_types;
int product_types_size;

void check_init()
{
    printf("port------->%d\n",port );
    printf("limit------->%d\n",limit );
    printf("total_amount------->%d\n",total_amount );
    printf("product_types_size------->%d\n",product_types_size );


    for (int i = 0; i < product_types_size; ++i)
    {
        printf("type: %s ------------------> limit: %d \n", 
            product_types[i].type,product_types[i].limit);
    }
}
void init(int argc ,char **argv)
{ 
    int index=2,tmp_limit=0;
    port=atoi(argv[1]);
    if(strchr(argv[2],':')==NULL)
    {
        limit=atoi(argv[2]);
        index=3;
    }
    if(index<argc)
    {
        product_types=(product_box*)malloc((argc-index)*sizeof(product_box));
        product_types_size=argc-index;
    }
    for (int i=0; index < argc; ++index,++i)
    {
        product_types[i].type=strtok(argv[index],":");
        product_types[i].limit=atoi(strtok(NULL,":"));
        tmp_limit+=product_types[i].limit;
        product_types[i].amount=0;
    }
    if(limit==0)
    {
        limit=tmp_limit;
    }
}

int main(int argc, char **argv)
{
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp;
    init(argc,argv);
    check_init();
    
    exit(0);
}