#ifndef RISC_V_ISA_INSTRUCTION_HPP
#define RISC_V_ISA_INSTRUCTION_HPP


#include "utility.hpp"
#include "register/register_file.hpp"
#include "memory/memory.hpp"


namespace risc_v_isa {
    class Instruction {

    };

#ifdef __RVC__

    class Instruction16 : public Instruction {

    };

#endif
    class Instruction32 : public Instruction {
    protected:
        using InnerT = i32;
        using UInnerT = u32;

        static constexpr usize INST_WIDTH = sizeof(UInnerT);

        static UInnerT slice_op_code(UInnerT val) {
            UInnerT ret = get_slice<UInnerT, 7, 0>(val);
            return ret;
        }

        static UInnerT slice_rd(UInnerT val) { return get_slice<UInnerT, 12, 7>(val); }

        static UInnerT slice_funct3(UInnerT val) { return get_slice<UInnerT, 15, 12>(val); }

        static UInnerT slice_rs1(UInnerT val) { return get_slice<UInnerT, 20, 15>(val); }

        static UInnerT slice_rs2(UInnerT val) { return get_slice<UInnerT, 25, 20>(val); }

        static UInnerT slice_funct7(UInnerT val) { return get_slice<UInnerT, 32, 25>(val); }

        static UInnerT slice_funct_shift32(UInnerT val) { return get_slice<UInnerT, 32, 25>(val); }

        static UInnerT slice_shift_amount32(UInnerT val) { return get_slice<UInnerT, 25, 20>(val); }

        InnerT inner;

    public:
        explicit Instruction32(InnerT val) : inner(val) {}

        usize get_op_code() const { return slice_op_code(inner); }

    };

    class Instruction32R : public Instruction32 {
    public:
        usize get_rd() const { return slice_rd(inner); }

        usize get_funct3() const { return slice_funct3(inner); }

        usize get_rs1() const { return slice_rs1(inner); }

        usize get_rs2() const { return slice_rs2(inner); }

        usize get_funct7() const { return slice_funct7(inner); }

    };

    class Instruction32I : public Instruction32 {
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
    public:
        usize get_rd() const { return slice_rd(inner); }

        XLenT get_imm() const {
            InnerT imm32_12 = get_slice<UInnerT, 32, 12, 12>(inner);
            return static_cast<XLenT>(imm32_12);
        }

    };

    class Instruction32J : public Instruction32 {
    public:
        usize get_rd() const { return slice_rd(inner); }

        XLenT get_imm() const {
            InnerT imm20_12 = get_slice<UInnerT, 20, 12, 12>(inner);
            InnerT imm21_20 = get_slice<UInnerT, 21, 20, 11>(inner);
            InnerT imm32_21 = inner >> 20 & (BITS_MASK<UInnerT, 32, 20> | BITS_MASK<UInnerT, 11, 1>);
            return static_cast<XLenT>(imm32_21 | imm20_12 | imm21_20);
        }

    };

    class InstructionLoadSet : public Instruction32I {
    protected:
        template <typename ValT, typename RegT, typename MemT>
        bool operate_on(RegT &reg, MemT &mem) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);
            static_assert(std::is_base_of<Memory, MemT>::value);

            size_t rd = get_rd();
            size_t rs1 = get_rs1();
            XLenT imm = get_imm();
            ValT *ptr = mem.template address<ValT>(static_cast<UXLenT>(reg.get_x(rs1)) + imm);
            if (ptr == nullptr) return false;
            InnerT val = *ptr;
            if (rd != 0) reg.set_x(rd, val);
            reg.inc_pc(INST_WIDTH);
            return true;
        }

    public:
        static constexpr UInnerT OP_CODE = 0b0000011;

    };

    class InstructionStoreSet : public Instruction32S {
    protected:
        template <typename ValT, typename RegT, typename MemT>
        bool operate_on(RegT &reg, MemT &mem) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);
            static_assert(std::is_base_of<Memory, MemT>::value);

            size_t rs1 = get_rs1();
            size_t rs2 = get_rs2();
            XLenT imm = get_imm();
            ValT *ptr = mem.template address<ValT>(static_cast<UXLenT>(reg.get_x(rs1)) + imm);
            if (ptr == nullptr) return false;
            *ptr = static_cast<ValT>(rs2);
            reg.inc_pc(INST_WIDTH);
            return true;
        }

    public:
        static constexpr UInnerT OP_CODE = 0b0100011;

    };

    class InstructionArithImmSet : public Instruction32I {
    protected:
        template <typename OP, typename RegT>
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

    };

    class InstructionArithRegSet : public Instruction32R {
    protected:
        template <typename OP, typename RegT>
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
        static constexpr UInnerT FUNC_7 = 0b0000000;
    };

    class InstructionFenceSet : public Instruction32I {
    public:
        static constexpr UInnerT OP_CODE = 0b0001111;

        usize get_unused() const { return inner & (BITS_MASK<UInnerT, 12, 7> | BITS_MASK<UInnerT, 20, 15>); }
    };

    class InstructionSystemSet : public Instruction32I {
    public:
        static constexpr UInnerT OP_CODE = 0b1110011;
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
    Instruction16 *dyn_cast<Instruction16, Instruction *>(Instruction *self) {
        if ((*reinterpret_cast<u16 *>(self) & BITS_MASK<u16, 2, 0>) != BITS_MASK<u16, 2, 0>)
            return reinterpret_cast<Instruction16 *>(self);
        else
            return nullptr;
    }

#endif

    template<>
    Instruction32 *dyn_cast<Instruction32, Instruction *>(Instruction *self) {
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
}


#endif //RISC_V_ISA_INSTRUCTION_HPP
