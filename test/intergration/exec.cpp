#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "target/hart.hpp"

using namespace riscv_isa;

#include "utility.hpp"
#include "elf_header.hpp"

using namespace elf;


class LinuxHart : public Hart<LinuxHart> {
public:
    LinuxHart(RegisterFile &reg, Memory &mem) : Hart{reg, mem} {}

    void start() {
        while (true) {
            Instruction *inst = mem.address<Instruction>(reg.get_pc());

            switch (inst == nullptr ? MEMORY_ERROR : visit(inst)) {
                case ILLEGAL_INSTRUCTION_EXCEPTION:
                    std::cerr << "Illegal instruction at " << std::hex << reg.get_pc() << ' '
                              << *reinterpret_cast<u32 *>(inst) << std::endl;

                    return;
                case MEMORY_ERROR:
                    std::cerr << "Memory error at " << std::hex << reg.get_pc() << std::endl;

                    return;
                case ECALL:
                    switch (reg.get_x(RegisterFile::A7)) {
                        case 57: {
                            int fd = reg.get_x(RegisterFile::A0);
                            reg.set_x(RegisterFile::A0, fd > 2 ? close(fd) : 0); // todo: stdin, stdout, stderr

                            break;
                        }
                        case 64:
                            reg.set_x(RegisterFile::A0, write(reg.get_x(RegisterFile::A0),
                                                              mem.address<char>(reg.get_x(RegisterFile::A1)),
                                                              RegisterFile::A3));

                            break;
                        case 80:
                            reg.set_x(RegisterFile::A0, -1); // todo: need convert

                            break;
                        case 93:
                            std::cout << std::endl << "[exit " << reg.get_x(RegisterFile::A0) << ']' << std::endl;

                            return;
                        case 214:

                            break;
                        default:
                            std::cerr << "Invalid ecall number at " << std::hex << reg.get_pc()
                                      << ", call number " << std::dec << reg.get_x(RegisterFile::A7) << std::endl;

                            return;
                    }
                    reg.inc_pc(ECALLInst::INST_WIDTH);

                    break;
                case EBREAK:
                    reg.inc_pc(EBREAKInst::INST_WIDTH);

                    break;
                default:;
            }
        }
    }
};


int main(int argc, char **argv) {
    if (argc != 2) riscv_isa_abort("receive one file name!");

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) riscv_isa_abort("open file failed!");

    struct stat file_stat{};
    if (fstat(fd, &file_stat) != 0) riscv_isa_abort("fstat file failed!");

    void *file = mmap(nullptr, file_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (file == MAP_FAILED) riscv_isa_abort("Memory mapped io failed!");

    if (close(fd) != 0) riscv_isa_abort("Close file failed!");

    MappedIOVisitor visitor{file};

    ELF32Header *elf_header = elf::dyn_cast<ELF32Header>(file);
    if (elf_header == nullptr) riscv_isa_abort("Incompatible format or broken file!");

//    std::cout << *elf_header << std::endl;

    ELF32StringTableSectionHeader *string_table_header = elf::dyn_cast<ELF32StringTableSectionHeader>(
            &elf_header->sections(visitor)[elf_header->string_table_index]);
    if (string_table_header == nullptr) riscv_isa_abort("Broken string table!");
//    auto string_table = string_table_header->get_string_table(visitor);

    if (elf_header->file_type != ELF32Header::Executable) riscv_isa_abort("Not an executable file!");

    RegisterFile reg{};
    reg.set_pc(elf_header->entry_point);
    reg.set_x(RegisterFile::SP, 0xfffff000);

    Memory mem{0x100000000};

    for (auto &program: elf_header->programs(visitor)) {
//        std::cout << program << std::endl;

        if (program.type == ELF32ProgramHeader::Loadable)
            mem.memory_copy(program.virtual_address, static_cast<u8 *>(file) + program.offset, program.file_size);
    }

//    for (auto &section: elf_header->sections(visitor))
//        std::cout << string_table.get_str(section.name) << std::endl;

    LinuxHart core{reg, mem};
    core.start();
}
