#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 256

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    struct pollfd fds[MAX_CLIENTS + 1];
    int client_sockets[MAX_CLIENTS] = {0};
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    while (1) {
        int activity = poll(fds, MAX_CLIENTS + 1, -1);

        if (activity < 0) {
            perror("poll error");
            continue;
        }

        if (fds[0].revents & POLLIN) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd is %d\n", new_socket);

            for (int i = 1; i <= MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    fds[i].fd = new_socket;
                    fds[i].events = POLLIN;
                    break;
                }
            }
        }

        for (int i = 1; i <= MAX_CLIENTS; i++) {
            if (fds[i].revents & POLLIN) {
                int bytes_read = read(fds[i].fd, buffer, sizeof(buffer) - 1);
                if (bytes_read == 0) {
                    close(fds[i].fd);
                    client_sockets[i] = 0;
                    fds[i].fd = -1;
                    printf("Client disconnected\n");
                } else {
                    buffer[bytes_read] = '\0';
                    printf("Received: %s\n", buffer);
                    send(fds[i].fd, buffer, bytes_read, 0);
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
