#ifndef RISCV_ISA_INSTRUCTION_HPP
#define RISCV_ISA_INSTRUCTION_HPP


#include "riscv_isa_utility.hpp"


namespace riscv_isa {
    class Instruction {
    protected:
        Instruction() = default;
    };

#if defined(__RV_EXTENSION_C__)

    class Instruction16 : public Instruction {
    protected:
        using InnerT = i16;
        using UInnerT = u16;

        static UInnerT slice_rd_rs1(UInnerT val) { return get_bits<UInnerT, 12, 7>(val); }

        static UInnerT slice_rs2(UInnerT val) { return get_bits<UInnerT, 7, 2>(val); }

        static UInnerT slice_rdc_rs2c(UInnerT val) { return get_bits<UInnerT, 5, 2>(val) + 8; }

        static UInnerT slice_rdc_rs1c(UInnerT val) { return get_bits<UInnerT, 10, 7>(val) + 8; }

        static UInnerT slice_funct2(UInnerT val) { return get_bits<UInnerT, 12, 10>(val); }

        static UInnerT slice_funct1 (UInnerT val) { return get_bits<UInnerT, 13, 12>(val); }

        static UInnerT slice_imm_w(UInnerT val) {
            UInnerT imm_3_2 = get_bits<UInnerT, 7, 6, 2>(val);
            UInnerT imm_6_3 = get_bits<UInnerT, 13, 10, 3>(val);
            UInnerT imm_7_6 = get_bits<UInnerT, 6, 5, 6>(val);

            return imm_7_6 | imm_6_3 | imm_3_2;
        }

        static UInnerT slice_imm_d(UInnerT val) {
            UInnerT imm_6_3 = get_bits<UInnerT, 13, 10, 3>(val);
            UInnerT imm_8_5 = get_bits<UInnerT, 7, 5, 6>(val);

            return imm_8_5 | imm_6_3;
        }

        static UInnerT slice_imm_q(UInnerT val) {
            UInnerT imm_6_4 = get_bits<UInnerT, 13, 11, 4>(val);
            UInnerT imm_8_6 = get_bits<UInnerT, 7, 5, 6>(val);
            UInnerT imm_9_8 = get_bits<UInnerT, 11, 10, 8>(val);

            return imm_9_8 | imm_8_6 | imm_6_4;
        }

        static InnerT slice_imm_low(UInnerT val) {
            UInnerT imm_5_0 = get_bits<UInnerT, 7, 2, 0>(val);
            UInnerT imm_16_5 = get_bits<UInnerT, 16, 5, 5>((val << 3) >> 10);

            return imm_16_5 | imm_5_0;
        }

        static UInnerT slice_shamt(UInnerT val) {
            UInnerT imm_5_0 = get_bits<UInnerT, 7, 2, 0>(val);
#if __RV_BIT_WIDTH__ == 32
            return imm_5_0;
#elif __RV_BIT_WIDTH__ == 64
            UInnerT imm_16_5 = get_bits<UInnerT, 13, 12, 5>(val);

            return imm_16_5 | imm_16_5;
#endif
        }

        InnerT inner;

        explicit Instruction16(InnerT val) : inner{val} {}

    public:
        using BaseT = Instruction;

        static bool is_self_type(BaseT *self) {
            return (*reinterpret_cast<u16 *>(self) & bits_mask<u16, 2, 0>::val) != bits_mask<u16, 2, 0>::val;
        }

        static constexpr usize INST_WIDTH = sizeof(UInnerT);

        usize get_op() const { return get_bits<UInnerT, 2, 0>(inner); }

        usize get_funct3() const { return get_bits<UInnerT, 16, 13>(inner); }
    };

#endif // defined(__RV_EXTENSION_C__)

    class Instruction32 : public Instruction {
    protected:
        using InnerT = i32;
        using UInnerT = u32;

        static UInnerT slice_rd(UInnerT val) {
#if defined(__RV_BASE_I__)
            return get_bits<UInnerT, 12, 7>(val);
#elif defined(__RV_BASE_M__)
            return get_bits<UInnerT, 11, 7>(val);
#endif
        }

        static UInnerT slice_funct3(UInnerT val) { return get_bits<UInnerT, 15, 12>(val); }

        static UInnerT slice_rs1(UInnerT val) {
#if defined(__RV_BASE_I__)
            return get_bits<UInnerT, 20, 15>(val);
#elif defined(__RV_BASE_M__)
            return get_bits<UInnerT, 19, 15>(val);
#endif
        }

        static UInnerT slice_rs2(UInnerT val) {
#if defined(__RV_BASE_I__)
            return get_bits<UInnerT, 25, 20>(val);
#elif defined(__RV_BASE_M__)
            return get_bits<UInnerT, 24, 20>(val);
#endif
        }

        static UInnerT slice_funct7(UInnerT val) { return get_bits<UInnerT, 32, 25>(val); }

        static UInnerT slice_funct_shift(UInnerT val) {
            return get_bits<UInnerT, 32, 20 + xlen_trait::XLEN_INDEX, xlen_trait::XLEN_INDEX>(val);
        }

        static UInnerT slice_shamt(UInnerT val) {
            return get_bits<UInnerT, 20 + xlen_trait::XLEN_INDEX, 20>(val);
        }

        InnerT inner;

        explicit Instruction32(InnerT val) : inner{val} {}

    public:
        using BaseT = Instruction;

        static bool is_self_type(BaseT *self) {
            return (*reinterpret_cast<u16 *>(self) & bits_mask<u16, 2, 0>::val) == bits_mask<u16, 2, 0>::val &&
                   (*reinterpret_cast<u16 *>(self) & bits_mask<u16, 5, 2>::val) != bits_mask<u16, 5, 2>::val;
        }

        static constexpr usize INST_WIDTH = sizeof(UInnerT);

        usize get_op_code() const { return get_bits<UInnerT, 7, 2>(inner); }
    };

    class InstructionR : public Instruction32 {
    protected:
        InstructionR(usize op, usize rd, usize funct3, usize rs1, usize rs2, usize funct7)
                : Instruction32{static_cast<InnerT>(
                                        bits_mask<UInnerT, 2, 0>::val |
                                        get_bits<UInnerT, 5, 0, 2>(op) |
                                        get_bits<UInnerT, 5, 0, 7>(rd) |
                                        get_bits<UInnerT, 3, 0, 12>(funct3) |
                                        get_bits<UInnerT, 5, 0, 15>(rs1) |
                                        get_bits<UInnerT, 5, 0, 20>(rs2) |
                                        get_bits<UInnerT, 7, 0, 25>(funct7))} {}

    public:
        usize get_rd() const { return slice_rd(inner); }

        usize get_funct3() const { return slice_funct3(inner); }

        usize get_rs1() const { return slice_rs1(inner); }

        usize get_rs2() const { return slice_rs2(inner); }

        usize get_funct7() const { return slice_funct7(inner); }
    };

    class InstructionI : public Instruction32 {
    protected:
        InstructionI(usize op, usize rd, usize funct3, usize rs1, UInnerT imm)
                : Instruction32{static_cast<InnerT>(
                                        bits_mask<UInnerT, 2, 0>::val |
                                        get_bits<UInnerT, 5, 0, 2>(op) |
                                        get_bits<UInnerT, 5, 0, 7>(rd) |
                                        get_bits<UInnerT, 3, 0, 12>(funct3) |
                                        get_bits<UInnerT, 5, 0, 15>(rs1) |
                                        get_bits<UInnerT, 12, 0, 20>(imm))} {}

    public:
        usize get_rd() const { return slice_rd(inner); }

        usize get_funct3() const { return slice_funct3(inner); }

        usize get_rs1() const { return slice_rs1(inner); }

        InnerT get_imm() const { return inner >> 20; }
    };

    class InstructionS : public Instruction32 {
    protected:
        InstructionS(usize op, usize funct3, usize rs1, usize rs2, UInnerT imm)
                : Instruction32{static_cast<InnerT>(
                                        bits_mask<UInnerT, 2, 0>::val |
                                        get_bits<UInnerT, 5, 0, 2>(op) |
                                        get_bits<UInnerT, 5, 0, 7>(imm) |
                                        get_bits<UInnerT, 3, 0, 12>(funct3) |
                                        get_bits<UInnerT, 5, 0, 15>(rs1) |
                                        get_bits<UInnerT, 5, 0, 20>(rs2) |
                                        get_bits<UInnerT, 12, 5, 25>(imm))} {}

    public:
        usize get_funct3() const { return slice_funct3(inner); }

        usize get_rs1() const { return slice_rs1(inner); }

        usize get_rs2() const { return slice_rs2(inner); }

        InnerT get_imm() const {
            InnerT imm12_7 = get_bits<UInnerT, 12, 7, 0>(inner);
            InnerT imm32_25 = inner >> 20 & bits_mask<UInnerT, 32, 5>::val;
            return imm32_25 | imm12_7;
        }
    };

    class InstructionB : public Instruction32 {
    protected:
        InstructionB(usize op, usize funct3, usize rs1, usize rs2, UInnerT imm)
                : Instruction32{static_cast<InnerT>(
                                        bits_mask<UInnerT, 2, 0>::val |
                                        get_bits<UInnerT, 5, 0, 2>(op) |
                                        get_bits<UInnerT, 12, 11, 7>(imm) |
                                        get_bits<UInnerT, 5, 1, 8>(imm) |
                                        get_bits<UInnerT, 3, 0, 12>(funct3) |
                                        get_bits<UInnerT, 5, 0, 15>(rs1) |
                                        get_bits<UInnerT, 5, 0, 20>(rs2) |
                                        get_bits<UInnerT, 11, 5, 25>(imm) |
                                        get_bits<UInnerT, 13, 12, 31>(imm))} {}

    public:
        usize get_funct3() const { return slice_funct3(inner); }

        usize get_rs1() const { return slice_rs1(inner); }

        usize get_rs2() const { return slice_rs2(inner); }

        InnerT get_imm() const {
            UInnerT imm8_7 = get_bits<UInnerT, 8, 7, 11>(inner);
            UInnerT imm12_8 = get_bits<UInnerT, 12, 8, 1>(inner);
            UInnerT imm32_25 = inner >> 20 & (bits_mask<UInnerT, 32, 12>::val | bits_mask<UInnerT, 11, 5>::val);
            return imm32_25 | imm8_7 | imm12_8;
        }
    };

    class InstructionU : public Instruction32 {
    protected:
        InstructionU(usize op, usize rd, UInnerT imm)
                : Instruction32{static_cast<InnerT>(
                                        bits_mask<UInnerT, 2, 0>::val |
                                        get_bits<UInnerT, 5, 0, 2>(op) |
                                        get_bits<UInnerT, 5, 0, 7>(rd) |
                                        get_bits<UInnerT, 32, 12, 12>(imm))} {}

    public:
        usize get_rd() const { return slice_rd(inner); }

        InnerT get_imm() const { return get_bits<UInnerT, 32, 12, 12>(inner); }
    };

    class InstructionJ : public Instruction32 {
    protected:
        InstructionJ(usize op, usize rd, UInnerT imm)
                : Instruction32{static_cast<InnerT>(
                                        bits_mask<UInnerT, 2, 0>::val |
                                        get_bits<UInnerT, 5, 0, 2>(op) |
                                        get_bits<UInnerT, 5, 0, 7>(rd) |
                                        get_bits<UInnerT, 20, 12, 12>(imm) |
                                        get_bits<UInnerT, 12, 11, 20>(imm) |
                                        get_bits<UInnerT, 11, 1, 21>(imm) |
                                        get_bits<UInnerT, 21, 20, 31>(imm))} {}

    public:
        usize get_rd() const { return slice_rd(inner); }

        InnerT get_imm() const {
            UInnerT imm20_12 = get_bits<UInnerT, 20, 12, 12>(inner);
            UInnerT imm21_20 = get_bits<UInnerT, 21, 20, 11>(inner);
            UInnerT imm32_21 = inner >> 20 & (bits_mask<UInnerT, 32, 20>::val | bits_mask<UInnerT, 11, 1>::val);
            return imm32_21 | imm20_12 | imm21_20;
        }
    };

    class InstructionBranchSet : public InstructionB {
    protected:
        InstructionBranchSet(usize funct3, usize rs1, usize rs2, UInnerT imm)
                : InstructionB{OP_CODE, funct3, rs1, rs2, imm} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b11000;
    };

    class InstructionLoadSet : public InstructionI {
    protected:
        InstructionLoadSet(usize rd, usize funct3, usize rs1, UInnerT imm)
                : InstructionI{OP_CODE, rd, funct3, rs1, imm} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b00000;
    };

    class InstructionStoreSet : public InstructionS {
    protected:
        InstructionStoreSet(usize funct3, usize rs1, usize rs2, UInnerT imm)
                : InstructionS{OP_CODE, funct3, rs1, rs2, imm} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b01000;
    };

    class InstructionArithImmSet : public InstructionI {
    protected:
        InstructionArithImmSet(usize rd, usize funct3, usize rs1, UInnerT imm)
                : InstructionI{OP_CODE, rd, funct3, rs1, imm} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b00100;
    };

    class InstructionArithRegSet : public InstructionR {
    protected:
        InstructionArithRegSet(usize rd, usize funct3, usize rs1, usize rs2, usize funct7)
                : InstructionR{OP_CODE, rd, funct3, rs1, rs2, funct7} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b01100;
    };

    class InstructionFenceSet : public InstructionI {
    protected:
        InstructionFenceSet(usize rd, usize funct3, usize rs1, UInnerT imm)
                : InstructionI{OP_CODE, rd, funct3, rs1, imm} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b00011;
    };

    class InstructionSystemSet : public InstructionR {
    protected:
        InstructionSystemSet(usize rd, usize funct3, usize rs1, usize rs2, usize funct7)
                : InstructionR{OP_CODE, rd, funct3, rs1, rs2, funct7} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b11100;
    };

    class InstructionPrivilegedSet : public InstructionSystemSet {
    protected:
        InstructionPrivilegedSet(usize rd, usize rs1, usize rs2, usize funct7)
                : InstructionSystemSet{rd, FUNCT3, rs1, rs2, funct7} {}

    public:
        static constexpr UInnerT FUNCT3 = 0b000;
    };

#if __RV_BIT_WIDTH__ == 64

    class InstructionArithImmWSet : public InstructionI {
    protected:
        InstructionArithImmWSet(usize rd, usize funct3, usize rs1, UInnerT imm)
                : InstructionI{OP_CODE, rd, funct3, rs1, imm} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b00110;
    };

    class InstructionArithRegWSet : public InstructionR {
    protected:
        InstructionArithRegWSet(usize rd, usize funct3, usize rs1, usize rs2, usize funct7)
                : InstructionR{OP_CODE, rd, funct3, rs1, rs2, funct7} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b01110;
    };

#endif // __RV_BIT_WIDTH__ == 64
#if defined(__RV_CUSTOM_0__)

    class InstructionCustom0 : public Instruction32 {
    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b00010;

        explicit InstructionCustom0(InnerT val) : Instruction32{val} {}
    };

#endif // defined(__RV_CUSTOM_0__)
#if defined(__RV_CUSTOM_1__)

    class InstructionCustom1 : public Instruction32 {
    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b01010;

        explicit InstructionCustom1(InnerT val) : Instruction32{val} {}
    };

#endif // defined(__RV_CUSTOM_1__)
#if defined(__RV_CUSTOM_2__)

    class InstructionCustom2 : public Instruction32 {
    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b10110;

        explicit InstructionCustom2(InnerT val) : Instruction32{val} {}
    };

#endif // defined(__RV_CUSTOM_2__)
#if defined(__RV_CUSTOM_3__)

    class InstructionCustom3 : public Instruction32 {
    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b11110;

        explicit InstructionCustom3(InnerT val) : Instruction32{val} {}
    };

#endif // defined(__RV_CUSTOM_3__)
}


#endif //RISCV_ISA_INSTRUCTION_HPP
