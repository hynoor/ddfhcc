#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "utils.c"

#define PORT 7890 // the port will be used for demo
#define MAXSIZE 10240

int fatal(unsigned char *message) {
  printf("Error occured: %s", message);
  return -1;
}

int forward_data(unsigned char* buffer) {
  int sockfd;
  int yes = 1;
  struct sockaddr_in dest_addr;

  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    fatal("in socket()");

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    fatal("setting socket option SO_REUSEADDR");


}
int main() {
  int sockfd, new_sockfd;
  struct sockaddr_in host_addr, client_addr;
  socklen_t sin_size;
  int recv_length=1, yes=1;
  char buffer[10240];

  printf("creating socket ...\n");
  /* create socket */
  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    fatal("in socket()");

  /* set socket options */
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    fatal("setting socket option SO_REUSEADDR");

  /* host address configuration */
  host_addr.sin_family = AF_INET;
  host_addr.sin_port = htons(PORT);  /* convert little endian(x86 processor on host) to big endian (network stream) */
  host_addr.sin_addr.s_addr = 0;     /* 0 denotes use local IP address */
  memset(&(host_addr.sin_zero), '\0', 8); /* zero the rest of the struct */

  /* binding socket address to socket */
  if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1)
    fatal("binding to socket");

  /* start off listening ... place to a backlog queue untill accepted */
  if (listen(sockfd, 5) == -1)
    fatal("listening on socket");
  
  printf("echo server started ...\n");
  while(1) { /* Accept loop */
    sin_size =sizeof(struct sockaddr_in);
    /* accept the incomming data from bound socket address */
    new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size); 
    if(new_sockfd == -1)
      fatal("accepting connection");
    printf("server: got connection from %s port %d \n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    recv_length = recv(new_sockfd, &buffer, 1024, 0);
    while((recv_length) > 0) {
      printf("RECV: %d bytes\n", recv_length);
      dump(buffer, recv_length);
      recv_length = recv(new_sockfd, &buffer, 1024, 0);
    }
    close(new_sockfd);
  } // End while
}
