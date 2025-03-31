#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <pthread.h>

int client_socket;

void* receive_messages(void*) {
    char buffer[1024];
    ssize_t received;
    while ((received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        std::cout << std::string(buffer, received) << std::endl;
    }
    return nullptr;
}

int main() {
    struct sockaddr_in server_address;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) exit(errno);

    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8888);

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) exit(errno);

    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);
    send(client_socket, username.c_str(), username.size(), 0);

    pthread_t recv_thread;
    pthread_create(&recv_thread, nullptr, receive_messages, nullptr);
    pthread_detach(recv_thread);

    std::string message;
    while (true) {
        std::getline(std::cin, message);
        send(client_socket, message.c_str(), message.size(), 0);
        if (message == "/exit") break;
    }

    close(client_socket);
    return 0;
}
