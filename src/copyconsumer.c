#include "../include/consumer.h"

int *warehouses;
sem_t *warehouses_mutex;

product_package *product_types;

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
        product_types=(product_package*)malloc(total_products*sizeof(product_package));

    int index=0;

    for (int i=1; i < argc; ++i)
    {
       
        char *tmp_str=strtok(argv[i],":");
        int tmp_int=atoi(strtok(NULL,":"));

        if(i<=total_products)
        {
            product_types[i-1].products=(product*)malloc(tmp_int*sizeof(product));
            product_types[i-1].type=tmp_str;
            strcat(product_types[i-1].type,"\0");
            product_types[i-1].limit=tmp_int;
            product_types[i-1].current=0;
            sem_init(&product_types[i-1].slots_available,0,tmp_int);
            sem_init(&product_types[i-1].slots_busy,0,0);
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
        product_types=(product_package*)malloc(total_products*sizeof(product_package));
        product_types[0].products=(product*)malloc(sizeof(product));
        product_types[0].type=(char*)malloc(3*sizeof(char));
        sprintf(product_types[0].type,"auto\0");
        product_types[0].limit=1;
        product_types[0].current=0;
        sem_init(&product_types[0].slots_available,0,1);
        sem_init(&product_types[0].slots_busy,0,0);
    }
}




void *receive_item(void *vargp)
{
   
    int item=*((int*)vargp);


    char msg[5];
    int count=0;


   
    for (int i = 0; count<product_types[item].limit ; ++i)
    {
        
        if(i>=total_warehouse)
            i=0;
       
        p(&warehouses_mutex[i]);

        if((warehouses[i]!=-1)&&(getpeername(warehouses[i],(SA *)&clientaddr, &clientlen)!=-1))
        {
            send(warehouses[i],product_types[item].type,strlen(product_types[item].type)+1,0);
            recv(warehouses[i],msg,5,0);         
            if(strncmp(msg,"OK",2)==0)
            {
            //     printf("\n");
            // printf("/***********Product*************/\n");
            // printf("Provider--------------->%s\n", product_types[item].products[product_types[item].current].provider_id);
            // printf("Product_ID------------->%d\n", product_types[item].products[product_types[item].current].product_id);
            // printf("Product_type----------->%s\n", product_types[item].products[product_types[item].current].product_type);
            // printf("\n");
            // printf("/*******************************/\n");

               //  printf("%d\n", product_types[item].current);
                recv(warehouses[i],&product_types[item].products[product_types[item].current],sizeof(product),0);
            //     printf("\n");
            // printf("/***********Product*************/\n");
              printf("Provider--------------->%s\n", product_types[item].products[product_types[item].current].provider_id);
             // printf("Product_ID------------->%d\n", product_types[item].products[product_types[item].current].product_id);
              //printf("Product_type----------->%s\n", product_types[item].products[product_types[item].current].product_type);
            // printf("/*******************************/\n");

            // printf("\n");
                product_types[item].current++;
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
        for (int j= 0; j < product_types[i].limit; ++j)
        {
            printf("\n");
            printf("/***********Product*************/\n");
            printf("Provider--------------->%s\n", product_types[i].products[j].provider_id);
            printf("Product_ID------------->%d\n", product_types[i].products[j].product_id);
            printf("Product_type----------->%s\n", product_types[i].products[j].product_type);
            printf("/*******************************/\n");

            printf("\n");
            
        }
        product_types[i].current=0;
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
            items[i]=i;
            pthread_create(&id[i],NULL,receive_item, &items[i]);
        }
        
        for (int i = 0; i < total_products; ++i)
            pthread_join(id[i],NULL);

        printf("sadsasasadsd\n");
         //consume();
        sleep(1);
    }
    
    
    return 0;
}

