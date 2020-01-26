#include "test.hpp"
#include "instruction/rv32i.hpp"
#include "instruction/instruction_visitor.hpp"

using namespace risc_v_isa;


constexpr usize R_INR = 13;
constexpr usize R_MAX = 32;

constexpr usize OP_CODE = 2;
constexpr usize RD = 7;
constexpr usize FUNCT3 = 12;
constexpr usize RS1 = 15;


template<typename T>
class CheckVisitor : public InstructionVisitor<CheckVisitor<T>> {
public:
    void illegal_instruction(risc_v_isa_unused Instruction *inst) {
        using IsType = std::is_same<void, T>;
        ASSERT(IsType::value);
    }

    void visit_lui_inst(risc_v_isa_unused LUIInst *inst) {
        using IsType = std::is_same<LUIInst, T>;
        ASSERT(IsType::value);
    }

    void visit_auipc_inst(risc_v_isa_unused AUIPCInst *inst) {
        using IsType = std::is_same<AUIPCInst, T>;
        ASSERT(IsType::value);
    }

    void visit_jal_inst(risc_v_isa_unused JALInst *inst) {
        using IsType = std::is_same<JALInst, T>;
        ASSERT(IsType::value);
    }

    void visit_jalr_inst(risc_v_isa_unused JALRInst *inst) {
        using IsType = std::is_same<JALRInst, T>;
        ASSERT(IsType::value);
    }

    void visit_beq_inst(risc_v_isa_unused BEQInst *inst) {
        using IsType = std::is_same<BEQInst, T>;
        ASSERT(IsType::value);
    }

    void visit_bne_inst(risc_v_isa_unused BNEInst *inst) {
        using IsType = std::is_same<BNEInst, T>;
        ASSERT(IsType::value);
    }

    void visit_blt_inst(risc_v_isa_unused BLTInst *inst) {
        using IsType = std::is_same<BLTInst, T>;
        ASSERT(IsType::value);
    }

    void visit_bge_inst(risc_v_isa_unused BGEInst *inst) {
        using IsType = std::is_same<BGEInst, T>;
        ASSERT(IsType::value);
    }

    void visit_bltu_inst(risc_v_isa_unused BLTUInst *inst) {
        using IsType = std::is_same<BLTUInst, T>;
        ASSERT(IsType::value);
    }

    void visit_bgeu_inst(risc_v_isa_unused BGEUInst *inst) {
        using IsType = std::is_same<BGEUInst, T>;
        ASSERT(IsType::value);
    }

    void visit_lb_inst(risc_v_isa_unused LBInst *inst) {
        using IsType = std::is_same<LBInst, T>;
        ASSERT(IsType::value);
    }

    void visit_lh_inst(risc_v_isa_unused LHInst *inst) {
        using IsType = std::is_same<LHInst, T>;
        ASSERT(IsType::value);
    }

    void visit_lw_inst(risc_v_isa_unused LWInst *inst) {
        using IsType = std::is_same<LWInst, T>;
        ASSERT(IsType::value);
    }

    void visit_lbu_inst(risc_v_isa_unused LBUInst *inst) {
        using IsType = std::is_same<LBUInst, T>;
        ASSERT(IsType::value);
    }

    void visit_lhu_inst(risc_v_isa_unused LHUInst *inst) {
        using IsType = std::is_same<LHUInst, T>;
        ASSERT(IsType::value);
    }

    void visit_sb_inst(risc_v_isa_unused SBInst *inst) {
        using IsType = std::is_same<SBInst, T>;
        ASSERT(IsType::value);
    }

    void visit_sh_inst(risc_v_isa_unused SHInst *inst) {
        using IsType = std::is_same<SHInst, T>;
        ASSERT(IsType::value);
    }

    void visit_sw_inst(risc_v_isa_unused SWInst *inst) {
        using IsType = std::is_same<SWInst, T>;
        ASSERT(IsType::value);
    }

    void visit_addi_inst(risc_v_isa_unused ADDIInst *inst) {
        using IsType = std::is_same<ADDIInst, T>;
        ASSERT(IsType::value);
    }

    void visit_slti_inst(risc_v_isa_unused SLTIInst *inst) {
        using IsType = std::is_same<SLTIInst, T>;
        ASSERT(IsType::value);
    }

    void visit_sltiu_inst(risc_v_isa_unused SLTIUInst *inst) {
        using IsType = std::is_same<SLTIUInst, T>;
        ASSERT(IsType::value);
    }

    void visit_xori_inst(risc_v_isa_unused XORIInst *inst) {
        using IsType = std::is_same<XORIInst, T>;
        ASSERT(IsType::value);
    }

    void visit_ori_inst(risc_v_isa_unused ORIInst *inst) {
        using IsType = std::is_same<ORIInst, T>;
        ASSERT(IsType::value);
    }

    void visit_andi_inst(risc_v_isa_unused ANDIInst *inst) {
        using IsType = std::is_same<ANDIInst, T>;
        ASSERT(IsType::value);
    }

    void visit_slli_inst(risc_v_isa_unused SLLIInst *inst) {
        using IsType = std::is_same<SLLIInst, T>;
        ASSERT(IsType::value);
    }

    void visit_srli_inst(risc_v_isa_unused SRLIInst *inst) {
        using IsType = std::is_same<SRLIInst, T>;
        ASSERT(IsType::value);
    }

    void visit_srai_inst(risc_v_isa_unused SRAIInst *inst) {
        using IsType = std::is_same<SRAIInst, T>;
        ASSERT(IsType::value);
    }

    void visit_add_inst(risc_v_isa_unused ADDInst *inst) {
        using IsType = std::is_same<ADDInst, T>;
        ASSERT(IsType::value);
    }

    void visit_sub_inst(risc_v_isa_unused SUBInst *inst) {
        using IsType = std::is_same<SUBInst, T>;
        ASSERT(IsType::value);
    }

    void visit_sll_inst(risc_v_isa_unused SLLInst *inst) {
        using IsType = std::is_same<SLLInst, T>;
        ASSERT(IsType::value);
    }

    void visit_slt_inst(risc_v_isa_unused SLTInst *inst) {
        using IsType = std::is_same<SLTInst, T>;
        ASSERT(IsType::value);
    }

    void visit_sltu_inst(risc_v_isa_unused SLTUInst *inst) {
        using IsType = std::is_same<SLTUInst, T>;
        ASSERT(IsType::value);
    }

    void visit_xor_inst(risc_v_isa_unused XORInst *inst) {
        using IsType = std::is_same<XORInst, T>;
        ASSERT(IsType::value);
    }

    void visit_srl_inst(risc_v_isa_unused SRLInst *inst) {
        using IsType = std::is_same<SRLInst, T>;
        ASSERT(IsType::value);
    }

    void visit_sra_inst(risc_v_isa_unused SRAInst *inst) {
        using IsType = std::is_same<SRAInst, T>;
        ASSERT(IsType::value);
    }

    void visit_or_inst(risc_v_isa_unused ORInst *inst) {
        using IsType = std::is_same<ORInst, T>;
        ASSERT(IsType::value);
    }

    void visit_and_inst(risc_v_isa_unused ANDInst *inst) {
        using IsType = std::is_same<ANDInst, T>;
        ASSERT(IsType::value);
    }

    void visit_fence_inst(risc_v_isa_unused FENCEInst *inst) {
        using IsType = std::is_same<FENCEInst, T>;
        ASSERT(IsType::value);
    }

    void visit_ecall_inst(risc_v_isa_unused ECALLInst *inst) {
        using IsType = std::is_same<ECALLInst, T>;
        ASSERT(IsType::value);
    }

    void visit_ebreak_inst(risc_v_isa_unused EBREAKInst *inst) {
        using IsType = std::is_same<EBREAKInst, T>;
        ASSERT(IsType::value);
    }
};

template<typename T, typename U>
void check_dyn_cast(Instruction *inst) {
    if constexpr (std::is_same<T, U>::value) ASSERT_NE(dyn_cast<U>(inst), static_cast<U *>(nullptr));
    else ASSERT_EQ(dyn_cast<U>(inst), static_cast<U *>(nullptr));
}

template<typename T>
T *check_all_dyn_cast(Instruction *inst) {
    check_dyn_cast<T, LUIInst>(inst);
    check_dyn_cast<T, AUIPCInst>(inst);
    check_dyn_cast<T, JALInst>(inst);
    check_dyn_cast<T, JALRInst>(inst);
    check_dyn_cast<T, BEQInst>(inst);
    check_dyn_cast<T, BNEInst>(inst);
    check_dyn_cast<T, BLTInst>(inst);
    check_dyn_cast<T, BGEInst>(inst);
    check_dyn_cast<T, BLTUInst>(inst);
    check_dyn_cast<T, BGEUInst>(inst);
    check_dyn_cast<T, LBInst>(inst);
    check_dyn_cast<T, LHInst>(inst);
    check_dyn_cast<T, LWInst>(inst);
    check_dyn_cast<T, LBUInst>(inst);
    check_dyn_cast<T, LHUInst>(inst);
    check_dyn_cast<T, SBInst>(inst);
    check_dyn_cast<T, SHInst>(inst);
    check_dyn_cast<T, SWInst>(inst);
    check_dyn_cast<T, ADDIInst>(inst);
    check_dyn_cast<T, SLTIInst>(inst);
    check_dyn_cast<T, SLTIUInst>(inst);
    check_dyn_cast<T, XORIInst>(inst);
    check_dyn_cast<T, ORIInst>(inst);
    check_dyn_cast<T, ANDIInst>(inst);
    check_dyn_cast<T, SLLIInst>(inst);
    check_dyn_cast<T, SRLIInst>(inst);
    check_dyn_cast<T, SRAIInst>(inst);
    check_dyn_cast<T, ADDInst>(inst);
    check_dyn_cast<T, SUBInst>(inst);
    check_dyn_cast<T, SLLInst>(inst);
    check_dyn_cast<T, SLTInst>(inst);
    check_dyn_cast<T, SLTUInst>(inst);
    check_dyn_cast<T, XORInst>(inst);
    check_dyn_cast<T, SRLInst>(inst);
    check_dyn_cast<T, SRAInst>(inst);
    check_dyn_cast<T, ORInst>(inst);
    check_dyn_cast<T, ANDInst>(inst);
    check_dyn_cast<T, FENCEInst>(inst);
    check_dyn_cast<T, ECALLInst>(inst);
    check_dyn_cast<T, EBREAKInst>(inst);

    if constexpr (std::is_void<T>::value) return nullptr;
    else return dyn_cast<T>(inst);
}


int main() {
    for (u32 rs1 = 0; rs1 < R_MAX; rs1 += R_INR) {
        constexpr usize op_code = 0b00000;

        for (u32 rd = 0; rd < R_MAX; rd += R_INR) {
            constexpr usize funct3 = 0b000;

            u32 val = 0b11u + (op_code << OP_CODE) + (rd << RD) + (funct3 << FUNCT3) + (rs1 << RS1);
            auto _inst = reinterpret_cast<Instruction *>(&val);
            CheckVisitor<LBInst>{}.visit(_inst);
            auto inst = check_all_dyn_cast<LBInst>(_inst);

            ASSERT_EQ(inst->get_op_code(), op_code);
            ASSERT_EQ(inst->get_funct3(), funct3);
            ASSERT_EQ(inst->get_rd(), rd);
            ASSERT_EQ(inst->get_rs1(), rs1);
            ASSERT_EQ(inst->get_imm(), 0);
        }

        for (u32 rd = 0; rd < R_MAX; rd += R_INR) {
            constexpr usize funct3 = 0b001;

            u32 val = 0b11u + (op_code << OP_CODE) + (rd << RD) + (funct3 << FUNCT3) + (rs1 << RS1);
            auto _inst = reinterpret_cast<Instruction *>(&val);
            CheckVisitor<LHInst>{}.visit(_inst);
            auto inst = check_all_dyn_cast<LHInst>(_inst);

            ASSERT_EQ(inst->get_op_code(), op_code);
            ASSERT_EQ(inst->get_funct3(), funct3);
            ASSERT_EQ(inst->get_rd(), rd);
            ASSERT_EQ(inst->get_rs1(), rs1);
            ASSERT_EQ(inst->get_imm(), 0);
        }

        for (u32 rd = 0; rd < R_MAX; rd += R_INR) {
            constexpr usize funct3 = 0b010;

            u32 val = 0b11u + (op_code << OP_CODE) + (rd << RD) + (funct3 << FUNCT3) + (rs1 << RS1);
            auto _inst = reinterpret_cast<Instruction *>(&val);
            CheckVisitor<LWInst>{}.visit(_inst);
            auto inst = check_all_dyn_cast<LWInst>(_inst);

            ASSERT_EQ(inst->get_op_code(), op_code);
            ASSERT_EQ(inst->get_funct3(), funct3);
            ASSERT_EQ(inst->get_rd(), rd);
            ASSERT_EQ(inst->get_rs1(), rs1);
            ASSERT_EQ(inst->get_imm(), 0);
        }

        for (u32 rd = 0; rd < R_MAX; rd += R_INR) {
            constexpr usize funct3 = 0b011;

            u32 val = 0b11u + (op_code << OP_CODE) + (rd << RD) + (funct3 << FUNCT3) + (rs1 << RS1);
            auto _inst = reinterpret_cast<Instruction *>(&val);
            CheckVisitor<void>{}.visit(_inst);
            check_all_dyn_cast<void>(_inst);
        }

        for (u32 rd = 0; rd < R_MAX; rd += R_INR) {
            constexpr usize funct3 = 0b100;

            u32 val = 0b11u + (op_code << OP_CODE) + (rd << RD) + (funct3 << FUNCT3) + (rs1 << RS1);
            auto _inst = reinterpret_cast<Instruction *>(&val);
            CheckVisitor<LBUInst>{}.visit(_inst);
            auto inst = check_all_dyn_cast<LBUInst>(_inst);

            ASSERT_EQ(inst->get_op_code(), op_code);
            ASSERT_EQ(inst->get_funct3(), funct3);
            ASSERT_EQ(inst->get_rd(), rd);
            ASSERT_EQ(inst->get_rs1(), rs1);
            ASSERT_EQ(inst->get_imm(), 0);
        }

        for (u32 rd = 0; rd < R_MAX; rd += R_INR) {
            constexpr usize funct3 = 0b101;

            u32 val = 0b11u + (op_code << OP_CODE) + (rd << RD) + (funct3 << FUNCT3) + (rs1 << RS1);
            auto _inst = reinterpret_cast<Instruction *>(&val);
            CheckVisitor<LHUInst>{}.visit(_inst);
            auto inst = check_all_dyn_cast<LHUInst>(_inst);

            ASSERT_EQ(inst->get_op_code(), op_code);
            ASSERT_EQ(inst->get_funct3(), funct3);
            ASSERT_EQ(inst->get_rd(), rd);
            ASSERT_EQ(inst->get_rs1(), rs1);
            ASSERT_EQ(inst->get_imm(), 0);
        }

        for (u32 rd = 0; rd < R_MAX; rd += R_INR) {
            constexpr usize funct3 = 0b110;

            u32 val = 0b11u + (op_code << OP_CODE) + (rd << RD) + (funct3 << FUNCT3) + (rs1 << RS1);
            auto _inst = reinterpret_cast<Instruction *>(&val);
            CheckVisitor<void>{}.visit(_inst);
            check_all_dyn_cast<void>(_inst);
        }

        for (u32 rd = 0; rd < R_MAX; rd += R_INR) {
            constexpr usize funct3 = 0b111;

            u32 val = 0b11u + (op_code << OP_CODE) + (rd << RD) + (funct3 << FUNCT3) + (rs1 << RS1);
            auto _inst = reinterpret_cast<Instruction *>(&val);
            CheckVisitor<void>{}.visit(_inst);
            check_all_dyn_cast<void>(_inst);
        }
    }
}
