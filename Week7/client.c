#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define BOARD_SIZE 3

char board[BOARD_SIZE][BOARD_SIZE];

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080); 
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(client_socket);
        exit(1);
    }

    printf("Connected to the Tic-Tac-Toe server.\n");

    while (1) {        
        memset(buffer, 0, BUFFER_SIZE);
        int n = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            printf("Server disconnected.\n");
            break;
        }

        if (strncmp(buffer, "0x03", 4) == 0) {  
            printf("%s\n", buffer + 4); 
        } 
        else if (strncmp(buffer, "0x04", 4) == 0) {  
            printf("%s\n", buffer + 4);  
            break;
        } 
        else if (strncmp(buffer, "0x05", 4) == 0) { 
            int row, col;
            while (1) {
                printf("Your turn! Enter your move (row and column): ");
                if (scanf("%d %d", &row, &col) != 2) {
                    printf("Invalid input. Please enter two numbers.\n");
                    while (getchar() != '\n');  // Clear the input buffer
                    continue;
                }

                if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
                    printf("Invalid move. Please enter row and column between 0 and %d.\n", BOARD_SIZE - 1);
                    continue;
                }

                snprintf(buffer, sizeof(buffer), "0x02 %d %d", row, col);
                send(client_socket, buffer, strlen(buffer), 0);
                break; // Exit the loop if a valid move is sent
            }
        } 
        else {
            printf("%s\n", buffer); 
        }
    }

    close(client_socket);
    return 0;
}
