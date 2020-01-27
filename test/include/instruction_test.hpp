#ifndef RISC_V_ISA_INSTRUCTION_TEST_HPP
#define RISC_V_ISA_INSTRUCTION_TEST_HPP


#include "utility.hpp"
#include "instruction/instruction.hpp"
#include "instruction/rv32i.hpp"
#include "instruction/instruction_visitor.hpp"


namespace risc_v_isa {
    template<typename InputT>
    class CheckVisitor : public InstructionVisitor<CheckVisitor<InputT>> {
    public:
        void illegal_instruction(risc_v_isa_unused Instruction *inst) {
            ASSERT((std::is_same<void, InputT>::value));
        }

        void visit_lui_inst(risc_v_isa_unused LUIInst *inst) {
            ASSERT((std::is_same<LUIInst, InputT>::value));
        }

        void visit_auipc_inst(risc_v_isa_unused AUIPCInst *inst) {
            ASSERT((std::is_same<AUIPCInst, InputT>::value));
        }

        void visit_jal_inst(risc_v_isa_unused JALInst *inst) {
            ASSERT((std::is_same<JALInst, InputT>::value));
        }

        void visit_jalr_inst(risc_v_isa_unused JALRInst *inst) {
            ASSERT((std::is_same<JALRInst, InputT>::value));
        }

        void visit_beq_inst(risc_v_isa_unused BEQInst *inst) {
            ASSERT((std::is_same<BEQInst, InputT>::value));
        }

        void visit_bne_inst(risc_v_isa_unused BNEInst *inst) {
            ASSERT((std::is_same<BNEInst, InputT>::value));
        }

        void visit_blt_inst(risc_v_isa_unused BLTInst *inst) {
            ASSERT((std::is_same<BLTInst, InputT>::value));
        }

        void visit_bge_inst(risc_v_isa_unused BGEInst *inst) {
            ASSERT((std::is_same<BGEInst, InputT>::value));
        }

        void visit_bltu_inst(risc_v_isa_unused BLTUInst *inst) {
            ASSERT((std::is_same<BLTUInst, InputT>::value));
        }

        void visit_bgeu_inst(risc_v_isa_unused BGEUInst *inst) {
            ASSERT((std::is_same<BGEUInst, InputT>::value));
        }

        void visit_lb_inst(risc_v_isa_unused LBInst *inst) {
            ASSERT((std::is_same<LBInst, InputT>::value));
        }

        void visit_lh_inst(risc_v_isa_unused LHInst *inst) {
            ASSERT((std::is_same<LHInst, InputT>::value));
        }

        void visit_lw_inst(risc_v_isa_unused LWInst *inst) {
            ASSERT((std::is_same<LWInst, InputT>::value));
        }

        void visit_lbu_inst(risc_v_isa_unused LBUInst *inst) {
            ASSERT((std::is_same<LBUInst, InputT>::value));
        }

        void visit_lhu_inst(risc_v_isa_unused LHUInst *inst) {
            ASSERT((std::is_same<LHUInst, InputT>::value));
        }

        void visit_sb_inst(risc_v_isa_unused SBInst *inst) {
            ASSERT((std::is_same<SBInst, InputT>::value));
        }

        void visit_sh_inst(risc_v_isa_unused SHInst *inst) {
            ASSERT((std::is_same<SHInst, InputT>::value));
        }

        void visit_sw_inst(risc_v_isa_unused SWInst *inst) {
            ASSERT((std::is_same<SWInst, InputT>::value));
        }

        void visit_addi_inst(risc_v_isa_unused ADDIInst *inst) {
            ASSERT((std::is_same<ADDIInst, InputT>::value));
        }

        void visit_slti_inst(risc_v_isa_unused SLTIInst *inst) {
            ASSERT((std::is_same<SLTIInst, InputT>::value));
        }

        void visit_sltiu_inst(risc_v_isa_unused SLTIUInst *inst) {
            ASSERT((std::is_same<SLTIUInst, InputT>::value));
        }

        void visit_xori_inst(risc_v_isa_unused XORIInst *inst) {
            ASSERT((std::is_same<XORIInst, InputT>::value));
        }

        void visit_ori_inst(risc_v_isa_unused ORIInst *inst) {
            ASSERT((std::is_same<ORIInst, InputT>::value));
        }

        void visit_andi_inst(risc_v_isa_unused ANDIInst *inst) {
            ASSERT((std::is_same<ANDIInst, InputT>::value));
        }

        void visit_slli_inst(risc_v_isa_unused SLLIInst *inst) {
            ASSERT((std::is_same<SLLIInst, InputT>::value));
        }

        void visit_srli_inst(risc_v_isa_unused SRLIInst *inst) {
            ASSERT((std::is_same<SRLIInst, InputT>::value));
        }

        void visit_srai_inst(risc_v_isa_unused SRAIInst *inst) {
            ASSERT((std::is_same<SRAIInst, InputT>::value));
        }

        void visit_add_inst(risc_v_isa_unused ADDInst *inst) {
            ASSERT((std::is_same<ADDInst, InputT>::value));
        }

        void visit_sub_inst(risc_v_isa_unused SUBInst *inst) {
            ASSERT((std::is_same<SUBInst, InputT>::value));
        }

        void visit_sll_inst(risc_v_isa_unused SLLInst *inst) {
            ASSERT((std::is_same<SLLInst, InputT>::value));
        }

        void visit_slt_inst(risc_v_isa_unused SLTInst *inst) {
            ASSERT((std::is_same<SLTInst, InputT>::value));
        }

        void visit_sltu_inst(risc_v_isa_unused SLTUInst *inst) {
            ASSERT((std::is_same<SLTUInst, InputT>::value));
        }

        void visit_xor_inst(risc_v_isa_unused XORInst *inst) {
            ASSERT((std::is_same<XORInst, InputT>::value));
        }

        void visit_srl_inst(risc_v_isa_unused SRLInst *inst) {
            ASSERT((std::is_same<SRLInst, InputT>::value));
        }

        void visit_sra_inst(risc_v_isa_unused SRAInst *inst) {
            ASSERT((std::is_same<SRAInst, InputT>::value));
        }

        void visit_or_inst(risc_v_isa_unused ORInst *inst) {
            ASSERT((std::is_same<ORInst, InputT>::value));
        }

        void visit_and_inst(risc_v_isa_unused ANDInst *inst) {
            ASSERT((std::is_same<ANDInst, InputT>::value));
        }

        void visit_fence_inst(risc_v_isa_unused FENCEInst *inst) {
            ASSERT((std::is_same<FENCEInst, InputT>::value));
        }

        void visit_ecall_inst(risc_v_isa_unused ECALLInst *inst) {
            ASSERT((std::is_same<ECALLInst, InputT>::value));
        }

        void visit_ebreak_inst(risc_v_isa_unused EBREAKInst *inst) {
            ASSERT((std::is_same<EBREAKInst, InputT>::value));
        }
    };

    template<typename T, typename U>
    void check_dyn_cast(Instruction *inst) {
        if constexpr (std::is_same<T, U>::value)
            ASSERT_NE(dyn_cast<U>(inst), static_cast<U *>(nullptr));
        else
            ASSERT_EQ(dyn_cast<U>(inst), static_cast<U *>(nullptr));
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

}


#endif //RISC_V_ISA_INSTRUCTION_TEST_HPP
