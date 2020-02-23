#ifndef RISCV_ISA_INTEGER_REGISTER_HPP
#define RISCV_ISA_INTEGER_REGISTER_HPP


#include "riscv_isa_utility.hpp"


namespace riscv_isa {
    template<typename xlen=xlen_trait>
    class IntegerRegister {
    private:
        using XlenT = typename xlen::XLenT;

#if defined(__RV_BASE_E__)
        static constexpr usize INTEGER_REGISTER_NUM = 16;
#else
        static constexpr usize INTEGER_REGISTER_NUM = 32;
#endif
        /// register size is controlled by macro
        typename xlen_trait::XLenT x[INTEGER_REGISTER_NUM];

    public:
        enum IntRegIndex : usize {
            ZERO = 0,
            RA = 1,
            SP = 2,
            GP = 3,
            TP = 4,
            T0 = 5,
            T1 = 6,
            T2 = 7,
            S0 = 8,
            FP = 8,
            S1 = 9,
            A0 = 10,
            A1 = 11,
            A2 = 12,
            A3 = 13,
            A4 = 14,
            A5 = 15,
#if !defined(__RV_BASE_E__)
            A6 = 16,
            A7 = 17,
            S2 = 18,
            S3 = 19,
            S4 = 20,
            S5 = 21,
            S6 = 22,
            S7 = 23,
            S8 = 24,
            S9 = 25,
            S10 = 26,
            S11 = 27,
            T3 = 28,
            T4 = 29,
            T5 = 30,
            T6 = 31,
#endif // !defined(__RV_EXTENSION_E__)
        };

        static bool is_link(usize reg) { return reg == 1 || reg == 5; }

        IntegerRegister() : x{} {}

        void set_x(usize index, XlenT val) {
            if (index >= INTEGER_REGISTER_NUM) riscv_isa_abort("Integer register index exceed.");
            if (index != 0) x[index] = val;
        }

        XlenT get_x(usize index) const {
            if (index >= INTEGER_REGISTER_NUM) riscv_isa_abort("Integer register index exceed.");
            return x[index];
        }
    };
}


#endif //RISCV_ISA_INTEGER_REGISTER_HPP
