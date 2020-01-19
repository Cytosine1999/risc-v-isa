#ifndef RISC_V_ISA_RV32M_HPP
#define RISC_V_ISA_RV32M_HPP


#include "instruction.hpp"


#if defined(__RV32M__) || defined(__RV64M__)
namespace risc_v_isa {
    class InstructionMulDivSet : public InstructionArithRegSet {
    public:
        static constexpr InnerT FUNC_7 = 0b0000001;
    };

    class MULInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b000;
    };

    class MULHInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b001;
    };

    class MULHSUInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b010;
    };

    class MULHUInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b011;
    };

    class DIVInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b100;
    };

    class DIVUInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b101;
    };

    class REMInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b110;
    };

    class REMUInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b111;
    };
}
#endif // defined(__RV32M__) || defined(__RV64M__)


#endif //RISC_V_ISA_RV32M_HPP
