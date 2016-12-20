#include "../include/producer.h"

int *warehouses;
sem_t *warehouses_mutex;

product_package *prouction;

int total_warehouse=0;
int total_products=0;
int id_product=0;
sem_t id_product_mutex;

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
        prouction=(product_package*)malloc(total_products*sizeof(product_package));

    int index=0;

    for (int i=1; i < argc; ++i)
    {
       
        char *tmp_str=strtok(argv[i],":");
        int tmp_int=atoi(strtok(NULL,":"));

        if(i<=total_products)
        {
            prouction[i-1].products=(product*)malloc(tmp_int*sizeof(product));
            prouction[i-1].type=tmp_str;
            strcat(prouction[i-1].type,"\n\0");
            prouction[i-1].limit=tmp_int;
            sem_init(&prouction[i-1].slots_available,0,tmp_int);
            sem_init(&prouction[i-1].slots_busy,0,0);
        }
        else
        {
            sem_init(&warehouses_mutex[index],0,1);
            warehouses[index]=open_clientfd(tmp_str,tmp_int);

            printf("------------------->%d\n",warehouses[index] );
            
            char buff[10]="producer\n\0";
            printf("%d\n",strlen(buff) );
            printf("%s\n",buff );
            send(warehouses[index],buff,strlen(buff),0);
            ++index;
        }
    }

    if(total_products==0)
    {
        total_products++;
        prouction=(product_package*)malloc(total_products*sizeof(product_package));
        prouction[0].products=(product*)malloc(sizeof(product));
        prouction[0].type=(char*)malloc(3*sizeof(char));
        sprintf(prouction[0].type,"X\n\0");
        prouction[0].limit=1;
        sem_init(&prouction[0].slots_available,0,1);
        sem_init(&prouction[0].slots_busy,0,0);
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
        printf("type---------->%s    ",prouction[i].type);
        printf("limit---------->%d\n",prouction[i].limit);
    }
}

void make_item(int item)
{
    for (int j = 0; j < prouction[item].limit; ++j)
    {
        strncpy(prouction[item].products[j].product_type,prouction[item].type,3);
        
        p(&id_product_mutex);
        
        id_product++;
        prouction[item].products[j].product_id=id_product;
        
        v(&id_product_mutex);

       
        printf("New product: id----->%d   type-----> %s\n",
                        id_product,prouction[item].type);
    }
    prouction[item].current=0;
}

void *produce_item(void *vargp)
{
    int item = *((int *) vargp);
    
    // free(vargp);
    while(1)
    {
        for (int j = 0; j < prouction[item].limit; ++j)
            p(&prouction[item].slots_available);
        
        make_item(item);     
        
         for (int j = 0; j < prouction[item].limit; ++j)
            v(&prouction[item].slots_busy);
        
        sleep(1);
    }
}

void *produce1(void *vargp) /*tmp*/
{
    while(1)
    {
         
        for (int i = 0; i < total_products; ++i)
            for (int j = 0; j < prouction[i].limit; ++j)
                p(&prouction[i].slots_available);
        
        
        for (int i = 0; i < total_products; ++i)
               make_item(i);
       
         for (int i = 0; i < total_products; ++i)
            for (int j = 0; j < prouction[i].limit; ++j)
                v(&prouction[i].slots_busy);      
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

    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(clientaddr);
    char msg[5];
    for (int i = 0; ; ++i)
    {
    
        if(i>=total_warehouse)
            i=0;
        p(&prouction[item].slots_busy);
        p(&warehouses_mutex[i]);

        if((warehouses[i]!=-1)&&(getpeername(warehouses[i],(SA *)&clientaddr, &clientlen)!=-1))
        {
            // printf("ssssseeeeeeennnnnnndddddd1\n");
            send(warehouses[i],prouction[item].type,strlen(prouction[item].type),0);
            recv(warehouses[i],msg,5,0);
            if(strncmp(msg,"OK",2)==0)
            {
                int name = warehouses[i]%1000;
                sprintf(prouction[item].products[prouction[item].current].provider_id,"%d",name);
                printf("ssssseeeeeeennnnnnndddddd2\n");
                
                send(warehouses[i],&prouction[item].products[prouction[item].current],sizeof(product),0);
                prouction[item].current++;
                v(&prouction[item].slots_available);
            }
            else
            {
                v(&prouction[item].slots_busy);
            }
        }
        else
        {
              v(&prouction[item].slots_busy);
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
    pthread_t producer_id;
    pthread_create(&producer_id,NULL,produce,NULL);
  
    pthread_t store_id;
    pthread_create(&store_id,NULL,store,NULL);
    

    pthread_join(producer_id,NULL);
    pthread_join(store_id,NULL);
    return 0;
}

