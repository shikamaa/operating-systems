#include <iostream>
#include <sys/mman.h>
#include <cstdlib>
#include <array>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h> 
#include "bank.hpp"


int main(int argc, char* argv[]) {

    const char* shm_path = "membank";
    int num_accounts = std::atoi(argv[1]);
    if (num_accounts < 0 || num_accounts > 100) {
        std::cerr << "Incorrect number of accounts, must be 1-100 \n";
        exit(EXIT_FAILURE);
    }
    size_t shm_size = sizeof(Bank);

    int fd = shm_open(shm_path, O_CREAT | O_RDWR, 0600);
    if (fd == -1) 
        exit(EXIT_FAILURE);

    if (ftruncate(fd, shm_size) == -1)
        exit(EXIT_FAILURE);

    void* ptr = mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
        exit(EXIT_FAILURE);
    
    Bank* bank = new(ptr) Bank();
    std::cout << "bank initialized \n";

    close(fd);
    munmap(ptr,shm_size);
    return EXIT_SUCCESS;
}
