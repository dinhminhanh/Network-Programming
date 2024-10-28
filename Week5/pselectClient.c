#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 128

volatile sig_atomic_t got_signal = 0;

void signal_handler(int sig) {
    got_signal = 1;
    printf("Caught signal %d\n", sig);
}

void setup_signal_handler() {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    fd_set readfds;
    sigset_t block_mask, orig_mask;
    char buffer[BUFFER_SIZE];
    int ready, bytes_received;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server at %s:%d\n", SERVER_IP, SERVER_PORT);

    setup_signal_handler();

    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGINT);
    sigprocmask(SIG_BLOCK, &block_mask, &orig_mask);

    printf("SIGINT blocked, ready to send/receive messages...\n");

    while (1) {
        if (got_signal) {
            printf("Signal received. Exiting...\n");
            break;
        }

        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sockfd, &readfds);

        ready = pselect(sockfd + 1, &readfds, NULL, NULL, NULL, &orig_mask);

        if (ready == -1) {
            if (errno == EINTR) {
                printf("pselect() was interrupted by a signal.\n");
                continue;
            } else {
                perror("pselect");
                break;
            }
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            memset(buffer, 0, sizeof(buffer));
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = '\0';

            if (strlen(buffer) > 0) {
                if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
                    perror("send() failed");
                    break;
                }
            }
        }

        if (FD_ISSET(sockfd, &readfds)) {
            memset(buffer, 0, sizeof(buffer));
            bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

            if (bytes_received == 0) {
                printf("Server disconnected.\n");
                break;
            } else if (bytes_received < 0) {
                perror("recv() failed");
                break;
            } else {
                buffer[bytes_received] = '\0';
                printf("Server: %s\n", buffer);
            }
        }
    }

    close(sockfd);
    sigprocmask(SIG_SETMASK, &orig_mask, NULL);
    printf("Exiting...\n");
    return 0;
}
