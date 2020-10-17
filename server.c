#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


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

      //parse HTTP request
      //check if we have a well-formed request (return error otherwise)
      //dettermine if target file exists = index.html?
      //transmit contents of file to connectio

  }

  //close the connection
  close(sockfd);

  return 0;
}
