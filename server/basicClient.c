#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int openClient() 
{
int sock = socket(AF_INET, SOCK_STREAM, 0);
if (sock == -1) 
{
    perror("Failed to create socket");
    exit(EXIT_FAILURE);
}

struct sockaddr_in server;
server.sin_family = AF_INET;
server.sin_port = htons(SERVER_PORT);
server.sin_addr.s_addr = inet_addr(SERVER_IP);

if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) 
{
    perror("Failed to connect to server");
    exit(EXIT_FAILURE);
}
printf("Connected to server!\n");

return sock;
}

int main() 
{
int clientSocket = openClient();

// Prepare the message
const char* msg = "Hello, server!";
uint32_t msg_len = strlen(msg);

// Send the length of the message
uint32_t net_msg_len = htonl(msg_len);
if (send(clientSocket, &net_msg_len, sizeof(net_msg_len), 0) != sizeof(net_msg_len)) 
{
    perror("send failed");
    close(clientSocket);
    exit(1);
}

if (send(clientSocket, msg, msg_len, 0) != msg_len) 
{
    perror("send failed");
    close(clientSocket);
    exit(1);
}

close(clientSocket);
return 0;
}
