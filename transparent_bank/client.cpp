#include <iostream>
#include <fcntl.h>
#include <cstdlib>
#include <sys/mman.h>
#include <cerrno>
#include <sys/stat.h>
#include "bank.hpp"
#include <unistd.h>

int main(int argc, char* argv[]) {
    const char* shm_path = "membank";
    int fd = shm_open(shm_path,O_RDWR,0600);
    struct stat sb;

    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        exit(EXIT_FAILURE);
    }
    size_t shm_size =sb.st_size;
    
    void* ptr = mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        exit(EXIT_FAILURE);
    }

    munmap(ptr, shm_size);
    close(fd);
}
