#ifndef RISCV_ISA_INSTRUCTION_TEST_HPP
#define RISCV_ISA_INSTRUCTION_TEST_HPP


#include "riscv_isa_utility.hpp"
#include "test.hpp"
#include "instruction/instruction.hpp"
#include "instruction/rv32i.hpp"
#include "instruction/instruction_visitor.hpp"


namespace riscv_isa {
    constexpr usize R_INR = 13;
    constexpr usize R_MAX = 0b100000;

    constexpr usize IMM_I_INR = 1021;
    constexpr usize IMM_I_MAX = 0b1000000000000;

    constexpr usize IMM_U_INR = 99991;
    constexpr usize IMM_U_MAX = 0b100000000000000000000;

    constexpr usize IMM_S_1_INR = 13;
    constexpr usize IMM_S_1_MAX = 0b100000;
    constexpr usize IMM_S_2_INR = 31;
    constexpr usize IMM_S_2_MAX = 0b10000000;

    constexpr usize OP_CODE = 2;
    constexpr usize RD = 7;
    constexpr usize FUNCT3 = 12;
    constexpr usize RS1 = 15;
    constexpr usize RS2 = 20;
    constexpr usize IMM_I = 20;
    constexpr usize IMM_U = 12;
    constexpr usize IMM_S_1 = 7;
    constexpr usize IMM_S_2 = 25;
    constexpr usize FUNCT7 = 25;

    template<typename InputT>
    class CheckVisitor : public InstructionVisitor<CheckVisitor<InputT>, void> {
    public:
        using RetT = void;

        RetT illegal_instruction(riscv_isa_unused const Instruction *inst) {
            ASSERT((std::is_same<void, InputT>::value));
        }

#define _riscv_isa_check_instruction(NAME, name) \
        RetT visit_##name##_inst(riscv_isa_unused const NAME##Inst *inst) { \
            ASSERT((std::is_same<NAME##Inst, InputT>::value)); \
        }
        riscv_isa_instruction_map(_riscv_isa_check_instruction)
#undef _riscv_isa_check_instruction
    };

    template<typename T, typename U, bool flag = std::is_same<T, U>::value>
    struct _check_dyn_cast;

    template<typename T, typename U>
    struct _check_dyn_cast<T, U, true> {
    public:
        static void inner(Instruction *inst) { ASSERT_NE(dyn_cast<U>(inst), static_cast<U *>(nullptr)); }
    };

    template<typename T, typename U>
    struct _check_dyn_cast<T, U, false> {
    public:
        static void inner(Instruction *inst) { ASSERT_EQ(dyn_cast<U>(inst), static_cast<U *>(nullptr)); }
    };

    template<typename T, typename U>
    void check_dyn_cast(Instruction *inst) { _check_dyn_cast<T, U>::inner(inst); }

    template<typename T>
    T *_dyn_cast_with_void(Instruction *inst);

    template<>
    void *_dyn_cast_with_void<void>(riscv_isa_unused Instruction *inst) { return nullptr; }

    template<typename T>
    T *_dyn_cast_with_void(Instruction *inst) { return dyn_cast<T>(inst); }

    template<typename T>
    T *check_all_dyn_cast(Instruction *inst) {
#define _riscv_isa_check_dyn_cast_instruction(NAME, name) \
        check_dyn_cast<T, NAME##Inst>(inst);
        riscv_isa_instruction_map(_riscv_isa_check_dyn_cast_instruction)
#undef _riscv_isa_check_dyn_cast_instruction

        return _dyn_cast_with_void<T>(inst);
    }

    void check_invalid_16_op_code(usize op_code) {
        for (u16 i = 0; i < 0b100000000000000; i += 5417) {
            u16 val = op_code + (i << 2u);
            auto _inst = reinterpret_cast<Instruction *>(&val);
            CheckVisitor<void>{}.visit_in_memory(_inst, 2);
            check_all_dyn_cast<void>(_inst);
        }
    }

    void check_invalid_32_op_code(usize op_code) {
        for (u32 i = 0; i < 0b10000000000000000000000000; i += 999983) {
            u32 val = 0b11u + (op_code << OP_CODE) + (i << 7u);
            auto _inst = reinterpret_cast<Instruction *>(&val);
            CheckVisitor<void>{}.visit_in_memory(_inst, 4);
            check_all_dyn_cast<void>(_inst);
        }
    }

    template<typename T, usize op_code, usize funct3>
    void check_i_type_inst(usize rd, usize rs1, usize imm_i) {
        u32 val = 0b11u | (op_code << OP_CODE) | (rd << RD) | (funct3 << FUNCT3) | (rs1 << RS1) |
                  (imm_i << IMM_I);
        auto _inst = reinterpret_cast<Instruction *>(&val);
        CheckVisitor<T>{}.visit_in_memory(_inst, 4);
        auto inst = check_all_dyn_cast<T>(_inst);

        ASSERT_EQ(inst->get_op_code(), op_code);
        ASSERT_EQ(inst->get_rd(), rd);
        ASSERT_EQ(inst->get_funct3(), funct3);
        ASSERT_EQ(inst->get_rs1(), rs1);
        ASSERT_EQ(inst->get_imm(), static_cast<i32>(imm_i << IMM_I) >> IMM_I);
    }

    void check_i_type_invalid(usize op_code, usize rd, usize funct3, usize rs1, usize imm_i) {
        u32 val = 0b11u | (op_code << OP_CODE) | (rd << RD) | (funct3 << FUNCT3) | (rs1 << RS1) |
                  (imm_i << IMM_I);
        auto _inst = reinterpret_cast<Instruction *>(&val);
        CheckVisitor<void>{}.visit_in_memory(_inst, 4);
        check_all_dyn_cast<void>(_inst);
    }

    template<typename T, usize op_code, usize funct3>
    void check_s_type_inst(usize imm_s_1, usize rs1, usize rs2, usize imm_s_2) {
        u32 val =
                0b11u | (op_code << OP_CODE) | (imm_s_1 << IMM_S_1) | (funct3 << FUNCT3) | (rs1 << RS1) | (rs2 << RS2) |
                (imm_s_2 << IMM_S_2);
        auto _inst = reinterpret_cast<Instruction *>(&val);
        CheckVisitor<T>{}.visit_in_memory(_inst, 4);
        auto inst = check_all_dyn_cast<T>(_inst);

        ASSERT_EQ(inst->get_op_code(), op_code);
        ASSERT_EQ(inst->get_funct3(), funct3);
        ASSERT_EQ(inst->get_rs1(), rs1);
        ASSERT_EQ(inst->get_rs2(), rs2);
        ASSERT_EQ(inst->get_imm(), static_cast<i32>((imm_s_1 | (imm_s_2 << 5u)) << IMM_I) >> IMM_I);
    }

    void check_s_type_invalid(usize op_code, usize imm_s_1, usize funct3, usize rs1, usize rs2, usize imm_s_2) {
        u32 val = 0b11u | (op_code << OP_CODE) | (imm_s_1 << IMM_S_1) | (funct3 << FUNCT3) | (rs1 << RS1) |
                  (rs2 << RS2) | (imm_s_2 << IMM_S_2);
        auto _inst = reinterpret_cast<Instruction *>(&val);
        CheckVisitor<void>{}.visit_in_memory(_inst, 4);
        check_all_dyn_cast<void>(_inst);
    }

    template<typename T, usize op_code, usize funct3>
    void check_b_type_inst(usize imm_s_1, usize rs1, usize rs2, usize imm_s_2) {
        u32 val = 0b11u | (op_code << OP_CODE) | (imm_s_1 << IMM_S_1) | (funct3 << FUNCT3) | (rs1 << RS1) |
                  (rs2 << RS2) | (imm_s_2 << IMM_S_2);
        auto _inst = reinterpret_cast<Instruction *>(&val);
        CheckVisitor<T>{}.visit_in_memory(_inst, 4);
        auto inst = check_all_dyn_cast<T>(_inst);

        ASSERT_EQ(inst->get_op_code(), op_code);
        ASSERT_EQ(inst->get_funct3(), funct3);
        ASSERT_EQ(inst->get_rs1(), rs1);
        ASSERT_EQ(inst->get_rs2(), rs2);
        ASSERT_EQ(inst->get_imm(), static_cast<i32>(
                ((imm_s_1 & 0b11110u) | ((imm_s_1 & 0b00001u) << 11u) | ((imm_s_2 & 0b0111111u) << 5u) |
                 ((imm_s_2 & 0b1000000u) << 6u)) << 19u) >> 19u);
    }

    void check_b_type_invalid(usize op_code, usize imm_s_1, usize funct3, usize rs1, usize rs2, usize imm_s_2) {
        u32 val = 0b11u | (op_code << OP_CODE) | (imm_s_1 << IMM_S_1) | (funct3 << FUNCT3) | (rs1 << RS1) |
                  (rs2 << RS2) | (imm_s_2 << IMM_S_2);
        auto _inst = reinterpret_cast<Instruction *>(&val);
        CheckVisitor<void>{}.visit_in_memory(_inst, 4);
        check_all_dyn_cast<void>(_inst);
    }

    template<typename T, usize op_code, usize funct3, usize funct7>
    void check_r_type_inst(usize rd, usize rs1, usize rs2) {
        u32 val = 0b11u | (op_code << OP_CODE) | (rd << RD) | (funct3 << FUNCT3) | (rs1 << RS1) |
                  (rs2 << RS2) | (funct7 << FUNCT7);
        auto _inst = reinterpret_cast<Instruction *>(&val);
        CheckVisitor<T>{}.visit_in_memory(_inst, 4);
        auto inst = check_all_dyn_cast<T>(_inst);

        ASSERT_EQ(inst->get_op_code(), op_code);
        ASSERT_EQ(inst->get_rd(), rd);
        ASSERT_EQ(inst->get_funct3(), funct3);
        ASSERT_EQ(inst->get_rs1(), rs1);
        ASSERT_EQ(inst->get_rs2(), rs2);
        ASSERT_EQ(inst->get_funct7(), funct7);
    }

    template<typename T, usize op_code>
    void check_u_type_inst(usize rd, typename xlen_32_trait::UXLenT imm_j) {
        u32 val = 0b11u | (op_code << OP_CODE) | (rd << RD) | (imm_j << IMM_U);
        auto _inst = reinterpret_cast<Instruction *>(&val);
        CheckVisitor<T>{}.visit_in_memory(_inst, 4);
        auto inst = check_all_dyn_cast<T>(_inst);

        ASSERT_EQ(inst->get_op_code(), op_code);
        ASSERT_EQ(inst->get_rd(), rd);
        ASSERT_EQ(inst->get_imm(), static_cast<typename xlen_32_trait::XLenT>(imm_j << IMM_U));
    }
}


#endif //RISCV_ISA_INSTRUCTION_TEST_HPP
