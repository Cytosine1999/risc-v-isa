#ifndef RISCV_ISA_HART_HPP
#define RISCV_ISA_HART_HPP


#include "instruction/instruction_visitor.hpp"
#include "register/integer_register.hpp"
//#include "register/register.hpp"
#include "memory/memory.hpp"
#include "trap/trap.hpp"


namespace riscv_isa {
    enum InternalInterrupt {
        NONE,
        INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION,
        ILLEGAL_INSTRUCTION_EXCEPTION,
        MEMORY_ERROR,
        ECALL,
        EBREAK,
    };

    template<typename xlen=xlen_trait>
    struct operators {
    private:
        using XLenT = typename xlen::XLenT;
        using UXLenT = typename xlen::UXLenT;
        static constexpr usize HALF_WIDTH = xlen::HALF_WIDTH;
        static constexpr UXLenT HALF_MASK = xlen::HALF_MASK;
        static constexpr XLenT XLenTMin = xlen::XLenTMin;

    public:
        struct EQ {
            static bool op(XLenT a, XLenT b) { return a == b; }
        };

        struct NE {
            static bool op(XLenT a, XLenT b) { return a != b; }
        };

        struct LT {
            static bool op(XLenT a, XLenT b) { return a < b; }
        };

        struct GE {
            static bool op(XLenT a, XLenT b) { return a >= b; }
        };

        struct LTU {
            static bool op(UXLenT a, UXLenT b) { return a < b; }
        };

        struct GEU {
            static bool op(UXLenT a, UXLenT b) { return a >= b; }
        };

        struct ADD {
            static XLenT op(XLenT a, XLenT b) {
                return a + b;
            }
        };

        struct SUB {
            static XLenT op(XLenT a, XLenT b) { return a - b; }
        };

        struct SLT {
            static XLenT op(XLenT a, XLenT b) { return a < b; }
        };

        struct SLTU {
            static XLenT op(UXLenT a, UXLenT b) { return a < b; }
        };

        struct XOR {
            static XLenT op(UXLenT a, UXLenT b) { return a ^ b; }
        };

        struct OR {
            static XLenT op(UXLenT a, UXLenT b) { return a | b; }
        };

        struct AND {
            static XLenT op(UXLenT a, UXLenT b) { return a & b; }
        };

        struct SLL {
            static XLenT op(UXLenT a, UXLenT b) { return a << b; }
        };

        struct SRL {
            static XLenT op(UXLenT a, UXLenT b) { return a >> b; }
        };

        struct SRA {
            static XLenT op(XLenT a, UXLenT b) { return a >> b; }
        };

#if defined(__RV_EXTENSION_M__)

        struct MUL {
            static XLenT op(XLenT a, XLenT b) { return a * b; }
        };

        struct MULH {
            static XLenT op(XLenT a, XLenT b) {
                UXLenT as = a < 0;
                UXLenT au = (~as + 1) ^(a - as);
                UXLenT bs = b < 0;
                UXLenT bu = (~bs + 1) ^(b - bs);
                UXLenT a0 = au & HALF_MASK;
                UXLenT a1 = au >> HALF_WIDTH;
                UXLenT b0 = bu & HALF_MASK;
                UXLenT b1 = bu >> HALF_WIDTH;

                UXLenT c0 = a0 * b0;
                UXLenT c1 = a1 * b0;
                UXLenT c2 = a0 * b1;
                UXLenT c3 = a1 * b1;

                UXLenT d0 = (c0 >> HALF_WIDTH) + c1;
                UXLenT d1 = (d0 & HALF_MASK) + c2;
                UXLenT lh = (d1 << HALF_WIDTH) + (c0 & HALF_MASK);
                UXLenT hh = (d0 >> HALF_WIDTH) + (d1 >> HALF_WIDTH) + c3;

                UXLenT s = as ^bs;
                UXLenT lhs = lh - s;
                UXLenT hhs = -s ^(hh - (lhs > lh));

                return hhs;
            }
        };

        struct MULHSU {
            static XLenT op(XLenT a, UXLenT b) {
                UXLenT as = a < 0;
                UXLenT au = (~as + 1) ^(a - as);
                UXLenT a0 = au & HALF_MASK;
                UXLenT a1 = au >> HALF_WIDTH;
                UXLenT b0 = b & HALF_MASK;
                UXLenT b1 = b >> HALF_WIDTH;

                UXLenT c0 = a0 * b0;
                UXLenT c1 = a1 * b0;
                UXLenT c2 = a0 * b1;
                UXLenT c3 = a1 * b1;

                UXLenT d0 = (c0 >> HALF_WIDTH) + c1;
                UXLenT d1 = (d0 & HALF_MASK) + c2;
                UXLenT lh = (d1 << HALF_WIDTH) + (c0 & HALF_MASK);
                UXLenT hh = (d0 >> HALF_WIDTH) + (d1 >> HALF_WIDTH) + c3;

                UXLenT s = as;
                UXLenT lhs = lh - s;
                UXLenT hhs = -s ^(hh - (lhs > lh));

                return hhs;
            }
        };

        struct MULHU {
            static XLenT op(UXLenT a, UXLenT b) {
                UXLenT a0 = a & HALF_MASK;
                UXLenT a1 = a >> HALF_WIDTH;
                UXLenT b0 = b & HALF_MASK;
                UXLenT b1 = b >> HALF_WIDTH;

                UXLenT c0 = a0 * b0;
                UXLenT c1 = a1 * b0;
                UXLenT c2 = a0 * b1;
                UXLenT c3 = a1 * b1;

                UXLenT d0 = (c0 >> HALF_WIDTH) + c1;
                UXLenT d1 = (d0 & HALF_MASK) + c2;
                UXLenT hh = (d0 >> HALF_WIDTH) + (d1 >> HALF_WIDTH) + c3;

                return hh;
            }
        };

        struct DIV {
            static XLenT op(XLenT a, XLenT b) {
                if (b == 0) return -1;
                else if (a == XLenTMin && b == -1) return XLenTMin;
                return a / b;
            }
        };

        struct DIVU {
            static XLenT op(UXLenT a, UXLenT b) { return b == 0 ? -1 : a / b; }
        };

        struct REM {
            static XLenT op(XLenT a, XLenT b) {
                if (b == 0) return a;
                else if (a == XLenTMin && b == -1) return 0;
                return a % b;
            }
        };

        struct REMU {
            static XLenT op(UXLenT a, UXLenT b) { return b == 0 ? a : a % b; }
        };

#endif // defined(__RV_EXTENSION_M__)
    };

    template<typename SubT, typename xlen=xlen_trait>
    class Hart : public InstructionVisitor<SubT, xlen, InternalInterrupt> {
    public:
        using RetT = InternalInterrupt;

    private:
        using XLenT = typename xlen::XLenT;
        using UXLenT = typename xlen::UXLenT;
        static constexpr usize XLEN_INDEX = xlen::XLEN_INDEX;
        static constexpr usize XLEN_BYTE = xlen::XLEN_BYTE;
        static constexpr usize XLEN = xlen::XLEN;

        template<typename OP, typename InstT>
        RetT operate_register(const InstT *inst) {
            usize rd = inst->get_rd();
            if (rd != 0) {
                usize rs1 = inst->get_rs1();
                usize rs2 = inst->get_rs2();
                reg.set_x(rd, OP::op(reg.get_x(rs1), reg.get_x(rs2)));
            }
            reg.inc_pc(InstT::INST_WIDTH);

            return NONE;
        }

        template<typename OP, typename InstT>
        RetT operate_immediate(const InstT *inst) {
            usize rd = inst->get_rd();
            if (rd != 0) {
                usize rs1 = inst->get_rs1();
                XLenT imm = inst->get_imm();
                reg.set_x(rd, OP::op(reg.get_x(rs1), imm));
            }
            reg.inc_pc(InstT::INST_WIDTH);

            return NONE;
        }

        template<typename OP, typename InstT>
        RetT operate_branch(const InstT *inst) {
            usize rs1 = inst->get_rs1();
            usize rs2 = inst->get_rs2();

            if (OP::op(reg.get_x(rs1), reg.get_x(rs2))) {
                XLenT imm = inst->get_imm();
#if IALIGN == 32
                if (get_bits<UXLenT, 2, 0>(imm) != 0) return INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION;
#endif
                reg.inc_pc(imm);
            } else {
                reg.inc_pc(InstT::INST_WIDTH);
            }

            return NONE;
        }

        template<typename ValT, typename InstT>
        RetT operate_load(const InstT *inst) {
            usize rd = inst->get_rd();
            usize rs1 = inst->get_rs1();
            XLenT imm = inst->get_imm();
            ValT *ptr = mem.template address<ValT>(static_cast<UXLenT>(reg.get_x(rs1)) + imm);
            if (ptr == nullptr) return MEMORY_ERROR;
            if (rd != 0) reg.set_x(rd, *ptr);
            reg.inc_pc(InstT::INST_WIDTH);
            return NONE;
        }

        template<typename ValT, typename InstT>
        RetT operate_store(const InstT *inst) const {
            usize rs1 = inst->get_rs1();
            usize rs2 = inst->get_rs2();
            XLenT imm = inst->get_imm();
            ValT *ptr = mem.template address<ValT>(static_cast<UXLenT>(reg.get_x(rs1)) + imm);
            if (ptr == nullptr) return MEMORY_ERROR;
            *ptr = static_cast<ValT>(reg.get_x(rs2));
            reg.inc_pc(InstT::INST_WIDTH);
            return NONE;
        }

        template<typename OP, typename InstT>
        RetT operate_immediate_shift(const InstT *inst) {
            usize rd = inst->get_rd();
            if (rd != 0) {
                usize rs1 = inst->get_rs1();
                XLenT imm = inst->get_shift_amount();
                reg.set_x(rd, OP::op(reg.get_x(rs1), imm));
            }
            reg.inc_pc(InstT::INST_WIDTH);

            return NONE;
        }

    protected:
        IntegerRegister<xlen> &reg;
        Memory<xlen> &mem;

    public:
        Hart(IntegerRegister<xlen> &reg, Memory<xlen> &mem) : reg{reg}, mem{mem} {}

        RetT illegal_instruction(riscv_isa_unused Instruction *inst) {
            return ILLEGAL_INSTRUCTION_EXCEPTION;
        }

        RetT visit_lui_inst(LUIInst *inst) {
            usize rd = inst->get_rd();
            if (rd != 0) {
                XLenT imm = inst->get_imm();
                reg.set_x(rd, imm);
            }
            reg.inc_pc(LUIInst::INST_WIDTH);

            return NONE;
        }

        RetT visit_auipc_inst(AUIPCInst *inst) {
            usize rd = inst->get_rd();
            if (rd != 0) {
                XLenT imm = inst->get_imm();
                reg.set_x(rd, imm + reg.get_pc());
            }
            reg.inc_pc(LUIInst::INST_WIDTH);

            return NONE;
        }

        RetT visit_jal_inst(JALInst *inst) {
            usize rd = inst->get_rd();
            XLenT imm = inst->get_imm();
#if IALIGN == 32
            if (get_bits<UXLenT, 2, 0>(imm) != 0) return INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION;
#endif
            XLenT pc = reg.get_pc();
            if (rd != 0) reg.set_x(rd, pc + JALInst::INST_WIDTH);
            reg.set_pc(pc + imm);

            return NONE;
        }

        RetT visit_jalr_inst(JALRInst *inst) {
            usize rd = inst->get_rd();
            usize rs1 = inst->get_rs1();
            XLenT imm = inst->get_imm();
            UXLenT target = get_bits<UXLenT, XLEN, 1, 1>(reg.get_x(rs1) + imm);
#if IALIGN == 32
            if (get_bits<UXLenT, 2, 0>(target) != 0) return INSTRUCTION_ADDRESS_MISALIGNED_EXCEPTION;
#endif
            if (rd != 0) reg.set_x(rd, reg.get_pc() + JALRInst::INST_WIDTH);
            reg.set_pc(target);

            return NONE;
        }

        RetT visit_beq_inst(BEQInst *inst) { return operate_branch<typename operators<xlen>::EQ>(inst); }

        RetT visit_bne_inst(BNEInst *inst) { return operate_branch<typename operators<xlen>::NE>(inst); }

        RetT visit_blt_inst(BLTInst *inst) { return operate_branch<typename operators<xlen>::LT>(inst); }

        RetT visit_bge_inst(BGEInst *inst) { return operate_branch<typename operators<xlen>::GE>(inst); }

        RetT visit_bltu_inst(BLTUInst *inst) { return operate_branch<typename operators<xlen>::LTU>(inst); }

        RetT visit_bgeu_inst(BGEUInst *inst) { return operate_branch<typename operators<xlen>::GEU>(inst); }

        RetT visit_lb_inst(LBInst *inst) { return operate_load<i8>(inst); }

        RetT visit_lh_inst(LHInst *inst) { return operate_load<i16>(inst); }

        RetT visit_lw_inst(LWInst *inst) { return operate_load<i32>(inst); }

        RetT visit_lbu_inst(LBUInst *inst) { return operate_load<u8>(inst); }

        RetT visit_lhu_inst(LHUInst *inst) { return operate_load<u16>(inst); }

        RetT visit_sb_inst(SBInst *inst) { return operate_store<u8>(inst); }

        RetT visit_sh_inst(SHInst *inst) { return operate_store<u16>(inst); }

        RetT visit_sw_inst(SWInst *inst) { return operate_store<u32>(inst); }

        RetT visit_addi_inst(ADDIInst *inst) { return operate_immediate<typename operators<xlen>::ADD>(inst); }

        RetT visit_slti_inst(SLTIInst *inst) { return operate_immediate<typename operators<xlen>::SLT>(inst); }

        RetT visit_sltiu_inst(SLTIUInst *inst) { return operate_immediate<typename operators<xlen>::SLTU>(inst); }

        RetT visit_xori_inst(XORIInst *inst) { return operate_immediate<typename operators<xlen>::XOR>(inst); }

        RetT visit_ori_inst(ORIInst *inst) { return operate_immediate<typename operators<xlen>::OR>(inst); }

        RetT visit_andi_inst(ANDIInst *inst) { return operate_immediate<typename operators<xlen>::AND>(inst); }

        RetT visit_slli_inst(SLLIInst<xlen> *inst) {
            return operate_immediate_shift<typename operators<xlen>::SLL>(inst);
        }

        RetT visit_srli_inst(SRLIInst<xlen> *inst) {
            return operate_immediate_shift<typename operators<xlen>::SRL>(inst);
        }

        RetT visit_srai_inst(SRAIInst<xlen> *inst) {
            return operate_immediate_shift<typename operators<xlen>::SRA>(inst);
        }

        RetT visit_add_inst(ADDInst *inst) { return operate_register<typename operators<xlen>::ADD>(inst); }

        RetT visit_sub_inst(SUBInst *inst) { return operate_register<typename operators<xlen>::SUB>(inst); }

        RetT visit_sll_inst(SLLInst *inst) { return operate_register<typename operators<xlen>::SLL>(inst); }

        RetT visit_slt_inst(SLTInst *inst) { return operate_register<typename operators<xlen>::SLT>(inst); }

        RetT visit_sltu_inst(SLTUInst *inst) { return operate_register<typename operators<xlen>::SLTU>(inst); }

        RetT visit_xor_inst(XORInst *inst) { return operate_register<typename operators<xlen>::XOR>(inst); }

        RetT visit_srl_inst(SRLInst *inst) { return operate_register<typename operators<xlen>::SRL>(inst); }

        RetT visit_sra_inst(SRAInst *inst) { return operate_register<typename operators<xlen>::SRA>(inst); }

        RetT visit_or_inst(ORInst *inst) { return operate_register<typename operators<xlen>::OR>(inst); }

        RetT visit_and_inst(ANDInst *inst) { return operate_register<typename operators<xlen>::AND>(inst); }

        RetT visit_fence_inst(riscv_isa_unused FENCEInst *inst) {
            return NONE; // todo
        }

        RetT visit_ecall_inst(riscv_isa_unused ECALLInst *inst) { return ECALL; }

        RetT visit_ebreak_inst(riscv_isa_unused EBREAKInst *inst) { return EBREAK; }

#if defined(__RV_EXTENSION_M__)

        RetT visit_mul_inst(MULInst *inst) { return operate_register<typename operators<xlen>::MUL>(inst); }

        RetT visit_mulh_inst(MULHInst *inst) { return operate_register<typename operators<xlen>::MULH>(inst); }

        RetT visit_mulhsu_inst(MULHSUInst *inst) { return operate_register<typename operators<xlen>::MULHSU>(inst); }

        RetT visit_mulhu_inst(MULHUInst *inst) { return operate_register<typename operators<xlen>::MULHU>(inst); }

        RetT visit_div_inst(DIVInst *inst) { return operate_register<typename operators<xlen>::DIV>(inst); }

        RetT visit_divu_inst(DIVUInst *inst) { return operate_register<typename operators<xlen>::DIVU>(inst); }

        RetT visit_rem_inst(REMInst *inst) { return operate_register<typename operators<xlen>::REM>(inst); }

        RetT visit_remu_inst(REMUInst *inst) { return operate_register<typename operators<xlen>::REMU>(inst); }

#endif // defined(__RV_EXTENSION_M__)
#if __RV_BIT_WIDTH__ == 64

        RetT visit_ld_inst(LDInst *inst) { return operate_load<i64>(inst); }

        RetT visit_lwu_inst(LWUInst *inst) { return operate_load<u32>(inst); }

        RetT visit_sd_inst(SDInst *inst) { return operate_store<u64>(inst); }

        RetT visit_addiw_inst(ADDIWInst *inst) {
            return operate_immediate<typename operators<xlen_32_trait>::ADD>(inst);
        }

        RetT visit_slliw_inst(SLLIWInst<xlen> *inst) {
            return operate_immediate<typename operators<xlen_32_trait>::SLL>(inst);;
        }

        RetT visit_srliw_inst(SRLIWInst<xlen> *inst) {
            return operate_immediate<typename operators<xlen_32_trait>::SRL>(inst);
        }

        RetT visit_sraiw_inst(SRAIWInst<xlen> *inst) {
            return operate_immediate<typename operators<xlen_32_trait>::SRA>(inst);
        }

        RetT visit_addw_inst(ADDWInst *inst) {
            return operate_register<typename operators<xlen_32_trait>::ADD>(inst);
        }

        RetT visit_subw_inst(SUBWInst *inst) {
            return operate_register<typename operators<xlen_32_trait>::SUB>(inst);
        }

        RetT visit_sllw_inst(SLLWInst *inst) {
            return operate_register<typename operators<xlen_32_trait>::SLL>(inst);
        }

        RetT visit_srlw_inst(SRLWInst *inst) {
            return operate_register<typename operators<xlen_32_trait>::SRL>(inst);
        }

        RetT visit_sraw_inst(SRAWInst *inst) {
            return operate_register<typename operators<xlen_32_trait>::SRA>(inst);
        }

#if defined(__RV_EXTENSION_M__)

        RetT visit_mulw_inst(MULWInst *inst) {
            return operate_register<typename operators<xlen_32_trait>::MUL>(inst);
        }

        RetT visit_divw_inst(DIVWInst *inst) {
            return operate_register<typename operators<xlen_32_trait>::DIV>(inst);
        }

        RetT visit_divuw_inst(DIVUWInst *inst) {
            return operate_register<typename operators<xlen_32_trait>::DIVU>(inst);
        }

        RetT visit_remw_inst(REMWInst *inst) {
            return operate_register<typename operators<xlen_32_trait>::REM>(inst);
        }

        RetT visit_remuw_inst(REMUWInst *inst) {
            return operate_register<typename operators<xlen_32_trait>::REMU>(inst);
        }

#endif // defined(__RV_EXTENSION_M__)
#endif // __RV_BIT_WIDTH__ == 64
    };
}


#endif //RISCV_ISA_HART_HPP
