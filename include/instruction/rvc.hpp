#ifndef NEUTRON_RVC_HPP
#define NEUTRON_RVC_HPP


#include "instruction.hpp"


#if defined(__RV_EXTENSION_C__)
namespace riscv_isa {
    class InstructionCR : public Instruction16 {
    public:
        static constexpr usize OP_CODE = 0b10;
        static constexpr usize FUNCT3 = 0b100;

        usize get_rs2() const { return slice_rs2(inner); }

        usize get_funct1() const { return slice_funct1(inner); }
    };

    class InstructionCI : public Instruction16 {};

    class InstructionCSS : public Instruction16 {
    public:
        usize get_rs2() const { return slice_rs2(inner); }
    };

    class InstructionCIW : public Instruction16 {};

    class InstructionCL : public Instruction16 {
    public:
        usize get_rdc() const { return slice_rdc_rs2c(inner); }

        usize get_rs1c() const { return slice_rdc_rs1c(inner); }
    };

    class InstructionCS : public Instruction16 {
    public:
        usize get_rs2c() const { return slice_rdc_rs2c(inner); }

        usize get_rs1c() const { return slice_rdc_rs1c(inner); }
    };

    class InstructionCA : public Instruction16 {
    public:
        static constexpr usize OP_CODE = 0b01;
        static constexpr usize FUNCT3 = 0b100;
        static constexpr usize FUNCT2 = 0b11;

        usize get_rs2c() const { return slice_rdc_rs2c(inner); }

        usize get_rd() const { return slice_rdc_rs1c(inner); }

        usize get_rs1() const { return slice_rdc_rs1c(inner); }

        usize get_funct2() const { return slice_funct2(inner); }

        usize get_funct1() const { return slice_funct1(inner); }

        usize get_funct_arith() const { return get_bits<UInnerT, 7, 5>(inner); }
    };

    class InstructionCB : public Instruction16 {
    protected:
        static InnerT slice_imm(InnerT val) {
            UInnerT imm_3_1 = get_bits<UInnerT, 5, 3, 1>(val);
            UInnerT imm_5_3 = get_bits<UInnerT, 12, 10, 3>(val);
            UInnerT imm_6_5 = get_bits<UInnerT, 3, 2, 5>(val);
            UInnerT imm_8_6 = get_bits<UInnerT, 7, 5, 6>(val);
            UInnerT imm_16_8 = get_bits<UInnerT, 16, 8, 8>((val << 3) >> 7);

            return imm_16_8 | imm_8_6 | imm_6_5 | imm_5_3 | imm_3_1;
        }

    public:
        static constexpr usize OP_CODE = 0b01;

        usize get_rs1c() const { return slice_rdc_rs1c(inner); }
    };

    class InstructionCJ : public Instruction16 {
    public:
        static constexpr usize OP_CODE = 0b01;

        InnerT get_imm() const {
            UInnerT imm_4_1 = get_bits<UInnerT, 6, 3, 1>(inner);
            UInnerT imm_5_4 = get_bits<UInnerT, 12, 11, 4>(inner);
            UInnerT imm_6_5 = get_bits<UInnerT, 3, 2, 5>(inner);
            UInnerT imm_8_7 = get_bits<UInnerT, 7, 6, 7>(inner);
            UInnerT imm_11_10 = get_bits<UInnerT, 9, 8, 10>(inner);
            UInnerT imm_combo = ((inner << 3) >> 4) & (bits_mask<UInnerT, 16, 11>::val |
                                                       bits_mask<UInnerT, 9, 8>::val | bits_mask<UInnerT, 7, 6>::val);

            return imm_combo | imm_11_10 | imm_8_7 | imm_6_5 | imm_5_4 | imm_4_1;
        }
    };

    class StackPointerLoadSet : public InstructionCI {
    protected:
        template<usize align>
        static UInnerT slice_imm(UInnerT val) {
            UInnerT imm_5_x = get_bits<UInnerT, 7, 2 + align, align>(val);
            UInnerT imm_6_5 = get_bits<UInnerT, 13, 12, 5>(val);
            UInnerT imm_x_6 = get_bits<UInnerT, 2 + align, 2, 6>(val);
            return imm_x_6 | imm_6_5 | imm_5_x;
        }

    public:
        static constexpr usize OP_CODE = 0b10;

        usize get_rd() const { return slice_rd_rs1(inner); }
    };

    class StackPointerStoreSet : public InstructionCSS {
    protected:
        template<usize align>
        static UInnerT slice_imm(UInnerT val) {
            UInnerT imm_6_x = get_bits<UInnerT, 13, 7 + align, align>(val);
            UInnerT imm_x_6 = get_bits<UInnerT, 7 + align, 7, 6>(val);
            return imm_x_6 | imm_6_x;
        }

    public:
        static constexpr usize OP_CODE = 0b10;
    };

    class RegisterLoadSet : public InstructionCL {
    public:
        static constexpr usize OP_CODE = 0b00;
    };

    class RegisterStoreSet : public InstructionCS {
    public:
        static constexpr usize OP_CODE = 0b00;
    };

#if __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128

    class CLDSPInst : public StackPointerLoadSet {
    public:
        static constexpr usize FUNCT3 = 0b011;

        UInnerT get_imm() const { return slice_imm<3>(inner); }
    };

    class CSDSPInst : public StackPointerStoreSet {
    public:
        static constexpr usize FUNCT3 = 0b111;

        UInnerT get_imm() const { return slice_imm<3>(inner); }
    };

    class CLDInst : public RegisterLoadSet {
    public:
        static constexpr usize FUNCT3 = 0b011;

        UInnerT get_imm() const { return slice_imm_d(inner); }
    };

    class CSDInst : public RegisterStoreSet {
    public:
        static constexpr usize FUNCT3 = 0b111;

        UInnerT get_imm() const { return slice_imm_d(inner); }
    };

#endif // __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128
#if __RV_BIT_WIDTH__ == 128

    class CLQSPInst : public StackPointerLoadSet {
    public:
        static constexpr usize FUNCT3 = 0b001;

        UInnerT get_imm() const { return slice_imm<4>(inner); }
    };

    class CSQSPInst : public StackPointerStoreSet {
    public:
        static constexpr usize FUNCT3 = 0b101;

        UInnerT get_imm() const { return slice_imm<4>(inner); }
    };

    class CLQInst : public RegisterLoadSet {
    public:
        static constexpr usize FUNCT3 = 0b001;

        UInnerT get_imm() const { return slice_imm_q(inner); }
    };

    class CSQInst : public RegisterStoreSet {
    public:
        static constexpr usize FUNCT3 = 0b101;

        UInnerT get_imm() const { return slice_imm_q(inner); }
    };

#endif // __RV_BIT_WIDTH__ == 128
#if defined(__RV_EXTENSION_F__)

    class CFLWSPInst : public StackPointerLoadSet {
    public:
        static constexpr usize FUNCT3 = 0b011;

        UInnerT get_imm() const { return slice_imm<2>(inner); }
    };

    class CFSWSPInst : public StackPointerStoreSet {
    public:
        static constexpr usize FUNCT3 = 0b111;

        UInnerT get_imm() const { return slice_imm<2>(inner); }
    };

        class CFLWInst : public RegisterLoadSet {
    public:
        static constexpr usize FUNCT3 = 0b011;

        UInnerT get_imm() const { return slice_imm_w(inner); }
    };

    class CFSWInst : public RegisterStoreSet {
    public:
        static constexpr usize FUNCT3 = 0b111;

        UInnerT get_imm() const { return slice_imm_w(inner); }
    };

#if defined(__RV_EXTENSION_D__)

    class CFLDSPInst : public StackPointerLoadSet {
    public:
        static constexpr usize FUNCT3 = 0b001;

        UInnerT get_imm() const { return slice_imm<3>(inner); }
    };

    class CFSDSPInst : public StackPointerStoreSet {
    public:
        static constexpr usize FUNCT3 = 0b101;

        UInnerT get_imm() const { return slice_imm<3>(inner); }
    };

    class CFLDInst : public RegisterLoadSet {
    public:
        static constexpr usize FUNCT3 = 0b001;

        UInnerT get_imm() const { return slice_imm_d(inner); }
    };

    class CFSDInst : public RegisterStoreSet {
    public:
        static constexpr usize FUNCT3 = 0b101;

        UInnerT get_imm() const { return slice_imm_d(inner); }
    };

#endif // defined(__RV_EXTENSION_D__)
#endif // defined(__RV_EXTENSION_F__)

    class CLWSPInst : public StackPointerLoadSet {
    public:
        static constexpr usize FUNCT3 = 0b010;

        u32 get_imm() const { return slice_imm<2>(inner); }
    };

    class CSWSPInst : public StackPointerStoreSet {
    public:
        static constexpr usize FUNCT3 = 0b110;

        UInnerT get_imm() const { return slice_imm<2>(inner); }
    };

    class CLWInst : public RegisterLoadSet {
    public:
        static constexpr usize FUNCT3 = 0b010;

        UInnerT get_imm() const { return slice_imm_w(inner); }
    };

    class CSWInst : public RegisterStoreSet {
    public:
        static constexpr usize FUNCT3 = 0b110;

        UInnerT get_imm() const { return slice_imm_w(inner); }
    };

    class CJInst : public InstructionCJ {
    public:
        static constexpr usize FUNCT3 = 0b101;
    };

    class CJALInst : public InstructionCJ {
    public:
        static constexpr usize FUNCT3 = 0b001;
    };

    class CJRInst : public InstructionCR {
    public:
        static constexpr usize FUNCT1 = 0b0;

        usize get_rs1() const { return slice_rd_rs1(inner); }
    };

    class CJALRInst : public InstructionCR {
    public:
        static constexpr usize FUNCT1 = 0b1;

        usize get_rs1() const { return slice_rd_rs1(inner); }
    };

    class CBEQZInst : public InstructionCB {
    public:
        static constexpr usize FUNCT3 = 0b110;

        usize get_rs1() const { return slice_rdc_rs1c(inner); }

        InnerT get_imm() const { return slice_imm(inner); }
    };

    class CBNEZInst : public InstructionCB {
    public:
        static constexpr usize FUNCT3 = 0b111;

        usize get_rs1() const { return slice_rdc_rs1c(inner); }

        InnerT get_imm() const { return slice_imm(inner); }
    };

    class CLIInst : public InstructionCI {
    public:
        static constexpr usize OP_CODE = 0b01;
        static constexpr usize FUNCT3 = 0b010;

        usize get_rd() const { return slice_rd_rs1(inner); }

        InnerT get_imm() const { return slice_imm_low(inner); }
    };

    class CLUIInst : public InstructionCI {
    public:
        static constexpr usize OP_CODE = 0b01;
        static constexpr usize FUNCT3 = 0b011;

        usize get_rd() const { return slice_rd_rs1(inner); }

        i32 get_imm() const {
            u32 imm_17_12 = get_bits<u32, 7, 2, 12>(inner);
            i32 imm_16_17 = get_bits<u32, 32, 17, 17>(static_cast<i32>(inner << 3) << 2);

            return imm_16_17 | imm_17_12;
        }
    };

    class CADDIInst : public InstructionCI {
    public:
        static constexpr usize OP_CODE = 0b01;
        static constexpr usize FUNCT3 = 0b000;

        usize get_rd() const { return slice_rd_rs1(inner); }

        usize get_rs1() const { return slice_rd_rs1(inner); }

        InnerT get_imm() const { return slice_imm_low(inner); }
    };

#if __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128

    class CADDIWInst : public InstructionCI {
    public:
        static constexpr usize OP_CODE = 0b01;
        static constexpr usize FUNCT3 = 0b001;

        usize get_rd() const { return slice_rd_rs1(inner); }

        usize get_rs1() const { return slice_rd_rs1(inner); }

        InnerT get_imm() const { return slice_imm_low(inner); }
    };

#endif // __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128

    class CADDI16SPInst : public InstructionCI {
    public:
        static constexpr usize OP_CODE = 0b01;
        static constexpr usize FUNCT3 = 0b011;

        usize get_rd() const { return slice_rd_rs1(inner); }

        usize get_rs1() const { return slice_rd_rs1(inner); }

        InnerT get_imm() const {
            UInnerT imm_5_4 = get_bits<UInnerT, 7, 6, 4>(inner);
            UInnerT imm_6_5 = get_bits<UInnerT, 3, 2, 5>(inner);
            UInnerT imm_7_6 = get_bits<UInnerT, 6, 5, 6>(inner);
            UInnerT imm_9_7 = get_bits<UInnerT, 5, 3, 7>(inner);
            UInnerT imm_16_9 = get_bits<UInnerT, 16, 5, 5>((inner << 3) >> 6);

            return imm_16_9 | imm_9_7 | imm_7_6 | imm_6_5 | imm_5_4;
        }
    };

    class CADDI4SPNInst : public InstructionCIW {
    public:
        static constexpr usize OP_CODE = 0b00;
        static constexpr usize FUNCT3 = 0b000;

        usize get_rd() const { return slice_rdc_rs2c(inner); }

        usize get_imm() const {
            UInnerT imm_3_2 = get_bits<UInnerT, 7, 6, 2>(inner);
            UInnerT imm_4_3 = get_bits<UInnerT, 6, 5, 3>(inner);
            UInnerT imm_6_4 = get_bits<UInnerT, 13, 11, 4>(inner);
            UInnerT imm_9_6 = get_bits<UInnerT, 11, 7, 6>(inner);

            return imm_9_6 | imm_6_4 | imm_4_3 | imm_3_2;
        }
    };

    class CSLLIInst : public InstructionCI {
    public:
        static constexpr usize OP_CODE = 0b10;
        static constexpr usize FUNCT3 = 0b000;

        usize get_rd() const { return slice_rd_rs1(inner); }

        usize get_rs1() const { return slice_rd_rs1(inner); }

        UInnerT get_shamt() const { return slice_shamt(inner); }
    };

    class CSRLIInst : public InstructionCB {
    public:
        static constexpr usize FUNCT3 = 0b100;
        static constexpr usize FUNCT2 = 0b00;

        usize get_rd() const { return slice_rdc_rs1c(inner); }

        usize get_rs1() const { return slice_rdc_rs1c(inner); }

        usize get_funct2() const { return slice_funct2(inner); }

        UInnerT get_shamt() const { return slice_shamt(inner); }
    };

    class CSRAIInst : public InstructionCB {
    public:
        static constexpr usize FUNCT3 = 0b100;
        static constexpr usize FUNCT2 = 0b01;

        usize get_rd() const { return slice_rdc_rs1c(inner); }

        usize get_rs1() const { return slice_rdc_rs1c(inner); }

        usize get_funct2() const { return slice_funct2(inner); }

        UInnerT get_shamt() const { return slice_shamt(inner); }
    };

    class CANDIInst : public InstructionCB {
    public:
        static constexpr usize FUNCT3 = 0b100;
        static constexpr usize FUNCT2 = 0b10;

        usize get_rd() const { return slice_rdc_rs1c(inner); }

        usize get_rs1() const { return slice_rdc_rs1c(inner); }

        usize get_funct2() const { return slice_funct2(inner); }

        InnerT get_imm() const { return slice_imm_low(inner); }
    };

    class CMVInst : public InstructionCR {
    public:
        static constexpr usize FUNCT1 = 0b0;

        usize get_rd() const { return slice_rd_rs1(inner); }

        usize get_rs1() const { return slice_rd_rs1(inner); }
    };

    class CADDInst : public InstructionCR {
    public:
        static constexpr usize FUNCT1 = 0b1;

        usize get_rd() const { return slice_rd_rs1(inner); }

        usize get_rs1() const { return slice_rd_rs1(inner); }
    };

    class CANDInst : public InstructionCA {
    public:
        static constexpr usize FUNCT_ARITH = 0b11;
        static constexpr usize FUNCT1 = 0b0;
    };

    class CORInst : public InstructionCA {
    public:
        static constexpr usize FUNCT_ARITH = 0b10;
        static constexpr usize FUNCT1 = 0b0;
    };

    class CXORInst : public InstructionCA {
    public:
        static constexpr usize FUNCT_ARITH = 0b01;
        static constexpr usize FUNCT1 = 0b0;
    };

    class CSUBInst : public InstructionCA {
    public:
        static constexpr usize FUNCT_ARITH = 0b00;
        static constexpr usize FUNCT1 = 0b0;
    };

#if __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128

    class CADDWInst : public InstructionCA {
    public:
        static constexpr usize FUNCT_ARITH = 0b01;
        static constexpr usize FUNCT1 = 0b1;
    };

    class CSUBWInst : public InstructionCA {
    public:
        static constexpr usize FUNCT_ARITH = 0b00;
        static constexpr usize FUNCT1 = 0b1;
    };

#endif // __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128

    class CEBREAKInst : public InstructionCR {
    public:
        static constexpr usize FUNCT1 = 0b1;
    };
}


#if __RV_BIT_WIDTH__ == 32 || __RV_BIT_WIDTH__ == 64
#if defined(__RV_EXTENSION_D__)
#define riscv_isa_instruction_cfld_clq_map(func) \
    func(CFLD, cfld)
#define riscv_isa_instruction_cfsd_csq_map(func) \
    func(CFSD, cfsd)
#define riscv_isa_instruction_cfldsp_clqsp_map(func) \
    func(CFLDSP, cfldp)
#define riscv_isa_instruction_cfsdsp_csqsp_map(func) \
    func(CFSDSP, cfsdsp)
#else
#define riscv_isa_instruction_cfld_clq_map(func)
#define riscv_isa_instruction_cfsd_csq_map(func)
#define riscv_isa_instruction_cfldsp_clqsp_map(func)
#define riscv_isa_instruction_cfsdsp_csqsp_map(func)
#endif // defined(__RV_EXTENSION_D__)
#elif __RV_BIT_WIDTH__ == 128
#define riscv_isa_instruction_cfld_clq_map(func) \
    func(CLQ, clq)
#define riscv_isa_instruction_cfsd_csq_map(func) \
    func(CSQ, csq)
#define riscv_isa_instruction_cfldsp_clqsp_map(func) \
    func(CLQSP, clqsp)
#define riscv_isa_instruction_cfsdsp_csqsp_map(func) \
    func(CSQSP, csqsp)
#endif

#if __RV_BIT_WIDTH__ == 32
#if defined(__RV_EXTENSION_F__)
#define riscv_isa_instruction_cflw_cld_map(func) \
    func(CFLW, cflw)
#define riscv_isa_instruction_cfsw_csd_map(func) \
    func(CFSW, cfsw)
#define riscv_isa_instruction_cflwsp_cldsp_map(func) \
    func(CFLWSP, cflwsp)
#define riscv_isa_instruction_cfswsp_csdsp_map(func) \
    func(CFSWSP, cfswsp)
#else
#define riscv_isa_instruction_cflw_cld_map(func)
#define riscv_isa_instruction_cfsw_csd_map(func)
#define riscv_isa_instruction_cflwsp_cldsp_map(func)
#define riscv_isa_instruction_cfswsp_csdsp_map(func)
#endif // defined(__RV_EXTENSION_F__)
#elif __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128
#define riscv_isa_instruction_cflw_cld_map(func) \
    func(CLD, cld)
#define riscv_isa_instruction_cfsw_csd_map(func) \
    func(CLD, cld)
#define riscv_isa_instruction_cflwsp_cldsp_map(func) \
    func(CLDSP, cldsp)
#define riscv_isa_instruction_cfswsp_csdsp_map(func) \
    func(CSDSP, csdsp)
#endif

#if __RV_BIT_WIDTH__ == 32
#define riscv_isa_instruction_cjal_caddiw_map(func) \
    func(CJAL, cjal)
#elif __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128
#define riscv_isa_instruction_cjal_caddiw_map(func) \
    func(CADDIW, caddiw)
#endif

#if __RV_BIT_WIDTH__ == 64 || __RV_BIT_WIDTH__ == 128
#define riscv_isa_instruction_arith_w_map(func) \
        func(CSUBW, csubw) \
        func(CADDW, caddw)
#else
#define riscv_isa_instruction_arith_w_map(func)
#endif

#define riscv_isa_instruction_c_map(func) \
        func(CADDI4SPN, caddi4spn) \
        riscv_isa_instruction_cfld_clq_map(func) \
        func(CLW, clw) \
        riscv_isa_instruction_cflw_cld_map(func) \
        riscv_isa_instruction_cfsd_csq_map(func) \
        func(CSW, csw) \
        riscv_isa_instruction_cfsw_csd_map(func) \
        func(CADDI, caddi) \
        riscv_isa_instruction_cjal_caddiw_map(func) \
        func(CLI, cli) \
        func(CADDI16SP, caddi16sp) \
        func(CLUI, clui) \
        func(CSRLI, csrli) \
        func(CSRAI, csrai) \
        func(CANDI, candi) \
        func(CSUB, csub) \
        func(CXOR, cxor) \
        func(COR, cor) \
        func(CAND, cand) \
        riscv_isa_instruction_arith_w_map(func) \
        func(CJ, cj) \
        func(CBEQZ, cbeqz) \
        func(CBNEZ, cbnez) \
        func(CSLLI, cslli) \
        riscv_isa_instruction_cfldsp_clqsp_map(func) \
        func(CLWSP, clwsp) \
        riscv_isa_instruction_cflwsp_cldsp_map(func) \
        func(CJR, cjr) \
        func(CMV, cmv) \
        func(CEBREAK, cebreak) \
        func(CJALR, cjalr) \
        func(CADD, cadd) \
        riscv_isa_instruction_cfsdsp_csqsp_map(func) \
        func(CSWSP, cswsp) \
        riscv_isa_instruction_cfswsp_csdsp_map(func)
#else
#define riscv_isa_instruction_c_map(func)
#endif // defined(__RV_EXTENSION_C__)


#endif //NEUTRON_RVC_HPP
