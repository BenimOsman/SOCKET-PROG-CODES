#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define MAX 1000

int main() 
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1) {
        perror("Socket creation failed.");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8000);

    char buf[MAX];
    const char *exit_command = "close";

    while (true) 
    {
        printf("Enter the Message from Client (type 'close' to quit) : ");
        fgets(buf, sizeof(buf), stdin);
        buf[strcspn(buf, "\n")] = '\0'; 
        if (sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) 
        {
            perror("Send failed");
            continue;
        }

        if (strcmp(buf, exit_command) == 0) {
            printf("Closing. . .\n");
            break;
        }
    }
    close(sockfd);
    return 0;
}
