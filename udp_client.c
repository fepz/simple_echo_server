// UDP client
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
	struct sockaddr_in	 servaddr; 

    if (argc < 3) {
        fprintf(stderr, "Use: %s ip port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

	// Create socket 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 
	
	// Server address
	servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &(servaddr.sin_addr));
	
	int n, len; 
    printf("Send data to server %s:%d ...\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
	
    len = sizeof(servaddr);
    do {
        fgets(buffer, MAXLINE, stdin);
        n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*) &servaddr, sizeof(servaddr));
        if (n < 0) {
            perror("sendto");
            exit(EXIT_FAILURE);
        }

        printf("Sended %d bytes to %s:%d ...\n", n, inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));

	    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *) &servaddr, (socklen_t*) &len); 
        if (n < 0) {
            perror("recvfrom");
        }
    } while (buffer[0] != '\n');

    printf("Bye!\n");

	close(sockfd); 
	return 0; 
} 
