#include <sys/mman.h>

#include "target/hart.hpp"
#include "target/dump.hpp"

using namespace riscv_isa;


template<typename xlen=xlen_trait>
class Memory {
private:
    using XLenT = typename xlen::UXLenT;

    u8 *memory_offset;
    usize memory_size;

public:

    Memory(usize _memory_size) : memory_size{_memory_size} {
        memory_offset = static_cast<u8 *>(mmap(nullptr, memory_size, PROT_READ | PROT_WRITE,
                                               MAP_ANONYMOUS | MAP_SHARED, -1, 0));
        if (memory_offset == MAP_FAILED) {
            memory_offset = nullptr;
            memory_size = 0;
        }
    }

    Memory(const Memory &other) = delete;

    Memory &operator=(const Memory &other) = delete;

    template<typename T>
    T *address(XLenT addr) {
        return addr < memory_size ? reinterpret_cast<T *>(memory_offset + addr) : nullptr;
    }

    bool memory_copy(XLenT offset, const void *src, usize length) {
        if (offset <= memory_size - length) {
            memcpy(memory_offset + offset, src, length);
            return true;
        } else {
            return false;
        }
    }

    ~Memory() { if (memory_offset != nullptr) munmap(memory_offset, memory_size); }
};


class NoneHart : public Hart<NoneHart> {
public:
    using MemT = Memory<>;

protected:
    MemT &mem;

public:
    NoneHart(UXLenT hart_id, XLenT pc, IntRegT &reg, MemT &mem) : Hart{hart_id, pc, reg}, mem{mem} {
        cur_level = USER_MODE;
    }

    void internal_interrupt_action(UXLenT interrupt, riscv_isa_unused UXLenT trap_value) {
        csr_reg[CSRRegT::SCAUSE] = interrupt;
    }

    template<typename ValT>
    RetT mmu_load_int_reg(usize dest, XLenT addr) {
        ValT *ptr = mem.template address<ValT>(addr);
        if (ptr == nullptr) {
            return internal_interrupt(trap::LOAD_PAGE_FAULT, addr);
        } else {
            if (dest != 0) set_x(dest, *ptr);
            return true;
        }
    }

    template<typename ValT>
    RetT mmu_store_int_reg(usize src, XLenT addr) {
        ValT *ptr = mem.template address<ValT>(addr);
        if (ptr == nullptr) {
            return internal_interrupt(trap::STORE_AMO_PAGE_FAULT, addr);
        } else {
            *ptr = static_cast<ValT>(get_x(src));
            return true;
        }
    }

    template<usize offset>
    RetT mmu_load_inst_half(XLenT addr) {
        u16 *ptr = mem.template address<u16>(addr + offset * sizeof(u16));
        if (ptr == nullptr) {
            return internal_interrupt(trap::INSTRUCTION_PAGE_FAULT, addr);
        } else {
            *(reinterpret_cast<u16 *>(&this->inst_buffer) + offset) = *ptr;
            return true;
        }
    }

#if defined(__RV_EXTENSION_ZICSR__)

    UXLenT get_csr_reg(riscv_isa_unused UXLenT index) { return csr_reg[index]; }

    RetT set_csr_reg(riscv_isa_unused UXLenT index, riscv_isa_unused UXLenT val) { return true; }

#endif // defined(__RV_EXTENSION_ZICSR__)

    RetT visit_fence_inst(riscv_isa_unused FENCEInst *inst) {
        inc_pc(FENCEInst::INST_WIDTH);
        return true;
    }

#if defined(__RV_SUPERVISOR_MODE__)

    RetT visit_sret_inst(riscv_isa_unused SRETInst *inst) { return illegal_instruction(inst); }

#endif // defined(__RV_SUPERVISOR_MODE__)

    RetT visit_mret_inst(riscv_isa_unused MRETInst *inst) { return illegal_instruction(inst); }

    RetT visit_wfi_inst(riscv_isa_unused WFIInst *inst) { return illegal_instruction(inst); }

#if defined(__RV_SUPERVISOR_MODE__)

    RetT visit_sfencevma_inst(riscv_isa_unused SFENCEVAMInst *inst) { return illegal_instruction(inst); }

#endif // defined(__RV_SUPERVISOR_MODE__)

    bool system_call() {
        switch (get_x(IntRegT::A0)) {
            case 1:
                std::cout << std::dec << get_x(IntRegT::A1);

                return true;
            case 11:
                std::cout << static_cast<char>(get_x(IntRegT::A1));

                return true;
            case 10:
                std::cout << std::endl << "[exit]" << std::endl;

                return false;
            default:
                std::cerr << "Invalid enviroment call number at " << std::hex << get_pc()
                          << ", call number " << std::dec << get_x(IntRegT::A7)
                          << std::endl;

                return false;
        }
    }

    void start() {
        while (true) {
            if (visit()) continue;

            switch (csr_reg[CSRRegT::SCAUSE]) {
                case trap::INSTRUCTION_ADDRESS_MISALIGNED:
                case trap::INSTRUCTION_ACCESS_FAULT:
                    std::cerr << "Instruction address misaligned at "
                              << std::hex << get_pc() << std::endl;

                    return;
                case trap::ILLEGAL_INSTRUCTION:
                    std::cerr << "Illegal instruction at "
                              << std::hex << get_pc() << ": " << std::dec
                              << *reinterpret_cast<Instruction *>(&inst_buffer) << std::endl;

                    return;
                case trap::BREAKPOINT:
                    std::cerr << "Break point at " << std::hex << get_pc() << std::endl;
                    inc_pc(ECALLInst::INST_WIDTH);

                    break;
                case trap::LOAD_ADDRESS_MISALIGNED:
                case trap::LOAD_ACCESS_FAULT:
                    std::cerr << "Load address misaligned at "
                              << std::hex << get_pc() << ": " << std::dec
                              << *reinterpret_cast<Instruction *>(&inst_buffer) << std::endl;

                    return;
                case trap::STORE_AMO_ADDRESS_MISALIGNED:
                case trap::STORE_AMO_ACCESS_FAULT:
                    std::cerr << "Store or AMO address misaligned at "
                              << std::hex << get_pc() << ": " << std::dec
                              << *reinterpret_cast<Instruction *>(&inst_buffer) << std::endl;

                    return;
                case trap::U_MODE_ENVIRONMENT_CALL:
                    if (!system_call()) return;
                    inc_pc(ECALLInst::INST_WIDTH);

                    break;
                case trap::S_MODE_ENVIRONMENT_CALL:
                    riscv_isa_unreachable("no system mode interrupt!");
                case trap::M_MODE_ENVIRONMENT_CALL:
                    riscv_isa_unreachable("no machine mode interrupt!");
                case trap::INSTRUCTION_PAGE_FAULT:
                    std::cerr << "Instruction page fault at " << std::hex << get_pc() << std::endl;

                    return;
                case trap::LOAD_PAGE_FAULT:
                    std::cerr << "Load page fault at " << std::hex << get_pc() << std::endl;

                    return;
                case trap::STORE_AMO_PAGE_FAULT:
                    std::cerr << "Store or AMO page fault at " << std::hex << get_pc() << std::endl;

                    return;
                default:
                    riscv_isa_unreachable("unknown internal interrupt!");
            }
        }
    }
};

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
            7, 8, 9, 10
    };

    NoneHart::IntRegT reg{};
    reg.set_x(NoneHart::IntRegT::SP, 4092);

    NoneHart::MemT mem{4096};
    if (mem.address<void>(0) == nullptr) riscv_isa_abort("memory allocate failed");

    mem.memory_copy(0, text, sizeof(text));
    mem.memory_copy(sizeof(text), data, sizeof(data));

    NoneHart core{0, 0, reg, mem};

    core.start();
}
