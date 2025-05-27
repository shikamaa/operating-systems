#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect");
        return 1;
    }

    std::cout << "Connected to server.\n"
              << "Available commands:\n"
              << "  deposit <id> <amount>\n"
              << "  withdraw <id> <amount>\n"
              << "  balance <id>\n"
              << "  exit\n";

    std::string line;
    char buffer[1024];

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        if (line == "exit") break;

        send(sock, line.c_str(), line.size(), 0);
        ssize_t bytes = recv(sock, buffer, sizeof(buffer)-1, 0);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';
        std::cout << "Server: " << buffer;
    }

    close(sock);
    return 0;
}

