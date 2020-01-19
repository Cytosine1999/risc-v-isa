#include <iostream>

#include "utility.hpp"
#include "instruction/instruction.hpp"
#include "instruction/rv32i.hpp"
#include "instruction/instruction_visitor.hpp"
#include "register/register_file.hpp"

using namespace risc_v_isa;


enum InternalInterrupt {
    NONE,
    ILLEGAL_INSTRUCTION,
    MEMORY_ERROR,
    ECALL,
    EBREAK,
};

class RV32Hart : public InstructionVisitor<RV32Hart, InternalInterrupt> {
private:
    RegisterFile &reg;
    Memory &mem;

public:
    RV32Hart(RegisterFile &reg, Memory &mem) : reg{reg}, mem{mem} {}

    void start() {
        while (true) {
            Instruction *inst = mem.address<Instruction>(reg.get_pc());

            switch (inst == nullptr ? MEMORY_ERROR : visit(inst)) {
                case ILLEGAL_INSTRUCTION:
                    std::cerr << "Illegal instruction at " << std::hex << reg.get_pc() << ' '
                              << *reinterpret_cast<u32 *>(inst) << std::endl;

                    return;
                case MEMORY_ERROR:
                    std::cerr << "Memory error at " << std::hex << reg.get_pc() << std::endl;

                    return;
                case ECALL:

                    break;
                case EBREAK:

                    break;
                default:;
            }
        }
    }

    RetT illegal_instruction(Instruction *inst) {
        return ILLEGAL_INSTRUCTION;
    }

    RetT visit_inst(Instruction *inst) {
        std::cerr << "Visitor of none void return not fully defined!" << std::endl;

        abort();
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
        (*inst)(reg);
        return NONE;
    }

    RetT visit_jalr_inst(JALRInst *inst) {
        (*inst)(reg);
        return NONE;
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

    RetT visit_slli_inst(SLLIWInst *inst) {
        (*inst)(reg);
        return NONE;
    }

    RetT visit_srli_inst(SRLIWInst *inst) {
        (*inst)(reg);
        return NONE;
    }

    RetT visit_srai_inst(SRAIWInst *inst) {
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

    RetT visit_sll_inst(SLLWInst *inst) {
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

    RetT visit_srl_inst(SRLWInst *inst) {
        (*inst)(reg);
        return NONE;
    }

    RetT visit_sra_inst(SRAWInst *inst) {
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
        (*inst)(reg);
        return NONE;
    }

    RetT visit_ecall_inst(ECALLInst *inst) {
        return ECALL;
    }

    RetT visit_ebreak_inst(EBREAKInst *inst) {
        return EBREAK;
    }

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

    std::cout << BITS_MASK<u32, 7, 0> << std::endl;

    RegisterFile reg{};
    Memory mem{1};

    RV32Hart core{reg, mem};

    mem.memory_copy(0, text, sizeof(text));
    mem.memory_copy(sizeof(text), data, sizeof(data));

    reg.set_pc(0);
    reg.set_x(RegisterFile::SP, 4092);

    core.start();
}
