#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> 
#include <netinet/in.h> 
int main() {
    struct sockaddr_in server_addr; 
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(8080); 
    
   
    if (inet_pton(AF_INET, "192.168.0.107", &server_addr.sin_addr) <= 0) {
        perror("inet_pton error"); 
        return -1;
    }

    memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));


    // Print the initialized structure
    printf("sockaddr_in structure initialized:\n");
    printf("  Family      : AF_INET (IPv4)\n");
    printf("  Port        : %d\n", ntohs(server_addr.sin_port));  
    printf("  IP Address  : 192.168.0.107\n");
    return 0;
}
