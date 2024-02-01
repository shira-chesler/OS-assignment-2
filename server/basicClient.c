#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int openClient() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Failed to connect to server");
        exit(EXIT_FAILURE);
    }
    printf("Connected to server!\n");

    return sock;
}

int main() {
    int clientSocket = openClient();

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    if (recv(clientSocket, buffer, sizeof(buffer), 0) < 0) {
        perror("Failed to receive message from server");
        exit(EXIT_FAILURE);
    }

    printf("Received message from server: %s\n", buffer);

    close(clientSocket);
    return 0;
}
