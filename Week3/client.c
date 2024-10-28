#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXLINE 1024
#define PORT 8080

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE];
    char server_ip[INET_ADDRSTRLEN];
    ssize_t n;

    // Nhập địa chỉ IP của server từ người dùng
    printf("Enter server IP address: ");
    fgets(server_ip, sizeof(server_ip), stdin);
    // Xóa ký tự xuống dòng ở cuối chuỗi nếu có
    server_ip[strcspn(server_ip, "\n")] = 0;

    // Tạo socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Khởi tạo cấu trúc địa chỉ server
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    // Chuyển đổi địa chỉ IP từ chuỗi văn bản sang dạng nhị phân
    if (inet_pton(AF_INET, server_ip, &servaddr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        exit(EXIT_FAILURE);
    }

    // Kết nối tới server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Gửi thông điệp tới server
    printf("Enter message: ");
    fgets(sendline, MAXLINE, stdin);
    write(sockfd, sendline, strlen(sendline));

    // Đọc phản hồi từ server
    if ((n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = '\0';  // Kết thúc chuỗi nhận được
        printf("Server response: %s", recvline);
    } else {
        perror("Read failed");
    }

    close(sockfd);
    return 0;
}
