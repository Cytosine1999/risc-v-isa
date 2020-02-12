#ifndef RISCV_ISA_INTEGER_REGISTER_HPP
#define RISCV_ISA_INTEGER_REGISTER_HPP


#include "utility.hpp"


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
        static constexpr usize ZERO = 0;
        static constexpr usize RA = 1;
        static constexpr usize SP = 2;
        static constexpr usize GP = 3;
        static constexpr usize TP = 4;
        static constexpr usize T0 = 5;
        static constexpr usize T1 = 6;
        static constexpr usize T2 = 7;
        static constexpr usize S0 = 8;
        static constexpr usize FP = 8;
        static constexpr usize S1 = 9;
        static constexpr usize A0 = 10;
        static constexpr usize A1 = 11;
        static constexpr usize A2 = 12;
        static constexpr usize A3 = 13;
        static constexpr usize A4 = 14;
        static constexpr usize A5 = 15;
#if !defined(__RV_BASE_E__)
        static constexpr usize A6 = 16;
        static constexpr usize A7 = 17;
        static constexpr usize S2 = 18;
        static constexpr usize S3 = 19;
        static constexpr usize S4 = 20;
        static constexpr usize S5 = 21;
        static constexpr usize S6 = 22;
        static constexpr usize S7 = 23;
        static constexpr usize S8 = 24;
        static constexpr usize S9 = 25;
        static constexpr usize S10 = 26;
        static constexpr usize S11 = 27;
        static constexpr usize T3 = 28;
        static constexpr usize T4 = 29;
        static constexpr usize T5 = 30;
        static constexpr usize T6 = 31;
#endif // !defined(__RV_EXTENSION_E__)

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
