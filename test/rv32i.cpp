#include "utility.hpp"
#include "target/hart.hpp"

using namespace risc_v_isa;


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
    reg.set_pc(0);
    reg.set_x(RegisterFile::SP, 4092);

    Memory mem{1};
    mem.memory_copy(0, text, sizeof(text));
    mem.memory_copy(sizeof(text), data, sizeof(data));

    Hart core{reg, mem};

    core.start();
}
