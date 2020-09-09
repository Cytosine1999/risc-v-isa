#ifndef RISCV_ISA_HART_HPP
#define RISCV_ISA_HART_HPP


#include <atomic>

#include "riscv_isa_utility.hpp"
#include "operators.hpp"
#include "instruction/instruction_visitor.hpp"
#include "register/register.hpp"
#include "trap/trap.hpp"


namespace riscv_isa {
    template<typename SubT, typename xlen>
    class Hart : public InstructionVisitor<SubT, bool> {
    public:
        using RetT = bool;
        using IntRegT = IntegerRegister<xlen>;
        using CSRRegT = CSRRegister<xlen>;
        using XLenT = typename xlen::XLenT;
        using UXLenT = typename xlen::UXLenT;
        static constexpr usize XLEN = xlen::XLEN;

    private:
        IntRegT int_reg;
        XLenT pc;
#if defined(__RV_EXTENSION_A__)
        UXLenT reserve_address, reserve_value;
#endif

    protected:
        CSRRegT csr_reg;
        PrivilegeLevel cur_level;

    private:
        SubT *sub_type() {
            static_assert(std::is_base_of<Hart, SubT>::value, "not subtype of visitor");

            return static_cast<SubT *>(this);
        }

        template<typename OP, typename InstT>
        RetT operate_branch(const InstT *inst) {
            typename InstT::UInnerT rs1 = inst->get_rs1();
            typename InstT::UInnerT rs2 = inst->get_rs2();

            if (OP::op(sub_type()->get_x(rs1), sub_type()->get_x(rs2))) {
                XLenT imm = inst->get_imm();
                UXLenT target = imm + sub_type()->get_pc();
                return sub_type()->jump_to_addr(target);
            } else {
                sub_type()->inc_pc(InstT::INST_WIDTH);
                return true;
            }
        }

        template<typename ValT, typename InstT>
        RetT operate_load(const InstT *inst) {
            static_assert(sizeof(ValT) <= sizeof(UXLenT), "load width exceed bit width!");

            typename InstT::UInnerT rd = inst->get_rd();
            typename InstT::UInnerT rs1 = inst->get_rs1();
            XLenT imm = inst->get_imm();
            UXLenT addr = sub_type()->get_x(rs1) + imm;

            if ((addr & (sizeof(ValT) - 1)) != 0) {
                return sub_type()->internal_interrupt(trap::LOAD_ACCESS_FAULT, addr);
            }

            auto *ptr = sub_type()->template address_load<ValT>(addr);
            if (ptr == nullptr) {
                return sub_type()->internal_interrupt(trap::LOAD_PAGE_FAULT, addr);
            } else {
                if (rd != 0) { sub_type()->set_x(rd, *ptr); }
            }

            sub_type()->inc_pc(InstT::INST_WIDTH);
            return true;
        }

        template<typename ValT, typename InstT>
        RetT operate_store(const InstT *inst) {
            static_assert(sizeof(ValT) <= sizeof(UXLenT), "store width exceed bit width!");

            typename InstT::UInnerT rs1 = inst->get_rs1();
            typename InstT::UInnerT rs2 = inst->get_rs2();
            XLenT imm = inst->get_imm();
            UXLenT addr = sub_type()->get_x(rs1) + imm;

            if ((addr & (sizeof(ValT) - 1)) != 0) {
                return sub_type()->internal_interrupt(trap::STORE_AMO_ACCESS_FAULT, addr);
            }

            auto *ptr = sub_type()->template address_store<ValT>(addr);
            if (ptr == nullptr) {
                return sub_type()->internal_interrupt(trap::STORE_AMO_PAGE_FAULT, addr);
            } else {
                *ptr = static_cast<ValT>(sub_type()->get_x(rs2));
            }

            sub_type()->inc_pc(InstT::INST_WIDTH);
            return true;
        }

        template<typename OP, typename InstT>
        RetT operate_imm(const InstT *inst) {
            typename InstT::UInnerT rd = inst->get_rd();
            if (rd != 0) {
                typename InstT::UInnerT rs1 = inst->get_rs1();
                XLenT imm = inst->get_imm();
                set_x(rd, OP::op(sub_type()->get_x(rs1), imm));
            }
            sub_type()->inc_pc(InstT::INST_WIDTH);

            return true;
        }

        template<typename OP, typename InstT>
        RetT operate_reg(const InstT *inst) {
            typename InstT::UInnerT rd = inst->get_rd();
            if (rd != 0) {
                typename InstT::UInnerT rs1 = inst->get_rs1();
                typename InstT::UInnerT rs2 = inst->get_rs2();
                set_x(rd, OP::op(sub_type()->get_x(rs1), sub_type()->get_x(rs2)));
            }
            sub_type()->inc_pc(InstT::INST_WIDTH);

            return true;
        }

        template<typename OP, typename InstT>
        RetT operate_imm_shift(const InstT *inst) {
            typename InstT::UInnerT rd = inst->get_rd();
            if (rd != 0) {
                typename InstT::UInnerT rs1 = inst->get_rs1();
                XLenT imm = inst->get_shamt();
                set_x(rd, OP::op(sub_type()->get_x(rs1), imm));
            }
            sub_type()->inc_pc(InstT::INST_WIDTH);

            return true;
        }

    public:
        Hart(UXLenT hart_id, XLenT pc, IntRegT &reg) :
                int_reg{reg}, pc{pc},
#if defined(__RV_EXTENSION_A__)
                reserve_address{0}, reserve_value{0},
#endif
                csr_reg{hart_id}, cur_level{MACHINE_MODE} {}

///     these functions are required to be implemented.
///
///     if interrupt generates in following three memory related functions,
///     false should be returned and internal interrupt should be invoked
///     explicitly.
///
///     template<typename ValT>
///     const ValT *address_load(UXLenT addr) {
///         riscv_isa_unreachable("address load permission undefined!");
///     }
///
///     template<typename ValT>
///     ValT *address_store(UXLenT addr) {
///         riscv_isa_unreachable("address store permission undefined!");
///     }
///
///     template<typename ValT>
///     const ValT *address_execute(UXLenT addr) {
///         riscv_isa_unreachable("address execute permission undefined!");
///     }

        RetT visit() {
            ILenT inst_buffer = 0; // zeroing instruction buffer
            UXLenT addr = sub_type()->get_pc();

#if RISCV_IALIGN == 32
            auto *ptr = sub_type()->template address_execute<u32>(addr);
            if (ptr == nullptr) {
                return sub_type()->internal_interrupt(trap::INSTRUCTION_PAGE_FAULT, addr);
            }
            inst_buffer = *ptr;
#else
            auto *ptr = sub_type()->template address_execute<u16>(addr);
            if (ptr == nullptr) {
                return sub_type()->internal_interrupt(trap::INSTRUCTION_PAGE_FAULT, addr);
            }
            inst_buffer = *ptr;

#if defined(__RV_EXTENSION_C__)
            if (is_type<Instruction16>(reinterpret_cast<Instruction *>(&inst_buffer))) {
                return this->visit_16(reinterpret_cast<Instruction16 *>(&inst_buffer));
            }
#endif // defined(__RV_EXTENSION_C__)

            ptr = sub_type()->template address_execute<u16>(addr + sizeof(u16));
            if (ptr == nullptr) {
                return sub_type()->internal_interrupt(trap::INSTRUCTION_PAGE_FAULT, addr);
            }
            inst_buffer |= static_cast<u32>(*ptr) << 16u;
#endif

            if (is_type<Instruction32>(reinterpret_cast<Instruction *>(&inst_buffer))) {
                return this->visit_32(reinterpret_cast<Instruction32 *>(&inst_buffer));
            }

            return sub_type()->illegal_instruction(reinterpret_cast<Instruction *>(&inst_buffer));
        }

        XLenT get_pc() const { return pc; }

        bool jump_to_addr(XLenT val) {
            pc = val;
            return true;
        }

        void inc_pc(XLenT val) { pc += val; }

        XLenT get_x(usize index) const { return int_reg.get_x(index); }

        void set_x(usize index, XLenT val) { int_reg.set_x(index, val); }

        void internal_interrupt_action(UXLenT interrupt, UXLenT trap_value) {
            csr_reg[CSRRegT::SCAUSE] = interrupt;
            csr_reg[CSRRegT::STVAL] = trap_value;
        }

        RetT internal_interrupt(UXLenT interrupt, UXLenT trap_value) {
            sub_type()->internal_interrupt_action(interrupt, trap_value);
            return false;
        }

        RetT illegal_instruction(const Instruction *inst) {
            return internal_interrupt(trap::ILLEGAL_INSTRUCTION, *reinterpret_cast<const UXLenT *>(inst));
        }

        RetT visit_lui_inst(const LUIInst *inst) {
            typename LUIInst::UInnerT rd = inst->get_rd();
            if (rd != 0) {
                XLenT imm = inst->get_imm();
                set_x(rd, imm);
            }
            sub_type()->inc_pc(LUIInst::INST_WIDTH);

            return true;
        }

        RetT visit_auipc_inst(const AUIPCInst *inst) {
            typename AUIPCInst::UInnerT rd = inst->get_rd();
            if (rd != 0) {
                XLenT imm = inst->get_imm();
                set_x(rd, imm + sub_type()->get_pc());
            }
            sub_type()->inc_pc(LUIInst::INST_WIDTH);

            return true;
        }

        RetT visit_jal_inst(const JALInst *inst) {
            typename JALInst::UInnerT rd = inst->get_rd();
            XLenT imm = inst->get_imm();
            UXLenT target = imm + sub_type()->get_pc();
            UXLenT save = sub_type()->get_pc() + JALInst::INST_WIDTH;

            if (!sub_type()->jump_to_addr(target)) { return false; }
            if (rd != 0) { set_x(rd, save); }

            return true;
        }

        RetT visit_jalr_inst(const JALRInst *inst) {
            typename JALRInst::UInnerT rd = inst->get_rd();
            typename JALRInst::UInnerT rs1 = inst->get_rs1();
            XLenT imm = inst->get_imm();
            UXLenT target = get_bits<UXLenT, XLEN, 1, 1>(sub_type()->get_x(rs1) + imm);
            UXLenT save = sub_type()->get_pc() + JALInst::INST_WIDTH;

            if (!sub_type()->jump_to_addr(target)) { return false; }
            if (rd != 0) { set_x(rd, save); }

            return true;
        }

        RetT visit_beq_inst(const BEQInst *inst) { return operate_branch<typename operators::EQ<xlen>>(inst); }

        RetT visit_bne_inst(const BNEInst *inst) { return operate_branch<typename operators::NE<xlen>>(inst); }

        RetT visit_blt_inst(const BLTInst *inst) { return operate_branch<typename operators::LT<xlen>>(inst); }

        RetT visit_bge_inst(const BGEInst *inst) { return operate_branch<typename operators::GE<xlen>>(inst); }

        RetT visit_bltu_inst(const BLTUInst *inst) { return operate_branch<typename operators::LTU<xlen>>(inst); }

        RetT visit_bgeu_inst(const BGEUInst *inst) { return operate_branch<typename operators::GEU<xlen>>(inst); }

        RetT visit_lb_inst(const LBInst *inst) { return operate_load<i8>(inst); }

        RetT visit_lh_inst(const LHInst *inst) { return operate_load<i16>(inst); }

        RetT visit_lw_inst(const LWInst *inst) { return operate_load<i32>(inst); }

        RetT visit_lbu_inst(const LBUInst *inst) { return operate_load<u8>(inst); }

        RetT visit_lhu_inst(const LHUInst *inst) { return operate_load<u16>(inst); }

        RetT visit_sb_inst(const SBInst *inst) { return operate_store<u8>(inst); }

        RetT visit_sh_inst(const SHInst *inst) { return operate_store<u16>(inst); }

        RetT visit_sw_inst(const SWInst *inst) { return operate_store<u32>(inst); }

        RetT visit_addi_inst(const ADDIInst *inst) { return operate_imm<typename operators::ADD<xlen>>(inst); }

        RetT visit_slti_inst(const SLTIInst *inst) { return operate_imm<typename operators::SLT<xlen>>(inst); }

        RetT visit_sltiu_inst(const SLTIUInst *inst) { return operate_imm<typename operators::SLTU<xlen>>(inst); }

        RetT visit_xori_inst(const XORIInst *inst) { return operate_imm<typename operators::XOR<xlen>>(inst); }

        RetT visit_ori_inst(const ORIInst *inst) { return operate_imm<typename operators::OR<xlen>>(inst); }

        RetT visit_andi_inst(const ANDIInst *inst) { return operate_imm<typename operators::AND<xlen>>(inst); }

        RetT visit_slli_inst(const SLLIInst *inst) { return operate_imm_shift<typename operators::SLL<xlen>>(inst); }

        RetT visit_srli_inst(const SRLIInst *inst) { return operate_imm_shift<typename operators::SRL<xlen>>(inst); }

        RetT visit_srai_inst(const SRAIInst *inst) { return operate_imm_shift<typename operators::SRA<xlen>>(inst); }

        RetT visit_add_inst(const ADDInst *inst) { return operate_reg<typename operators::ADD<xlen>>(inst); }

        RetT visit_sub_inst(const SUBInst *inst) { return operate_reg<typename operators::SUB<xlen>>(inst); }

        RetT visit_sll_inst(const SLLInst *inst) { return operate_reg<typename operators::SLL<xlen>>(inst); }

        RetT visit_slt_inst(const SLTInst *inst) { return operate_reg<typename operators::SLT<xlen>>(inst); }

        RetT visit_sltu_inst(const SLTUInst *inst) { return operate_reg<typename operators::SLTU<xlen>>(inst); }

        RetT visit_xor_inst(const XORInst *inst) { return operate_reg<typename operators::XOR<xlen>>(inst); }

        RetT visit_srl_inst(const SRLInst *inst) { return operate_reg<typename operators::SRL<xlen>>(inst); }

        RetT visit_sra_inst(const SRAInst *inst) { return operate_reg<typename operators::SRA<xlen>>(inst); }

        RetT visit_or_inst(const ORInst *inst) { return operate_reg<typename operators::OR<xlen>>(inst); }

        RetT visit_and_inst(const ANDInst *inst) { return operate_reg<typename operators::AND<xlen>>(inst); }

        RetT visit_ecall_inst(riscv_isa_unused const ECALLInst *inst) {
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

        RetT visit_ebreak_inst(riscv_isa_unused const EBREAKInst *inst) {
            return internal_interrupt(trap::BREAKPOINT, sub_type()->get_pc());
        }

#if defined(__RV_EXTENSION_M__)

        RetT visit_mul_inst(const MULInst *inst) { return operate_reg<typename operators::MUL<xlen>>(inst); }

        RetT visit_mulh_inst(const MULHInst *inst) { return operate_reg<typename operators::MULH<xlen>>(inst); }

        RetT visit_mulhsu_inst(const MULHSUInst *inst) { return operate_reg<typename operators::MULHSU<xlen>>(inst); }

        RetT visit_mulhu_inst(const MULHUInst *inst) { return operate_reg<typename operators::MULHU<xlen>>(inst); }

        RetT visit_div_inst(const DIVInst *inst) { return operate_reg<typename operators::DIV<xlen>>(inst); }

        RetT visit_divu_inst(const DIVUInst *inst) { return operate_reg<typename operators::DIVU<xlen>>(inst); }

        RetT visit_rem_inst(const REMInst *inst) { return operate_reg<typename operators::REM<xlen>>(inst); }

        RetT visit_remu_inst(const REMUInst *inst) { return operate_reg<typename operators::REMU<xlen>>(inst); }

#endif // defined(__RV_EXTENSION_M__)
#if defined(__RV_EXTENSION_A__)

        template<typename OP, typename InstT>
        RetT operate_atomic(const InstT *inst) {
            using ValT = typename OP::XLenT;
            static_assert(sizeof(ValT) <= sizeof(UXLenT), "load width exceed bit width!");

            typename InstT::UInnerT rd = inst->get_rd();
            typename InstT::UInnerT rs1 = inst->get_rs1();
            typename InstT::UInnerT rs2 = inst->get_rs2();
            UXLenT rs2_value = sub_type()->get_x(rs2);
            UXLenT addr = sub_type()->get_x(rs1);

            if ((addr & (sizeof(ValT) - 1)) != 0) {
                return sub_type()->internal_interrupt(trap::STORE_AMO_ACCESS_FAULT, addr);
            }

            auto *ptr = sub_type()->template address_store<std::atomic<ValT>>(addr);
            if (ptr == nullptr) {
                return sub_type()->internal_interrupt(trap::STORE_AMO_PAGE_FAULT, addr);
            }

            set_x(rd, OP::op(ptr, rs2_value));

            sub_type()->inc_pc(InstT::INST_WIDTH);
            return true;
        }

        // todo: only comare the value, memory ordering.
        RetT visit_lrw_inst(const LRWInst *inst) {
            typename LRWInst::UInnerT rd = inst->get_rd();
            typename LRWInst::UInnerT rs1 = inst->get_rs1();
            UXLenT addr = sub_type()->get_x(rs1);

            if ((addr & (sizeof(u32) - 1)) != 0) {
                return sub_type()->internal_interrupt(trap::LOAD_ACCESS_FAULT, addr);
            }

            auto *ptr = sub_type()->template address_load<u32>(addr);
            if (ptr == nullptr) {
                return sub_type()->internal_interrupt(trap::LOAD_PAGE_FAULT, addr);
            } else {
                auto value = *ptr;
                reserve_address = addr;
                reserve_value = value;
                if (rd != 0) { set_x(rd, value); }
            }

            sub_type()->inc_pc(LRWInst::INST_WIDTH);
            return true;
        }

        RetT visit_scw_inst(const SCWInst *inst) {
            typename SCWInst::UInnerT rd = inst->get_rd();
            typename SCWInst::UInnerT rs1 = inst->get_rs1();
            typename SCWInst::UInnerT rs2 = inst->get_rs2();
            UXLenT addr = sub_type()->get_x(rs1);

            if ((addr & (sizeof(u32) - 1)) != 0) {
                return sub_type()->internal_interrupt(trap::STORE_AMO_ACCESS_FAULT, addr);
            }

            auto *ptr = sub_type()->template address_store<std::atomic<u32>>(addr);
            if (ptr == nullptr) {
                return sub_type()->internal_interrupt(trap::STORE_AMO_PAGE_FAULT, addr);
            } else {
                if (reserve_address == addr &&
                    ptr->compare_exchange_weak(reserve_value, sub_type()->get_x(rs2))) {
                    set_x(rd, 0);
                } else {
                    set_x(rd, 1);
                }
            }

            sub_type()->inc_pc(SCWInst::INST_WIDTH);
            return true;
        }

        RetT visit_amoswapw_inst(const AMOSWAPWInst *inst) {
            return operate_atomic<typename operators::AMOSWAP<xlen_32_trait>>(inst);
        }

        RetT visit_amoaddw_inst(const AMOADDWInst *inst) {
            return operate_atomic<typename operators::AMOADD<xlen_32_trait>>(inst);
        }

        RetT visit_amoxorw_inst(const AMOXORWInst *inst) {
            return operate_atomic<typename operators::AMOXOR<xlen_32_trait>>(inst);
        }

        RetT visit_amoandw_inst(const AMOANDWInst *inst) {
            return operate_atomic<typename operators::AMOAND<xlen_32_trait>>(inst);
        }

        RetT visit_amoorw_inst(const AMOORWInst *inst) {
            return operate_atomic<typename operators::AMOOR<xlen_32_trait>>(inst);
        }

        RetT visit_amominw_inst(const AMOMINWInst *inst) {
            return operate_atomic<typename operators::AMOMIN<xlen_32_trait>>(inst);
        }

        RetT visit_amomaxw_inst(const AMOMAXWInst *inst) {
            return operate_atomic<typename operators::AMOMAX<xlen_32_trait>>(inst);
        }

        RetT visit_amominuw_inst(const AMOMINUWInst *inst) {
            return operate_atomic<typename operators::AMOMINU<xlen_32_trait>>(inst);
        }

        RetT visit_amomaxuw_inst(const AMOMAXUWInst *inst) {
            return operate_atomic<typename operators::AMOMAXU<xlen_32_trait>>(inst);
        }

#endif // defined(__RV_EXTENSION_A__)
#if __RV_BIT_WIDTH__ == 64

        RetT visit_ld_inst(const LDInst *inst) { return operate_load<i64>(inst); }

        RetT visit_lwu_inst(const LWUInst *inst) { return operate_load<u32>(inst); }

        RetT visit_sd_inst(const SDInst *inst) { return operate_store<u64>(inst); }

        RetT visit_addiw_inst(const ADDIWInst *inst) {
            return operate_imm<typename operators::ADD<xlen_32_trait>>(inst);
        }

        RetT visit_slliw_inst(const SLLIWInst *inst) {
            return operate_imm<typename operators::SLL<xlen_32_trait>>(inst);
        }

        RetT visit_srliw_inst(const SRLIWInst *inst) {
            return operate_imm<typename operators::SRL<xlen_32_trait>>(inst);
        }

        RetT visit_sraiw_inst(const SRAIWInst *inst) {
            return operate_imm<typename operators::SRA<xlen_32_trait>>(inst);
        }

        RetT visit_addw_inst(const ADDWInst *inst) {
            return operate_reg<typename operators::ADD<xlen_32_trait>>(inst);
        }

        RetT visit_subw_inst(const SUBWInst *inst) {
            return operate_reg<typename operators::SUB<xlen_32_trait>>(inst);
        }

        RetT visit_sllw_inst(const SLLWInst *inst) {
            return operate_reg<typename operators::SLL<xlen_32_trait>>(inst);
        }

        RetT visit_srlw_inst(const SRLWInst *inst) {
            return operate_reg<typename operators::SRL<xlen_32_trait>>(inst);
        }

        RetT visit_sraw_inst(const SRAWInst *inst) {
            return operate_reg<typename operators::SRA<xlen_32_trait>>(inst);
        }

#if defined(__RV_EXTENSION_M__)

        RetT visit_mulw_inst(const MULWInst *inst) {
            return operate_reg<typename operators::MUL<xlen_32_trait>>(inst);
        }

        RetT visit_divw_inst(const DIVWInst *inst) {
            return operate_reg<typename operators::DIV<xlen_32_trait>>(inst);
        }

        RetT visit_divuw_inst(const DIVUWInst *inst) {
            return operate_reg<typename operators::DIVU<xlen_32_trait>>(inst);
        }

        RetT visit_remw_inst(const REMWInst *inst) {
            return operate_reg<typename operators::REM<xlen_32_trait>>(inst);
        }

        RetT visit_remuw_inst(const REMUWInst *inst) {
            return operate_reg<typename operators::REMU<xlen_32_trait>>(inst);
        }

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

        RetT visit_csrrw_inst(const CSRRWInst *inst) {
            using UInnerT = typename CSRRWInst::UInnerT;

            UInnerT rd = inst->get_rd();
            UInnerT rs1 = inst->get_rs1();
            UInnerT csr = inst->get_csr();

            UInnerT index = check_csr(csr);
            if (index >= CSRRegT::CSR_REGISTER_NUM) return illegal_instruction(inst);

            if (rd != 0) set_x(rd, get_csr(index));
            return set_csr(index, sub_type()->get_x(rs1));
        }

        RetT visit_csrrs_inst(const CSRRSInst *inst) {
            using UInnerT = typename CSRRSInst::UInnerT;

            UInnerT rd = inst->get_rd();
            UInnerT rs1 = inst->get_rs1();
            UInnerT csr = inst->get_csr();

            UInnerT index = check_csr(csr);
            if (index >= CSRRegT::CSR_REGISTER_NUM) return illegal_instruction(inst);

            UXLenT csr_val = get_csr(index);
            if (rs1 != 0) {
                if (CSRRegT::get_read_write_bits(csr) == CSRRegT::READ_ONLY_BITS) return illegal_instruction(inst);
                if (rd != 0) set_x(rd, csr_val);
                return set_csr(index, csr_val | sub_type()->get_x(rs1));
            } else {
                if (rd != 0) set_x(rd, csr_val);
                return true;
            }
        }

        RetT visit_csrrc_inst(const CSRRCInst *inst) {
            using UInnerT = typename CSRRCInst::UInnerT;

            UInnerT rd = inst->get_rd();
            UInnerT rs1 = inst->get_rs1();
            UInnerT csr = inst->get_csr();

            UInnerT index = check_csr(csr);
            if (index >= CSRRegT::CSR_REGISTER_NUM) return illegal_instruction(inst);

            UXLenT csr_val = get_csr(index);
            if (rs1 != 0) {
                if (CSRRegT::get_read_write_bits(csr) == CSRRegT::READ_ONLY_BITS) return illegal_instruction(inst);
                if (rd != 0) set_x(rd, csr_val);
                return set_csr(index, csr_val & ~sub_type()->get_x(rs1));
            } else {
                if (rd != 0) set_x(rd, csr_val);
                return true;
            }
        }

        RetT visit_csrrwi_inst(const CSRRWIInst *inst) {
            using UInnerT = typename CSRRWIInst::UInnerT;

            UInnerT rd = inst->get_rd();
            UInnerT imm = inst->get_rs1();
            UInnerT csr = inst->get_csr();

            UInnerT index = check_csr(csr);
            if (index >= CSRRegT::CSR_REGISTER_NUM) return illegal_instruction(inst);

            if (rd != 0) set_x(rd, get_csr(index));
            return set_csr(index, imm);
        }

        RetT visit_csrrsi_inst(const CSRRSIInst *inst) {
            using UInnerT = typename CSRRSIInst::UInnerT;

            UInnerT rd = inst->get_rd();
            UInnerT imm = inst->get_rs1();
            UInnerT csr = inst->get_csr();

            UInnerT index = check_csr(csr);
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

        RetT visit_csrrci_inst(const CSRRCIInst *inst) {
            using UInnerT = typename CSRRCIInst::UInnerT;

            UInnerT rd = inst->get_rd();
            UInnerT imm = inst->get_rs1();
            UInnerT csr = inst->get_csr();

            UInnerT index = check_csr(csr);
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
                      << sub_type()->get_pc() << ", jump to: " << addr << std::dec << std::endl;
            return false;
        }

        RetT instruction_access_fault_handler(UXLenT addr) {
            std::cerr << "Instruction access fault at " << std::hex
                      << sub_type()->get_pc() << ", jump to: " << addr << std::dec << std::endl;
            return false;
        }

        RetT illegal_instruction_handler(UXLenT inst) {
            std::cerr << "Illegal instruction at " << std::hex << sub_type()->get_pc() << ": "
                      << std::dec << *reinterpret_cast<Instruction *>(&inst) << std::endl;
            return false;
        }

        RetT break_point_handler(riscv_isa_unused UXLenT addr) {
#if defined(__RV_EXTENSION_C__)
            if (is_type<const CEBREAKInst>(sub_type()->template address_execute<Instruction16>(addr))) {
                sub_type()->inc_pc(CEBREAKInst::INST_WIDTH);
                return true;
            }
#endif

            if (is_type<const EBREAKInst>(sub_type()->template address_execute<Instruction32>(addr))) {
                sub_type()->inc_pc(EBREAKInst::INST_WIDTH);
                return true;
            }

            riscv_isa_unreachable("Break point trap at non break point instruction!");
        }

        RetT load_address_misaligned_handler(UXLenT addr) {
            std::cerr << "Load address misaligned at " << std::hex << sub_type()->get_pc()
                      << ", load at: " << addr << std::dec << std::endl;
            return false;
        }

        RetT load_access_fault_handler(UXLenT addr) {
            std::cerr << "Load address access fault at " << std::hex << sub_type()->get_pc()
                      << ", load at: " << addr << std::dec << std::endl;
            return false;
        }

        RetT store_amo_address_misaligned_handler(UXLenT addr) {
            std::cerr << "Store or AMO address misaligned at " << std::hex << sub_type()->get_pc()
                      << ", store or AMO at: " << addr << std::dec << std::endl;
            return false;
        }

        RetT store_amo_access_fault_handler(UXLenT addr) {
            std::cerr << "Store or AMO access fault at " << std::hex << sub_type()->get_pc()
                      << ", store or AMO at: " << addr << std::dec << std::endl;
            return false;
        }

        RetT u_mode_environment_call_handler() {
            std::cerr << "User mode environment call at " << std::hex
                      << sub_type()->get_pc() << std::dec << std::endl;
            return false;
        }

        RetT s_mode_environment_call_handler() {
            std::cerr << "Supervisor mode environment call at " << std::hex
                      << sub_type()->get_pc() << std::dec << std::endl;
            return false;
        }

        RetT m_mode_environment_call_handler() {
            std::cerr << "Machine mode environment call at " << std::hex
                      << sub_type()->get_pc() << std::dec << std::endl;
            return false;
        }

        RetT instruction_page_fault_handler(UXLenT addr) {
            std::cerr << "Instruction page fault at " << std::hex << addr << std::dec << std::endl;
            return false;
        }

        RetT load_page_fault_handler(UXLenT addr) {
            std::cerr << "Load page fault at " << std::hex << sub_type()->get_pc()
                      << ", load address: " << addr << std::dec << std::endl;
            return false;
        }

        RetT store_amo_page_fault_handler(UXLenT addr) {
            std::cerr << "Store or AMO fault at " << std::hex << sub_type()->get_pc()
                      << ", load address: " << addr << std::dec << std::endl;
            return false;
        }

        RetT platformed_specified_trap_handler(UXLenT cause, UXLenT trap_value) {
            std::cerr << "Unknown internal interrupt at " << std::hex
                      << sub_type()->get_pc() << std::dec << ", cause: " << cause
                      << ", trap value" << trap_value << std::endl;
            return false;
        }

        RetT trap_handler() {
            RetT ret;

            switch (csr_reg[CSRRegT::SCAUSE]) {
                case trap::INSTRUCTION_ADDRESS_MISALIGNED:
                    ret = sub_type()->instruction_address_misaligned_handler(csr_reg[CSRRegT::STVAL]);
                    break;
                case trap::INSTRUCTION_ACCESS_FAULT:
                    ret = sub_type()->instruction_access_fault_handler(csr_reg[CSRRegT::STVAL]);
                    break;
                case trap::ILLEGAL_INSTRUCTION:
                    ret = sub_type()->illegal_instruction_handler(csr_reg[CSRRegT::STVAL]);
                    break;
                case trap::BREAKPOINT:
                    ret = sub_type()->break_point_handler(csr_reg[CSRRegT::STVAL]);
                    break;
                case trap::LOAD_ADDRESS_MISALIGNED:
                    ret = sub_type()->load_address_misaligned_handler(csr_reg[CSRRegT::STVAL]);
                    break;
                case trap::LOAD_ACCESS_FAULT:
                    ret = sub_type()->load_access_fault_handler(csr_reg[CSRRegT::STVAL]);
                    break;
                case trap::STORE_AMO_ADDRESS_MISALIGNED:
                    ret = sub_type()->store_amo_address_misaligned_handler(csr_reg[CSRRegT::STVAL]);
                    break;
                case trap::STORE_AMO_ACCESS_FAULT:
                    ret = sub_type()->store_amo_access_fault_handler(csr_reg[CSRRegT::STVAL]);
                    break;
                case trap::U_MODE_ENVIRONMENT_CALL:
                    ret = sub_type()->u_mode_environment_call_handler();
                    break;
                case trap::S_MODE_ENVIRONMENT_CALL:
                    ret = sub_type()->s_mode_environment_call_handler();
                    break;
                case trap::M_MODE_ENVIRONMENT_CALL:
                    ret = sub_type()->m_mode_environment_call_handler();
                    break;
                case trap::INSTRUCTION_PAGE_FAULT:
                    ret = sub_type()->instruction_page_fault_handler(csr_reg[CSRRegT::STVAL]);
                    break;
                case trap::LOAD_PAGE_FAULT:
                    ret = sub_type()->load_page_fault_handler(csr_reg[CSRRegT::STVAL]);
                    break;
                case trap::STORE_AMO_PAGE_FAULT:
                    ret = sub_type()->store_amo_page_fault_handler(csr_reg[CSRRegT::STVAL]);
                    break;
                default:
                    ret = sub_type()->platformed_specified_trap_handler(csr_reg[CSRRegT::SCAUSE],
                                                                        csr_reg[CSRRegT::STVAL]);
            }

            return ret;
        }

        void start() { while (sub_type()->visit() || sub_type()->trap_handler()) {}}
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
