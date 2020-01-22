#ifndef RISC_V_ISA_RV32M_HPP
#define RISC_V_ISA_RV32M_HPP


#include "instruction.hpp"


#if defined(__RV_EXTENSION_M__)
namespace risc_v_isa {
    class InstructionMulDivSet : public InstructionArithRegSet {
    public:
        static constexpr InnerT FUNC_7 = 0b0000001;
    };

    class MULInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b000;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<MUL>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const MULInst &inst) {
            stream << "\tmul\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class MULHInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b001;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<MULH>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const MULHInst &inst) {
            stream << "\tmulh\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class MULHSUInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b010;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<MULHSU>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const MULHSUInst &inst) {
            stream << "\tmulhsu\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class MULHUInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b011;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<MULHU>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const MULHUInst &inst) {
            stream << "\tmulhu\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class DIVInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b100;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<DIV>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const DIVInst &inst) {
            stream << "\tdiv\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class DIVUInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b101;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<DIVU>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const DIVUInst &inst) {
            stream << "\tdivu\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class REMInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b110;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<REM>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const REMInst &inst) {
            stream << "\trem\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class REMUInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b111;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<REMU>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const REMUInst &inst) {
            stream << "\tremu\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };
}
#endif // defined(__RV_EXTENSION_M__)


#endif //RISC_V_ISA_RV32M_HPP
