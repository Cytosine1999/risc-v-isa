#ifndef RISC_V_ISA_ZIFENCEI_HPP
#define RISC_V_ISA_ZIFENCEI_HPP


#include "instruction.hpp"


class FENCEIInst : public InstructionFenceSet {
public:
    static constexpr UInnerT FUNC_3 = 0b001;

    usize get_unused() const { return inner & (BITS_MASK<UInnerT, 32, 20>); }

    template<typename RegT>
    void operator()(RegT &reg) const {
        static_assert(std::is_base_of<RegisterFile, RegT>::value);

        // todo
    }
};


#endif //RISC_V_ISA_ZIFENCEI_HPP
