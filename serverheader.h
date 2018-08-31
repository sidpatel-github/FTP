#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#define LENGTH 512
#define MAX_CLIENT 50
#include "common.h"
#ifndef SERVERHEADER_H_INCLUDED
#define SERVERHEADER_H_INCLUDED

int serverSetup(struct sockaddr_in *serv_addr, char * port)
{
     int portno, sockfd;
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     
     if (sockfd < 0) 
        error("ERROR : could not open parent socket");

     bzero((char *) serv_addr, sizeof(*serv_addr));
     portno = atoi(port);
     serv_addr->sin_family = AF_INET;
     serv_addr->sin_addr.s_addr = INADDR_ANY;
     serv_addr->sin_port = htons(portno);
     
     if (bind(sockfd, (struct sockaddr *) serv_addr, sizeof(*serv_addr)) < 0) 
        error("ERROR while binding the port");
     
     listen(sockfd, MAX_CLIENT);

     fprintf(stderr,"=========== server started ===========\n");
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
      n = write(sock, "Quit request from user", 18);
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
      token = strtok(NULL, " ");
      
      char * filename = malloc(sizeof(char) * (strlen(token) - 1));
      strncpy(filename, token, (strlen(token) - 1));

      char description[29] = " Server got your get command";
      strcat(token, description);

      n = write(sock, token, 150);
      if (n < 0) error("ERROR writing to socket");
      sleep(1);

      int stat = sendFile(filename, sock, 'S');
      free(filename);
      if (stat > 0) {
        n = write(sock, "ERROR:File Not Found on Server.", 31);
        if (n < 0) error("ERROR writing to socket");
        if (stat == 1)
          errorW("ERROR: File not found on server.\n");
        else if (stat == 2)
          errorW("ERROR: Failed to send file to client.\n");
        else
          errorW("ERROR:\n");

      }
      continue;

    } else {
      n = write(sock, "Server got your message", 24);
      if (n < 0) error("ERROR writing to socket");
    }
  }
}

#endif
