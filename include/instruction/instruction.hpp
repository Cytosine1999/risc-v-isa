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

    };

#ifdef __RVC__

    class Instruction16 : public Instruction {
        using InnerT = i16;
        using UInnerT = u16;

        static constexpr usize INST_WIDTH = sizeof(UInnerT);

        InnerT inner;
    };

#endif

    class Instruction32 : public Instruction {
    protected:
        using InnerT = i32;
        using UInnerT = u32;

        static UInnerT slice_op_code(UInnerT val) { return get_slice<UInnerT, 7, 0>(val); }

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

    public:
        static constexpr usize INST_WIDTH = sizeof(UInnerT);

        explicit Instruction32(InnerT val) : inner(val) {}

        usize get_op_code() const { return slice_op_code(inner); }

    };

    class Instruction32R : public Instruction32 {
    public:
        Instruction32R(usize op, usize rd, usize funct3, usize rs1, usize rs2, usize funct7)
                : Instruction32{static_cast<InnerT>(
                                        get_slice<UInnerT, 7, 0, 0>(op) &
                                        get_slice<UInnerT, 5, 0, 7>(rd) &
                                        get_slice<UInnerT, 3, 0, 12>(funct3) &
                                        get_slice<UInnerT, 5, 0, 15>(rs1) &
                                        get_slice<UInnerT, 5, 0, 20>(rs2) &
                                        get_slice<UInnerT, 7, 0, 25>(funct7))} {}

        usize get_rd() const { return slice_rd(inner); }

        usize get_funct3() const { return slice_funct3(inner); }

        usize get_rs1() const { return slice_rs1(inner); }

        usize get_rs2() const { return slice_rs2(inner); }

        usize get_funct7() const { return slice_funct7(inner); }

    };

    class Instruction32I : public Instruction32 {
    public:
        Instruction32I(usize op, usize rd, usize funct3, usize rs1, UXLenT imm)
                : Instruction32{static_cast<InnerT>(
                                        get_slice<UInnerT, 7, 0, 0>(op) &
                                        get_slice<UInnerT, 5, 0, 7>(rd) &
                                        get_slice<UInnerT, 3, 0, 12>(funct3) &
                                        get_slice<UInnerT, 5, 0, 15>(rs1) &
                                        get_slice<UInnerT, 12, 0, 20>(imm))} {}

        usize get_rd() const { return slice_rd(inner); }

        usize get_funct3() const { return slice_funct3(inner); }

        usize get_rs1() const { return slice_rs1(inner); }

        XLenT get_imm() const {
            InnerT imm32_20 = inner >> 20;
            return static_cast<XLenT>(imm32_20);
        }
    };

    class Instruction32S : public Instruction32 {
    public:
        Instruction32S(usize op, usize funct3, usize rs1, usize rs2, UXLenT imm)
                : Instruction32{static_cast<InnerT>(
                                        get_slice<UInnerT, 7, 0, 0>(op) &
                                        get_slice<UInnerT, 5, 0, 7>(imm) &
                                        get_slice<UInnerT, 3, 0, 12>(funct3) &
                                        get_slice<UInnerT, 5, 0, 15>(rs1) &
                                        get_slice<UInnerT, 5, 0, 20>(rs2) &
                                        get_slice<UInnerT, 12, 5, 25>(imm))} {}

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
    public:
        Instruction32B(usize op, usize funct3, usize rs1, usize rs2, UXLenT imm)
                : Instruction32{static_cast<InnerT>(
                                        get_slice<UInnerT, 7, 0, 0>(op) &
                                        get_slice<UInnerT, 12, 11, 7>(imm) &
                                        get_slice<UInnerT, 5, 1, 8>(imm) &
                                        get_slice<UInnerT, 3, 0, 12>(funct3) &
                                        get_slice<UInnerT, 5, 0, 15>(rs1) &
                                        get_slice<UInnerT, 5, 0, 20>(rs2) &
                                        get_slice<UInnerT, 11, 5, 25>(imm) &
                                        get_slice<UInnerT, 13, 12, 31>(imm))} {}

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
    public:
        Instruction32U(usize op, usize rd, UXLenT imm)
                : Instruction32{static_cast<InnerT>(
                                        get_slice<UInnerT, 7, 0, 0>(op) &
                                        get_slice<UInnerT, 5, 0, 7>(rd) &
                                        get_slice<UInnerT, 32, 12, 12>(imm))} {}

        usize get_rd() const { return slice_rd(inner); }

        XLenT get_imm() const {
            InnerT imm32_12 = get_slice<UInnerT, 32, 12, 12>(inner);
            return static_cast<XLenT>(imm32_12);
        }

    };

    class Instruction32J : public Instruction32 {
    public:
        Instruction32J(usize op, usize rd, UXLenT imm)
                : Instruction32{static_cast<InnerT>(
                                        get_slice<UInnerT, 7, 0, 0>(op) &
                                        get_slice<UInnerT, 5, 0, 7>(rd) &
                                        get_slice<UInnerT, 20, 12, 12>(imm) &
                                        get_slice<UInnerT, 12, 11, 20>(imm) &
                                        get_slice<UInnerT, 11, 1, 21>(imm) &
                                        get_slice<UInnerT, 21, 20, 31>(imm))} {}

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
        template<typename OP, typename RegT>
        void operate_on(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            usize rs1 = get_rs1();
            usize rs2 = get_rs2();
            XLenT imm = get_imm();
            reg.inc_pc(OP::op(reg.get_x(rs1), reg.get_x(rs2)) ? imm : INST_WIDTH);
        }

    public:
        static constexpr UInnerT OP_CODE = 0b1100011;

        InstructionBranchSet(usize funct3, usize rs1, usize rs2, UXLenT imm)
                : Instruction32B{OP_CODE, funct3, rs1, rs2, imm} {}
    };

    class InstructionLoadSet : public Instruction32I {
    protected:
        template<typename ValT, typename RegT, typename MemT>
        bool operate_on(RegT &reg, MemT &mem) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);
            static_assert(std::is_base_of<Memory, MemT>::value);

            size_t rd = get_rd();
            size_t rs1 = get_rs1();
            XLenT imm = get_imm();
            ValT *ptr = mem.template address<ValT>(static_cast<UXLenT>(reg.get_x(rs1)) + imm);
            if (ptr == nullptr) return false;
            if (rd != 0) reg.set_x(rd, *ptr);
            reg.inc_pc(INST_WIDTH);
            return true;
        }

    public:
        static constexpr UInnerT OP_CODE = 0b0000011;

        InstructionLoadSet(usize rd, usize funct3, usize rs1, UXLenT imm)
                : Instruction32I{OP_CODE, rd, funct3, rs1, imm} {}
    };

    class InstructionStoreSet : public Instruction32S {
    protected:
        template<typename ValT, typename RegT, typename MemT>
        bool operate_on(RegT &reg, MemT &mem) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);
            static_assert(std::is_base_of<Memory, MemT>::value);

            size_t rs1 = get_rs1();
            size_t rs2 = get_rs2();
            XLenT imm = get_imm();
            ValT *ptr = mem.template address<ValT>(static_cast<UXLenT>(reg.get_x(rs1)) + imm);
            if (ptr == nullptr) return false;
            *ptr = static_cast<ValT>(reg.get_x(rs2));
            reg.inc_pc(INST_WIDTH);
            return true;
        }

    public:
        static constexpr UInnerT OP_CODE = 0b0100011;

        InstructionStoreSet(usize funct3, usize rs1, usize rs2, UXLenT imm)
                : Instruction32S{OP_CODE, funct3, rs1, rs2, imm} {}
    };

    class InstructionArithImmSet : public Instruction32I {
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
        static constexpr UInnerT OP_CODE = 0b0010011;

        InstructionArithImmSet(usize rd, usize funct3, usize rs1, UXLenT imm)
                : Instruction32I{OP_CODE, rd, funct3, rs1, imm} {}
    };

    class InstructionArithRegSet : public Instruction32R {
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
        static constexpr UInnerT OP_CODE = 0b0110011;

        InstructionArithRegSet(usize rd, usize funct3, usize rs1, usize rs2, usize funct7)
                : Instruction32R{OP_CODE, rd, funct3, rs1, rs2, funct7} {}
    };

    class InstructionFenceSet : public Instruction32I {
    public:
        static constexpr UInnerT OP_CODE = 0b0001111;

        InstructionFenceSet(usize rd, usize funct3, usize rs1, UXLenT imm)
                : Instruction32I{OP_CODE, rd, funct3, rs1, imm} {}
    };

    class InstructionSystemSet : public Instruction32I {
    public:
        static constexpr UInnerT OP_CODE = 0b1110011;

        InstructionSystemSet(usize rd, usize funct3, usize rs1, UXLenT imm)
                : Instruction32I{OP_CODE, rd, funct3, rs1, imm} {}
    };

#if defined(__RVCUSTOM0__)
    class InstructionCustome0 : public Instruction32 {
    public:
        static constexpr UInnerT OP_CODE = 0b0001011;

    };
#endif // defined(__RVCUSTOM0__)
#if defined(__RVCUSTOM1__)
    class InstructionCustome1 : public Instruction32 {
    public:
        static constexpr UInnerT OP_CODE = 0b0101011;

    };
#endif // defined(__RVCUSTOM1__)
#if defined(__RVCUSTOM2__)
    class InstructionCustome2 : public Instruction32 {
    public:
        static constexpr UInnerT OP_CODE = 0b1011011;

    };
#endif // defined(__RVCUSTOM2__)
#if defined(__RVCUSTOM3__)
    class InstructionCustome3 : public Instruction32 {
    public:
        static constexpr UInnerT OP_CODE = 0b1111011;

    };
#endif // defined(__RVCUSTOM3__)

#ifdef __RVC__

    template<>
    Instruction16 *dyn_cast<Instruction16 *, Instruction *>(Instruction *self) {
        if ((*reinterpret_cast<u16 *>(self) & BITS_MASK<u16, 2, 0>) != BITS_MASK<u16, 2, 0>)
            return reinterpret_cast<Instruction16 *>(self);
        else
            return nullptr;
    }

#endif

    template<>
    Instruction32 *dyn_cast<Instruction32 *, Instruction *>(Instruction *self) {
#ifdef __RVC__
        if (dyn_cast<Instruction16>(self) == nullptr &&
            (*reinterpret_cast<u16 *>(self) & BITS_MASK<u16, 5, 2>) != BITS_MASK<u16, 5, 2>)
#else
        if ((*reinterpret_cast<u16 *>(self) & BITS_MASK<u16, 5, 2>) != BITS_MASK<u16, 5, 2>)
#endif
            return reinterpret_cast<Instruction32 *>(self);
        else
            return nullptr;
    }

    // todo: more dyn_cast
}


#endif //RISC_V_ISA_INSTRUCTION_HPP
