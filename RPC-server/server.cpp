#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>

pthread_mutex_t cmutex = PTHREAD_MUTEX_INITIALIZER;

std::string process_command(const std::string& command) {
    std::istringstream iss(command);
    std::string op;
    int a, b;

    iss >> op >> a >> b;
    if (op == "ADD") return std::to_string(a + b);
    else if (op == "SUB") return std::to_string(a - b);
    else if (op == "MUL") return std::to_string(a * b);
    else if (op == "DIV") {
        if (b == 0) return "error: division by zero";
        return std::to_string(a / b);
    }
    else if (op == "MOD") {
        if (b == 0) return "error: division by zero";
        return std::to_string(a % b);
    }
    else return "error";
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("err");
        exit(errno);
    }
    
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(8888);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("bind failed");
        exit(errno);
    }

    if (listen(server_socket, 10) < 0) {
        perror("listen failed");
        exit(errno);
    }
    std::cout << "Waiting for connection\n";

    int client_socket;
    struct sockaddr_in client_address;
    unsigned int client_addr_len = sizeof(client_address);

    client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_addr_len);
    if (client_socket < 0) {
        perror("accept failed");
        exit(errno);
    }

    std::cout << "connected client with address: " << inet_ntoa(client_address.sin_addr) << "\n";

    char buffer[3001];

    while (true) {
        int rs = recv(client_socket, buffer, 3001, 0);
        if (rs <= 0) {
            pthread_mutex_lock(&cmutex);
            std::cout << "client disconnected\n";
            pthread_mutex_unlock(&cmutex);
            break;
        }
        buffer[rs] = '\0';

        std::string server_response = process_command(buffer);

        send(client_socket, server_response.c_str(), server_response.size(), 0);
        pthread_mutex_lock(&cmutex);
        std::cout << server_response << "\n";
        pthread_mutex_unlock(&cmutex);
    }
    
    close(client_socket);
    close(server_socket);
}
