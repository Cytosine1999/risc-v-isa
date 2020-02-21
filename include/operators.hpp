#ifndef RISCV_ISA_OPERATORS_HPP
#define RISCV_ISA_OPERATORS_HPP


#include "utility.hpp"


#define _riscv_isa_use_all_xlen(xlen) \
    using XLenT = typename xlen::XLenT; \
    using UXLenT = typename xlen::UXLenT; \
    static constexpr usize HALF_WIDTH = xlen::HALF_WIDTH; \
    static constexpr UXLenT HALF_MASK = xlen::HALF_MASK; \
    static constexpr XLenT XLenTMin = xlen::XLenTMin


namespace riscv_isa {
    namespace operators {
        template<typename xlen=xlen_trait>
        struct EQ {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static bool op(XLenT a, XLenT b) { return a == b; }
        };

        template<typename xlen=xlen_trait>
        struct NE {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static bool op(XLenT a, XLenT b) { return a != b; }
        };

        template<typename xlen=xlen_trait>
        struct LT {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static bool op(XLenT a, XLenT b) { return a < b; }
        };

        template<typename xlen=xlen_trait>
        struct GE {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static bool op(XLenT a, XLenT b) { return a >= b; }
        };

        template<typename xlen=xlen_trait>
        struct LTU {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static bool op(UXLenT a, UXLenT b) { return a < b; }
        };

        template<typename xlen=xlen_trait>
        struct GEU {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static bool op(UXLenT a, UXLenT b) { return a >= b; }
        };

        template<typename xlen=xlen_trait>
        struct ADD {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(XLenT a, XLenT b) { return a + b; }
        };

        template<typename xlen=xlen_trait>
        struct SUB {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(XLenT a, XLenT b) { return a - b; }
        };

        template<typename xlen=xlen_trait>
        struct SLT {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(XLenT a, XLenT b) { return a < b; }
        };

        template<typename xlen=xlen_trait>
        struct SLTU {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(UXLenT a, UXLenT b) { return a < b; }
        };

        template<typename xlen=xlen_trait>
        struct XOR {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(UXLenT a, UXLenT b) { return a ^ b; }
        };

        template<typename xlen=xlen_trait>
        struct OR {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(UXLenT a, UXLenT b) { return a | b; }
        };

        template<typename xlen=xlen_trait>
        struct AND {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(UXLenT a, UXLenT b) { return a & b; }
        };

        template<typename xlen=xlen_trait>
        struct SLL {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(UXLenT a, UXLenT b) { return a << b; }
        };

        template<typename xlen=xlen_trait>
        struct SRL {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(UXLenT a, UXLenT b) { return a >> b; }
        };

        template<typename xlen=xlen_trait>
        struct SRA {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(XLenT a, UXLenT b) { return a >> b; }
        };

#if defined(__RV_EXTENSION_M__)

        template<typename xlen=xlen_trait>
        struct MUL {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(XLenT a, XLenT b) { return a * b; }
        };

        template<typename xlen=xlen_trait>
        struct MULH {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(XLenT a, XLenT b) {
                UXLenT as = a < 0;
                UXLenT au = (~as + 1) ^(a - as);
                UXLenT bs = b < 0;
                UXLenT bu = (~bs + 1) ^(b - bs);
                UXLenT a0 = au & HALF_MASK;
                UXLenT a1 = au >> HALF_WIDTH;
                UXLenT b0 = bu & HALF_MASK;
                UXLenT b1 = bu >> HALF_WIDTH;

                UXLenT c0 = a0 * b0;
                UXLenT c1 = a1 * b0;
                UXLenT c2 = a0 * b1;
                UXLenT c3 = a1 * b1;

                UXLenT d0 = (c0 >> HALF_WIDTH) + c1;
                UXLenT d1 = (d0 & HALF_MASK) + c2;
                UXLenT lh = (d1 << HALF_WIDTH) + (c0 & HALF_MASK);
                UXLenT hh = (d0 >> HALF_WIDTH) + (d1 >> HALF_WIDTH) + c3;

                UXLenT s = as ^bs;
                UXLenT lhs = lh - s;
                UXLenT hhs = -s ^(hh - (lhs > lh));

                return hhs;
            }
        };

        template<typename xlen=xlen_trait>
        struct MULHSU {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(XLenT a, UXLenT b) {
                UXLenT as = a < 0;
                UXLenT au = (~as + 1) ^(a - as);
                UXLenT a0 = au & HALF_MASK;
                UXLenT a1 = au >> HALF_WIDTH;
                UXLenT b0 = b & HALF_MASK;
                UXLenT b1 = b >> HALF_WIDTH;

                UXLenT c0 = a0 * b0;
                UXLenT c1 = a1 * b0;
                UXLenT c2 = a0 * b1;
                UXLenT c3 = a1 * b1;

                UXLenT d0 = (c0 >> HALF_WIDTH) + c1;
                UXLenT d1 = (d0 & HALF_MASK) + c2;
                UXLenT lh = (d1 << HALF_WIDTH) + (c0 & HALF_MASK);
                UXLenT hh = (d0 >> HALF_WIDTH) + (d1 >> HALF_WIDTH) + c3;

                UXLenT s = as;
                UXLenT lhs = lh - s;
                UXLenT hhs = -s ^(hh - (lhs > lh));

                return hhs;
            }
        };

        template<typename xlen=xlen_trait>
        struct MULHU {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(UXLenT a, UXLenT b) {
                UXLenT a0 = a & HALF_MASK;
                UXLenT a1 = a >> HALF_WIDTH;
                UXLenT b0 = b & HALF_MASK;
                UXLenT b1 = b >> HALF_WIDTH;

                UXLenT c0 = a0 * b0;
                UXLenT c1 = a1 * b0;
                UXLenT c2 = a0 * b1;
                UXLenT c3 = a1 * b1;

                UXLenT d0 = (c0 >> HALF_WIDTH) + c1;
                UXLenT d1 = (d0 & HALF_MASK) + c2;
                UXLenT hh = (d0 >> HALF_WIDTH) + (d1 >> HALF_WIDTH) + c3;

                return hh;
            }
        };

        template<typename xlen=xlen_trait>
        struct DIV {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(XLenT a, XLenT b) {
                if (b == 0) return -1;
                else if (a == XLenTMin && b == -1) return XLenTMin;
                return a / b;
            }
        };

        template<typename xlen=xlen_trait>
        struct DIVU {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(UXLenT a, UXLenT b) { return b == 0 ? -1 : a / b; }
        };

        template<typename xlen=xlen_trait>
        struct REM {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(XLenT a, XLenT b) {
                if (b == 0) return a;
                else if (a == XLenTMin && b == -1) return 0;
                return a % b;
            }
        };

        template<typename xlen=xlen_trait>
        struct REMU {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(UXLenT a, UXLenT b) { return b == 0 ? a : a % b; }
        };

#endif // defined(__RV_EXTENSION_M__)
    }
}

#undef _riscv_isa_use_all_xlen


#endif //RISCV_ISA_OPERATORS_HPP
