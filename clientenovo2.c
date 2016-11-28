/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#define PORT "202" // the port client will be connecting to 
#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:

/*
      STRUCT ADDRINFO SOH PARA CONSULTA E MAIOR ENTENDIMENTO DO CODIGO
struct addrinfo {
    int              ai_flags;
    int              ai_family;
    int              ai_socktype;
    int              ai_protocol;
    socklen_t        ai_addrlen;
    struct sockaddr *ai_addr;
    char            *ai_canonname;
    struct addrinfo *ai_next;
};
*/

char comando[100];
char myid = '1';
//---------------------

    int sockfd, sock_send, numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    int i;
    char caracter;
    char login;
    char senha;	
    int status;		
    int pid;
    int pid2;
    int status, status2;
//----------------------

void recebestring();
void checkcomando();
void checalogin();

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



int main(int argc, char *argv[])
{
	status = 0;


     if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }


    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
   
    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }


    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next)
	{


				if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
				{
				    perror("client: socket");
				    continue;
				}

				if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
				    close(sockfd);
				    perror("client: connect");
				 continue;    
			        }

				break;
   	 }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }


    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);

    printf("client: connecting to %s\n", s);
    
    freeaddrinfo(servinfo); // all done with this structure

 if((pid2=fork())==0)
{
	do{
    if((numbytes = recv(sockfd, &caracter, 1, 0)) == -1) {
        perror("recv");
        exit(1);}

	printf("received: %c\n",caracter);
	}while(caracter!='Q');

    if (send(sockfd,&myid,1, 0) == -1)
        perror("send");
    // while(status==0){

    // 	printf("vou receber login e senha...\n");
    //     if((numbytes = recv(sockfd, &login, 1, 0)) == -1) {
    //         perror("recv");
    //         printf("get it get it \n");
    //         exit(1);}
    // 	printf("recebi login: %c\n",login);
    	
    //     if((numbytes = recv(sockfd, &senha, 1, 0)) == -1) {
    //         perror("recv");
    //         exit(1);}
    // 	printf("recebi senha: %c\n",senha);	
    
    // 	printf("vou checar o login...\n");
    // 	checalogin();

    // 	printf("chequei login, o status novo vale %d...\n",status);
    //  }

    // printf("orginal\n");
    // dup2(sockfd,STDOUT_FILENO);
    // usleep(10);
    // printf("begin\n");
    fflush(stdout);
	printf("Waitting command...\n");
	fflush(stdout);

while(1)
{	
	if((pid=fork())==0)
{
			recebestring();
			usleep(10000);
			system(comando);
			fflush(stdout);
			exit(0);
}


	waitpid(pid,NULL,0);
	//recebestring();
	//checkcomando();

}

}
   // buf[numbytes] = '\0';
	waitpid(pid2, NULL,0);
   	close(sockfd);
    return 0;

}





void recebestring()
{
	if(recv(sockfd,comando,100,0)== -1)
	{
	perror("recv");
	exit(1);
	}
	printf("receive command: %s\n", comando );
}

void checalogin()
{

	switch(login)
	{
	case 'U':

	printf("entrou no caseu U...\n");
	if(senha!=(myid+1))
		    {
			if (send(sockfd,"e",1, 0) == -1)
	        		perror("send");

	        	status = 0;
	        	printf("senha errada, status = %d\n",status);
		    }
	else
		    {
			if (write(sockfd,"c",1) == -1)
	        		perror("send");

	        status=1;
	        printf("senha correta, status = %d\n",status);
		    }
	break;
	case 'A':
	if(senha!=(myid+2))
		    {
			if (send(sockfd,"e",1, 0) == -1)
	        		perror("send");

	        	status = 0;
	        	printf("senha errada, status = %d\n",status);	        	
		    }
		
	else
		    {
			if (send(sockfd,"c",1, 0) == -1)
	        		perror("send");

	        status=1;
	        printf("senha correta, status = %d\n",status);
		    }

	break;

	default:
	printf("Nao bateu nenhum case\n");
	}
}

void checkcomando()
{
	int icomand;

	if(strcmp(comando,"liga1")==0)
	{
	
		printf("recebi liga1");	
	}
	if(strcmp(comando,"liga2")==0)
	{
	
		printf("recebi 1iga2");	
	}
	if(strcmp(comando,"desliga1")==0)
	{
	
		printf("recebi liga3");	
	}
}

