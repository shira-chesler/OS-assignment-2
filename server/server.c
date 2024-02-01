#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include "server.h"
#include <pthread.h>

#define SERVER_PORT 8080
char* rootDirectory;

void* handleClient(void* clientFD) {
    int clientSocket = *(int*)clientFD;
    // Print "Hello" and client details
    printf("Hello\n");
    // Add your logic to handle the client request

    // Send a message to the client
    char* message = "Hello from the server!";
    write(clientSocket, message, strlen(message));

    close(clientSocket);
    pthread_exit(NULL);
}

void openTcpServer() {
    //opens a tcp server
    int domain = AF_INET;
    int server_socket = socket(domain, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(1);
    }

    int val = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(SERVER_PORT);
    // bind the port to the server socket
    if (bind(server_socket, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("bind failed");
        exit(1);
    }
    //waiting for requests
    if (listen(server_socket, 5) == -1) {
        perror("listen failed");
        exit(1);
    }
    printf("Server is ready to use.\n");
    printf("Server details:\n");
    printf("IP Address: 127.0.0.1\n");
    printf("Port: %d\n", SERVER_PORT);
    while (1) {
        //accepting new clients
        struct sockaddr_storage client_addr;
        socklen_t addrLen = sizeof(client_addr);
        int clientFD = accept(server_socket, (struct sockaddr *) &client_addr, &addrLen);
        if (clientFD == -1) {
            perror("accept failed");
            continue;
        }
        printf("New client accepted.\n");
        printf("Client details:\n");
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(((struct sockaddr_in *)&client_addr)->sin_addr), clientIP, INET_ADDRSTRLEN);
        printf("IP Address: %s\n", clientIP);
        printf("Port: %d\n", ntohs(((struct sockaddr_in *)&client_addr)->sin_port));
        // fire up a pthread for each client socket
        pthread_t thread;
        if (pthread_create(&thread, NULL, handleClient, (void*)&clientFD) != 0) {
            perror("pthread_create failed");
            continue;
        }
        pthread_detach(thread);
    }
    close(server_socket);
}

char* checkPath(int argc, char *const *argv) {
    //This function get the flag from the user when you run the server
    // find the current working dir and concat the directory to the root dir
    if (argc == 2) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd error");
            exit(1);
        }

        char* path = argv[1];
        char* formattedPath = malloc(strlen(cwd) + strlen(path) + 3);
        strcpy(formattedPath, cwd);

        if (formattedPath[strlen(formattedPath) - 1] != '/') {
            strcat(formattedPath, "/");
        }

        strcat(formattedPath, path);

        if (formattedPath[strlen(formattedPath) - 1] != '/') {
            strcat(formattedPath, "/");
        }

        return formattedPath;
    } else {
        perror("no flag was passed");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    rootDirectory = checkPath(argc, argv);
    printf("Root Directory: %s\n", rootDirectory);
    openTcpServer();
    return 0;
}