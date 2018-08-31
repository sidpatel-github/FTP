#include "serverheader.h"

int main(int argc, char *argv[])
{
     int parentsocket, childsocket, clilen, pid;
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2) {
         fprintf(stderr,"Please provide the port as a command line argument\n");
         exit(1);
     }
    
     parentsocket = serverSetup(&serv_addr, argv[1]);
     clilen = sizeof(cli_addr);
     while (1) {
         childsocket = accept(parentsocket, (struct sockaddr *) &cli_addr, &clilen);
         if (childsocket < 0) 
             error("ERROR while opening child socket");
         pid = fork();
         if (pid < 0)
             error("There is some problem while forking the process");
         if (pid == 0)  {
             close(parentsocket);
             serveClient(childsocket);
             exit(0);
         }
         else close(childsocket);
     }
     return 0;
}
