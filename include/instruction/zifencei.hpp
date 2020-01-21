#ifndef RISC_V_ISA_ZIFENCEI_HPP
#define RISC_V_ISA_ZIFENCEI_HPP


#include "instruction.hpp"


#if defined(__RV_EXTENSION_ZIFENCEI__)

namespace risc_v_isa {
    class FENCEIInst : public InstructionFenceSet {
    public:
        static constexpr UInnerT FUNC_3 = 0b001;

        template<typename RegT>
        void operator()(RegT &reg) const {
            static_assert(std::is_base_of<RegisterFile, RegT>::value);

            // todo
        }
    };
}

#endif // defined(__RV_EXTENSION_ZIFENCEI__)


#endif //RISC_V_ISA_ZIFENCEI_HPP
