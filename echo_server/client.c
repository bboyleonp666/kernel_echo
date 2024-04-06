#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define DEFAULT_SERVER_IP "127.0.0.1"
#define MAX_MSG_SIZE 4096

void usage() {
    fprintf(stdout, "Usage: %s [<server_addr>] <server_port> <message>\n", __FILE__);
}

int main(int argc, char **argv) {
    int server_port;
    char *server_ip, *message;

    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        usage();
        exit(EXIT_SUCCESS);
    } else if (argc == 3) {
        server_ip = DEFAULT_SERVER_IP;
        server_port = atoi(argv[1]);
        message = argv[2];
    } else if (argc == 4) {
        server_ip = argv[1];
        server_port = atoi(argv[2]);
        message = argv[3];
    } else {
        usage();
        exit(EXIT_FAILURE);
    }

    ssize_t msg_len = strlen(message);
    if (msg_len > MAX_MSG_SIZE) {
        fprintf(stderr, "[error] client: message longer than limit: %d\n", MAX_MSG_SIZE);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(server_port),
        .sin_addr = inet_addr(server_ip)
    };

    fprintf(stdout, "[info] client: create socket\n");
    int sock_fd;
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[error] client: create socket failed");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "[info] client: connect to %s:%d\n", server_ip, server_port);
    int status;
    if ((status = connect(sock_fd, (struct sockaddr *) &server_addr, 
        sizeof(server_addr))) < 0) {
        perror("[error] client: connect to server failed");
        exit(EXIT_FAILURE);
    }
    send(sock_fd, message, msg_len, 0);
    printf("[message] client: sent '%s' to %s:%d\n", message, DEFAULT_SERVER_IP, 
        server_port);

    close(sock_fd);

    return 0;
}