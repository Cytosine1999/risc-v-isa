#include "target/hart.hpp"

using namespace risc_v_isa;


int main() {
    u32 text[] = {
            //    main:
            0x000002B3, //        add t0, x0, x0                0x00
            0x00400313, //        addi t1, x0, 4                0x04
            0x00000397, //        la t2, test_input             0x08
            0x09038393, //                                      0x0c
            //    main_loop:
            0x06628263, //        beq t0, t1, main_exit         0x10
            0x00229E13, //        slli t3, t0, 2                0x14
            0x01C38EB3, //        add t4, t2, t3                0x18
            0x000EA503, //        lw a0, 0(t4)                  0x1c
            0xFEC10113, //        addi sp, sp, -20              0x20
            0x00512023, //        sw t0, 0(sp)                  0x24
            0x00612223, //        sw t1, 4(sp)                  0x28
            0x00712423, //        sw t2, 8(sp)                  0x2c
            0x01C12623, //        sw t3, 12(sp)                 0x30
            0x01D12823, //        sw t4, 16(sp)                 0x34
            0x044000EF, //        jal ra, factorial             0x38
            0x00012283, //        lw t0, 0(sp)                  0x3c
            0x00412303, //        lw t1, 4(sp)                  0x40
            0x00812383, //        lw t2, 8(sp)                  0x44
            0x00C12E03, //        lw t3, 12(sp)                 0x48
            0x01012E83, //        lw t4, 16(sp)                 0x4c
            0x01410113, //        addi sp, sp, 20               0x50
            0x00050593, //        addi a1, a0, 0                0x54
            0x00100513, //        addi a0, x0, 1                0x58
            0x00000073, //        ecall # Print Result          0x5c
            0x02000593, //        addi a1, x0, ' '              0x60
            0x00B00513, //        addi a0, x0, 11               0x64
            0x00000073, //        ecall                         0x68
            0x00128293, //        addi t0, t0, 1                0x6c
            0xFA1FF06F, //        jal x0, main_loop             0x70
            //    main_exit:
            0x00A00513, //        addi a0, x0, 10               0x74
            0x00000073, //        ecall # Exit                  0x78
            //    factorial:
            0x00050293, //        addi t0, a0, 0                0x7c
            0x00100513, //        addi a0, x0, 1                0x80
            //    factorial_loop:
            0x00028863, //        beq t0, x0, factorial_ret     0x84
            0x02550533, //        mul a0, a0, t0                0x88
            0xFFF28293, //        addi t0 t0 -1                 0x8c
            0xFF5FF06F, //        j factorial_loop              0x90
            //    factorial_ret:
            0x00008067, //        jr, ra                        0x94
    };

    u32 data[] = {
            3, 6, 7, 8
    };

    BEQInst inst{0, 0, 0};

    std::cout << (dyn_cast<Instruction32>(static_cast<Instruction *>(&inst)) != nullptr) << std::endl;

    RegisterFile reg{};
    reg.set_pc(0);
    reg.set_x(RegisterFile::SP, 4092);

    Memory mem{4096};
    mem.memory_copy(0, text, sizeof(text));
    mem.memory_copy(sizeof(text), data, sizeof(data));

    Hart core{reg, mem};

    core.start();
}
