#include "target/dump.hpp"


namespace riscv_isa {
    std::ostream &operator<<(std::ostream &stream, const Instruction &inst) {
        Dump dump{stream};
        dump.visit_in_memory(const_cast<Instruction *>(&inst), RISCV_ILEN / 8);
        return stream;
    }
}
