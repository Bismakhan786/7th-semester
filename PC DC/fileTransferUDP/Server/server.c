#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SIZE 1024

void write_file(char filename[], int sockfd, struct sockaddr_in addr){
  FILE *fp;
  int n;  
  char buffer[SIZE];
  socklen_t addr_size;

  // Creating a file.
  fp = fopen(filename, "w");

  // for video file
  /*size_t vfsize;
  vfsize = recvfrom(sockfd, buffer, SIZE, 0, (struct sockaddr *)&addr, &addr_size);
  if(fp){
  
	fwrite(filename, vfsize, 1, fp);
	printf("file recieved successfully..\n");
  }
  else{
  	printf("Cannot open output file..\n");
  }*/
  
  // Receiving the data from text file and writing it into the file.
  while(1){

    n = recvfrom(sockfd, buffer, SIZE, 0, (struct sockaddr*)&addr, &addr_size);

    if (strcmp(buffer, "END") == 0){
      break;
      return;
    }

    printf("[RECEVING] Data: %s", buffer);
    fprintf(fp, "%s", buffer);
    bzero(buffer, SIZE);

  }

  fclose(fp);
  return;
}

int main(int argc, char *argv[]){

  // Defining the IP and Port
  char *ip = "127.0.0.1";
  int port = 8080;

  // Defining variables
  int server_sockfd;
  struct sockaddr_in server_addr, client_addr;
  char buffer[SIZE];
  int e;

  // Creating a UDP socket
  server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_sockfd < 0){
    perror("[ERROR] socket error");
    exit(1);
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  e = bind(server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (e < 0){
    perror("[ERROR] bind error");
    exit(1);
  }

  printf("[STARTING] UDP File Server started. \n");
  
  socklen_t addr_size;
  addr_size = sizeof(client_addr);
  char filename[2000];
  int n;
  n = recvfrom(server_sockfd, filename, sizeof(filename), 0, (struct sockaddr *)&client_addr, &addr_size);
  if (n == -1){
      perror("[ERROR] recieving data to the server.");
      exit(1);
  }
  printf("File recieved: %s\n", filename);
  write_file(filename, server_sockfd, client_addr);

  printf("[SUCCESS] Data transfer complete.\n");
  printf("[CLOSING] Closing the server.\n");

  close(server_sockfd);

  return 0;
}
