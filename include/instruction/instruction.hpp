#ifndef RISC_V_ISA_INSTRUCTION_HPP
#define RISC_V_ISA_INSTRUCTION_HPP


#include "utility.hpp"
#include "register/register_file.hpp"
#include "memory/memory.hpp"


// todo: some question to be defined,
//      1. use operator() as semantic?
//      2. unify signature of all instruction?
//      3. use bool as return type?

namespace risc_v_isa {
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
            return (*reinterpret_cast<u16 *>(self) & BITS_MASK<u16, 2, 0>) != BITS_MASK<u16, 2, 0>;
        }
    };

#endif // defined(__RV_EXTENSION_C__)
#if defined(__RV_32_BIT__) || defined(__RV_64_BIT__)

    class Instruction32 : public Instruction {
    protected:
        using InnerT = i32;
        using UInnerT = u32;

        static constexpr UXLenT PTR_MASK = BITS_MASK<UXLenT, XLEN, 1>;

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

#if defined(__RV_EXTENSION_M__)
        static constexpr usize HALF_WIDTH = XLEN_BYTE / 2;
        static constexpr UXLenT HALF_MASK = BITS_MASK<UXLenT, HALF_WIDTH, 0>;
        static constexpr XLenT XLenTMin = (XLenT) (1u << (XLEN_BYTE - 1));

        struct MUL {
            static XLenT op(XLenT a, XLenT b) { return a * b; }
        };

        struct MULH {
            static XLenT op(XLenT a, XLenT b) {
                UXLenT as = a < 0;
                UXLenT au = (~as + 1) ^(a - as);
                UXLenT bs = b < 0;
                UXLenT bu = (~bs + 1) ^(b - bs);
                UXLenT a0 = au & HALF_MASK;
                UXLenT a1 = au >> HALF_WIDTH;
                UXLenT b0 = bu & HALF_MASK;
                UXLenT b1 = bu >> HALF_WIDTH;

                UXLenT c0 = a0 * b0;
                UXLenT c1 = a1 * b0;
                UXLenT c2 = a0 * b1;
                UXLenT c3 = a1 * b1;

                UXLenT d0 = (c0 >> HALF_WIDTH) + c1;
                UXLenT d1 = (d0 & HALF_MASK) + c2;
                UXLenT lh = (d1 << HALF_WIDTH) + (c0 & HALF_MASK);
                UXLenT hh = (d0 >> HALF_WIDTH) + (d1 >> HALF_WIDTH) + c3;

                UXLenT s = as ^bs;
                UXLenT lhs = lh - s;
                UXLenT hhs = -s ^(hh - (lhs > lh));

                return hhs;
            }
        };

        struct MULHSU {
            static XLenT op(XLenT a, UXLenT b) {
                UXLenT as = a < 0;
                UXLenT au = (~as + 1) ^(a - as);
                UXLenT a0 = au & HALF_MASK;
                UXLenT a1 = au >> HALF_WIDTH;
                UXLenT b0 = b & HALF_MASK;
                UXLenT b1 = b >> HALF_WIDTH;

                UXLenT c0 = a0 * b0;
                UXLenT c1 = a1 * b0;
                UXLenT c2 = a0 * b1;
                UXLenT c3 = a1 * b1;

                UXLenT d0 = (c0 >> HALF_WIDTH) + c1;
                UXLenT d1 = (d0 & HALF_MASK) + c2;
                UXLenT lh = (d1 << HALF_WIDTH) + (c0 & HALF_MASK);
                UXLenT hh = (d0 >> HALF_WIDTH) + (d1 >> HALF_WIDTH) + c3;

                UXLenT s = as;
                UXLenT lhs = lh - s;
                UXLenT hhs = -s ^(hh - (lhs > lh));

                return hhs;
            }
        };

        struct MULHU {
            static XLenT op(UXLenT a, UXLenT b) {
                UXLenT a0 = a & HALF_MASK;
                UXLenT a1 = a >> HALF_WIDTH;
                UXLenT b0 = b & HALF_MASK;
                UXLenT b1 = b >> HALF_WIDTH;

                UXLenT c0 = a0 * b0;
                UXLenT c1 = a1 * b0;
                UXLenT c2 = a0 * b1;
                UXLenT c3 = a1 * b1;

                UXLenT d0 = (c0 >> HALF_WIDTH) + c1;
                UXLenT d1 = (d0 & HALF_MASK) + c2;
                UXLenT hh = (d0 >> HALF_WIDTH) + (d1 >> HALF_WIDTH) + c3;

                return hh;
            }
        };

        struct DIV {
            static XLenT op(XLenT a, XLenT b) {
                if (b == 0) return -1;
                else if (a == XLenTMin && b == -1) return XLenTMin;
                return a / b;
            }
        };

        struct DIVU {
            static XLenT op(UXLenT a, UXLenT b) { return b == 0 ? -1 : a / b; }
        };

        struct REM {
            static XLenT op(XLenT a, XLenT b) {
                if (b == 0) return a;
                else if (a == XLenTMin && b == -1) return 0;
                return a % b;
            }
        };

        struct REMU {
            static XLenT op(UXLenT a, UXLenT b) { return b == 0 ? a : a % b; }
        };

#endif // defined(__RV_EXTENSION_M__)
#if defined(__RV_64_BIT__)

        struct ADDW {
            static i32 op(i32 a, i32 b) { return a + b; }
        };

        struct SUBW {
            static i32 op(i32 a, i32 b) { return a - b; }
        };

        struct SLLW {
            static i32 op(u32 a, u32 b) { return a << b; }
        };

        struct SRLW {
            static i32 op(u32 a, u32 b) { return a >> b; }
        };

        struct SRAW {
            static i32 op(i32 a, u32 b) { return a >> b; }
        };

#if defined(__RV_EXTENSION_M__)

        struct MULW {
            static i32 op(i32 a, i32 b) { return a * b; }
        };

        struct DIVW {
            static i32 op(i32 a, i32 b) {
                if (b == 0) return -1;
                else if (a == XLenTMin && b == -1) return XLenTMin;
                return a / b;
            }
        };

        struct DIVUW {
            static i32 op(u32 a, u32 b) { return b == 0 ? -1 : a / b; }
        };

        struct REMW {
            static i32 op(i32 a, i32 b) {
                if (b == 0) return a;
                else if (a == XLenTMin && b == -1) return 0;
                return a % b;
            }
        };

        struct REMUW {
            static i32 op(u32 a, u32 b) { return b == 0 ? a : a % b; }
        };

#endif // defined(__RV_EXTENSION_M__)
#endif // defined(__RV_64_BIT__)

        static UInnerT slice_op_code(UInnerT val) { return get_slice<UInnerT, 7, 2>(val); }

        static UInnerT slice_rd(UInnerT val) { return get_slice<UInnerT, 12, 7>(val); }

        static UInnerT slice_funct3(UInnerT val) { return get_slice<UInnerT, 15, 12>(val); }

        static UInnerT slice_rs1(UInnerT val) { return get_slice<UInnerT, 20, 15>(val); }

        static UInnerT slice_rs2(UInnerT val) { return get_slice<UInnerT, 25, 20>(val); }

        static UInnerT slice_funct7(UInnerT val) { return get_slice<UInnerT, 32, 25>(val); }

        static UInnerT slice_funct_shift(UInnerT val) {
            return get_slice<UInnerT, 32, 20 + XLEN_INDEX, XLEN_INDEX>(val);
        }

        static UInnerT slice_shift_amount(UInnerT val) { return static_cast<UInnerT>(val) >> 20u; }

        InnerT inner;

        explicit Instruction32(InnerT val) : inner{val} {}

    public:
        using BaseT = Instruction;

        static bool is_self_type(BaseT *self) {
            return (*reinterpret_cast<u16 *>(self) & BITS_MASK<u16, 2, 0>) == BITS_MASK<u16, 2, 0> &&
                   (*reinterpret_cast<u16 *>(self) & BITS_MASK<u16, 5, 2>) != BITS_MASK<u16, 5, 2>;
        }

        static constexpr usize INST_WIDTH = sizeof(UInnerT);

        usize get_op_code() const { return slice_op_code(inner); }
    };

    class Instruction32R : public Instruction32 {
    protected:
        Instruction32R(usize op, usize rd, usize funct3, usize rs1, usize rs2, usize funct7)
                : Instruction32{static_cast<InnerT>(
                                        BITS_MASK<UInnerT, 2, 0> |
                                        get_slice<UInnerT, 5, 0, 2>(op) |
                                        get_slice<UInnerT, 5, 0, 7>(rd) |
                                        get_slice<UInnerT, 3, 0, 12>(funct3) |
                                        get_slice<UInnerT, 5, 0, 15>(rs1) |
                                        get_slice<UInnerT, 5, 0, 20>(rs2) |
                                        get_slice<UInnerT, 7, 0, 25>(funct7))} {}

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
        usize get_rd() const { return slice_rd(inner); }

        usize get_funct3() const { return slice_funct3(inner); }

        usize get_rs1() const { return slice_rs1(inner); }

        usize get_rs2() const { return slice_rs2(inner); }

        usize get_funct7() const { return slice_funct7(inner); }
    };

    class Instruction32I : public Instruction32 {
    protected:
        template<typename OP, typename Self, typename RegT>
        static void operate_shift(Self *self, RegT &reg) {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            usize rd = self->get_rd();
            if (rd != 0) {
                usize rs1 = self->get_rs1();
                XLenT imm = self->get_shift_amount();
                reg.set_x(rd, OP::op(reg.get_x(rs1), imm));
            }
            reg.inc_pc(INST_WIDTH);
        }

        Instruction32I(usize op, usize rd, usize funct3, usize rs1, UXLenT imm)
                : Instruction32{static_cast<InnerT>(
                                        BITS_MASK<UInnerT, 2, 0> |
                                        get_slice<UInnerT, 5, 0, 2>(op) |
                                        get_slice<UInnerT, 5, 0, 7>(rd) |
                                        get_slice<UInnerT, 3, 0, 12>(funct3) |
                                        get_slice<UInnerT, 5, 0, 15>(rs1) |
                                        get_slice<UInnerT, 12, 0, 20>(imm))} {}

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
        usize get_rd() const { return slice_rd(inner); }

        usize get_funct3() const { return slice_funct3(inner); }

        usize get_rs1() const { return slice_rs1(inner); }

        XLenT get_imm() const {
            InnerT imm32_20 = inner >> 20;
            return static_cast<XLenT>(imm32_20);
        }
    };

    class Instruction32S : public Instruction32 {
    protected:
        Instruction32S(usize op, usize funct3, usize rs1, usize rs2, UXLenT imm)
                : Instruction32{static_cast<InnerT>(
                                        BITS_MASK<UInnerT, 2, 0> |
                                        get_slice<UInnerT, 5, 0, 2>(op) |
                                        get_slice<UInnerT, 5, 0, 7>(imm) |
                                        get_slice<UInnerT, 3, 0, 12>(funct3) |
                                        get_slice<UInnerT, 5, 0, 15>(rs1) |
                                        get_slice<UInnerT, 5, 0, 20>(rs2) |
                                        get_slice<UInnerT, 12, 5, 25>(imm))} {}

    public:
        usize get_funct3() const { return slice_funct3(inner); }

        usize get_rs1() const { return slice_rs1(inner); }

        usize get_rs2() const { return slice_rs2(inner); }

        XLenT get_imm() const {
            InnerT imm12_7 = get_slice<UInnerT, 12, 7, 0>(inner);
            InnerT imm32_25 = inner >> 20 & BITS_MASK<UInnerT, 32, 5>;
            return static_cast<XLenT>(imm32_25 | imm12_7);
        }
    };

    class Instruction32B : public Instruction32 {
    protected:
        Instruction32B(usize op, usize funct3, usize rs1, usize rs2, UXLenT imm)
                : Instruction32{static_cast<InnerT>(
                                        BITS_MASK<UInnerT, 2, 0> |
                                        get_slice<UInnerT, 5, 0, 2>(op) |
                                        get_slice<UInnerT, 12, 11, 7>(imm) |
                                        get_slice<UInnerT, 5, 1, 8>(imm) |
                                        get_slice<UInnerT, 3, 0, 12>(funct3) |
                                        get_slice<UInnerT, 5, 0, 15>(rs1) |
                                        get_slice<UInnerT, 5, 0, 20>(rs2) |
                                        get_slice<UInnerT, 11, 5, 25>(imm) |
                                        get_slice<UInnerT, 13, 12, 31>(imm))} {}

    public:
        usize get_funct3() const { return slice_funct3(inner); }

        usize get_rs1() const { return slice_rs1(inner); }

        usize get_rs2() const { return slice_rs2(inner); }

        XLenT get_imm() const {
            InnerT imm8_7 = get_slice<UInnerT, 8, 7, 11>(inner);
            InnerT imm12_8 = get_slice<UInnerT, 12, 8, 1>(inner);
            InnerT imm32_25 = inner >> 20 & (BITS_MASK<UInnerT, 32, 12> | BITS_MASK<UInnerT, 11, 5>);
            return static_cast<XLenT>(imm32_25 | imm8_7 | imm12_8);
        }
    };

    class Instruction32U : public Instruction32 {
    protected:
        Instruction32U(usize op, usize rd, UXLenT imm)
                : Instruction32{static_cast<InnerT>(
                                        BITS_MASK<UInnerT, 2, 0> |
                                        get_slice<UInnerT, 5, 0, 2>(op) |
                                        get_slice<UInnerT, 5, 0, 7>(rd) |
                                        get_slice<UInnerT, 32, 12, 12>(imm))} {}

    public:
        usize get_rd() const { return slice_rd(inner); }

        XLenT get_imm() const {
            InnerT imm32_12 = get_slice<UInnerT, 32, 12, 12>(inner);
            return static_cast<XLenT>(imm32_12);
        }
    };

    class Instruction32J : public Instruction32 {
    protected:
        Instruction32J(usize op, usize rd, UXLenT imm)
                : Instruction32{static_cast<InnerT>(
                                        BITS_MASK<UInnerT, 2, 0> |
                                        get_slice<UInnerT, 5, 0, 2>(op) |
                                        get_slice<UInnerT, 5, 0, 7>(rd) |
                                        get_slice<UInnerT, 20, 12, 12>(imm) |
                                        get_slice<UInnerT, 12, 11, 20>(imm) |
                                        get_slice<UInnerT, 11, 1, 21>(imm) |
                                        get_slice<UInnerT, 21, 20, 31>(imm))} {}

    public:
        usize get_rd() const { return slice_rd(inner); }

        XLenT get_imm() const {
            InnerT imm20_12 = get_slice<UInnerT, 20, 12, 12>(inner);
            InnerT imm21_20 = get_slice<UInnerT, 21, 20, 11>(inner);
            InnerT imm32_21 = inner >> 20 & (BITS_MASK<UInnerT, 32, 20> | BITS_MASK<UInnerT, 11, 1>);
            return static_cast<XLenT>(imm32_21 | imm20_12 | imm21_20);
        }
    };

    class InstructionBranchSet : public Instruction32B {
    protected:
        InstructionBranchSet(usize funct3, usize rs1, usize rs2, UXLenT imm)
                : Instruction32B{OP_CODE, funct3, rs1, rs2, imm} {}

        template<typename OP, typename RegT>
        bool operate_on(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            usize rs1 = get_rs1();
            usize rs2 = get_rs2();

            if (OP::op(reg.get_x(rs1), reg.get_x(rs2))) {
                XLenT imm = get_imm();
                if constexpr (IALIGN == 32)
                    if (get_slice<UXLenT, 2, 0>(imm) != 0) return false;
                reg.inc_pc(imm);
            } else {
                reg.inc_pc(INST_WIDTH);
            }

            return true;
        }

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b11000;
    };

    class InstructionLoadSet : public Instruction32I {
    protected:
        InstructionLoadSet(usize rd, usize funct3, usize rs1, UXLenT imm)
                : Instruction32I{OP_CODE, rd, funct3, rs1, imm} {}

        template<typename ValT, typename RegT, typename MemT>
        bool operate_on(RegT &reg, MemT &mem) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);
            static_assert(std::is_base_of<Memory, MemT>::value);

            size_t rd = get_rd();
            size_t rs1 = get_rs1();
            XLenT imm = get_imm();
            ValT *ptr = mem.template address<ValT>(static_cast<UXLenT>(reg.get_x(rs1)) + imm);
            if (ptr == nullptr) return false;
            if (rd != 0) reg.set_x(rd, *ptr);  // todo: this may fail from interrupt
            reg.inc_pc(INST_WIDTH);
            return true;
        }

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b00000;
    };

    class InstructionStoreSet : public Instruction32S {
    protected:
        InstructionStoreSet(usize funct3, usize rs1, usize rs2, UXLenT imm)
                : Instruction32S{OP_CODE, funct3, rs1, rs2, imm} {}

        template<typename ValT, typename RegT, typename MemT>
        bool operate_on(RegT &reg, MemT &mem) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);
            static_assert(std::is_base_of<Memory, MemT>::value);

            size_t rs1 = get_rs1();
            size_t rs2 = get_rs2();
            XLenT imm = get_imm();
            ValT *ptr = mem.template address<ValT>(static_cast<UXLenT>(reg.get_x(rs1)) + imm);
            if (ptr == nullptr) return false;
            *ptr = static_cast<ValT>(reg.get_x(rs2)); // todo: this may fail from interrupt
            reg.inc_pc(INST_WIDTH);
            return true;
        }

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b01000;
    };

    class InstructionArithImmSet : public Instruction32I {
    protected:
        InstructionArithImmSet(usize rd, usize funct3, usize rs1, UXLenT imm)
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
        InstructionFenceSet(usize rd, usize funct3, usize rs1, UXLenT imm)
                : Instruction32I{OP_CODE, rd, funct3, rs1, imm} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b00011;
    };

    class InstructionSystemSet : public Instruction32I {
    protected:
        InstructionSystemSet(usize rd, usize funct3, usize rs1, UXLenT imm)
                : Instruction32I{OP_CODE, rd, funct3, rs1, imm} {}

    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b11100;
    };

#endif // defined(__RV_32_BIT__) || defined(__RV_64_BIT__)
#if defined(__RV_64_BIT__)

    class InstructionArithImmWSet : public Instruction32I {
    protected:
        InstructionArithImmWSet(usize rd, usize funct3, usize rs1, UXLenT imm)
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

#endif // defined(__RV_64_BIT__)
#if defined(__RV_CUSTOM_0__)
    class InstructionCustome0 : public Instruction32 {
    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b00010;

        explicit InstructionCustome0(InnerT val) : Instruction32{val} {}
    };
#endif // defined(__RV_CUSTOM_0__)
#if defined(__RV_CUSTOM_1__)
    class InstructionCustome1 : public Instruction32 {
    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b01010;

        explicit InstructionCustome1(InnerT val) : Instruction32{val} {}
    };
#endif // defined(__RV_CUSTOM_1__)
#if defined(__RV_CUSTOM_2__)
    class InstructionCustome2 : public Instruction32 {
    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b10110;

        explicit InstructionCustome2(InnerT val) : Instruction32{val} {}
    };
#endif // defined(__RV_CUSTOM_2__)
#if defined(__RV_CUSTOM_3__)
    class InstructionCustome3 : public Instruction32 {
    public:
        using BaseT = Instruction32;

        static bool is_self_type(BaseT *self) { return self->get_op_code() == OP_CODE; }

        static constexpr UInnerT OP_CODE = 0b11110;

        explicit InstructionCustome3(InnerT val) : Instruction32{val} {}
    };
#endif // defined(__RV_CUSTOM_3__)
}


#endif //RISC_V_ISA_INSTRUCTION_HPP
