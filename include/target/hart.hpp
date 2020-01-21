#ifndef RISC_V_ISA_HART_HPP
#define RISC_V_ISA_HART_HPP


#include "instruction/instruction_visitor.hpp"
#include "register/register_file.hpp"
#include "memory/memory.hpp"


namespace risc_v_isa {
    enum InternalInterrupt {
        NONE,
        INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION,
        ILLEGAL_INSTRUCTION_EXCEPTION,
        MEMORY_ERROR,
        ECALL,
        EBREAK,
    };

    class Hart : public InstructionVisitor<Hart, InternalInterrupt> {
    private:
        RegisterFile reg;
        Memory &mem;

    public:
        Hart(RegisterFile &reg, Memory &mem) : reg{reg}, mem{mem} {}

        void start() {
            while (true) {
                Instruction *inst = mem.address<Instruction>(reg.get_pc());

                switch (inst == nullptr ? MEMORY_ERROR : visit(inst)) {
                    case ILLEGAL_INSTRUCTION_EXCEPTION:
                        std::cerr << "Illegal instruction at " << std::hex << reg.get_pc() << ' '
                                  << *reinterpret_cast<u32 *>(inst) << std::endl;

                        return;
                    case MEMORY_ERROR:
                        std::cerr << "Memory error at " << std::hex << reg.get_pc() << std::endl;

                        return;
                    case ECALL:
                        switch (reg.get_x(RegisterFile::A0)) {
                            case 1:
                                std::cout << std::dec << reg.get_x(RegisterFile::A1);
                                break;
                            case 11:
                                std::cout << static_cast<char>(reg.get_x(RegisterFile::A1));
                                break;
                            case 10:
                                std::cout << std::endl << "[exit]" << std::endl;

                                return;
                            default:
                                std::cerr << "Invalid ecall number at " << std::hex << reg.get_pc() << ' '
                                          << *reinterpret_cast<u32 *>(inst) << std::endl;

                                return;
                        }
                        reg.inc_pc(ECALLInst::INST_WIDTH);

                        break;
                    case EBREAK:
                        reg.inc_pc(EBREAKInst::INST_WIDTH);

                        break;
                    default:;
                }
            }
        }

        RetT illegal_instruction(Instruction *inst) {
            return ILLEGAL_INSTRUCTION_EXCEPTION;
        }

#if defined(__RV_32_BIT__) || defined(__RV_64_BIT__)

        RetT visit_lui_inst(LUIInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_auipc_inst(AUIPCInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_jal_inst(JALInst *inst) {
            return (*inst)(reg) ? NONE : INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION;
        }

        RetT visit_jalr_inst(JALRInst *inst) {
            return (*inst)(reg) ? NONE : INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION;
        }

        RetT visit_beq_inst(BEQInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_bne_inst(BNEInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_blt_inst(BLTInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_bge_inst(BGEInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_bltu_inst(BLTUInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_bgeu_inst(BGEUInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_lb_inst(LBInst *inst) { return (*inst)(reg, mem) ? NONE : MEMORY_ERROR; }

        RetT visit_lh_inst(LHInst *inst) { return (*inst)(reg, mem) ? NONE : MEMORY_ERROR; }

        RetT visit_lw_inst(LWInst *inst) { return (*inst)(reg, mem) ? NONE : MEMORY_ERROR; }

        RetT visit_lbu_inst(LBUInst *inst) { return (*inst)(reg, mem) ? NONE : MEMORY_ERROR; }

        RetT visit_lhu_inst(LHUInst *inst) { return (*inst)(reg, mem) ? NONE : MEMORY_ERROR; }

        RetT visit_sb_inst(SBInst *inst) { return (*inst)(reg, mem) ? NONE : MEMORY_ERROR; }

        RetT visit_sh_inst(SHInst *inst) { return (*inst)(reg, mem) ? NONE : MEMORY_ERROR; }

        RetT visit_sw_inst(SWInst *inst) { return (*inst)(reg, mem) ? NONE : MEMORY_ERROR; }

        RetT visit_addi_inst(ADDIInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_slti_inst(SLTIInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_sltiu_inst(SLTIUInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_xori_inst(XORIInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_ori_inst(ORIInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_andi_inst(ANDIInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_slli_inst(SLLIInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_srli_inst(SRLIInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_srai_inst(SRAIInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_add_inst(ADDInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_sub_inst(SUBInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_sll_inst(SLLInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_slt_inst(SLTInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_sltu_inst(SLTUInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_xor_inst(XORInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_srl_inst(SRLInst *inst) {
            std::cout << *inst << std::endl;
            (*inst)(reg);
            return NONE;
        }

        RetT visit_sra_inst(SRAInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_or_inst(ORInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_and_inst(ANDInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_fence_inst(FENCEInst *inst) {
            return NONE; // todo
        }

        RetT visit_ecall_inst(ECALLInst *inst) {
            return ECALL;
        }

#endif // defined(__RV_32_BIT__) || defined(__RV_64_BIT__)
#if defined(__RV_EXTENSION_M__)

        RetT visit_ebreak_inst(EBREAKInst *inst) {
            return EBREAK;
        }

        RetT visit_mul_inst(MULInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_mulh_inst(MULHInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_mulhsu_inst(MULHSUInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_mulhu_inst(MULHUInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_div_inst(DIVInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_divu_inst(DIVUInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_rem_inst(REMInst *inst) {
            (*inst)(reg);
            return NONE;
        }

        RetT visit_remu_inst(REMUInst *inst) {
            (*inst)(reg);
            return NONE;
        }

#endif // defined(__RV_EXTENSION_M__)
    };
}


#endif //RISC_V_ISA_HART_HPP
