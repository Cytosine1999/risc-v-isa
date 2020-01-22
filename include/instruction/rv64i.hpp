#ifndef RISC_V_ISA_RV64I_HPP
#define RISC_V_ISA_RV64I_HPP


#include "instruction.hpp"


#if defined(__RV_64_BIT__)
namespace risc_v_isa {
    class InstructionShiftImmWSet : public InstructionArithImmWSet {
    public:
        usize get_shift_amount() const { return slice_shift_amount(inner); }
        usize get_funct_shift() const { return slice_funct_shift(inner); }
    };

    class InstructionShiftRightImmWSet : public InstructionShiftImmWSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;
    };

    class InstructionIntegerRegWSet : public InstructionArithRegWSet {
    public:
        static constexpr UInnerT FUNC_7 = 0b0000000;
    };

    class InstructionIntegerRegModWSet : public InstructionArithRegWSet {
    public:
        static constexpr UInnerT FUNC_7 = 0b0100000;
    };

    class LDInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b011;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<i64>(reg, mem); }

        friend std::ostream &operator<<(std::ostream &stream, const LDInst &inst) {
            stream << "\tld\tx" << inst.get_rd() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class LWUInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b110;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<u32>(reg, mem); }

        friend std::ostream &operator<<(std::ostream &stream, const LWUInst &inst) {
            stream << "\tlwu\tx" << inst.get_rd() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class SDInst : public InstructionStoreSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b011;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<i64>(reg, mem); }

        friend std::ostream &operator<<(std::ostream &stream, const SDInst &inst) {
            stream << "\tsd\tx" << inst.get_rs2() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class ADDIWInst : public InstructionArithImmWSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<ADD>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const ADDIWInst &inst) {
            stream << "\taddiw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_imm();
            return stream;
        }
    };

    class SLLIWInst : public InstructionShiftImmWSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;
        static constexpr UInnerT FUNC_SHIFT = 0b000000000000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_shift<SLLW>(this, reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SLLIWInst &inst) {
            stream << "\tslliw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_shift_amount();
            return stream;
        }
    };

    class SRLIWInst : public InstructionShiftRightImmWSet {
    public:
        static constexpr UInnerT FUNC_SHIFT = 0b000000000000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_shift<SRLW>(this, reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SRLIWInst &inst) {
            stream << "\tsrliw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_shift_amount();
            return stream;
        }
    };

    class SRAIWInst : public InstructionShiftRightImmWSet {
    public:
        static constexpr UInnerT FUNC_SHIFT = 0b010000000000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_shift<SRAW>(this, reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SRAIWInst &inst) {
            stream << "\tsraiw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_shift_amount();
            return stream;
        }
    };

    class ADDWInst : public InstructionIntegerRegWSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<ADDW>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const ADDWInst &inst) {
            stream << "\taddw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SUBWInst : public InstructionIntegerRegModWSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SUBW>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SUBWInst &inst) {
            stream << "\tsubw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SLLWInst : public InstructionIntegerRegWSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SLLW>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SLLWInst &inst) {
            stream << "\tsllw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SRLWInst : public InstructionIntegerRegWSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SRLW>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SRLWInst &inst) {
            stream << "\tsrlw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SRAWInst : public InstructionIntegerRegModWSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SRAW>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SRAWInst &inst) {
            stream << "\tsraw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };
}
#endif // defined(__RV_64_BIT__)


#endif //RISC_V_ISA_RV64I_HPP
