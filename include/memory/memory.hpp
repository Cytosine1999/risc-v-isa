#ifndef RISCV_ISA_MEMORY_HPP
#define RISCV_ISA_MEMORY_HPP


#include <cstring>
#include <cstdlib>
#include <zconf.h>
#include <sys/mman.h>


namespace riscv_isa {
    template<typename xlen=xlen_trait>
    class Memory {
    private:
        using XLenT = typename xlen::UXLenT;

        u8 *memory_offset;
        usize memory_size;

    public:

        Memory(usize _memory_size) : memory_size{_memory_size} {
            memory_offset = static_cast<u8 *>(mmap(nullptr, memory_size, PROT_READ | PROT_WRITE,
                                                       MAP_ANONYMOUS | MAP_SHARED, -1, 0));
            if (memory_offset == MAP_FAILED) {
                memory_offset = nullptr;
                memory_size = 0;
            }
        }

        Memory(const Memory &other) = delete;

        Memory &operator=(const Memory &other) = delete;

        template<typename T=void *>
        T *address(XLenT addr) {
            return addr < memory_size ? reinterpret_cast<T *>(memory_offset + addr) : nullptr;
        }

        bool memory_copy(XLenT offset, const void *src, usize length) {
            if (offset <= memory_size - length) {
                memcpy(memory_offset + offset, src, length);
                return true;
            } else {
                return false;
            }
        }

        ~Memory() { if (memory_offset != nullptr) munmap(memory_offset, memory_size); }
    };
}


#endif //RISCV_ISA_MEMORY_HPP
