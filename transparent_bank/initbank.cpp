#include <iostream>
#include <sys/mman.h>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include "bank.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./initbank <number_of_accounts>\n";
        return 1;
    }

    const char* shm_path = "membank";
    int num_accounts = std::atoi(argv[1]);
    if (num_accounts <= 0 || num_accounts > 100) {
        std::cerr << "Incorrect number of accounts, must be 1-100\n";
        return 1;
    }

    size_t shm_size = sizeof(Bank);
    int fd = shm_open(shm_path, O_CREAT | O_RDWR, 0600);
    if (fd == -1) {
        perror("shm_open");
        return 1;
    }

    if (ftruncate(fd, shm_size) == -1) {
        perror("ftruncate");
        close(fd);
        return 1;
    }

    void* ptr = mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    new(ptr) Bank();
    std::cout << "Bank initialized with "<< num_accounts << " accounts\n";

    close(fd);
    munmap(ptr, shm_size);
    return 0;
}

