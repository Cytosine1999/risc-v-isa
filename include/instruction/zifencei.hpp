#ifndef RISCV_ISA_ZIFENCEI_HPP
#define RISCV_ISA_ZIFENCEI_HPP


#include "instruction.hpp"


#if defined(__RV_EXTENSION_ZIFENCEI__)

namespace riscv_isa {
    class FENCEIInst : public InstructionFenceSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b001;

        template<typename RegT>
        void operator()(RegT &reg) const {
            // todo
        }
    };
}

#endif // defined(__RV_EXTENSION_ZIFENCEI__)


#endif //RISCV_ISA_ZIFENCEI_HPP
