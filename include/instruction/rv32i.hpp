#ifndef RISCV_ISA_RV32I_HPP
#define RISCV_ISA_RV32I_HPP


#include "utility.hpp"
#include "instruction.hpp"


namespace riscv_isa {
    class InstructionShiftImmSet : public InstructionArithImmSet {
    protected:
        InstructionShiftImmSet(usize rd, usize funct3, usize rs1, UInnerT imm)
                : InstructionArithImmSet{rd, funct3, rs1, imm} {}

    public:
        usize get_shift_amount() const { return slice_shift_amount(inner); }

        usize get_funct_shift() const { return slice_funct_shift(inner); }
    };

    class InstructionShiftRightImmSet : public InstructionShiftImmSet {
    public:
        using UInnerT = typename InstructionShiftImmSet::UInnerT;

    protected:
        InstructionShiftRightImmSet(usize rd, usize rs1, UInnerT imm) :
                InstructionShiftImmSet{rd, FUNCT3, rs1, imm} {}

    public:
        static constexpr UInnerT FUNCT3 = 0b101;
    };

    class InstructionIntegerRegSet : public InstructionArithRegSet {
    protected:
        InstructionIntegerRegSet(usize rd, usize funct3, usize rs1, usize rs2)
                : InstructionArithRegSet{rd, funct3, rs1, rs2, FUNCT7} {}

    public:
        static constexpr UInnerT FUNCT7 = 0b0000000;
    };

    class InstructionIntegerRegModSet : public InstructionArithRegSet {
    protected:
        InstructionIntegerRegModSet(usize rd, usize funct3, usize rs1, usize rs2)
                : InstructionArithRegSet{rd, funct3, rs1, rs2, FUNCT7} {}

    public:
        static constexpr UInnerT FUNCT7 = 0b0100000;
    };

    class LUIInst : public Instruction32U {
    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b01101;

        LUIInst(usize rd, UInnerT imm) : Instruction32U{OP_CODE, rd, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const LUIInst &inst) {
            stream << "lui\tx" << inst.get_rd() << ", " << inst.get_imm();
            return stream;
        }
    };

    class AUIPCInst : public Instruction32U {
    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b00101;

        AUIPCInst(usize rd, UInnerT imm) : Instruction32U{OP_CODE, rd, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const AUIPCInst &inst) {
            stream << "auipc\tx" << inst.get_rd() << ", " << inst.get_imm();
            return stream;
        }
    };

    class JALInst : public Instruction32J {
    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b11011;

        JALInst(usize rd, UInnerT imm) : Instruction32J{OP_CODE, rd, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const JALInst &inst) {
            stream << "jal\tx" << inst.get_rd() << ", " << inst.get_imm();
            return stream;
        }
    };

    class JALRInst : public Instruction32I {
    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *_self) {
            Instruction32I *self = reinterpret_cast<Instruction32I *>(_self);
            return self->get_op_code() == OP_CODE && self->get_funct3() == FUNCT3;
        }

        static constexpr UInnerT OP_CODE = 0b11001;
        static constexpr UInnerT FUNCT3 = 0b000;

        JALRInst(usize rd, usize rs1, UInnerT imm) : Instruction32I{OP_CODE, rd, FUNCT3, rs1, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const JALRInst &inst) {
            stream << "jalr\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_imm();
            return stream;
        }
    };

    class BEQInst : public InstructionBranchSet {
    public:
        using BaseT = InstructionBranchSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b000;

        BEQInst(usize rs1, usize rs2, UInnerT imm) : InstructionBranchSet{FUNCT3, rs1, rs2, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const BEQInst &inst) {
            stream << "beq\tx" << inst.get_rs1() << ", x" << inst.get_rs2() << ", " << inst.get_imm();
            return stream;
        }
    };

    class BNEInst : public InstructionBranchSet {
    public:
        using BaseT = InstructionBranchSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b001;

        BNEInst(usize rs1, usize rs2, UInnerT imm) : InstructionBranchSet{FUNCT3, rs1, rs2, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const BNEInst &inst) {
            stream << "bne\tx" << inst.get_rs1() << ", x" << inst.get_rs2() << ", " << inst.get_imm();
            return stream;
        }
    };

    class BLTInst : public InstructionBranchSet {
    public:
        using BaseT = InstructionBranchSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b100;

        BLTInst(usize rs1, usize rs2, UInnerT imm) : InstructionBranchSet{FUNCT3, rs1, rs2, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const BLTInst &inst) {
            stream << "blt\tx" << inst.get_rs1() << ", x" << inst.get_rs2() << ", " << inst.get_imm();
            return stream;
        }
    };

    class BGEInst : public InstructionBranchSet {
    public:
        using BaseT = InstructionBranchSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b101;

        BGEInst(usize rs1, usize rs2, UInnerT imm) : InstructionBranchSet{FUNCT3, rs1, rs2, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const BGEInst &inst) {
            stream << "bne\tx" << inst.get_rs1() << ", x" << inst.get_rs2() << ", " << inst.get_imm();
            return stream;
        }
    };

    class BLTUInst : public InstructionBranchSet {
    public:
        using BaseT = InstructionBranchSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b110;

        BLTUInst(usize rs1, usize rs2, UInnerT imm) : InstructionBranchSet{FUNCT3, rs1, rs2, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const BLTUInst &inst) {
            stream << "bltu\tx" << inst.get_rs1() << ", x" << inst.get_rs2() << ", " << inst.get_imm();
            return stream;
        }
    };

    class BGEUInst : public InstructionBranchSet {
    public:
        using BaseT = InstructionBranchSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b111;

        BGEUInst(usize rs1, usize rs2, UInnerT imm) : InstructionBranchSet{FUNCT3, rs1, rs2, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const BGEUInst &inst) {
            stream << "bgeu\tx" << inst.get_rs1() << ", x" << inst.get_rs2() << ", " << inst.get_imm();
            return stream;
        }
    };

    class LBInst : public InstructionLoadSet {
    public:
        using BaseT = InstructionLoadSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b000;

        LBInst(usize rd, usize rs1, UInnerT imm) : InstructionLoadSet{rd, FUNCT3, rs1, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const LBInst &inst) {
            stream << "lb\tx" << inst.get_rd() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class LHInst : public InstructionLoadSet {
    public:
        using BaseT = InstructionLoadSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b001;

        LHInst(usize rd, usize rs1, UInnerT imm) : InstructionLoadSet{rd, FUNCT3, rs1, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const LHInst &inst) {
            stream << "lh\tx" << inst.get_rd() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class LWInst : public InstructionLoadSet {
    public:
        using BaseT = InstructionLoadSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b010;

        LWInst(usize rd, usize rs1, UInnerT imm) : InstructionLoadSet{rd, FUNCT3, rs1, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const LWInst &inst) {
            stream << "lw\tx" << inst.get_rd() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class LBUInst : public InstructionLoadSet {
    public:
        using BaseT = InstructionLoadSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b100;

        LBUInst(usize rd, usize rs1, UInnerT imm) : InstructionLoadSet{rd, FUNCT3, rs1, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const LBUInst &inst) {
            stream << "lbu\tx" << inst.get_rd() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class LHUInst : public InstructionLoadSet {
    public:
        using BaseT = InstructionLoadSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b101;

        LHUInst(usize rd, usize rs1, UInnerT imm) : InstructionLoadSet{rd, FUNCT3, rs1, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const LHUInst &inst) {
            stream << "lhu\tx" << inst.get_rd() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class SBInst : public InstructionStoreSet {
    public:
        using BaseT = InstructionStoreSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b000;

        SBInst(usize rs1, usize rs2, UInnerT imm) : InstructionStoreSet{FUNCT3, rs1, rs2, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const SBInst &inst) {
            stream << "sb\tx" << inst.get_rs2() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class SHInst : public InstructionStoreSet {
    public:
        using BaseT = InstructionStoreSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b001;

        SHInst(usize rs1, usize rs2, UInnerT imm) : InstructionStoreSet{FUNCT3, rs1, rs2, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const SHInst &inst) {
            stream << "sh\tx" << inst.get_rs2() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class SWInst : public InstructionStoreSet {
    public:
        using BaseT = InstructionStoreSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b010;

        SWInst(usize rs1, usize rs2, UInnerT imm) : InstructionStoreSet{FUNCT3, rs1, rs2, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const SWInst &inst) {
            stream << "sw\tx" << inst.get_rs2() << ", " << inst.get_imm() << "(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class ADDIInst : public InstructionArithImmSet {
    public:
        using BaseT = InstructionArithImmSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b000;

        ADDIInst(usize rd, usize rs1, UInnerT imm) : InstructionArithImmSet{rd, FUNCT3, rs1, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const ADDIInst &inst) {
            stream << "addi\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_imm();
            return stream;
        }
    };

    class SLTIInst : public InstructionArithImmSet {
    public:
        using BaseT = InstructionArithImmSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b010;

        SLTIInst(usize rd, usize rs1, UInnerT imm) : InstructionArithImmSet{rd, FUNCT3, rs1, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const SLTIInst &inst) {
            stream << "slti\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_imm();
            return stream;
        }
    };

    class SLTIUInst : public InstructionArithImmSet {
    public:
        using BaseT = InstructionArithImmSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b011;

        SLTIUInst(usize rd, usize rs1, UInnerT imm) : InstructionArithImmSet{rd, FUNCT3, rs1, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const SLTIUInst &inst) {
            stream << "sltiu\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_imm();
            return stream;
        }
    };

    class XORIInst : public InstructionArithImmSet {
    public:
        using BaseT = InstructionArithImmSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b100;

        XORIInst(usize rd, usize rs1, UInnerT imm) : InstructionArithImmSet{rd, FUNCT3, rs1, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const XORIInst &inst) {
            stream << "xori\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_imm();
            return stream;
        }
    };

    class ORIInst : public InstructionArithImmSet {
    public:
        using BaseT = InstructionArithImmSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b110;

        ORIInst(usize rd, usize rs1, UInnerT imm) : InstructionArithImmSet{rd, FUNCT3, rs1, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const ORIInst &inst) {
            stream << "ori\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_imm();
            return stream;
        }
    };

    class ANDIInst : public InstructionArithImmSet {
    public:
        using BaseT = InstructionArithImmSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b111;

        ANDIInst(usize rd, usize rs1, UInnerT imm) : InstructionArithImmSet{rd, FUNCT3, rs1, imm} {}

        friend std::ostream &operator<<(std::ostream &stream, const ANDIInst &inst) {
            stream << "andi\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_imm();
            return stream;
        }
    };

    class SLLIInst : public InstructionShiftImmSet {
    public:

        using BaseT = InstructionArithImmSet;

        static bool is_self_type(BaseT *self) {
            return self->get_funct3() == FUNCT3 &&
                   reinterpret_cast<InstructionShiftImmSet *>(self)->get_funct_shift() == FUNCT_SHIFT;
        }

        static constexpr UInnerT FUNCT3 = 0b001;
        static constexpr UInnerT FUNCT_SHIFT = 0b000000000000;

        SLLIInst(usize rd, usize rs1, UInnerT imm)
                : InstructionShiftImmSet{rd, FUNCT3, rs1,
                                         (imm & bits_mask<UInnerT, xlen_trait::XLEN_INDEX, 0>::val) | FUNCT_SHIFT} {}

        friend std::ostream &operator<<(std::ostream &stream, const SLLIInst &inst) {
            stream << "slli\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_shift_amount();
            return stream;
        }
    };

    class SRLIInst : public InstructionShiftRightImmSet {
    public:
        using UInnerT = typename InstructionShiftRightImmSet::UInnerT;
        static constexpr UInnerT FUNCT3 = InstructionShiftRightImmSet::FUNCT3;

        using BaseT = InstructionArithImmSet;

        static bool is_self_type(BaseT *self) {
            return self->get_funct3() == FUNCT3 &&
                   reinterpret_cast<InstructionShiftImmSet *>(self)->get_funct_shift() == FUNCT_SHIFT;
        }

        static constexpr UInnerT FUNCT_SHIFT = 0b000000000000;

        SRLIInst(usize rd, usize rs1, UInnerT imm)
                : InstructionShiftRightImmSet{rd, rs1,
                                              get_bits<UInnerT, xlen_trait::XLEN_INDEX, 0>(imm) | FUNCT_SHIFT} {}

        friend std::ostream &operator<<(std::ostream &stream, const SRLIInst &inst) {
            stream << "srli\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_shift_amount();
            return stream;
        }
    };

    class SRAIInst : public InstructionShiftRightImmSet {
    public:
        using UInnerT = typename InstructionShiftRightImmSet::UInnerT;
        static constexpr UInnerT FUNCT3 = InstructionShiftRightImmSet::FUNCT3;

        using BaseT = InstructionArithImmSet;

        static bool is_self_type(BaseT *self) {
            return self->get_funct3() == FUNCT3 &&
                   reinterpret_cast<InstructionShiftImmSet *>(self)->get_funct_shift() == FUNCT_SHIFT;
        }

        static constexpr UInnerT FUNCT_SHIFT = 0b010000000000;

        SRAIInst(usize rd, usize rs1, UInnerT imm)
                : InstructionShiftRightImmSet{rd, rs1,
                                              get_bits<UInnerT, xlen_trait::XLEN_INDEX, 0>(imm) | FUNCT_SHIFT} {}

        friend std::ostream &operator<<(std::ostream &stream, const SRAIInst &inst) {
            stream << "srai\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", " << inst.get_shift_amount();
            return stream;
        }
    };

    class ADDInst : public InstructionIntegerRegSet {
    public:
        using BaseT = InstructionArithRegSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3 && self->get_funct7() == FUNCT7; }

        static constexpr UInnerT FUNCT3 = 0b000;

        ADDInst(usize rd, usize rs1, usize rs2) : InstructionIntegerRegSet{rd, FUNCT3, rs1, rs2} {}

        friend std::ostream &operator<<(std::ostream &stream, const ADDInst &inst) {
            stream << "add\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SUBInst : public InstructionIntegerRegModSet {
    public:
        using BaseT = InstructionArithRegSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3 && self->get_funct7() == FUNCT7; }

        static constexpr UInnerT FUNCT3 = 0b000;

        SUBInst(usize rd, usize rs1, usize rs2) : InstructionIntegerRegModSet{rd, FUNCT3, rs1, rs2} {}

        friend std::ostream &operator<<(std::ostream &stream, const SUBInst &inst) {
            stream << "sub\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SLLInst : public InstructionIntegerRegSet {
    public:
        using BaseT = InstructionArithRegSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3 && self->get_funct7() == FUNCT7; }

        static constexpr UInnerT FUNCT3 = 0b001;

        SLLInst(usize rd, usize rs1, usize rs2) : InstructionIntegerRegSet{rd, FUNCT3, rs1, rs2} {}

        friend std::ostream &operator<<(std::ostream &stream, const SLLInst &inst) {
            stream << "sll\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SLTInst : public InstructionIntegerRegSet {
    public:
        using BaseT = InstructionArithRegSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3 && self->get_funct7() == FUNCT7; }

        static constexpr UInnerT FUNCT3 = 0b010;

        SLTInst(usize rd, usize rs1, usize rs2) : InstructionIntegerRegSet{rd, FUNCT3, rs1, rs2} {}

        friend std::ostream &operator<<(std::ostream &stream, const SLTInst &inst) {
            stream << "slt\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SLTUInst : public InstructionIntegerRegSet {
    public:
        using BaseT = InstructionArithRegSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3 && self->get_funct7() == FUNCT7; }

        static constexpr UInnerT FUNCT3 = 0b011;

        SLTUInst(usize rd, usize rs1, usize rs2) : InstructionIntegerRegSet{rd, FUNCT3, rs1, rs2} {}

        friend std::ostream &operator<<(std::ostream &stream, const SLTUInst &inst) {
            stream << "sltu\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class XORInst : public InstructionIntegerRegSet {
    public:
        using BaseT = InstructionArithRegSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3 && self->get_funct7() == FUNCT7; }

        static constexpr UInnerT FUNCT3 = 0b100;

        XORInst(usize rd, usize rs1, usize rs2) : InstructionIntegerRegSet{rd, FUNCT3, rs1, rs2} {}

        friend std::ostream &operator<<(std::ostream &stream, const XORInst &inst) {
            stream << "xor\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SRLInst : public InstructionIntegerRegSet {
    public:
        using BaseT = InstructionArithRegSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3 && self->get_funct7() == FUNCT7; }

        static constexpr UInnerT FUNCT3 = 0b101;

        SRLInst(usize rd, usize rs1, usize rs2) : InstructionIntegerRegSet{rd, FUNCT3, rs1, rs2} {}

        friend std::ostream &operator<<(std::ostream &stream, const SRLInst &inst) {
            stream << "srl\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class SRAInst : public InstructionIntegerRegModSet {
    public:
        using BaseT = InstructionArithRegSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3 && self->get_funct7() == FUNCT7; }

        static constexpr UInnerT FUNCT3 = 0b101;

        SRAInst(usize rd, usize rs1, usize rs2) : InstructionIntegerRegModSet{rd, FUNCT3, rs1, rs2} {}

        friend std::ostream &operator<<(std::ostream &stream, const SRAInst &inst) {
            stream << "sra\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class ORInst : public InstructionIntegerRegSet {
    public:
        using BaseT = InstructionArithRegSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3 && self->get_funct7() == FUNCT7; }

        static constexpr UInnerT FUNCT3 = 0b110;

        ORInst(usize rd, usize rs1, usize rs2) : InstructionIntegerRegSet{rd, FUNCT3, rs1, rs2} {}

        friend std::ostream &operator<<(std::ostream &stream, const ORInst &inst) {
            stream << "or\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class ANDInst : public InstructionIntegerRegSet {
    public:
        using BaseT = InstructionArithRegSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3 && self->get_funct7() == FUNCT7; }

        static constexpr UInnerT FUNCT3 = 0b111;

        ANDInst(usize rd, usize rs1, usize rs2) : InstructionIntegerRegSet{rd, FUNCT3, rs1, rs2} {}

        friend std::ostream &operator<<(std::ostream &stream, const ANDInst &inst) {
            stream << "and\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class FENCEInst : public InstructionFenceSet {
    public:
        using BaseT = InstructionFenceSet;

        static bool is_self_type(BaseT *self) { return self->get_funct3() == FUNCT3; }

        static constexpr UInnerT FUNCT3 = 0b000;

        FENCEInst(usize sw, usize sr, usize so, usize si, usize pw, usize pr, usize po, usize pi, usize fm) :
                InstructionFenceSet{0, FUNCT3, 0,
                                    get_bit<UInnerT, 0, 0>(sw) |
                                    get_bit<UInnerT, 0, 1>(sr) |
                                    get_bit<UInnerT, 0, 2>(so) |
                                    get_bit<UInnerT, 0, 3>(si) |
                                    get_bit<UInnerT, 0, 4>(pw) |
                                    get_bit<UInnerT, 0, 5>(pr) |
                                    get_bit<UInnerT, 0, 6>(po) |
                                    get_bit<UInnerT, 0, 7>(pi) |
                                    get_bits<UInnerT, 4, 0, 8>(fm)} {}

        bool get_sw() const { return get_bit<UInnerT, 20>(inner); }

        bool get_sr() const { return get_bit<UInnerT, 21>(inner); }

        bool get_so() const { return get_bit<UInnerT, 22>(inner); }

        bool get_si() const { return get_bit<UInnerT, 23>(inner); }

        bool get_pw() const { return get_bit<UInnerT, 24>(inner); }

        bool get_pr() const { return get_bit<UInnerT, 25>(inner); }

        bool get_po() const { return get_bit<UInnerT, 26>(inner); }

        bool get_pi() const { return get_bit<UInnerT, 27>(inner); }

        usize get_fm() const { return get_bits<UInnerT, 32, 28>(inner); }

        friend std::ostream &operator<<(std::ostream &stream, riscv_isa_unused const FENCEInst &inst) {
            stream << "fence";
            return stream;
        }
    };

    class ECALLInst : public InstructionPrivilegedSet {
    public:
        using BaseT = InstructionSystemSet;

        static bool is_self_type(BaseT *self) {
            return reinterpret_cast<ECALLInst *>(self)->inner == ECALLInst().inner;
        }

        static constexpr UInnerT FUNCT7 = 0b0000000;
        static constexpr UInnerT FUNCT_PRIVILEGE = 0b00000;

        ECALLInst() : InstructionPrivilegedSet{0, 0, FUNCT_PRIVILEGE, FUNCT7} {}

        friend std::ostream &operator<<(std::ostream &stream, riscv_isa_unused const ECALLInst &inst) {
            stream << "ecall";
            return stream;
        }
    };

    class EBREAKInst : public InstructionPrivilegedSet {
    public:
        using BaseT = InstructionSystemSet;

        static bool is_self_type(BaseT *self) {
            return reinterpret_cast<EBREAKInst *>(self)->inner == EBREAKInst().inner;
        }

        static constexpr UInnerT FUNCT7 = 0b0000000;
        static constexpr UInnerT FUNCT_PRIVILEGE = 0b00001;

        EBREAKInst() : InstructionPrivilegedSet{0, 0, FUNCT_PRIVILEGE, FUNCT7} {}

        friend std::ostream &operator<<(std::ostream &stream, riscv_isa_unused const EBREAKInst &inst) {
            stream << "ebreak";
            return stream;
        }
    };
}


#endif //RISCV_ISA_RV32I_HPP
