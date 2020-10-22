#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//connect() is in the client and we DON't HAVE select()

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
  //printf("Before%s\n",fname);
  if (fname[0] == '/')
    fname++;

  //printf("%s\n",fname);
  return fname;
}


void *client_handler(void *arg)
{

    char msg[100];
    int sockfd;
    sockfd = *(int *)arg;
    char* input;
    char* path;
    char* fname;


    if (read(sockfd, msg, 100) > 0)
    {
        msg[strlen(msg)-1] = '\0';
        //grabbing input
        path = parse(msg);
        //printf("path:%s\n", path);
        //ensure well formatted request (later)
        fname=removeSlash(path);
        //printf("fname:%s\n", fname);

        //dettermine if target file exists = index.html
        if( access(fname, F_OK ) != -1 ) {        // file exists
          printf("exists\n");
        } else {    // file doesn't exist
          printf("doesn't exists\n");
        }

  ///
        long fsize;
        FILE *f = fopen("index.html", "rb");
        if (!f){
            perror("The file was not opened\n");
            exit(1);
        }
        else{
          printf("opened\n");
        }

        char data[100];

        char *msg = (char*) malloc(fsize);
        while (fgets(data,100,f) != NULL){
          /* echo message back to client */
          printf("%s",data);
          write(sockfd, data, strlen(data));

        }
        printf("\n");
        //printf("Received message: %s \n", msg);
        fclose(f);
    }
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
