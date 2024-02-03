#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define DEFAULT_SERVER_PORT 9453
#define MAX_MSG_SIZE 4096
#define MAX_CONN_BACKLOG 1024
#define ENABLE_REUSEADDR 1

void usage() {
    fprintf(stdout, "Usage: %s [<server_port>]\n", __FILE__);
}

int main(int argc, char **argv) {
    int server_port;

    if (argc == 1) {
        server_port = DEFAULT_SERVER_PORT;
    } else if (argc == 2) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            usage();
            exit(EXIT_SUCCESS);
        } else if (atoi(argv[1]) > 0 & atoi(argv[1]) < 65536){
            server_port = atoi(argv[1]);
        } else {
            fprintf(stderr, "[error] invalid server port: %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    } else {
        usage();
        exit(EXIT_FAILURE);
    }

    /* create struct for socket */
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(server_port),
        .sin_addr = htonl(INADDR_ANY)
    };
    socklen_t sock_len = sizeof(server_addr);

    /* create struct for client */
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    /* establish a socket */
    fprintf(stdout, "[info] create server socket\n");
    int sock_fd;
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[error] create server socket");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, 
        &((int) {ENABLE_REUSEADDR}), sizeof(int)) < 0){
        perror("[error] setsockopt SO_REUSEADDR failed");
    }


    /* bind the socket */
    fprintf(stdout, "[info] bind server socket to port: %d\n", server_port);
    if (bind(sock_fd, (struct sockaddr *) &server_addr, sock_len) < 0) {
        perror("[error] bind server socket");
        exit(EXIT_FAILURE);
    }

    /* listen to the socket with port */
    fprintf(stdout, "[info] listen to server socket with port: %d\n", 
        server_port);
    if (listen(sock_fd, MAX_CONN_BACKLOG) < 0) {
        fprintf(stderr, "[error] listen to server socket with port: %d\n", 
            server_port);
        exit(EXIT_FAILURE);
    }

    while (true) {
        /* accept the incoming connection attempts */
        int client = accept(sock_fd, (struct sockaddr *) &server_addr, &sock_len);
        if (client < 0) {
            perror("[error] accept client");
            exit(EXIT_FAILURE);
        }

        /* echo the message from the client */
        char buf[MAX_MSG_SIZE] = { 0 };
        ssize_t msg_sz = read(client, buf, MAX_MSG_SIZE);
        printf("[message] '%s' accepted from %s:%d\n", buf, 
            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }

    /* close the socket fd */
    close(sock_fd);

    return 0;
}