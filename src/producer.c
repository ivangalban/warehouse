#include "../include/producer.h"

int *warehouses;
sem_t *warehouses_mutex;

product_package *production;

int total_warehouse=0;
int total_products=0;
int id_product=0;
sem_t id_product_mutex;
char producer_id[5];

struct sockaddr_in clientaddr;
socklen_t clientlen = sizeof(clientaddr);

void init(int argc ,char **argv)
{ 
    producer_id[0]=mrand();
    producer_id[1]=mrand();
    producer_id[2]=mrand();
    producer_id[3]=0;

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
        production=(product_package*)malloc(total_products*sizeof(product_package));

    int index=0;

    for (int i=1; i < argc; ++i)
    {
       
        char *tmp_str=strtok(argv[i],":");
        int tmp_int=atoi(strtok(NULL,":"));

        if(i<=total_products)
        {
            production[i-1].products=(product*)malloc(tmp_int*sizeof(product));
            production[i-1].type=tmp_str;
            strcat(production[i-1].type,"\0");
            production[i-1].limit=tmp_int;
            sem_init(&production[i-1].slots_available,0,tmp_int);
            sem_init(&production[i-1].slots_busy,0,0);
        }
        else
        {
            sem_init(&warehouses_mutex[index],0,1);
            warehouses[index]=open_clientfd(tmp_str,tmp_int);            
            char *buff="producer\0";
            write(warehouses[index],buff,strlen(buff));
            ++index;
        }
    }

    if(total_products==0)
    {
        total_products++;
        production=(product_package*)malloc(total_products*sizeof(product_package));
        production[0].products=(product*)malloc(sizeof(product));
        production[0].type=(char*)malloc(3*sizeof(char));
        sprintf(production[0].type,"X\0");
        production[0].limit=1;
        sem_init(&production[0].slots_available,0,1);
        sem_init(&production[0].slots_busy,0,0);
    }

    sem_init(&id_product_mutex,0,1);

}

void checkinit()
{
    for (int i = 0; i < total_warehouse; ++i)
    {
        printf("warehouses-------->%d    \n",warehouses[i]);
    }
    printf("%d\n",total_products );
    for (int i = 0; i < total_products; ++i)
    {
        printf("type---------->%s    ",production[i].type);
        printf("limit---------->%d\n",production[i].limit);
    }
}

void make_item(int item)
{
    for (int j = 0; j < production[item].limit; ++j)
    {
        strncpy(production[item].products[j].product_type,production[item].type,3);
        
        p(&id_product_mutex);
        
        id_product++;
        production[item].products[j].product_id=id_product;
        
        v(&id_product_mutex);

       
        printf("New product: id----->%d   type-----> %s\n",
                        id_product,production[item].type);
    }
    production[item].current=0;
}

void *produce_item(void *vargp)
{
    int item = *((int *) vargp);
    
    // free(vargp);
    while(1)
    {
        for (int j = 0; j < production[item].limit; ++j)
            p(&production[item].slots_available);
        
        make_item(item);     
        
         for (int j = 0; j < production[item].limit; ++j)
            v(&production[item].slots_busy);
        
        sleep(1);
    }
}

void *produce1(void *vargp) /*tmp*/
{
    while(1)
    {
         
        for (int i = 0; i < total_products; ++i)
            for (int j = 0; j < production[i].limit; ++j)
                p(&production[i].slots_available);
        
        
        for (int i = 0; i < total_products; ++i)
               make_item(i);
       
         for (int i = 0; i < total_products; ++i)
            for (int j = 0; j < production[i].limit; ++j)
                v(&production[i].slots_busy);      
        sleep(1);
    }
}

void *produce2(void *vargp) /*tmp*/
{
    pthread_t *id=(pthread_t*)malloc(total_products*sizeof(pthread_t));
    for (int i = 0; i < total_products; ++i)
    {
        int *item=(int*)malloc(sizeof(int));
        *item=i;
        pthread_create(&id[i],NULL,produce_item, item);
    }
    for (int i = 0; i < total_products; ++i)
    {
        pthread_join(id[i],NULL);
    }
}
 
void *produce(void *vargp)
{
    produce1(vargp);
}

void *store_item(void *vargp)
{
    int item=*((int*)vargp);
    char msg[5];
    for (int i = 0; ; ++i)
    {
    
        if(i>=total_warehouse)
            i=0;
        p(&production[item].slots_busy);
        p(&warehouses_mutex[i]);

        if((warehouses[i]!=-1)&&(getpeername(warehouses[i],(SA *)&clientaddr, &clientlen)!=-1))
        {
            write(warehouses[i],production[item].type,strlen(production[item].type));
            read(warehouses[i],msg,5);
            if(strncmp(msg,"OK",2)==0)
            {
                sprintf(production[item].products[production[item].current].provider_id,"%s",producer_id);
                write(warehouses[i],&production[item].products[production[item].current],sizeof(product));
                production[item].current++;
                v(&production[item].slots_available);
            }
            else
            {
                v(&production[item].slots_busy);
            }
        }
        else
        {
              v(&production[item].slots_busy);
        }
            v(&warehouses_mutex[i]);
        
    }

}

void *store(void *vargp)
{
    pthread_t *id=(pthread_t*)malloc(total_products*sizeof(pthread_t));
    for (int i = 0; i < total_products; ++i)
    {
        int *item=(int*)malloc(sizeof(int));
        *item=i;
        pthread_create(&id[i],NULL,store_item, item);
    }
    for (int i = 0; i < total_products; ++i)
    {
        pthread_join(id[i],NULL);
    }
}

int main(int argc, char **argv) 
{
    init(argc,argv);
    //checkinit();
    pthread_t producer_tid;
    pthread_create(&producer_tid,NULL,produce,NULL);
  
    pthread_t store_tid;
    pthread_create(&store_tid,NULL,store,NULL);
    

    pthread_join(producer_tid,NULL);
    pthread_join(store_tid,NULL);
    return 0;
}

