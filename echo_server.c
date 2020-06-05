#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define BUF_SIZE 4096

static void handle_request(int sock)
{
    char buf[BUF_SIZE];
    ssize_t n;

    while((n = read(sock, buf, BUF_SIZE)) > 0) {
        buf[n] = '\0';
        printf("[%d] Read %ld bytes: %s", getpid(), n, buf);
        write(sock, buf, n);
    }
}

int main(int argc, char *argv[])
{
    int lsock, csock;
    struct sockaddr_in addr;
    struct sockaddr_in peeraddr;

    if (argc < 3) {
        fprintf(stderr, "Uso: %s ip puerto\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    lsock = socket(AF_INET, SOCK_STREAM, 0);
    if (lsock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &(addr.sin_addr));

    int optval = 1;
    if(setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    if (bind(lsock, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(lsock, 10) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("[%d] Listening for connections...\n", getpid());

    for (;;) {
        memset(&peeraddr, 0, sizeof(struct sockaddr_in));
        socklen_t socklen = sizeof(struct sockaddr_in);

        csock = accept(lsock, &peeraddr, &socklen);
        if (csock == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("[%d] Connection accepted from %s:%d ...\n", getpid(), inet_ntoa(peeraddr.sin_addr), peeraddr.sin_port);

        switch (fork()) {
            case -1:
                perror("fork");
                close(csock);
                break;
            case 0:
                close(lsock);
                handle_request(csock);
                _exit(EXIT_SUCCESS);
            default:
                close(csock);
                break;
        }
    }
}
