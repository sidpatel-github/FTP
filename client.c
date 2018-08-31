
#include "clientheader.h"

int main(int argc, char * argv[]) {
  int parentsocket, portno, n;

  struct sockaddr_in serv_addr;
  struct hostent *server;

  char buffer[256];
  char clientDescription[50] = " Client Message : ";

  if (argc < 3) {
    fprintf(stderr, "Requied %s Hostname Port ClientName\n", argv[0]);
    exit(0);
  }

// parentsocket= clientSetup(&serv_addr, argv[2], argv[1]);

  portno = atoi(argv[2]);
  parentsocket = socket(AF_INET, SOCK_STREAM, 0);
  if (parentsocket < 0)
    error("ERROR opening socket");
  server = gethostbyname(argv[1]);
  if (server == NULL) {
    fprintf(stderr, "ERROR, no such host\n");
    exit(0);
  }
  bzero((char * ) & serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char * ) server -> h_addr,
    (char * ) & serv_addr.sin_addr.s_addr,
    server -> h_length);
  serv_addr.sin_port = htons(portno);
  if (connect(parentsocket, (struct sockaddr * ) & serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR connecting");

 fprintf(stderr,"=========== client created ===========\n"); 
  
  while (1) {
    printf("Enter the message: ");
    bzero(buffer, 256);
    fgets(buffer, 255, stdin);
    strcat(buffer, clientDescription);
    n = write(parentsocket, buffer, strlen(buffer));
    if (n < 0)
      error("ERROR writing to socket");
    bzero(buffer, 256);
    n = read(parentsocket, buffer, 255);
    if (n < 0)
      error("ERROR reading from socket");
    if (strstr(buffer, "Quit request from user") != NULL) {
      printf("%s\n", "Quit!");

      return 0;
    }
    if (strstr(buffer, "put command") != NULL) {
      printf("%s\n", "I have Requested put command");

      char * token = strtok(buffer, " ");
     
      char * filename;
      filename = strndup(token, (strlen(token) - 1));

      sleep(1);
      int stat = sendFile(filename, parentsocket, 'C');
      free(filename);

      if (stat > 0) {
        n = write(parentsocket, "ERROR:File Not Found on Client.", 31);
        if (n < 0) error("ERROR writing to socket");
        if (stat == 1)
          errorW("ERROR: File not found on Client.\n");
        else if (stat == 2)
          errorW("ERROR: Failed to send file to client.\n");
        else
          errorW("ERROR:\n");

      }
      continue;

    }
    if (strstr(buffer, "get command") != NULL) {
      printf("%s\n", "I have Reuested get command");
      char * token = strtok(buffer, " ");

      char * filename;
      filename = strndup(token, (strlen(token) - 1));

      int stat = receiveFile(filename, parentsocket, 'C');
      free(filename);

      if (stat > 0) {
        if (stat == 1)
