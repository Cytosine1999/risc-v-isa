#ifndef RISCV_ISA_INSTRUCTION_HPP
#define RISCV_ISA_INSTRUCTION_HPP


#include "utility.hpp"
#include "memory/memory.hpp"


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

        static constexpr usize INST_WIDTH = sizeof(UInnerT);

        InnerT inner;
    public:
        using BaseT = Instruction;

        static bool is_self_type(BaseT *self) {
            return (*reinterpret_cast<u16 *>(self) & bits_mask<u16, 2, 0>::val) != bits_mask<u16, 2, 0>::val;
        }
    };

#endif // defined(__RV_EXTENSION_C__)

    class Instruction32 : public Instruction {
    protected:
        using InnerT = i32;
        using UInnerT = u32;

        static UInnerT slice_op_code(UInnerT val) { return get_bits<UInnerT, 7, 2>(val); }

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

        static UInnerT slice_shift_amount(UInnerT val) {
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

        usize get_op_code() const { return slice_op_code(inner); }
    };

    class Instruction32R : public Instruction32 {
    protected:
        Instruction32R(usize op, usize rd, usize funct3, usize rs1, usize rs2, usize funct7)
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

    class Instruction32I : public Instruction32 {
    protected:
        Instruction32I(usize op, usize rd, usize funct3, usize rs1, UInnerT imm)
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

    class Instruction32S : public Instruction32 {
    protected:
        Instruction32S(usize op, usize funct3, usize rs1, usize rs2, UInnerT imm)
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

    class Instruction32B : public Instruction32 {
    protected:
        Instruction32B(usize op, usize funct3, usize rs1, usize rs2, UInnerT imm)
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
            InnerT imm8_7 = get_bits<UInnerT, 8, 7, 11>(inner);
            InnerT imm12_8 = get_bits<UInnerT, 12, 8, 1>(inner);
            InnerT imm32_25 = inner >> 20 & (bits_mask<UInnerT, 32, 12>::val | bits_mask<UInnerT, 11, 5>::val);
            return imm32_25 | imm8_7 | imm12_8;
        }
    };

    class Instruction32U : public Instruction32 {
    protected:
        Instruction32U(usize op, usize rd, UInnerT imm)
                : Instruction32{static_cast<InnerT>(
                                        bits_mask<UInnerT, 2, 0>::val |
                                        get_bits<UInnerT, 5, 0, 2>(op) |
                                        get_bits<UInnerT, 5, 0, 7>(rd) |
                                        get_bits<UInnerT, 32, 12, 12>(imm))} {}

    public:
        usize get_rd() const { return slice_rd(inner); }

        InnerT get_imm() const { return get_bits<UInnerT, 32, 12, 12>(inner); }
    };

    class Instruction32J : public Instruction32 {
    protected:
        Instruction32J(usize op, usize rd, UInnerT imm)
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
            InnerT imm20_12 = get_bits<UInnerT, 20, 12, 12>(inner);
            InnerT imm21_20 = get_bits<UInnerT, 21, 20, 11>(inner);
            InnerT imm32_21 = inner >> 20 & (bits_mask<UInnerT, 32, 20>::val | bits_mask<UInnerT, 11, 1>::val);
            return imm32_21 | imm20_12 | imm21_20;
        }
    };

    class InstructionBranchSet : public Instruction32B {
    protected:
        InstructionBranchSet(usize funct3, usize rs1, usize rs2, UInnerT imm)
                : Instruction32B{OP_CODE, funct3, rs1, rs2, imm} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b11000;
    };

    class InstructionLoadSet : public Instruction32I {
    protected:
        InstructionLoadSet(usize rd, usize funct3, usize rs1, UInnerT imm)
                : Instruction32I{OP_CODE, rd, funct3, rs1, imm} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b00000;
    };

    class InstructionStoreSet : public Instruction32S {
    protected:
        InstructionStoreSet(usize funct3, usize rs1, usize rs2, UInnerT imm)
                : Instruction32S{OP_CODE, funct3, rs1, rs2, imm} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b01000;
    };

    class InstructionArithImmSet : public Instruction32I {
    protected:
        InstructionArithImmSet(usize rd, usize funct3, usize rs1, UInnerT imm)
                : Instruction32I{OP_CODE, rd, funct3, rs1, imm} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b00100;
    };

    class InstructionArithRegSet : public Instruction32R {
    protected:
        InstructionArithRegSet(usize rd, usize funct3, usize rs1, usize rs2, usize funct7)
                : Instruction32R{OP_CODE, rd, funct3, rs1, rs2, funct7} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b01100;
    };

    class InstructionFenceSet : public Instruction32I {
    protected:
        InstructionFenceSet(usize rd, usize funct3, usize rs1, UInnerT imm)
                : Instruction32I{OP_CODE, rd, funct3, rs1, imm} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b00011;
    };

    class InstructionSystemSet : public Instruction32I {
    protected:
        InstructionSystemSet(usize rd, usize funct3, usize rs1, UInnerT imm)
                : Instruction32I{OP_CODE, rd, funct3, rs1, imm} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b11100;
    };

#if __RV_BIT_WIDTH__ == 64

    class InstructionArithImmWSet : public Instruction32I {
    protected:
        InstructionArithImmWSet(usize rd, usize funct3, usize rs1, UInnerT imm)
                : Instruction32I{OP_CODE, rd, funct3, rs1, imm} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b00110;
    };

    class InstructionArithRegWSet : public Instruction32R {
    protected:
        InstructionArithRegWSet(usize rd, usize funct3, usize rs1, usize rs2, usize funct7)
                : Instruction32R{OP_CODE, rd, funct3, rs1, rs2, funct7} {}

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
