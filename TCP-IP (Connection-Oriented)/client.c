#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX 1000

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket Creation Failed.");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8000);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connect Failed.");
        exit(EXIT_FAILURE);
    }

    char buf[MAX];
    const char *exit_command = "close";

    while (true) {
        // Print updated prompt message
        printf("Enter the Message from Client (type 'close' to quit) : ");

        fgets(buf, sizeof(buf), stdin);
        buf[strcspn(buf, "\n")] = '\0';

        if (send(sockfd, buf, strlen(buf), 0) == -1) {
            perror("Send Failed.");
            continue;
        }

        // Check for exit command
        if (strcmp(buf, exit_command) == 0) {
            printf("Closing...\n");
            break;
        }

        // Receive and print server response
        int bytes_received = recv(sockfd, buf, sizeof(buf), 0);
        if (bytes_received <= 0) {
            perror("Receive Failed.");
            break;
        }
        buf[bytes_received] = '\0';
        printf("Received from Server: %s\n", buf);

        // Check for exit command after receiving response
        if (strcmp(buf, exit_command) == 0) {
            printf("Closing...\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}
