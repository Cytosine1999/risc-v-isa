#ifndef RISC_V_ISA_INSTRUCTION_VISITOR_HPP
#define RISC_V_ISA_INSTRUCTION_VISITOR_HPP


#include <iostream>

#include "utility.hpp"
#include "instruction.hpp"

#if defined(__RV32I__) || defined(__RV64I__) || defined(__RV128I__)

#include "rv32i.hpp"

#endif // defined(__RV32I__) || defined(__RV64I__) || defined(__RV128I__)

namespace risc_v_isa {
    template<typename T, typename _RetT = void>
    class InstructionVisitor {
    public:
        using RetT = _RetT;

        RetT visit(Instruction *self) {
            static_assert(std::is_base_of<InstructionVisitor, T>::value);

            usize val = *reinterpret_cast<u16 *>(self);

            RetT ret{};

            while (true) {
#if defined(__RVC__)
                if ((val & BITS_MASK<u16, 2, 0>) != BITS_MASK<u16, 2, 0>) {
                    ret = visit_32(reinterpret_cast<Instruction16 *>(self));
                    break;
                }
#endif
#if defined(__RV32I__) || defined(__RV64I__) || defined(__RV128I__)
                if ((val & BITS_MASK<u16, 5, 2>) != BITS_MASK<u16, 5, 2>) {
                    ret = visit_32(reinterpret_cast<Instruction32 *>(self));
                    break;
                }
#endif
                return static_cast<T *>(this)->illegal_instruction(self);
            }

            end_inst(self);

            return ret;
        }

        RetT illegal_instruction(Instruction *self) {
            std::cerr << "Illegal instruction at " << self << std::endl;

            abort();
        }

        RetT visit_inst(Instruction *self) { return; }

        void end_inst(Instruction *self) {}

#if defined(__RVC__)
        RetT visit_16(InstructionBranchSet *self) {
            return;
        }
#endif // defined(__RVC__)

        RetT visit_32(Instruction32 *self) {
            RetT ret{};

            switch (self->get_op_code()) {
#if defined(__RV32I__) || defined(__RV64I__) || defined(__RV128I__)
                case LUIInst::OP_CODE:
                    ret = visit_lui_inst(reinterpret_cast<LUIInst *>(self));
                    break;
                case AUIPCInst::OP_CODE:
                    ret = visit_auipc_inst(reinterpret_cast<AUIPCInst *>(self));
                    break;
                case JALInst::OP_CODE:
                    ret = visit_jal_inst(reinterpret_cast<JALInst *>(self));
                    break;
                case JALRInst::OP_CODE:
                    if (reinterpret_cast<JALRInst *>(self)->get_funct3() != JALRInst::FUNC_3)
                        return static_cast<T *>(this)->illegal_instruction(self);
                    ret = visit_jalr_inst(reinterpret_cast<JALRInst *>(self));
                    break;
                case InstructionBranchSet::OP_CODE:
                    ret = visit_branch_set(reinterpret_cast<InstructionBranchSet *>(self));
                    break;
                case InstructionLoadSet::OP_CODE:
                    ret = visit_load_set(reinterpret_cast<InstructionLoadSet *>(self));
                    break;
                case InstructionStoreSet::OP_CODE:
                    ret = visit_store_set(reinterpret_cast<InstructionStoreSet *>(self));
                    break;
                case InstructionArithImmSet::OP_CODE:
                    ret = visit_arith_imm_set(reinterpret_cast<InstructionArithImmSet *>(self));
                    break;
                case InstructionArithRegSet::OP_CODE:
                    ret = visit_arith_reg_set(reinterpret_cast<InstructionArithRegSet *>(self));
                    break;
                case InstructionFenceSet::OP_CODE:
                    if (reinterpret_cast<InstructionFenceSet *>(self)->get_unused() != 0)
                        return static_cast<T *>(this)->illegal_instruction(self);
                    ret = visit_fence_set(reinterpret_cast<InstructionFenceSet *>(self));
                    break;
                case InstructionSystemSet::OP_CODE:
                    ret = visit_system_set(reinterpret_cast<InstructionSystemSet *>(self));
                    break;
#endif // defined(__RV32I__) || defined(__RV64I__) || defined(__RV128I__)
#if defined(__RVCUSTOM0__)
                case InstructionCustome0::OP_CODE:
                    ret = visit_custom_0_inst(reinterpret_cast<InstructionCustome0 *>(self));
                    break;
#endif // defined(__RVCUSTOM0__)
#if defined(__RVCUSTOM1__)
                case InstructionCustome1::OP_CODE:
                    ret = visit_custom_1_inst(reinterpret_cast<InstructionCustome0 *>(self));
                    break;
#endif // defined(__RVCUSTOM1__)
#if defined(__RVCUSTOM2__)
                case InstructionCustome2::OP_CODE:
                    ret = visit_custom_2_inst(reinterpret_cast<InstructionCustome0 *>(self));
                    break;
#endif // defined(__RVCUSTOM2__)
#if defined(__RVCUSTOM3__)
                case InstructionCustome3::OP_CODE:
                    ret = visit_custom_3_inst(reinterpret_cast<InstructionCustome0 *>(self));
                    break;
#endif // defined(__RVCUSTOM3__)
                default:
                    return static_cast<T *>(this)->illegal_instruction(self);
            }

            static_cast<T *>(this)->end_32_inst(self);

            return ret;
        }

#if defined(__RV32I__) || defined(__RV64I__) || defined(__RV128I__) // todo: end action

        RetT visit_branch_set(InstructionBranchSet *self) {
            RetT ret{};

            switch (self->get_funct3()) {
                case BEQInst::FUNC_3:
                    ret = visit_beq_inst(reinterpret_cast<BEQInst *>(self));
                    break;
                case BNEInst::FUNC_3:
                    ret = visit_bne_inst(reinterpret_cast<BNEInst *>(self));
                    break;
                case BLTInst::FUNC_3:
                    ret = visit_blt_inst(reinterpret_cast<BLTInst *>(self));
                    break;
                case BGEInst::FUNC_3:
                    ret = visit_bge_inst(reinterpret_cast<BGEInst *>(self));
                    break;
                case BLTUInst::FUNC_3:
                    ret = visit_bltu_inst(reinterpret_cast<BLTUInst *>(self));
                    break;
                case BGEUInst::FUNC_3:
                    ret = visit_bgeu_inst(reinterpret_cast<BGEUInst *>(self));
                    break;
                default:
                    return static_cast<T *>(this)->illegal_instruction(self);
            }

            static_cast<T *>(this)->end_branch_set_inst(self);

            return ret;
        }

        RetT visit_load_set(InstructionLoadSet *self) {
            switch (self->get_funct3()) {
                case LBInst::FUNC_3:
                    return visit_lb_inst(reinterpret_cast<LBInst *>(self));
                case LHInst::FUNC_3:
                    return visit_lh_inst(reinterpret_cast<LHInst *>(self));
                case LWInst::FUNC_3:
                    return visit_lw_inst(reinterpret_cast<LWInst *>(self));
                case LBUInst::FUNC_3:
                    return visit_lbu_inst(reinterpret_cast<LBUInst *>(self));
                case LHUInst::FUNC_3:
                    return visit_lhu_inst(reinterpret_cast<LHUInst *>(self));
                default:
                    return static_cast<T *>(this)->illegal_instruction(self);
            }
        }

        RetT visit_store_set(InstructionStoreSet *self) {
            switch (self->get_funct3()) {
                case SBInst::FUNC_3:
                    return visit_sb_inst(reinterpret_cast<SBInst *>(self));
                case SHInst::FUNC_3:
                    return visit_sh_inst(reinterpret_cast<SHInst *>(self));
                case SWInst::FUNC_3:
                    return visit_sw_inst(reinterpret_cast<SWInst *>(self));
                default:
                    return static_cast<T *>(this)->illegal_instruction(self);
            }
        }

        RetT visit_arith_imm_set(InstructionArithImmSet *self) {
            switch (self->get_funct3()) {
                case ADDIInst::FUNC_3:
                    return visit_addi_inst(reinterpret_cast<ADDIInst *>(self));
                case SLTIInst::FUNC_3:
                    return visit_slti_inst(reinterpret_cast<SLTIInst *>(self));
                case SLTIUInst::FUNC_3:
                    return visit_sltiu_inst(reinterpret_cast<SLTIUInst *>(self));
                case XORIInst::FUNC_3:
                    return visit_xori_inst(reinterpret_cast<XORIInst *>(self));
                case ORIInst::FUNC_3:
                    return visit_ori_inst(reinterpret_cast<ORIInst *>(self));
                case ANDIInst::FUNC_3:
                    return visit_andi_inst(reinterpret_cast<ANDIInst *>(self));
                case SLLIWInst::FUNC_3:
                    return visit_slli_inst(reinterpret_cast<SLLIWInst *>(self));
                case InstructionSrliwSraiw::FUNC_3:
                    return visit_srli_srai(reinterpret_cast<InstructionSrliwSraiw *>(self));
                default:
                    return static_cast<T *>(this)->illegal_instruction(self);
            }
        }

        RetT visit_srli_srai(InstructionSrliwSraiw *self) {
            switch (self->get_funct_shift()) {
                case SRLIWInst::FUNC_SHIFT:
                    return visit_srli_inst(reinterpret_cast<SRLIWInst *>(self));
                case SRAIWInst::FUNC_SHIFT:
                    return visit_srai_inst(reinterpret_cast<SRAIWInst *>(self));
                default:
                    return static_cast<T *>(this)->illegal_instruction(self);
            }
        }

        RetT visit_arith_reg_set(InstructionArithRegSet *self) {
            switch (self->get_funct3()) {
                case InstructionAddSub::FUNC_3:
                    return visit_add_sub(reinterpret_cast<InstructionAddSub *>(self));
                case SLLWInst::FUNC_3:
                    if (reinterpret_cast<SLLWInst *>(self)->get_funct7() != SLLWInst::FUNC_7)
                        return static_cast<T *>(this)->illegal_instruction(self);
                    return visit_sll_inst(reinterpret_cast<SLLWInst *>(self));
                case SLTInst::FUNC_3:
                    if (reinterpret_cast<SLTInst *>(self)->get_funct7() != SLTInst::FUNC_7)
                        return static_cast<T *>(this)->illegal_instruction(self);
                    return visit_slt_inst(reinterpret_cast<SLTInst *>(self));
                case SLTUInst::FUNC_3:
                    if (reinterpret_cast<SLTUInst *>(self)->get_funct7() != SLTUInst::FUNC_7)
                        return static_cast<T *>(this)->illegal_instruction(self);
                    return visit_sltu_inst(reinterpret_cast<SLTUInst *>(self));
                case XORInst::FUNC_3:
                    if (reinterpret_cast<XORInst *>(self)->get_funct7() != XORInst::FUNC_7)
                        return static_cast<T *>(this)->illegal_instruction(self);
                    return visit_xor_inst(reinterpret_cast<XORInst *>(self));
                case InstructionSrlwSraw::FUNC_3:
                    return visit_srl_sra(reinterpret_cast<InstructionSrlwSraw *>(self));
                case ORInst::FUNC_3:
                    if (reinterpret_cast<ORInst *>(self)->get_funct7() != ORInst::FUNC_7)
                        return static_cast<T *>(this)->illegal_instruction(self);
                    return visit_or_inst(reinterpret_cast<ORInst *>(self));
                case ANDInst::FUNC_3:
                    if (reinterpret_cast<ANDInst *>(self)->get_funct7() != ANDInst::FUNC_7)
                        return static_cast<T *>(this)->illegal_instruction(self);
                    return visit_and_inst(reinterpret_cast<ANDInst *>(self));
                default:
                    return static_cast<T *>(this)->illegal_instruction(self);
            }
        }

        RetT visit_add_sub(InstructionAddSub *self) {
            switch (self->get_funct7()) {
                case ADDInst::FUNC_7:
                    return visit_add_inst(reinterpret_cast<ADDInst *>(self));
                case SUBInst::FUNC_7:
                    return visit_sub_inst(reinterpret_cast<SUBInst *>(self));
                default:
                    return static_cast<T *>(this)->illegal_instruction(self);
            }
        }

        RetT visit_srl_sra(InstructionSrlwSraw *self) {
            switch (self->get_funct7()) {
                case SRLWInst::FUNC_7:
                    return visit_srl_inst(reinterpret_cast<SRLWInst *>(self));
                case SRAWInst::FUNC_7:
                    return visit_sra_inst(reinterpret_cast<SRAWInst *>(self));
                default:
                    return static_cast<T *>(this)->illegal_instruction(self);
            }
        }

        RetT visit_fence_set(InstructionFenceSet *self) {
            switch (self->get_funct3()) {
                case FENCEInst::FUNC_3:
                    if (reinterpret_cast<FENCEInst *>(self)->get_unused() != 0)
                        return static_cast<T *>(this)->illegal_instruction(self);
                    return visit_fence_inst(reinterpret_cast<FENCEInst *>(self));
                case FENCEIInst::FUNC_3:
                    if (reinterpret_cast<FENCEIInst *>(self)->get_unused() != 0)
                        return static_cast<T *>(this)->illegal_instruction(self);
                    return visit_fencei_inst(reinterpret_cast<FENCEIInst *>(self));
                default:
                    return static_cast<T *>(this)->illegal_instruction(self);
            }
        }

        RetT visit_system_set(InstructionSystemSet *self) {
            switch (self->get_funct3()) {
                case InstructionEnvironmentSet::FUNC_3:
                    if (reinterpret_cast<InstructionEnvironmentSet *>(self)->get_unused() != 0)
                        return static_cast<T *>(this)->illegal_instruction(self);
                    return visit_environment_set(reinterpret_cast<InstructionEnvironmentSet *>(self));
                case CSRRWInst::FUNC_3:
                    return visit_csrrw_inst(reinterpret_cast<CSRRWInst *>(self));
                case CSRRSInst::FUNC_3:
                    return visit_csrrs_inst(reinterpret_cast<CSRRSInst *>(self));
                case CSRRCInst::FUNC_3:
                    return visit_csrrc_inst(reinterpret_cast<CSRRCInst *>(self));
                case CSRRWIInst::FUNC_3:
                    return visit_csrrwi_inst(reinterpret_cast<CSRRWIInst *>(self));
                case CSRRSIInst::FUNC_3:
                    return visit_csrrsi_inst(reinterpret_cast<CSRRSIInst *>(self));
                case CSRRCIInst::FUNC_3:
                    return visit_csrrci_inst(reinterpret_cast<CSRRCIInst *>(self));
                default:
                    return static_cast<T *>(this)->illegal_instruction(self);
            }
        }

        RetT visit_environment_set(InstructionEnvironmentSet *self) {
            switch (self->get_funct_environment()) {
                case ECALLInst::FUNCT_ENVIRONMENT:
                    return visit_ecall_inst(reinterpret_cast<ECALLInst *>(self));
                case EBREAKInst::FUNCT_ENVIRONMENT:
                    return visit_ebreak_inst(reinterpret_cast<EBREAKInst *>(self));
                default:
                    return static_cast<T *>(this)->illegal_instruction(self);
            }
        }

#endif // defined(__RV32I__) || defined(__RV64I__) || defined(__RV128I__)
#if defined(__RVC__)
        RetT visit_16_inst(Instruction16 *self) { return static_cast<T *>(this)->visit_inst(); }
#endif // defined(__RVC__)

        RetT visit_32_inst(Instruction32 *self) { return static_cast<T *>(this)->visit_inst(self); }

        void end_32_inst(Instruction32 *self) {}

#if defined(__RV32I__) || defined(__RV64I__) || defined(__RV128I__)

        RetT visit_lui_inst(LUIInst *self) { return static_cast<T *>(this)->visit_32_inst(self); }

        RetT visit_auipc_inst(AUIPCInst *self) { return static_cast<T *>(this)->visit_32_inst(self); }

        RetT visit_jal_inst(JALInst *self) { return static_cast<T *>(this)->visit_32_inst(self); }

        RetT visit_jalr_inst(JALRInst *self) { return static_cast<T *>(this)->visit_32_inst(self); }

        RetT visit_branch_set_inst(InstructionBranchSet *self) { return static_cast<T *>(this)->visit_32_inst(self); }

        void end_branch_set_inst(InstructionBranchSet *self) {}

        RetT visit_beq_inst(BEQInst *self) { return static_cast<T *>(this)->visit_branch_set_inst(self); }

        RetT visit_bne_inst(BNEInst *self) { return static_cast<T *>(this)->visit_branch_set_inst(self); }

        RetT visit_blt_inst(BLTInst *self) { return static_cast<T *>(this)->visit_branch_set_inst(self); }

        RetT visit_bge_inst(BGEInst *self) { return static_cast<T *>(this)->visit_branch_set_inst(self); }

        RetT visit_bltu_inst(BLTUInst *self) { return static_cast<T *>(this)->visit_branch_set_inst(self); }

        RetT visit_bgeu_inst(BGEUInst *self) { return static_cast<T *>(this)->visit_branch_set_inst(self); }

        RetT visit_load_set_inst(InstructionLoadSet *self) { return static_cast<T *>(this)->visit_32_inst(self); }

        void end_load_set_inst(InstructionLoadSet *self) {}

        RetT visit_lb_inst(LBInst *self) { return static_cast<T *>(this)->visit_load_set_inst(self); }

        RetT visit_lh_inst(LHInst *self) { return static_cast<T *>(this)->visit_load_set_inst(self); }

        RetT visit_lw_inst(LWInst *self) { return static_cast<T *>(this)->visit_load_set_inst(self); }

        RetT visit_lbu_inst(LBUInst *self) { return static_cast<T *>(this)->visit_load_set_inst(self); }

        RetT visit_lhu_inst(LHUInst *self) { return static_cast<T *>(this)->visit_load_set_inst(self); }

        RetT visit_store_set_inst(InstructionStoreSet *self) { return static_cast<T *>(this)->visit_32_inst(self); }

        void end_store_set_inst(InstructionStoreSet *self) {}

        RetT visit_sb_inst(SBInst *self) { return static_cast<T *>(this)->visit_store_set_inst(self); }

        RetT visit_sh_inst(SHInst *self) { return static_cast<T *>(this)->visit_store_set_inst(self); }

        RetT visit_sw_inst(SWInst *self) { return static_cast<T *>(this)->visit_store_set_inst(self); }

        RetT visit_arith_imm_set_inst(InstructionArithImmSet *self) {
            return static_cast<T *>(this)->visit_32_inst(self);
        }

        void end_arith_imm_set_inst(InstructionArithImmSet *self) {}

        RetT visit_addi_inst(ADDIInst *self) { return static_cast<T *>(this)->visit_arith_imm_set_inst(self); }

        RetT visit_slti_inst(SLTIInst *self) { return static_cast<T *>(this)->visit_arith_imm_set_inst(self); }

        RetT visit_sltiu_inst(SLTIUInst *self) { return static_cast<T *>(this)->visit_arith_imm_set_inst(self); }

        RetT visit_xori_inst(XORIInst *self) { return static_cast<T *>(this)->visit_arith_imm_set_inst(self); }

        RetT visit_ori_inst(ORIInst *self) { return static_cast<T *>(this)->visit_arith_imm_set_inst(self); }

        RetT visit_andi_inst(ANDIInst *self) { return static_cast<T *>(this)->visit_arith_imm_set_inst(self); }

        RetT visit_slli_inst(SLLIWInst *self) { return static_cast<T *>(this)->visit_arith_imm_set_inst(self); }

        RetT visit_srli_srai_inst(InstructionSrliwSraiw *self) {
            return static_cast<T *>(this)->visit_arith_imm_set_inst(self);
        }

        void end_srli_srai_inst(InstructionSrliwSraiw *self) {}

        RetT visit_srli_inst(SRLIWInst *self) { return static_cast<T *>(this)->visit_srli_srai_inst(self); }

        RetT visit_srai_inst(SRAIWInst *self) { return static_cast<T *>(this)->visit_srli_srai_inst(self); }

        RetT visit_arith_reg_set_inst(InstructionArithRegSet *self) {
            return static_cast<T *>(this)->visit_32_inst(self);
        }

        void end_arith_reg_set_inst(InstructionArithRegSet *self) {}

        RetT visit_add_inst(ADDInst *self) { return static_cast<T *>(this)->visit_arith_reg_set_inst(self); }

        RetT visit_sub_inst(SUBInst *self) { return static_cast<T *>(this)->visit_arith_reg_set_inst(self); }

        RetT visit_sll_inst(SLLWInst *self) { return static_cast<T *>(this)->visit_arith_reg_set_inst(self); }

        RetT visit_slt_inst(SLTInst *self) { return static_cast<T *>(this)->visit_arith_reg_set_inst(self); }

        RetT visit_sltu_inst(SLTUInst *self) { return static_cast<T *>(this)->visit_arith_reg_set_inst(self); }

        RetT visit_xor_inst(XORInst *self) { return static_cast<T *>(this)->visit_arith_reg_set_inst(self); }

        RetT visit_srl_sra_inst(InstructionSrlwSraw *self) {
            return static_cast<T *>(this)->visit_arith_reg_set_inst(self);
        }

        void end_srl_sra_inst(InstructionSrlwSraw *self) {}

        RetT visit_srl_inst(SRLWInst *self) { return static_cast<T *>(this)->visit_srl_sra_inst(self); }

        RetT visit_sra_inst(SRAWInst *self) { return static_cast<T *>(this)->visit_srl_sra_inst(self); }

        RetT visit_or_inst(ORInst *self) { return static_cast<T *>(this)->visit_arith_reg_set_inst(self); }

        RetT visit_and_inst(ANDInst *self) { return static_cast<T *>(this)->visit_arith_reg_set_inst(self); }

        RetT visit_fence_set_inst(InstructionFenceSet *self) { return static_cast<T *>(this)->visit_32_inst(self); }

        void end_fence_set_inst(InstructionFenceSet *self) {}

        RetT visit_fence_inst(FENCEInst *self) { return static_cast<T *>(this)->visit_fence_set_inst(self); }

        RetT visit_fencei_inst(FENCEIInst *self) { return static_cast<T *>(this)->visit_fence_set_inst(self); }

        RetT visit_system_set_inst(InstructionSystemSet *self) { return static_cast<T *>(this)->visit_32_inst(self); }

        void end_system_set_inst(InstructionSystemSet *self) {}

        RetT visit_environment_set_inst(InstructionEnvironmentSet *self) {
            return static_cast<T *>(this)->visit_system_set_inst(self);
        }

        void end_environment_set_inst(InstructionEnvironmentSet *self) {}

        RetT visit_ecall_inst(ECALLInst *self) { return static_cast<T *>(this)->visit_environment_set_inst(self); }

        RetT visit_ebreak_inst(EBREAKInst *self) { return static_cast<T *>(this)->visit_environment_set_inst(self); }

        RetT visit_csrrw_inst(CSRRWInst *self) { return static_cast<T *>(this)->visit_system_set_inst(self); }

        RetT visit_csrrs_inst(CSRRSInst *self) { return static_cast<T *>(this)->visit_system_set_inst(self); }

        RetT visit_csrrc_inst(CSRRCInst *self) { return static_cast<T *>(this)->visit_system_set_inst(self); }

        RetT visit_csrrwi_inst(CSRRWIInst *self) { return static_cast<T *>(this)->visit_system_set_inst(self); }

        RetT visit_csrrsi_inst(CSRRSIInst *self) { return static_cast<T *>(this)->visit_system_set_inst(self); }

        RetT visit_csrrci_inst(CSRRCIInst *self) { return static_cast<T *>(this)->visit_system_set_inst(self); }

#endif // defined(__RV32I__) || defined(__RV64I__) || defined(__RV128I__)
#if defined(__RVCUSTOM0__)
        RetT visit_custom_0_inst(InstructionCustome0 *self) { return static_cast<T *>(this)->visit_32_inst(self); }
#endif // defined(__RVCUSTOM0__)
#if defined(__RVCUSTOM1__)
        RetT visit_custom_1_inst(InstructionCustome1 *self) { return static_cast<T *>(this)->visit_32_inst(self); }
#endif // defined(__RVCUSTOM1__)
#if defined(__RVCUSTOM2__)
        RetT visit_custom_2_inst(InstructionCustome2 *self) { return static_cast<T *>(this)->visit_32_inst(self); }
#endif // defined(__RVCUSTOM2__)
#if defined(__RVCUSTOM3__)
        RetT visit_custom_3_inst(InstructionCustome3 *self) { return static_cast<T *>(this)->visit_32_inst(self); }
#endif // defined(__RVCUSTOM3__)
    };
}


#endif //RISC_V_ISA_INSTRUCTION_VISITOR_HPP
