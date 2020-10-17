#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int parse(const char* line)
{
    /* Find out where everything is */
    const char *start_of_path = strchr(line, ' ') + 1;
    const char *start_of_query = strchr(start_of_path, '?');
    const char *end_of_query = strchr(start_of_query, ' ');

    /* Get the right amount of memory */
    char path[start_of_query - start_of_path];
    char query[end_of_query - start_of_query];

    /* Copy the strings into our memory */
    strncpy(path, start_of_path,  start_of_query - start_of_path);
    strncpy(query, start_of_query, end_of_query - start_of_query);

    /* Null terminators (because strncpy does not provide them) */
    path[sizeof(path)] = 0;
    query[sizeof(query)] = 0;

    /*Print */
    printf("%s\n", query, sizeof(query));
    printf("%s\n", path, sizeof(path));
}

int main(int argc , char *argv[])
{

    int sockfd, n;
    struct sockaddr_in addr;
    char client_msg[80], server_msg[80];

    if (argc != 3) {
        printf("Usage: %s <ip-address> <port> \n", argv[0]);
        return -1;
    }

    /* Create socket */
    sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if (sockfd == -1) {
       perror("Could not create socket");
       return -1;
    }

    printf("Socket created fd: %d \n", sockfd);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    // Connect to remote server
    if (connect(sockfd, (struct sockaddr *)&addr , sizeof(addr)) < 0) {
        perror("connect failed. Error");
        return 1;
    }
}
