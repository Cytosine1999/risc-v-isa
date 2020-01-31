#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "elf_header.hpp"

using namespace elf;


int main(int argc, char **argv) {
    if (argc != 2) std::cout << "receive one file name!" << std::endl;

    int fd = open(argv[1], O_RDONLY);

    if (fd == -1) std::cout << "open file fail!" << std::endl;

    struct stat file_stat{};

    if (fstat(fd, &file_stat) != 0) std::cout << "fstat file fail!" << std::endl;

    void *file = mmap(nullptr, file_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);

    MappedIOVisitor visitor{file};

    ELF32Header *elf_header = reinterpret_cast<ELF32Header *>(file);

    for (auto &sector: elf_header->sectors(visitor))
        std::cout << sector << std::endl;
}
