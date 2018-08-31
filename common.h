#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

void error(char *msg)
{
    perror(msg);
    exit(1);
}
void errorW(char *msg)
{
    perror(msg);
}

int sendFile(char *filename, int sock, char typeOfHost)
{
	char sdbuf[LENGTH]; 
	int n;
	FILE* fp=fopen(filename, "r");
  
    	if (fp == NULL) {
    	    return 1;
    	}

        if(typeOfHost == 'S'){
	 n = write(sock,"Sending File from the server s.",31);
	}
	else if (typeOfHost == 'C'){
	 n = write(sock,"Sending File from the Client s.",31);
	}

	if (n < 0) 
	 error("ERROR writing to socket");
 
 	bzero(sdbuf, LENGTH); 
	int fs_block_sz; 
	while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fp)) > 0)
	{
	  if(send(sock, sdbuf, fs_block_sz, 0) < 0)
	  {
	    return 2;
	  }
	  bzero(sdbuf, LENGTH);
        }
	printf("File transfered Successfully!\n");
	fclose(fp);		
    	return 0;
}

int receiveFile(char* filename, int sockfd, char typeOfHost)
{
	char revbuf[LENGTH]; 
	char buffer[31]; 
        bzero(buffer,31);
	 	
	int n = read(sockfd, buffer, 31);
        if (n < 0) 
         error("ERROR reading from socket");
	
        if (strstr(buffer, "ERROR:File") != NULL)
	{
	  printf("%s\n",buffer);
	  return 3;
	}
	 
        // Receive File from Client
	if(typeOfHost == 'S'){
	printf("SERVER Says : ---------Receiving file from Client---------\n");
	}
	else if (typeOfHost == 'C'){
	printf("CLIENT Says : ---------Receiving file from Server---------\n");
	}
	
	FILE *fp = fopen(filename, "w+");
	
        if (fp == NULL)
      	 return 1;
        
	bzero(revbuf, LENGTH); 
	int fr_block_sz = 0;
	while((fr_block_sz = recv(sockfd, revbuf, LENGTH, 0)) > 0)
	{
		int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fp);
	        if(write_sz < fr_block_sz)
		{
		    return 2;
	        }
		bzero(revbuf, LENGTH);
		if (fr_block_sz == 0 || fr_block_sz != LENGTH) 
		{
		    break;
		}
	}
	if(fr_block_sz < 0)
  	{
		 return 3;
	}
	printf("File transfered Successfully!\n");
	fclose(fp);
	return 0;
}
#endif
