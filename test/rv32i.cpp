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
        return ILLEGAL_INSTRUCTION;
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
        std::cout << *inst << std::endl;
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
        return NONE; // todo
    }

    RetT visit_ecall_inst(ECALLInst *inst) {
        return ECALL;
    }

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

};

int main() {
    u32 text[] = {
            //    main:
            0x000002B3, //        add t0, x0, x0
            0x00400313, //        addi t1, x0, 4
            0x00000397, //        la t2, test_input
            0x09038393,
            //    main_loop:
            0x06628263, //        beq t0, t1, main_exit
            0x00229E13, //        slli t3, t0, 2
            0x01C38EB3, //        add t4, t2, t3
            0x000EA503, //        lw a0, 0(t4)
            0xFEC10113, //        addi sp, sp, -20
            0x00512023, //        sw t0, 0(sp)
            0x00612223, //        sw t1, 4(sp)
            0x00712423, //        sw t2, 8(sp)
            0x01C12623, //        sw t3, 12(sp)
            0x01D12823, //        sw t4, 16(sp)
            0x044000EF, //        jal ra, factorial
            0x00012283, //        lw t0, 0(sp)
            0x00412303, //        lw t1, 4(sp)
            0x00812383, //        lw t2, 8(sp)
            0x00C12E03, //        lw t3, 12(sp)
            0x01012E83, //        lw t4, 16(sp)
            0x01410113, //        addi sp, sp, 20
            0x00050593, //        addi a1, a0, 0
            0x00100513, //        addi a0, x0, 1
            0x00000073, //        ecall # Print Result
            0x02000593, //        addi a1, x0, ' '
            0x00B00513, //        addi a0, x0, 11
            0x00000073, //        ecall
            0x00128293, //        addi t0, t0, 1
            0xFA1FF06F, //        jal x0, main_loop
            //    main_exit:
            0x00A00513, //        addi a0, x0, 10
            0x00000073, //        ecall # Exit
            //    factorial:
            0x00050293, //        addi t0, a0, 0
            0x00100513, //        addi a0, x0, 1
            //    factorial_loop:
            0x00028863, //        beq t0, x0, factorial_ret
            0x02550533, //        mul a0, a0, t0
            0xFFF28293, //        addi t0 t0 -1
            0xFF5FF06F, //        j factorial_loop
            //    factorial_ret:
            0x00008067, //        jr, ra
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

    core.start();
}
