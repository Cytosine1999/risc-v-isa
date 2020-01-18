#ifndef RISC_V_ISA_MEMORY_HPP
#define RISC_V_ISA_MEMORY_HPP


#include <cstring>
#include <cstdlib>
#include <zconf.h>
#include <sys/mman.h>
#include <mach/vm_page_size.h>


namespace risc_v_isa {
    class Memory {
    private:
        usize memory_size;
        u8 *memory_offset;

    public:
        Memory(usize page_num) : memory_size{page_num * page_size()} {
            memory_offset = static_cast<u8 *>(mmap(nullptr, memory_size + page_size(), PROT_READ | PROT_WRITE,
                                                   MAP_ANONYMOUS | MAP_SHARED, -1, 0));
            if (memory_offset == MAP_FAILED) abort();

            mprotect(memory_offset + memory_size, page_size(), PROT_NONE);
        }

        Memory(const Memory &other) = delete;

        Memory &operator=(const Memory &other) = delete;

        template<typename LoadT>
        LoadT *address(UXLenT addr) {
            return addr < memory_size ? reinterpret_cast<LoadT *>(addr + memory_size) : nullptr;
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


#endif //RISC_V_ISA_MEMORY_HPP
