#ifndef RISCV_ISA_HART_HPP
#define RISCV_ISA_HART_HPP


#include "utility.hpp"
#include "operators.hpp"
#include "instruction/instruction_visitor.hpp"
#include "register/register.hpp"
#include "trap/trap.hpp"


namespace riscv_isa {
    template<typename SubT, typename xlen=xlen_trait>
    class Hart : public InstructionVisitor<SubT, bool> {
    public:
        using RetT = bool;

    private:
        xlen_trait::XLenT pc;

        SubT *sub_type() { return static_cast<SubT *>(this); }

    protected:
        using XLenT = typename xlen::XLenT;
        using UXLenT = typename xlen::UXLenT;
        static constexpr usize XLEN = xlen::XLEN;

///        template<typename ValT>
///        RetT mmu_load_int_reg(riscv_isa_unused usize dest, riscv_isa_unused XLenT addr) {
///            csr_reg.scause = LOAD_PAGE_FAULT;
///            return false;
///        }
///
///        template<typename ValT>
///        RetT mmu_store_int_reg(riscv_isa_unused usize src, riscv_isa_unused XLenT addr) {
///            csr_reg.scause = STORE_AMO_PAGE_FAULT;
///            return false;
///        }
///
///        template<usize offset>
///        RetT mmu_load_inst_half(riscv_isa_unused XLenT addr) {
///            csr_reg.scause = INSTRUCTION_PAGE_FAULT;
///            return false;
///        }

        template<typename OP, typename InstT>
        RetT operate_reg(const InstT *inst) {
            usize rd = inst->get_rd();
            if (rd != 0) {
                usize rs1 = inst->get_rs1();
                usize rs2 = inst->get_rs2();
                int_reg.set_x(rd, OP::op(int_reg.get_x(rs1), int_reg.get_x(rs2)));
            }
            inc_pc(InstT::INST_WIDTH);

            return true;
        }

        template<typename OP, typename InstT>
        RetT operate_imm(const InstT *inst) {
            usize rd = inst->get_rd();
            if (rd != 0) {
                usize rs1 = inst->get_rs1();
                XLenT imm = inst->get_imm();
                int_reg.set_x(rd, OP::op(int_reg.get_x(rs1), imm));
            }
            inc_pc(InstT::INST_WIDTH);

            return true;
        }

        template<typename OP, typename InstT>
        RetT operate_branch(const InstT *inst) {
            usize rs1 = inst->get_rs1();
            usize rs2 = inst->get_rs2();

            if (OP::op(int_reg.get_x(rs1), int_reg.get_x(rs2))) {
                XLenT imm = inst->get_imm();
#if IALIGN == 32
                if (get_bits<UXLenT, 2, 0>(imm) != 0) {
                    csr_reg.scause = trap::INSTRUCTION_ADDRESS_MISALIGNED;
                    return false;
                }
#endif
                inc_pc(imm);
            } else {
                inc_pc(InstT::INST_WIDTH);
            }

            return true;
        }

        template<typename ValT, typename InstT>
        RetT operate_load(const InstT *inst) {
            usize rd = inst->get_rd();
            usize rs1 = inst->get_rs1();
            XLenT imm = inst->get_imm();
            if (!sub_type()->template mmu_load_int_reg<ValT>(rd, int_reg.get_x(rs1) + imm)) return false;
            pc += InstT::INST_WIDTH;
            return true;
        }

        template<typename ValT, typename InstT>
        RetT operate_store(const InstT *inst) {
            usize rs1 = inst->get_rs1();
            usize rs2 = inst->get_rs2();
            XLenT imm = inst->get_imm();
            if (!sub_type()->template mmu_store_int_reg<ValT>(rs2, int_reg.get_x(rs1) + imm)) return false;
            inc_pc(InstT::INST_WIDTH);
            return true;
        }

        template<typename OP, typename InstT>
        RetT operate_imm_shift(const InstT *inst) {
            usize rd = inst->get_rd();
            if (rd != 0) {
                usize rs1 = inst->get_rs1();
                XLenT imm = inst->get_shift_amount();
                int_reg.set_x(rd, OP::op(int_reg.get_x(rs1), imm));
            }
            inc_pc(InstT::INST_WIDTH);

            return true;
        }

        IntegerRegister<xlen> &int_reg;
        CSRRegister<xlen> csr_reg;
        ILenT inst_buffer;

        XLenT get_pc() const { return pc; }

        void set_pc(XLenT val) { pc = val; }

        void inc_pc(XLenT val) { set_pc(get_pc() + val); }

    public:
        Hart(XLenT pc, IntegerRegister<xlen> &reg) : pc{pc}, int_reg{reg}, csr_reg{} {}

        RetT visit() {
            if (!sub_type()->template mmu_load_inst_half<0>(get_pc())) return false;

            if ((this->inst_buffer & bits_mask<u16, 2, 0>::val) != bits_mask<u16, 2, 0>::val) {
#if defined(__RV_EXTENSION_C__)
                return this->visit_16(reinterpret_cast<Instruction16 *>(&this->buffer));
#else
                return this->sub_type()->illegal_instruction(reinterpret_cast<Instruction *>(&this->inst_buffer));
#endif // defined(__RV_EXTENSION_C__)
            } else if ((this->inst_buffer & bits_mask<u16, 5, 2>::val) != bits_mask<u16, 5, 2>::val) {
                if (!sub_type()->template mmu_load_inst_half<1>(get_pc())) return false;

                return this->visit_32(reinterpret_cast<Instruction32 *>(&this->inst_buffer));
            } else {
                return sub_type()->illegal_instruction(reinterpret_cast<Instruction *>(&this->inst_buffer));
            }
        }

        RetT illegal_instruction(riscv_isa_unused Instruction *inst) {
            csr_reg.scause = trap::ILLEGAL_INSTRUCTION;
            return false;
        }

        RetT visit_lui_inst(LUIInst *inst) {
            usize rd = inst->get_rd();
            if (rd != 0) {
                XLenT imm = inst->get_imm();
                int_reg.set_x(rd, imm);
            }
            inc_pc(LUIInst::INST_WIDTH);

            return true;
        }

        RetT visit_auipc_inst(AUIPCInst *inst) {
            usize rd = inst->get_rd();
            if (rd != 0) {
                XLenT imm = inst->get_imm();
                int_reg.set_x(rd, imm + get_pc());
            }
            inc_pc(LUIInst::INST_WIDTH);

            return true;
        }

        RetT visit_jal_inst(JALInst *inst) {
            usize rd = inst->get_rd();
            XLenT imm = inst->get_imm();
#if IALIGN == 32
            if (get_bits<UXLenT, 2, 0>(imm) != 0) {
                csr_reg.scause = trap::INSTRUCTION_ADDRESS_MISALIGNED;
                return false;
            }
#endif
            XLenT pc = get_pc();
            if (rd != 0) int_reg.set_x(rd, pc + JALInst::INST_WIDTH);
            set_pc(pc + imm);

            return true;
        }

        RetT visit_jalr_inst(JALRInst *inst) {
            usize rd = inst->get_rd();
            usize rs1 = inst->get_rs1();
            XLenT imm = inst->get_imm();
            UXLenT target = get_bits<UXLenT, XLEN, 1, 1>(int_reg.get_x(rs1) + imm);
#if IALIGN == 32
            if (get_bits<UXLenT, 2, 0>(target) != 0) {
                csr_reg.scause = trap::INSTRUCTION_ADDRESS_MISALIGNED;
                return false;
            }
#endif
            if (rd != 0) int_reg.set_x(rd, get_pc() + JALRInst::INST_WIDTH);
            set_pc(target);

            return true;
        }

        RetT visit_beq_inst(BEQInst *inst) { return operate_branch<typename operators::EQ<xlen>>(inst); }

        RetT visit_bne_inst(BNEInst *inst) { return operate_branch<typename operators::NE<xlen>>(inst); }

        RetT visit_blt_inst(BLTInst *inst) { return operate_branch<typename operators::LT<xlen>>(inst); }

        RetT visit_bge_inst(BGEInst *inst) { return operate_branch<typename operators::GE<xlen>>(inst); }

        RetT visit_bltu_inst(BLTUInst *inst) { return operate_branch<typename operators::LTU<xlen>>(inst); }

        RetT visit_bgeu_inst(BGEUInst *inst) { return operate_branch<typename operators::GEU<xlen>>(inst); }

        RetT visit_lb_inst(LBInst *inst) { return operate_load<i8>(inst); }

        RetT visit_lh_inst(LHInst *inst) { return operate_load<i16>(inst); }

        RetT visit_lw_inst(LWInst *inst) { return operate_load<i32>(inst); }

        RetT visit_lbu_inst(LBUInst *inst) { return operate_load<u8>(inst); }

        RetT visit_lhu_inst(LHUInst *inst) { return operate_load<u16>(inst); }

        RetT visit_sb_inst(SBInst *inst) { return operate_store<u8>(inst); }

        RetT visit_sh_inst(SHInst *inst) { return operate_store<u16>(inst); }

        RetT visit_sw_inst(SWInst *inst) { return operate_store<u32>(inst); }

        RetT visit_addi_inst(ADDIInst *inst) { return operate_imm<typename operators::ADD<xlen>>(inst); }

        RetT visit_slti_inst(SLTIInst *inst) { return operate_imm<typename operators::SLT<xlen>>(inst); }

        RetT visit_sltiu_inst(SLTIUInst *inst) { return operate_imm<typename operators::SLTU<xlen>>(inst); }

        RetT visit_xori_inst(XORIInst *inst) { return operate_imm<typename operators::XOR<xlen>>(inst); }

        RetT visit_ori_inst(ORIInst *inst) { return operate_imm<typename operators::OR<xlen>>(inst); }

        RetT visit_andi_inst(ANDIInst *inst) { return operate_imm<typename operators::AND<xlen>>(inst); }

        RetT visit_slli_inst(SLLIInst *inst) { return operate_imm_shift<typename operators::SLL<xlen>>(inst); }

        RetT visit_srli_inst(SRLIInst *inst) { return operate_imm_shift<typename operators::SRL<xlen>>(inst); }

        RetT visit_srai_inst(SRAIInst *inst) { return operate_imm_shift<typename operators::SRA<xlen>>(inst); }

        RetT visit_add_inst(ADDInst *inst) { return operate_reg<typename operators::ADD<xlen>>(inst); }

        RetT visit_sub_inst(SUBInst *inst) { return operate_reg<typename operators::SUB<xlen>>(inst); }

        RetT visit_sll_inst(SLLInst *inst) { return operate_reg<typename operators::SLL<xlen>>(inst); }

        RetT visit_slt_inst(SLTInst *inst) { return operate_reg<typename operators::SLT<xlen>>(inst); }

        RetT visit_sltu_inst(SLTUInst *inst) { return operate_reg<typename operators::SLTU<xlen>>(inst); }

        RetT visit_xor_inst(XORInst *inst) { return operate_reg<typename operators::XOR<xlen>>(inst); }

        RetT visit_srl_inst(SRLInst *inst) { return operate_reg<typename operators::SRL<xlen>>(inst); }

        RetT visit_sra_inst(SRAInst *inst) { return operate_reg<typename operators::SRA<xlen>>(inst); }

        RetT visit_or_inst(ORInst *inst) { return operate_reg<typename operators::OR<xlen>>(inst); }

        RetT visit_and_inst(ANDInst *inst) { return operate_reg<typename operators::AND<xlen>>(inst); }

        RetT visit_fence_inst(riscv_isa_unused FENCEInst *inst) { return true; } // todo

        RetT visit_ecall_inst(riscv_isa_unused ECALLInst *inst) {
            csr_reg.scause = trap::U_MODE_ENVIRONMENT_CALL;
            return false;
        }

        RetT visit_ebreak_inst(riscv_isa_unused EBREAKInst *inst) {
            csr_reg.scause = trap::BREAKPOINT;
            return false;
        }

#if defined(__RV_EXTENSION_M__)

        RetT visit_mul_inst(MULInst *inst) { return operate_reg<typename operators::MUL<xlen>>(inst); }

        RetT visit_mulh_inst(MULHInst *inst) { return operate_reg<typename operators::MULH<xlen>>(inst); }

        RetT visit_mulhsu_inst(MULHSUInst *inst) { return operate_reg<typename operators::MULHSU<xlen>>(inst); }

        RetT visit_mulhu_inst(MULHUInst *inst) { return operate_reg<typename operators::MULHU<xlen>>(inst); }

        RetT visit_div_inst(DIVInst *inst) { return operate_reg<typename operators::DIV<xlen>>(inst); }

        RetT visit_divu_inst(DIVUInst *inst) { return operate_reg<typename operators::DIVU<xlen>>(inst); }

        RetT visit_rem_inst(REMInst *inst) { return operate_reg<typename operators::REM<xlen>>(inst); }

        RetT visit_remu_inst(REMUInst *inst) { return operate_reg<typename operators::REMU<xlen>>(inst); }

#endif // defined(__RV_EXTENSION_M__)
#if __RV_BIT_WIDTH__ == 64

        RetT visit_ld_inst(LDInst *inst) { return operate_load<i64>(inst); }

        RetT visit_lwu_inst(LWUInst *inst) { return operate_load<u32>(inst); }

        RetT visit_sd_inst(SDInst *inst) { return operate_store<u64>(inst); }

        RetT visit_addiw_inst(ADDIWInst *inst) { return operate_imm<typename operators::ADD<xlen_32_trait>>(inst); }

        RetT visit_slliw_inst(SLLIWInst *inst) { return operate_imm<typename operators::SLL<xlen_32_trait>>(inst); }

        RetT visit_srliw_inst(SRLIWInst *inst) { return operate_imm<typename operators::SRL<xlen_32_trait>>(inst); }

        RetT visit_sraiw_inst(SRAIWInst *inst) { return operate_imm<typename operators::SRA<xlen_32_trait>>(inst); }

        RetT visit_addw_inst(ADDWInst *inst) { return operate_reg<typename operators::ADD<xlen_32_trait>>(inst); }

        RetT visit_subw_inst(SUBWInst *inst) { return operate_reg<typename operators::SUB<xlen_32_trait>>(inst); }

        RetT visit_sllw_inst(SLLWInst *inst) { return operate_reg<typename operators::SLL<xlen_32_trait>>(inst); }

        RetT visit_srlw_inst(SRLWInst *inst) { return operate_reg<typename operators::SRL<xlen_32_trait>>(inst); }

        RetT visit_sraw_inst(SRAWInst *inst) { return operate_reg<typename operators::SRA<xlen_32_trait>>(inst); }

#if defined(__RV_EXTENSION_M__)

        RetT visit_mulw_inst(MULWInst *inst) { return operate_reg<typename operators::MUL<xlen_32_trait>>(inst); }

        RetT visit_divw_inst(DIVWInst *inst) { return operate_reg<typename operators::DIV<xlen_32_trait>>(inst); }

        RetT visit_divuw_inst(DIVUWInst *inst) { return operate_reg<typename operators::DIVU<xlen_32_trait>>(inst); }

        RetT visit_remw_inst(REMWInst *inst) { return operate_reg<typename operators::REM<xlen_32_trait>>(inst); }

        RetT visit_remuw_inst(REMUWInst *inst) { return operate_reg<typename operators::REMU<xlen_32_trait>>(inst); }

#endif // defined(__RV_EXTENSION_M__)
#endif // __RV_BIT_WIDTH__ == 64
    };
}


#endif //RISCV_ISA_HART_HPP
