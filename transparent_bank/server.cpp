#include <iostream>
#include <pthread.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include "bank.hpp"
#include <sys/mman.h>
#include <fcntl.h>

Bank* bank;
pthread_mutex_t req_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t req_cond = PTHREAD_COND_INITIALIZER;
int request_count = 0;
bool server_running = true;

void* client_handler(void* arg) {
    int client_sock = *(int*)arg;
    delete (int*)arg;

    std::string welcome_msg = "[SERVER] Welcome! There are " + std::to_string(bank->size()) + " accounts available.\n"
                              "Type 'help' to see available commands.\n";
    write(client_sock, welcome_msg.c_str(), welcome_msg.size());

    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t len = read(client_sock, buffer, sizeof(buffer) - 1);
        if (len <= 0) break;

        std::string cmd(buffer);
        std::string response = "[SERVER] Unknown command. \n";

        if (cmd.find("deposit") == 0) {
            int id, amt;
            if (sscanf(buffer, "deposit %d %d", &id, &amt) == 2) {
                bank->get_account(id).deposit(amt);
                response = "[SERVER] Deposited " + std::to_string(amt) + " to account #" + std::to_string(id) + "\n";
            } else {
                response = "[SERVER] Usage: deposit <id> <amount>\n";
            }

        } else if (cmd.find("withdraw") == 0) {
            int id, amt;
            if (sscanf(buffer, "withdraw %d %d", &id, &amt) == 2) {
                bool ok = bank->get_account(id).withdraw(amt);
                response = ok
                    ? "[SERVER] Withdrew " + std::to_string(amt) + " from account #" + std::to_string(id) + "\n"
                    : "[SERVER] Withdrawal failed. Not enough funds.\n";
            } else {
                response = "[SERVER] Usage: withdraw <id> <amount>\n";
            }

        } else if (cmd.find("balance") == 0) {
            int id;
            if (sscanf(buffer, "balance %d", &id) == 1) {
                int b = bank->get_account(id).get_balance();
                response = "[SERVER] Account #" + std::to_string(id) + " balance is " + std::to_string(b) + "\n";
            } else {
                response = "[SERVER] Usage: balance <id>\n";
            }

        } 
        else if (cmd.find("shutdown") == 0) {
            server_running = false;
            response = "[SERVER] Shutting down...\n";
        }

        write(client_sock, response.c_str(), response.size());

        pthread_mutex_lock(&req_mutex);
        request_count++;
        if (request_count % 5 == 0) {
            pthread_cond_signal(&req_cond);
        }
        pthread_mutex_unlock(&req_mutex);

        if (!server_running) break;
    }

    close(client_sock);
    return nullptr;
}


void* logger_thread(void*) {
    while (server_running) {
        pthread_mutex_lock(&req_mutex);
        pthread_cond_wait(&req_cond, &req_mutex);
        std::cout << "[LOG] Requests processed: " << request_count << "\n";
        pthread_mutex_unlock(&req_mutex);
    }
    return nullptr;
}

int main() {
    const char* shm_path = "membank";
    int fd = shm_open(shm_path, O_RDWR, 0600);
    void* ptr = mmap(nullptr, sizeof(Bank), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    bank = reinterpret_cast<Bank*>(ptr);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);
    std::cout << "[SERVER] Listening on port 8888...\n";

    pthread_t log_thread;
    pthread_create(&log_thread, nullptr, logger_thread, nullptr);

    while (true) {
        sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int* client_fd = new int(accept(server_fd, (sockaddr*)&client_addr, &len));
        pthread_t tid;
        pthread_create(&tid, nullptr, client_handler, client_fd);
        pthread_detach(tid);
    }


    pthread_mutex_destroy(&req_mutex);
    pthread_cond_destroy(&req_cond);
    munmap(ptr, sizeof(Bank));
    close(server_fd);
    return 0;
}
