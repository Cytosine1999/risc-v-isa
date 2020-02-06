#ifndef RISCV_ISA_RV64I_HPP
#define RISCV_ISA_RV64I_HPP


#include "instruction.hpp"


#if __RV_BIT_WIDTH__ == 64
namespace riscv_isa {
    template<typename xlen=xlen_trait>
    class InstructionShiftImmWSet : public InstructionArithImmWSet {
    public:
        usize get_shift_amount() const { return slice_shift_amount<xlen>(inner); }
        usize get_funct_shift() const { return slice_funct_shift<xlen>(inner); }
    };

    template<typename xlen=xlen_trait>
    class InstructionShiftRightImmWSet : public InstructionShiftImmWSet<xlen> {
    public:
        using UInnerT = typename InstructionShiftImmSet<xlen>::UInnerT;

        static constexpr UInnerT FUNCT3 = 0b101;
    };

    class InstructionIntegerRegWSet : public InstructionArithRegWSet {
    public:
        static constexpr UInnerT FUNCT7 = 0b0000000;
    };

    class InstructionIntegerRegModWSet : public InstructionArithRegWSet {
    public:
        static constexpr UInnerT FUNCT7 = 0b0100000;
    };

    class LDInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b011;

        friend std::ostream &operator<<(std::ostream &stream, const LDInst &inst) {
            stream << "\tld\tx" << inst.get_rd() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class LWUInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b110;

        friend std::ostream &operator<<(std::ostream &stream, const LWUInst &inst) {
            stream << "\tlwu\tx" << inst.get_rd() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class SDInst : public InstructionStoreSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b011;

        friend std::ostream &operator<<(std::ostream &stream, const SDInst &inst) {
            stream << "\tsd\tx" << inst.get_rs2() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class ADDIWInst : public InstructionArithImmWSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b000;

        friend std::ostream &operator<<(std::ostream &stream, const ADDIWInst &inst) {
            stream << "\taddiw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_imm();
            return stream;
        }
    };

    template<typename xlen=xlen_trait>
    class SLLIWInst : public InstructionShiftImmWSet<xlen> {
    public:
        using UInnerT = typename InstructionShiftImmSet<xlen>::UInnerT;

        static constexpr UInnerT FUNCT3 = 0b001;
        static constexpr UInnerT FUNCT_SHIFT = 0b000000000000;

        friend std::ostream &operator<<(std::ostream &stream, const SLLIWInst &inst) {
            stream << "\tslliw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_shift_amount();
            return stream;
        }
    };

    template<typename xlen=xlen_trait>
    class SRLIWInst : public InstructionShiftRightImmWSet<xlen> {
    public:
        using UInnerT = typename InstructionShiftRightImmSet<xlen>::UInnerT;
        static constexpr UInnerT FUNCT3 = InstructionShiftRightImmSet<xlen>::FUNCT3;

        static constexpr UInnerT FUNCT_SHIFT = 0b000000000000;

        friend std::ostream &operator<<(std::ostream &stream, const SRLIWInst &inst) {
            stream << "\tsrliw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_shift_amount();
            return stream;
        }
    };

    template<typename xlen=xlen_trait>
    class SRAIWInst : public InstructionShiftRightImmWSet<xlen> {
    public:
        using UInnerT = typename InstructionShiftRightImmSet<xlen>::UInnerT;
        static constexpr UInnerT FUNCT3 = InstructionShiftRightImmSet<xlen>::FUNCT3;

        static constexpr UInnerT FUNCT_SHIFT = 0b010000000000;

        friend std::ostream &operator<<(std::ostream &stream, const SRAIWInst &inst) {
            stream << "\tsraiw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_shift_amount();
            return stream;
        }
    };

    class ADDWInst : public InstructionIntegerRegWSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b000;

        friend std::ostream &operator<<(std::ostream &stream, const ADDWInst &inst) {
            stream << "\taddw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SUBWInst : public InstructionIntegerRegModWSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b000;

        friend std::ostream &operator<<(std::ostream &stream, const SUBWInst &inst) {
            stream << "\tsubw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SLLWInst : public InstructionIntegerRegWSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b001;

        friend std::ostream &operator<<(std::ostream &stream, const SLLWInst &inst) {
            stream << "\tsllw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SRLWInst : public InstructionIntegerRegWSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b101;

        friend std::ostream &operator<<(std::ostream &stream, const SRLWInst &inst) {
            stream << "\tsrlw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SRAWInst : public InstructionIntegerRegModWSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b101;

        friend std::ostream &operator<<(std::ostream &stream, const SRAWInst &inst) {
            stream << "\tsraw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };
}
#endif // __RV_BIT_WIDTH__ == 64


#endif //RISCV_ISA_RV64I_HPP
