#ifndef RISCV_ISA_INSTRUCTION_TEST_HPP
#define RISCV_ISA_INSTRUCTION_TEST_HPP


#include "utility.hpp"
#include "instruction/instruction.hpp"
#include "instruction/rv32i.hpp"
#include "instruction/instruction_visitor.hpp"


namespace riscv_isa {
    template<typename InputT, typename xlen>
    class CheckVisitor : public InstructionVisitor<CheckVisitor<InputT, xlen>, xlen> {
    public:
        using RetT = void;

        RetT illegal_instruction(riscv_isa_unused Instruction *inst) {
            ASSERT((std::is_same<void, InputT>::value));
        }

        RetT visit_lui_inst(riscv_isa_unused LUIInst *inst) {
            ASSERT((std::is_same<LUIInst, InputT>::value));
        }

        RetT visit_auipc_inst(riscv_isa_unused AUIPCInst *inst) {
            ASSERT((std::is_same<AUIPCInst, InputT>::value));
        }

        RetT visit_jal_inst(riscv_isa_unused JALInst *inst) {
            ASSERT((std::is_same<JALInst, InputT>::value));
        }

        RetT visit_jalr_inst(riscv_isa_unused JALRInst *inst) {
            ASSERT((std::is_same<JALRInst, InputT>::value));
        }

        RetT visit_beq_inst(riscv_isa_unused BEQInst *inst) {
            ASSERT((std::is_same<BEQInst, InputT>::value));
        }

        RetT visit_bne_inst(riscv_isa_unused BNEInst *inst) {
            ASSERT((std::is_same<BNEInst, InputT>::value));
        }

        RetT visit_blt_inst(riscv_isa_unused BLTInst *inst) {
            ASSERT((std::is_same<BLTInst, InputT>::value));
        }

        RetT visit_bge_inst(riscv_isa_unused BGEInst *inst) {
            ASSERT((std::is_same<BGEInst, InputT>::value));
        }

        RetT visit_bltu_inst(riscv_isa_unused BLTUInst *inst) {
            ASSERT((std::is_same<BLTUInst, InputT>::value));
        }

        RetT visit_bgeu_inst(riscv_isa_unused BGEUInst *inst) {
            ASSERT((std::is_same<BGEUInst, InputT>::value));
        }

        RetT visit_lb_inst(riscv_isa_unused LBInst *inst) {
            ASSERT((std::is_same<LBInst, InputT>::value));
        }

        RetT visit_lh_inst(riscv_isa_unused LHInst *inst) {
            ASSERT((std::is_same<LHInst, InputT>::value));
        }

        RetT visit_lw_inst(riscv_isa_unused LWInst *inst) {
            ASSERT((std::is_same<LWInst, InputT>::value));
        }

        RetT visit_lbu_inst(riscv_isa_unused LBUInst *inst) {
            ASSERT((std::is_same<LBUInst, InputT>::value));
        }

        RetT visit_lhu_inst(riscv_isa_unused LHUInst *inst) {
            ASSERT((std::is_same<LHUInst, InputT>::value));
        }

        RetT visit_sb_inst(riscv_isa_unused SBInst *inst) {
            ASSERT((std::is_same<SBInst, InputT>::value));
        }

        RetT visit_sh_inst(riscv_isa_unused SHInst *inst) {
            ASSERT((std::is_same<SHInst, InputT>::value));
        }

        RetT visit_sw_inst(riscv_isa_unused SWInst *inst) {
            ASSERT((std::is_same<SWInst, InputT>::value));
        }

        RetT visit_addi_inst(riscv_isa_unused ADDIInst *inst) {
            ASSERT((std::is_same<ADDIInst, InputT>::value));
        }

        RetT visit_slti_inst(riscv_isa_unused SLTIInst *inst) {
            ASSERT((std::is_same<SLTIInst, InputT>::value));
        }

        RetT visit_sltiu_inst(riscv_isa_unused SLTIUInst *inst) {
            ASSERT((std::is_same<SLTIUInst, InputT>::value));
        }

        RetT visit_xori_inst(riscv_isa_unused XORIInst *inst) {
            ASSERT((std::is_same<XORIInst, InputT>::value));
        }

        RetT visit_ori_inst(riscv_isa_unused ORIInst *inst) {
            ASSERT((std::is_same<ORIInst, InputT>::value));
        }

        RetT visit_andi_inst(riscv_isa_unused ANDIInst *inst) {
            ASSERT((std::is_same<ANDIInst, InputT>::value));
        }

        RetT visit_slli_inst(riscv_isa_unused SLLIInst<xlen> *inst) {
            ASSERT((std::is_same<SLLIInst<xlen>, InputT>::value));
        }

        RetT visit_srli_inst(riscv_isa_unused SRLIInst<xlen> *inst) {
            ASSERT((std::is_same<SRLIInst<xlen>, InputT>::value));
        }

        RetT visit_srai_inst(riscv_isa_unused SRAIInst<xlen> *inst) {
            ASSERT((std::is_same<SRAIInst<xlen>, InputT>::value));
        }

        RetT visit_add_inst(riscv_isa_unused ADDInst *inst) {
            ASSERT((std::is_same<ADDInst, InputT>::value));
        }

        RetT visit_sub_inst(riscv_isa_unused SUBInst *inst) {
            ASSERT((std::is_same<SUBInst, InputT>::value));
        }

        RetT visit_sll_inst(riscv_isa_unused SLLInst *inst) {
            ASSERT((std::is_same<SLLInst, InputT>::value));
        }

        RetT visit_slt_inst(riscv_isa_unused SLTInst *inst) {
            ASSERT((std::is_same<SLTInst, InputT>::value));
        }

        RetT visit_sltu_inst(riscv_isa_unused SLTUInst *inst) {
            ASSERT((std::is_same<SLTUInst, InputT>::value));
        }

        RetT visit_xor_inst(riscv_isa_unused XORInst *inst) {
            ASSERT((std::is_same<XORInst, InputT>::value));
        }

        RetT visit_srl_inst(riscv_isa_unused SRLInst *inst) {
            ASSERT((std::is_same<SRLInst, InputT>::value));
        }

        RetT visit_sra_inst(riscv_isa_unused SRAInst *inst) {
            ASSERT((std::is_same<SRAInst, InputT>::value));
        }

        RetT visit_or_inst(riscv_isa_unused ORInst *inst) {
            ASSERT((std::is_same<ORInst, InputT>::value));
        }

        RetT visit_and_inst(riscv_isa_unused ANDInst *inst) {
            ASSERT((std::is_same<ANDInst, InputT>::value));
        }

        RetT visit_fence_inst(riscv_isa_unused FENCEInst *inst) {
            ASSERT((std::is_same<FENCEInst, InputT>::value));
        }

        RetT visit_ecall_inst(riscv_isa_unused ECALLInst *inst) {
            ASSERT((std::is_same<ECALLInst, InputT>::value));
        }

        RetT visit_ebreak_inst(riscv_isa_unused EBREAKInst *inst) {
            ASSERT((std::is_same<EBREAKInst, InputT>::value));
        }
    };

    template<typename T, typename U, bool flag = std::is_same<T, U>::value>
    struct _check_dyn_cast;

    template<typename T, typename U>
    struct _check_dyn_cast<T, U, true> {
    public:
        static void inner(Instruction *inst) { ASSERT_NE(dyn_cast<U>(inst), static_cast<U *>(nullptr)); }
    };

    template<typename T, typename U>
    struct _check_dyn_cast<T, U, false> {
    public:
        static void inner(Instruction *inst) { ASSERT_EQ(dyn_cast<U>(inst), static_cast<U *>(nullptr)); }
    };

    template<typename T, typename U>
    void check_dyn_cast(Instruction *inst) { _check_dyn_cast<T, U>::inner(inst); }

    template<typename T>
    T *_dyn_cast_with_void(Instruction *inst);

    template<>
    void *_dyn_cast_with_void<void>(riscv_isa_unused Instruction *inst) { return nullptr; }

    template<typename T>
    T *_dyn_cast_with_void(Instruction *inst) { return dyn_cast<T>(inst); }

    template<typename T, typename xlen>
    T *check_all_dyn_cast(Instruction *inst) {
        check_dyn_cast<T, LUIInst>(inst);
        check_dyn_cast<T, AUIPCInst>(inst);
        check_dyn_cast<T, JALInst>(inst);
        check_dyn_cast<T, JALRInst>(inst);
        check_dyn_cast<T, BEQInst>(inst);
        check_dyn_cast<T, BNEInst>(inst);
        check_dyn_cast<T, BLTInst>(inst);
        check_dyn_cast<T, BGEInst>(inst);
        check_dyn_cast<T, BLTUInst>(inst);
        check_dyn_cast<T, BGEUInst>(inst);
        check_dyn_cast<T, LBInst>(inst);
        check_dyn_cast<T, LHInst>(inst);
        check_dyn_cast<T, LWInst>(inst);
        check_dyn_cast<T, LBUInst>(inst);
        check_dyn_cast<T, LHUInst>(inst);
        check_dyn_cast<T, SBInst>(inst);
        check_dyn_cast<T, SHInst>(inst);
        check_dyn_cast<T, SWInst>(inst);
        check_dyn_cast<T, ADDIInst>(inst);
        check_dyn_cast<T, SLTIInst>(inst);
        check_dyn_cast<T, SLTIUInst>(inst);
        check_dyn_cast<T, XORIInst>(inst);
        check_dyn_cast<T, ORIInst>(inst);
        check_dyn_cast<T, ANDIInst>(inst);
        check_dyn_cast<T, SLLIInst<xlen>>(inst);
        check_dyn_cast<T, SRLIInst<xlen>>(inst);
        check_dyn_cast<T, SRAIInst<xlen>>(inst);
        check_dyn_cast<T, ADDInst>(inst);
        check_dyn_cast<T, SUBInst>(inst);
        check_dyn_cast<T, SLLInst>(inst);
        check_dyn_cast<T, SLTInst>(inst);
        check_dyn_cast<T, SLTUInst>(inst);
        check_dyn_cast<T, XORInst>(inst);
        check_dyn_cast<T, SRLInst>(inst);
        check_dyn_cast<T, SRAInst>(inst);
        check_dyn_cast<T, ORInst>(inst);
        check_dyn_cast<T, ANDInst>(inst);
        check_dyn_cast<T, FENCEInst>(inst);
        check_dyn_cast<T, ECALLInst>(inst);
        check_dyn_cast<T, EBREAKInst>(inst);

        return _dyn_cast_with_void<T>(inst);
    }

}


#endif //RISCV_ISA_INSTRUCTION_TEST_HPP
