#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>

int commandLine(int newsockfd, char *buffer);
int fileOper(int newsockfd, char *buffer);

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    // Initialize socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind and wait for connection
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    // Connect to client
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
        error("ERROR on accept");
    while (1) {
    	printf("Tell me what you want to do:\n");
    	printf("1. Use command line tool.\n");
    	printf("2. Read or write files.\n");
    	printf("You can use command exit to exit each mode.\n");
    	char mode[255];
    	fgets(mode , 255, stdin);
    	int mode_no = atoi(mode);
    	while (mode_no == 1 || mode_no == 2) {
    		if (mode_no == 1)
    			mode_no = commandLine(newsockfd, buffer);
    		if (mode_no == 2)
    			mode_no = fileOper(newsockfd, buffer);
    	}
    	system("clear");
    }    
    return 0;
}


// Use command line tool
int commandLine(int newsockfd, char *buffer) {
	printf("Please enter the command: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    if (strcmp(buffer, "exit\n") == 0)
    	return 0;
    // Send command
    if (write(newsockfd, "COMMAND LINE MODE\n", 255) < 0)
        error("ERROR writing to socket");
    if (write(newsockfd, buffer, strlen(buffer)) < 0)
        error("ERROR writing to socket");
    // Recieve output
    bzero(buffer, 256);
    while (1) {
     	if (read(newsockfd, buffer, 255) < 0)
       		error("ERROR reading from socket");
        if (strcmp(buffer, "END\n") == 0)
            break;
        printf("%s",buffer);
        bzero(buffer, 256);
    }
    return 1;
}

// Read or write files
int fileOper(int newsockfd, char *buffer) {
	printf("------------------------------------------\n");
	printf("| There are two commands: read and write |\n");
	printf("| READ is used to read files from the remote computer.\n");
	printf("| It automatically copy the file to the current location and open it with vim.\n");
	printf("|     $ read filePath\n");
	printf("| Write is used to write file to the remote computer.\n");
	printf("----------------------------------------------------------------------------------\n");
	// not finished yet.

	char command[256], *inst, *fileName, copyFileName[256];
	FILE *file_p;
	fgets(command, 255, stdin);
	inst = strtok(command, " ");
	fileName = strtok(NULL, " ");
	printf("%s\n", fileName);
	printf("%s\n", strtok(fileName, "\n"));
	bzero(copyFileName, 256);
	strcat(copyFileName, "copy_");
	strcat(copyFileName, fileName);
	printf("%s\n", copyFileName);
	bzero(buffer, 256);
	if (strcmp(inst, "exit\n") == 0) 
		return 0;

	// Read file
	else if (strcmp(inst, "read") == 0) {
		if (write(newsockfd, "READ MODE\n", 255) < 0)
        	error("ERROR writing to socket");
        if (write(newsockfd, fileName, 255) < 0)
        	error("ERROR writing to socket");
        file_p = fopen(copyFileName, "w");
        while (1) {
        	if (read(newsockfd, buffer, 255) < 0)
       		error("ERROR reading from socket");
        	if (strcmp(buffer, "END\n") == 0)
            	break;
        	fputs(buffer, file_p);
        	bzero(buffer, 256);
        }
        fclose(file_p);
        char vim[256];
        bzero(vim, 256);
        strcat(vim, "vim ");
        strcat(vim, copyFileName);
        printf("%s\n", vim);
        system(vim);
		return 2;
	}

	// Write file
	else if (strcmp(inst, "write") == 0) {
		return 2;
	}
 	else return 2;
}