#ifndef RISCV_ISA_ZICSR_HPP
#define RISCV_ISA_ZICSR_HPP


#include "instruction.hpp"


#if defined(__RV_EXTENSION_ZICSR__)
namespace riscv_isa {
    class CSRRWInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b001;
    };

    class CSRRSInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b010;
    };

    class CSRRCInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b011;
    };

    class CSRRWIInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b101;
    };

    class CSRRSIInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b110;
    };

    class CSRRCIInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b111;
    };
}
#endif // defined(__RV_EXTENSION_ZICSR__)


#endif //RISCV_ISA_ZICSR_HPP
