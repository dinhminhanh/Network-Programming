#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

#define PORT 8080
#define BUFFER_SIZE 256

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    struct pollfd fds[2];
    fds[0].fd = sockfd;      
    fds[0].events = POLLIN;   
    fds[1].fd = STDIN_FILENO; 
    fds[1].events = POLLIN; 

    while (1) {
        int activity = poll(fds, 2, -1);

        if (activity < 0) {
            perror("poll error");
            break;
        }

        // Check if there is input from the keyboard
        if (fds[1].revents & POLLIN) {
            memset(buffer, 0, sizeof(buffer));
            if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                // Send message to the server
                send(sockfd, buffer, strlen(buffer), 0);
            }
        }

        // Check if there is data from the server
        if (fds[0].revents & POLLIN) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
            if (bytes_received <= 0) {
                printf("Server disconnected or error occurred.\n");
                break;
            } else {
                buffer[bytes_received] = '\0';
                printf("Received from server: %s", buffer);
            }
        }
    }

    close(sockfd);
    return 0;
}
