#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

void convert_ipv4(const char *ip_str)
{
    struct in_addr ipv4_addr; // Structure to hold the IPv4 address

    // Convert the IPv4 address from string to binary form
    if (inet_pton(AF_INET, ip_str, &ipv4_addr) == 1)
    {
        printf("inet_pton (IPv4): Successfully converted IP address: %s\n", ip_str);

        // Print binary representation of the IP address
        printf("Binary representation of IP address: ");
        for (int i = 0; i < sizeof(ipv4_addr.s_addr); i++) {
            unsigned char byte = ((unsigned char*)&ipv4_addr.s_addr)[i];
            for (int j = 7; j >= 0; j--) {
                printf("%d", (byte >> j) & 1);
            }
            if (i < sizeof(ipv4_addr.s_addr) - 1) {
                printf("."); // Separate bytes with a dot for clarity
            }
        }
        printf("\n");
    }
    else
    {
        printf("inet_pton (IPv4): Failed to convert IP address: %s\n", ip_str);
        exit(EXIT_FAILURE);
    }

    char ip_str_converted[INET_ADDRSTRLEN]; // Buffer to hold the converted IP address back to string

    // Convert the binary IPv4 address back to string form
    if (inet_ntop(AF_INET, &ipv4_addr, ip_str_converted, INET_ADDRSTRLEN))
    {
        printf("inet_ntop (IPv4): Converted back to string IP address: %s\n", ip_str_converted);
    }
    else
    {
        printf("inet_ntop (IPv4): Failed to convert IP address back to string\n");
        exit(EXIT_FAILURE);
    }
}

int main()
{
    char ipv4_str[64];
    printf("Enter an IPv4 address: ");
    fgets(ipv4_str, sizeof(ipv4_str), stdin);
    ipv4_str[strcspn(ipv4_str, "\n")] = 0; 
    convert_ipv4(ipv4_str);
    return 0;
}
