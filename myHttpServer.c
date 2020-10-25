
//Gabriela Pinto and Katherine Hansen
//2318655 and 2326665
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

bool format_check(char* msg){
  int n = 9;
  char* checkGet=(char*)malloc(n*sizeof(char));
  char* checkHttp=(char*)malloc(n*sizeof(char));
  char* get = "GET /";
  char* http = "1.1/PTTH";
  int i;

  //checking to see if beginning matches get
  for(i=0; i<5;i++){
    checkGet[i] = msg[i];
  }
  //comparing if they match
  if(strcmp(get,checkGet) != 0){
    return false;
  }
  //checking to see if end matches http
  int j = strlen(msg)-2;//24
  int count=0;
  for (;j>strlen(msg)-10;j--){
    checkHttp[count] = msg[j];
    count++;
  }
  //comparing if they match
  if(strcmp(http,checkHttp) != 0){
    return false;
  }
  //if both match, it is true
  return true;
}

char* parse(char* line)
{
  /* Find out where everything is */
    char *start_of_path = strchr(line, ' ') + 1;
    char *start_of_query = strchr(start_of_path, ' ');
    char path[start_of_query - start_of_path];
    //copying strings
    strncpy(path, start_of_path,  start_of_query - start_of_path);

    // Null terminators (because strncpy does not provide them)
    path[sizeof(path)] = 0;

    char* pathRet = path;
    return pathRet;
}


void *client_handler(void *arg)
{
    char msg[100];    //"GET /index.html HTTP/1.1";
    char data[100];  //file data
    int sockfd;
    sockfd = *(int *)arg;
    char* input;
    char* path=NULL;
    char* fname=NULL;
    char* httpOK = "HTTP/1.1 200 OK\r\n";
    char* httpNotFound = "HTTP/1.1 404 Not Found\r\n";
    char notFoundmsg[] = "<html><head><title>404 No File Found.</title></head><body><h1>404 FILE NOT FOUND.</h1</body></html>";

    if (read(sockfd, msg, 100) > 0)
    {
        int n = 11;
        char* somechars;
        somechars = (char*)malloc(n*sizeof(char));
        msg[strlen(msg)-1] = '\0';
        printf("%s", msg);


        bool format =format_check(msg);
        if(format==false){//check format of message
          //to run on a browser, comment out the 5 lines below :)
          printf("If using telnet, incorrect format\n");
          perror("incorrect format\n");
          printf("Exiting...\n");
          close(sockfd);
          exit(1);

        }

        path = parse(msg);  //parse...which returns /<htmlFileName>.html

        if(strcmp("/",path) == 0){
          somechars = "index.html";
        }
        else{
          //returns fileName only...
          for(int i=0; i<12;i++){
            if (path[i+1] == '\0'){
                break;
            }
            somechars[i] = path[i+1];
          }
        }

        //determine if target file exists (in GET command)
        if( access(somechars, F_OK ) != -1 ) {
          // file exists
          FILE *f = fopen(somechars, "rb");
          long fsize;
          //send the header
          write(sockfd,httpOK,strlen(httpOK));
          write(sockfd,"\r\n",2);

          while (fgets(data,100,f) != NULL){
            /* echo message back to client */
            write(sockfd, data, strlen(data));
          }
          fclose(f);

        } else {
          // file doesn't exist
          //send the HTML header
          write(sockfd,httpNotFound,strlen(httpNotFound));
          write(sockfd,"\r\n",2);
          //404 HTML Message
          write(sockfd,notFoundmsg,strlen(notFoundmsg));
        }

        write(sockfd,"\r\n",2);
        printf("\n");
   }
   sleep(60);
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
