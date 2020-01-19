#ifndef RISC_V_ISA_RV32M_HPP
#define RISC_V_ISA_RV32M_HPP


#include "instruction.hpp"


namespace risc_v_isa {
    class InstructionMulDivSet : public InstructionArithRegSet {
    public:
        static constexpr InnerT FUNC_7 = 0b0000001;
    };

    class MULInst : public InstructionMulDivSet {

    };
}


#endif //RISC_V_ISA_RV32M_HPP
