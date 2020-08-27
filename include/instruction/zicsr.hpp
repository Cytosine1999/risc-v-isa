#ifndef RISCV_ISA_ZICSR_HPP
#define RISCV_ISA_ZICSR_HPP


#include "instruction.hpp"


#if defined(__RV_EXTENSION_ZICSR__)
namespace riscv_isa {
    class InstructionCSRSet : public InstructionSystemSet {
    public:
        UInnerT get_csr() const { return get_bits<UInnerT, 32, 20>(inner); }
    };

    class CSRRWInst : public InstructionCSRSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b001;

        friend std::ostream &operator<<(std::ostream &stream, const CSRRWInst &self) {
            stream << "csrrw\tx" << self.get_rd() << ", csr" << self.get_csr() << ", x" << self.get_rs1();

            return stream;
        }
    };

    class CSRRSInst : public InstructionCSRSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b010;

        friend std::ostream &operator<<(std::ostream &stream, const CSRRSInst &self) {
            stream << "csrrs\tx" << self.get_rd() << ", csr" << self.get_csr() << ", x" << self.get_rs1();

            return stream;
        }
    };

    class CSRRCInst : public InstructionCSRSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b011;

        friend std::ostream &operator<<(std::ostream &stream, const CSRRCInst &self) {
            stream << "csrrc\tx" << self.get_rd() << ", csr" << self.get_csr() << ", x" << self.get_rs1();

            return stream;
        }
    };

    class CSRRWIInst : public InstructionCSRSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b101;

        friend std::ostream &operator<<(std::ostream &stream, const CSRRWIInst &self) {
            stream << "csrrwi\tx" << self.get_rd() << ", csr" << self.get_csr() << ", " << self.get_rs1();

            return stream;
        }
    };

    class CSRRSIInst : public InstructionCSRSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b110;

        friend std::ostream &operator<<(std::ostream &stream, const CSRRSIInst &self) {
            stream << "csrrsi\tx" << self.get_rd() << ", csr" << self.get_csr() << ", " << self.get_rs1();

            return stream;
        }
    };

    class CSRRCIInst : public InstructionCSRSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b111;

        friend std::ostream &operator<<(std::ostream &stream, const CSRRCIInst &self) {
            stream << "csrrci\tx" << self.get_rd() << ", csr" << self.get_csr() << ", " << self.get_rs1();

            return stream;
        }
    };
}


#define riscv_isa_instruction_zicsr_map(func) \
    func(CSRRW, csrrw) \
    func(CSRRS, csrrs) \
    func(CSRRC, csrrc) \
    func(CSRRWI, csrrwi) \
    func(CSRRSI, csrrsi) \
    func(CSRRCI, csrrci)
#else
#define riscv_isa_instruction_zicsr_map(func)
#endif // defined(__RV_EXTENSION_ZICSR__)


#endif //RISCV_ISA_ZICSR_HPP
