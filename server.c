#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


//include... thread2.c


void *client_handler(void *arg)  //modify this bad boi
{

    char msg[80];

    int sockfd;

    sockfd = *(int *)arg;

    if (read(sockfd, msg, 80) > 0) {
        msg[strlen(msg)-1] = '\0';
        printf("Received message: %s \n", msg);

        /* echo message back to client */
        write(sockfd, msg, strlen(msg));
    }

    close(sockfd);

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
  //printf("Before%s\n",fname);
  if (fname[0] == '/')
    fname++;

  //printf("%s\n",fname);
  return fname;
}

int main(int argc, char *argv[])
{
  const int backlog = 5;
  struct  sockaddr_in  server_addr;
  struct  sockaddr_in  client_addr;

  //thread id
  pthread_t tid;

  //sockets
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

  while (1) {
      /* wait for client to connect */
      listen(sockfd, backlog);
      //parse http request....in client_handler???
      //grabbing input
      char* input;
      char* path;
      char* fname;
      input = (char*)malloc(25*sizeof(char));
      for(int i=0; i<25;i++){
        scanf("%c",input+i);
      }

      //parse http request....in client_handler???
      //printf("%s",parse(input));
      path = parse(input);
      //printf("path:%s\n", path);
      //ensure well formatted request (later)
      fname=removeSlash(path);
      //printf("fname:%s\n", fname);

      //dettermine if target file exists = index.html
      if( access(fname, F_OK ) != -1 ) {        // file exists
        printf("hola");
      } else {    // file doesn't exist
        printf("triste");
      }

///
      long fsize;
      FILE *f = fopen("index.html", "rb");
      if (!f){
          perror("The file was not opened");
          exit(1);
      }
      else{
        printf("opened");
      }


      fsize = ftell(f);
      if (fsize == -1) {
          perror("The file size was not retrieved");
          exit(1);
      }
      rewind(f);

      char *msg = (char*) malloc(fsize);
      if (fread(msg, fsize, 1, f) != 1){
          perror("The file was not read\n");
          exit(1);
      }
      fclose(f);

///

      if (pthread_create(&tid, NULL, client_handler, (void *)&client_sockfd) < 0) {
          perror("Unable to create client thread");
          break;
      }

      //transmit contents of file to connection

  }

  /* Accept a connection */
  clientlen = sizeof(client_addr);
  client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &clientlen);
  printf("The socket is bound\n");

  if (client_sockfd == -1) {
      perror("Unable to accept client connection request");
  }

  //close the connection
  close(sockfd);

  return 0;
}
