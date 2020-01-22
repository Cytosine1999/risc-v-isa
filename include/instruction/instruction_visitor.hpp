#ifndef RISC_V_ISA_INSTRUCTION_VISITOR_HPP
#define RISC_V_ISA_INSTRUCTION_VISITOR_HPP


#include "utility.hpp"
#include "instruction.hpp"
#include "rv32i.hpp"
#include "rv64i.hpp"
#include "zifencei.hpp"
#include "zicsr.hpp"
#include "rv32m.hpp"
#include "rv64m.hpp"


namespace risc_v_isa {
    template<typename SubT, typename _RetT = void>
    class InstructionVisitor {
    private:
        SubT *sub_type() { return static_cast<SubT *>(this); }

    public:
        using RetT = _RetT;

        RetT visit(Instruction *inst) {
            static_assert(std::is_base_of<InstructionVisitor, SubT>::value);

            usize leading_16 = *reinterpret_cast<u16 *>(inst);

            while (true) {
#if defined(__RV_EXTENSION_C__)
                if ((val & BITS_MASK<u16, 2, 0>) != BITS_MASK<u16, 2, 0>) {
                    return visit_16(reinterpret_cast<Instruction16 *>(inst));
                }
#endif
#if defined(__RV_32_BIT__) || defined(__RV_64_BIT__)
                if ((leading_16 & BITS_MASK<u16, 5, 2>) != BITS_MASK<u16, 5, 2>) {
                    return visit_32(reinterpret_cast<Instruction32 *>(inst));
                }
#endif // defined(__RV_32_BIT__) || defined(__RV_64_BIT__)
                return sub_type()->illegal_instruction(inst);
            }
        }

#if defined(__RV_EXTENSION_C__)

        RetT visit_16(Instruction16 *inst) {
            return;
        }

#endif // defined(__RV_EXTENSION_C__)
#if defined(__RV_32_BIT__) || defined(__RV_64_BIT__)

        RetT visit_32(Instruction32 *inst) {
            switch (inst->get_op_code()) {
                case LUIInst::OP_CODE:
                    return sub_type()->visit_lui_inst(reinterpret_cast<LUIInst *>(inst));
                case AUIPCInst::OP_CODE:
                    return sub_type()->visit_auipc_inst(reinterpret_cast<AUIPCInst *>(inst));
                case JALInst::OP_CODE:
                    return sub_type()->visit_jal_inst(reinterpret_cast<JALInst *>(inst));
                case JALRInst::OP_CODE:
                    if (reinterpret_cast<JALRInst *>(inst)->get_funct3() != JALRInst::FUNCT3)
                        return sub_type()->illegal_instruction(inst);
                    return sub_type()->visit_jalr_inst(reinterpret_cast<JALRInst *>(inst));
                case InstructionBranchSet::OP_CODE:
                    return visit_branch_set(reinterpret_cast<InstructionBranchSet *>(inst));
                case InstructionLoadSet::OP_CODE:
                    return visit_load_set(reinterpret_cast<InstructionLoadSet *>(inst));
                case InstructionStoreSet::OP_CODE:
                    return visit_store_set(reinterpret_cast<InstructionStoreSet *>(inst));
                case InstructionArithImmSet::OP_CODE:
                    return visit_arith_imm_set(reinterpret_cast<InstructionArithImmSet *>(inst));
                case InstructionArithRegSet::OP_CODE:
                    return visit_arith_reg_set(reinterpret_cast<InstructionArithRegSet *>(inst));
                case InstructionFenceSet::OP_CODE:
                    return visit_fence_set(reinterpret_cast<InstructionFenceSet *>(inst));
                case InstructionSystemSet::OP_CODE:
                    return visit_system_set(reinterpret_cast<InstructionSystemSet *>(inst));
#if defined(__RV_64_BIT__)
                case InstructionArithImmWSet::OP_CODE:
                    return visit_arith_imm_w_set(reinterpret_cast<InstructionArithImmWSet *>(inst));
                case InstructionArithRegWSet::OP_CODE:
                    return visit_arith_reg_w_set(reinterpret_cast<InstructionArithRegWSet *>(inst));
#endif // defined(__RV_64_BIT__)
#if defined(__RV_CUSTOM_0__)
                case InstructionCustome0::OP_CODE:
                    return sub_type()->visit_custom_0_inst(reinterpret_cast<InstructionCustome0 *>(inst));
#endif // defined(__RV_CUSTOM_0__)
#if defined(__RV_CUSTOM_1__)
                case InstructionCustome1::OP_CODE:
                    return sub_type()->visit_custom_1_inst(reinterpret_cast<InstructionCustome0 *>(inst));
#endif // defined(__RV_CUSTOM_1__)
#if defined(__RV_CUSTOM_2__)
                case InstructionCustome2::OP_CODE:
                    return sub_type()->visit_custom_2_inst(reinterpret_cast<InstructionCustome0 *>(inst));
#endif // defined(__RV_CUSTOM_2__)
#if defined(__RV_CUSTOM_3__)
                case InstructionCustome3::OP_CODE:
                    return sub_type()->visit_custom_3_inst(reinterpret_cast<InstructionCustome0 *>(inst));
#endif // defined(__RV_CUSTOM_3__)
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_branch_set(InstructionBranchSet *inst) {
            switch (inst->get_funct3()) {
                case BEQInst::FUNCT3:
                    return sub_type()->visit_beq_inst(reinterpret_cast<BEQInst *>(inst));
                case BNEInst::FUNCT3:
                    return sub_type()->visit_bne_inst(reinterpret_cast<BNEInst *>(inst));
                case BLTInst::FUNCT3:
                    return sub_type()->visit_blt_inst(reinterpret_cast<BLTInst *>(inst));
                case BGEInst::FUNCT3:
                    return sub_type()->visit_bge_inst(reinterpret_cast<BGEInst *>(inst));
                case BLTUInst::FUNCT3:
                    return sub_type()->visit_bltu_inst(reinterpret_cast<BLTUInst *>(inst));
                case BGEUInst::FUNCT3:
                    return sub_type()->visit_bgeu_inst(reinterpret_cast<BGEUInst *>(inst));
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_load_set(InstructionLoadSet *inst) {
            switch (inst->get_funct3()) {
                case LBInst::FUNCT3:
                    return sub_type()->visit_lb_inst(reinterpret_cast<LBInst *>(inst));
                case LHInst::FUNCT3:
                    return sub_type()->visit_lh_inst(reinterpret_cast<LHInst *>(inst));
                case LWInst::FUNCT3:
                case LBUInst::FUNCT3:
                    return sub_type()->visit_lbu_inst(reinterpret_cast<LBUInst *>(inst));
                case LHUInst::FUNCT3:
                    return sub_type()->visit_lhu_inst(reinterpret_cast<LHUInst *>(inst));
#if defined(__RV_64_BIT__)
                case LDInst::FUNCT3:
                    return sub_type()->visit_ld_inst(reinterpret_cast<LDInst *>(inst));
                case LWUInst::FUNCT3:
                    return sub_type()->visit_lwu_inst(reinterpret_cast<LWUInst *>(inst));
#endif // defined(__RV_64_BIT__)
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_store_set(InstructionStoreSet *inst) {
            switch (inst->get_funct3()) {
                case SBInst::FUNCT3:
                    return sub_type()->visit_sb_inst(reinterpret_cast<SBInst *>(inst));
                case SHInst::FUNCT3:
                    return sub_type()->visit_sh_inst(reinterpret_cast<SHInst *>(inst));
                case SWInst::FUNCT3:
                    return sub_type()->visit_sw_inst(reinterpret_cast<SWInst *>(inst));
#if defined(__RV_64_BIT__)
                case SDInst::FUNCT3:
                    return sub_type()->visit_sd_inst(reinterpret_cast<SDInst *>(inst));
#endif // defined(__RV_64_BIT__)
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_arith_imm_set(InstructionArithImmSet *inst) {
            switch (inst->get_funct3()) {
                case ADDIInst::FUNCT3:
                    return sub_type()->visit_addi_inst(reinterpret_cast<ADDIInst *>(inst));
                case SLTIInst::FUNCT3:
                    return sub_type()->visit_slti_inst(reinterpret_cast<SLTIInst *>(inst));
                case SLTIUInst::FUNCT3:
                    return sub_type()->visit_sltiu_inst(reinterpret_cast<SLTIUInst *>(inst));
                case XORIInst::FUNCT3:
                    return sub_type()->visit_xori_inst(reinterpret_cast<XORIInst *>(inst));
                case ORIInst::FUNCT3:
                    return sub_type()->visit_ori_inst(reinterpret_cast<ORIInst *>(inst));
                case ANDIInst::FUNCT3:
                    return sub_type()->visit_andi_inst(reinterpret_cast<ANDIInst *>(inst));
                case SLLIInst::FUNCT3:
                    switch (reinterpret_cast<SLLIInst *>(inst)->get_funct_shift()) {
                        case SLLIInst::FUNCT_SHIFT:
                            return sub_type()->visit_slli_inst(reinterpret_cast<SLLIInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
                case InstructionShiftRightImmSet::FUNCT3:
                    switch (reinterpret_cast<InstructionShiftRightImmSet *>(inst)->get_funct_shift()) {
                        case SRLIInst::FUNCT_SHIFT:
                            return sub_type()->visit_srli_inst(reinterpret_cast<SRLIInst *>(inst));
                        case SRAIInst::FUNCT_SHIFT:
                            return sub_type()->visit_srai_inst(reinterpret_cast<SRAIInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_arith_reg_set(InstructionArithRegSet *inst) {
            switch (inst->get_funct7()) {
                case InstructionIntegerRegSet::FUNCT7:
                    switch (inst->get_funct3()) {
                        case ADDInst::FUNCT3:
                            return sub_type()->visit_add_inst(reinterpret_cast<ADDInst *>(inst));
                        case SLLInst::FUNCT3:
                            return sub_type()->visit_sll_inst(reinterpret_cast<SLLInst *>(inst));
                        case SLTInst::FUNCT3:
                            return sub_type()->visit_slt_inst(reinterpret_cast<SLTInst *>(inst));
                        case SLTUInst::FUNCT3:
                            return sub_type()->visit_sltu_inst(reinterpret_cast<SLTUInst *>(inst));
                        case XORInst::FUNCT3:
                            return sub_type()->visit_xor_inst(reinterpret_cast<XORInst *>(inst));
                        case SRLInst::FUNCT3:
                            return sub_type()->visit_srl_inst(reinterpret_cast<SRLInst *>(inst));
                        case ORInst::FUNCT3:
                            return sub_type()->visit_or_inst(reinterpret_cast<ORInst *>(inst));
                        case ANDInst::FUNCT3:
                            return sub_type()->visit_and_inst(reinterpret_cast<ANDInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
                case InstructionIntegerRegModSet::FUNCT7:
                    switch (inst->get_funct3()) {
                        case SUBInst::FUNCT3:
                            return sub_type()->visit_sub_inst(reinterpret_cast<SUBInst *>(inst));
                        case SRAInst::FUNCT3:
                            return sub_type()->visit_sra_inst(reinterpret_cast<SRAInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
#if defined(__RV_EXTENSION_M__)
                case InstructionMulDivSet::FUNCT7:
                    switch (inst->get_funct3()) {
                        case MULInst::FUNCT_3:
                            return sub_type()->visit_mul_inst(reinterpret_cast<MULInst *>(inst));
                        case MULHInst::FUNCT_3:
                            return sub_type()->visit_mulh_inst(reinterpret_cast<MULHInst *>(inst));
                        case MULHSUInst::FUNCT_3:
                            return sub_type()->visit_mulhsu_inst(reinterpret_cast<MULHSUInst *>(inst));
                        case MULHUInst::FUNCT_3:
                            return sub_type()->visit_mulhu_inst(reinterpret_cast<MULHUInst *>(inst));
                        case DIVInst::FUNCT_3:
                            return sub_type()->visit_div_inst(reinterpret_cast<DIVInst *>(inst));
                        case DIVUInst::FUNCT_3:
                            return sub_type()->visit_divu_inst(reinterpret_cast<DIVUInst *>(inst));
                        case REMInst::FUNCT_3:
                            return sub_type()->visit_rem_inst(reinterpret_cast<REMInst *>(inst));
                        case REMUInst::FUNCT_3:
                            return sub_type()->visit_remu_inst(reinterpret_cast<REMUInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
#endif // defined(__RV_EXTENSION_M__)
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_fence_set(InstructionFenceSet *inst) {
            switch (inst->get_funct3()) {
                case FENCEInst::FUNCT3:
                    return sub_type()->visit_fence_inst(reinterpret_cast<FENCEInst *>(inst));
#if defined(__RV_EXTENSION_ZIFENCEI__)
                case FENCEIInst::FUNCT3:
                    return sub_type()->visit_fencei_inst(reinterpret_cast<FENCEIInst *>(inst));
#endif // defined(__RV_EXTENSION_ZIFENCEI__)
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_system_set(InstructionSystemSet *inst) {
            switch (inst->get_funct3()) {
                case InstructionEnvironmentSet::FUNCT3:
                    if (reinterpret_cast<InstructionEnvironmentSet *>(inst)->get_unused() != 0)
                        return sub_type()->illegal_instruction(inst);
                    switch (reinterpret_cast<InstructionEnvironmentSet *>(inst)->get_funct_environment()) {
                        case ECALLInst::FUNCT_ENVIRONMENT:
                            return sub_type()->visit_ecall_inst(reinterpret_cast<ECALLInst *>(inst));
                        case EBREAKInst::FUNCT_ENVIRONMENT:
                            return sub_type()->visit_ebreak_inst(reinterpret_cast<EBREAKInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
#if defined(__RV_EXTENSION_ZICSR__)
                case CSRRWInst::FUNCT3:
                    return sub_type()->visit_csrrw_inst(reinterpret_cast<CSRRWInst *>(inst));
                case CSRRSInst::FUNCT3:
                    return sub_type()->visit_csrrs_inst(reinterpret_cast<CSRRSInst *>(inst));
                case CSRRCInst::FUNCT3:
                    return sub_type()->visit_csrrc_inst(reinterpret_cast<CSRRCInst *>(inst));
                case CSRRWIInst::FUNCT3:
                    return sub_type()->visit_csrrwi_inst(reinterpret_cast<CSRRWIInst *>(inst));
                case CSRRSIInst::FUNCT3:
                    return sub_type()->visit_csrrsi_inst(reinterpret_cast<CSRRSIInst *>(inst));
                case CSRRCIInst::FUNCT3:
                    return sub_type()->visit_csrrci_inst(reinterpret_cast<CSRRCIInst *>(inst));
#endif // defined(__RV_EXTENSION_ZICSR__)
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

#endif // defined(__RV_32_BIT__) || defined(__RV_64_BIT__)
#if defined(__RV_64_BIT__)

        RetT visit_arith_imm_w_set(InstructionArithImmWSet *inst) {
            switch (inst->get_funct3()) {
                case ADDIWInst::FUNCT3:
                    return sub_type()->visit_addiw_inst(reinterpret_cast<ADDIWInst *>(inst));
                case SLLIWInst::FUNCT3:
                    switch (reinterpret_cast<SLLIWInst *>(inst)->get_funct_shift()) {
                        case SLLIWInst::FUNCT_SHIFT:
                            return sub_type()->visit_slliw_inst(reinterpret_cast<SLLIWInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
                case InstructionShiftRightImmWSet::FUNCT3:
                    switch (reinterpret_cast<InstructionShiftRightImmWSet *>(inst)->get_funct_shift()) {
                        case SRLIWInst::FUNCT_SHIFT:
                            return sub_type()->visit_srliw_inst(reinterpret_cast<SRLIWInst *>(inst));
                        case SRAIWInst::FUNCT_SHIFT:
                            return sub_type()->visit_sraiw_inst(reinterpret_cast<SRAIWInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_arith_reg_w_set(InstructionArithRegWSet *inst) {
            switch (inst->get_funct7()) {
                case InstructionIntegerRegWSet::FUNCT7:
                    switch (inst->get_funct3()) {
                        case ADDWInst::FUNCT3:
                            return sub_type()->visit_addw_inst(reinterpret_cast<ADDWInst *>(inst));
                        case SLLWInst::FUNCT3:
                            return sub_type()->visit_sllw_inst(reinterpret_cast<SLLWInst *>(inst));
                        case SRLWInst::FUNCT3:
                            return sub_type()->visit_srlw_inst(reinterpret_cast<SRLWInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
                case InstructionIntegerRegModWSet::FUNCT7:
                    switch (inst->get_funct3()) {
                        case SUBWInst::FUNCT3:
                            return sub_type()->visit_subw_inst(reinterpret_cast<SUBWInst *>(inst));
                        case SRAWInst::FUNCT3:
                            return sub_type()->visit_sraw_inst(reinterpret_cast<SRAWInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
#if defined(__RV_EXTENSION_M__)
                case InstructionMulDivWSet::FUNCT7:
                    switch (inst->get_funct3()) {
                        case MULWInst::FUNCT_3:
                            return sub_type()->visit_mulw_inst(reinterpret_cast<MULWInst *>(inst));
                        case DIVWInst::FUNCT_3:
                            return sub_type()->visit_divw_inst(reinterpret_cast<DIVWInst *>(inst));
                        case DIVUWInst::FUNCT_3:
                            return sub_type()->visit_divuw_inst(reinterpret_cast<DIVUWInst *>(inst));
                        case REMWInst::FUNCT_3:
                            return sub_type()->visit_remw_inst(reinterpret_cast<REMWInst *>(inst));
                        case REMUWInst::FUNCT_3:
                            return sub_type()->visit_remuw_inst(reinterpret_cast<REMUWInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
#endif // defined(__RV_EXTENSION_M__)
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

#endif // defined(__RV_64_BIT__)

        RetT illegal_instruction(risc_v_isa_unused Instruction *inst) {
            risc_v_isa_unreachable("Illegal instruction met!");
        }

        RetT visit_inst(risc_v_isa_unused Instruction *inst) {
            risc_v_isa_unreachable("Uncaught instruction in visitor definition!");
        }

#if defined(__RV_EXTENSION_C__)

        RetT visit_16_inst(Instruction16 *inst) { return sub_type()->visit_inst(); }

#endif // defined(__RV_EXTENSION_C__)
#if defined(__RV_32_BIT__) || defined(__RV_64_BIT__)

        RetT visit_32_inst(Instruction32 *inst) { return sub_type()->visit_inst(inst); }

        RetT visit_branch_set_inst(InstructionBranchSet *inst) { return sub_type()->visit_32_inst(inst); }

        RetT visit_load_set_inst(InstructionLoadSet *inst) { return sub_type()->visit_32_inst(inst); }

        RetT visit_store_set_inst(InstructionStoreSet *inst) { return sub_type()->visit_32_inst(inst); }

        RetT visit_arith_imm_set_inst(InstructionArithImmSet *inst) { return sub_type()->visit_32_inst(inst); }

        RetT visit_arith_reg_set_inst(InstructionArithRegSet *inst) { return sub_type()->visit_32_inst(inst); }

        RetT visit_fence_set_inst(InstructionFenceSet *inst) { return sub_type()->visit_32_inst(inst); }

        RetT visit_system_set_inst(InstructionSystemSet *inst) { return sub_type()->visit_32_inst(inst); }

        RetT visit_lui_inst(LUIInst *inst) { return sub_type()->visit_32_inst(inst); }

        RetT visit_auipc_inst(AUIPCInst *inst) { return sub_type()->visit_32_inst(inst); }

        RetT visit_jal_inst(JALInst *inst) { return sub_type()->visit_32_inst(inst); }

        RetT visit_jalr_inst(JALRInst *inst) { return sub_type()->visit_32_inst(inst); }

        RetT visit_beq_inst(BEQInst *inst) { return sub_type()->visit_branch_set_inst(inst); }

        RetT visit_bne_inst(BNEInst *inst) { return sub_type()->visit_branch_set_inst(inst); }

        RetT visit_blt_inst(BLTInst *inst) { return sub_type()->visit_branch_set_inst(inst); }

        RetT visit_bge_inst(BGEInst *inst) { return sub_type()->visit_branch_set_inst(inst); }

        RetT visit_bltu_inst(BLTUInst *inst) { return sub_type()->visit_branch_set_inst(inst); }

        RetT visit_bgeu_inst(BGEUInst *inst) { return sub_type()->visit_branch_set_inst(inst); }

        RetT visit_lb_inst(LBInst *inst) { return sub_type()->visit_load_set_inst(inst); }

        RetT visit_lh_inst(LHInst *inst) { return sub_type()->visit_load_set_inst(inst); }

        RetT visit_lw_inst(LWInst *inst) { return sub_type()->visit_load_set_inst(inst); }

        RetT visit_lbu_inst(LBUInst *inst) { return sub_type()->visit_load_set_inst(inst); }

        RetT visit_lhu_inst(LHUInst *inst) { return sub_type()->visit_load_set_inst(inst); }

        RetT visit_sb_inst(SBInst *inst) { return sub_type()->visit_store_set_inst(inst); }

        RetT visit_sh_inst(SHInst *inst) { return sub_type()->visit_store_set_inst(inst); }

        RetT visit_sw_inst(SWInst *inst) { return sub_type()->visit_store_set_inst(inst); }

        RetT visit_addi_inst(ADDIInst *inst) { return sub_type()->visit_arith_imm_set_inst(inst); }

        RetT visit_slti_inst(SLTIInst *inst) { return sub_type()->visit_arith_imm_set_inst(inst); }

        RetT visit_sltiu_inst(SLTIUInst *inst) { return sub_type()->visit_arith_imm_set_inst(inst); }

        RetT visit_xori_inst(XORIInst *inst) { return sub_type()->visit_arith_imm_set_inst(inst); }

        RetT visit_ori_inst(ORIInst *inst) { return sub_type()->visit_arith_imm_set_inst(inst); }

        RetT visit_andi_inst(ANDIInst *inst) { return sub_type()->visit_arith_imm_set_inst(inst); }

        RetT visit_slli_inst(SLLIInst *inst) { return sub_type()->visit_arith_imm_set_inst(inst); }

        RetT visit_srli_inst(SRLIInst *inst) { return sub_type()->visit_arith_imm_set_inst(inst); }

        RetT visit_srai_inst(SRAIInst *inst) { return sub_type()->visit_arith_imm_set_inst(inst); }

        RetT visit_add_inst(ADDInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

        RetT visit_sub_inst(SUBInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

        RetT visit_sll_inst(SLLInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

        RetT visit_slt_inst(SLTInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

        RetT visit_sltu_inst(SLTUInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

        RetT visit_xor_inst(XORInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

        RetT visit_srl_inst(SRLInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

        RetT visit_sra_inst(SRAInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

        RetT visit_or_inst(ORInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

        RetT visit_and_inst(ANDInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

        RetT visit_fence_inst(FENCEInst *inst) { return sub_type()->visit_fence_set_inst(inst); }

        RetT visit_ecall_inst(ECALLInst *inst) { return sub_type()->visit_system_set_inst(inst); }

        RetT visit_ebreak_inst(EBREAKInst *inst) { return sub_type()->visit_system_set_inst(inst); }

#if defined(__RV_EXTENSION_M__)

        RetT visit_mul_inst(MULInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

        RetT visit_mulh_inst(MULHInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

        RetT visit_mulhsu_inst(MULHSUInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

        RetT visit_mulhu_inst(MULHUInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

        RetT visit_div_inst(DIVInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

        RetT visit_divu_inst(DIVUInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

        RetT visit_rem_inst(REMInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

        RetT visit_remu_inst(REMUInst *inst) { return sub_type()->visit_arith_reg_set_inst(inst); }

#endif // defined(__RV_EXTENSION_M__)
#endif // defined(__RV_32_BIT__) || defined(__RV_64_BIT__)
#if defined(__RV_64_BIT__)

        RetT visit_arith_imm_w_set_inst(InstructionArithImmWSet *inst) { return sub_type()->visit_32_inst(inst); }

        RetT visit_arith_reg_w_set_inst(InstructionArithRegWSet *inst) { return sub_type()->visit_32_inst(inst); }

        RetT visit_ld_inst(LDInst *inst) { return sub_type()->visit_load_set_inst(inst); }

        RetT visit_lwu_inst(LWUInst *inst) { return sub_type()->visit_load_set_inst(inst); }

        RetT visit_sd_inst(SDInst *inst) { return sub_type()->visit_store_set_inst(inst); }

        RetT visit_addiw_inst(ADDIWInst *inst) { return sub_type()->visit_arith_imm_w_set_inst(inst); }

        RetT visit_slliw_inst(SLLIWInst *inst) { return sub_type()->visit_arith_imm_w_set_inst(inst); }

        RetT visit_srliw_inst(SRLIWInst *inst) { return sub_type()->visit_arith_imm_w_set_inst(inst); }

        RetT visit_sraiw_inst(SRAIWInst *inst) { return sub_type()->visit_arith_imm_w_set_inst(inst); }

        RetT visit_addw_inst(ADDWInst *inst) { return sub_type()->visit_arith_reg_w_set_inst(inst); }

        RetT visit_subw_inst(SUBWInst *inst) { return sub_type()->visit_arith_reg_w_set_inst(inst); }

        RetT visit_sllw_inst(SLLWInst *inst) { return sub_type()->visit_arith_reg_w_set_inst(inst); }

        RetT visit_srlw_inst(SRLWInst *inst) { return sub_type()->visit_arith_reg_w_set_inst(inst); }

        RetT visit_sraw_inst(SRAWInst *inst) { return sub_type()->visit_arith_reg_w_set_inst(inst); }

#if defined(__RV_EXTENSION_M__)

        RetT visit_mulw_inst(MULWInst *inst) { return sub_type()->visit_arith_reg_w_set_inst(inst); }

        RetT visit_divw_inst(DIVWInst *inst) { return sub_type()->visit_arith_reg_w_set_inst(inst); }

        RetT visit_divuw_inst(DIVUWInst *inst) { return sub_type()->visit_arith_reg_w_set_inst(inst); }

        RetT visit_remw_inst(REMWInst *inst) { return sub_type()->visit_arith_reg_w_set_inst(inst); }

        RetT visit_remuw_inst(REMUWInst *inst) { return sub_type()->visit_arith_reg_w_set_inst(inst); }

#endif // defined(__RV_EXTENSION_M__)
#endif // defined(__RV_64_BIT__)
#if defined(__RV_EXTENSION_ZIFENCEI__)

        RetT visit_fencei_inst(FENCEIInst *inst) { return sub_type()->visit_fence_set_inst(inst); }

#endif // defined(__RV_EXTENSION_ZIFENCEI__)
#if defined(__RV_EXTENSION_ZICSR__)

        RetT visit_csrrw_inst(CSRRWInst *inst) { return sub_type()->visit_system_set_inst(inst); }

        RetT visit_csrrs_inst(CSRRSInst *inst) { return sub_type()->visit_system_set_inst(inst); }

        RetT visit_csrrc_inst(CSRRCInst *inst) { return sub_type()->visit_system_set_inst(inst); }

        RetT visit_csrrwi_inst(CSRRWIInst *inst) { return sub_type()->visit_system_set_inst(inst); }

        RetT visit_csrrsi_inst(CSRRSIInst *inst) { return sub_type()->visit_system_set_inst(inst); }

        RetT visit_csrrci_inst(CSRRCIInst *inst) { return sub_type()->visit_system_set_inst(inst); }

#endif // defined(__RV_EXTENSION_ZICSR__)
#if defined(__RV_CUSTOM_0__)

        RetT visit_custom_0_inst(InstructionCustome0 *inst) { return sub_type()->visit_32_inst(inst); }

#endif // defined(__RV_CUSTOM_0__)
#if defined(__RV_CUSTOM_1__)

        RetT visit_custom_1_inst(InstructionCustome1 *inst) { return sub_type()->visit_32_inst(inst); }

#endif // defined(__RV_CUSTOM_1__)
#if defined(__RV_CUSTOM_2__)

        RetT visit_custom_2_inst(InstructionCustome2 *inst) { return sub_type()->visit_32_inst(inst); }

#endif // defined(__RV_CUSTOM_2__)
#if defined(__RV_CUSTOM_3__)

        RetT visit_custom_3_inst(InstructionCustome3 *inst) { return sub_type()->visit_32_inst(inst); }

#endif // defined(__RV_CUSTOM_3__)
    };
}


#endif //RISC_V_ISA_INSTRUCTION_VISITOR_HPP
