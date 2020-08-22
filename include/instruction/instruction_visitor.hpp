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

        RetT visit(Instruction *inst) {
            u16 half = *reinterpret_cast<u16 *>(inst);

            if ((half & bits_mask<u16, 2, 0>::val) != bits_mask<u16, 2, 0>::val) {
#if defined(__RV_EXTENSION_C__)
                return visit_16(reinterpret_cast<Instruction16 *>(inst));
#else
                return sub_type()->illegal_instruction(inst);
#endif // defined(__RV_EXTENSION_C__)
            } else if ((half & bits_mask<u16, 5, 2>::val) != bits_mask<u16, 5, 2>::val) {
                return visit_32(reinterpret_cast<Instruction32 *>(inst));
            } else {
                return sub_type()->illegal_instruction(inst);
            }
        }

    protected:
#if defined(__RV_EXTENSION_C__)

        RetT visit_16(Instruction16 *inst) {
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

        RetT visit_16_op0(Instruction16 *inst) {
            switch (inst->get_funct3()) {
                case CADDI4SPNInst::FUNCT3:
                    if (get_bits<u16, 13, 5>(*reinterpret_cast<u16 *>(inst)) != 0)
                        return sub_type()->illegal_instruction(inst);
                    return sub_type()->visit_caddi4spn_inst(reinterpret_cast<CADDI4SPNInst *>(inst));
#if __RV_BIT_WIDTH__ == 32 || __RV_BIT_WIDTH__ == 64
#if defined(__RV_EXTENSION_D__)
                case CFLDInst::FUNCT3:
                    return sub_type()->visit_cfld_inst(reinterpret_cast<CFLDInst *>(inst));
#endif // defined(__RV_EXTENSION_D__)
#elif __RV_BIT_WIDTH__ == 128
                case CLQInst::FUNCT3:
                    return sub_type()->visit_clq_inst(reinterpret_cast<CLQInst *>(inst));
#endif
                case CLWInst::FUNCT3:
                    return sub_type()->visit_clw_inst(reinterpret_cast<CLWInst *>(inst));
#if __RV_BIT_WIDTH__ == 32
#if defined(__RV_EXTENSION_F__)
                case CFLWInst::FUNCT3:
                    return sub_type()->visit_cflw_inst(reinterpret_cast<CFLWInst *>(inst));
#endif // defined(__RV_EXTENSION_F__)
#elif __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128
                case CLDInst::FUNCT3:
                    return sub_type()->visit_cld_inst(reinterpret_cast<CLDInst *>(inst));
#endif
#if __RV_BIT_WIDTH__ == 32 || __RV_BIT_WIDTH__ == 64
#if defined(__RV_EXTENSION_D__)
                case CFSDInst::FUNCT3:
                    return sub_type()->visit_cfsd_inst(reinterpret_cast<CFSDInst *>(inst));
#endif // defined(__RV_EXTENSION_D__)
#elif __RV_BIT_WIDTH__ == 128
                case CSQInst::FUNCT3:
                    return sub_type()->visit_csq_inst(reinterpret_cast<CSQInst *>(inst));
#endif
                case CSWInst::FUNCT3:
                    return sub_type()->visit_csw_inst(reinterpret_cast<CSWInst *>(inst));
#if __RV_BIT_WIDTH__ == 32
#if defined(__RV_EXTENSION_F__)
                case CFSWInst::FUNCT3:
                    return sub_type()->visit_cfsw_inst(reinterpret_cast<CFSWInst *>(inst));
#endif // defined(__RV_EXTENSION_F__)
#elif __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128
                case CLDInst::FUNCT3:
                    return sub_type()->visit_cld_inst(reinterpret_cast<CLDInst *>(inst));
#endif
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_16_op1(Instruction16 *inst) {
            switch (inst->get_op()) {
                case CADDIInst::FUNCT3:
                    return sub_type()->visit_caddi_inst(reinterpret_cast<CADDIInst *>(inst));
#if __RV_BIT_WIDTH__ == 32
                case CJALInst::FUNCT3:
                    return sub_type()->visit_cjal_inst(reinterpret_cast<CJALInst *>(inst));
#elif __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128
                case CADDIWInst::FUNCT3:
                    if (reinterpret_cast<CADDIWInst *>(inst)->get_rd() == 0)
                        return sub_type()->illegal_instruction(inst);
                    return sub_type()->visit_caddiw_inst(reinterpret_cast<CADDIWInst *>(inst));
#endif
                case CLIInst::FUNCT3:
                    return sub_type()->visit_cli_inst(reinterpret_cast<CLIInst *>(inst));
                case 0b011:
                    if ((*reinterpret_cast<u16 *>(inst) &
                         (bits_mask<u16, 7, 2>::val | bits_mask<u16, 13, 12>::val)) == 0)
                        return sub_type()->illegal_instruction(inst);
                    switch (reinterpret_cast<CLUIInst *>(inst)->get_rd()) {
                        case 2:
                            return sub_type()->visit_caddi16sp_inst(reinterpret_cast<CADDI16SPInst *>(inst));
                        default:
                            return sub_type()->visit_clui_inst(reinterpret_cast<CLUIInst *>(inst));
                    }
                case 0b100:
                    switch (reinterpret_cast<InstructionCA *>(inst)->get_funct2()) {
                        case CSRLIInst::FUNCT2:
                            if (reinterpret_cast<InstructionCR *>(inst)->get_funct1() != 0)
                                return sub_type()->illegal_instruction(inst);
                            return sub_type()->visit_csrli_inst(reinterpret_cast<CSRLIInst *>(inst));
                        case CSRAIInst::FUNCT2:
                            if (reinterpret_cast<InstructionCR *>(inst)->get_funct1() != 0)
                                return sub_type()->illegal_instruction(inst);
                            return sub_type()->visit_csrai_inst(reinterpret_cast<CSRAIInst *>(inst));
                        case CANDIInst::FUNCT2:
                            return sub_type()->visit_candi_inst(reinterpret_cast<CANDIInst *>(inst));
                        case 0b11:
                            return visit_instruction_ca_type(reinterpret_cast<InstructionCA *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }

                case CJInst::FUNCT3:
                    return sub_type()->visit_cj_inst(reinterpret_cast<CJInst *>(inst));
                case CBEQZInst::FUNCT3:
                    return sub_type()->visit_cbeqz_inst(reinterpret_cast<CBEQZInst *>(inst));
                case CBNEZInst::FUNCT3:
                    return sub_type()->visit_cbnez_inst(reinterpret_cast<CBNEZInst *>(inst));
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_16_op2(Instruction16 *inst) {
            switch (inst->get_funct3()) {
                case CSLLIInst::FUNCT3:
                    if (reinterpret_cast<InstructionCR *>(inst)->get_funct1() != 0)
                        return sub_type()->illegal_instruction(inst);
                    return sub_type()->visit_cslli_inst(reinterpret_cast<CSLLIInst *>(inst));
#if __RV_BIT_WIDTH__ == 32 || __RV_BIT_WIDTH__ == 64
#if defined(__RV_EXTENSION_D__)
                case CFLDSPInst::FUNCT3:
                    return sub_type()->visit_cfldsp_inst(reinterpret_cast<CFLDSPInst *>(inst));
#endif // defined(__RV_EXTENSION_D__)
#elif __RV_BIT_WIDTH__ == 128
                case CLQSPInst::FUNCT3:
                    if (reinterpret_cast<CLQSPInst *>(inst)->get_rd() == 0)
                        return sub_type()->illegal_instruction(inst);
                    return sub_type()->visit_clqsp_inst(reinterpret_cast<CLQSPInst *>(inst));
#endif
                case CLWSPInst::FUNCT3:
                    return sub_type()->visit_clwsp_inst(reinterpret_cast<CLWSPInst *>(inst));
#if __RV_BIT_WIDTH__ == 32
#if defined(__RV_EXTENSION_F__)
                case CFLWSPInst::FUNCT3:
                    return sub_type()->visit_cflwsp_inst(reinterpret_cast<CFLWSPInst *>(inst));
#endif // defined(__RV_EXTENSION_F__)
#elif __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128
                case CLDSPInst::FUNCT3:
                    return sub_type()->visit_cldsp_inst();
#endif
                case InstructionCR::FUNCT3:
                    return visit_instruction_cr_type(reinterpret_cast<InstructionCR *>(inst));
#if __RV_BIT_WIDTH__ == 32 || __RV_BIT_WIDTH__ == 64
#if defined(__RV_EXTENSION_D__)
                case CFSDSPInst::FUNCT3:
                    return sub_type()->visit_cfsdsp_inst(reinterpret_cast<CFSDSPInst *>(inst));
#endif // defined(__RV_EXTENSION_D__)
#elif __RV_BIT_WIDTH__ == 128
                case CSQSPInst::FUNCT3:
                    return sub_type()->visit_csqsp_inst(reinterpret_cast<CSQSPInst *>(inst));
#endif
                case CSWSPInst::FUNCT3:
                    return sub_type()->visit_cswsp_inst(reinterpret_cast<CSWSPInst *>(inst));
#if __RV_BIT_WIDTH__ == 32
#if defined(__RV_EXTENSION_F__)
                case CFSWSPInst::FUNCT3:
                    return sub_type()->visit_cfswsp_inst(reinterpret_cast<CFSWSPInst *>(inst));
#endif // defined(__RV_EXTENSION_F__)
#elif __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128
                case CSDSPInst::FUNCT3:
                    return sub_type()->visit_csdsp_inst(reinterpret_cast<CSDSPInst *>(inst));
#endif
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_instruction_ca_type(InstructionCA *inst) {
            switch (inst->get_funct1()) {
                case 0b0:
                    switch (inst->get_funct_arith()) {
                        case CSUBInst::FUNCT_ARITH:
                            return sub_type()->visit_csub_inst(reinterpret_cast<CSUBInst *>(inst));
                        case CXORInst::FUNCT_ARITH:
                            return sub_type()->visit_cxor_inst(reinterpret_cast<CXORInst *>(inst));
                        case CORInst::FUNCT_ARITH:
                            return sub_type()->visit_cor_inst(reinterpret_cast<CORInst *>(inst));
                        case CANDInst::FUNCT_ARITH:
                            return sub_type()->visit_cand_inst(reinterpret_cast<CANDInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
#if __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128
                case 0b1:
                    switch (inst->get_funct_arith()) {
                        case CSUBWInst::FUNCT_ARITH:
                            return sub_type()->visit_csubw_inst(reinterpret_cast<CSUBWInst *>(inst));
                        case CADDWInst::FUNCT_ARITH:
                            return sub_type()->visit_caddw_inst(reinterpret_cast<CADDWInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
#endif
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_instruction_cr_type(InstructionCR *inst) {
            switch (inst->get_funct1()) {
                case 0b0:
                    switch (inst->get_rs2()) {
                        case 0:
                            if (reinterpret_cast<CJRInst *>(inst)->get_rs1() == 0)
                                return sub_type()->illegal_instruction(inst);
                            return sub_type()->visit_cjr_inst(reinterpret_cast<CJRInst *>(inst));
                        default:
                            return sub_type()->visit_cmv_inst(reinterpret_cast<CMVInst *>(inst));
                    }
                case 0b1:
                    switch (inst->get_rs2()) {
                        case 0:
                            switch (reinterpret_cast<CJALRInst *>(inst)->get_rs1()) {
                                case 0:
                                    return sub_type()->visit_cebreak_inst(reinterpret_cast<CEBREAKInst *>(inst));
                                default:
                                    return sub_type()->visit_cjalr_inst(reinterpret_cast<CJALRInst *>(inst));
                            }
                        default:
                            return sub_type()->visit_cadd_inst(reinterpret_cast<CADDInst *>(inst));
                    }
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

#endif // defined(__RV_EXTENSION_C__)

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
#if defined(__RV_EXTENSION_A__)
                case InstructionAtomicSet::OP_CODE:
                    return visit_atomic_set(reinterpret_cast<InstructionAtomicSet *>(inst));
#endif
#if __RV_BIT_WIDTH__ == 64
                    case InstructionArithImmWSet::OP_CODE:
                        return visit_arith_imm_w_set(reinterpret_cast<InstructionArithImmWSet *>(inst));
                    case InstructionArithRegWSet::OP_CODE:
                        return visit_arith_reg_w_set(reinterpret_cast<InstructionArithRegWSet *>(inst));
#endif // __RV_BIT_WIDTH__ == 64
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
                    return sub_type()->visit_lw_inst(reinterpret_cast<LWInst *>(inst));
                case LBUInst::FUNCT3:
                    return sub_type()->visit_lbu_inst(reinterpret_cast<LBUInst *>(inst));
                case LHUInst::FUNCT3:
                    return sub_type()->visit_lhu_inst(reinterpret_cast<LHUInst *>(inst));
#if __RV_BIT_WIDTH__ == 64
                    case LDInst::FUNCT3:
                        return sub_type()->visit_ld_inst(reinterpret_cast<LDInst *>(inst));
                    case LWUInst::FUNCT3:
                        return sub_type()->visit_lwu_inst(reinterpret_cast<LWUInst *>(inst));
#endif // __RV_BIT_WIDTH__ == 64
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
#if __RV_BIT_WIDTH__ == 64
                    case SDInst::FUNCT3:
                        return sub_type()->visit_sd_inst(reinterpret_cast<SDInst *>(inst));
#endif // __RV_BIT_WIDTH__ == 64
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
#define _riscv_isa_visit_32m_instruction(NAME, name) \
                        case NAME##Inst::FUNCT3: \
                            return sub_type()->visit_##name##_inst(reinterpret_cast<NAME##Inst *>(inst));
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

        RetT visit_fence_set(InstructionFenceSet *inst) {
            switch (inst->get_funct3()) {
                case FENCEInst::FUNCT3:
                    return sub_type()->visit_fence_inst(reinterpret_cast<FENCEInst *>(inst));
#define _riscv_isa_visit_zifencei_instruction(NAME, name) \
                        case NAME##Inst::FUNCT3: \
                            return sub_type()->visit_##name##_inst(reinterpret_cast<NAME##Inst *>(inst));
                    riscv_isa_instruction_zifencei_map(_riscv_isa_visit_zifencei_instruction)
#undef _riscv_isa_visit_zifencei_instruction
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_system_set(InstructionSystemSet *inst) {
            switch (inst->get_funct3()) {
                case InstructionPrivilegedSet::FUNCT3:
                    return visit_privilege_set(reinterpret_cast<InstructionPrivilegedSet *>(inst));
#define _riscv_isa_visit_zicsr_instruction(NAME, name) \
                        case NAME##Inst::FUNCT3: \
                            return sub_type()->visit_##name##_inst(reinterpret_cast<NAME##Inst *>(inst));
                riscv_isa_instruction_zicsr_map(_riscv_isa_visit_zicsr_instruction)
#undef _riscv_isa_visit_zicsr_instruction
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

        RetT visit_privilege_set(InstructionPrivilegedSet *inst) {
            switch (inst->get_funct7()) {
                case 0b0000000:
                    if (inst->get_rd() != 0 || inst->get_rs1() != 0)
                        return sub_type()->illegal_instruction(inst);
                    switch (inst->get_rs2()) {
                        case ECALLInst::FUNCT_PRIVILEGE:
                            return sub_type()->visit_ecall_inst(reinterpret_cast<ECALLInst *>(inst));
                        case EBREAKInst::FUNCT_PRIVILEGE:
                            return sub_type()->visit_ebreak_inst(reinterpret_cast<EBREAKInst *>(inst));
#if defined(__RV_EXTENSION_N__)
                            case URETInst::FUNCT_PRIVILEGE:
                                return sub_type()->visit_uret_inst(reinterpret_cast<URETInst *>(inst));
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
                            return sub_type()->visit_sret_inst(reinterpret_cast<SRETInst *>(inst));
#endif // defined(__RV_SUPERVISOR_MODE__)
                        case WFIInst::FUNCT_PRIVILEGE:
                            return sub_type()->visit_wfi_inst(reinterpret_cast<WFIInst *>(inst));
                        default:
                            return sub_type()->illegal_instruction(inst);
                    }
                case MRETInst::FUNCT7:
                    if (inst->get_rd() != 0 || inst->get_rs1() != 0)
                        return sub_type()->illegal_instruction(inst);
                    return sub_type()->visit_mret_inst(reinterpret_cast<MRETInst *>(inst));
#if defined(__RV_SUPERVISOR_MODE__)
                case SFENCEVAMInst::FUNCT7:
                    if (inst->get_rd() != 0) return sub_type()->illegal_instruction(inst);
                    return sub_type()->visit_sfencevma_inst(reinterpret_cast<SFENCEVAMInst *>(inst));
#endif // defined(__RV_SUPERVISOR_MODE__)
#if defined(__RV_HYPERVISOR_MODE__)
                    case HFENCEVVMAInst::FUNCT7:
                        if (inst->get_rd() != 0) return sub_type()->illegal_instruction(inst);
                        return sub_type()->visit_hfencevvma_inst(reinterpret_cast<HFENCEVVMAInst *>(inst));
                    case HFENCEGVMAInst::FUNCT7:
                        if (inst->get_rd() != 0) return sub_type()->illegal_instruction(inst);
                        return sub_type()->visit_hfencegvma_inst(reinterpret_cast<HFENCEGVMAInst *>(inst));
#endif
                default:
                    return sub_type()->illegal_instruction(inst);
            }
        }

#if defined(__RV_EXTENSION_A__)

        RetT visit_atomic_set(InstructionAtomicSet *inst) {
            switch (inst->get_funct3()) {
                case InstructionAtomicWSet::FUNCT3:
                    switch (inst->get_funct_atomic()) {
#define _riscv_isa_visit_32a_instruction(NAME, name) \
                        case NAME##Inst::FUNCT_ATOMIC: \
                            return sub_type()->visit_##name##_inst(reinterpret_cast<NAME##Inst *>(inst));
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
#define _riscv_isa_visit_64m_instruction(NAME, name) \
                        case NAME##Inst::FUNCT3: \
                            return sub_type()->visit_##name##_inst(reinterpret_cast<NAME##Inst *>(inst));
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

        RetT illegal_instruction(riscv_isa_unused Instruction *inst) {
            riscv_isa_abort("Illegal instruction met!");
        }

    public:
///     this function is required to be implemented if any of visit_##name##_inst function is not being overwritten.
///
///     RetT visit_inst(riscv_isa_unused Instruction *inst) {
///         riscv_isa_unreachable("Uncaught instruction in visitor definition!");
///     }

#define _riscv_isa_visit_instruction(NAME, name) \
        RetT visit_##name##_inst(NAME##Inst *inst) { return sub_type()->visit_inst(inst); }

        riscv_isa_instruction_map(_riscv_isa_visit_instruction)

#undef _riscv_isa_visit_instruction
    };
}


#endif //RISCV_ISA_INSTRUCTION_VISITOR_HPP
