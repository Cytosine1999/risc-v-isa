#ifndef RISCV_ISA_DUMP_HPP
#define RISCV_ISA_DUMP_HPP


#include <ostream>

#include "utility.hpp"
#include "instruction/instruction_visitor.hpp"


namespace riscv_isa {
    class Dump : public InstructionVisitor<Dump, usize> {
    public:
        using RetT = usize;

    private:
        template<typename InstT>
        RetT dump_inst(InstT *inst) {
            stream << *inst;
            return InstT::INST_WIDTH;
        }

    public:
        std::ostream &stream;

        explicit Dump(std::ostream &stream) : stream{stream} {}

        RetT illegal_instruction(riscv_isa_unused Instruction *inst) {
            stream << "[unknown instruction]";
            return 0;
        }

        RetT visit_lui_inst(LUIInst *inst) { return dump_inst(inst); }

        RetT visit_auipc_inst(AUIPCInst *inst) { return dump_inst(inst); }

        RetT visit_jal_inst(JALInst *inst) { return dump_inst(inst); }

        RetT visit_jalr_inst(JALRInst *inst) { return dump_inst(inst); }

        RetT visit_beq_inst(BEQInst *inst) { return dump_inst(inst); }

        RetT visit_bne_inst(BNEInst *inst) { return dump_inst(inst); }

        RetT visit_blt_inst(BLTInst *inst) { return dump_inst(inst); }

        RetT visit_bge_inst(BGEInst *inst) { return dump_inst(inst); }

        RetT visit_bltu_inst(BLTUInst *inst) { return dump_inst(inst); }

        RetT visit_bgeu_inst(BGEUInst *inst) { return dump_inst(inst); }

        RetT visit_lb_inst(LBInst *inst) { return dump_inst(inst); }

        RetT visit_lh_inst(LHInst *inst) { return dump_inst(inst); }

        RetT visit_lw_inst(LWInst *inst) { return dump_inst(inst); }

        RetT visit_lbu_inst(LBUInst *inst) { return dump_inst(inst); }

        RetT visit_lhu_inst(LHUInst *inst) { return dump_inst(inst); }

        RetT visit_sb_inst(SBInst *inst) { return dump_inst(inst); }

        RetT visit_sh_inst(SHInst *inst) { return dump_inst(inst); }

        RetT visit_sw_inst(SWInst *inst) { return dump_inst(inst); }

        RetT visit_addi_inst(ADDIInst *inst) { return dump_inst(inst); }

        RetT visit_slti_inst(SLTIInst *inst) { return dump_inst(inst); }

        RetT visit_sltiu_inst(SLTIUInst *inst) { return dump_inst(inst); }

        RetT visit_xori_inst(XORIInst *inst) { return dump_inst(inst); }

        RetT visit_ori_inst(ORIInst *inst) { return dump_inst(inst); }

        RetT visit_andi_inst(ANDIInst *inst) { return dump_inst(inst); }

        RetT visit_slli_inst(SLLIInst *inst) { return dump_inst(inst); }

        RetT visit_srli_inst(SRLIInst *inst) { return dump_inst(inst); }

        RetT visit_srai_inst(SRAIInst *inst) { return dump_inst(inst); }

        RetT visit_add_inst(ADDInst *inst) { return dump_inst(inst); }

        RetT visit_sub_inst(SUBInst *inst) { return dump_inst(inst); }

        RetT visit_sll_inst(SLLInst *inst) { return dump_inst(inst); }

        RetT visit_slt_inst(SLTInst *inst) { return dump_inst(inst); }

        RetT visit_sltu_inst(SLTUInst *inst) { return dump_inst(inst); }

        RetT visit_xor_inst(XORInst *inst) { return dump_inst(inst); }

        RetT visit_srl_inst(SRLInst *inst) { return dump_inst(inst); }

        RetT visit_sra_inst(SRAInst *inst) { return dump_inst(inst); }

        RetT visit_or_inst(ORInst *inst) { return dump_inst(inst); }

        RetT visit_and_inst(ANDInst *inst) { return dump_inst(inst); }

        RetT visit_fence_inst(FENCEInst *inst) { return dump_inst(inst); }

        RetT visit_ecall_inst(ECALLInst *inst) { return dump_inst(inst); }

        RetT visit_ebreak_inst(EBREAKInst *inst) { return dump_inst(inst); }

        RetT visit_mul_inst(MULInst *inst) { return dump_inst(inst); }

        RetT visit_mulh_inst(MULHInst *inst) { return dump_inst(inst); }

        RetT visit_mulhsu_inst(MULHSUInst *inst) { return dump_inst(inst); }

        RetT visit_mulhu_inst(MULHUInst *inst) { return dump_inst(inst); }

        RetT visit_div_inst(DIVInst *inst) { return dump_inst(inst); }

        RetT visit_divu_inst(DIVUInst *inst) { return dump_inst(inst); }

        RetT visit_rem_inst(REMInst *inst) { return dump_inst(inst); }

        RetT visit_remu_inst(REMUInst *inst) { return dump_inst(inst); }

#if __RV_BIT_WIDTH__ == 64

        RetT visit_ld_inst(LDInst *inst) { return dump_inst(inst); }

        RetT visit_lwu_inst(LWUInst *inst) { return dump_inst(inst); }

        RetT visit_sd_inst(SDInst *inst) { return dump_inst(inst); }

        RetT visit_addiw_inst(ADDIWInst *inst) { return dump_inst(inst); }

        RetT visit_slliw_inst(SLLIWInst *inst) { return dump_inst(inst); }

        RetT visit_srliw_inst(SRLIWInst *inst) { return dump_inst(inst); }

        RetT visit_sraiw_inst(SRAIWInst *inst) { return dump_inst(inst); }

        RetT visit_addw_inst(ADDWInst *inst) { return dump_inst(inst); }

        RetT visit_subw_inst(SUBWInst *inst) { return dump_inst(inst); }

        RetT visit_sllw_inst(SLLWInst *inst) { return dump_inst(inst); }

        RetT visit_srlw_inst(SRLWInst *inst) { return dump_inst(inst); }

        RetT visit_sraw_inst(SRAWInst *inst) { return dump_inst(inst); }

#if defined(__RV_EXTENSION_M__)

        RetT visit_mulw_inst(MULWInst *inst) { return dump_inst(inst); }

        RetT visit_divw_inst(DIVWInst *inst) { return dump_inst(inst); }

        RetT visit_divuw_inst(DIVUWInst *inst) { return dump_inst(inst); }

        RetT visit_remw_inst(REMWInst *inst) { return dump_inst(inst); }

        RetT visit_remuw_inst(REMUWInst *inst) { return dump_inst(inst); }

#endif // defined(__RV_EXTENSION_M__)
#endif // __RV_BIT_WIDTH__ == 64
#if defined(__RV_EXTENSION_ZICSR__)

        RetT visit_csrrw_inst(CSRRWInst *inst) { return dump_inst(inst); }

        RetT visit_csrrs_inst(CSRRSInst *inst) { return dump_inst(inst); }

        RetT visit_csrrc_inst(CSRRCInst *inst) { return dump_inst(inst); }

        RetT visit_csrrwi_inst(CSRRWIInst *inst) { return dump_inst(inst); }

        RetT visit_csrrsi_inst(CSRRSIInst *inst) { return dump_inst(inst); }

        RetT visit_csrrci_inst(CSRRCIInst *inst) { return dump_inst(inst); }

#endif // defined(__RV_EXTENSION_ZICSR__)
#if defined(__RV_EXTENSION_N__)

        RetT visit_uret_inst(URETInst *inst) { return dump_inst(inst); }

#endif // defined(__RV_EXTENSION_N__)
#if defined(__RV_SUPERVISOR_MODE__)

        RetT visit_sret_inst(SRETInst *inst) { return dump_inst(inst); }

#endif // defined(__RV_SUPERVISOR_MODE__)

        RetT visit_mret_inst(MRETInst *inst) { return dump_inst(inst); }

        RetT visit_wfi_inst(WFIInst *inst) { return dump_inst(inst); }

#if defined(__RV_SUPERVISOR_MODE__)

        RetT visit_sfencevma_inst(SFENCEVAMInst *inst) { return dump_inst(inst); }

#endif // defined(__RV_SUPERVISOR_MODE__)
#if defined(__RV_HYPERVISOR_MODE__)

        RetT visit_hfencevvma_inst(HFENCEVVMAInst *inst) { return dump_inst(inst); }

        RetT visit_hfencegvma_inst(HFENCEGVMAInst *inst) { return dump_inst(inst); }

#endif // defined(__RV_HYPERVISOR_MODE__)
    };

    std::ostream &operator<<(std::ostream &stream, const Instruction &inst) {
        Dump dump{stream};
        dump.visit(const_cast<Instruction *>(&inst));
        return stream;
    }
}


#endif //RISCV_ISA_DUMP_HPP
