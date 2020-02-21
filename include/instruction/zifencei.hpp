#ifndef RISCV_ISA_ZIFENCEI_HPP
#define RISCV_ISA_ZIFENCEI_HPP


#include "instruction.hpp"


#if defined(__RV_EXTENSION_ZIFENCEI__)
namespace riscv_isa {
    class FENCEIInst : public InstructionFenceSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b001;
    };
}


#define riscv_isa_instruction_zifencei_map(func) \
    func(FENCEI, fencei)
#else
#define riscv_isa_instruction_zifencei_map(func)
#endif // defined(__RV_EXTENSION_ZIFENCEI__)


#endif //RISCV_ISA_ZIFENCEI_HPP
