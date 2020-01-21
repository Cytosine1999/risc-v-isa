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
    template<typename T, typename _RetT = void>
    class InstructionVisitor {
    public:
        using RetT = _RetT;

        RetT visit(Instruction *inst) {
            static_assert(std::is_base_of<InstructionVisitor, T>::value);

            usize leading_16 = *reinterpret_cast<u16 *>(inst);

            RetT ret{};

            while (true) {
#if defined(__RV_EXTENSION_C__)
                if ((val & BITS_MASK<u16, 2, 0>) != BITS_MASK<u16, 2, 0>) {
                    return visit_16(reinterpret_cast<Instruction16 *>(inst));
                    break;
                }
#endif
                if ((leading_16 & BITS_MASK<u16, 5, 2>) != BITS_MASK<u16, 5, 2>) {
                    return visit_32(reinterpret_cast<Instruction32 *>(inst));
                    break;
                }
                return static_cast<T *>(this)->illegal_instruction(inst);
            }

            end_inst(inst);

            return ret;
        }

#if defined(__RV_EXTENSION_C__)

        RetT visit_16(Instruction16 *inst) {
            return;
        }

#endif // defined(__RV_EXTENSION_C__)

        RetT visit_32(Instruction32 *inst) {
            switch (inst->get_op_code()) {
                case LUIInst::OP_CODE:
                    return static_cast<T *>(this)->visit_lui_inst(reinterpret_cast<LUIInst *>(inst));
                case AUIPCInst::OP_CODE:
                    return static_cast<T *>(this)->visit_auipc_inst(reinterpret_cast<AUIPCInst *>(inst));
                case JALInst::OP_CODE:
                    return static_cast<T *>(this)->visit_jal_inst(reinterpret_cast<JALInst *>(inst));
                case JALRInst::OP_CODE:
                    if (reinterpret_cast<JALRInst *>(inst)->get_funct3() != JALRInst::FUNC_3)
                        return static_cast<T *>(this)->illegal_instruction(inst);
                    return static_cast<T *>(this)->visit_jalr_inst(reinterpret_cast<JALRInst *>(inst));
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
                    return static_cast<T *>(this)->visit_custom_0_inst(reinterpret_cast<InstructionCustome0 *>(inst));
#endif // defined(__RV_CUSTOM_0__)
#if defined(__RV_CUSTOM_1__)
                case InstructionCustome1::OP_CODE:
                    return static_cast<T *>(this)->visit_custom_1_inst(reinterpret_cast<InstructionCustome0 *>(inst));
#endif // defined(__RV_CUSTOM_1__)
#if defined(__RV_CUSTOM_2__)
                case InstructionCustome2::OP_CODE:
                    return static_cast<T *>(this)->visit_custom_2_inst(reinterpret_cast<InstructionCustome0 *>(inst));
#endif // defined(__RV_CUSTOM_2__)
#if defined(__RV_CUSTOM_3__)
                case InstructionCustome3::OP_CODE:
                    return static_cast<T *>(this)->visit_custom_3_inst(reinterpret_cast<InstructionCustome0 *>(inst));
#endif // defined(__RV_CUSTOM_3__)
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }
        }

        RetT visit_branch_set(InstructionBranchSet *inst) {
            switch (inst->get_funct3()) {
                case BEQInst::FUNC_3:
                    return static_cast<T *>(this)->visit_beq_inst(reinterpret_cast<BEQInst *>(inst));
                case BNEInst::FUNC_3:
                    return static_cast<T *>(this)->visit_bne_inst(reinterpret_cast<BNEInst *>(inst));
                case BLTInst::FUNC_3:
                    return static_cast<T *>(this)->visit_blt_inst(reinterpret_cast<BLTInst *>(inst));
                case BGEInst::FUNC_3:
                    return static_cast<T *>(this)->visit_bge_inst(reinterpret_cast<BGEInst *>(inst));
                case BLTUInst::FUNC_3:
                    return static_cast<T *>(this)->visit_bltu_inst(reinterpret_cast<BLTUInst *>(inst));
                case BGEUInst::FUNC_3:
                    return static_cast<T *>(this)->visit_bgeu_inst(reinterpret_cast<BGEUInst *>(inst));
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }
        }

        RetT visit_load_set(InstructionLoadSet *inst) {
            switch (inst->get_funct3()) {
                case LBInst::FUNC_3:
                    return static_cast<T *>(this)->visit_lb_inst(reinterpret_cast<LBInst *>(inst));
                case LHInst::FUNC_3:
                    return static_cast<T *>(this)->visit_lh_inst(reinterpret_cast<LHInst *>(inst));
                case LWInst::FUNC_3:
                case LBUInst::FUNC_3:
                    return static_cast<T *>(this)->visit_lbu_inst(reinterpret_cast<LBUInst *>(inst));
                case LHUInst::FUNC_3:
                    return static_cast<T *>(this)->visit_lhu_inst(reinterpret_cast<LHUInst *>(inst));
#if defined(__RV_64_BIT__)
                case LDInst::FUNC_3:
                    return static_cast<T *>(this)->visit_ld_inst(reinterpret_cast<LDInst *>(inst));
                case LWUInst::FUNC_3:
                    return static_cast<T *>(this)->visit_lwu_inst(reinterpret_cast<LWUInst *>(inst));
#endif // defined(__RV_64_BIT__)
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }
        }

        RetT visit_store_set(InstructionStoreSet *inst) {
            switch (inst->get_funct3()) {
                case SBInst::FUNC_3:
                    return static_cast<T *>(this)->visit_sb_inst(reinterpret_cast<SBInst *>(inst));
                case SHInst::FUNC_3:
                    return static_cast<T *>(this)->visit_sh_inst(reinterpret_cast<SHInst *>(inst));
                case SWInst::FUNC_3:
                    return static_cast<T *>(this)->visit_sw_inst(reinterpret_cast<SWInst *>(inst));
#if defined(__RV_64_BIT__)
                case SDInst::FUNC_3:
                    return static_cast<T *>(this)->visit_sd_inst(reinterpret_cast<SDInst *>(inst));
#endif // defined(__RV_64_BIT__)
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }
        }

        RetT visit_arith_imm_set(InstructionArithImmSet *inst) {
            switch (inst->get_funct3()) {
                case ADDIInst::FUNC_3:
                    return static_cast<T *>(this)->visit_addi_inst(reinterpret_cast<ADDIInst *>(inst));
                case SLTIInst::FUNC_3:
                    return static_cast<T *>(this)->visit_slti_inst(reinterpret_cast<SLTIInst *>(inst));
                case SLTIUInst::FUNC_3:
                    return static_cast<T *>(this)->visit_sltiu_inst(reinterpret_cast<SLTIUInst *>(inst));
                case XORIInst::FUNC_3:
                    return static_cast<T *>(this)->visit_xori_inst(reinterpret_cast<XORIInst *>(inst));
                case ORIInst::FUNC_3:
                    return static_cast<T *>(this)->visit_ori_inst(reinterpret_cast<ORIInst *>(inst));
                case ANDIInst::FUNC_3:
                    return static_cast<T *>(this)->visit_andi_inst(reinterpret_cast<ANDIInst *>(inst));
                case SLLIInst::FUNC_3:
                    switch (reinterpret_cast<SLLIInst *>(inst)->get_funct_shift()) {
                        case SLLIInst::FUNC_SHIFT:
                            return static_cast<T *>(this)->visit_slli_inst(reinterpret_cast<SLLIInst *>(inst));
                        default:
                            return static_cast<T *>(this)->illegal_instruction(inst);
                    }
                case InstructionShiftRightImmSet::FUNC_3:
                    switch (reinterpret_cast<InstructionShiftRightImmSet *>(inst)->get_funct_shift()) {
                        case SRLIInst::FUNC_SHIFT:
                            return static_cast<T *>(this)->visit_srli_inst(reinterpret_cast<SRLIInst *>(inst));
                        case SRAIInst::FUNC_SHIFT:
                            return static_cast<T *>(this)->visit_srai_inst(reinterpret_cast<SRAIInst *>(inst));
                        default:
                            return static_cast<T *>(this)->illegal_instruction(inst);
                    }
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }
        }

        RetT visit_arith_reg_set(InstructionArithRegSet *inst) {
            switch (inst->get_funct7()) {
                case InstructionIntegerRegSet::FUNC_7:
                    switch (inst->get_funct3()) {
                        case ADDInst::FUNC_3:
                            return static_cast<T *>(this)->visit_add_inst(reinterpret_cast<ADDInst *>(inst));
                        case SLLInst::FUNC_3:
                            return static_cast<T *>(this)->visit_sll_inst(reinterpret_cast<SLLInst *>(inst));
                        case SLTInst::FUNC_3:
                            return static_cast<T *>(this)->visit_slt_inst(reinterpret_cast<SLTInst *>(inst));
                        case SLTUInst::FUNC_3:
                            return static_cast<T *>(this)->visit_sltu_inst(reinterpret_cast<SLTUInst *>(inst));
                        case XORInst::FUNC_3:
                            return static_cast<T *>(this)->visit_xor_inst(reinterpret_cast<XORInst *>(inst));
                        case SRLInst::FUNC_3:
                            return static_cast<T *>(this)->visit_srl_inst(reinterpret_cast<SRLInst *>(inst));
                        case ORInst::FUNC_3:
                            return static_cast<T *>(this)->visit_or_inst(reinterpret_cast<ORInst *>(inst));
                        case ANDInst::FUNC_3:
                            return static_cast<T *>(this)->visit_and_inst(reinterpret_cast<ANDInst *>(inst));
                        default:
                            return static_cast<T *>(this)->illegal_instruction(inst);
                    }
                case InstructionIntegerRegModSet::FUNC_7:
                    switch (inst->get_funct3()) {
                        case SUBInst::FUNC_3:
                            return static_cast<T *>(this)->visit_sub_inst(reinterpret_cast<SUBInst *>(inst));
                        case SRAInst::FUNC_3:
                            return static_cast<T *>(this)->visit_sra_inst(reinterpret_cast<SRAInst *>(inst));
                        default:
                            return static_cast<T *>(this)->illegal_instruction(inst);
                    }
#if defined(__RV_EXTENSION_M__)
                case InstructionMulDivSet::FUNC_7:
                    switch (inst->get_funct3()) {
                        case MULInst::FUNCT_3:
                            return static_cast<T *>(this)->visit_mul_inst(reinterpret_cast<MULInst *>(inst));
                        case MULHInst::FUNCT_3:
                            return static_cast<T *>(this)->visit_mulh_inst(reinterpret_cast<MULHInst *>(inst));
                        case MULHSUInst::FUNCT_3:
                            return static_cast<T *>(this)->visit_mulhsu_inst(reinterpret_cast<MULHSUInst *>(inst));
                        case MULHUInst::FUNCT_3:
                            return static_cast<T *>(this)->visit_mulhu_inst(reinterpret_cast<MULHUInst *>(inst));
                        case DIVInst::FUNCT_3:
                            return static_cast<T *>(this)->visit_div_inst(reinterpret_cast<DIVInst *>(inst));
                        case DIVUInst::FUNCT_3:
                            return static_cast<T *>(this)->visit_divu_inst(reinterpret_cast<DIVUInst *>(inst));
                        case REMInst::FUNCT_3:
                            return static_cast<T *>(this)->visit_rem_inst(reinterpret_cast<REMInst *>(inst));
                        case REMUInst::FUNCT_3:
                            return static_cast<T *>(this)->visit_remu_inst(reinterpret_cast<REMUInst *>(inst));
                        default:
                            return static_cast<T *>(this)->illegal_instruction(inst);
                    }
#endif // defined(__RV_EXTENSION_M__)
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }
        }

        RetT visit_fence_set(InstructionFenceSet *inst) {
            switch (inst->get_funct3()) {
                case FENCEInst::FUNC_3:
                    return static_cast<T *>(this)->visit_fence_inst(reinterpret_cast<FENCEInst *>(inst));
#if defined(__RV_EXTENSION_ZIFENCEI__)
                case FENCEIInst::FUNC_3:
                    return static_cast<T *>(this)->visit_fencei_inst(reinterpret_cast<FENCEIInst *>(inst));
#endif // defined(__RV_EXTENSION_ZIFENCEI__)
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }
        }

        RetT visit_system_set(InstructionSystemSet *inst) {
            switch (inst->get_funct3()) {
                case InstructionEnvironmentSet::FUNC_3:
                    if (reinterpret_cast<InstructionEnvironmentSet *>(inst)->get_unused() != 0)
                        return static_cast<T *>(this)->illegal_instruction(inst);
                    switch (reinterpret_cast<InstructionEnvironmentSet *>(inst)->get_funct_environment()) {
                        case ECALLInst::FUNCT_ENVIRONMENT:
                            return static_cast<T *>(this)->visit_ecall_inst(reinterpret_cast<ECALLInst *>(inst));
                        case EBREAKInst::FUNCT_ENVIRONMENT:
                            return static_cast<T *>(this)->visit_ebreak_inst(reinterpret_cast<EBREAKInst *>(inst));
                        default:
                            return static_cast<T *>(this)->illegal_instruction(inst);
                    }
#if defined(__RV_EXTENSION_ZICSR__)
                case CSRRWInst::FUNC_3:
                    return static_cast<T *>(this)->visit_csrrw_inst(reinterpret_cast<CSRRWInst *>(inst));
                case CSRRSInst::FUNC_3:
                    return static_cast<T *>(this)->visit_csrrs_inst(reinterpret_cast<CSRRSInst *>(inst));
                case CSRRCInst::FUNC_3:
                    return static_cast<T *>(this)->visit_csrrc_inst(reinterpret_cast<CSRRCInst *>(inst));
                case CSRRWIInst::FUNC_3:
                    return static_cast<T *>(this)->visit_csrrwi_inst(reinterpret_cast<CSRRWIInst *>(inst));
                case CSRRSIInst::FUNC_3:
                    return static_cast<T *>(this)->visit_csrrsi_inst(reinterpret_cast<CSRRSIInst *>(inst));
                case CSRRCIInst::FUNC_3:
                    return static_cast<T *>(this)->visit_csrrci_inst(reinterpret_cast<CSRRCIInst *>(inst));
#endif // defined(__RV_EXTENSION_ZICSR__)
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }
        }

#if defined(__RV_64_BIT__)

        RetT visit_arith_imm_w_set(InstructionArithImmWSet *inst) {
            switch (inst->get_funct3()) {
                case ADDIWInst::FUNC_3:
                    return static_cast<T *>(this)->visit_addiw_inst(reinterpret_cast<ADDIWInst *>(inst));
                case SLLIWInst::FUNC_3:
                    switch (reinterpret_cast<SLLIWInst *>(inst)->get_funct_shift()) {
                        case SLLIWInst::FUNC_SHIFT:
                            return static_cast<T *>(this)->visit_slliw_inst(reinterpret_cast<SLLIWInst *>(inst));
                        default:
                            return static_cast<T *>(this)->illegal_instruction(inst);
                    }
                case InstructionShiftRightImmWSet::FUNC_3:
                    switch (reinterpret_cast<InstructionShiftRightImmWSet *>(inst)->get_funct_shift()) {
                        case SRLIWInst::FUNC_SHIFT:
                            return static_cast<T *>(this)->visit_srliw_inst(reinterpret_cast<SRLIWInst *>(inst));
                        case SRAIWInst::FUNC_SHIFT:
                            return static_cast<T *>(this)->visit_sraiw_inst(reinterpret_cast<SRAIWInst *>(inst));
                        default:
                            return static_cast<T *>(this)->illegal_instruction(inst);
                    }
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }
        }

        RetT visit_arith_reg_w_set(InstructionArithRegWSet *inst) {
            switch (inst->get_funct7()) {
                case InstructionIntegerRegWSet::FUNC_7:
                    switch (inst->get_funct3()) {
                        case ADDWInst::FUNC_3:
                            return static_cast<T *>(this)->visit_addw_inst(reinterpret_cast<ADDWInst *>(inst));
                        case SLLWInst::FUNC_3:
                            return static_cast<T *>(this)->visit_sllw_inst(reinterpret_cast<SLLWInst *>(inst));
                        case SRLWInst::FUNC_3:
                            return static_cast<T *>(this)->visit_srlw_inst(reinterpret_cast<SRLWInst *>(inst));
                        default:
                            return static_cast<T *>(this)->illegal_instruction(inst);
                    }
                case InstructionIntegerRegModWSet::FUNC_7:
                    switch (inst->get_funct3()) {
                        case SUBWInst::FUNC_3:
                            return static_cast<T *>(this)->visit_subw_inst(reinterpret_cast<SUBWInst *>(inst));
                        case SRAWInst::FUNC_3:
                            return static_cast<T *>(this)->visit_sraw_inst(reinterpret_cast<SRAWInst *>(inst));
                        default:
                            return static_cast<T *>(this)->illegal_instruction(inst);
                    }
#if defined(__RV_EXTENSION_M__)
                case InstructionMulDivWSet::FUNC_7:
                    switch (inst->get_funct3()) {
                        case MULWInst::FUNCT_3:
                            return static_cast<T *>(this)->visit_mulw_inst(reinterpret_cast<MULWInst *>(inst));
                        case DIVWInst::FUNCT_3:
                            return static_cast<T *>(this)->visit_divw_inst(reinterpret_cast<DIVWInst *>(inst));
                        case DIVUWInst::FUNCT_3:
                            return static_cast<T *>(this)->visit_divuw_inst(reinterpret_cast<DIVUWInst *>(inst));
                        case REMWInst::FUNCT_3:
                            return static_cast<T *>(this)->visit_remw_inst(reinterpret_cast<REMWInst *>(inst));
                        case REMUWInst::FUNCT_3:
                            return static_cast<T *>(this)->visit_remuw_inst(reinterpret_cast<REMUWInst *>(inst));
                        default:
                            return static_cast<T *>(this)->illegal_instruction(inst);
                    }
#endif // defined(__RV_EXTENSION_M__)
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }
        }

#endif // defined(__RV_64_BIT__)

        RetT illegal_instruction(Instruction *inst) {
            risc_v_isa_unreachable("Illegal instruction met!");
        }

        RetT visit_inst(Instruction *inst) {
            risc_v_isa_unreachable("Uncaught instruction in visitor definition!");
        }

        void end_inst(Instruction *inst) {}

#if defined(__RV_EXTENSION_C__)

        RetT visit_16_inst(Instruction16 *inst) { return static_cast<T *>(this)->visit_inst(); }

#endif // defined(__RV_EXTENSION_C__)

        RetT visit_32_inst(Instruction32 *inst) { return static_cast<T *>(this)->visit_inst(inst); }

        RetT visit_branch_set_inst(InstructionBranchSet *inst) { return static_cast<T *>(this)->visit_32b_inst(inst); }

        RetT visit_load_set_inst(InstructionLoadSet *inst) { return static_cast<T *>(this)->visit_32i_inst(inst); }

        RetT visit_store_set_inst(InstructionStoreSet *inst) { return static_cast<T *>(this)->visit_32s_inst(inst); }

        RetT visit_arith_imm_set_inst(InstructionArithImmSet *inst) {
            return static_cast<T *>(this)->visit_32i_inst(inst);
        }

        RetT visit_arith_reg_set_inst(InstructionArithRegSet *inst) {
            return static_cast<T *>(this)->visit_32r_inst(inst);
        }

        RetT visit_fence_set_inst(InstructionFenceSet *inst) { return static_cast<T *>(this)->visit_32i_inst(inst); }

        RetT visit_system_set_inst(InstructionSystemSet *inst) { return static_cast<T *>(this)->visit_32i_inst(inst); }

        RetT visit_32r_inst(Instruction32R *inst) { return static_cast<T *>(this)->visit_32_inst(inst); }

        RetT visit_32i_inst(Instruction32I *inst) { return static_cast<T *>(this)->visit_32_inst(inst); }

        RetT visit_32s_inst(Instruction32S *inst) { return static_cast<T *>(this)->visit_32_inst(inst); }

        RetT visit_32b_inst(Instruction32B *inst) { return static_cast<T *>(this)->visit_32_inst(inst); }

        RetT visit_32u_inst(Instruction32U *inst) { return static_cast<T *>(this)->visit_32_inst(inst); }

        RetT visit_32j_inst(Instruction32J *inst) { return static_cast<T *>(this)->visit_32_inst(inst); }

        RetT visit_lui_inst(LUIInst *inst) { return static_cast<T *>(this)->visit_32u_inst(inst); }

        RetT visit_auipc_inst(AUIPCInst *inst) { return static_cast<T *>(this)->visit_32u_inst(inst); }

        RetT visit_jal_inst(JALInst *inst) { return static_cast<T *>(this)->visit_32j_inst(inst); }

        RetT visit_jalr_inst(JALRInst *inst) { return static_cast<T *>(this)->visit_32i_inst(inst); }

        RetT visit_beq_inst(BEQInst *inst) { return static_cast<T *>(this)->visit_branch_set_inst(inst); }

        RetT visit_bne_inst(BNEInst *inst) { return static_cast<T *>(this)->visit_branch_set_inst(inst); }

        RetT visit_blt_inst(BLTInst *inst) { return static_cast<T *>(this)->visit_branch_set_inst(inst); }

        RetT visit_bge_inst(BGEInst *inst) { return static_cast<T *>(this)->visit_branch_set_inst(inst); }

        RetT visit_bltu_inst(BLTUInst *inst) { return static_cast<T *>(this)->visit_branch_set_inst(inst); }

        RetT visit_bgeu_inst(BGEUInst *inst) { return static_cast<T *>(this)->visit_branch_set_inst(inst); }

        RetT visit_lb_inst(LBInst *inst) { return static_cast<T *>(this)->visit_load_set_inst(inst); }

        RetT visit_lh_inst(LHInst *inst) { return static_cast<T *>(this)->visit_load_set_inst(inst); }

        RetT visit_lw_inst(LWInst *inst) { return static_cast<T *>(this)->visit_load_set_inst(inst); }

        RetT visit_lbu_inst(LBUInst *inst) { return static_cast<T *>(this)->visit_load_set_inst(inst); }

        RetT visit_lhu_inst(LHUInst *inst) { return static_cast<T *>(this)->visit_load_set_inst(inst); }

        RetT visit_sb_inst(SBInst *inst) { return static_cast<T *>(this)->visit_store_set_inst(inst); }

        RetT visit_sh_inst(SHInst *inst) { return static_cast<T *>(this)->visit_store_set_inst(inst); }

        RetT visit_sw_inst(SWInst *inst) { return static_cast<T *>(this)->visit_store_set_inst(inst); }

        RetT visit_addi_inst(ADDIInst *inst) { return static_cast<T *>(this)->visit_arith_imm_set_inst(inst); }

        RetT visit_slti_inst(SLTIInst *inst) { return static_cast<T *>(this)->visit_arith_imm_set_inst(inst); }

        RetT visit_sltiu_inst(SLTIUInst *inst) { return static_cast<T *>(this)->visit_arith_imm_set_inst(inst); }

        RetT visit_xori_inst(XORIInst *inst) { return static_cast<T *>(this)->visit_arith_imm_set_inst(inst); }

        RetT visit_ori_inst(ORIInst *inst) { return static_cast<T *>(this)->visit_arith_imm_set_inst(inst); }

        RetT visit_andi_inst(ANDIInst *inst) { return static_cast<T *>(this)->visit_arith_imm_set_inst(inst); }

        RetT visit_slli_inst(SLLIInst *inst) { return static_cast<T *>(this)->visit_arith_imm_set_inst(inst); }

        RetT visit_srli_inst(SRLIInst *inst) { return static_cast<T *>(this)->visit_arith_imm_set_inst(inst); }

        RetT visit_srai_inst(SRAIInst *inst) { return static_cast<T *>(this)->visit_arith_imm_set_inst(inst); }

        RetT visit_add_inst(ADDInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_sub_inst(SUBInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_sll_inst(SLLInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_slt_inst(SLTInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_sltu_inst(SLTUInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_xor_inst(XORInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_srl_inst(SRLInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_sra_inst(SRAInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_or_inst(ORInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_and_inst(ANDInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_fence_inst(FENCEInst *inst) { return static_cast<T *>(this)->visit_fence_set_inst(inst); }

        RetT visit_ecall_inst(ECALLInst *inst) { return static_cast<T *>(this)->visit_system_set_inst(inst); }

        RetT visit_ebreak_inst(EBREAKInst *inst) { return static_cast<T *>(this)->visit_system_set_inst(inst); }

#if defined(__RV_EXTENSION_M__)

        RetT visit_mul_inst(MULInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_mulh_inst(MULHInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_mulhsu_inst(MULHSUInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_mulhu_inst(MULHUInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_div_inst(DIVInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_divu_inst(DIVUInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_rem_inst(REMInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_remu_inst(REMUInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

#endif // defined(__RV_EXTENSION_M__)
#if defined(__RV_64_BIT__)

        RetT visit_arith_imm_w_set_inst(InstructionArithImmWSet *inst) {
            return static_cast<T *>(this)->visit_32i_inst(inst);
        }

        RetT visit_arith_reg_w_set_inst(InstructionArithRegWSet *inst) {
            return static_cast<T *>(this)->visit_32r_inst(inst);
        }

        RetT visit_ld_inst(LDInst *inst) { return static_cast<T *>(this)->visit_load_set_inst(inst); }

        RetT visit_lwu_inst(LWUInst *inst) { return static_cast<T *>(this)->visit_load_set_inst(inst); }

        RetT visit_sd_inst(SDInst *inst) { return static_cast<T *>(this)->visit_store_set_inst(inst); }

        RetT visit_addiw_inst(ADDIWInst *inst) { return static_cast<T *>(this)->visit_arith_imm_w_set_inst(inst); }

        RetT visit_slliw_inst(SLLIWInst *inst) { return static_cast<T *>(this)->visit_arith_imm_w_set_inst(inst); }

        RetT visit_srliw_inst(SRLIWInst *inst) { return static_cast<T *>(this)->visit_arith_imm_w_set_inst(inst); }

        RetT visit_sraiw_inst(SRAIWInst *inst) { return static_cast<T *>(this)->visit_arith_imm_w_set_inst(inst); }

        RetT visit_addw_inst(ADDWInst *inst) { return static_cast<T *>(this)->visit_arith_reg_w_set_inst(inst); }

        RetT visit_subw_inst(SUBWInst *inst) { return static_cast<T *>(this)->visit_arith_reg_w_set_inst(inst); }

        RetT visit_sllw_inst(SLLWInst *inst) { return static_cast<T *>(this)->visit_arith_reg_w_set_inst(inst); }

        RetT visit_srlw_inst(SRLWInst *inst) { return static_cast<T *>(this)->visit_arith_reg_w_set_inst(inst); }

        RetT visit_sraw_inst(SRAWInst *inst) { return static_cast<T *>(this)->visit_arith_reg_w_set_inst(inst); }

#if defined(__RV_EXTENSION_M__)

        RetT visit_mulw_inst(MULWInst *inst) { return static_cast<T *>(this)->visit_arith_reg_w_set_inst(inst); }

        RetT visit_divw_inst(DIVWInst *inst) { return static_cast<T *>(this)->visit_arith_reg_w_set_inst(inst); }

        RetT visit_divuw_inst(DIVUWInst *inst) { return static_cast<T *>(this)->visit_arith_reg_w_set_inst(inst); }

        RetT visit_remw_inst(REMWInst *inst) { return static_cast<T *>(this)->visit_arith_reg_w_set_inst(inst); }

        RetT visit_remuw_inst(REMUWInst *inst) { return static_cast<T *>(this)->visit_arith_reg_w_set_inst(inst); }

#endif // defined(__RV_EXTENSION_M__)
#endif // defined(__RV_64_BIT__)
#if defined(__RV_EXTENSION_ZIFENCEI__)

        RetT visit_fencei_inst(FENCEIInst *inst) { return static_cast<T *>(this)->visit_fence_set_inst(inst); }

#endif // defined(__RV_EXTENSION_ZIFENCEI__)
#if defined(__RV_EXTENSION_ZICSR__)

        RetT visit_csrrw_inst(CSRRWInst *inst) { return static_cast<T *>(this)->visit_system_set_inst(inst); }

        RetT visit_csrrs_inst(CSRRSInst *inst) { return static_cast<T *>(this)->visit_system_set_inst(inst); }

        RetT visit_csrrc_inst(CSRRCInst *inst) { return static_cast<T *>(this)->visit_system_set_inst(inst); }

        RetT visit_csrrwi_inst(CSRRWIInst *inst) { return static_cast<T *>(this)->visit_system_set_inst(inst); }

        RetT visit_csrrsi_inst(CSRRSIInst *inst) { return static_cast<T *>(this)->visit_system_set_inst(inst); }

        RetT visit_csrrci_inst(CSRRCIInst *inst) { return static_cast<T *>(this)->visit_system_set_inst(inst); }

#endif // defined(__RV_EXTENSION_ZICSR__)
#if defined(__RV_CUSTOM_0__)

        RetT visit_custom_0_inst(InstructionCustome0 *inst) { return static_cast<T *>(this)->visit_32_inst(inst); }

#endif // defined(__RV_CUSTOM_0__)
#if defined(__RV_CUSTOM_1__)

        RetT visit_custom_1_inst(InstructionCustome1 *inst) { return static_cast<T *>(this)->visit_32_inst(inst); }

#endif // defined(__RV_CUSTOM_1__)
#if defined(__RV_CUSTOM_2__)

        RetT visit_custom_2_inst(InstructionCustome2 *inst) { return static_cast<T *>(this)->visit_32_inst(inst); }

#endif // defined(__RV_CUSTOM_2__)
#if defined(__RV_CUSTOM_3__)

        RetT visit_custom_3_inst(InstructionCustome3 *inst) { return static_cast<T *>(this)->visit_32_inst(inst); }

#endif // defined(__RV_CUSTOM_3__)
    };
}


#endif //RISC_V_ISA_INSTRUCTION_VISITOR_HPP
