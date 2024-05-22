#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<sys/time.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define MAX 1000

int main() 
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed.");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8000);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    char buf[MAX];
    const char *exit_cmd = "close";

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_sockfd == -1) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    fd_set readfds;
    struct timeval timeout;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(client_sockfd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        timeout.tv_sec = 0;
        timeout.tv_usec = 10000; // 10 milliseconds

        int max_fd = (client_sockfd > STDIN_FILENO) ? client_sockfd : STDIN_FILENO;
        int activity = select(max_fd + 1, &readfds, NULL, NULL, &timeout);

        if (activity < 0) {
            perror("Select error");
            break;
        }

        if (FD_ISSET(client_sockfd, &readfds)) {
            // Receive message from client
            int bytes_received = recv(client_sockfd, buf, sizeof(buf), 0);
            if (bytes_received <= 0) {
                perror("Receive failed");
                break;
            }
            buf[bytes_received] = '\0';
            printf("Received Message from Client: %s\n", buf);

            // Check for exit command
            if (strcmp(buf, exit_cmd) == 0) {
                printf("Closing connection...\n");
                break;
            }

            // Prompt for response
            printf("Enter the Response to Client : ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = '\0';

            // Send response to the client
            send(client_sockfd, buf, strlen(buf), 0);

            // Check for exit command after sending response
            if (strcmp(buf, exit_cmd) == 0) {
                printf("Closing connection...\n");
                break;
            }
        }
    }

    close(client_sockfd);
    close(sockfd);
    return 0;
}
