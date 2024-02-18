#include "utils.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024
#define CWD_SIZE 1024

//Error handling functions
void error_exit(const char* msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

//Check if the result of an operation is equal to a certain value
void check_operation(int result, const char* operation_name, int check_equal_to) 
{
    if (result == check_equal_to) 
    {
        perror(operation_name);
        exit(EXIT_FAILURE);
    }
}

//This function decodes the contents of a file
void Decode_contents(char* contents, char** decoded_contents, int client)
{
    int cont_len = strlen(contents);
    int decoded_len = size_of_decoded_string(cont_len);
    *decoded_contents = malloc(decoded_len + 1);
    base64_decode_string(contents, cont_len, decoded_contents);
    free(contents);
    return;
}

//This function gets the current working directory
void get_check_cwd(char* cwd, char* path, char* server_ip) 
{
    if (getcwd(cwd, sizeof(cwd)) == NULL) 
    {
        free(path);
        free(server_ip);
        error_exit("getcwd error");
    }
    return;
}

//This function handles the file
void file_handler_write(char* cwd, char* path, char* decoded_response, char* server_ip)
{
    int filename_len = get_length_filename(path);
    char* filename = malloc(filename_len + 1);
    get_filename(path, &filename);
    char* filepath = malloc(strlen(cwd) + strlen(filename) + 2);
    sprintf(filepath, "%s/%s", cwd, filename);
    FILE* file = fopen(filepath, "w");
    free(filepath);
    free(filename);

    if (file == NULL) 
    {
        free(decoded_response);
        free(path);
        free(server_ip);
        error_exit("fopen error");
    }
    
    fwrite(decoded_response, 1, strlen(decoded_response), file);
    fclose(file);
    free(decoded_response);
    return;
}

//Sends a message to the server
void send_to(int clientSocket, char* message) 
{
    uint32_t msg_len = strlen(message);
    uint32_t net_msg_len = htonl(msg_len);
    
    int send_result = send(clientSocket, &net_msg_len, sizeof(net_msg_len), 0);
    check_operation(send_result, "send", sizeof(net_msg_len));
    
    send_result = send(clientSocket, message, msg_len, 0);
    check_operation(send_result, "send", msg_len);
}

//Receives a message from the server
void recieve_from(int clientSocket, char** message_ptr) 
{
    uint32_t msg_len;
    int recv_result = recv(clientSocket, &msg_len, sizeof(msg_len), 0);
    check_operation(recv_result, "recv", sizeof(msg_len));

    msg_len = ntohl(msg_len);
    *(message_ptr) = malloc(msg_len + 1);
    

    recv_result = recv(clientSocket, *(message_ptr), msg_len, 0);
    check_operation(recv_result, "recv", msg_len);

    (*(message_ptr))[msg_len] = '\0';

    return;
}

//Opens a client socket and connects to the server
int openClient(char* server_ip, char* path)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    check_operation(sock, "socket", -1);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) 
    {
        perror("Failed to connect to server");
        free(server_ip);
        free(path);
        exit(EXIT_FAILURE);
    }
    
    printf("Connected to server!\n");

    return sock;
}

//Checks if the method is valid and if the number of arguments is correct
void checkmethod(int argc, char *argv[], char* server_ip, char* path) 
{
    if (argc < 3) {
        free(server_ip);
        free(path);
        error_exit("Usage: ./client <method> <path>");
    }
    char* method = argv[1];
    if ((strcmp(method,"GET") != 0) && (strcmp(method,"POST") != 0)) {
        printf("Invalid method: %s\n", method);
        exit(EXIT_FAILURE);
    }
    if ((strcmp(method,"POST") == 0) && argc < 4) {
        free(server_ip);
        free(path);
        error_exit("Usage: ./client POST <path> <Path to file>");
    }
    return;
}

//Returns the length of the filename
int get_length_filename(char* path)
{
    char* filename = strrchr(path, '/');
    if (filename == NULL) {
        return strlen(path);
    }
    return strlen(filename + 1);
}

//Returns the filename
void get_filename(char* path, char** filename)
{
    char* file_name = strrchr(path, '/');
    if (file_name == NULL) {
        strcpy(*filename, path);
    }
    else {
        strcpy(*filename, file_name + 1);
    }
    return;
}

//Parses the response from the server
void parse_response(char* response, char** message, char** contents)
{
    char* token = strtok(response, "\r\n");
    *message = malloc(strlen(token) + 1);
    strcpy(*message, token);
    if (token != NULL)
    {
        token = strtok(NULL, "\n");
        *contents = malloc(strlen(token) + 1);
        strcpy(*contents, token);
    }
    else *contents = malloc(1);
    return;

}

//Formats the request
void format_request(char* method, char* path, char** request)
{
    char* double_crlf = "\r\n\r\n";
    *request = malloc(strlen(method) + strlen(path) + strlen(double_crlf) + 2);
    sprintf(*request, "%s %s", method, path);
    char* tmp_request = *request;
    sprintf(*request, "%s%s", tmp_request, double_crlf);
    return;
}

//This function gets a path to a file and sends a GET request for the file
void sendGetRequestFile(int client, char* path, char* server_ip) 
{
    //Sending the request
    char* request;
    format_request("GET", path, &request);
    send_to(client, request);
    free(request);

    //Receiving the response
    char* response;
    recieve_from(client, &response);

    //parsing the response
    char* message;
    char* contents;
    parse_response(response, &message, &contents);
    
    //validating (the desired message is "OK")
    if(strcmp(message, "200 OK") != 0)
    {
        free(response);
        free(message);
        free(contents);
        close(client);
        perror(message);
        free(server_ip);
        free(path);
        exit(EXIT_FAILURE);
    }
    printf("%s", message);
    free(response);
    free(message);

    //Decoding the response
    char* decoded_response;
    Decode_contents(contents, &decoded_response, client);

    //Writing the response to a file
    char cwd[CWD_SIZE];
    get_check_cwd(cwd, path, server_ip);

    file_handler_write(cwd, path, decoded_response, server_ip);

    return;
}

//This function sends a GET request for a list of files
void sendGetRequestList(int client, char* path, char* server_ip)
{
    //Sending the request
    char* request;
    format_request("GET", path, &request);
    send_to(client, request);
    free(request);

    //Receiving the response
    char* response;
    recieve_from(client, &response);

    //parsing the response
    char* message;
    char* contents;
    parse_response(response, &message, &contents);
    
    //validating (the desired message is "OK")
    if(strcmp(message, "200 OK") != 0)
    {
        free(response);
        free(message);
        free(contents);
        close(client);
        perror(message);
        free(server_ip);
        free(path);
    }
    printf("%s", message);
    free(response);
    free(message);

    //Decoding the response
    char* decoded_response;
    Decode_contents(contents, &decoded_response, client);


    sendListToPoll(decoded_response, client, path, server_ip);
    return;

}

//This function gets the contents of the files into a struct
void get_into_struct(char* list, int num_files, int client, char* cwd, char* path,  struct pollfd* fds, char* server_ip)
{
    char* token = strtok(list, "\n");
    for (int i = 0; i < num_files; i++)
    {
        int file_len = strlen(token);
        char* file = malloc(file_len + 1);
        strcpy(file, token);
        char* request;
        format_request("GET", file, &request);
        int new_client = openClient(server_ip, path);
        send_to(new_client, request);
        free(request);
        fds[i].fd = new_client;
        fds[i].events = POLLIN;
        token = strtok(NULL, "\n");
    }
    free(list);
    return;
}

//This function sends a list of files to the server
void sendListToPoll(char* list, int client, char* path, char* server_ip)
{
    char cwd[CWD_SIZE];
    get_check_cwd(cwd, path, server_ip);
    
    char* token = strtok(list, "\n");
    int num_files = 0;
    
    while (token != NULL)
    {
        num_files++;
        token = strtok(NULL, "\n");
    }
    
    struct pollfd fds[num_files];
    get_into_struct(list, num_files, client, cwd, path, fds, server_ip);

    //Using poll() to receive the responses
    int poll_result = poll(fds, num_files, -1);
    check_operation(poll_result, "poll", -1);

    for (int i = 0; i < num_files; i++)
    {
        if (fds[i].revents & POLLIN)
        {
            char* response;
            recieve_from(client, &response);
            char* message;
            char* contents;

            parse_response(response, &message, &contents);
            free(response);

            //check what the mesage is
            if(strcmp(message ,"200 OK") != 0)
            {
                free(message);
                free(contents);
                close(fds[i].fd);
            }
            free(message);
            

            //decoding the contents
            char* decoded_contents;
            Decode_contents(contents, &decoded_contents, fds[i].fd);
            free(contents);

            file_handler_write(cwd, path, decoded_contents, server_ip);
            close(fds[i].fd);
        }
    }
    return;
}

//This function reads the contents of a file
int file_handler_read(int client, char* file_path, char** file_contents, char* server_ip, char* path)
{
    FILE* file = fopen(file_path, "r");
    if (file == NULL) 
    {
        free(path);
        free(server_ip);
        error_exit("fopen error");
    }
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    *file_contents = malloc(file_size + 1);
    fread(*file_contents, 1, file_size, file);
    (*file_contents)[file_size] = '\0';
    fclose(file);
    return file_size;
}

//This function sends a POST request
void sendPostRequest(int client, char* path, char* file_path, char* server_ip)
{
    //Sending the request
    char* request;
    format_request("POST", path, &request);
    send_to(client, request);
    free(request);

    //Reading the file
    char* file_contents;
    int file_size = file_handler_read(client, file_path, &file_contents, server_ip, path);

    //Encoding the file
    char* encoded_file;
    base64_encode_string(file_contents, file_size, &encoded_file);
    free(file_contents);

    //Sending the file
    send_to(client, encoded_file);
    free(encoded_file);

    //Receiving the response
    char* response;
    recieve_from(client, &response);

    //parsing the response
    char* message;
    char* contents;
    parse_response(response, &message, &contents);
    
    //validating (the desired message is "OK")
    if(strcmp(message, "200 OK") != 0)
    {
        free(response);
        free(message);
        free(contents);
        close(client);
        perror(message);
        free(server_ip);
        free(path);
    }
    printf("%s", message);
    free(response);
    free(message);
    free(contents);
    return;
}

//This function checks the file extension
int file_extension_list(char* path)
{
    char* filename = strrchr(path, '/');
    char* extension = strrchr(filename, '.');
    if (extension == NULL) {
        return -1;
    }
    else if (strcmp(extension, ".list") == 0) {
        return 1;
    }
    else {
        return 0;
    }
}

//function to parse the path (using DNS to get the server IP - somethig like "http://www.google.com/file.txt")
void parse_path(int argc, char *argv[], char** server_ip, char** route_path) 
{
    char* path = argv[2];

    // server name = whats between "http://" and the first "/" or between "https://" and the first "/"
    //get the server name
    //check the prefix of the path
    if (strncmp(path, "http://", 7) != 0 && strncmp(path, "https://", 8) != 0) {
        error_exit("Invalid path - must start with http:// or https://");
    }
    else if (strncmp(path, "http://", 7) == 0) {
        path += 7;
    }
    else {
        path += 8;
    }

    char* token = strtok(path, "/");
    char* server_name = token;
    token = strtok(NULL, "/");
    char* file_path = token;
    struct hostent *server = gethostbyname(server_name);
    if (server == NULL) {
        error_exit("No such host");
    }
    *server_ip = malloc(16);
    strcpy(*server_ip, inet_ntoa(*((struct in_addr *)server->h_addr_list[0])));

    *route_path = malloc(strlen(file_path) + 1);
    strcpy(*route_path, file_path);
    return;
}

int main(int argc, char *argv[]) {
    char* server_ip;
    char* path;
    parse_path(argc, argv, &server_ip, &path);
    int client = openClient(server_ip, path);
    checkmethod(argc, argv, server_ip, path);
    char* method = argv[1];
    
    if (strcmp(method,"GET") != 0)
    {
        int extension = file_extension_list(path);
        if (extension == 1) {
            sendGetRequestList(client, path, server_ip);
        }
        else if (extension == 0) {
            sendGetRequestFile(client, path, server_ip);
        }
        else { 
            free(server_ip);
            free(path);
            error_exit("No file extension provided");
        }
    }
    else // POST
    {
        sendPostRequest(client, path, argv[3], server_ip);
    }
    free(server_ip);
    free(path);

    return 0;
}