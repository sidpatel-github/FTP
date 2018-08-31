#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <stdlib.h>
#define LENGTH 512
#include "common.h"
#ifndef CLIENTHEADER_H_INCLUDED
#define CLIENTHEADER_H_INCLUDED


int clientSetup(struct sockaddr_in *serv_addr, char * port, char * hostIp)
{
  int portno, sockfd;
  struct hostent * server;

  portno = atoi(port);
//fprintf(stderr,"0\n"); 
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
//fprintf(stderr,"0.1\n"); 
  if (sockfd < 0)
    error("ERROR opening socket");
  server = gethostbyname(hostIp);

  if (server == NULL) {
    fprintf(stderr, "ERROR, no host found\n");
    exit(0);
  }

//fprintf(stderr,  server -> h_length);
//fprintf(stderr,"before 1\n");     
  bzero((char * ) serv_addr, sizeof(*serv_addr));
//fprintf(stderr,"2\n");    
  serv_addr->sin_family = AF_INET;
//fprintf(stderr,"3\n");    
  bcopy((char * ) server->h_addr, (char * ) serv_addr->sin_addr.s_addr, server -> h_length);
//fprintf(stderr,"4\n");    
  serv_addr->sin_port = htons(portno);
//fprintf(stderr,"5\n");    
  if (connect(sockfd, (struct sockaddr * ) serv_addr, sizeof(*serv_addr)) < 0)
     error("ERROR while connecting with the Server");

 fprintf(stderr,"=========== client created ===========\n");     
 return sockfd;
}

void serveClient(int sock) {
  int n;
  char buffer[256];
  char * seperator;

  while (1) {
    bzero(buffer, 256);
    n = read(sock, buffer, 255);
    if (n < 0) error(" ERROR while reading from socket ");
    printf("\n Request from client:  %s\n", buffer);

    if (strstr(buffer, "quit") != NULL) {
      n = write(sock, "User has quit", 18);
      if (n < 0) error("ERROR while writing to socket");
      exit(0);
    } else if (strstr(buffer, "put ") != NULL) {

      //here we are extracting the file name by doing tokenzing
      char * token = strtok(buffer, " ");
      token = strtok(NULL, " ");
      
      char * filename = malloc(sizeof(char) * (strlen(token) - 1));
      strncpy(filename, token, (strlen(token) - 1));

      char description[29] = " Server got your put command ";
      strcat(token, description);

      n = write(sock, token, 150);
      if (n < 0) error("ERROR writing to socket");

      int stat = receiveFile(filename, sock, 'S');
      free(filename);
      if (stat > 0) {
        if (stat == 1)
          errorW("ERROR in opening file.\n");
        else if (stat == 2)
          errorW("ERROR: File write failed.\n");
        else if (stat == 3) {} else
          errorW("ERROR:\n");
      }

      continue;

    } else if (strstr(buffer, "get ") != NULL) {

      //here we are extracting the file name by doing tokenzing
      char * token = strtok(buffer, " ");
      token = strtok(NULL,