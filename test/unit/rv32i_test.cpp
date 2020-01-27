#include "test.hpp"
#include "instruction_test.hpp"
#include "utility.hpp"
#include "instruction/instruction.hpp"
#include "instruction/rv32i.hpp"
#include "instruction/instruction_visitor.hpp"

using namespace risc_v_isa;


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

void check_s_type_invalid_funct3(usize op_code, usize imm_s_1, usize funct3, usize rs1, usize rs2, usize imm_s_2) {
    u32 val = 0b11u | (op_code << OP_CODE) | (imm_s_1 << IMM_S_1) | (funct3 << FUNCT3) | (rs1 << RS1) | (rs2 << RS2) |
              (imm_s_2 << IMM_S_2);
    auto _inst = reinterpret_cast<Instruction *>(&val);
    CheckVisitor<void>{}.visit(_inst);
    check_all_dyn_cast<void>(_inst);
}

template<typename T, usize op_code, usize funct3, usize funct_shift>
void check_shift_imm_inst(usize rd, usize rs1, usize shift_amount) {
    u32 val = 0b11u | (op_code << OP_CODE) | (rd << RD) | (funct3 << FUNCT3) | (rs1 << RS1) |
              ((shift_amount | funct_shift) << IMM_I);
    auto _inst = reinterpret_cast<Instruction *>(&val);
    CheckVisitor<T>{}.visit(_inst);
    auto inst = check_all_dyn_cast<T>(_inst);

    ASSERT_EQ(inst->get_op_code(), op_code);
    ASSERT_EQ(inst->get_rd(), rd);
    ASSERT_EQ(inst->get_funct3(), funct3);
    ASSERT_EQ(inst->get_rs1(), rs1);
    ASSERT_EQ(inst->get_shift_amount(), shift_amount);
    ASSERT_EQ(inst->get_funct_shift(), funct_shift);
}

void
check_invalid_funct_shift(usize op_code, usize rd, usize funct3, usize rs1, usize shift_amount, usize funct_shift) {
    u32 val = 0b11u | (op_code << OP_CODE) | (rd << RD) | (funct3 << FUNCT3) | (rs1 << RS1) |
              ((shift_amount | funct_shift) << IMM_I);
    auto _inst = reinterpret_cast<Instruction *>(&val);
    CheckVisitor<void>{}.visit(_inst);
    check_all_dyn_cast<void>(_inst);
}

void check_i_type_invalid_funct3(usize op_code, usize rd, usize funct3, usize rs1, usize imm_i) {
    u32 val = 0b11u | (op_code << OP_CODE) | (rd << RD) | (funct3 << FUNCT3) | (rs1 << RS1) |
              (imm_i << IMM_I);
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
void check_j_type_inst(usize rd, UXLenT imm_u) {
    u32 val = 0b11u | (op_code << OP_CODE) | (rd << RD) | (imm_u << IMM_U);
    auto _inst = reinterpret_cast<Instruction *>(&val);
    CheckVisitor<T>{}.visit(_inst);
    auto inst = check_all_dyn_cast<T>(_inst);

    ASSERT_EQ(inst->get_op_code(), op_code);
    ASSERT_EQ(inst->get_rd(), rd);
    ASSERT_EQ(inst->get_imm(), static_cast<XLenT>(imm_u << IMM_U));
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
                check_i_type_invalid_funct3(0b00000, rd, 0b011, rs1, imm_i);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_inst<LBUInst, 0b00000, 0b100>(rd, rs1, imm_i);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_inst<LHUInst, 0b00000, 0b101>(rd, rs1, imm_i);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_invalid_funct3(0b00000, rd, 0b110, rs1, imm_i);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_invalid_funct3(0b00000, rd, 0b111, rs1, imm_i);
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
                    check_i_type_invalid_funct3(0b00011, rd, funct3, rs1, imm_i);
    }

    for (u32 rd = 0; rd < R_MAX; rd += R_INR) {
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_inst<ADDIInst, 0b00100, 0b000>(rd, rs1, imm_i);
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR) {
                check_shift_imm_inst<SLLIInst, 0b00100, 0b001, 0b000000000000>(rd, rs1, get_slice<usize, 5, 0>(imm_i));

                for (u32 funct_shift = 32; funct_shift < 0b1000000000000; funct_shift += 32 * 32)
                    check_invalid_funct_shift(0b00100, rd, 0b001, rs1, get_slice<usize, 5, 0>(imm_i), funct_shift);
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
                check_shift_imm_inst<SRLIInst, 0b00100, 0b101, 0b000000000000>(rd, rs1, get_slice<usize, 5, 0>(imm_i));
                check_shift_imm_inst<SRAIInst, 0b00100, 0b101, 0b010000000000>(rd, rs1, get_slice<usize, 5, 0>(imm_i));

                for (u32 funct_shift = 32; funct_shift < 0b1000000000000; funct_shift += 32 * 32)
                    check_invalid_funct_shift(0b00100, rd, 0b101, rs1, get_slice<usize, 5, 0>(imm_i), funct_shift);
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
            check_j_type_inst<AUIPCInst, 0b00101>(rd, imm_u);

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
                        check_s_type_invalid_funct3(0b01000, imm_s_1, funct3, rs1, rs2, imm_s_2);
    }

    for (u32 rd = 0; rd < R_MAX; rd += R_INR)
        for (u32 imm_u = 0; imm_u < IMM_U_MAX; imm_u += IMM_U_INR)
            check_j_type_inst<LUIInst, 0b01101>(rd, imm_u);

    check_invalid_op_code(0b01001);
    check_invalid_op_code(0b01010);
    check_invalid_op_code(0b01011);

    // todo

    for (u32 rd = 0; rd < R_MAX; rd += R_INR) {
        for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
            for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                check_i_type_inst<JALRInst, 0b11001, 0b000>(rd, rs1, imm_i);
        for (u32 funct3 = 0b001; funct3 < 0b1000; funct3 += 1)
            for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR)
                for (u32 imm_i = 0; imm_i < IMM_I_MAX; imm_i += IMM_I_INR)
                    check_i_type_invalid_funct3(0b11001, rd, funct3, rs1, imm_i);
    }

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

    // todo

    check_invalid_op_code(0b11010);

    // todo

    check_invalid_op_code(0b11101);
    check_invalid_op_code(0b11110);
    check_invalid_op_code(0b11111);
}
