#ifndef RISC_V_ISA_ZICSR_HPP
#define RISC_V_ISA_ZICSR_HPP


#include "instruction.hpp"


#if defined(__RV_ZICSR_EXTENSION__)
namespace risc_v_isa {
    class CSRRWInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            // todo
        }
    };

    class CSRRSInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b010;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            // todo
        }
    };

    class CSRRCInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b011;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            // todo
        }
    };

    class CSRRWIInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b101;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            // todo
        }
    };

    class CSRRSIInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b110;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            // todo
        }
    };

    class CSRRCIInst : public InstructionSystemSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b111;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            // todo
        }
    };
}
#endif // defined(__RV_ZICSR_EXTENSION__)


#endif //RISC_V_ISA_ZICSR_HPP
