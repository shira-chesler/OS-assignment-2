#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>

void error_exit(const char* msg);

void check_socket_operation(int result, const char* operation_name, int check_equal_to, int clientSocket);

void handleClient(void* args);

void createServerSocket(int* serverSocket);

void bindServerSocket(int serverSocket, struct sockaddr_in* serverAddress);

void listenServerSocket(int serverSocket);

void acceptClient(int serverSocket, int* clientSocket, struct sockaddr_in* clientAddress);

void openTcpServer(int argc, char *const *argv);

char* checkPath(int argc, char *const *argv);

void parse_message(char** message, char type);

char get_message_type(char* message);

void get_file(int clientSocket, char* fullpath);

void post_file(int clientSocket, char* fullpath, char* contents, int length);

void send_to(int clientSocket, char* message);

void recieve_from(int clientSocket, char** message);

#endif