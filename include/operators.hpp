#ifndef RISCV_ISA_OPERATORS_HPP
#define RISCV_ISA_OPERATORS_HPP


#include <atomic>

#include "riscv_isa_utility.hpp"


#define _riscv_isa_use_all_xlen(xlen) \
    using XLenT = typename xlen::XLenT; \
    using UXLenT = typename xlen::UXLenT; \
    using WLenT = typename xlen::WLenT; \
    using UWLenT = typename xlen::UWLenT; \
    static constexpr usize XLEN = xlen::XLEN; \
    static constexpr usize HALF_WIDTH = xlen::HALF_WIDTH; \
    static constexpr UXLenT HALF_MASK = xlen::HALF_MASK; \
    static constexpr XLenT XLEN_MIN = xlen::XLEN_MIN


namespace riscv_isa {
    namespace operators {
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
                else if (a == XLEN_MIN && b == -1) return XLEN_MIN;
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
                else if (a == XLEN_MIN && b == -1) return 0;
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
#if defined(__RV_EXTENSION_A__)
        // todo: memory order

        template<typename xlen>
        struct AMOSWAP {
        public:
            _riscv_isa_use_all_xlen(xlen);

            static XLenT op(std::atomic<XLenT> *a, XLenT b) { return a->exchange(b); }
        };

        template<typename xlen>
        struct AMOADD {
        public:
            _riscv_isa_use_all_xlen(xlen);

            static XLenT op(std::atomic<XLenT> *a, XLenT b) { return a->fetch_add(b); }
        };

        template<typename xlen>
        struct AMOAND {
        public:
            _riscv_isa_use_all_xlen(xlen);

            static XLenT op(std::atomic<XLenT> *a, XLenT b) { return a->fetch_and(b); }
        };

        template<typename xlen>
        struct AMOOR {
        public:
            _riscv_isa_use_all_xlen(xlen);

            static XLenT op(std::atomic<XLenT> *a, XLenT b) { return a->fetch_or(b); }
        };

        template<typename xlen>
        struct AMOXOR {
        public:
            _riscv_isa_use_all_xlen(xlen);

            static XLenT op(std::atomic<XLenT> *a, XLenT b) { return a->fetch_xor(b); }
        };

        template<typename xlen>
        struct AMOMIN {
        public:
            _riscv_isa_use_all_xlen(xlen);

            static XLenT op(std::atomic<XLenT> *a, XLenT b) {
                XLenT ret = a->load();

                while(ret > b && !a->compare_exchange_weak(ret, b)) {}

                return ret;
            }
        };

        template<typename xlen>
        struct AMOMAX {
        public:
            _riscv_isa_use_all_xlen(xlen);

            static XLenT op(std::atomic<XLenT> *a, XLenT b) {
                XLenT ret = a->load();

                while(ret < b && !a->compare_exchange_weak(ret, b)) {}

                return ret;
            }
        };

        template<typename xlen>
        struct AMOMINU {
        public:
            _riscv_isa_use_all_xlen(xlen);

            static XLenT op(std::atomic<XLenT> *a, UXLenT b) {
                XLenT ret = a->load();

                while(static_cast<UXLenT>(ret) > b && !a->compare_exchange_weak(ret, b)) {}

                return ret;
            }
        };

        template<typename xlen>
        struct AMOMAXU {
        public:
            _riscv_isa_use_all_xlen(xlen);

            static XLenT op(std::atomic<XLenT> *a, UXLenT b) {
                XLenT ret = a->load();

                while(static_cast<UXLenT>(ret) < b && !a->compare_exchange_weak(ret, b)) {}

                return ret;
            }
        };
#endif
    }
}

#undef _riscv_isa_use_all_xlen


#endif //RISCV_ISA_OPERATORS_HPP
