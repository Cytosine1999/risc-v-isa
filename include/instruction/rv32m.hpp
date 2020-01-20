#ifndef RISC_V_ISA_RV32M_HPP
#define RISC_V_ISA_RV32M_HPP


#include "instruction.hpp"


#if defined(__RV32M__) || defined(__RV64M__)
namespace risc_v_isa {
    namespace {
        constexpr usize HALF_WIDTH = XLEN_BYTE / 2;
        constexpr UXLenT HALF_MASK = BITS_MASK<UXLenT, HALF_WIDTH, 0>;
        constexpr XLenT XLenTMin = (XLenT) (1u << (XLEN_BYTE - 1));


        struct MUL { static XLenT op(XLenT a, XLenT b) { return a * b; } };

        struct MULH {
            static XLenT op(XLenT a, XLenT b) {
                UXLenT as = a < 0;
                UXLenT au = (~as + 1) ^ (a - as);
                UXLenT bs = b < 0;
                UXLenT bu = (~bs + 1) ^ (b - bs);
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

                UXLenT s = as ^ bs;
                UXLenT lhs = lh - s;
                UXLenT hhs = -s ^ (hh - (lhs > lh));

                return hhs;
            }
        };

        struct MULHSU {
            static XLenT op(XLenT a, UXLenT b) {
                UXLenT as = a < 0;
                UXLenT au = (~as + 1) ^ (a - as);
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
                UXLenT hhs = -s ^ (hh - (lhs > lh));

                return hhs;
            }
        };

        struct MULHU {
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

        struct DIV {
            static XLenT op(XLenT a, XLenT b) {
                if (b == 0) return -1;
                else if (a == XLenTMin && b == -1) return XLenTMin;
                return a / b;
            }
        };

        struct DIVU { static XLenT op(UXLenT a, UXLenT b) { return b == 0 ? -1 : a / b; } };

        struct REM {
            static XLenT op(XLenT a, XLenT b) {
                if (b == 0) return a;
                else if (a == XLenTMin && b == -1) return 0;
                return a % b;
            }
        };

        struct REMU { static XLenT op(UXLenT a, UXLenT b) { return b == 0 ? a : a % b; } };
    }

    class InstructionMulDivSet : public InstructionArithRegSet {
    public:
        static constexpr InnerT FUNC_7 = 0b0000001;
    };

    class MULInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b000;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<MUL>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const MULInst &inst) {
            stream << "\tmul\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class MULHInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b001;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<MULH>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const MULHInst &inst) {
            stream << "\tmulh\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class MULHSUInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b010;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<MULHSU>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const MULHSUInst &inst) {
            stream << "\tmulhsu\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class MULHUInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b011;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<MULHU>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const MULHUInst &inst) {
            stream << "\tmulhu\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class DIVInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b100;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<DIV>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const DIVInst &inst) {
            stream << "\tdiv\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class DIVUInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b101;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<DIVU>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const DIVUInst &inst) {
            stream << "\tdivu\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class REMInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b110;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<REM>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const REMInst &inst) {
            stream << "\trem\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };

    class REMUInst : public InstructionMulDivSet {
    public:
        static constexpr InnerT FUNCT_3 = 0b111;

        template <typename RegT>
        void operator()(RegT &reg) { operate_on<REMU>(reg); }

        friend std::ostream &operator<<(std::ostream &stream, const REMUInst &inst) {
            stream << "\tremu\tx" << inst.get_rd() << ", x" << inst.get_rs1() << ", x" << inst.get_rs2();
            return stream;
        }
    };
}
#endif // defined(__RV32M__) || defined(__RV64M__)


#endif //RISC_V_ISA_RV32M_HPP
