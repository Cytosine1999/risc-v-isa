#ifndef RISCV_ISA_RV32M_HPP
#define RISCV_ISA_RV32M_HPP


#include "instruction.hpp"


#if defined(__RV_EXTENSION_M__)
namespace riscv_isa {
    class InstructionMulDivSet : public InstructionArithRegSet {
    public:
        static constexpr InnerT FUNCT7 = 0b0000001;
    };

    class MULInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b000;

        friend std::ostream &operator<<(std::ostream &stream, const MULInst &inst) {
            stream << "mul\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class MULHInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b001;

        friend std::ostream &operator<<(std::ostream &stream, const MULHInst &inst) {
            stream << "mulh\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class MULHSUInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b010;

        friend std::ostream &operator<<(std::ostream &stream, const MULHSUInst &inst) {
            stream << "mulhsu\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class MULHUInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b011;

        friend std::ostream &operator<<(std::ostream &stream, const MULHUInst &inst) {
            stream << "mulhu\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class DIVInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b100;

        friend std::ostream &operator<<(std::ostream &stream, const DIVInst &inst) {
            stream << "div\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class DIVUInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b101;

        friend std::ostream &operator<<(std::ostream &stream, const DIVUInst &inst) {
            stream << "divu\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class REMInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b110;

        friend std::ostream &operator<<(std::ostream &stream, const REMInst &inst) {
            stream << "rem\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class REMUInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b111;

        friend std::ostream &operator<<(std::ostream &stream, const REMUInst &inst) {
            stream << "remu\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };
}
#endif // defined(__RV_EXTENSION_M__)


#endif //RISCV_ISA_RV32M_HPP
