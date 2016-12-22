#include "../include/warehouse.h"

int limit, total_slots_available, port;
product_package *product_types;
product *buffer;
sem_t buffer_mutex;
int product_types_size;
int id_producer=1;

void print_status()
{

    printf("\n*************************************\n");
    int count;
 
    sem_getvalue(&product_types[0].slots_busy, &count);
    printf("Amount of products------> %d\n",  count);
    sem_getvalue(&product_types[0].slots_available, &count);
    printf("Slots available------> %d\n",  count);
    for (int i = 1; i < product_types_size; ++i)
    {
       
        sem_getvalue(&product_types[i].slots_busy, &count);
        printf("Number of products of type:%s ------> %d\n", product_types[i].type, count);
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
        index++;
    }

   
    product_types=(product_package*)malloc((argc-index+1)*sizeof(product_package));
    product_types_size=argc-index+1;
    

   
    for (int i=1; index < argc; ++index,++i)
    {
       
        product_types[i].type=(char*)malloc(5*sizeof(char));
        for (int j = 0; j < 5; ++j)
            product_types[i].type[j]=0;
        char *tmp=strtok(argv[index],":");
        sprintf(product_types[i].type,"%s",tmp);

        if(strlen(product_types[i].type)>3)
        {
            unix_error("incorrect type name");
        }
        product_types[i].limit=atoi(strtok(NULL,":"));
        sem_init(&product_types[i].slots_available,0,product_types[i].limit);
        sem_init(&product_types[i].slots_busy,0,0);
        tmp_limit+=product_types[i].limit;
    }


    if(limit==0)
        limit=tmp_limit;

    buffer=(product*)malloc(limit*sizeof(product));

    product_types[0].type=(char*)malloc(5*sizeof(char));
   
    //printf("%d\n",limit );


    sprintf(product_types[0].type,"auto\0");
    

    sem_init(&product_types[0].slots_available,0,limit);
    sem_init(&product_types[0].slots_busy,0,0);
    sem_init(&buffer_mutex,0,1);
    
    for (int i = 0; i < limit; ++i)
        buffer[i].product_id=-1;
}



void *thread(void *vargp) 
{  
    int connfd = *((int *)vargp);
    pthread_detach(pthread_self()); 
    //free(vargp);                  
   
    char buff[10];

    read(connfd, buff, sizeof(buff)); 
    if(strncmp("producer",buff,8)==0)
        producer_service(connfd);
    else if(strncmp("consumer",buff,8)==0)
        consumer_service(connfd);
        printf("asdasdsd\n");
    close(connfd);
    return NULL;
}
char okmessage[6]="OK\0";
char nomessage[6]="NO\0";

void producer_service(int connfd)
{
    char buff[5];

    struct sockaddr_in clientaddr;
    
    socklen_t clientlen = sizeof(clientaddr);
    
    while(1)
    {
        for (int i = 0; i < 5; ++i)
        {
            buff[i]=0;
        }
        
        
        
        read(connfd,buff,5); 

        int count=1;
       
        int limit_count=1;
        
        p(&buffer_mutex);
      
        int index_type;
        int index_product;

        for(index_type=product_types_size-1; index_type>0;--index_type)
            if(strncmp(buff,product_types[index_type].type,3)==0)
                break;
         
        if(index_type!=0)
        {
            sem_getvalue(&product_types[index_type].slots_available,&count); 
        }  

        sem_getvalue(&product_types[0].slots_available,&limit_count);
               
        if(count > 0 && limit_count>0)
        {
            write(connfd,okmessage,strlen(okmessage)+1);

            for (index_product = 0; index_product < limit; ++index_product)
            {
                if(buffer[index_product].product_id==-1)
                    break;
            }

            read(connfd,&buffer[index_product],sizeof(product));
            
            if(index_type!=0)
            {
                p(&product_types[index_type].slots_available);
                v(&product_types[index_type].slots_busy);
            }
           
            p(&product_types[0].slots_available);
            v(&product_types[0].slots_busy);
             print_status();
            printf("in\n");

        }    
        else
            write(connfd,nomessage,strlen(nomessage)+1);
        
        v(&buffer_mutex);
    }
}

void consumer_service(int connfd)
{
   char buff[5];

    struct sockaddr_in clientaddr;
    
    socklen_t clientlen = sizeof(clientaddr);
    
    while(1)
    {

        for (int i = 0; i < 5; ++i)
        {
            buff[i]=0;
        }
        
        int count=1;
       
        int limit_count=1;
        
        read(connfd,buff,5); 
        
        p(&buffer_mutex);
      
        int index_type;
        int index_product;

        for(index_type=product_types_size-1; index_type>0;--index_type)
            if(strncmp(buff,product_types[index_type].type,3)==0)
                break;
         
        if(index_type!=0)
        {
            sem_getvalue(&product_types[index_type].slots_busy,&count); 
        }  

        sem_getvalue(&product_types[0].slots_busy,&limit_count);
               
        if(count > 0 && limit_count>0)
        {
            write(connfd,okmessage,strlen(okmessage)+1);

            int is_auto=strncmp(buff,"auto",4);
            for (index_product = 0; index_product < limit; ++index_product)
            {
                if(buffer[index_product].product_id!=-1 && 
                    (is_auto || strncmp(buff,buffer[index_product].product_type,3)))
                    break;
            }

            write(connfd,&buffer[index_product],sizeof(product));
            buffer[index_product].product_id=-1;
            if(index_type!=0)
            {
                v(&product_types[index_type].slots_available);
                p(&product_types[index_type].slots_busy);
            }
           
            v(&product_types[0].slots_available);
            p(&product_types[0].slots_busy);
            
            print_status();
            printf("out\n");
            
        }    
        else
            write(connfd,nomessage,strlen(nomessage)+1);
        
        v(&buffer_mutex);
    }
}


int main(int argc, char **argv)
{

      init(argc,argv);

    int listenfd, *connfdp;
    socklen_t clientlen=sizeof(struct sockaddr_in);
    struct sockaddr_in clientaddr;
    pthread_t tid; 


    listenfd = open_listenfd(port);
    while (1) {

    connfdp = malloc(sizeof(int)); 
    *connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen);
   

     pthread_create(&tid, NULL, thread, connfdp);
    }

}