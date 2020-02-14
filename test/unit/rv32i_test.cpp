#include "test.hpp"
#include "instruction_test.hpp"
#include "utility.hpp"
#include "instruction/instruction.hpp"
#include "instruction/rv32i.hpp"
#include "instruction/instruction_visitor.hpp"

using namespace riscv_isa;


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

void check_invalid_op_code(usize op_code) {
    for (u32 i = 0; i < 0b10000000000000000000000000; i += 999983) {
        u32 val = 0b11u + (op_code << OP_CODE) + (i << 7u);
        auto _inst = reinterpret_cast<Instruction *>(&val);
        CheckVisitor<void>{}.visit(_inst);
        check_all_dyn_cast<void>(_inst);
    }
}

template<typename T, usize op_code, usize funct3>
void check_i_type_inst(usize rd, usize rs1, usize imm_i) {
    u32 val = 0b11u | (op_code << OP_CODE) | (rd << RD) | (funct3 << FUNCT3) | (rs1 << RS1) |
              (imm_i << IMM_I);
    auto _inst = reinterpret_cast<Instruction *>(&val);
    CheckVisitor<T>{}.visit(_inst);
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
    CheckVisitor<void>{}.visit(_inst);
    check_all_dyn_cast<void>(_inst);
}

template<typename T, usize op_code, usize funct3>
void check_s_type_inst(usize imm_s_1, usize rs1, usize rs2, usize imm_s_2) {
    u32 val = 0b11u | (op_code << OP_CODE) | (imm_s_1 << IMM_S_1) | (funct3 << FUNCT3) | (rs1 << RS1) | (rs2 << RS2) |
              (imm_s_2 << IMM_S_2);
    auto _inst = reinterpret_cast<Instruction *>(&val);
    CheckVisitor<T>{}.visit(_inst);
    auto inst = check_all_dyn_cast<T>(_inst);

    ASSERT_EQ(inst->get_op_code(), op_code);
    ASSERT_EQ(inst->get_funct3(), funct3);
    ASSERT_EQ(inst->get_rs1(), rs1);
    ASSERT_EQ(inst->get_rs2(), rs2);
    ASSERT_EQ(inst->get_imm(), static_cast<i32>((imm_s_1 | (imm_s_2 << 5u)) << IMM_I) >> IMM_I);
}

void check_s_type_invalid(usize op_code, usize imm_s_1, usize funct3, usize rs1, usize rs2, usize imm_s_2) {
    u32 val = 0b11u | (op_code << OP_CODE) | (imm_s_1 << IMM_S_1) | (funct3 << FUNCT3) | (rs1 << RS1) | (rs2 << RS2) |
              (imm_s_2 << IMM_S_2);
    auto _inst = reinterpret_cast<Instruction *>(&val);
    CheckVisitor<void>{}.visit(_inst);
    check_all_dyn_cast<void>(_inst);
}

template<typename T, usize op_code, usize funct3>
void check_b_type_inst(usize imm_s_1, usize rs1, usize rs2, usize imm_s_2) {
    u32 val = 0b11u | (op_code << OP_CODE) | (imm_s_1 << IMM_S_1) | (funct3 << FUNCT3) | (rs1 << RS1) | (rs2 << RS2) |
              (imm_s_2 << IMM_S_2);
    auto _inst = reinterpret_cast<Instruction *>(&val);
    CheckVisitor<T>{}.visit(_inst);
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
    u32 val = 0b11u | (op_code << OP_CODE) | (imm_s_1 << IMM_S_1) | (funct3 << FUNCT3) | (rs1 << RS1) | (rs2 << RS2) |
              (imm_s_2 << IMM_S_2);
    auto _inst = reinterpret_cast<Instruction *>(&val);
    CheckVisitor<void>{}.visit(_inst);
    check_all_dyn_cast<void>(_inst);
}

template<typename T, usize op_code, usize funct3, usize funct7>
void check_r_type_inst(usize rd, usize rs1, usize rs2) {
    u32 val = 0b11u | (op_code << OP_CODE) | (rd << RD) | (funct3 << FUNCT3) | (rs1 << RS1) |
              (rs2 << RS2) | (funct7 << FUNCT7);
    auto _inst = reinterpret_cast<Instruction *>(&val);
    CheckVisitor<T>{}.visit(_inst);
    auto inst = check_all_dyn_cast<T>(_inst);

    ASSERT_EQ(inst->get_op_code(), op_code);
    ASSERT_EQ(inst->get_rd(), rd);
    ASSERT_EQ(inst->get_funct3(), funct3);
    ASSERT_EQ(inst->get_rs1(), rs1);
    ASSERT_EQ(inst->get_rs2(), rs2);
    ASSERT_EQ(inst->get_funct7(), funct7);
}

template<typename T, usize op_code, usize funct3, usize funct_shift>
void check_shift_imm_inst(usize rd, usize rs1, usize shamt) {
    u32 val = 0b11u | (op_code << OP_CODE) | (rd << RD) | (funct3 << FUNCT3) | (rs1 << RS1) |
              ((shamt | funct_shift) << IMM_I);
    auto _inst = reinterpret_cast<Instruction *>(&val);
    CheckVisitor<T>{}.visit(_inst);
    auto inst = check_all_dyn_cast<T>(_inst);

    ASSERT_EQ(inst->get_op_code(), op_code);
    ASSERT_EQ(inst->get_rd(), rd);
    ASSERT_EQ(inst->get_funct3(), funct3);
    ASSERT_EQ(inst->get_rs1(), rs1);
    ASSERT_EQ(inst->get_shamt(), shamt);
    ASSERT_EQ(inst->get_funct_shift(), funct_shift);
}

void check_invalid_funct_shift(usize op_code, usize rd, usize funct3, usize rs1, usize shamt,
                               usize funct_shift) {
    u32 val = 0b11u | (op_code << OP_CODE) | (rd << RD) | (funct3 << FUNCT3) | (rs1 << RS1) |
              ((shamt | funct_shift) << IMM_I);
    auto _inst = reinterpret_cast<Instruction *>(&val);
    CheckVisitor<void>{}.visit(_inst);
    check_all_dyn_cast<void>(_inst);
}

void check_invalid_funct7(usize op_code, usize rd, usize funct3, usize rs1, usize rs2, usize funct7) {
    u32 val = 0b11u | (op_code << OP_CODE) | (rd << RD) | (funct3 << FUNCT3) | (rs1 << RS1) | (rs2 << RS2) |
              (funct7 << FUNCT7);
    auto _inst = reinterpret_cast<Instruction *>(&val);
    CheckVisitor<void>{}.visit(_inst);
    check_all_dyn_cast<void>(_inst);
}

void check_fence_inst(usize rd, usize rs1, usize sw, usize sr, usize so, usize si, usize pw, usize pr, usize po,
                      usize pi, usize fm) {
    constexpr usize op_code = 0b00011;
    constexpr usize funct3 = 0b000;

    u32 val = 0b11u | (op_code << OP_CODE) | (rd << RD) | (funct3 << FUNCT3) | (rs1 << RS1) | (sw << 20u) |
              (sr << 21u) | (so << 22u) | (si << 23u) | (pw << 24u) | (pr << 25u) | (po << 26u) | (pi << 27u) |
              (fm << 28u);
    auto _inst = reinterpret_cast<Instruction *>(&val);
    CheckVisitor<FENCEInst>{}.visit(_inst);
    auto inst = check_all_dyn_cast<FENCEInst>(_inst);

    ASSERT_EQ(inst->get_op_code(), op_code);
    ASSERT_EQ(inst->get_rd(), rd);
    ASSERT_EQ(inst->get_funct3(), funct3);
    ASSERT_EQ(inst->get_rs1(), rs1);
    ASSERT_EQ(inst->get_sw(), sw);
    ASSERT_EQ(inst->get_sr(), sr);
    ASSERT_EQ(inst->get_so(), so);
    ASSERT_EQ(inst->get_si(), si);
    ASSERT_EQ(inst->get_pw(), pw);
    ASSERT_EQ(inst->get_pr(), pr);
    ASSERT_EQ(inst->get_po(), po);
    ASSERT_EQ(inst->get_pi(), pi);
    ASSERT_EQ(inst->get_fm(), fm);
}

template<typename T, usize op_code>
void check_u_type_inst(usize rd, typename xlen_32_trait::UXLenT imm_j) {
    u32 val = 0b11u | (op_code << OP_CODE) | (rd << RD) | (imm_j << IMM_U);
    auto _inst = reinterpret_cast<Instruction *>(&val);
    CheckVisitor<T>{}.visit(_inst);
    auto inst = check_all_dyn_cast<T>(_inst);

    ASSERT_EQ(inst->get_op_code(), op_code);
    ASSERT_EQ(inst->get_rd(), rd);
    ASSERT_EQ(inst->get_imm(), static_cast<typename xlen_32_trait::XLenT>(imm_j << IMM_U));
}

void check_jal_inst(usize rd, typename xlen_32_trait::UXLenT imm_u) {
    constexpr usize op_code = 0b11011;

    u32 val = 0b11u | (op_code << OP_CODE) | (rd << RD) | ((imm_u & 0b1111111111u) << 21u) |
              ((imm_u & 0b10000000000u) << 10u) | ((imm_u & 0b1111111100000000000u) << 1u) |
              ((imm_u & 0b10000000000000000000u) << 12u);
    auto _inst = reinterpret_cast<Instruction *>(&val);
    CheckVisitor<JALInst>{}.visit(_inst);
    auto inst = check_all_dyn_cast<JALInst>(_inst);

    ASSERT_EQ(inst->get_op_code(), op_code);
    ASSERT_EQ(inst->get_rd(), rd);
    ASSERT_EQ(inst->get_imm(), static_cast<typename xlen_32_trait::XLenT>(imm_u << 12u) >> 11u);
}

int main() {
    for (u32 rd = 0; rd < R_MAX; rd += R_INR) {
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_inst<LBInst, 0b00000, 0b000>(rd, rs1, imm_i);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_inst<LHInst, 0b00000, 0b001>(rd, rs1, imm_i);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_inst<LWInst, 0b00000, 0b010>(rd, rs1, imm_i);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_invalid(0b00000, rd, 0b011, rs1, imm_i);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_inst<LBUInst, 0b00000, 0b100>(rd, rs1, imm_i);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_inst<LHUInst, 0b00000, 0b101>(rd, rs1, imm_i);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_invalid(0b00000, rd, 0b110, rs1, imm_i);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_invalid(0b00000, rd, 0b111, rs1, imm_i);
    }

    check_invalid_op_code(0b00001);
    check_invalid_op_code(0b00010);

    for (u32 rd = 0; rd < R_MAX; rd += R_INR) {
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR) {
            check_fence_inst(rd, rs1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            check_fence_inst(rd, rs1, 1, 0, 0, 0, 0, 0, 0, 0, 0);
            check_fence_inst(rd, rs1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
            check_fence_inst(rd, rs1, 0, 0, 1, 0, 0, 0, 0, 0, 0);
            check_fence_inst(rd, rs1, 0, 0, 0, 1, 0, 0, 0, 0, 0);
            check_fence_inst(rd, rs1, 0, 0, 0, 0, 1, 0, 0, 0, 0);
            check_fence_inst(rd, rs1, 0, 0, 0, 0, 0, 1, 0, 0, 0);
            check_fence_inst(rd, rs1, 0, 0, 0, 0, 0, 0, 1, 0, 0);
            check_fence_inst(rd, rs1, 0, 0, 0, 0, 0, 0, 0, 1, 0);
            check_fence_inst(rd, rs1, 0, 0, 0, 0, 0, 0, 0, 0, 1);
            check_fence_inst(rd, rs1, 0, 0, 0, 0, 0, 0, 0, 0, 2);
            check_fence_inst(rd, rs1, 0, 0, 0, 0, 0, 0, 0, 0, 3);
        }

        for (usize funct3 = 0b011; funct3 < 0b1000; funct3 += 1)
            for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
                for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                    check_i_type_invalid(0b00011, rd, funct3, rs1, imm_i);
    }

    for (u32 rd = 0; rd < R_MAX; rd += R_INR) {
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_inst<ADDIInst, 0b00100, 0b000>(rd, rs1, imm_i);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR) {
                check_shift_imm_inst<SLLIInst, 0b00100, 0b001, 0b000000000000>(rd, rs1, get_bits<usize, 5, 0>(imm_i));

                for (u32 funct_shift = 32; funct_shift < 0b1000000000000; funct_shift += 32 * 32)
                    check_invalid_funct_shift(0b00100, rd, 0b001, rs1, get_bits<usize, 5, 0>(imm_i), funct_shift);
            }
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_inst<SLTIInst, 0b00100, 0b010>(rd, rs1, imm_i);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_inst<SLTIUInst, 0b00100, 0b011>(rd, rs1, imm_i);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_inst<XORIInst, 0b00100, 0b100>(rd, rs1, imm_i);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR) {
                check_shift_imm_inst<SRLIInst, 0b00100, 0b101, 0b000000000000>(rd, rs1, get_bits<usize, 5, 0>(imm_i));
                check_shift_imm_inst<SRAIInst, 0b00100, 0b101, 0b010000000000>(rd, rs1, get_bits<usize, 5, 0>(imm_i));

                for (u32 funct_shift = 32; funct_shift < 0b1000000000000; funct_shift += 32 * 32)
                    check_invalid_funct_shift(0b00100, rd, 0b101, rs1, get_bits<usize, 5, 0>(imm_i), funct_shift);
            }
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_inst<ORIInst, 0b00100, 0b110>(rd, rs1, imm_i);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_inst<ANDIInst, 0b00100, 0b111>(rd, rs1, imm_i);
    }

    for (u32 rd = 0; rd < R_MAX; rd += R_INR)
        for (u32 imm_u = 0; imm_u < IMM_U_MAX; imm_u += IMM_U_INR)
            check_u_type_inst<AUIPCInst, 0b00101>(rd, imm_u);

    check_invalid_op_code(0b00110);
    check_invalid_op_code(0b00111);

    for (u32 imm_s_1 = 0; imm_s_1 < IMM_S_1_MAX; imm_s_1 += IMM_S_1_INR) {
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR)
                for (u32 imm_s_2 = 0; imm_s_2 < IMM_S_2_MAX; imm_s_2 += IMM_S_2_INR)
                    check_s_type_inst<SBInst, 0b01000, 0b000>(imm_s_1, rs1, rs2, imm_s_2);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR)
                for (u32 imm_s_2 = 0; imm_s_2 < IMM_S_2_MAX; imm_s_2 += IMM_S_2_INR)
                    check_s_type_inst<SHInst, 0b01000, 0b001>(imm_s_1, rs1, rs2, imm_s_2);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR)
                for (u32 imm_s_2 = 0; imm_s_2 < IMM_S_2_MAX; imm_s_2 += IMM_S_2_INR)
                    check_s_type_inst<SWInst, 0b01000, 0b010>(imm_s_1, rs1, rs2, imm_s_2);
        for (u32 funct3 = 0b011; funct3 < 0b1000; funct3 += 1)
            for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
                for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR)
                    for (u32 imm_s_2 = 0; imm_s_2 < IMM_S_2_MAX; imm_s_2 += IMM_S_2_INR)
                        check_s_type_invalid(0b01000, imm_s_1, funct3, rs1, rs2, imm_s_2);
    }

    check_invalid_op_code(0b01001);
    check_invalid_op_code(0b01010);
    check_invalid_op_code(0b01011);

    for (u32 rd = 0; rd < R_MAX; rd += R_INR) {
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR) {
                check_r_type_inst<ADDInst, 0b01100, 0b000, 0b0000000>(rd, rs1, rs2);
                check_r_type_inst<SUBInst, 0b01100, 0b000, 0b0100000>(rd, rs1, rs2);

                for (u32 funct_7 = 1; funct_7 < 0b10000000; funct_7 += 32)
                    check_invalid_funct7(0b01100, rd, 0b000, rs1, rs2, funct_7);
            }
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR) {
                check_r_type_inst<SLLInst, 0b01100, 0b001, 0b0000000>(rd, rs1, rs2);

                for (u32 funct_7 = 1; funct_7 < 0b10000000; funct_7 += 32)
                    check_invalid_funct7(0b01100, rd, 0b001, rs1, rs2, funct_7);
            }
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR) {
                check_r_type_inst<SLTInst, 0b01100, 0b010, 0b0000000>(rd, rs1, rs2);

                for (u32 funct_7 = 1; funct_7 < 0b10000000; funct_7 += 32)
                    check_invalid_funct7(0b01100, rd, 0b010, rs1, rs2, funct_7);
            }
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR) {
                check_r_type_inst<SLTUInst, 0b01100, 0b011, 0b0000000>(rd, rs1, rs2);

                for (u32 funct_7 = 1; funct_7 < 0b10000000; funct_7 += 32)
                    check_invalid_funct7(0b01100, rd, 0b011, rs1, rs2, funct_7);
            }
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR) {
                check_r_type_inst<XORInst, 0b01100, 0b100, 0b0000000>(rd, rs1, rs2);

                for (u32 funct_7 = 1; funct_7 < 0b10000000; funct_7 += 32)
                    check_invalid_funct7(0b01100, rd, 0b100, rs1, rs2, funct_7);
            }
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR) {
                check_r_type_inst<SRLInst, 0b01100, 0b101, 0b0000000>(rd, rs1, rs2);
                check_r_type_inst<SRAInst, 0b01100, 0b101, 0b0100000>(rd, rs1, rs2);

                for (u32 funct_7 = 1; funct_7 < 0b10000000; funct_7 += 32)
                    check_invalid_funct7(0b01100, rd, 0b101, rs1, rs2, funct_7);
            }
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR) {
                check_r_type_inst<ORInst, 0b01100, 0b110, 0b0000000>(rd, rs1, rs2);

                for (u32 funct_7 = 1; funct_7 < 0b10000000; funct_7 += 32)
                    check_invalid_funct7(0b01100, rd, 0b110, rs1, rs2, funct_7);
            }
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR) {
                check_r_type_inst<ANDInst, 0b01100, 0b111, 0b0000000>(rd, rs1, rs2);

                for (u32 funct_7 = 1; funct_7 < 0b10000000; funct_7 += 32)
                    check_invalid_funct7(0b01100, rd, 0b111, rs1, rs2, funct_7);
            }
    }

    for (u32 rd = 0; rd < R_MAX; rd += R_INR)
        for (u32 imm_u = 0; imm_u < IMM_U_MAX; imm_u += IMM_U_INR)
            check_u_type_inst<LUIInst, 0b01101>(rd, imm_u);

    check_invalid_op_code(0b01110);
    check_invalid_op_code(0b01111);

    check_invalid_op_code(0b10000);
    check_invalid_op_code(0b10001);
    check_invalid_op_code(0b10010);
    check_invalid_op_code(0b10011);
    check_invalid_op_code(0b10100);
    check_invalid_op_code(0b10101);
    check_invalid_op_code(0b10110);
    check_invalid_op_code(0b10111);

    for (u32 imm_s_1 = 0; imm_s_1 < IMM_S_1_MAX; imm_s_1 += IMM_S_1_INR) {
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR)
                for (u32 imm_s_2 = 0; imm_s_2 < IMM_S_2_MAX; imm_s_2 += IMM_S_2_INR)
                    check_b_type_inst<BEQInst, 0b11000, 0b000>(imm_s_1, rs1, rs2, imm_s_2);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR)
                for (u32 imm_s_2 = 0; imm_s_2 < IMM_S_2_MAX; imm_s_2 += IMM_S_2_INR)
                    check_b_type_inst<BNEInst, 0b11000, 0b001>(imm_s_1, rs1, rs2, imm_s_2);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR)
                for (u32 imm_s_2 = 0; imm_s_2 < IMM_S_2_MAX; imm_s_2 += IMM_S_2_INR)
                    check_b_type_invalid(0b11000, imm_s_1, 0b010, rs1, rs2, imm_s_2);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR)
                for (u32 imm_s_2 = 0; imm_s_2 < IMM_S_2_MAX; imm_s_2 += IMM_S_2_INR)
                    check_b_type_invalid(0b11000, imm_s_1, 0b011, rs1, rs2, imm_s_2);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR)
                for (u32 imm_s_2 = 0; imm_s_2 < IMM_S_2_MAX; imm_s_2 += IMM_S_2_INR)
                    check_b_type_inst<BLTInst, 0b11000, 0b100>(imm_s_1, rs1, rs2, imm_s_2);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR)
                for (u32 imm_s_2 = 0; imm_s_2 < IMM_S_2_MAX; imm_s_2 += IMM_S_2_INR)
                    check_b_type_inst<BGEInst, 0b11000, 0b101>(imm_s_1, rs1, rs2, imm_s_2);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR)
                for (u32 imm_s_2 = 0; imm_s_2 < IMM_S_2_MAX; imm_s_2 += IMM_S_2_INR)
                    check_b_type_inst<BLTUInst, 0b11000, 0b110>(imm_s_1, rs1, rs2, imm_s_2);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 rs2 = 0; rs2 < R_MAX; rs2 += R_INR)
                for (u32 imm_s_2 = 0; imm_s_2 < IMM_S_2_MAX; imm_s_2 += IMM_S_2_INR)
                    check_b_type_inst<BGEUInst, 0b11000, 0b111>(imm_s_1, rs1, rs2, imm_s_2);
    }

    for (u32 rd = 0; rd < R_MAX; rd += R_INR) {
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_inst<JALRInst, 0b11001, 0b000>(rd, rs1, imm_i);
        for (u32 funct3 = 0b001; funct3 < 0b1000; funct3 += 1)
            for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
                for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                    check_i_type_invalid(0b11001, rd, funct3, rs1, imm_i);
    }

    check_invalid_op_code(0b11010);

    for (u32 rd = 0; rd < R_MAX; rd += R_INR)
        for (u32 imm_j = 0; imm_j < IMM_U_MAX; imm_j += IMM_U_INR)
            check_jal_inst(rd, imm_j);

    check_r_type_inst<ECALLInst, 0b11100, 0b000, 0b0000000>(0b00000, 0b00000, 0b00000);
    check_r_type_inst<EBREAKInst, 0b11100, 0b000, 0b0000000>(0b00000, 0b00000, 0b00001);
    check_r_type_inst<MRETInst, 0b11100, 0b000, 0b0011000>(0b00000, 0b00000, 0b00010);
    check_r_type_inst<WFIInst, 0b11100, 0b000, 0b0001000>(0b00000, 0b00000, 0b00101);

    for (u32 rd = 1; rd < R_MAX; rd += R_INR) {
        for (u32 rs1 = 1; rs1 < R_MAX; rs1 += R_INR) {
            for (u32 imm_i = 2; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_invalid(0b11100, rd, 0b000, rs1, imm_i);
        }
    }

    for (u32 rd = 0; rd < R_MAX; rd += R_INR) {
        for (u32 funct3 = 1; funct3 < 0b1000; funct3 += 1)
            for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR) {
                for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                    check_i_type_invalid(0b11100, rd, funct3, rs1, imm_i);
            }
    }

    check_invalid_op_code(0b11101);
    check_invalid_op_code(0b11110);
    check_invalid_op_code(0b11111);
}
