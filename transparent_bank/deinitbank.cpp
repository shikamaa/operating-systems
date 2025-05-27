#include <iostream>
#include <sys/stat.h>
#include <cerrno>
#include <unistd.h>
#include "bank.hpp"
#include <sys/mman.h>

int main() {
    const char* shm_path = "membank";
    std::cout << "Destroying bank...\n";

    if (shm_unlink(shm_path) == -1) {
        perror("shm_unlink");
        return 1;
    }

    std::cout << "Bank destroyed successfully.\n";
    return 0;
}
