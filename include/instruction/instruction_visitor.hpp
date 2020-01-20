#ifndef RISC_V_ISA_INSTRUCTION_VISITOR_HPP
#define RISC_V_ISA_INSTRUCTION_VISITOR_HPP


#include "utility.hpp"
#include "instruction.hpp"
#include "rv32i.hpp"
#include "zifencei.hpp"
#include "zicsr.hpp"
#include "rv32m.hpp"


namespace risc_v_isa {
    template<typename T, typename _RetT = void>
    class InstructionVisitor {
    public:
        using RetT = _RetT;

        RetT visit(Instruction *inst) {
            static_assert(std::is_base_of<InstructionVisitor, T>::value);

            usize val = *reinterpret_cast<u16 *>(inst);

            RetT ret{};

            while (true) {
#if defined(__RVC__)
                if ((val & BITS_MASK<u16, 2, 0>) != BITS_MASK<u16, 2, 0>) {
                    ret = visit_32(reinterpret_cast<Instruction16 *>(inst));
                    break;
                }
#endif
#if defined(__RV32I__) || defined(__RV64I__)
                if ((val & BITS_MASK<u16, 5, 2>) != BITS_MASK<u16, 5, 2>) {
                    ret = visit_32(reinterpret_cast<Instruction32 *>(inst));
                    break;
                }
#endif
                return static_cast<T *>(this)->illegal_instruction(inst);
            }

            end_inst(inst);

            return ret;
        }

#if defined(__RVC__)

        RetT visit_16(InstructionBranchSet *inst) {
            return;
        }

#endif // defined(__RVC__)

#if defined(__RV32I__) || defined(__RV64I__)

        RetT visit_32(Instruction32 *inst) {
            RetT ret{};

            switch (inst->get_op_code()) {
#if defined(__RV32I__) || defined(__RV64I__)
                case LUIInst::OP_CODE:
                    ret = static_cast<T *>(this)->visit_lui_inst(reinterpret_cast<LUIInst *>(inst));
                    break;
                case AUIPCInst::OP_CODE:
                    ret = static_cast<T *>(this)->visit_auipc_inst(reinterpret_cast<AUIPCInst *>(inst));
                    break;
                case JALInst::OP_CODE:
                    ret = static_cast<T *>(this)->visit_jal_inst(reinterpret_cast<JALInst *>(inst));
                    break;
                case JALRInst::OP_CODE:
                    if (reinterpret_cast<JALRInst *>(inst)->get_funct3() != JALRInst::FUNC_3)
                        return static_cast<T *>(this)->illegal_instruction(inst);
                    ret = static_cast<T *>(this)->visit_jalr_inst(reinterpret_cast<JALRInst *>(inst));
                    break;
                case InstructionBranchSet::OP_CODE:
                    ret = visit_branch_set(reinterpret_cast<InstructionBranchSet *>(inst));
                    break;
                case InstructionLoadSet::OP_CODE:
                    ret = visit_load_set(reinterpret_cast<InstructionLoadSet *>(inst));
                    break;
                case InstructionStoreSet::OP_CODE:
                    ret = visit_store_set(reinterpret_cast<InstructionStoreSet *>(inst));
                    break;
                case InstructionArithImmSet::OP_CODE:
                    ret = visit_arith_imm_set(reinterpret_cast<InstructionArithImmSet *>(inst));
                    break;
                case InstructionArithRegSet::OP_CODE:
                    ret = visit_arith_reg_set(reinterpret_cast<InstructionArithRegSet *>(inst));
                    break;
                case InstructionFenceSet::OP_CODE:
                    if (reinterpret_cast<InstructionFenceSet *>(inst)->get_unused() != 0)
                        return static_cast<T *>(this)->illegal_instruction(inst);
                    ret = visit_fence_set(reinterpret_cast<InstructionFenceSet *>(inst));
                    break;
                case InstructionSystemSet::OP_CODE:
                    ret = visit_system_set(reinterpret_cast<InstructionSystemSet *>(inst));
                    break;
#endif // defined(__RV32I__) || defined(__RV64I__)
#if defined(__RVCUSTOM0__)
                case InstructionCustome0::OP_CODE:
                    ret = static_cast<T *>(this)->visit_custom_0_inst(reinterpret_cast<InstructionCustome0 *>(inst));
                    break;
#endif // defined(__RVCUSTOM0__)
#if defined(__RVCUSTOM1__)
                case InstructionCustome1::OP_CODE:
                    ret = static_cast<T *>(this)->visit_custom_1_inst(reinterpret_cast<InstructionCustome0 *>(inst));
                    break;
#endif // defined(__RVCUSTOM1__)
#if defined(__RVCUSTOM2__)
                case InstructionCustome2::OP_CODE:
                    ret = static_cast<T *>(this)->visit_custom_2_inst(reinterpret_cast<InstructionCustome0 *>(inst));
                    break;
#endif // defined(__RVCUSTOM2__)
#if defined(__RVCUSTOM3__)
                case InstructionCustome3::OP_CODE:
                    ret = static_cast<T *>(this)->visit_custom_3_inst(reinterpret_cast<InstructionCustome0 *>(inst));
                    break;
#endif // defined(__RVCUSTOM3__)
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }

            static_cast<T *>(this)->end_32_inst(inst);

            return ret;
        }

        RetT visit_branch_set(InstructionBranchSet *inst) {
            RetT ret{};

            switch (inst->get_funct3()) {
                case BEQInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_beq_inst(reinterpret_cast<BEQInst *>(inst));
                    break;
                case BNEInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_bne_inst(reinterpret_cast<BNEInst *>(inst));
                    break;
                case BLTInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_blt_inst(reinterpret_cast<BLTInst *>(inst));
                    break;
                case BGEInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_bge_inst(reinterpret_cast<BGEInst *>(inst));
                    break;
                case BLTUInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_bltu_inst(reinterpret_cast<BLTUInst *>(inst));
                    break;
                case BGEUInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_bgeu_inst(reinterpret_cast<BGEUInst *>(inst));
                    break;
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }

            static_cast<T *>(this)->end_branch_set_inst(inst);

            return ret;
        }

        RetT visit_load_set(InstructionLoadSet *inst) {
            RetT ret{};

            switch (inst->get_funct3()) {
                case LBInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_lb_inst(reinterpret_cast<LBInst *>(inst));
                    break;
                case LHInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_lh_inst(reinterpret_cast<LHInst *>(inst));
                    break;
                case LWInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_lw_inst(reinterpret_cast<LWInst *>(inst));
                    break;
                case LBUInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_lbu_inst(reinterpret_cast<LBUInst *>(inst));
                    break;
                case LHUInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_lhu_inst(reinterpret_cast<LHUInst *>(inst));
                    break;
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }

            static_cast<T *>(this)->end_load_set_inst(inst);

            return ret;
        }

        RetT visit_store_set(InstructionStoreSet *inst) {
            RetT ret{};

            switch (inst->get_funct3()) {
                case SBInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_sb_inst(reinterpret_cast<SBInst *>(inst));
                    break;
                case SHInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_sh_inst(reinterpret_cast<SHInst *>(inst));
                    break;
                case SWInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_sw_inst(reinterpret_cast<SWInst *>(inst));
                    break;
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }

            static_cast<T *>(this)->end_store_set_inst(inst);

            return ret;
        }

        RetT visit_arith_imm_set(InstructionArithImmSet *inst) {
            RetT ret{};

            switch (inst->get_funct3()) {
                case ADDIInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_addi_inst(reinterpret_cast<ADDIInst *>(inst));
                    break;
                case SLTIInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_slti_inst(reinterpret_cast<SLTIInst *>(inst));
                    break;
                case SLTIUInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_sltiu_inst(reinterpret_cast<SLTIUInst *>(inst));
                    break;
                case XORIInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_xori_inst(reinterpret_cast<XORIInst *>(inst));
                    break;
                case ORIInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_ori_inst(reinterpret_cast<ORIInst *>(inst));
                    break;
                case ANDIInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_andi_inst(reinterpret_cast<ANDIInst *>(inst));
                    break;
                case SLLIWInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_slli_inst(reinterpret_cast<SLLIWInst *>(inst));
                    break;
                case InstructionSrliwSraiw::FUNC_3:
                    switch (reinterpret_cast<InstructionSrliwSraiw *>(inst)->get_funct_shift()) {
                        case SRLIWInst::FUNC_SHIFT:
                            ret = static_cast<T *>(this)->visit_srli_inst(reinterpret_cast<SRLIWInst *>(inst));
                            break;
                        case SRAIWInst::FUNC_SHIFT:
                            ret = static_cast<T *>(this)->visit_srai_inst(reinterpret_cast<SRAIWInst *>(inst));
                            break;
                        default:
                            return static_cast<T *>(this)->illegal_instruction(inst);
                    }
                    break;
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }

            static_cast<T *>(this)->end_arith_imm_set_inst(inst);

            return ret;
        }

        RetT visit_arith_reg_set(InstructionArithRegSet *inst) {
            RetT ret{};

            switch (inst->get_funct7()) {
                case InstructionIntegerRegSet::FUNC_7:
                    switch (inst->get_funct3()) {
                        case ADDInst::FUNC_3:
                            ret = static_cast<T *>(this)->visit_add_inst(reinterpret_cast<ADDInst *>(inst));
                            break;
                        case SLLWInst::FUNC_3:
                            ret = static_cast<T *>(this)->visit_sll_inst(reinterpret_cast<SLLWInst *>(inst));
                            break;
                        case SLTInst::FUNC_3:
                            ret = static_cast<T *>(this)->visit_slt_inst(reinterpret_cast<SLTInst *>(inst));
                            break;
                        case SLTUInst::FUNC_3:
                            ret = static_cast<T *>(this)->visit_sltu_inst(reinterpret_cast<SLTUInst *>(inst));
                            break;
                        case XORInst::FUNC_3:
                            ret = static_cast<T *>(this)->visit_xor_inst(reinterpret_cast<XORInst *>(inst));
                            break;
                        case SRLWInst::FUNC_3:
                            ret = static_cast<T *>(this)->visit_srl_inst(reinterpret_cast<SRLWInst *>(inst));
                            break;
                        case ORInst::FUNC_3:
                            ret = static_cast<T *>(this)->visit_or_inst(reinterpret_cast<ORInst *>(inst));
                            break;
                        case ANDInst::FUNC_3:
                            ret = static_cast<T *>(this)->visit_and_inst(reinterpret_cast<ANDInst *>(inst));
                            break;
                        default:
                            return static_cast<T *>(this)->illegal_instruction(inst);
                    }
                    break;
                case InstructionIntegerRegModSet::FUNC_7:
                    switch (inst->get_funct3()) {
                        case SUBInst::FUNC_3:
                            ret = static_cast<T *>(this)->visit_sub_inst(reinterpret_cast<SUBInst *>(inst));
                            break;
                        case SRAWInst::FUNC_3:
                            ret = static_cast<T *>(this)->visit_sra_inst(reinterpret_cast<SRAWInst *>(inst));
                            break;
                        default:
                            return static_cast<T *>(this)->illegal_instruction(inst);
                    }
                    break;
#if defined(__RV32M__) || defined(__RV64M__)
                case InstructionMulDivSet::FUNC_7:
                    switch (inst->get_funct3()) {
                        case MULInst::FUNCT_3:
                            ret = static_cast<T *>(this)->visit_mul_inst(reinterpret_cast<MULInst *>(inst));
                            break;
                        case MULHInst::FUNCT_3:
                            ret = static_cast<T *>(this)->visit_mulh_inst(reinterpret_cast<MULHInst *>(inst));
                            break;
                        case MULHSUInst::FUNCT_3:
                            ret = static_cast<T *>(this)->visit_mulhsu_inst(reinterpret_cast<MULHSUInst *>(inst));
                            break;
                        case MULHUInst::FUNCT_3:
                            ret = static_cast<T *>(this)->visit_mulhu_inst(reinterpret_cast<MULHUInst *>(inst));
                            break;
                        case DIVInst::FUNCT_3:
                            ret = static_cast<T *>(this)->visit_div_inst(reinterpret_cast<DIVInst *>(inst));
                            break;
                        case DIVUInst::FUNCT_3:
                            ret = static_cast<T *>(this)->visit_divu_inst(reinterpret_cast<DIVUInst *>(inst));
                            break;
                        case REMInst::FUNCT_3:
                            ret = static_cast<T *>(this)->visit_rem_inst(reinterpret_cast<REMInst *>(inst));
                            break;
                        case REMUInst::FUNCT_3:
                            ret = static_cast<T *>(this)->visit_remu_inst(reinterpret_cast<REMUInst *>(inst));
                            break;
                        default:
                            return static_cast<T *>(this)->illegal_instruction(inst);
                    }
                    break;
#endif // defined(__RV32M__) || defined(__RV64M__)
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }

            static_cast<T *>(this)->end_arith_reg_set_inst(inst);

            return ret;
        }

        RetT visit_fence_set(InstructionFenceSet *inst) {
            RetT ret{};

            switch (inst->get_funct3()) {
                case FENCEInst::FUNC_3:
                    if (reinterpret_cast<FENCEInst *>(inst)->get_unused() != 0)
                        return static_cast<T *>(this)->illegal_instruction(inst);
                    ret = static_cast<T *>(this)->visit_fence_inst(reinterpret_cast<FENCEInst *>(inst));
                    break;
#if defined(__RV_ZIFENCEI_EXTENSION__)
                case FENCEIInst::FUNC_3:
                    if (reinterpret_cast<FENCEIInst *>(inst)->get_unused() != 0)
                        return static_cast<T *>(this)->illegal_instruction(inst);
                    ret = static_cast<T *>(this)->visit_fencei_inst(reinterpret_cast<FENCEIInst *>(inst));
                    break;
#endif // defined(__RV_ZIFENCEI_EXTENSION__)
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }

            static_cast<T *>(this)->end_fence_set_inst(inst);

            return ret;
        }

        RetT visit_system_set(InstructionSystemSet *inst) {
            RetT ret{};

            switch (inst->get_funct3()) {
                case InstructionEnvironmentSet::FUNC_3:
                    if (reinterpret_cast<InstructionEnvironmentSet *>(inst)->get_unused() != 0)
                        return static_cast<T *>(this)->illegal_instruction(inst);
                    switch (reinterpret_cast<InstructionEnvironmentSet *>(inst)->get_funct_environment()) {
                        case ECALLInst::FUNCT_ENVIRONMENT:
                            ret = static_cast<T *>(this)->visit_ecall_inst(reinterpret_cast<ECALLInst *>(inst));
                            break;
                        case EBREAKInst::FUNCT_ENVIRONMENT:
                            ret = static_cast<T *>(this)->visit_ebreak_inst(reinterpret_cast<EBREAKInst *>(inst));
                            break;
                        default:
                            return static_cast<T *>(this)->illegal_instruction(inst);
                    }
                    break;
#if defined(__RV_ZICSR_EXTENSION__)
                case CSRRWInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_csrrw_inst(reinterpret_cast<CSRRWInst *>(inst));
                    break;
                case CSRRSInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_csrrs_inst(reinterpret_cast<CSRRSInst *>(inst));
                    break;
                case CSRRCInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_csrrc_inst(reinterpret_cast<CSRRCInst *>(inst));
                    break;
                case CSRRWIInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_csrrwi_inst(reinterpret_cast<CSRRWIInst *>(inst));
                    break;
                case CSRRSIInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_csrrsi_inst(reinterpret_cast<CSRRSIInst *>(inst));
                    break;
                case CSRRCIInst::FUNC_3:
                    ret = static_cast<T *>(this)->visit_csrrci_inst(reinterpret_cast<CSRRCIInst *>(inst));
                    break;
#endif // defined(__RV_ZICSR_EXTENSION__)
                default:
                    return static_cast<T *>(this)->illegal_instruction(inst);
            }

            static_cast<T *>(this)->end_system_set_inst(inst);

            return ret;
        }

        RetT illegal_instruction(Instruction *inst) {
            risc_v_isa_unreachable("Illegal instruction met!");
        }

        RetT visit_inst(Instruction *inst) {
            risc_v_isa_unreachable("Uncaught instruction in visitor definition!");
        }

        void end_inst(Instruction *inst) {}

#endif // defined(__RV32I__) || defined(__RV64I__)
#if defined(__RVC__)

        RetT visit_16_inst(Instruction16 *inst) { return static_cast<T *>(this)->visit_inst(); }

#endif // defined(__RVC__)
#if defined(__RV32I__) || defined(__RV64I__)

        RetT visit_32_inst(Instruction32 *inst) { return static_cast<T *>(this)->visit_inst(inst); }

        void end_32_inst(Instruction32 *inst) {}

        RetT visit_branch_set_inst(InstructionBranchSet *inst) { return static_cast<T *>(this)->visit_32b_inst(inst); }

        void end_branch_set_inst(InstructionBranchSet *inst) {}

        RetT visit_load_set_inst(InstructionLoadSet *inst) { return static_cast<T *>(this)->visit_32i_inst(inst); }

        void end_load_set_inst(InstructionLoadSet *inst) {}

        RetT visit_store_set_inst(InstructionStoreSet *inst) { return static_cast<T *>(this)->visit_32s_inst(inst); }

        void end_store_set_inst(InstructionStoreSet *inst) {}

        RetT visit_arith_imm_set_inst(InstructionArithImmSet *inst) {
            return static_cast<T *>(this)->visit_32i_inst(inst);
        }

        void end_arith_imm_set_inst(InstructionArithImmSet *inst) {}

        RetT visit_arith_reg_set_inst(InstructionArithRegSet *inst) {
            return static_cast<T *>(this)->visit_32r_inst(inst);
        }

        void end_arith_reg_set_inst(InstructionArithRegSet *inst) {}

        RetT visit_fence_set_inst(InstructionFenceSet *inst) { return static_cast<T *>(this)->visit_32i_inst(inst); }

        void end_fence_set_inst(InstructionFenceSet *inst) {}

        RetT visit_system_set_inst(InstructionSystemSet *inst) { return static_cast<T *>(this)->visit_32i_inst(inst); }

        void end_system_set_inst(InstructionSystemSet *inst) {}

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

        RetT visit_slli_inst(SLLIWInst *inst) { return static_cast<T *>(this)->visit_arith_imm_set_inst(inst); }

        RetT visit_srli_inst(SRLIWInst *inst) { return static_cast<T *>(this)->visit_arith_imm_set_inst(inst); }

        RetT visit_srai_inst(SRAIWInst *inst) { return static_cast<T *>(this)->visit_arith_imm_set_inst(inst); }

        RetT visit_add_inst(ADDInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_sub_inst(SUBInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_sll_inst(SLLWInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_slt_inst(SLTInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_sltu_inst(SLTUInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_xor_inst(XORInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_srl_inst(SRLWInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_sra_inst(SRAWInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_or_inst(ORInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_and_inst(ANDInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_fence_inst(FENCEInst *inst) { return static_cast<T *>(this)->visit_fence_set_inst(inst); }

        RetT visit_ecall_inst(ECALLInst *inst) { return static_cast<T *>(this)->visit_system_set_inst(inst); }

        RetT visit_ebreak_inst(EBREAKInst *inst) { return static_cast<T *>(this)->visit_system_set_inst(inst); }

#if defined(__RV32M__) || defined(__RV64M__)

        RetT visit_mul_inst(MULInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_mulh_inst(MULHInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_mulhsu_inst(MULHSUInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_mulhu_inst(MULHUInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_div_inst(DIVInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_divu_inst(DIVUInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_rem_inst(REMInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

        RetT visit_remu_inst(REMUInst *inst) { return static_cast<T *>(this)->visit_arith_reg_set_inst(inst); }

#endif // defined(__RV32M__) || defined(__RV64M__)
#if defined(__RV_ZIFENCEI_EXTENSION__)

        RetT visit_fencei_inst(FENCEIInst *inst) { return static_cast<T *>(this)->visit_fence_set_inst(inst); }

#endif // defined(__RV_ZIFENCEI_EXTENSION__)
#if defined(__RV_ZICSR_EXTENSION__)

        RetT visit_csrrw_inst(CSRRWInst *inst) { return static_cast<T *>(this)->visit_system_set_inst(inst); }

        RetT visit_csrrs_inst(CSRRSInst *inst) { return static_cast<T *>(this)->visit_system_set_inst(inst); }

        RetT visit_csrrc_inst(CSRRCInst *inst) { return static_cast<T *>(this)->visit_system_set_inst(inst); }

        RetT visit_csrrwi_inst(CSRRWIInst *inst) { return static_cast<T *>(this)->visit_system_set_inst(inst); }

        RetT visit_csrrsi_inst(CSRRSIInst *inst) { return static_cast<T *>(this)->visit_system_set_inst(inst); }

        RetT visit_csrrci_inst(CSRRCIInst *inst) { return static_cast<T *>(this)->visit_system_set_inst(inst); }

#endif // defined(__RV_ZICSR_EXTENSION__)
#if defined(__RVCUSTOM0__)

        RetT visit_custom_0_inst(InstructionCustome0 *inst) { return static_cast<T *>(this)->visit_32_inst(inst); }

#endif // defined(__RVCUSTOM0__)
#if defined(__RVCUSTOM1__)

        RetT visit_custom_1_inst(InstructionCustome1 *inst) { return static_cast<T *>(this)->visit_32_inst(inst); }

#endif // defined(__RVCUSTOM1__)
#if defined(__RVCUSTOM2__)

        RetT visit_custom_2_inst(InstructionCustome2 *inst) { return static_cast<T *>(this)->visit_32_inst(inst); }

#endif // defined(__RVCUSTOM2__)
#if defined(__RVCUSTOM3__)

        RetT visit_custom_3_inst(InstructionCustome3 *inst) { return static_cast<T *>(this)->visit_32_inst(inst); }

#endif // defined(__RVCUSTOM3__)
#endif // defined(__RV32I__) || defined(__RV64I__)
    };
}


#endif //RISC_V_ISA_INSTRUCTION_VISITOR_HPP
