#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#define PORT "202"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold

void print_status();

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
}
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int numtivas;
int status=0;
int tivaid[5];
int currentid;
char read_msg[1000];
static volatile char output_cliente[2000];
char write_msg[10]="0000000000";
char terminal[100];
char s[INET6_ADDRSTRLEN];
int usermode;
char senha[5],login[5];
int loop;
int pid;
int statuspid;


int main(void)
{

	//--------- mine -
	char buffer_msg;
	char caracter=65;
	int count = 0;
	int i;
	// ---------------	
    numtivas=0;
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
   
    int rv;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP



				    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
					{

					fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
					return 1;
					}


    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next)
	{

				if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
				{
				    perror("server: socket");
				    continue; 
				}


				if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) 
				{
				    perror("setsockopt");
			            exit(1);
			        }


				if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
				 {
				    close(sockfd);
				    perror("server: bind");
				    continue; 
				 }
	
	break; 

	}


					if (p == NULL)
					{
					fprintf(stderr, "server: failed to bind\n");
					return 2;
					}


					freeaddrinfo(servinfo); // all done with this structure
					if (listen(sockfd, BACKLOG) == -1) 
					{
					perror("listen");
					exit(1);
					}



    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;


					    if (sigaction(SIGCHLD, &sa, NULL) == -1) 
					     {
						perror("sigaction");
						exit(1);
					     }


    printf("server: waiting for connections...\n");



					while(1){  

							//print_status();
							// main accept() loop
							sin_size = sizeof their_addr;
							new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
			
								if (new_fd == -1) 
								{
									 perror("accept");
									 continue; 
								}
							        else
								{
									status=1;

									inet_ntop(their_addr.ss_family,
									get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
									printf("\nconectado a %s",s);
									numtivas++;  
								//	get_currentid();

											//FILHO

											if (!fork())
											{ 
														
											close(sockfd); // child doesn't need the listener
											if (send(new_fd, "Q", 1, 0) == -1) perror("send");	
											recv(new_fd, read_msg, 1, 0);
											
											// printf("O device com id %c se conectou. \nGostaria de acessa-lo? (S/N)", read_msg[0]);
											
											// scanf("%s",terminal); 	
											// 	if(terminal[0]=='S')
											// 	{	
												    
											// 		printf("SIM");	
													// fflush(stdout);	
													// read_msg[0]='e';

													// while(read_msg[0]=='e')
													// {
													// //fflush(stdout);
													// printf("\nLOGIN:");
													// //fflush(stdin);
													// scanf("%s",login); 
													// if (send(new_fd, login, 1, 0) == -1) perror("send");	
													
													// printf("\nSENHA:");
													// fflush(stdin);										
													// scanf("%s",senha); 
				                                                                  
													// if (send(new_fd, senha, 1, 0) == -1) perror("send");	

													// printf("\nSENHA ENVIADA");

											  //               recv(new_fd, read_msg, 1, 0);
													// printf("\n ERRO DE SENHA %c", read_msg[0]);
													
												 //         }

											  //      }
											  //      else{
													// printf("Fim de conexão");

		 										// 	fflush(stdout);
													// close(new_fd);
													 
													//  exit(0); 


												// }
											  		printf("Waiting comandos... \n");

													while(1)
										      			{
													 fflush(stdin);
													 fgets(terminal,100,stdin);
													 printf("\n");
										
													 if(send(new_fd, terminal, 100, 0) == -1) perror("send");
													 printf("Your command is %s\n",terminal);
													
													 //fflush(stdout);

													 if((pid=fork())==0)
													 {	
													 	printf("Waiting for receive...\n"); 
													 	recv(new_fd, output_cliente, 2000, 0);
													 	printf("%s\n",output_cliente);	
													 	fflush(stdout);										
													 	exit(0);
														      
													 }

													 //pai espera alguns segundos pelo retorno do filho com output
													 for(i = 0; i<500; i++)
													 {
													 	if(waitpid(pid,NULL,WNOHANG))
													 	break;
													 	usleep(1000);
													 }
													 printf("---------------------------\n");
							
													 usleep(100);

										    			}
												 
											close(new_fd);
													 
													 exit(0); 

											}


								//      FIM DO ELSE DE QUANDO 

				          	                  }


						        wait(0);
							
							close(new_fd);  // parent doesn't need this
						    }
						// FIM DO WHILE(1)



return 0;


}






























void print_status()
{
	int i;
	system("clear");
	printf("\n--------------------------------TIVA SUPERVISORIO--------------------------------");
	printf("\nStatus operacional:");				

					switch(status)
					{        
					case(0):  printf("Sem conexões...");
					
					break;

					case(1):  
				
					printf("Tivas online: %i", numtivas);
                                        printf("\n IDS: ");
				        for(i=0;i<numtivas-1;i++)
					printf("%i ;",tivaid[i]); 					
					break;

					case(2):  printf("\nAcesso usuário id: %i", currentid);
					
					break;		

					case(3): printf("\nAcesso administrador id: %i", currentid);
					
					break;

					}
       
	printf("\n%s", read_msg);
	printf("\n>/"); 
	fflush(stdin);
     //   scanf("%s", write_msg );
        fflush(stdout);


}

