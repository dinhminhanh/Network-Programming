#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXLINE 1024
#define PORT 8080

ssize_t readline(int fd, void *vptr, size_t maxlen);
char *sock_ntop(const struct sockaddr_in *sa, char *str, size_t len);

int main() {
    int listenfd, connfd;
    struct sockaddr_in servaddr, clientaddr;
    char buffer[MAXLINE];
    ssize_t n;
    socklen_t cl = sizeof(clientaddr);

    // Create a listening socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize the server address structure
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Bind the socket to the address and port
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(listenfd, 10) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {  // Loop to accept multiple client connections
        // Accept a connection from a client
        connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &cl);
        if (connfd < 0) {
            perror("Accept failed");
            continue;
        }

        // Use sock_ntop to convert the client's IP and port to a string
        char client_str[INET_ADDRSTRLEN + 6];  // Buffer to hold IP and port in string form
        printf("Client connected from: %s\n", sock_ntop(&clientaddr, client_str, sizeof(client_str)));

        // Read the message from the client
        while ((n = readline(connfd, buffer, MAXLINE)) > 0) {
            buffer[n] = '\0';
            printf("Received message: %s", buffer);

            // Echo the message back to the client
            if (write(connfd, buffer, n) < 0) {
                perror("Write failed");
                break;
            }
        }

        if (n < 0) {
            perror("Readline failed");
        }

        close(connfd);  // Close connection with the client
    }

    close(listenfd);  // Close the listening socket
    return 0;
}

// Function to read a line from a descriptor (up to newline)
ssize_t readline(int fd, void *vptr, size_t maxlen) {
    ssize_t n, rc;
    char c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ((rc = read(fd, &c, 1)) == 1) {
            *ptr++ = c;
            if (c == '\n') {
                break;  // Stop at newline character
            }
        } else if (rc == 0) {
            if (n == 1) {
                return 0;  // No data read
            } else {
                break;  // Some data was read
            }
        } else {
            return -1;  // Error during read
        }
    }
    *ptr = 0;  // Null-terminate the string
    return n;
}

// Function to convert a client's IP address and port to a string
char *sock_ntop(const struct sockaddr_in *sa, char *str, size_t len) {
    char ip[INET_ADDRSTRLEN];

    // Convert the binary IP address to string format
    inet_ntop(AF_INET, &(sa->sin_addr), ip, sizeof(ip));

    // Convert the port number from network byte order to host byte order
    unsigned short port = ntohs(sa->sin_port);

    // Combine the IP address and port into a string
    snprintf(str, len, "%s:%u", ip, port);

    return str;
}
