#include "../include/consumer.h"

int *warehouses;
sem_t *warehouses_mutex;

int total_warehouse=0;
int total_products=0;
char ** types;
int *limit;
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
    {
        types=(char**)malloc(total_products*sizeof(char*));
        limit=(int*)malloc(total_products*sizeof(int));
    }

    int index=0;

    for (int i=1; i < argc; ++i)
    {
       
        char *tmp_str=strtok(argv[i],":");
        int tmp_int=atoi(strtok(NULL,":"));

        if(i<=total_products)
        {
            types[i-1]=tmp_str;
            strcat(types[i-1],"\0");
            limit[i-1]=tmp_int;
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
        types=(char**)malloc(sizeof(char*));
        limit=(int*)malloc(sizeof(int));
        *types=(char*)malloc(6*sizeof(char));

        sprintf(*types,"auto\0");

        *limit=1;
    }

}



product prd[1];

void *receive_item(void *vargp)
{
   
    int item=*((int*)vargp);


    char msg[6];
    char msg1[20];
    char msg2[20];
    char msg3[20];

    int count=0;


   
    for (int i = 0; count<limit[item] ; ++i)
    {
        
        if(i>=total_warehouse)
            i=0;
       
        p(&warehouses_mutex[i]);

        if((warehouses[i]!=-1)&&(getpeername(warehouses[i],(SA *)&clientaddr, &clientlen)!=-1))
        {
            write(warehouses[i],types[item],strlen(types[item])+1);
            read(warehouses[i],msg,6);         
            
            if(strncmp(msg,"OK",2)==0)
            {
                // printf("blblb\n");

            // read(warehouses[i],msg1,20);
            // read(warehouses[i],msg2,20);
            // read(warehouses[i],msg3,20);
            //     printf("Provider--------------->%s\n", msg1);
            //     printf("Product_ID------------->%s\n", msg2);
            //      printf("Product_type----------->%s\n", msg3);
                
                //count++;

                void *p=malloc(10*sizeof(product));
                char s[20];
                read(warehouses[i],s,10*sizeof(char));
                read(warehouses[i],p,10*sizeof(product));

                prd[0]=*((product*)p);
                printf("%s\n",s );
                printf("Provider--------------->%s\n", prd->provider_id);
                printf("Product_ID------------->%d\n", prd->product_id);
               printf("Product_type----------->%s\n", prd->product_type);
                
                count++;
            }
        }
        
        v(&warehouses_mutex[i]);

    }
}



int main(int argc, char **argv) 
{
    init(argc,argv);
   printf("%s\n","aaaaaaa" );
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

        sleep(1);
    }
    
    
    return 0;
}

