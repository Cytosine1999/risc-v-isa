#ifndef RISCV_ISA_MEMORY_HPP
#define RISCV_ISA_MEMORY_HPP


#include <cstring>
#include <cstdlib>
#include <zconf.h>
#include <sys/mman.h>


namespace riscv_isa {
    class Memory {
    private:
        usize page_size;
        usize memory_size;
        u8 *memory_offset;

    public:
        Memory(usize _memory_size) {
            isize _page_size = sysconf(_SC_PAGESIZE);

            if (_page_size <= 0) riscv_isa_abort("Unable to get page size or improper page size!");

            page_size = _page_size;

            memory_size = ((_memory_size + page_size - 1) / page_size) * page_size;

            memory_offset = static_cast<u8 *>(mmap(nullptr, memory_size + page_size, PROT_READ | PROT_WRITE,
                                                   MAP_ANONYMOUS | MAP_SHARED, -1, 0));
            if (memory_offset == MAP_FAILED) riscv_isa_abort("Memory map failed!");

            // todo: memory wrap around through interrupt
            if(mprotect(memory_offset + memory_size, page_size, PROT_NONE) != 0)
                riscv_isa_abort("Guard page set failed!");
        }

        Memory(const Memory &other) = delete;

        Memory &operator=(const Memory &other) = delete;

        template<typename LoadT>
        LoadT *address(UXLenT addr) {
            return addr < memory_size ? reinterpret_cast<LoadT *>(memory_offset + addr) : nullptr;
        }

        bool memory_copy(UXLenT offset, const void *src, usize length) {
            if (offset + length <= memory_size) {
                memcpy(memory_offset + offset, src, length);
                return true;
            } else {
                return false;
            }
        }

        ~Memory() { munmap(memory_offset, memory_size); }
    };
}


#endif //RISCV_ISA_MEMORY_HPP
