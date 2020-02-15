#ifndef RISCV_ISA_PRIVILEGED_INSTRUCTION_HPP
#define RISCV_ISA_PRIVILEGED_INSTRUCTION_HPP


#include "instruction.hpp"


#if defined(__RV_EXTENSION_N__)
#define riscv_isa_instruction_uret_map(func) \
    func(URET, uret)
#else
#define riscv_isa_instruction_uret_map(func)
#endif // defined(__RV_EXTENSION_N__)

#if defined(__RV_SUPERVISOR_MODE__)
#define riscv_isa_instruction_privilege_supervisor_map(func) \
    func(SRET, sret) \
    func(SFENCEVAM, sfencevma)
#else
#define riscv_isa_instruction_privilege_supervisor_map(func)
#endif // defined(__RV_SUPERVISOR_MODE__)

#if defined(__RV_HYPERVISOR_MODE__)
#define riscv_isa_instruction_privilege_hypervisor_map(func) \
    func(HFENCEVVMA, hfencevvma) \
    func(HFENCEGVMA, hfencegvma)
#else
#define riscv_isa_instruction_privilege_hypervisor_map(func)
#endif // defined(__RV_HYPERVISOR_MODE__)

#define riscv_isa_instruction_privilege_machine_map(func) \
    func(MRET, mret) \
    func(WFI, wfi)

#define riscv_isa_instruction_privilege_map(func) \
    riscv_isa_instruction_uret_map(func) \
    riscv_isa_instruction_privilege_supervisor_map(func) \
    riscv_isa_instruction_privilege_hypervisor_map(func) \
    riscv_isa_instruction_privilege_machine_map(func)


namespace riscv_isa {
#if defined(__RV_EXTENSION_N__)

    class URETInst : public InstructionPrivilegedSet {
    public:
        using BaseT = InstructionSystemSet;

        static bool is_self_type(BaseT *self) {
            return reinterpret_cast<URETInst *>(self)->inner == URETInst().inner;
        }

        static constexpr UInnerT FUNCT7 = 0b0000000;
        static constexpr UInnerT FUNCT_PRIVILEGE = 0b00010;

        URETInst() : InstructionPrivilegedSet{0, 0, FUNCT_PRIVILEGE, FUNCT7} {}

        friend std::ostream &operator<<(std::ostream &stream, riscv_isa_unused const URETInst &inst) {
            stream << "uret";
            return stream;
        }
    };

#endif // defined(__RV_EXTENSION_N__)
#if defined(__RV_SUPERVISOR_MODE__)

    class SRETInst : public InstructionPrivilegedSet {
    public:
        using BaseT = InstructionSystemSet;

        static bool is_self_type(BaseT *self) {
            return reinterpret_cast<SRETInst *>(self)->inner == SRETInst().inner;
        }

        static constexpr UInnerT FUNCT7 = 0b0001000;
        static constexpr UInnerT FUNCT_PRIVILEGE = 0b00010;

        SRETInst() : InstructionPrivilegedSet{0, 0, FUNCT_PRIVILEGE, FUNCT7} {}

        friend std::ostream &operator<<(std::ostream &stream, riscv_isa_unused const SRETInst &inst) {
            stream << "sret";
            return stream;
        }
    };

#endif // defined(__RV_SUPERVISOR_MODE__)

    class MRETInst : public InstructionPrivilegedSet {
    public:
        using BaseT = InstructionSystemSet;

        static bool is_self_type(BaseT *self) {
            return reinterpret_cast<MRETInst *>(self)->inner == MRETInst().inner;
        }

        static constexpr UInnerT FUNCT7 = 0b0011000;
        static constexpr UInnerT FUNCT_PRIVILEGE = 0b00010;

        MRETInst() : InstructionPrivilegedSet{0, 0, FUNCT_PRIVILEGE, FUNCT7} {}

        friend std::ostream &operator<<(std::ostream &stream, riscv_isa_unused const MRETInst &inst) {
            stream << "mret";
            return stream;
        }
    };

    class WFIInst : public InstructionPrivilegedSet {
    public:
        using BaseT = InstructionSystemSet;

        static bool is_self_type(BaseT *self) {
            return reinterpret_cast<WFIInst *>(self)->inner == WFIInst().inner;
        }

        static constexpr UInnerT FUNCT7 = 0b0001000;
        static constexpr UInnerT FUNCT_PRIVILEGE = 0b00101;

        WFIInst() : InstructionPrivilegedSet{0, 0, FUNCT_PRIVILEGE, FUNCT7} {}

        friend std::ostream &operator<<(std::ostream &stream, riscv_isa_unused const WFIInst &inst) {
            stream << "wfi";
            return stream;
        }
    };

#if defined(__RV_SUPERVISOR_MODE__)

    class SFENCEVAMInst : public InstructionPrivilegedSet {
    public:
        using BaseT = InstructionSystemSet;

        static bool is_self_type(BaseT *self) {
            return self->get_funct3() == FUNCT3 && self->get_funct7() == FUNCT7 && self->get_rd() == 0;
        }

        static constexpr UInnerT FUNCT7 = 0b0001001;

        SFENCEVAMInst(usize rs1, usize rs2) : InstructionPrivilegedSet{0, rs1, rs2, FUNCT7} {}

        friend std::ostream &operator<<(std::ostream &stream, riscv_isa_unused const SFENCEVAMInst &inst) {
            stream << "sfence.vam\tx" << inst.get_rs1() << " x" << inst.get_rs2();
            return stream;
        }
    };

#endif // defined(__RV_SUPERVISOR_MODE__)
#if defined(__RV_HYPERVISOR_MODE__)

    class HFENCEVVMAInst : public InstructionPrivilegedSet {
    public:
        using BaseT = InstructionSystemSet;

        static bool is_self_type(BaseT *self) {
            return self->get_funct3() == FUNCT3 && self->get_funct7() == FUNCT7 && self->get_rd() == 0;
        }

        static constexpr UInnerT FUNCT7 = 0b0010001;

        HFENCEVVMAInst(usize rs1, usize rs2) : InstructionPrivilegedSet{0, rs1, rs2, FUNCT7} {}

        friend std::ostream &operator<<(std::ostream &stream, riscv_isa_unused const HFENCEVVMAInst &inst) {
            stream << "hfence.vvam\tx" << inst.get_rs1() << " x" << inst.get_rs2();
            return stream;
        }
    };

    class HFENCEGVMAInst : public InstructionPrivilegedSet {
    public:
        using BaseT = InstructionSystemSet;

        static bool is_self_type(BaseT *self) {
            return self->get_funct3() == FUNCT3 && self->get_funct7() == FUNCT7 && self->get_rd() == 0;
        }

        static constexpr UInnerT FUNCT7 = 0b0110001;

        HFENCEGVMAInst(usize rs1, usize rs2) : InstructionPrivilegedSet{0, rs1, rs2, FUNCT7} {}

        friend std::ostream &operator<<(std::ostream &stream, riscv_isa_unused const HFENCEGVMAInst &inst) {
            stream << "hfence.gvam\tx" << inst.get_rs1() << " x" << inst.get_rs2();
            return stream;
        }
    };

#endif // defined(__RV_HYPERVISOR_MODE__)
}


#endif //RISCV_ISA_PRIVILEGED_INSTRUCTION_HPP
