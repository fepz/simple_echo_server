#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUF_SIZE 4096

int thread_id = 0;
struct in_addr ip_srv;

struct conn_info {
    int sock;
    struct sockaddr_in addr;
};

static void* handle_request_udp(void* s)
{
    char buf[BUF_SIZE];
    ssize_t n;
    socklen_t len = sizeof(struct sockaddr_in);

    struct conn_info *client = (struct conn_info *) s;
    int id = thread_id;

    struct sockaddr_in naddr;
    memset(&naddr, 0, sizeof(struct sockaddr_in));
    naddr.sin_family = AF_INET;
    naddr.sin_port = htons(8081);
    naddr.sin_addr.s_addr = ip_srv.s_addr;

    client->sock = socket(AF_INET, SOCK_DGRAM, 0);

    int optval = 1;
    if (setsockopt(client->sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    if (bind(client->sock, (struct sockaddr*) &naddr, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("[Thread %d] I'm listening at port %d, and handling client %s:%d ...\n", thread_id, ntohs(naddr.sin_port), inet_ntoa((client->addr).sin_addr), ntohs((client->addr).sin_port));

    strcpy(buf, "Hola!");
    sendto(client->sock, buf, strlen(buf), 0, (struct sockaddr*) &(client->addr), sizeof(struct sockaddr_in));
    
    while((n = recvfrom(client->sock, buf, BUF_SIZE, 0, (struct sockaddr*) &naddr, &len)) > 0) {
        buf[n]='\0';
        printf("[%d] Read %ld bytes: %s", id, n, buf);
        sendto(client->sock, buf, strlen(buf), 0, (struct sockaddr*) &(client->addr), sizeof(struct sockaddr_in));
    }
}


int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in addr;
    struct conn_info *client;
    char buf[100];

    if (argc < 3) {
        fprintf(stderr, "Uso: %s ip puerto\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    inet_aton(argv[1], &ip_srv);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = ip_srv.s_addr;

    if (bind(sock, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    pthread_t thread;

    printf("[%d] Listening for connections...\n", getpid());

    int n;
    for (;;) {
        socklen_t clientlen = sizeof(struct sockaddr_in);
        client = (struct conn_info*) malloc(sizeof(struct conn_info));
        memset(client, 0, sizeof(struct conn_info));
        n = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*) &(client->addr), &clientlen);

        printf("[%d] Connection accepted from %s:%d ...\n", getpid(), inet_ntoa((client->addr).sin_addr), ntohs((client->addr).sin_port));
        buf[n] = '\0';
        printf("[%d] Received: %s (%d bytes)...\n", getpid(), buf, n);

        thread_id++;

        pthread_create(&thread, NULL, handle_request_udp, (void*) client);
    }
}
