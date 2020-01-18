#include <iostream>

#include "utility.hpp"
#include "instruction/instruction.hpp"
#include "instruction/rv32i.hpp"
#include "instruction/instruction_visitor.hpp"
#include "register/register_file.hpp"

using namespace risc_v_isa;


enum InternalInterrupt {

};

class RV32Hart : public InstructionVisitor<RV32Hart, bool> {
private:
    RegisterFile &reg;
    Memory &mem;

public:
    RV32Hart(RegisterFile &reg, Memory &mem) : reg{reg}, mem{mem} {}

    void start() {
//        while(true) {
//            Instruction *inst = mem.address<Instruction>(reg.get_pc());
//
//            if (!visit(inst))
//                break;
//        }
    }

//    RetT visit_lui_inst(LUIInst *self) { return static_cast<T *>(this)->visit_32_inst(self); }
//
//    RetT visit_auipc_inst(AUIPCInst *self) { return static_cast<T *>(this)->visit_32_inst(self); }
//
//    RetT visit_jal_inst(JALInst *self) { return static_cast<T *>(this)->visit_32_inst(self); }
//
//    RetT visit_jalr_inst(JALRInst *self) { return static_cast<T *>(this)->visit_32_inst(self); }
//
//    RetT visit_beq_inst(BEQInst *self) { return static_cast<T *>(this)->visit_branch_set_inst(self); }
//
//    RetT visit_bne_inst(BNEInst *self) { return static_cast<T *>(this)->visit_branch_set_inst(self); }
//
//    RetT visit_blt_inst(BLTInst *self) { return static_cast<T *>(this)->visit_branch_set_inst(self); }
//
//    RetT visit_bge_inst(BGEInst *self) { return static_cast<T *>(this)->visit_branch_set_inst(self); }
//
//    RetT visit_bltu_inst(BLTUInst *self) { return static_cast<T *>(this)->visit_branch_set_inst(self); }
//
//    RetT visit_bgeu_inst(BGEUInst *self) { return static_cast<T *>(this)->visit_branch_set_inst(self); }
//
//    RetT visit_lb_inst(LBInst *self) { return static_cast<T *>(this)->visit_load_set_inst(self); }
//
//    RetT visit_lh_inst(LHInst *self) { return static_cast<T *>(this)->visit_load_set_inst(self); }
//
//    RetT visit_lw_inst(LWInst *self) { return static_cast<T *>(this)->visit_load_set_inst(self); }
//
//    RetT visit_lbu_inst(LBUInst *self) { return static_cast<T *>(this)->visit_load_set_inst(self); }
//
//    RetT visit_lhu_inst(LHUInst *self) { return static_cast<T *>(this)->visit_load_set_inst(self); }
//
//    RetT visit_sb_inst(SBInst *self) { return static_cast<T *>(this)->visit_store_set_inst(self); }
//
//    RetT visit_sh_inst(SHInst *self) { return static_cast<T *>(this)->visit_store_set_inst(self); }
//
//    RetT visit_sw_inst(SWInst *self) { return static_cast<T *>(this)->visit_store_set_inst(self); }
//
//    RetT visit_addi_inst(ADDIInst *self) { return static_cast<T *>(this)->visit_arith_imm_set_inst(self); }
//
//    RetT visit_slti_inst(SLTIInst *self) { return static_cast<T *>(this)->visit_arith_imm_set_inst(self); }
//
//    RetT visit_sltiu_inst(SLTIUInst *self) { return static_cast<T *>(this)->visit_arith_imm_set_inst(self); }
//
//    RetT visit_xori_inst(XORIInst *self) { return static_cast<T *>(this)->visit_arith_imm_set_inst(self); }
//
//    RetT visit_ori_inst(ORIInst *self) { return static_cast<T *>(this)->visit_arith_imm_set_inst(self); }
//
//    RetT visit_andi_inst(ANDIInst *self) { return static_cast<T *>(this)->visit_arith_imm_set_inst(self); }
//
//    RetT visit_slli_inst(SLLIWInst *self) { return static_cast<T *>(this)->visit_arith_imm_set_inst(self); }
//
//    RetT visit_srli_inst(SRLIWInst *self) { return static_cast<T *>(this)->visit_srli_srai_inst(self); }
//
//    RetT visit_srai_inst(SRAIWInst *self) { return static_cast<T *>(this)->visit_srli_srai_inst(self); }
//
//    RetT visit_add_inst(ADDInst *self) { return static_cast<T *>(this)->visit_arith_reg_set_inst(self); }
//
//    RetT visit_sub_inst(SUBInst *self) { return static_cast<T *>(this)->visit_arith_reg_set_inst(self); }
//
//    RetT visit_sll_inst(SLLWInst *self) { return static_cast<T *>(this)->visit_arith_reg_set_inst(self); }
//
//    RetT visit_slt_inst(SLTInst *self) { return static_cast<T *>(this)->visit_arith_reg_set_inst(self); }
//
//    RetT visit_sltu_inst(SLTUInst *self) { return static_cast<T *>(this)->visit_arith_reg_set_inst(self); }
//
//    RetT visit_xor_inst(XORInst *self) { return static_cast<T *>(this)->visit_arith_reg_set_inst(self); }
//
//    RetT visit_srl_inst(SRLWInst *self) { return static_cast<T *>(this)->visit_srl_sra_inst(self); }
//
//    RetT visit_sra_inst(SRAWInst *self) { return static_cast<T *>(this)->visit_srl_sra_inst(self); }
//
//    RetT visit_or_inst(ORInst *self) { return static_cast<T *>(this)->visit_arith_reg_set_inst(self); }
//
//    RetT visit_and_inst(ANDInst *self) { return static_cast<T *>(this)->visit_arith_reg_set_inst(self); }
//
//    RetT visit_fence_inst(FENCEInst *self) { return static_cast<T *>(this)->visit_fence_set_inst(self); }
//
//    RetT visit_fencei_inst(FENCEIInst *self) { return static_cast<T *>(this)->visit_fence_set_inst(self); }
//
//    RetT visit_ecall_inst(ECALLInst *self) { return static_cast<T *>(this)->visit_environment_set_inst(self); }
//
//    RetT visit_ebreak_inst(EBREAKInst *self) { return static_cast<T *>(this)->visit_environment_set_inst(self); }
//
//    RetT visit_csrrw_inst(CSRRWInst *self) { return static_cast<T *>(this)->visit_system_set_inst(self); }
//
//    RetT visit_csrrs_inst(CSRRSInst *self) { return static_cast<T *>(this)->visit_system_set_inst(self); }
//
//    RetT visit_csrrc_inst(CSRRCInst *self) { return static_cast<T *>(this)->visit_system_set_inst(self); }
//
//    RetT visit_csrrwi_inst(CSRRWIInst *self) { return static_cast<T *>(this)->visit_system_set_inst(self); }
//
//    RetT visit_csrrsi_inst(CSRRSIInst *self) { return static_cast<T *>(this)->visit_system_set_inst(self); }
//
//    RetT visit_csrrci_inst(CSRRCIInst *self) { return static_cast<T *>(this)->visit_system_set_inst(self); }

};

int main() {
    u32 text[] = {
            0x000002B3, 0x00400313, 0x00000397, 0x09038393,
            0x06628263, 0x00229E13, 0x01C38EB3, 0x000EA503,
            0xFEC10113, 0x00512023, 0x00612223, 0x00712423,
            0x01C12623, 0x01D12823, 0x044000EF, 0x00012283,
            0x00412303, 0x00812383, 0x00C12E03, 0x01012E83,
            0x01410113, 0x00050593, 0x00100513, 0x00000073,
            0x02000593, 0x00B00513, 0x00000073, 0x00128293,
            0xFA1FF06F, 0x00A00513, 0x00000073, 0x00050293,
            0x00100513, 0x00028863, 0x02550533, 0xFFF28293,
            0xFF5FF06F, 0x00008067
    };

    u32 data[] = {
            3, 6, 7, 8
    };

    RegisterFile reg{};
    Memory mem{1};

    RV32Hart core{reg, mem};

    mem.memory_copy(0, text, sizeof(text));
    mem.memory_copy(sizeof(text), data, sizeof(data));

    reg.set_pc(0);
    reg.set_x(RegisterFile::SP, 4092);



    std::cout << page_size() << std::endl;
}
