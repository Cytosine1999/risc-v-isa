#ifndef RISC_V_ISA_RV64I_HPP
#define RISC_V_ISA_RV64I_HPP


#include "instruction.hpp"


#if defined(__RV64I__)
namespace risc_v_isa {
    namespace {
        struct ADD32 { static i32 op(i32 a, i32 b) { return a + b; } };

        struct SUB32 { static i32 op(i32 a, i32 b) { return a - b; } };

        struct SLL32 { static i32 op(u32 a, u32 b) { return a << b; } };

        struct SRL32 { static i32 op(u32 a, u32 b) { return a >> b; } };

        struct SRA32 { static i32 op(i32 a, u32 b) { return a >> b; } };
    }

    class InstructionArithImmWSet : public Instruction32I {
    protected:
        template<typename OP, typename RegT>
        void operate_on(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            usize rd = get_rd();
            if (rd != 0) {
                usize rs1 = get_rs1();
                XLenT imm = get_imm();
                reg.set_x(rd, OP::op(reg.get_x(rs1), imm));
            }
            reg.inc_pc(INST_WIDTH);
        }

    public:
        static constexpr UInnerT OP_CODE = 0b0011011;
    };

    class InstructionShiftImmWSet : public InstructionArithImmWSet {
    protected:
        template<typename OP, typename RegT>
        void operate_on(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            usize rd = get_rd();
            if (rd != 0) {
                usize rs1 = get_rs1();
                XLenT imm = get_shift_amount();
                reg.set_x(rd, OP::op(reg.get_x(rs1), imm));
            }
            reg.inc_pc(INST_WIDTH);
        }

    public:
        usize get_shift_amount() const { return slice_shift_amount(inner); }
        usize get_funct_shift() const { return slice_funct_shift(inner); }
    };

    class InstructionShiftRightImmWSet : public InstructionShiftImmWSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;
    };

    class InstructionArithRegWSet : public Instruction32R {
    protected:
        template<typename OP, typename RegT>
        void operate_on(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            usize rd = get_rd();
            if (rd != 0) {
                usize rs1 = get_rs1();
                usize rs2 = get_rs2();
                reg.set_x(rd, OP::op(reg.get_x(rs1), reg.get_x(rs2)));
            }
            reg.inc_pc(INST_WIDTH);
        }

    public:
        static constexpr UInnerT OP_CODE = 0b0111011;

        InstructionArithRegWSet(usize rd, usize funct3, usize rs1, usize rs2, usize funct7)
                : Instruction32R{OP_CODE, rd, funct3, rs1, rs2, funct7} {}
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
        void operator()(RegT &reg) const { operate_on<SLL32>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SLLIWInst &inst) {
            stream << "\tslliw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_shift_amount();
            return stream;
        }
    };

    class SRLIWInst : public InstructionShiftRightImmWSet {
    public:
        static constexpr UInnerT FUNC_SHIFT = 0b000000000000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SRL32>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SRLIWInst &inst) {
            stream << "\tsrliw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_shift_amount();
            return stream;
        }
    };

    class SRAIWInst : public InstructionShiftRightImmWSet {
    public:
        static constexpr UInnerT FUNC_SHIFT = 0b010000000000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SRA32>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SRAIWInst &inst) {
            stream << "\tsraiw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_shift_amount();
            return stream;
        }
    };

    class ADDWInst : public InstructionIntegerRegWSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<ADD32>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const ADDWInst &inst) {
            stream << "\taddw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SUBWInst : public InstructionIntegerRegModWSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SUB32>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SUBWInst &inst) {
            stream << "\tsubw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SLLWInst : public InstructionIntegerRegWSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SLL32>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SLLWInst &inst) {
            stream << "\tsllw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SRLWInst : public InstructionIntegerRegWSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SRL32>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SRLWInst &inst) {
            stream << "\tsrlw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SRAWInst : public InstructionIntegerRegModWSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SRA32>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SRAWInst &inst) {
            stream << "\tsraw\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };
}
#endif // defined(__RV64I__)


#endif //RISC_V_ISA_RV64I_HPP
