#ifndef RISCV_ISA_RV64M_HPP
#define RISCV_ISA_RV64M_HPP


#include "instruction.hpp"


#if defined(__RV_EXTENSION_M__) && __RV_BIT_WIDTH__ == 64
namespace riscv_isa {
    class InstructionMulDivWSet : public InstructionArithRegWSet {
    public:
        static constexpr InnerT FUNCT7 = 0b0000001;
    };

    class MULWInst : public InstructionMulDivWSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b000;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<MULW>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const MULWInst &inst) {
            stream << "\tmulw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class DIVWInst : public InstructionMulDivWSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b100;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<DIVW>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const DIVWInst &inst) {
            stream << "\tdivw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class DIVUWInst : public InstructionMulDivWSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b101;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<DIVUW>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const DIVUWInst &inst) {
            stream << "\tdivuw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class REMWInst : public InstructionMulDivWSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b110;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<REMW>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const REMWInst &inst) {
            stream << "\tremw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class REMUWInst : public InstructionMulDivWSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b111;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<REMUW>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const REMUWInst &inst) {
            stream << "\tremuw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };


}
#endif // defined(__RV_EXTENSION_M__) && __RV_BIT_WIDTH__ == 64


#endif //RISCV_ISA_RV64M_HPP
