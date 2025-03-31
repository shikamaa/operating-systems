#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <map>
#include <pthread.h>
#include <mutex>

std::map<int, std::string> users;
std::mutex users_mutex;

void send_to_all(const std::string& message, int sender_socket) {
    users_mutex.lock();
    for (const auto& [sock, name] : users) {
        send(sock, message.c_str(), message.length(), 0);
    }
    users_mutex.unlock();
}

void show_users(int client_socket) {
    std::string user_list = "Connected users:\n";
    users_mutex.lock();
    for (const auto& [sock, name] : users) user_list += name + "\n";
    users_mutex.unlock();
    send(client_socket, user_list.c_str(), user_list.length(), 0);
}

void* handle_client(void* arg) {
    int client_socket = *(int*)arg;
    char buffer[1024];
    ssize_t received;

    received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (received > 0) {
        std::string name(buffer, received);
        users_mutex.lock();
        users[client_socket] = name;
        users_mutex.unlock();

        std::string login_message = name + " logged in";
        std::cout << login_message << std::endl;
        send_to_all(login_message, client_socket);
    }

    std::cout << "Write your message. /list - see users, /exit - leave chat\n";

    while ((received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        std::string message(buffer, received);
        if (message == "/exit") break;
        else if (message == "/list") show_users(client_socket);
        else send_to_all(users[client_socket] + ": " + message, client_socket);
    }

    users_mutex.lock();
    std::string exit_message = users[client_socket] + " left the chat\n";
    users.erase(client_socket);
    users_mutex.unlock();

    std::cout << exit_message << std::endl;
    send_to_all(exit_message, client_socket);
    close(client_socket);
    return nullptr;
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) exit(errno);

    struct sockaddr_in server_address;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8888);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) exit(errno);
    if (listen(server_socket, 5) == -1) exit(errno);

    std::cout << "Server started on port 8888...\n";

    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket == -1) continue;

        pthread_t thread;
        pthread_create(&thread, nullptr, handle_client, &client_socket);
        pthread_detach(thread);
    }

    close(server_socket);
    return 0;
}
