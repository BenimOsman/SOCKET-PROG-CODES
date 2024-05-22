#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) 
{
    if (argc != 3) {
        printf("Usage: %s <server_ip> <port_number>\n", argv[0]);
        exit(1);
    }
    int client_socket;
    struct sockaddr_in server_addr;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);                    // Create socket
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    memset(&server_addr, 0, sizeof(server_addr));                       // Initialize server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));                        // Port Number from command-line argument
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);                   // Server IP address from command-line argument

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }
    printf("Connected to the server.\n");
    char buffer[BUFFER_SIZE];
    printf("Client 2 (You): ");
    fgets(buffer, BUFFER_SIZE, stdin);
    send(client_socket, buffer, strlen(buffer), 0);
    while (1) 
    {
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            printf("Server closed the connection. Exiting...\n");
            break;
        }
        buffer[bytes_received] = '\0';
        printf("Received from Server: %s", buffer);

        printf("Client 2 (You): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        send(client_socket, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "exit\n") == 0) {
            printf("You closed the connection. Exiting...\n");
            break;
        }
    }
    close(client_socket);
    return 0;
}