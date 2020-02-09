#ifndef RISCV_ISA_CSR_REGISTER_HPP
#define RISCV_ISA_CSR_REGISTER_HPP


#include "utility.hpp"


#include "range.def"

#define _riscv_isa_declare_csr(NAME, name, index) \
    static constexpr usize NAME = index; \
    typename xlen_trait::XLenT name

#define _riscv_isa_csr_reg(NAME, name, base, n) \
    static constexpr usize NAME##n = base + n; \
    typename xlen_trait::XLenT name##n

#define _riscv_isa_csr_reg_range(begin, end, NAME, name, base) \
    _riscv_isa_range(begin, end, _riscv_isa_csr_reg, NAME, name, base)

#define _riscv_isa_csr_reg_pos(NAME, name, POS, pos, base, n) \
    static constexpr usize NAME##n##POS = base + n; \
    typename xlen_trait::XLenT name##n##pos

#define _riscv_isa_csr_reg_pos_range(begin, end, NAME, name, POS, pos, base) \
    _riscv_isa_range(begin, end, _riscv_isa_csr_reg_pos, NAME, name, POS, pos, base)

//get_bits<UXLenT, 2, 0, XLEN - 2>(XLEN_INDEX - 4)
//#ifdef __RV_EXTENSION_A__
//| bit_mask<UXLenT, 0>::val
//#endif
//#ifdef __RV_EXTENSION_B__
//| bit_mask<UXLenT, 1>::val
//#endif
//#ifdef __RV_EXTENSION_C__
//| bit_mask<UXLenT, 2>::val
//#endif
//#ifdef __RV_EXTENSION_D__
//| bit_mask<UXLenT, 3>::val
//#endif
//#ifdef __RV_BASE_E__
//| bit_mask<UXLenT, 4>::val
//#endif
//#ifdef __RV_EXTENSION_F__
//| bit_mask<UXLenT, 5>::val
//#endif
//#ifdef __RV_PRIVILEGE_H__
//| bit_mask<UXLenT, 7>::val
//#endif
//#ifdef __RV_BASE_I__
//| bit_mask<UXLenT, 8>::val
//#endif
//#ifdef __RV_EXTENSION_J__
//| bit_mask<UXLenT, 9>::val
//#endif
//#ifdef __RV_EXTENSION_L__
//| bit_mask<UXLenT, 11>::val
//#endif
//#ifdef __RV_EXTENSION_M__
//| bit_mask<UXLenT, 12>::val
//#endif
//#ifdef __RV_EXTENSION_N__
//| bit_mask<UXLenT, 13>::val
//#endif
//#ifdef __RV_EXTENSION_P__
//| bit_mask<UXLenT, 15>::val
//#endif
//#ifdef __RV_EXTENSION_Q__
//| bit_mask<UXLenT, 16>::val
//#endif
//#ifdef __RV_PRIVILEGE_S__
//| bit_mask<UXLenT, 18>::val
//#endif
//#ifdef __RV_EXTENSION_T__
//| bit_mask<UXLenT, 19>::val
//#endif
//#ifdef __RV_PRIVILEGE_U__
//| bit_mask<UXLenT, 20>::val
//#endif
//#ifdef __RV_EXTENSION_V__
//| bit_mask<UXLenT, 21>::val
//#endif
//#ifdef __RV_NON_STANDARD__
//| bit_mask<UXLenT, 23>::val
//#endif

namespace riscv_isa {
    template<typename xlen=xlen_trait>
    class CSRRegister {

    public:
        /// User Trap Setup
        _riscv_isa_declare_csr(USTATUS, ustatus, 0x000); /// User status register.
        _riscv_isa_declare_csr(UIE, uie, 0x004); /// User interrupt-enable register.
        _riscv_isa_declare_csr(UTVEC, utvec, 0x005); /// User trap handler base address.
        /// User Trap Handling
        _riscv_isa_declare_csr(USCRATCH, uscratch, 0x040); /// Scratch register for user trap handlers.
        _riscv_isa_declare_csr(UEPC, uepc, 0x041); /// User exception program counter.
        _riscv_isa_declare_csr(UCAUSE, ucause, 0x042); /// User trap cause.
        _riscv_isa_declare_csr(UTVAL, utval, 0x043); /// User bad address or instruction.
        _riscv_isa_declare_csr(UIP, uip, 0x044); /// User interrupt pending.
        /// User Floating-Point CSRs
        _riscv_isa_declare_csr(FFLAGS, fflags, 0x001); /// Floating-Point Accrued Exceptions.
        _riscv_isa_declare_csr(FRM, frm, 0x002); /// Floating-Point Dynamic Rounding Mode.
        _riscv_isa_declare_csr(FCSR, fcsr, 0x003); /// Floating-Point Control and Status Register.
        /// User Counter/Timers
        _riscv_isa_declare_csr(CYCLE, cycle, 0xC00); /// Cycle counter for RDCYCLE instruction.
        _riscv_isa_declare_csr(TIME, time, 0xC01); /// Timer for RDTIME instruction
        _riscv_isa_declare_csr(INSTRET, instret, 0xC02); /// Instructions-retired counter for RDINSTRET instruction.
        _riscv_isa_csr_reg_range(3, 32, HPMCOUNTER, hpmcounter, 0xC00); /// Performance-monitoring counter.
//      _riscv_isa_declare_csr(HPMCOUNTER3, hpmcounter3, 0xC03);
//      ...
//      _riscv_isa_declare_csr(HPMCOUNTER31, hpmcounter31, 0xC1F);
#if __RV_BIT_WIDTH__ == 32
        /// User Counter/Timers Upper 32 bits
        _riscv_isa_declare_csr(CYCLEH, cycleh, 0xC80);
        _riscv_isa_declare_csr(TIMEH, timeh, 0xC81);
        _riscv_isa_declare_csr(INSTRETH, instreth, 0xC82);
        _riscv_isa_csr_reg_pos_range(3, 32, HPMCOUNTER, hpmcounter, H, h, 0xC80);
//      _riscv_isa_declare_csr(HPMCOUNTER3H, hpmcounter3h, 0xC83);
//      ...
//      _riscv_isa_declare_csr(HPMCOUNTER31H, hpmcounter31h, 0xC9F);
#endif
        /// Supervisor Trap Setup
        _riscv_isa_declare_csr(SSTATUS, sstatus, 0x100); /// Supervisor status register.
        _riscv_isa_declare_csr(SEDELEG, sedeleg, 0x102); /// Supervisor exception delegation register.
        _riscv_isa_declare_csr(SIDELEG, sideleg, 0x103); /// Supervisor interrupt delegation register.
        _riscv_isa_declare_csr(SIE, sie, 0x104); /// Supervisor interrupt-enable register.
        _riscv_isa_declare_csr(STVEC, stvec, 0x105); /// Supervisor trap handler base address.
        _riscv_isa_declare_csr(SCOUNTEREN, scounteren, 0x106); /// Supervisor counter enable.
        /// Supervisor Trap Handling
        _riscv_isa_declare_csr(SSCRATCH, sscratch, 0x140); /// Scratch register for supervisor trap handlers.
        _riscv_isa_declare_csr(SEPC, sepc, 0x141); /// Supervisor exception program counter.
        _riscv_isa_declare_csr(SCAUSE, scause, 0x142); /// Supervisor trap cause.
        _riscv_isa_declare_csr(STVAL, stval, 0x143); /// Supervisor bad address or instruction.
        _riscv_isa_declare_csr(SIP, sip, 0x144); /// Supervisor interrupt pending.
        /// Supervisor Protection and Translation
        _riscv_isa_declare_csr(SATP, satp, 0x180); /// Supervisor address translation and protection.
        /// Hypervisor Trap Setup
        _riscv_isa_declare_csr(HSTATUS, hstatus, 0x600); /// Hypervisor status register.
        _riscv_isa_declare_csr(HEDELEG, hedeleg, 0x602); /// Hypervisor exception delegation register.
        _riscv_isa_declare_csr(HIDELEG, hideleg, 0x603); /// Hypervisor interrupt delegation register.
        _riscv_isa_declare_csr(HIE, hie, 0x604); /// Hypervisor interrupt-enable register.
        _riscv_isa_declare_csr(HCOUNTEREN, hcounteren, 0x606); /// Hypervisor counter enable.
        _riscv_isa_declare_csr(HGEIE, hgeie, 0x606); /// Hypervisor guest external interrupt-enable register.
        /// Hypervisor Trap Handling
        _riscv_isa_declare_csr(HTVAL, htval, 0x643); /// Hypervisor bad guest physical address.
        _riscv_isa_declare_csr(HIP, hip, 0x644); /// Hypervisor interrupt pending.
        _riscv_isa_declare_csr(HTINST, htinst, 0x64A); /// Hypervisor trap instruction (transformed).
        _riscv_isa_declare_csr(HGEIP, hgeip, 0xE07); /// Hypervisor guest external interrupt pending.
        /// Hypervisor Protection and Translation
        _riscv_isa_declare_csr(HGATP, hgatp, 0x680); /// Hypervisor guest address translation and protection.
        /// Hypervisor Counter/Timer Virtualization Registers
        _riscv_isa_declare_csr(HTIMEDELTA, htimedelta, 0x605);
#if __RV_BIT_WIDTH__ == 32
        _riscv_isa_declare_csr(HTIMEDELTAH, htimedeltah, 0x615);
#endif
        /// Virtual Supervisor Registers
        _riscv_isa_declare_csr(VSSTATUS, vsstatus, 0x200); /// Virtual supervisor status register.
        _riscv_isa_declare_csr(VSIE, vsie, 0x204); /// Virtual supervisor interrupt-enable register.
        _riscv_isa_declare_csr(VSTVEC, vstvec, 0x205); /// Virtual supervisor trap handler base address.
        _riscv_isa_declare_csr(VSSCRATCH, vsscratch, 0x240); /// Virtual supervisor scratch register.
        _riscv_isa_declare_csr(VSEPC, vsepc, 0x241); /// Virtual supervisor exception program counter.
        _riscv_isa_declare_csr(VSCAUSE, vscause, 0x242); /// Virtual supervisor trap cause.
        _riscv_isa_declare_csr(VSTVAL, vstval, 0x243); /// Virtual supervisor bad address or instruction.
        _riscv_isa_declare_csr(VSIP, vsip, 0x244); /// Virtual supervisor interrupt pending.
        _riscv_isa_declare_csr(VSATP, vsatp, 0x280); /// Virtual supervisor address translation and protection.
        /// Machine Information Registers
        _riscv_isa_declare_csr(MVENDORID, mvendorid, 0xF11);
        _riscv_isa_declare_csr(MARCHID, marchid, 0xF12);
        _riscv_isa_declare_csr(MIMPID, mimpid, 0xF13);
        _riscv_isa_declare_csr(MHARTID, mhartid, 0xF14); /// Machine Trap Setup
        _riscv_isa_declare_csr(MSTATUS, mstatus, 0x300); /// Machine status register.
        _riscv_isa_declare_csr(MISA, misa, 0x301); /// ISA and extensions.
        _riscv_isa_declare_csr(MEDELEG, medeleg, 0x302); /// Machine exception delegation register.
        _riscv_isa_declare_csr(MIDELEG, mideleg, 0x303); /// Machine interrupt delegation register.
        _riscv_isa_declare_csr(MIE, mie, 0x304); /// Machine interrupt-enable register.
        _riscv_isa_declare_csr(MTVEC, mtvec, 0x305); /// Machine trap handler base address.
        _riscv_isa_declare_csr(MCOUNTEREN, mcounteren, 0x306); /// Machine counter enable.
#if __RV_BIT_WIDTH__ == 32
        _riscv_isa_declare_csr(MSTATUSH, mstatush, 0x310); /// Additional machine status register.
#endif
        /// Machine Trap Handling
        _riscv_isa_declare_csr(MSCRATCH, mscratch, 0x340); /// Scratch register for machine trap handlers.
        _riscv_isa_declare_csr(MEPC, mepc, 0x341); /// Machine exception program counter.
        _riscv_isa_declare_csr(MCAUSE, mcause, 0x342); /// Machine trap cause.
        _riscv_isa_declare_csr(MTVAL, mtval, 0x343); /// Machine bad address or instruction.
        _riscv_isa_declare_csr(MIP, mip, 0x344); /// Machine interrupt pending.
        _riscv_isa_declare_csr(MTINST, mtinst, 0x34A); /// Machine trap instruction (transformed).
        _riscv_isa_declare_csr(MTVAL2, mtval2, 0x34B); /// Machine bad guest physical address.
        /// Machine Memory Protection
        _riscv_isa_declare_csr(PMPCFG0, pmpcfg0, 0x3A0); /// Physical memory protection configuration.
#if __RV_BIT_WIDTH__ == 32
        _riscv_isa_declare_csr(PMPCFG1, pmpcfg1, 0x3A1);
#endif
        _riscv_isa_declare_csr(PMPCFG2, pmpcfg2, 0x3A2);
#if __RV_BIT_WIDTH__ == 32
        _riscv_isa_declare_csr(PMPCFG3, pmpcfg3, 0x3A3);
#endif
        _riscv_isa_csr_reg_range(0, 16, PMPADDR, pmpaddr, 0x3B0); /// Physical memory protection address register.
//      _riscv_isa_declare_csr(PMPADDR0, pmpaddr0, 0x3B0);
//      ...
//      _riscv_isa_declare_csr(PMPADDR15, pmpaddr15, 0x3BF);
        /// Machine Counter/Timers
        _riscv_isa_declare_csr(MCYCLE, mcycle, 0xB00); /// Machine cycle counter.
        _riscv_isa_declare_csr(MINSTRET, minstret, 0xB02); /// Machine instructions-retired counter.
        _riscv_isa_csr_reg_range(3, 32, MHPMCOUNTER, mhpmcounter, 0xB00); /// Machine performance-monitoring counter.
//      _riscv_isa_declare_csr(MHPMCOUNTER3, mhpmcounter3, 0xB03);
//      ...
//      _riscv_isa_declare_csr(MHPMCOUNTER31, mhpmcounter31, 0xB1F);
#if __RV_BIT_WIDTH__ == 32
        /// User Counter/Timers Upper 32 bits
        _riscv_isa_declare_csr(MCYCLEH, mcycleh, 0xB80);
        _riscv_isa_declare_csr(MINSTRETH, minstreth, 0xB82);
        _riscv_isa_csr_reg_pos_range(3, 32, MHPMCOUNTER, mhpmcounter, H, h, 0xB00);
//      _riscv_isa_declare_csr(MHPMCOUNTER3H, mhpmcounter3h, 0xB83);
//      ...
//      _riscv_isa_declare_csr(MHPMCOUNTER31H, mhpmcounter31h, 0xB9F);
#endif
        /// Machine Counter Setup
        _riscv_isa_declare_csr(MCOUNTINHIBIT, mcountinhibit, 0x320); /// Machine counter-inhibit register.
        _riscv_isa_csr_reg_range(3, 32, MHPMEVENT, mhpmevent, 0xB00); /// Machine performance-monitoring event selector.
//      _riscv_isa_declare_csr(MHPMEVENT3, mhpmevent3, 0x323);
//      ...
//      _riscv_isa_declare_csr(MHPMEVENT31, mhpmevent31, 0x33F);
        /// Debug/Trace Registers (shared with Debug Mode)
        _riscv_isa_declare_csr(TSELECT, tselect, 0x7A0); /// Debug/Trace trigger register select.
        _riscv_isa_declare_csr(TDATA1, tdata1, 0x7A1); /// First Debug/Trace trigger data register.
        _riscv_isa_declare_csr(TDATA2, tdata2, 0x7A2); /// Second Debug/Trace trigger data register.
        _riscv_isa_declare_csr(TDATA3, tdata3, 0x7A3); /// Third Debug/Trace trigger data register.
        /// Debug Mode Registers
        _riscv_isa_declare_csr(DCSAR, dcsar, 0x7B0); /// Debug control and status register.
        _riscv_isa_declare_csr(DPC, dpc, 0x7B1); /// Debug PC.
        _riscv_isa_declare_csr(DSCRATCH0, dscratch0, 0x7B2); /// Debug scratch register 0.
        _riscv_isa_declare_csr(DSCRATCH1, dscratch1, 0x7B3); /// Debug scratch register 1.

        CSRRegister() = default;
    };
}


#endif // RISCV_ISA_CSR_REGISTER_HPP