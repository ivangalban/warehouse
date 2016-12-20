#include "../include/consumer.h"

int *warehouses;
sem_t *warehouses_mutex;

product_package *taken;

int total_warehouse=0;
int total_products=0;

struct sockaddr_in clientaddr;
socklen_t clientlen = sizeof(clientaddr);




void init(int argc ,char **argv)
{ 
   




    for (int i = 1; i < argc; ++i)
    {
        char *p;
        for (int j = 0; j < strlen(argv[i]); ++j)
        {
            if(argv[i][j]==':' && j>3)
            {
                total_products=i-1;
                total_warehouse=argc-i;
                i=argc; //super break
                break;
            }
        }
    }

    warehouses=(int*)malloc(total_warehouse*sizeof(int));
    warehouses_mutex=(sem_t*)malloc(total_warehouse*sizeof(sem_t));
    if(total_products>0)
        taken=(product_package*)malloc(total_products*sizeof(product_package));

    int index=0;

    for (int i=1; i < argc; ++i)
    {
       
        char *tmp_str=strtok(argv[i],":");
        int tmp_int=atoi(strtok(NULL,":"));

        if(i<=total_products)
        {
            taken[i-1].products=(product*)malloc(tmp_int*sizeof(product));
            prouction[i-1].type=tmp_str;
            strcat(prouction[i-1].type,"\0");
            prouction[i-1].limit=tmp_int;
            sem_init(&prouction[i-1].slots_available,0,tmp_int);
            sem_init(&prouction[i-1].slots_busy,0,0);
        }
        else
        {
            sem_init(&warehouses_mutex[index],0,1);
            warehouses[index]=open_clientfd(tmp_str,tmp_int);
            char *buff="consumer\0";
            send(warehouses[index],buff,strlen(buff),0);

            ++index;
        }
    }

    if(total_products==0)
    {
        total_products++;
        taken=(product_package*)malloc(total_products*sizeof(product_package));
        taken[0].products=(product*)malloc(sizeof(product));
        taken[0].type=(char*)malloc(3*sizeof(char));
        sprintf(production[0].type,"auto\0");
        taken[0].limit=1;
        sem_init(&taken[0].slots_available,0,1);
        sem_init(&taken[0].slots_busy,0,0);
    }
}

void *receive_item(void *vargp)
{
    int item=*((int*)vargp);


    char msg[3];
    int count=0;

    for (int i = 0; count<taken[item].limit ; ++i)
    {
        if(i>=total_warehouse)
            i=0;
       
        p(&warehouses_mutex[i]);

        if((warehouses[i]!=-1)&&(getpeername(warehouses[i],(SA *)&clientaddr, &clientlen)!=-1))
        {

            send(warehouses[i],taken[item].type,strlen(taken[item].type),0);
            recv(warehouses[i],msg,strlen(msg),0);
            if(strcmp(msg,"OK")==0)
            {
                recv(warehouses[i],&taken[item].products[taken[item].current],sizeof(product),0);
                taken[item].current++;
                count++;
            }
        }
        
        v(&warehouses_mutex[i]);

    }
}

void consume()
{
    for (int i = 0; i < total_products; ++i)
    {
        for (int j= 0; j < taken[i].limit; ++j)
        {
            printf("\n");
            printf("/***********Product*************/\n");
            printf("Provider--------------->%s\n", taken[i].products[j].provider_id);
            printf("Product_ID------------->%d\n", taken[i].products[j].product_id);
            printf("Product_type----------->%s\n", taken[i].products[j].product_type);
            printf("/*******************************/\n");

            printf("\n");
            
        }
        taken[i].current=0;
    }
}



int main(int argc, char **argv) 
{
    init(argc,argv);
   
    pthread_t *id=(pthread_t*)malloc(total_products*sizeof(pthread_t));
    int *items=(int*)malloc(total_products*sizeof(int));
    while(1)
    {
        for (int i = 0; i < total_products; ++i)
        {
            item[i]=i;
            pthread_create(&id[i],NULL,receive_item, item+i);
        }
        
        for (int i = 0; i < total_products; ++i)
            pthread_join(id[i],NULL);

        consume();
        sleep(1);
    }
    
    
    return 0;
}

