#ifndef RISCV_ISA_HART_HPP
#define RISCV_ISA_HART_HPP


#include "instruction/instruction_visitor.hpp"
#include "register/register_file.hpp"
#include "memory/memory.hpp"


namespace riscv_isa {
    enum InternalInterrupt {
        NONE,
        INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION,
        ILLEGAL_INSTRUCTION_EXCEPTION,
        MEMORY_ERROR,
        ECALL,
        EBREAK,
    };

    template<typename SubT>
    class Hart : public InstructionVisitor<SubT, InternalInterrupt> {
    protected:
        RegisterFile &reg;
        Memory &mem;

    public:
        using RetT = InternalInterrupt;

        Hart(RegisterFile &reg, Memory &mem) : reg{reg}, mem{mem} {}

        RetT illegal_instruction(riscv_isa_unused Instruction *inst) {
            return ILLEGAL_INSTRUCTION_EXCEPTION;
        }

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
            return (*inst)(reg) ? NONE : INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION;
        }

        RetT visit_bne_inst(BNEInst *inst) {
            return (*inst)(reg) ? NONE : INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION;
        }

        RetT visit_blt_inst(BLTInst *inst) {
            return (*inst)(reg) ? NONE : INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION;
        }

        RetT visit_bge_inst(BGEInst *inst) {
            return (*inst)(reg) ? NONE : INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION;
        }

        RetT visit_bltu_inst(BLTUInst *inst) {
            return (*inst)(reg) ? NONE : INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION;
        }

        RetT visit_bgeu_inst(BGEUInst *inst) {
            return (*inst)(reg) ? NONE : INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION;
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

        RetT visit_fence_inst(riscv_isa_unused FENCEInst *inst) {
            return NONE; // todo
        }

        RetT visit_ecall_inst(riscv_isa_unused ECALLInst *inst) { return ECALL; }

        RetT visit_ebreak_inst(riscv_isa_unused EBREAKInst *inst) { return EBREAK; }

#if defined(__RV_EXTENSION_M__)

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


#endif //RISCV_ISA_HART_HPP
