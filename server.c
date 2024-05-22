#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/select.h>

#define MAX_CLIENTS 2
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) 
{
    if (argc != 3) {
        printf("Usage: %s <server_ip> <port_number>\n", argv[0]);
        exit(1);
    }

    int server_socket, client_sockets[MAX_CLIENTS];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);                                    // Creating Socket
    if (server_socket < 0) 
    {
        perror("Socket creation failed.");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));                                       // Initialize server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);                                   // IP address from command-line argument
    server_addr.sin_port = htons(atoi(argv[2]));                                        // Port number from command-line argument

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)   //  Bind socket to address
    { 
        perror("Binding failed");
        exit(1);
    }
    if (listen(server_socket, MAX_CLIENTS) < 0)                                         // Listening for incoming connections
    {
        perror("Listening failed");
        exit(1);
    }

    fd_set read_fds;
    int max_fd = server_socket;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = -1;
    }
    printf("Server is listening on %s:%s...\n", argv[1], argv[2]);
    while (1) 
    {
        FD_ZERO(&read_fds);                                                             // Clear the set
        FD_SET(server_socket, &read_fds);                                               // Add server socket to the set
        for (int i = 0; i < MAX_CLIENTS; i++)                                           // Add client sockets to the set
        {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &read_fds);
                if (client_sockets[i] > max_fd) {
                    max_fd = client_sockets[i];
                }
            }
        }
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Select error");
            exit(1);
        }

        if (FD_ISSET(server_socket, &read_fds))                                          // Check for incoming connections on the server socket
        {
            int new_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
            if (new_socket < 0) {
                perror("Accept error");
                exit(1);
            }
            printf("New connection, socket fd is %d\n", new_socket);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == -1) {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }
        for (int i = 0; i < MAX_CLIENTS; i++) 
        {
            int client_socket = client_sockets[i];
            if (client_socket > 0 && FD_ISSET(client_socket, &read_fds)) {
                char buffer[BUFFER_SIZE];
                int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
                if (bytes_received <= 0) {
                    printf("Client %d disconnected\n", client_socket);
                    close(client_socket);
                    client_sockets[i] = -1;
                } else {
                    buffer[bytes_received] = '\0';
                    printf("Received from client %d: %s", client_socket, buffer);
                    printf("Enter the Client Number whom to send (1 or 2): ");
                    int client_num;
                    scanf("%d", &client_num);
                    getchar(); 
                    if (client_num < 1 || client_num > MAX_CLIENTS || client_sockets[client_num - 1] == -1) {
                        printf("Invalid client number or client not connected.\n");
                    } else {
                        printf("Enter the Message: ");
                        fgets(buffer, BUFFER_SIZE, stdin);
                        send(client_sockets[client_num - 1], buffer, strlen(buffer), 0);
                        if (strcmp(buffer, "exit\n") == 0) {
                            printf("Client %d closed the socket for port %s. Exiting...\n", client_num, argv[2]);
                            close(client_sockets[client_num - 1]);
                            client_sockets[client_num - 1] = -1;
                        }
                    }
                }
            }
        }
    }
    close(server_socket);
    return 0;
}