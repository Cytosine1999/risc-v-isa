#ifndef RISCV_ISA_TRAP_HPP
#define RISCV_ISA_TRAP_HPP


namespace riscv_isa {
    namespace trap {
        enum ExceptionCode {
            INSTRUCTION_ADDRESS_MISALIGNED = 0,
            INSTRUCTION_ACCESS_FAULT = 1,
            ILLEGAL_INSTRUCTION = 2,
            BREAKPOINT = 3,
            LOAD_ADDRESS_MISALIGNED = 4,
            LOAD_ACCESS_FAULT = 5,
            STORE_AMO_ADDRESS_MISALIGNED = 6,
            STORE_AMO_ACCESS_FAULT = 7,
            U_MODE_ENVIRONMENT_CALL = 8,
            S_MODE_ENVIRONMENT_CALL = 9,
            INSTRUCTION_PAGE_FAULT = 12,
            LOAD_PAGE_FAULT = 13,
            STORE_AMO_PAGE_FAULT = 15,
        };

        enum InterruptCode {
            SUPERVISOR_SOFTWARE_INTERRUPT = 1,
            SUPERVISOR_TIMER_INTERRUPT = 5,
            SUPERVISOR_EXTERNAL_INTERRUPT = 9,
        };
    }
}


#endif //RISCV_ISA_TRAP_HPP
