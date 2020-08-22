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
        template<typename xlen>
        struct SELECT1 {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static bool op(XLenT a, riscv_isa_unused XLenT b) { return a; }
        };

        template<typename xlen>
        struct SELECT2 {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static bool op(riscv_isa_unused XLenT a, XLenT b) { return b; }
        };

        template<typename xlen>
        struct MIN {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static bool op(XLenT a, XLenT b) { return std::min(a, b); }
        };

        template<typename xlen>
        struct MAX {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static bool op(XLenT a, XLenT b) { return std::max(a, b); }
        };

        template<typename xlen>
        struct MINU {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static bool op(UXLenT a, UXLenT b) { return std::min(a, b); }
        };

        template<typename xlen>
        struct MAXU {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static bool op(UXLenT a, UXLenT b) { return std::max(a, b); }
        };

        template<typename xlen>
        struct EQ {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static bool op(XLenT a, XLenT b) { return a == b; }
        };

        template<typename xlen>
        struct NE {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static bool op(XLenT a, XLenT b) { return a != b; }
        };

        template<typename xlen>
        struct LT {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static bool op(XLenT a, XLenT b) { return a < b; }
        };

        template<typename xlen>
        struct GE {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static bool op(XLenT a, XLenT b) { return a >= b; }
        };

        template<typename xlen>
        struct LTU {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static bool op(UXLenT a, UXLenT b) { return a < b; }
        };

        template<typename xlen>
        struct GEU {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static bool op(UXLenT a, UXLenT b) { return a >= b; }
        };

        template<typename xlen>
        struct ADD {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(XLenT a, XLenT b) { return a + b; }
        };

        template<typename xlen>
        struct SUB {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(XLenT a, XLenT b) { return a - b; }
        };

        template<typename xlen>
        struct SLT {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(XLenT a, XLenT b) { return a < b; }
        };

        template<typename xlen>
        struct SLTU {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(UXLenT a, UXLenT b) { return a < b; }
        };

        template<typename xlen>
        struct XOR {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(UXLenT a, UXLenT b) { return a ^ b; }
        };

        template<typename xlen>
        struct OR {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(UXLenT a, UXLenT b) { return a | b; }
        };

        template<typename xlen>
        struct AND {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(UXLenT a, UXLenT b) { return a & b; }
        };

        template<typename xlen>
        struct SLL {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(UXLenT a, UXLenT b) { return a << b; }
        };

        template<typename xlen>
        struct SRL {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(UXLenT a, UXLenT b) { return a >> b; }
        };

        template<typename xlen>
        struct SRA {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(XLenT a, UXLenT b) { return a >> b; }
        };

#if defined(__RV_EXTENSION_M__)

        template<typename xlen>
        struct MUL {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(XLenT a, XLenT b) { return a * b; }
        };

        template<typename xlen>
        struct MULH {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(XLenT a, XLenT b) { return (static_cast<WLenT>(a) * b) >> XLEN; }
        };

        template<typename xlen>
        struct MULHSU {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(XLenT a, UXLenT b) { return (static_cast<WLenT>(a) * b) >> XLEN; }
        };

        template<typename xlen>
        struct MULHU {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(UXLenT a, UXLenT b) { return (static_cast<UWLenT>(a) * b) >> XLEN; }
        };

        template<typename xlen>
        struct DIV {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(XLenT a, XLenT b) {
                if (b == 0) return -1;
                else if (a == XLenTMin && b == -1) return XLenTMin;
                return a / b;
            }
        };

        template<typename xlen>
        struct DIVU {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(UXLenT a, UXLenT b) { return b == 0 ? -1 : a / b; }
        };

        template<typename xlen>
        struct REM {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(XLenT a, XLenT b) {
                if (b == 0) return a;
                else if (a == XLenTMin && b == -1) return 0;
                return a % b;
            }
        };

        template<typename xlen>
        struct REMU {
        public:
            _riscv_isa_use_all_xlen(xlen);
            
            static XLenT op(UXLenT a, UXLenT b) { return b == 0 ? a : a % b; }
        };

#endif // defined(__RV_EXTENSION_M__)
    }
}

#undef _riscv_isa_use_all_xlen


#endif //RISCV_ISA_OPERATORS_HPP
