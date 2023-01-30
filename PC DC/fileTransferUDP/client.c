#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SIZE 1024

void send_file_data(FILE *fp, char *filename, int sockfd, struct sockaddr_in addr){
  int n;
  char buffer[SIZE];


  /*fseek(fp, 0, SEEK_END);
  size_t vfsize = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  
  if(sendto(sockfd, filename, vfsize, 0, (struct sockaddr*)&addr, sizeof(addr))<0){
  	printf("File wasn't sent..\n");
  }
  else{
  	printf("File sent successfully..\n");
  }*/
  // Sending the data of a text file
  while(fgets(buffer, SIZE, fp) != NULL){
    printf("[SENDING] Data: %s", buffer);

    n = sendto(sockfd, buffer, SIZE, 0, (struct sockaddr*)&addr, sizeof(addr));
    if (n == -1){
      perror("[ERROR] sending data to the server.");
      exit(1);
    }
    bzero(buffer, SIZE);

  }

  // Sending the 'END'
  strcpy(buffer, "END");
  sendto(sockfd, buffer, SIZE, 0, (struct sockaddr*)&addr, sizeof(addr));
  
  fclose(fp);
  return;
}

int main(int argc, char *argv[]){

  // Defining the IP and Port
  char *ip = "127.0.0.1";
  int port = 8080;

  // Defining variables
  int server_sockfd;
  struct sockaddr_in server_addr;
  FILE *fp;
  char *filename = argv[1];
  
  
  // Creating a UDP socket
  server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_sockfd < 0){
    perror("[ERROR] socket error");
    exit(1);
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);
  
  printf("Name of the file to be transfer: %s\n", filename);

  int n;
  //sending file name to server
  n = sendto(server_sockfd, filename, strlen(filename), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (n == -1){
      perror("[ERROR] sending data to the server.");
      exit(1);
  }
  printf("Filename sent to server successfully.. \n\n");

  // Reading the text file
  fp = fopen(filename, "r");
  if (fp == NULL){
    perror("[ERROR] reading the file");
    exit(1);
  }

  // Sending the file data to the server
  send_file_data(fp, filename, server_sockfd, server_addr);

  printf("[SUCCESS] Data transfer complete.\n");
  printf("[CLOSING] Disconnecting from the server.\n");

  close(server_sockfd);
  return 0;
}
