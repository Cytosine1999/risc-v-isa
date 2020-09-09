#ifndef RISCV_ISA_INSTRUCTION_VISITOR_HPP
#define RISCV_ISA_INSTRUCTION_VISITOR_HPP


#include "riscv_isa_utility.hpp"
#include "instruction.hpp"
#include "rvc.hpp"
#include "rv32i.hpp"
#include "rv32m.hpp"
#include "rv32a.hpp"
#include "rv64i.hpp"
#include "rv64m.hpp"
#include "zifencei.hpp"
#include "zicsr.hpp"
#include "privileged_instruction.hpp"


#define riscv_isa_instruction_map(func) \
    riscv_isa_instruction_c_map(func) \
    riscv_isa_instruction_32i_map(func) \
    riscv_isa_instruction_32m_map(func) \
    riscv_isa_instruction_32a_map(func) \
    riscv_isa_instruction_64i_map(func) \
    riscv_isa_instruction_64m_map(func) \
    riscv_isa_instruction_zifencei_map(func) \
    riscv_isa_instruction_zicsr_map(func) \
    riscv_isa_instruction_privilege_map(func)


namespace riscv_isa {
    template<typename SubT, typename RetT_ = void>
    class InstructionVisitor {
    private:
        SubT *sub_type() {
            static_assert(std::is_base_of<InstructionVisitor, SubT>::value, "not subtype of visitor");

            return static_cast<SubT *>(this);
        }

    public:
        using RetT = RetT_;

        RetT visit_in_memory(const Instruction *inst, usize length) {
#if defined(__RV_EXTENSION_C__)
            if (length >= 2 && is_type<const Instruction16>(inst)) {
                return this->visit_16(reinterpret_cast<const Instruction16 *>(inst));
            }
#endif // defined(__RV_EXTENSION_C__)

            if (length >= 4 && is_type<const Instruction32>(inst)) {
                return this->visit_32(reinterpret_cast<const Instruction32 *>(inst));
            }

            return sub_type()->illegal_instruction(inst);
        }

    protected:
#if defined(__RV_EXTENSION_C__)

        RetT visit_16(const Instruction16 *inst) {
            switch (inst->get_op()) {
                case 0b00:
                    return visit_16_op0(inst);
                case 0b01:
                    return visit_16_op1(inst);
                case 0b10:
                    return visit_16_op2(inst);
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_16_op0(const Instruction16 *inst) {
            switch (inst->get_funct3()) {
                case CADDI4SPNInst::FUNCT3:
                    if (get_bits<u16, 13, 5>(*reinterpret_cast<const u16 *>(inst)) != 0)
                        return sub_type()->illegal_instruction(inst);
                    return sub_type()->visit_caddi4spn_inst(reinterpret_cast<const CADDI4SPNInst *>(inst));
#if __RV_BIT_WIDTH__ == 32 || __RV_BIT_WIDTH__ == 64
#if defined(__RV_EXTENSION_D__)
                case CFLDInst::FUNCT3:
                    return sub_type()->visit_cfld_inst(reinterpret_cast<const CFLDInst *>(inst));
#endif // defined(__RV_EXTENSION_D__)
#elif __RV_BIT_WIDTH__ == 128
                case CLQInst::FUNCT3:
                    return sub_type()->visit_clq_inst(reinterpret_cast<const CLQInst *>(inst));
#endif
                case CLWInst::FUNCT3:
                    return sub_type()->visit_clw_inst(reinterpret_cast<const CLWInst *>(inst));
#if __RV_BIT_WIDTH__ == 32
#if defined(__RV_EXTENSION_F__)
                case CFLWInst::FUNCT3:
                    return sub_type()->visit_cflw_inst(reinterpret_cast<const CFLWInst *>(inst));
#endif // defined(__RV_EXTENSION_F__)
#elif __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128
                case CLDInst::FUNCT3:
                    return sub_type()->visit_cld_inst(reinterpret_cast<const CLDInst *>(inst));
#endif
#if __RV_BIT_WIDTH__ == 32 || __RV_BIT_WIDTH__ == 64
#if defined(__RV_EXTENSION_D__)
                case CFSDInst::FUNCT3:
                    return sub_type()->visit_cfsd_inst(reinterpret_cast<const CFSDInst *>(inst));
#endif // defined(__RV_EXTENSION_D__)
#elif __RV_BIT_WIDTH__ == 128
                case CSQInst::FUNCT3:
                    return sub_type()->visit_csq_inst(reinterpret_cast<const CSQInst *>(inst));
#endif
                case CSWInst::FUNCT3:
                    return sub_type()->visit_csw_inst(reinterpret_cast<const CSWInst *>(inst));
#if __RV_BIT_WIDTH__ == 32
#if defined(__RV_EXTENSION_F__)
                case CFSWInst::FUNCT3:
                    return sub_type()->visit_cfsw_inst(reinterpret_cast<const CFSWInst *>(inst));
#endif // defined(__RV_EXTENSION_F__)
#elif __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128
                case CLDInst::FUNCT3:
                    return sub_type()->visit_cld_inst(reinterpret_cast<const CLDInst *>(inst));
#endif
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_16_op1(const Instruction16 *inst) {
            switch (inst->get_op()) {
                case CADDIInst::FUNCT3:
                    return sub_type()->visit_caddi_inst(reinterpret_cast<const CADDIInst *>(inst));
#if __RV_BIT_WIDTH__ == 32
                case CJALInst::FUNCT3:
                    return sub_type()->visit_cjal_inst(reinterpret_cast<const CJALInst *>(inst));
#elif __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128
                case CADDIWInst::FUNCT3:
                    if (reinterpret_cast<const CADDIWInst *>(inst)->get_rd() == 0)
                        return sub_type()->illegal_instruction(inst);
                    return sub_type()->visit_caddiw_inst(reinterpret_cast<const CADDIWInst *>(inst));
#endif
                case CLIInst::FUNCT3:
                    return sub_type()->visit_cli_inst(reinterpret_cast<const CLIInst *>(inst));
                case 0b011:
                    if ((*reinterpret_cast<const u16 *>(inst) &
                         (bits_mask<u16, 7, 2>::val | bits_mask<u16, 13, 12>::val)) == 0)
                        return sub_type()->illegal_instruction(inst);
                    switch (reinterpret_cast<const CLUIInst *>(inst)->get_rd()) {
                        case 2:
                            return sub_type()->visit_caddi16sp_inst(reinterpret_cast<const CADDI16SPInst *>(inst));
                        default:
                            return sub_type()->visit_clui_inst(reinterpret_cast<const CLUIInst *>(inst));
                    }
                case 0b100:
                    switch (reinterpret_cast<const InstructionCA *>(inst)->get_funct2()) {
                        case CSRLIInst::FUNCT2:
                            if (reinterpret_cast<const InstructionCR *>(inst)->get_funct1() != 0)
                                return sub_type()->illegal_instruction(inst);
                            return sub_type()->visit_csrli_inst(reinterpret_cast<const CSRLIInst *>(inst));
                        case CSRAIInst::FUNCT2:
                            if (reinterpret_cast<const InstructionCR *>(inst)->get_funct1() != 0)
                                return sub_type()->illegal_instruction(inst);
                            return sub_type()->visit_csrai_inst(reinterpret_cast<const CSRAIInst *>(inst));
                        case CANDIInst::FUNCT2:
                            return sub_type()->visit_candi_inst(reinterpret_cast<const CANDIInst *>(inst));
                        case 0b11:
                            return visit_instruction_ca_type(reinterpret_cast<const InstructionCA *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }

                case CJInst::FUNCT3:
                    return sub_type()->visit_cj_inst(reinterpret_cast<const CJInst *>(inst));
                case CBEQZInst::FUNCT3:
                    return sub_type()->visit_cbeqz_inst(reinterpret_cast<const CBEQZInst *>(inst));
                case CBNEZInst::FUNCT3:
                    return sub_type()->visit_cbnez_inst(reinterpret_cast<const CBNEZInst *>(inst));
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_16_op2(const Instruction16 *inst) {
            switch (inst->get_funct3()) {
                case CSLLIInst::FUNCT3:
                    if (reinterpret_cast<const InstructionCR *>(inst)->get_funct1() != 0)
                        return sub_type()->illegal_instruction(inst);
                    return sub_type()->visit_cslli_inst(reinterpret_cast<const CSLLIInst *>(inst));
#if __RV_BIT_WIDTH__ == 32 || __RV_BIT_WIDTH__ == 64
#if defined(__RV_EXTENSION_D__)
                case CFLDSPInst::FUNCT3:
                    return sub_type()->visit_cfldsp_inst(reinterpret_cast<const CFLDSPInst *>(inst));
#endif // defined(__RV_EXTENSION_D__)
#elif __RV_BIT_WIDTH__ == 128
                case CLQSPInst::FUNCT3:
                    if (reinterpret_cast<const CLQSPInst *>(inst)->get_rd() == 0)
                        return sub_type()->illegal_instruction(inst);
                    return sub_type()->visit_clqsp_inst(reinterpret_cast<const CLQSPInst *>(inst));
#endif
                case CLWSPInst::FUNCT3:
                    return sub_type()->visit_clwsp_inst(reinterpret_cast<const CLWSPInst *>(inst));
#if __RV_BIT_WIDTH__ == 32
#if defined(__RV_EXTENSION_F__)
                case CFLWSPInst::FUNCT3:
                    return sub_type()->visit_cflwsp_inst(reinterpret_cast<const CFLWSPInst *>(inst));
#endif // defined(__RV_EXTENSION_F__)
#elif __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128
                case CLDSPInst::FUNCT3:
                    return sub_type()->visit_cldsp_inst();
#endif
                case InstructionCR::FUNCT3:
                    return visit_instruction_cr_type(reinterpret_cast<const InstructionCR *>(inst));
#if __RV_BIT_WIDTH__ == 32 || __RV_BIT_WIDTH__ == 64
#if defined(__RV_EXTENSION_D__)
                case CFSDSPInst::FUNCT3:
                    return sub_type()->visit_cfsdsp_inst(reinterpret_cast<const CFSDSPInst *>(inst));
#endif // defined(__RV_EXTENSION_D__)
#elif __RV_BIT_WIDTH__ == 128
                case CSQSPInst::FUNCT3:
                    return sub_type()->visit_csqsp_inst(reinterpret_cast<const CSQSPInst *>(inst));
#endif
                case CSWSPInst::FUNCT3:
                    return sub_type()->visit_cswsp_inst(reinterpret_cast<const CSWSPInst *>(inst));
#if __RV_BIT_WIDTH__ == 32
#if defined(__RV_EXTENSION_F__)
                case CFSWSPInst::FUNCT3:
                    return sub_type()->visit_cfswsp_inst(reinterpret_cast<const CFSWSPInst *>(inst));
#endif // defined(__RV_EXTENSION_F__)
#elif __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128
                case CSDSPInst::FUNCT3:
                    return sub_type()->visit_csdsp_inst(reinterpret_cast<const CSDSPInst *>(inst));
#endif
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_instruction_ca_type(const InstructionCA *inst) {
            switch (inst->get_funct1()) {
                case 0b0:
                    switch (inst->get_funct_arith()) {
                        case CSUBInst::FUNCT_ARITH:
                            return sub_type()->visit_csub_inst(reinterpret_cast<const CSUBInst *>(inst));
                        case CXORInst::FUNCT_ARITH:
                            return sub_type()->visit_cxor_inst(reinterpret_cast<const CXORInst *>(inst));
                        case CORInst::FUNCT_ARITH:
                            return sub_type()->visit_cor_inst(reinterpret_cast<const CORInst *>(inst));
                        case CANDInst::FUNCT_ARITH:
                            return sub_type()->visit_cand_inst(reinterpret_cast<const CANDInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
#if __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128
                case 0b1:
                    switch (inst->get_funct_arith()) {
                        case CSUBWInst::FUNCT_ARITH:
                            return sub_type()->visit_csubw_inst(reinterpret_cast<const CSUBWInst *>(inst));
                        case CADDWInst::FUNCT_ARITH:
                            return sub_type()->visit_caddw_inst(reinterpret_cast<const CADDWInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
#endif
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_instruction_cr_type(const InstructionCR *inst) {
            switch (inst->get_funct1()) {
                case 0b0:
                    switch (inst->get_rs2()) {
                        case 0:
                            if (reinterpret_cast<const CJRInst *>(inst)->get_rs1() == 0)
                                return sub_type()->illegal_instruction(inst);
                            return sub_type()->visit_cjr_inst(reinterpret_cast<const CJRInst *>(inst));
                        default:
                            return sub_type()->visit_cmv_inst(reinterpret_cast<const CMVInst *>(inst));
                    }
                case 0b1:
                    switch (inst->get_rs2()) {
                        case 0:
                            switch (reinterpret_cast<const CJALRInst *>(inst)->get_rs1()) {
                                case 0:
                                    return sub_type()->visit_cebreak_inst(reinterpret_cast<const CEBREAKInst *>(inst));
                                default:
                                    return sub_type()->visit_cjalr_inst(reinterpret_cast<const CJALRInst *>(inst));
                            }
                        default:
                            return sub_type()->visit_cadd_inst(reinterpret_cast<const CADDInst *>(inst));
                    }
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

#endif // defined(__RV_EXTENSION_C__)

        RetT visit_32(const Instruction32 *inst) {
            switch (inst->get_op_code()) {
                case LUIInst::OP_CODE:
                    return sub_type()->visit_lui_inst(reinterpret_cast<const LUIInst *>(inst));
                case AUIPCInst::OP_CODE:
                    return sub_type()->visit_auipc_inst(reinterpret_cast<const AUIPCInst *>(inst));
                case JALInst::OP_CODE:
                    return sub_type()->visit_jal_inst(reinterpret_cast<const JALInst *>(inst));
                case JALRInst::OP_CODE:
                    if (reinterpret_cast<const JALRInst *>(inst)->get_funct3() != JALRInst::FUNCT3)
                        return sub_type()->illegal_instruction(inst);
                    return sub_type()->visit_jalr_inst(reinterpret_cast<const JALRInst *>(inst));
                case InstructionBranchSet::OP_CODE:
                    return visit_branch_set(reinterpret_cast<const InstructionBranchSet *>(inst));
                case InstructionLoadSet::OP_CODE:
                    return visit_load_set(reinterpret_cast<const InstructionLoadSet *>(inst));
                case InstructionStoreSet::OP_CODE:
                    return visit_store_set(reinterpret_cast<const InstructionStoreSet *>(inst));
                case InstructionArithImmSet::OP_CODE:
                    return visit_arith_imm_set(reinterpret_cast<const InstructionArithImmSet *>(inst));
                case InstructionArithRegSet::OP_CODE:
                    return visit_arith_reg_set(reinterpret_cast<const InstructionArithRegSet *>(inst));
                case InstructionFenceSet::OP_CODE:
                    return visit_fence_set(reinterpret_cast<const InstructionFenceSet *>(inst));
                case InstructionSystemSet::OP_CODE:
                    return visit_system_set(reinterpret_cast<const InstructionSystemSet *>(inst));
#if defined(__RV_EXTENSION_A__)
                case InstructionAtomicSet::OP_CODE:
                    return visit_atomic_set(reinterpret_cast<const InstructionAtomicSet *>(inst));
#endif
#if __RV_BIT_WIDTH__ == 64
                    case InstructionArithImmWSet::OP_CODE:
                        return visit_arith_imm_w_set(reinterpret_cast<const InstructionArithImmWSet *>(inst));
                    case InstructionArithRegWSet::OP_CODE:
                        return visit_arith_reg_w_set(reinterpret_cast<const InstructionArithRegWSet *>(inst));
#endif // __RV_BIT_WIDTH__ == 64
#if defined(__RV_CUSTOM_0__)
                    case InstructionCustome0::OP_CODE:
                        return sub_type()->visit_custom_0_inst(reinterpret_cast<const InstructionCustome0 *>(inst));
#endif // defined(__RV_CUSTOM_0__)
#if defined(__RV_CUSTOM_1__)
                    case InstructionCustome1::OP_CODE:
                        return sub_type()->visit_custom_1_inst(reinterpret_cast<const InstructionCustome0 *>(inst));
#endif // defined(__RV_CUSTOM_1__)
#if defined(__RV_CUSTOM_2__)
                    case InstructionCustome2::OP_CODE:
                        return sub_type()->visit_custom_2_inst(reinterpret_cast<const InstructionCustome0 *>(inst));
#endif // defined(__RV_CUSTOM_2__)
#if defined(__RV_CUSTOM_3__)
                    case InstructionCustome3::OP_CODE:
                        return sub_type()->visit_custom_3_inst(reinterpret_cast<const InstructionCustome0 *>(inst));
#endif // defined(__RV_CUSTOM_3__)
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_branch_set(const InstructionBranchSet *inst) {
            switch (inst->get_funct3()) {
                case BEQInst::FUNCT3:
                    return sub_type()->visit_beq_inst(reinterpret_cast<const BEQInst *>(inst));
                case BNEInst::FUNCT3:
                    return sub_type()->visit_bne_inst(reinterpret_cast<const BNEInst *>(inst));
                case BLTInst::FUNCT3:
                    return sub_type()->visit_blt_inst(reinterpret_cast<const BLTInst *>(inst));
                case BGEInst::FUNCT3:
                    return sub_type()->visit_bge_inst(reinterpret_cast<const BGEInst *>(inst));
                case BLTUInst::FUNCT3:
                    return sub_type()->visit_bltu_inst(reinterpret_cast<const BLTUInst *>(inst));
                case BGEUInst::FUNCT3:
                    return sub_type()->visit_bgeu_inst(reinterpret_cast<const BGEUInst *>(inst));
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_load_set(const InstructionLoadSet *inst) {
            switch (inst->get_funct3()) {
                case LBInst::FUNCT3:
                    return sub_type()->visit_lb_inst(reinterpret_cast<const LBInst *>(inst));
                case LHInst::FUNCT3:
                    return sub_type()->visit_lh_inst(reinterpret_cast<const LHInst *>(inst));
                case LWInst::FUNCT3:
                    return sub_type()->visit_lw_inst(reinterpret_cast<const LWInst *>(inst));
                case LBUInst::FUNCT3:
                    return sub_type()->visit_lbu_inst(reinterpret_cast<const LBUInst *>(inst));
                case LHUInst::FUNCT3:
                    return sub_type()->visit_lhu_inst(reinterpret_cast<const LHUInst *>(inst));
#if __RV_BIT_WIDTH__ == 64
                    case LDInst::FUNCT3:
                        return sub_type()->visit_ld_inst(reinterpret_cast<const LDInst *>(inst));
                    case LWUInst::FUNCT3:
                        return sub_type()->visit_lwu_inst(reinterpret_cast<const LWUInst *>(inst));
#endif // __RV_BIT_WIDTH__ == 64
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_store_set(const InstructionStoreSet *inst) {
            switch (inst->get_funct3()) {
                case SBInst::FUNCT3:
                    return sub_type()->visit_sb_inst(reinterpret_cast<const SBInst *>(inst));
                case SHInst::FUNCT3:
                    return sub_type()->visit_sh_inst(reinterpret_cast<const SHInst *>(inst));
                case SWInst::FUNCT3:
                    return sub_type()->visit_sw_inst(reinterpret_cast<const SWInst *>(inst));
#if __RV_BIT_WIDTH__ == 64
                    case SDInst::FUNCT3:
                        return sub_type()->visit_sd_inst(reinterpret_cast<const SDInst *>(inst));
#endif // __RV_BIT_WIDTH__ == 64
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_arith_imm_set(const InstructionArithImmSet *inst) {
            switch (inst->get_funct3()) {
                case ADDIInst::FUNCT3:
                    return sub_type()->visit_addi_inst(reinterpret_cast<const ADDIInst *>(inst));
                case SLTIInst::FUNCT3:
                    return sub_type()->visit_slti_inst(reinterpret_cast<const SLTIInst *>(inst));
                case SLTIUInst::FUNCT3:
                    return sub_type()->visit_sltiu_inst(reinterpret_cast<const SLTIUInst *>(inst));
                case XORIInst::FUNCT3:
                    return sub_type()->visit_xori_inst(reinterpret_cast<const XORIInst *>(inst));
                case ORIInst::FUNCT3:
                    return sub_type()->visit_ori_inst(reinterpret_cast<const ORIInst *>(inst));
                case ANDIInst::FUNCT3:
                    return sub_type()->visit_andi_inst(reinterpret_cast<const ANDIInst *>(inst));
                case SLLIInst::FUNCT3:
                    switch (reinterpret_cast<const SLLIInst *>(inst)->get_funct_shift()) {
                        case SLLIInst::FUNCT_SHIFT:
                            return sub_type()->visit_slli_inst(reinterpret_cast<const SLLIInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
                case InstructionShiftRightImmSet::FUNCT3:
                    switch (reinterpret_cast<const InstructionShiftRightImmSet *>(inst)->get_funct_shift()) {
                        case SRLIInst::FUNCT_SHIFT:
                            return sub_type()->visit_srli_inst(reinterpret_cast<const SRLIInst *>(inst));
                        case SRAIInst::FUNCT_SHIFT:
                            return sub_type()->visit_srai_inst(reinterpret_cast<const SRAIInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_arith_reg_set(const InstructionArithRegSet *inst) {
            switch (inst->get_funct7()) {
                case InstructionIntegerRegSet::FUNCT7:
                    switch (inst->get_funct3()) {
                        case ADDInst::FUNCT3:
                            return sub_type()->visit_add_inst(reinterpret_cast<const ADDInst *>(inst));
                        case SLLInst::FUNCT3:
                            return sub_type()->visit_sll_inst(reinterpret_cast<const SLLInst *>(inst));
                        case SLTInst::FUNCT3:
                            return sub_type()->visit_slt_inst(reinterpret_cast<const SLTInst *>(inst));
                        case SLTUInst::FUNCT3:
                            return sub_type()->visit_sltu_inst(reinterpret_cast<const SLTUInst *>(inst));
                        case XORInst::FUNCT3:
                            return sub_type()->visit_xor_inst(reinterpret_cast<const XORInst *>(inst));
                        case SRLInst::FUNCT3:
                            return sub_type()->visit_srl_inst(reinterpret_cast<const SRLInst *>(inst));
                        case ORInst::FUNCT3:
                            return sub_type()->visit_or_inst(reinterpret_cast<const ORInst *>(inst));
                        case ANDInst::FUNCT3:
                            return sub_type()->visit_and_inst(reinterpret_cast<const ANDInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
                case InstructionIntegerRegModSet::FUNCT7:
                    switch (inst->get_funct3()) {
                        case SUBInst::FUNCT3:
                            return sub_type()->visit_sub_inst(reinterpret_cast<const SUBInst *>(inst));
                        case SRAInst::FUNCT3:
                            return sub_type()->visit_sra_inst(reinterpret_cast<const SRAInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
#if defined(__RV_EXTENSION_M__)
                case InstructionMulDivSet::FUNCT7:
                    switch (inst->get_funct3()) {
#define _riscv_isa_visit_32m_instruction(NAME, name) \
                        case NAME##Inst::FUNCT3: \
                            return sub_type()->visit_##name##_inst(reinterpret_cast<const NAME##Inst *>(inst));
                        riscv_isa_instruction_32m_map(_riscv_isa_visit_32m_instruction)
#undef _riscv_isa_visit_32m_instruction
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
#endif // defined(__RV_EXTENSION_M__)
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_fence_set(const InstructionFenceSet *inst) {
            switch (inst->get_funct3()) {
                case FENCEInst::FUNCT3:
                    return sub_type()->visit_fence_inst(reinterpret_cast<const FENCEInst *>(inst));
#define _riscv_isa_visit_zifencei_instruction(NAME, name) \
                        case NAME##Inst::FUNCT3: \
                            return sub_type()->visit_##name##_inst(reinterpret_cast<const NAME##Inst *>(inst));
                    riscv_isa_instruction_zifencei_map(_riscv_isa_visit_zifencei_instruction)
#undef _riscv_isa_visit_zifencei_instruction
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_system_set(const InstructionSystemSet *inst) {
            switch (inst->get_funct3()) {
                case InstructionPrivilegedSet::FUNCT3:
                    return visit_privilege_set(reinterpret_cast<const InstructionPrivilegedSet *>(inst));
#define _riscv_isa_visit_zicsr_instruction(NAME, name) \
                        case NAME##Inst::FUNCT3: \
                            return sub_type()->visit_##name##_inst(reinterpret_cast<const NAME##Inst *>(inst));
                riscv_isa_instruction_zicsr_map(_riscv_isa_visit_zicsr_instruction)
#undef _riscv_isa_visit_zicsr_instruction
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_privilege_set(const InstructionPrivilegedSet *inst) {
            switch (inst->get_funct7()) {
                case 0b0000000:
                    if (inst->get_rd() != 0 || inst->get_rs1() != 0)
                        return sub_type()->illegal_instruction(inst);
                    switch (inst->get_rs2()) {
                        case ECALLInst::FUNCT_PRIVILEGE:
                            return sub_type()->visit_ecall_inst(reinterpret_cast<const ECALLInst *>(inst));
                        case EBREAKInst::FUNCT_PRIVILEGE:
                            return sub_type()->visit_ebreak_inst(reinterpret_cast<const EBREAKInst *>(inst));
#if defined(__RV_EXTENSION_N__)
                            case URETInst::FUNCT_PRIVILEGE:
                                return sub_type()->visit_uret_inst(reinterpret_cast<const URETInst *>(inst));
#endif // defined(__RV_EXTENSION_N__)
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
                case 0b0001000:
                    if (inst->get_rd() != 0 || inst->get_rs1() != 0)
                        return sub_type()->illegal_instruction(inst);
                    switch (inst->get_rs2()) {
#if defined(__RV_SUPERVISOR_MODE__)
                        case SRETInst::FUNCT_PRIVILEGE:
                            return sub_type()->visit_sret_inst(reinterpret_cast<const SRETInst *>(inst));
#endif // defined(__RV_SUPERVISOR_MODE__)
                        case WFIInst::FUNCT_PRIVILEGE:
                            return sub_type()->visit_wfi_inst(reinterpret_cast<const WFIInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
                case MRETInst::FUNCT7:
                    if (inst->get_rd() != 0 || inst->get_rs1() != 0)
                        return sub_type()->illegal_instruction(inst);
                    return sub_type()->visit_mret_inst(reinterpret_cast<const MRETInst *>(inst));
#if defined(__RV_SUPERVISOR_MODE__)
                case SFENCEVAMInst::FUNCT7:
                    if (inst->get_rd() != 0) return sub_type()->illegal_instruction(inst);
                    return sub_type()->visit_sfencevma_inst(reinterpret_cast<const SFENCEVAMInst *>(inst));
#endif // defined(__RV_SUPERVISOR_MODE__)
#if defined(__RV_HYPERVISOR_MODE__)
                    case HFENCEVVMAInst::FUNCT7:
                        if (inst->get_rd() != 0) return sub_type()->illegal_instruction(inst);
                        return sub_type()->visit_hfencevvma_inst(reinterpret_cast<const HFENCEVVMAInst *>(inst));
                    case HFENCEGVMAInst::FUNCT7:
                        if (inst->get_rd() != 0) return sub_type()->illegal_instruction(inst);
                        return sub_type()->visit_hfencegvma_inst(reinterpret_cast<const HFENCEGVMAInst *>(inst));
#endif
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

#if defined(__RV_EXTENSION_A__)

        RetT visit_atomic_set(const InstructionAtomicSet *inst) {
            switch (inst->get_funct3()) {
                case InstructionAtomicWSet::FUNCT3:
                    switch (inst->get_funct_atomic()) {
#define _riscv_isa_visit_32a_instruction(NAME, name) \
                        case NAME##Inst::FUNCT_ATOMIC: \
                            return sub_type()->visit_##name##_inst(reinterpret_cast<const NAME##Inst *>(inst));
                        riscv_isa_instruction_32a_map(_riscv_isa_visit_32a_instruction)
#undef _riscv_isa_visit_32a_instruction
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

#endif
#if __RV_BIT_WIDTH__ == 64

        RetT visit_arith_imm_w_set(InstructionArithImmWSet *inst) {
            switch (inst->get_funct3()) {
                case ADDIWInst::FUNCT3:
                    return sub_type()->visit_addiw_inst(reinterpret_cast<const ADDIWInst *>(inst));
                case SLLIWInst::FUNCT3:
                    switch (reinterpret_cast<const SLLIWInst *>(inst)->get_funct_shift()) {
                        case SLLIWInst::FUNCT_SHIFT:
                            return sub_type()->visit_slliw_inst(reinterpret_cast<const SLLIWInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
                case InstructionShiftRightImmWSet::FUNCT3:
                    switch (reinterpret_cast<const InstructionShiftRightImmWSet *>(inst)->get_funct_shift()) {
                        case SRLIWInst::FUNCT_SHIFT:
                            return sub_type()->visit_srliw_inst(reinterpret_cast<const SRLIWInst *>(inst));
                        case SRAIWInst::FUNCT_SHIFT:
                            return sub_type()->visit_sraiw_inst(reinterpret_cast<const SRAIWInst *>(inst));
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
                            return sub_type()->visit_addw_inst(reinterpret_cast<const ADDWInst *>(inst));
                        case SLLWInst::FUNCT3:
                            return sub_type()->visit_sllw_inst(reinterpret_cast<const SLLWInst *>(inst));
                        case SRLWInst::FUNCT3:
                            return sub_type()->visit_srlw_inst(reinterpret_cast<const SRLWInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
                case InstructionIntegerRegModWSet::FUNCT7:
                    switch (inst->get_funct3()) {
                        case SUBWInst::FUNCT3:
                            return sub_type()->visit_subw_inst(reinterpret_cast<const SUBWInst *>(inst));
                        case SRAWInst::FUNCT3:
                            return sub_type()->visit_sraw_inst(reinterpret_cast<const SRAWInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
#if defined(__RV_EXTENSION_M__)
                case InstructionMulDivWSet::FUNCT7:
                    switch (inst->get_funct3()) {
#define _riscv_isa_visit_64m_instruction(NAME, name) \
                        case NAME##Inst::FUNCT3: \
                            return sub_type()->visit_##name##_inst(reinterpret_cast<const NAME##Inst *>(inst));
                        riscv_isa_instruction_64m_map(_riscv_isa_visit_64m_instruction)
#undef _riscv_isa_visit_64m_instruction
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
#endif // defined(__RV_EXTENSION_M__)
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

#endif // __RV_BIT_WIDTH__ == 64

        RetT illegal_instruction(riscv_isa_unused const Instruction *inst) {
            riscv_isa_abort("Illegal instruction met!");
        }

    public:
///     this function is required to be implemented if any of visit_##name##_inst function is not being overwritten.
///
///     RetT visit_inst(riscv_isa_unused const Instruction *inst) {
///         riscv_isa_unreachable("Uncaught instruction in visitor definition!");
///     }

#define _riscv_isa_visit_instruction(NAME, name) \
        RetT visit_##name##_inst(const NAME##Inst *inst) { return sub_type()->visit_inst(inst); }

        riscv_isa_instruction_map(_riscv_isa_visit_instruction)

#undef _riscv_isa_visit_instruction
    };
}


#endif //RISCV_ISA_INSTRUCTION_VISITOR_HPP
