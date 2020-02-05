#ifndef RISCV_ISA_ZICSR_HPP
#define RISCV_ISA_ZICSR_HPP


#include "instruction.hpp"


#if defined(__RV_EXTENSION_ZICSR__)
namespace riscv_isa {
    class CSRRWInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b001;

        template<typename RegT>
        void operator()(RegT &reg) const {
            // todo
        }
    };

    class CSRRSInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b010;

        template<typename RegT>
        void operator()(RegT &reg) const {
            // todo
        }
    };

    class CSRRCInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b011;

        template<typename RegT>
        void operator()(RegT &reg) const {
            // todo
        }
    };

    class CSRRWIInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b101;

        template<typename RegT>
        void operator()(RegT &reg) const {
            // todo
        }
    };

    class CSRRSIInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b110;

        template<typename RegT>
        void operator()(RegT &reg) const {
            // todo
        }
    };

    class CSRRCIInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNCT3 = 0b111;

        template<typename RegT>
        void operator()(RegT &reg) const {
            // todo
        }
    };
}
#endif // defined(__RV_EXTENSION_ZICSR__)


#endif //RISCV_ISA_ZICSR_HPP
