#ifndef RISC_V_ISA_RV32I_HPP
#define RISC_V_ISA_RV32I_HPP


#include "utility.hpp"
#include "instruction.hpp"
#include "register/register_file.hpp"
#include "memory/memory.hpp"


#if defined(__RV32I__) || defined(__RV64I__)
namespace risc_v_isa {
    namespace {
        struct EQ {
            static bool op(XLenT a, XLenT b) { return a == b; }
        };

        struct NE {
            static bool op(XLenT a, XLenT b) { return a != b; }
        };

        struct LT {
            static bool op(XLenT a, XLenT b) { return a < b; }
        };

        struct GE {
            static bool op(XLenT a, XLenT b) { return a >= b; }
        };

        struct LTU {
            static bool op(UXLenT a, UXLenT b) { return a < b; }
        };

        struct ADD {
            static XLenT op(XLenT a, XLenT b) { return a + b; }
        };

        struct SUB {
            static XLenT op(XLenT a, XLenT b) { return a - b; }
        };

        struct SLT {
            static XLenT op(XLenT a, XLenT b) { return a < b; }
        };

        struct SLTU {
            static XLenT op(UXLenT a, UXLenT b) { return a < b; }
        };

        struct XOR {
            static XLenT op(UXLenT a, UXLenT b) { return a ^ b; }
        };

        struct OR {
            static XLenT op(UXLenT a, UXLenT b) { return a | b; }
        };

        struct AND {
            static XLenT op(UXLenT a, UXLenT b) { return a & b; }
        };

        struct SLL {
            static XLenT op(UXLenT a, UXLenT b) { return a << b; }
        };

        struct SRL {
            static XLenT op(UXLenT a, UXLenT b) { return a >> b; }
        };

        struct SRA {
            static XLenT op(XLenT a, UXLenT b) { return a >> b; }
        };
    }

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
    };

    class JALInst : public Instruction32J {
    public:
        static constexpr UInnerT OP_CODE = 0b1101111;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            usize rd = get_rd();
            XLenT imm = get_imm();
            XLenT pc = reg.get_pc();
            if (rd != 0) reg.set_x(rd, pc + INST_WIDTH);
            reg.set_pc(pc + imm);
        }
    };

    class JALRInst : public Instruction32I {
    public:
        static constexpr UInnerT OP_CODE = 0b1100111;
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            usize rd = get_rd();
            usize rs1 = get_rs1();
            XLenT imm = get_slice<UInnerT, 32, 21, 1>(inner);
            if (rd != 0) reg.set_x(rd, reg.get_pc() + INST_WIDTH);
            reg.inc_pc(imm);
        }
    };

    class BEQInst : public InstructionBranchSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<EQ>(reg); }
    };

    class BNEInst : public InstructionBranchSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<NE>(reg); }
    };

    class BLTInst : public InstructionBranchSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b100;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<LT>(reg); }
    };

    class BGEInst : public InstructionBranchSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<GE>(reg); }
    };

    class BLTUInst : public InstructionBranchSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b110;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<LTU>(reg); }
    };

    class BGEUInst : public InstructionBranchSet {
    private:
        struct LTU {
            static XLenT op(UXLenT a, UXLenT b) { return a >= b; }
        };

    public:
        static constexpr UInnerT FUNC_3 = 0b111;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<LTU>(reg); }
    };

    class LBInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<i8>(reg, mem); }
    };

    class LHInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<i16>(reg, mem); }
    };

    class LWInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b010;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<i32>(reg, mem); }
    };

    class LBUInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b100;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<u8>(reg, mem); }
    };

    class LHUInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<u16>(reg, mem); }
    };

    class SBInst : public InstructionStoreSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<i8>(reg, mem); }
    };

    class SHInst : public InstructionStoreSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<i16>(reg, mem); }
    };

    class SWInst : public InstructionStoreSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b010;

        template<typename RegT, typename MemT>
        bool operator()(RegT &reg, MemT &mem) const { return operate_on<i32>(reg, mem); }
    };

    class ADDIInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<ADD>(reg); }
    };

    class SLTIInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b010;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SLT>(reg); }
    };

    class SLTIUInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b011;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SLTU>(reg); }
    };

    class XORIInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b100;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<XOR>(reg); }
    };

    class ORIInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b110;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<OR>(reg); }
    };

    class ANDIInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b111;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<AND>(reg); }
    };

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
        usize get_shift_amount() const { return slice_shift_amount32(inner); }

    };

    class SLLIWInst : public InstructionShiftImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SLL>(reg); }
    };

    class InstructionSrliwSraiw : public InstructionShiftImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

        usize get_funct_shift() const { return slice_funct_shift32(inner); }

    };

    class SRLIWInst : public InstructionSrliwSraiw {
    public:
        static constexpr UInnerT FUNC_SHIFT = 0b0000000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SRL>(reg); }
    };

    class SRAIWInst : public InstructionSrliwSraiw {
    public:
        static constexpr UInnerT FUNC_SHIFT = 0b0100000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SRA>(reg); }
    };

    class InstructionIntegerRegSet : public InstructionArithRegSet {
    public:
        static constexpr UInnerT FUNC_7 = 0b0000000;
    };

    class InstructionIntegerRegModSet : public InstructionArithRegSet {
    public:
        static constexpr UInnerT FUNC_7 = 0b0100000;
    };

    class ADDInst : public InstructionIntegerRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<ADD>(reg); }
    };

    class SUBInst : public InstructionIntegerRegModSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SUB>(reg); }
    };

    class SLLWInst : public InstructionIntegerRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SLL>(reg); }
    };

    class SLTInst : public InstructionIntegerRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b010;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SLT>(reg); }
    };

    class SLTUInst : public InstructionIntegerRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b011;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SLTU>(reg); }
    };

    class XORInst : public InstructionIntegerRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b100;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<XOR>(reg); }
    };

    class SRLWInst : public InstructionIntegerRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SRL>(reg); }
    };

    class SRAWInst : public InstructionIntegerRegModSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<SRA>(reg); }
    };

    class ORInst : public InstructionIntegerRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b110;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<OR>(reg); }
    };

    class ANDInst : public InstructionIntegerRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b111;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<AND>(reg); }
    };

    class FENCEInst : public InstructionFenceSet { // todo: deal with modification
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        usize get_unused() const { return inner & (BITS_MASK<UInnerT, 32, 28>); }
    };

    class InstructionEnvironmentSet : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        usize get_funct_environment() const { return inner & BITS_MASK<UInnerT, 32, 20>; }

        usize get_unused() const { return inner & (BITS_MASK<UInnerT, 12, 7> | BITS_MASK<UInnerT, 20, 15>); }
    };

    class ECALLInst : public InstructionEnvironmentSet {
    public:
        static constexpr UInnerT FUNCT_ENVIRONMENT = 0b000000000000;
    };

    class EBREAKInst : public InstructionEnvironmentSet {
    public:
        static constexpr UInnerT FUNCT_ENVIRONMENT = 0b000000000001;
    };
}
#endif // defined(__RV32I__) || defined(__RV64I__)


#endif //RISC_V_ISA_RV32I_HPP
