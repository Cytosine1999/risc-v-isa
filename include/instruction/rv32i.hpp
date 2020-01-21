#ifndef RISC_V_ISA_RV32I_HPP
#define RISC_V_ISA_RV32I_HPP


#include "utility.hpp"
#include "instruction.hpp"
#include "register/register_file.hpp"
#include "memory/memory.hpp"


#if defined(__RV32I__) || defined(__RV64I__)
namespace risc_v_isa {
    namespace {
        constexpr UXLenT PTR_MASK = BITS_MASK<UXLenT, XLEN, 1>;

        struct EQ { static bool op(XLenT a, XLenT b) { return a == b; } };

        struct NE { static bool op(XLenT a, XLenT b) { return a != b; } };

        struct LT { static bool op(XLenT a, XLenT b) { return a < b; } };

        struct GE { static bool op(XLenT a, XLenT b) { return a >= b; } };

        struct LTU { static bool op(UXLenT a, UXLenT b) { return a < b; } };

        struct ADD { static XLenT op(XLenT a, XLenT b) { return a + b; } };

        struct SUB { static XLenT op(XLenT a, XLenT b) { return a - b; } };

        struct SLT { static XLenT op(XLenT a, XLenT b) { return a < b; } };

        struct SLTU { static XLenT op(UXLenT a, UXLenT b) { return a < b; } };

        struct XOR { static XLenT op(UXLenT a, UXLenT b) { return a ^ b; } };

        struct OR { static XLenT op(UXLenT a, UXLenT b) { return a | b; } };

        struct AND { static XLenT op(UXLenT a, UXLenT b) { return a & b; } };

        struct SLL { static XLenT op(UXLenT a, UXLenT b) { return a << b; } };

        struct SRL { static XLenT op(UXLenT a, UXLenT b) { return a >> b; } };

        struct SRA { static XLenT op(XLenT a, UXLenT b) { return a >> b; } };
    }

    class InstructionShiftImmSet : public InstructionArithImmSet {
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

    class InstructionShiftRightImmSet : public InstructionShiftImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;
    };

    class InstructionIntegerRegSet : public InstructionArithRegSet {
    public:
        static constexpr UInnerT FUNC_7 = 0b0000000;
    };

    class InstructionIntegerRegModSet : public InstructionArithRegSet {
    public:
        static constexpr UInnerT FUNC_7 = 0b0100000;
    };

    class InstructionEnvironmentSet : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        usize get_funct_environment() const { return inner & BITS_MASK<UInnerT, 32, 20>; }

        usize get_unused() const { return inner & (BITS_MASK<UInnerT, 12, 7> | BITS_MASK<UInnerT, 20, 15>); }
    };

    class LUIInst : public Instruction32U {
    public:
        static constexpr UInnerT OP_CODE = 0b0110111;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            size_t rd = get_rd();
            if (rd != 0) {
                XLenT imm = get_imm();
                reg.set_x(rd, imm);
            }
            reg.inc_pc(INST_WIDTH);
        }

        friend std::ostream &operator<<(std::ostream &stream, const LUIInst &inst) {
            stream << "\tlui\tx" << inst.get_rd() << ", " << inst.get_imm();
            return stream;
        }
    };

    class AUIPCInst : public Instruction32U {
    public:
        static constexpr UInnerT OP_CODE = 0b0010111;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            size_t rd = get_rd();
            if (rd != 0) {
                XLenT imm = get_imm();
                reg.set_x(rd, imm + reg.get_pc());
            }
            reg.inc_pc(INST_WIDTH);
        }

        friend std::ostream &operator<<(std::ostream &stream, const AUIPCInst &inst) {
            stream << "\tauipc\tx" << inst.get_rd() << ", " << inst.get_imm();
            return stream;
        }
    };

    class JALInst : public Instruction32J {
    public:
        static constexpr UInnerT OP_CODE = 0b1101111;

        template<typename RegT>
        bool operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            usize rd = get_rd();
            XLenT imm = get_imm();
            XLenT pc = reg.get_pc();
            if (rd != 0) reg.set_x(rd, pc + INST_WIDTH);
            UXLenT target = pc + imm;
#if !defined(INSTRUCTION_ADDRESS_MISALIGNED)
            if (get_slice<UXLenT , 2, 1>(target) != 0) return false;
#endif // !defined(INSTRUCTION_ADDRESS_MISALIGNED)
            reg.set_pc(target);
            return true;
        }

        friend std::ostream &operator<<(std::ostream &stream, const JALInst &inst) {
            stream << "\tjal\tx" << inst.get_rd() << ", " << inst.get_imm();
            return stream;
        }
    };

    class JALRInst : public Instruction32I {
    public:
        static constexpr UInnerT OP_CODE = 0b1100111;
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT>
        bool operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            usize rd = get_rd();
            usize rs1 = get_rs1();
            XLenT imm = get_slice<UInnerT, 32, 21, 1>(inner);
            if (rd != 0) reg.set_x(rd, reg.get_pc() + INST_WIDTH);
            UXLenT target = (reg.get_x(rs1) + imm) & PTR_MASK;
#if !defined(INSTRUCTION_ADDRESS_MISALIGNED)
            if (get_slice<UXLenT , 2, 1>(target) != 0) return false;
#endif // !defined(INSTRUCTION_ADDRESS_MISALIGNED)
            reg.set_pc(target);
            return true;
        }

        friend std::ostream &operator<<(std::ostream &stream, const JALRInst &inst) {
            stream << "\tjalr\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_imm();
            return stream;
        }
    };

    class BEQInst : public InstructionBranchSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<EQ>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const BEQInst &inst) {
            stream << "\tbeq\tx" << inst.get_rs1() << ", x" << inst.get_rs2() << ", " << inst.get_imm();
            return stream;
        }
    };

    class BNEInst : public InstructionBranchSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<NE>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const BNEInst &inst) {
            stream << "\tbne\tx" << inst.get_rs1() << ", x" << inst.get_rs2() << ", " << inst.get_imm();
            return stream;
        }
    };

    class BLTInst : public InstructionBranchSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b100;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<LT>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const BLTInst &inst) {
            stream << "\tblt\tx" << inst.get_rs1() << ", x" << inst.get_rs2() << ", " << inst.get_imm();
            return stream;
        }
    };

    class BGEInst : public InstructionBranchSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<GE>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const BGEInst &inst) {
            stream << "\tbne\tx" << inst.get_rs1() << ", x" << inst.get_rs2() << ", " << inst.get_imm();
            return stream;
        }
    };

    class BLTUInst : public InstructionBranchSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b110;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<LTU>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const BLTUInst &inst) {
            stream << "\tbltu\tx" << inst.get_rs1() << ", x" << inst.get_rs2() << ", " << inst.get_imm();
            return stream;
        }
    };

    class BGEUInst : public InstructionBranchSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b111;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<LTU>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const BGEUInst &inst) {
            stream << "\tbgeu\tx" << inst.get_rs1() << ", x" << inst.get_rs2() << ", " << inst.get_imm();
            return stream;
        }
    };

    class LBInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<i8>(reg, mem); }

        friend std::ostream &operator<<(std::ostream &stream, const LBInst &inst) {
            stream << "\tlb\tx" << inst.get_rd() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class LHInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<i16>(reg, mem); }

        friend std::ostream &operator<<(std::ostream &stream, const LHInst &inst) {
            stream << "\tlh\tx" << inst.get_rd() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class LWInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b010;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<i32>(reg, mem); }

        friend std::ostream &operator<<(std::ostream &stream, const LWInst &inst) {
            stream << "\tlw\tx" << inst.get_rd() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class LBUInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b100;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<u8>(reg, mem); }

        friend std::ostream &operator<<(std::ostream &stream, const LBUInst &inst) {
            stream << "\tlbu\tx" << inst.get_rd() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class LHUInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<u16>(reg, mem); }

        friend std::ostream &operator<<(std::ostream &stream, const LHUInst &inst) {
            stream << "\tlhu\tx" << inst.get_rd() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class SBInst : public InstructionStoreSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<i8>(reg, mem); }

        friend std::ostream &operator<<(std::ostream &stream, const SBInst &inst) {
            stream << "\tsb\tx" << inst.get_rs2() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class SHInst : public InstructionStoreSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<i16>(reg, mem); }

        friend std::ostream &operator<<(std::ostream &stream, const SHInst &inst) {
            stream << "\tsh\tx" << inst.get_rs2() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class SWInst : public InstructionStoreSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b010;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<i32>(reg, mem); }

        friend std::ostream &operator<<(std::ostream &stream, const SWInst &inst) {
            stream << "\tsw\tx" << inst.get_rs2() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class ADDIInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<ADD>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const ADDIInst &inst) {
            stream << "\taddi\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_imm();
            return stream;
        }
    };

    class SLTIInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b010;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SLT>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SLTIInst &inst) {
            stream << "\tslti\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_imm();
            return stream;
        }
    };

    class SLTIUInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b011;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SLTU>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SLTIUInst &inst) {
            stream << "\tsltiu\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_imm();
            return stream;
        }
    };

    class XORIInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b100;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<XOR>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const XORIInst &inst) {
            stream << "\txori\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_imm();
            return stream;
        }
    };

    class ORIInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b110;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<OR>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const ORIInst &inst) {
            stream << "\tori\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_imm();
            return stream;
        }
    };

    class ANDIInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b111;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<AND>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const ANDIInst &inst) {
            stream << "\tandi\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_imm();
            return stream;
        }
    };

    class SLLIInst : public InstructionShiftImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;
        static constexpr UInnerT FUNC_SHIFT = 0b000000000000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SLL>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SLLIInst &inst) {
            stream << "\tslli\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_shift_amount();
            return stream;
        }
    };

    class SRLIInst : public InstructionShiftRightImmSet {
    public:
        static constexpr UInnerT FUNC_SHIFT = 0b000000000000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SRL>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SRLIInst &inst) {
            stream << "\tsrli\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_shift_amount();
            return stream;
        }
    };

    class SRAIInst : public InstructionShiftRightImmSet {
    public:
        static constexpr UInnerT FUNC_SHIFT = 0b010000000000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SRA>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SRAIInst &inst) {
            stream << "\tsrai\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_shift_amount();
            return stream;
        }
    };

    class ADDInst : public InstructionIntegerRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<ADD>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const ADDInst &inst) {
            stream << "\tadd\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SUBInst : public InstructionIntegerRegModSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SUB>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SUBInst &inst) {
            stream << "\tsub\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SLLInst : public InstructionIntegerRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SLL>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SLLInst &inst) {
            stream << "\tsll\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SLTInst : public InstructionIntegerRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b010;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SLT>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SLTInst &inst) {
            stream << "\tslt\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SLTUInst : public InstructionIntegerRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b011;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SLTU>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SLTUInst &inst) {
            stream << "\tsltu\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class XORInst : public InstructionIntegerRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b100;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<XOR>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const XORInst &inst) {
            stream << "\txor\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SRLInst : public InstructionIntegerRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SRL>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SRLInst &inst) {
            stream << "\tsrl\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SRAInst : public InstructionIntegerRegModSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SRA>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const SRAInst &inst) {
            stream << "\tsra\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class ORInst : public InstructionIntegerRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b110;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<OR>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const ORInst &inst) {
            stream << "\tor\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class ANDInst : public InstructionIntegerRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b111;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<AND>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const ANDInst &inst) {
            stream << "\tand\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class FENCEInst : public InstructionFenceSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        friend std::ostream &operator<<(std::ostream &stream, const FENCEInst &inst) {
            stream << "\tfence";
            return stream;
        }
    };

    class ECALLInst : public InstructionEnvironmentSet {
    public:
        static constexpr UInnerT FUNCT_ENVIRONMENT = 0b000000000000;

        friend std::ostream &operator<<(std::ostream &stream, const ECALLInst &inst) {
            stream << "\tecall";
            return stream;
        }
    };

    class EBREAKInst : public InstructionEnvironmentSet {
    public:
        static constexpr UInnerT FUNCT_ENVIRONMENT = 0b000000000001;

        friend std::ostream &operator<<(std::ostream &stream, const EBREAKInst &inst) {
            stream << "\tebreak";
            return stream;
        }
    };
}
#endif // defined(__RV32I__) || defined(__RV64I__)


#endif //RISC_V_ISA_RV32I_HPP
