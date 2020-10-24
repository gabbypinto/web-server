#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdbool.h>


int compare_str(char* first, char* second){
  while(*first == *second){
    if(*first == '\0' ||*second == '\0' ){
      break;
    }
    first++;
    second++;

  }

  if(*first == '\0' && *second == '\0'){
    return 0;
  }
  else{
    return -1;
  }
}

bool format_check(char* msg){
  int check=strcmp(".html HTTP/1.1", ".html HTTP/1.1");
  printf("CHeck:%d\n", check);
  char* get = "GET /";
  char* http = ".html HTTP/1.1";
  int getInt=0;
  int getHttp=0;

  printf("here\n");
  char *t=msg;
  for (t; *t != '\0';t++){
    int check2=compare_str(t, http);

    printf("%d\n",check2);
  }
  //printf("int check:\n%d", getHttp);
  if(getInt ==1&& getHttp==1)
    return true;
  return false;
}

char* parse(char* line)
{
  /* Find out where everything is */
    char *start_of_path = strchr(line, ' ') + 1;
    char *start_of_query = strchr(start_of_path, ' ');

    /* Get the right amount of memory */
    char path[start_of_query - start_of_path];
    //char query[end_of_query - start_of_query];


    /* Copy the strings into our memory */
    strncpy(path, start_of_path,  start_of_query - start_of_path);
    //strncpy(query, start_of_query, end_of_query - start_of_query);

    /* Null terminators (because strncpy does not provide them) */
    path[sizeof(path)] = 0;
    //query[sizeof(query)] = 0;

    /*Print */
    //printf("%s\n", query, sizeof(query));
    //printf("%s\n", path, sizeof(path));
    return path;
}

char* removeSlash(char *fname)
{
  printf("hi there: %c\n",*fname);
  char *fname2;
  //strncpy fname to fname2
  //start at 1
  // destination = file
  //   fname2=fname;
    //strncpy(fname2,&fname[1],strlen(fname)-1);
    for(int i=0; i<11;i++){
    //  printf("fname[i+1]%c\n", fname[i+1]);
      fname2[i]=fname[i+1];
    //  printf("fname2[1]%c\n", fname2[i]);
    //  printf("hello\n");
    }
  //printf("Before%s\n",fname);
  // if (fname2[0] == '/'){
  //   //fname[0]='\0';
  //   fname2[0]="\0";
  // }



printf("fname 2: %s\n",fname2);
  // printf("fname in function:%s\n",fname);
  // if(fname[strlen(fname)]!='\0')
  //   fname[strlen(fname)]='\0';
  return fname2;
}


void *client_handler(void *arg)
{

    char msg[100];
    char data[100];  //file data
    int sockfd;
    sockfd = *(int *)arg;
    char* input;
    char* path=NULL;
    char* fname=NULL;
    char* httpOK = "HTTP/1.1 200 OK\r\n";
    char* httpNotFound = "HTTP/1.1 404 Not Found\r\n";
    //msg[] = "GET /index.html HTTP/1.1";


//"<html><head></head><body><h1>404 Not Found</h1></body></html>\r\n"
    if (read(sockfd, msg, 100) > 0)
    {
        int n = 11;
        char* somechars;

        somechars = (char*)malloc(n*sizeof(char));
        msg[strlen(msg)-1] = '\0';
        format_check(msg);  //grabbing input
        path = parse(msg);  //parse...which returns /<htmlFileName>.html
        printf("path:%s\n", path);

        //returns fileName only...
        for(int i=0; i<12;i++){
          if (path[i+1] == '\0'){
              break;
          }
          printf("path[i+1]%c\n", path[i+1]);
          somechars[i] = path[i+1];
        }
        printf("%s\n",&path[1]);
        printf("somechars:%s \n",somechars);

        //determine if target file exists = index.html
        if( access(somechars, F_OK ) != -1 ) {        // file exists
          printf("exists\n");
        } else {                                      // file doesn't exist
          printf("doesn't exists\n");
        }

        long fsize;
        printf("fileName:%s",somechars);
        FILE *f = fopen(somechars, "rb");
        if (!f){
            perror("The file was not opened\n");
            write(sockfd,httpNotFound,strlen(httpNotFound));
            write(sockfd,"\r\n",2);
            //exit(1);
        }
        else{
          //send the header
          write(sockfd,httpOK,strlen(httpOK));
          write(sockfd,"\r\n",2);
          printf("opened\n");
        }

        char *msg = (char*) malloc(fsize);


        while (fgets(data,100,f) != NULL){
          /* echo message back to client */
          write(sockfd, data, strlen(data));

        }
        write(sockfd,"\r\n",2);
        printf("\n");
        //printf("Received message: %s \n", msg);
        fclose(f);
   }
   sleep(5);
   //man -s 2 read
   close(sockfd);
}

int main(int argc, char *argv[])
{

   const int backlog = 5;

   struct  sockaddr_in  server_addr;
   struct  sockaddr_in  client_addr;
   pthread_t tid;

   int sockfd, client_sockfd;
   int serverlen, clientlen;


   if (argc != 3) {
       printf("Usage: %s <ip-address> <port> \n", argv[0]);
       return -1;
   }

   /* Create the socket */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd == -1) {
       perror("Could not create socket");
       return -1;
   }

   /* Name the socket */
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = inet_addr(argv[1]);
   server_addr.sin_port = htons(atoi(argv[2]));

   /* bind to server socket */
   if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
       perror("Could not bind to socket");
       close(sockfd);
       return -1;
   }

   /* wait for client to connect */
   listen(sockfd, backlog);

   while (1) {

       /* Accept a connection */
       clientlen = sizeof(client_addr);
       client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &clientlen);
       if (client_sockfd == -1) {
           perror("Unable to accept client connection request");
           continue;
       }

       if (pthread_create(&tid, NULL, client_handler, (void *)&client_sockfd) < 0) {
           perror("Unable to create client thread");
           break;
       }

   }

   close(sockfd);

   return 0;

}
