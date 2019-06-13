// Client side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define MAXLINE 1024 

// Driver code 
int main(int argc, char* argv[])
{ 
	int sockfd; 
	char buffer[MAXLINE]; 
	char *hello = "Hello from client"; 
    char *s = "s";
	struct sockaddr_in	 servaddr; 

    if (argc < 3) {
        fprintf(stderr, "Uso: %s ip puerto\n", argv[0]);
        exit(EXIT_FAILURE);
    }

	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &(servaddr.sin_addr));
	
	int n, len; 
    printf("Connecting to %s:%d ...\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
	
	n = sendto(sockfd, (const char *)hello, strlen(hello), 
		0, (const struct sockaddr *) &servaddr, 
	    sizeof(servaddr));
    if (n < 0) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
    
    //send(sockfd, (const char*)hello, strlen(hello), MSG_CONFIRM);
	printf("Hello message sent.\n"); 
		
	n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
				0, (struct sockaddr *) &servaddr, 
				&len); 
    //n = recv(sockfd, (char*)buffer, MAXLINE, MSG_WAITALL);
	buffer[n] = '\0'; 
	printf("Server : %s\n", buffer); 
    printf("Port: %d\n", ntohs(servaddr.sin_port));

    do {
        fgets(buffer, MAXLINE, stdin);
        n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*) &servaddr, sizeof(servaddr));
        if (n < 0) {
            perror("sendto");
            exit(EXIT_FAILURE);
        }
    } while (buffer[0] != '\n');

	close(sockfd); 
	return 0; 
} 
