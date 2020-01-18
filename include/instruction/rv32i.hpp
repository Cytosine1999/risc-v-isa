#ifndef RISC_V_ISA_RV32I_HPP
#define RISC_V_ISA_RV32I_HPP


#include "utility.hpp"
#include "instruction.hpp"
#include "register/register_file.hpp"
#include "memory/memory.hpp"


namespace risc_v_isa {
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
                set_gp_reg(rd, imm + reg.get_pc());
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

    class InstructionBranchSet : public Instruction32B {
    protected:
        template<typename OP, typename RegT>
        void operate_on(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            usize rs1 = get_rs1();
            usize rs2 = get_rs2();
            XLenT imm = get_imm();
            reg.inc_pc(OP(reg.get_x(rs1), reg.get_x(rs2)) ? imm : INST_WIDTH);
        }

    public:
        static constexpr UInnerT OP_CODE = 0b1100011;

    };

    class BEQInst : public InstructionBranchSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](XLenT a, XLenT b) { return a == b; }>(reg); }
    };

    class BNEInst : public InstructionBranchSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](XLenT a, XLenT b) { return a != b; }>(reg); }
    };

    class BLTInst : public InstructionBranchSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b100;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](XLenT a, XLenT b) { return a < b; }>(reg); }
    };

    class BGEInst : public InstructionBranchSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](XLenT a, XLenT b) { return a >= b; }>(reg); }
    };

    class BLTUInst : public InstructionBranchSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b110;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](UXLenT a, UXLenT b) { return a < b; }>(reg); }
    };

    class BGEUInst : public InstructionBranchSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b111;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](UXLenT a, UXLenT b) { return a >= b; }>(reg); }
    };

    class LBInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT, typename MemT>
        void operator()(RegT &reg, MemT &mem) const { operate_on<i8>(reg, mem); }
    };

    class LHInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT, typename MemT>
        void operator()(RegT &reg, MemT &mem) const { operate_on<i16>(reg, mem); }
    };

    class LWInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b010;

        template<typename RegT, typename MemT>
        void operator()(RegT &reg, MemT &mem) const { operate_on<i32>(reg, mem); }
    };

    class LBUInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b100;

        template<typename RegT, typename MemT>
        void operator()(RegT &reg, MemT &mem) const { operate_on<u8>(reg, mem); }
    };

    class LHUInst : public InstructionLoadSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

        template<typename RegT, typename MemT>
        void operator()(RegT &reg, MemT &mem) const { operate_on<u16>(reg, mem); }
    };

    class SBInst : public InstructionStoreSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT, typename MemT>
        void operator()(RegT &reg, MemT &mem) const { operate_on<i8>(reg, mem); }
    };

    class SHInst : public InstructionStoreSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT, typename MemT>
        void operator()(RegT &reg, MemT &mem) const { operate_on<i16>(reg, mem); }
    };

    class SWInst : public InstructionStoreSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b010;

        template<typename RegT, typename MemT>
        void operator()(RegT &reg, MemT &mem) const { operate_on<i32>(reg, mem); }
    };

    class ADDIInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](XLenT a, XLenT b) { return a + b; }>(reg); }
    };

    class SLTIInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b010;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](XLenT a, XLenT b) { return a < b; }>(reg); }
    };

    class SLTIUInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b011;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](UXLenT a, UXLenT b) { return a + b; }>(reg); }
    };

    class XORIInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b100;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](UXLenT a, UXLenT b) { return a ^ b; }>(reg); }
    };

    class ORIInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b110;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](UXLenT a, UXLenT b) { return a | b; }>(reg); }
    };

    class ANDIInst : public InstructionArithImmSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b111;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](UXLenT a, UXLenT b) { return a & b; }>(reg); }
    };

    class InstructionShiftImmSet : public InstructionArithImmSet {
    protected:
        template <typename OP, typename RegT>
        void operate_on(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            usize rd = get_rd();
            if (rd != 0) {
                usize rs1 = get_rs1();
                XLenT imm = get_shift_amount();
                set_gp_reg(rd, OP(reg.get_x(rs1), imm));
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
        void operator()(RegT &reg) const { operate_on<[](UXLenT a, UXLenT b) { return a << b; }>(reg); }
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
        void operator()(RegT &reg) const { operate_on<[](UXLenT a, UXLenT b) { return a >> b; }>(reg); }
    };

    class SRAIWInst : public InstructionSrliwSraiw {
    public:
        static constexpr UInnerT FUNC_SHIFT = 0b0100000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](XLenT a, UXLenT b) { return a >> b; }>(reg); }
    };

    class InstructionAddSub : public InstructionArithRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;
    };

    class ADDInst : public InstructionAddSub {
    public:
        static constexpr UInnerT FUNC_7 = 0b0000000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](XLenT a, XLenT b) { return a + b; }>(reg); }
    };

    class SUBInst : public InstructionAddSub {
    public:
        static constexpr UInnerT FUNC_7 = 0b0100000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](XLenT a, XLenT b) { return a - b; }>(reg); }
    };

    class SLLWInst : public InstructionArithRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](UXLenT a, UXLenT b) { return a << b; }>(reg); }
    };

    class SLTInst : public InstructionArithRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b010;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](XLenT a, XLenT b) { return a < b; }>(reg); }
    };

    class SLTUInst : public InstructionArithRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b011;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](UXLenT a, UXLenT b) { return a < b; }>(reg); }
    };

    class XORInst : public InstructionArithRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b100;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](UXLenT a, UXLenT b) { return a ^ b; }>(reg); }
    };

    class InstructionSrlwSraw : public InstructionArithRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

    };

    class SRLWInst : public InstructionSrlwSraw {
    public:
        static constexpr UInnerT FUNC_7 = 0b0000000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](UXLenT a, UXLenT b) { return a >> b; }>(reg); }
    };

    class SRAWInst : public InstructionSrlwSraw {
    public:
        static constexpr UInnerT FUNC_7 = 0b0100000;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](XLenT a, UXLenT b) { return a >> b; }>(reg); }
    };

    class ORInst : public InstructionArithRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b110;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](UXLenT a, UXLenT b) { return a | b; }>(reg); }
    };

    class ANDInst : public InstructionArithRegSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b111;

        template<typename RegT>
        void operator()(RegT &reg) const { operate_on<[](UXLenT a, UXLenT b) { return a & b; }>(reg); }
    };

    class InstructionFenceSet : public Instruction32I {
    public:
        static constexpr UInnerT OP_CODE = 0110111;

        usize get_unused() const { return inner & (BITS_MASK<UInnerT, 12, 7> | BITS_MASK<UInnerT, 20, 15>); }
    };

    class FENCEInst : public InstructionFenceSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b000;

        usize get_unused() const { return inner & (BITS_MASK<UInnerT, 32, 28>); }

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            // todo
        }
    };

    class FENCEIInst : public InstructionFenceSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        usize get_unused() const { return inner & (BITS_MASK<UInnerT, 32, 20>); }

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            // todo
        }
    };

    class InstructionSystemSet : public Instruction32I {
    public:
        static constexpr UInnerT OP_CODE = 0b1110011;
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

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            // todo
        }
    };

    class EBREAKInst : public InstructionEnvironmentSet {
    public:
        static constexpr UInnerT FUNCT_ENVIRONMENT = 0b000000000001;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            // todo
        }
    };

    class CSRRWInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            // todo
        }
    };

    class CSRRSInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b010;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            // todo
        }
    };

    class CSRRCInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b011;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            // todo
        }
    };

    class CSRRWIInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            // todo
        }
    };

    class CSRRSIInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b110;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            // todo
        }
    };

    class CSRRCIInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b111;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            // todo
        }
    };
}


#endif //RISC_V_ISA_RV32I_HPP
