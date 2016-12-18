#include "../include/producer.h"

int *warehouses;

product_package *production;

int total_warehouse=0;
int total_products=0;
int id_product=0;
sem_t id_product_mutex;

void init(int argc ,char **argv)
{ 
    sem_init(&id_product_mutex,0,1);
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

    warehouses=(int*)malloc(total_warehouse*sizeof(total_warehouse));

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
            production[i-1].limit=tmp_int;
            sem_init(&production[i-1].slots_available,0,tmp_int);
        }
        else
        {
            warehouses[index]=open_clientfd(tmp_str,tmp_int);
            ++index;
        }
    }

    if(total_products==0)
    {
        total_products++;
        production=(product_package*)malloc(total_products*sizeof(product_package));
        production[0].products=(product*)malloc(sizeof(product));
        production[0].type=(char*)malloc(3*sizeof(char));
        production[0].type[0]='X';
        production[0].type[1]=0;
        production[0].limit=1;
        sem_init(&production[0].slots_available,0,0);
    }

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

int main(int argc, char **argv) 
{
    init(argc,argv);
    //checkinit();
    pthread_t producer_id;
    pthread_create(&producer_id,NULL,produce,NULL);
    

    pthread_join(producer_id,NULL);
    return 0;
}

