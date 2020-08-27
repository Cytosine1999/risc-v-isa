#ifndef NEUTRON_RV32A_HPP
#define NEUTRON_RV32A_HPP


#include "riscv_isa_utility.hpp"
#include "instruction.hpp"


#if defined(__RV_EXTENSION_A__)
namespace riscv_isa {
    class InstructionAtomicWSet : public InstructionAtomicSet {
    protected:
        InstructionAtomicWSet(usize rd, usize rs1, usize rs2, bool aq, bool rl, usize funct_atomic) :
                InstructionAtomicSet{rd, FUNCT3, rs1, rs2, aq, rl, funct_atomic} {}

    public:
        static constexpr UInnerT FUNCT3 = 0b010;
    };

    class LRWInst : public InstructionAtomicWSet {
    public:
        LRWInst(usize rd, usize rs1, bool aq, bool rl) :
                InstructionAtomicWSet{rd, rs1, 0, aq, rl, FUNCT_ATOMIC} {}

        static constexpr UInnerT FUNCT_ATOMIC = 0b00010;

        friend std::ostream &operator<<(std::ostream &stream, const LRWInst &inst) {
            stream << "lr.w";
            if (inst.get_rl()) stream << ".rl";
            if (inst.get_aq()) stream << ".aq";
            stream << "\tx" << inst.get_rd() << ",(x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class SCWInst : public InstructionAtomicWSet {
    public:
        SCWInst(usize rd, usize rs1, usize rs2, bool aq, bool rl) :
                InstructionAtomicWSet{rd, rs1, rs2, aq, rl, FUNCT_ATOMIC} {}

        static constexpr UInnerT FUNCT_ATOMIC = 0b00011;

        friend std::ostream &operator<<(std::ostream &stream, const SCWInst &inst) {
            stream << "sc.w";
            if (inst.get_rl()) stream << ".rl";
            if (inst.get_aq()) stream << ".aq";
            stream << "\tx" << inst.get_rd() << ", x" << inst.get_rs2() << ", (x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class AMOSWAPWInst : public InstructionAtomicWSet {
    public:
        AMOSWAPWInst(usize rd, usize rs1, usize rs2, bool aq, bool rl) :
                InstructionAtomicWSet{rd, rs1, rs2, aq, rl, FUNCT_ATOMIC} {}

        static constexpr UInnerT FUNCT_ATOMIC = 0b00001;

        friend std::ostream &operator<<(std::ostream &stream, const AMOSWAPWInst &inst) {
            stream << "amoswap.w";
            if (inst.get_rl()) stream << ".rl";
            if (inst.get_aq()) stream << ".aq";
            stream << "\tx" << inst.get_rd() << ", x" << inst.get_rs2() << ", (x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class AMOADDWInst : public InstructionAtomicWSet {
    public:
        AMOADDWInst(usize rd, usize rs1, usize rs2, bool aq, bool rl) :
                InstructionAtomicWSet{rd, rs1, rs2, aq, rl, FUNCT_ATOMIC} {}

        static constexpr UInnerT FUNCT_ATOMIC = 0b00000;

        friend std::ostream &operator<<(std::ostream &stream, const AMOADDWInst &inst) {
            stream << "amoadd.w";
            if (inst.get_rl()) stream << ".rl";
            if (inst.get_aq()) stream << ".aq";
            stream << "\tx" << inst.get_rd() << ", x" << inst.get_rs2() << ", (x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class AMOXORWInst : public InstructionAtomicWSet {
    public:
        AMOXORWInst(usize rd, usize rs1, usize rs2, bool aq, bool rl) :
                InstructionAtomicWSet{rd, rs1, rs2, aq, rl, FUNCT_ATOMIC} {}

        static constexpr UInnerT FUNCT_ATOMIC = 0b00100;

        friend std::ostream &operator<<(std::ostream &stream, const AMOXORWInst &inst) {
            stream << "amoxor.w";
            if (inst.get_rl()) stream << ".rl";
            if (inst.get_aq()) stream << ".aq";
            stream << "\tx" << inst.get_rd() << ", x" << inst.get_rs2() << ", (x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class AMOANDWInst : public InstructionAtomicWSet {
    public:
        AMOANDWInst(usize rd, usize rs1, usize rs2, bool aq, bool rl) :
                InstructionAtomicWSet{rd, rs1, rs2, aq, rl, FUNCT_ATOMIC} {}

        static constexpr UInnerT FUNCT_ATOMIC = 0b01100;

        friend std::ostream &operator<<(std::ostream &stream, const AMOANDWInst &inst) {
            stream << "amoand.w";
            if (inst.get_rl()) stream << ".rl";
            if (inst.get_aq()) stream << ".aq";
            stream << "\tx" << inst.get_rd() << ", x" << inst.get_rs2() << ", (x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class AMOORWInst : public InstructionAtomicWSet {
    public:
        AMOORWInst(usize rd, usize rs1, usize rs2, bool aq, bool rl) :
                InstructionAtomicWSet{rd, rs1, rs2, aq, rl, FUNCT_ATOMIC} {}

        static constexpr UInnerT FUNCT_ATOMIC = 0b01000;

        friend std::ostream &operator<<(std::ostream &stream, const AMOORWInst &inst) {
            stream << "amoor.w";
            if (inst.get_rl()) stream << ".rl";
            if (inst.get_aq()) stream << ".aq";
            stream << "\tx" << inst.get_rd() << ", x" << inst.get_rs2() << ", (x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class AMOMINWInst : public InstructionAtomicWSet {
    public:
        AMOMINWInst(usize rd, usize rs1, usize rs2, bool aq, bool rl) :
                InstructionAtomicWSet{rd, rs1, rs2, aq, rl, FUNCT_ATOMIC} {}

        static constexpr UInnerT FUNCT_ATOMIC = 0b10000;

        friend std::ostream &operator<<(std::ostream &stream, const AMOMINWInst &inst) {
            stream << "amomin.w";
            if (inst.get_rl()) stream << ".rl";
            if (inst.get_aq()) stream << ".aq";
            stream << "\tx" << inst.get_rd() << ", x" << inst.get_rs2() << ", (x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class AMOMAXWInst : public InstructionAtomicWSet {
    public:
        AMOMAXWInst(usize rd, usize rs1, usize rs2, bool aq, bool rl) :
                InstructionAtomicWSet{rd, rs1, rs2, aq, rl, FUNCT_ATOMIC} {}

        static constexpr UInnerT FUNCT_ATOMIC = 0b10100;

        friend std::ostream &operator<<(std::ostream &stream, const AMOMAXWInst &inst) {
            stream << "amomax.w";
            if (inst.get_rl()) stream << ".rl";
            if (inst.get_aq()) stream << ".aq";
            stream << "\tx" << inst.get_rd() << ", x" << inst.get_rs2() << ", (x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class AMOMINUWInst : public InstructionAtomicWSet {
    public:
        AMOMINUWInst(usize rd, usize rs1, usize rs2, bool aq, bool rl) :
                InstructionAtomicWSet{rd, rs1, rs2, aq, rl, FUNCT_ATOMIC} {}

        static constexpr UInnerT FUNCT_ATOMIC = 0b11000;

        friend std::ostream &operator<<(std::ostream &stream, const AMOMINUWInst &inst) {
            stream << "amominu.w";
            if (inst.get_rl()) stream << ".rl";
            if (inst.get_aq()) stream << ".aq";
            stream << "\tx" << inst.get_rd() << ", x" << inst.get_rs2() << ", (x" << inst.get_rs1() << ')';
            return stream;
        }
    };

    class AMOMAXUWInst : public InstructionAtomicWSet {
    public:
        AMOMAXUWInst(usize rd, usize rs1, usize rs2, bool aq, bool rl) :
                InstructionAtomicWSet{rd, rs1, rs2, aq, rl, FUNCT_ATOMIC} {}

        static constexpr UInnerT FUNCT_ATOMIC = 0b11100;

        friend std::ostream &operator<<(std::ostream &stream, const AMOMAXUWInst &inst) {
            stream << "amomaxu.w";
            if (inst.get_rl()) stream << ".rl";
            if (inst.get_aq()) stream << ".aq";
            stream << "\tx" << inst.get_rd() << ", x" << inst.get_rs2() << ", (x" << inst.get_rs1() << ')';
            return stream;
        }
    };
}

#define riscv_isa_instruction_32a_map(func) \
    func(LRW, lrw) \
    func(SCW, scw) \
    func(AMOSWAPW, amoswapw) \
    func(AMOADDW, amoaddw) \
    func(AMOXORW, amoxorw) \
    func(AMOANDW, amoandw) \
    func(AMOORW, amoorw) \
    func(AMOMINW, amominw) \
    func(AMOMAXW, amomaxw) \
    func(AMOMINUW, amominuw) \
    func(AMOMAXUW, amomaxuw)

#else
#define riscv_isa_instruction_32a_map(func)
#endif // defined(__RV_EXTENSION_A__)


#endif //NEUTRON_RV32A_HPP
