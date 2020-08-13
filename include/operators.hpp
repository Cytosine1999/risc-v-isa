#ifndef RISCV_ISA_OPERATORS_HPP
#define RISCV_ISA_OPERATORS_HPP


#include "riscv_isa_utility.hpp"


#define _riscv_isa_use_all_xlen(xlen) \
    using XLenT = typename xlen::XLenT; \
    using UXLenT = typename xlen::UXLenT; \
    using WLenT = typename xlen::WLenT; \
    using UWLenT = typename xlen::UWLenT; \
    static constexpr usize XLEN = xlen::XLEN; \
    static constexpr usize HALF_WIDTH = xlen::HALF_WIDTH; \
    static constexpr UXLenT HALF_MASK = xlen::HALF_MASK; \
    static constexpr XLenT XLenTMin = xlen::XLenTMin


namespace riscv_isa {
    namespace operators {
        template<typename xlen=xlen_trait>
        struct SELECT1 {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static bool op(XLenT a, riscv_isa_unused XLenT b) { return a; }
        };

        template<typename xlen=xlen_trait>
        struct SELECT2 {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static bool op(riscv_isa_unused XLenT a, XLenT b) { return b; }
        };

        template<typename xlen=xlen_trait>
        struct MIN {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static bool op(XLenT a, XLenT b) { return std::min(a, b); }
        };

        template<typename xlen=xlen_trait>
        struct MAX {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static bool op(XLenT a, XLenT b) { return std::max(a, b); }
        };

        template<typename xlen=xlen_trait>
        struct MINU {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static bool op(UXLenT a, UXLenT b) { return std::min(a, b); }
        };

        template<typename xlen=xlen_trait>
        struct MAXU {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static bool op(UXLenT a, UXLenT b) { return std::max(a, b); }
        };

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
            static XLenT op(XLenT a, XLenT b) { return (static_cast<WLenT>(a) * b) >> XLEN; }
        };

        template<typename xlen=xlen_trait>
        struct MULHSU {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(XLenT a, UXLenT b) { return (static_cast<WLenT>(a) * b) >> XLEN; }
        };

        template<typename xlen=xlen_trait>
        struct MULHU {
        private:
            _riscv_isa_use_all_xlen(xlen);

        public:
            static XLenT op(UXLenT a, UXLenT b) { return (static_cast<UWLenT>(a) * b) >> XLEN; }
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
