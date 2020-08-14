#ifndef RISCV_ISA_HART_HPP
#define RISCV_ISA_HART_HPP


#include "riscv_isa_utility.hpp"
#include "operators.hpp"
#include "instruction/instruction_visitor.hpp"
#include "register/register.hpp"
#include "trap/trap.hpp"


namespace riscv_isa {
    template<typename SubT, typename xlen=xlen_trait>
    class Hart : public InstructionVisitor<SubT, bool> {
    public:
        using RetT = bool;
        using IntRegT = IntegerRegister<xlen>;
        using CSRRegT = CSRRegister<xlen>;
        using XLenT = typename xlen::XLenT;
        using UXLenT = typename xlen::UXLenT;
        static constexpr usize XLEN = xlen::XLEN;

    private:
        xlen_trait::XLenT pc;
        IntRegT int_reg;

    protected:
        CSRRegT csr_reg;
        ILenT inst_buffer;
        PrivilegeLevel cur_level;

    private:
        SubT *sub_type() {
            static_assert(std::is_base_of<Hart, SubT>::value, "not subtype of hart!");

            return static_cast<SubT *>(this);
        }

    protected:
        XLenT get_pc() const { return pc; }

        void set_pc(XLenT val) { pc = val; }

        void inc_pc(XLenT val) { pc += val; }

        XLenT get_x(usize index) const { return int_reg.get_x(index); }

        void set_x(usize index, XLenT val) { int_reg.set_x(index, val); }

///     these functions are required to be implemented.
///
///     void internal_interrupt_action(riscv_isa_unused UXLenT interrupt, riscv_isa_unused UXLenT trap_value) {
///         riscv_isa_unreachable("internal interrupt action undefined!");
///     }
///
///     if interrupt generates in following three memory related functions, false should be returned and internal
///     interrupt should be invoked explicitly.
///
///     template<typename ValT>
///     RetT mmu_load_int_reg(riscv_isa_unused usize dest, riscv_isa_unused XLenT addr) {
///         riscv_isa_unreachable("memory management unit load integer register undefined!");
///     }
///
///     template<typename ValT>
///     RetT mmu_store_int_reg(riscv_isa_unused usize src, riscv_isa_unused XLenT addr) {
///         riscv_isa_unreachable("memory management unit store integer register undefined!");
///     }
///
///     template<usize offset>
///     RetT mmu_load_inst_half(riscv_isa_unused XLenT addr) {
///         riscv_isa_unreachable("memory management unit load instruction two byte undefined!");
///     }

        void internal_interrupt_action(UXLenT interrupt, UXLenT trap_value) {
            csr_reg[CSRRegT::SCAUSE] = interrupt;
            csr_reg[CSRRegT::STVAL] = trap_value;
        }

        RetT internal_interrupt(UXLenT interrupt, UXLenT trap_value) {
            sub_type()->internal_interrupt_action(interrupt, trap_value);
            return false;
        }

        template<typename OP, typename InstT>
        RetT operate_reg(const InstT *inst) {
            usize rd = inst->get_rd();
            if (rd != 0) {
                usize rs1 = inst->get_rs1();
                usize rs2 = inst->get_rs2();
                set_x(rd, OP::op(get_x(rs1), get_x(rs2)));
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
                set_x(rd, OP::op(get_x(rs1), imm));
            }
            inc_pc(InstT::INST_WIDTH);

            return true;
        }

        template<typename OP, typename InstT>
        RetT operate_branch(const InstT *inst) {
            usize rs1 = inst->get_rs1();
            usize rs2 = inst->get_rs2();

            if (OP::op(get_x(rs1), get_x(rs2))) {
                XLenT imm = inst->get_imm();
                UXLenT target = imm + get_pc();
#if RISCV_IALIGN == 32
                if (get_bits<UXLenT, 2, 0>(target) != 0)
                    return internal_interrupt(trap::INSTRUCTION_ADDRESS_MISALIGNED, target);
#endif
                set_pc(target);
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
            if (!sub_type()->template mmu_load_int_reg<ValT>(rd, get_x(rs1) + imm)) return false;
            inc_pc(InstT::INST_WIDTH);
            return true;
        }

        template<typename ValT, typename InstT>
        RetT operate_store(const InstT *inst) {
            usize rs1 = inst->get_rs1();
            usize rs2 = inst->get_rs2();
            XLenT imm = inst->get_imm();
            if (!sub_type()->template mmu_store_int_reg<ValT>(rs2, get_x(rs1) + imm)) return false;
            inc_pc(InstT::INST_WIDTH);
            return true;
        }

        template<typename OP, typename InstT>
        RetT operate_imm_shift(const InstT *inst) {
            usize rd = inst->get_rd();
            if (rd != 0) {
                usize rs1 = inst->get_rs1();
                XLenT imm = inst->get_shamt();
                set_x(rd, OP::op(get_x(rs1), imm));
            }
            inc_pc(InstT::INST_WIDTH);

            return true;
        }

    public:
        Hart(xlen_trait::UXLenT hart_id, xlen_trait::XLenT pc, IntRegT &reg) :
                pc{pc}, int_reg{reg}, csr_reg{hart_id}, inst_buffer{0}, cur_level{MACHINE_MODE} {}

        RetT visit() {
            static_assert(std::is_base_of<Hart, SubT>::value, "not subtype of visitor");

            inst_buffer = 0; // zeroing instruction buffer

            if (!sub_type()->template mmu_load_inst_half<0>(get_pc())) return false;

            if ((this->inst_buffer & bits_mask<u16, 2, 0>::val) != bits_mask<u16, 2, 0>::val) {
#if defined(__RV_EXTENSION_C__)
                return this->visit_16(reinterpret_cast<Instruction16 *>(&this->inst_buffer));
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
            return internal_interrupt(trap::ILLEGAL_INSTRUCTION, inst_buffer);
        }

        RetT visit_lui_inst(LUIInst *inst) {
            usize rd = inst->get_rd();
            if (rd != 0) {
                XLenT imm = inst->get_imm();
                set_x(rd, imm);
            }
            inc_pc(LUIInst::INST_WIDTH);

            return true;
        }

        RetT visit_auipc_inst(AUIPCInst *inst) {
            usize rd = inst->get_rd();
            if (rd != 0) {
                XLenT imm = inst->get_imm();
                set_x(rd, imm + get_pc());
            }
            inc_pc(LUIInst::INST_WIDTH);

            return true;
        }

        RetT visit_jal_inst(JALInst *inst) {
            usize rd = inst->get_rd();
            XLenT imm = inst->get_imm();
            UXLenT target = imm + get_pc();
#if RISCV_IALIGN == 32
            if (get_bits<UXLenT, 2, 0>(target) != 0)
                return internal_interrupt(trap::INSTRUCTION_ADDRESS_MISALIGNED, target);
#endif

            if (rd != 0) set_x(rd, get_pc() + JALInst::INST_WIDTH);
            set_pc(target);

            return true;
        }

        RetT visit_jalr_inst(JALRInst *inst) {
            usize rd = inst->get_rd();
            usize rs1 = inst->get_rs1();
            XLenT imm = inst->get_imm();
            UXLenT target = get_bits<UXLenT, XLEN, 1, 1>(get_x(rs1) + imm);
#if RISCV_IALIGN == 32
            if (get_bits<UXLenT, 2, 0>(target) != 0)
                return internal_interrupt(trap::INSTRUCTION_ADDRESS_MISALIGNED, target);
#endif
            if (rd != 0) set_x(rd, get_pc() + JALRInst::INST_WIDTH);
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

        RetT visit_ecall_inst(riscv_isa_unused ECALLInst *inst) {
            switch (cur_level) {
#if defined(__RV_USER_MODE__)
                case USER_MODE:
                    return internal_interrupt(trap::U_MODE_ENVIRONMENT_CALL, 0);
#endif
#if defined(__RV_SUPERVISOR_MODE__)
                case SUPERVISOR_MODE:
                    return internal_interrupt(trap::S_MODE_ENVIRONMENT_CALL, 0);
#endif
                case MACHINE_MODE:
                    return internal_interrupt(trap::M_MODE_ENVIRONMENT_CALL, 0);
                default:
                    riscv_isa_unreachable("unrecognized privilege mode!");
            }
        }

        RetT visit_ebreak_inst(riscv_isa_unused EBREAKInst *inst) {
            return internal_interrupt(trap::BREAKPOINT, get_pc());
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
#if defined(__RV_EXTENSION_A__)
///     this functions are required to be implemented.
///
///     template<typename ValT>
///     RetT mmu_store_xlen(XLenT val, riscv_isa_unused XLenT addr) {
///         riscv_isa_unreachable("memory management unit store integer register undefined!");
///     }

        template<typename OP, typename InstT>
        RetT operate_atomic(const InstT *inst) {
            usize rd = inst->get_rd();
            usize rs1 = inst->get_rs1();
            usize rs2 = inst->get_rs2();
            UXLenT rs2_value = get_x(rs2);
            if (!sub_type()->template mmu_load_int_reg<i32>(rd, get_x(rs1))) return false;
            UXLenT result = OP::op(get_x(rd), rs2_value);
            if (!sub_type()->template mmu_store_xlen<i32>(result, get_x(rs1))) return false;
            inc_pc(InstT::INST_WIDTH);
            return true;
        }

        // todo: implement for single thread, assuming success, even incorrect for single thread.
        RetT visit_lrw_inst(LRWInst *inst) {
            usize rd = inst->get_rd();
            usize rs1 = inst->get_rs1();
            if (!sub_type()->template mmu_load_int_reg<i32>(rd, get_x(rs1))) return false;
            inc_pc(LRWInst::INST_WIDTH);
            return true;
        }

        RetT visit_scw_inst(SCWInst *inst) {
            usize rd = inst->get_rd();
            usize rs1 = inst->get_rs1();
            usize rs2 = inst->get_rs2();
            if (!sub_type()->template mmu_store_int_reg<i32>(rs2, get_x(rs1))) return false;
            set_x(rd, 0);
            inc_pc(SCWInst::INST_WIDTH);
            return true;
        }

        RetT visit_amoswapw_inst(AMOSWAPWInst *inst) {
            return operate_atomic<typename operators::SELECT2<xlen_32_trait>>(inst);
        }

        RetT visit_amoaddw_inst(AMOADDWInst *inst) {
            return operate_atomic<typename operators::ADD<xlen_32_trait>>(inst);
        }

        RetT visit_amoxorw_inst(AMOXORWInst *inst) {
            return operate_atomic<typename operators::XOR<xlen_32_trait>>(inst);
        }

        RetT visit_amoandw_inst(AMOANDWInst *inst) {
            return operate_atomic<typename operators::AND<xlen_32_trait>>(inst);
        }

        RetT visit_amoorw_inst(AMOORWInst *inst) {
            return operate_atomic<typename operators::OR<xlen_32_trait>>(inst);
        }

        RetT visit_amominw_inst(AMOMINWInst *inst) {
            return operate_atomic<typename operators::MIN<xlen_32_trait>>(inst);
        }

        RetT visit_amomaxw_inst(AMOMAXWInst *inst) {
            return operate_atomic<typename operators::MAX<xlen_32_trait>>(inst);
        }

        RetT visit_amominuw_inst(AMOMINUWInst *inst) {
            return operate_atomic<typename operators::MINU<xlen_32_trait>>(inst);
        }

        RetT visit_amomaxuw_inst(AMOMAXUWInst *inst) {
            return operate_atomic<typename operators::MAXU<xlen_32_trait>>(inst);
        }

#endif // defined(__RV_EXTENSION_A__)
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
#if defined(__RV_EXTENSION_ZICSR__)
    private:
        /// static wrapper enable putting into array
#define _riscv_isa_static_get_csr(NAME, name, num) \
        static UXLenT _get_##name##_csr_reg(Hart *self) { \
            return self->sub_type()->get_##name##_csr_reg(); \
        }

        riscv_isa_csr_reg_map(_riscv_isa_static_get_csr)

#undef _riscv_isa_static_get_csr

        static UXLenT (*const _get_csr_reg_table[CSRRegT::CSR_REGISTER_NUM])(Hart *);

        UXLenT get_csr(usize index) { return _get_csr_reg_table[index](this); }

        /// static wrapper enable putting into array
#define _riscv_isa_static_set_csr(NAME, name, num) \
        static RetT _set_##name##_csr_reg(Hart *self, UXLenT val) { \
            return self->sub_type()->set_##name##_csr_reg(val); \
        }

        riscv_isa_csr_reg_map(_riscv_isa_static_set_csr)

#undef _riscv_isa_static_set_csr

        static RetT (*const _set_csr_reg_table[CSRRegT::CSR_REGISTER_NUM])(Hart *, UXLenT);

        RetT set_csr(usize index, UXLenT val) { return _set_csr_reg_table[index](this, val); }

        usize check_csr(usize num) {
            if (CSRRegT::get_privilege_bits(num) > cur_level) return CSRRegT::CSR_REGISTER_NUM;
            else return CSRRegT::get_index(num);
        }

    public:
///     default implementation of get_##name##_csr, directly return the register.
///     used for implement default action for some csr.
///     the get_csr function needed to be defined to enable this.
///
///     RetT get_csr_reg(riscv_isa_unused UXLenT index) {
///         riscv_isa_unreachable("get csr register undefined");
///     }

#define _riscv_isa_get_csr(NAME, name, num) \
        UXLenT get_##name##_csr_reg() { return sub_type()->get_csr_reg(CSRRegT::NAME); }

        riscv_isa_csr_reg_map(_riscv_isa_get_csr)

#undef _riscv_isa_get_csr

///     default implementation of set_##name##_csr, calls set_csr.
///     used for implement default action for some csr.
///     the set_csr function needed to be defined to enable this.
///
///     RetT set_csr_reg(riscv_isa_unused UXLenT index, riscv_isa_unused UXLenT val) {
///         riscv_isa_unreachable("write csr register undefined");
///     }

#define _riscv_isa_set_csr(NAME, name, num) \
        RetT set_##name##_csr_reg(UXLenT val) { return sub_type()->set_csr_reg(CSRRegT::NAME, val); }

        riscv_isa_csr_reg_map(_riscv_isa_set_csr)

#undef _riscv_isa_set_csr

        RetT visit_csrrw_inst(CSRRWInst *inst) {
            usize rd = inst->get_rd();
            usize rs1 = inst->get_rs1();
            usize csr = inst->get_csr();

            usize index = check_csr(csr);
            if (index >= CSRRegT::CSR_REGISTER_NUM) return illegal_instruction(inst);

            if (rd != 0) set_x(rd, get_csr(index));
            return set_csr(index, get_x(rs1));
        }

        RetT visit_csrrs_inst(CSRRSInst *inst) {
            usize rd = inst->get_rd();
            usize rs1 = inst->get_rs1();
            usize csr = inst->get_csr();

            usize index = check_csr(csr);
            if (index >= CSRRegT::CSR_REGISTER_NUM) return illegal_instruction(inst);

            UXLenT csr_val = get_csr(index);
            if (rs1 != 0) {
                if (CSRRegT::get_read_write_bits(csr) == CSRRegT::READ_ONLY_BITS) return illegal_instruction(inst);
                if (rd != 0) set_x(rd, csr_val);
                return set_csr(index, csr_val | get_x(rs1));
            } else {
                if (rd != 0) set_x(rd, csr_val);
                return true;
            }
        }

        RetT visit_csrrc_inst(CSRRCInst *inst) {
            usize rd = inst->get_rd();
            usize rs1 = inst->get_rs1();
            usize csr = inst->get_csr();

            usize index = check_csr(csr);
            if (index >= CSRRegT::CSR_REGISTER_NUM) return illegal_instruction(inst);

            UXLenT csr_val = get_csr(index);
            if (rs1 != 0) {
                if (CSRRegT::get_read_write_bits(csr) == CSRRegT::READ_ONLY_BITS) return illegal_instruction(inst);
                if (rd != 0) set_x(rd, csr_val);
                return set_csr(index, csr_val & ~get_x(rs1));
            } else {
                if (rd != 0) set_x(rd, csr_val);
                return true;
            }
        }

        RetT visit_csrrwi_inst(CSRRWIInst *inst) {
            usize rd = inst->get_rd();
            usize imm = inst->get_rs1();
            usize csr = inst->get_csr();

            usize index = check_csr(csr);
            if (index >= CSRRegT::CSR_REGISTER_NUM) return illegal_instruction(inst);

            if (rd != 0) set_x(rd, get_csr(index));
            return set_csr(index, imm);
        }

        RetT visit_csrrsi_inst(CSRRSIInst *inst) {
            usize rd = inst->get_rd();
            usize imm = inst->get_rs1();
            usize csr = inst->get_csr();

            usize index = check_csr(csr);
            if (index >= CSRRegT::CSR_REGISTER_NUM) return illegal_instruction(inst);

            UXLenT csr_val = get_csr(index);

            if (imm != 0) {
                if (CSRRegT::get_read_write_bits(csr) == CSRRegT::READ_ONLY_BITS) return illegal_instruction(inst);
                if (rd != 0) set_x(rd, csr_val);
                return set_csr(index, csr_val | imm);
            } else {
                if (rd != 0) set_x(rd, csr_val);
                return true;
            }
        }

        RetT visit_csrrci_inst(CSRRCIInst *inst) {
            usize rd = inst->get_rd();
            usize imm = inst->get_rs1();
            usize csr = inst->get_csr();

            usize index = check_csr(csr);
            if (index >= CSRRegT::CSR_REGISTER_NUM) return illegal_instruction(inst);

            UXLenT csr_val = get_csr(index);
            if (imm != 0) {
                if (CSRRegT::get_read_write_bits(csr) == CSRRegT::READ_ONLY_BITS) return illegal_instruction(inst);
                set_x(rd, csr_val);
                return set_csr(index, csr_val & ~imm);
            } else {
                set_x(rd, csr_val);
                return true;
            }
        }

#endif // defined(__RV_EXTENSION_ZICSR__)

        RetT instruction_address_misaligned_handler(UXLenT addr) {
            std::cerr << "Instruction address misaligned at " << std::hex
                      << get_pc() << ", jump to: " << addr << std::dec << std::endl;
            return false;
        }

        RetT instruction_access_fault_handler(UXLenT addr) {
            std::cerr << "Instruction access fault at " << std::hex
                      << get_pc() << ", jump to: " << addr << std::dec << std::endl;
            return false;
        }

        RetT illegal_instruction_handler(UXLenT inst) {
            std::cerr << "Illegal instruction at " << std::hex << get_pc() << ": " << std::dec
                      << *reinterpret_cast<Instruction *>(&inst) << std::endl;
            return false;
        }

        bool break_point_handler(neutron_unused UXLenT addr) {
            this->inc_pc(riscv_isa::ECALLInst::INST_WIDTH); // todo: c extension
            return true;
        }

        RetT load_address_misaligned_handler(UXLenT addr) {
            std::cerr << "Load address misaligned at " << std::hex
                      << get_pc() << ", load at: " << addr << std::dec << std::endl;
            return false;
        }

        RetT load_access_fault_handler(UXLenT addr) {
            std::cerr << "Load address access fault at " << std::hex
                      << get_pc() << ", load at: " << addr << std::dec << std::endl;
            return false;
        }

        RetT store_amo_address_misaligned_handler(UXLenT addr) {
            std::cerr << "Store or AMO address misaligned at " << std::hex
                      << get_pc() << ", store or AMO at: " << addr << std::dec << std::endl;
            return false;
        }

        RetT store_amo_access_fault_handler(UXLenT addr) {
            std::cerr << "Store or AMO access fault at " << std::hex
                      << get_pc() << ", store or AMO at: " << addr << std::dec << std::endl;
            return false;
        }

        RetT u_mode_environment_call_handler() {
            std::cerr << "User mode environment call at " << std::hex << get_pc() << std::dec << std::endl;
            return false;
        }

        RetT s_mode_environment_call_handler() {
            std::cerr << "Supervisor mode environment call at " << std::hex << get_pc() << std::dec << std::endl;
            return false;
        }

        RetT m_mode_environment_call_handler() {
            std::cerr << "Machine mode environment call at " << std::hex << get_pc() << std::dec << std::endl;
            return false;
        }

        RetT instruction_page_fault_handler(UXLenT addr) {
            std::cerr << "Instruction page fault at " << std::hex << addr << std::dec << std::endl;
            return false;
        }

        RetT load_page_fault_handler(UXLenT addr) {
            std::cerr << "Load page fault at " << std::hex
                      << get_pc() << ", load address: " << addr << std::dec << std::endl;
            return false;
        }

        RetT store_amo_page_fault_handler(UXLenT addr) {
            std::cerr << "Store or AMO fault at " << std::hex
                      << get_pc() << ", load address: " << addr << std::dec << std::endl;
            return false;
        }

        RetT platformed_specified_trap_handler(UXLenT cause, UXLenT trap_value) {
            std::cerr << "Unknown internal interrupt at " << std::hex
                      << get_pc() << std::dec << ", cause: " << cause
                      << ", trap value" << trap_value << std::endl;
            return false;
        }

        bool trap_handler() {
            switch (csr_reg[CSRRegT::SCAUSE]) {
                case trap::INSTRUCTION_ADDRESS_MISALIGNED:
                    return sub_type()->instruction_address_misaligned_handler(csr_reg[CSRRegT::STVAL]);
                case trap::INSTRUCTION_ACCESS_FAULT:
                    return sub_type()->instruction_access_fault_handler(csr_reg[CSRRegT::STVAL]);
                case trap::ILLEGAL_INSTRUCTION:
                    return sub_type()->illegal_instruction_handler(csr_reg[CSRRegT::STVAL]);
                case trap::BREAKPOINT:
                    return sub_type()->break_point_handler(csr_reg[CSRRegT::STVAL]);
                case trap::LOAD_ADDRESS_MISALIGNED:
                    return sub_type()->load_address_misaligned_handler(csr_reg[CSRRegT::STVAL]);
                case trap::LOAD_ACCESS_FAULT:
                    return sub_type()->load_access_fault_handler(csr_reg[CSRRegT::STVAL]);
                case trap::STORE_AMO_ADDRESS_MISALIGNED:
                    return sub_type()->store_amo_address_misaligned_handler(csr_reg[CSRRegT::STVAL]);
                case trap::STORE_AMO_ACCESS_FAULT:
                    return sub_type()->store_amo_access_fault_handler(csr_reg[CSRRegT::STVAL]);
                case trap::U_MODE_ENVIRONMENT_CALL:
                    return sub_type()->u_mode_environment_call_handler();
                case trap::S_MODE_ENVIRONMENT_CALL:
                    return sub_type()->s_mode_environment_call_handler();
                case trap::M_MODE_ENVIRONMENT_CALL:
                    return sub_type()->m_mode_environment_call_handler();
                case trap::INSTRUCTION_PAGE_FAULT:
                    return sub_type()->instruction_page_fault_handler(csr_reg[CSRRegT::STVAL]);
                case trap::LOAD_PAGE_FAULT:
                    return sub_type()->load_page_fault_handler(csr_reg[CSRRegT::STVAL]);
                case trap::STORE_AMO_PAGE_FAULT:
                    return sub_type()->store_amo_page_fault_handler(csr_reg[CSRRegT::STVAL]);
                default:
                    return sub_type()->platformed_specified_trap_handler(csr_reg[CSRRegT::SCAUSE],
                                                                         csr_reg[CSRRegT::STVAL]);
            }
        }
    };

    template<typename SubT, typename xlen>
    typename xlen::UXLenT (*const Hart<SubT, xlen>::_get_csr_reg_table[])(Hart *) = {
#define _riscv_isa_get_csr_table(NAME, name, num) \
            _get_##name##_csr_reg,
            riscv_isa_csr_reg_map(_riscv_isa_get_csr_table)
#undef _riscv_isa_get_csr_table
    };

    template<typename SubT, typename xlen>
    typename Hart<SubT, xlen>::RetT (*const Hart<SubT, xlen>::_set_csr_reg_table[])(Hart *, UXLenT) = {
#define _riscv_isa_set_csr_table(NAME, name, num) \
            _set_##name##_csr_reg,
            riscv_isa_csr_reg_map(_riscv_isa_set_csr_table)
#undef _riscv_isa_set_csr_table
    };
}


#endif //RISCV_ISA_HART_HPP
