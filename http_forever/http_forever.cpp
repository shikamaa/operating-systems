#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define HOST "httpforever.com"
#define PORT 80
#define REQUEST "GET / HTTP/1.1\r\nHost: httpforever.com\r\nConnection: close\r\n\r\n"

int main() {
    struct hostent *host = gethostbyname(HOST);
    if (!host) {
        std::cerr << "Error: could not get host IP address" << std::endl;
        exit(errno);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation error");
        exit(errno);
    }

    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    std::memcpy(&server_addr.sin_addr, host->h_addr, host->h_length);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection error");
        close(sock);
        exit(errno);
    }

    if (send(sock, REQUEST, strlen(REQUEST), 0) == -1) {
        perror("Request sending error");
        close(sock);
        exit(errno);
    }

    std::ofstream output_file("httpforever.html", std::ios::binary);
    if (!output_file) {
        std::cerr << "Error: could not open file for writing" << std::endl;
        close(sock);
        exit(errno);
    }

    char buffer[4096];
    ssize_t bytes_received;
    while ((bytes_received = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        output_file.write(buffer, bytes_received);
    }

    if (bytes_received == -1) {
        perror("Data receiving error");
    }

    close(sock);
    output_file.close();

    std::cout << "Response saved in httpforever.html" << std::endl;
    return 0;
}
