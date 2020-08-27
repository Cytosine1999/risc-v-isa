#ifndef NEUTRON_JIT_COMPILER_HPP
#define NEUTRON_JIT_COMPILER_HPP


#include "instruction/instruction_visitor.hpp"


namespace riscv_isa {
    template<typename SubT>
    class _JITCompiler : public InstructionVisitor<SubT, bool> {
    private:
        SubT *sub_type() { return static_cast<SubT *>(this); }

    public:
        using SuperT = InstructionVisitor<SubT, bool>;

        using RetT = typename SuperT::RetT;

#define _riscv_isa_emit_instruction(NAME, name) \
        RetT visit_##name##_inst(const NAME##Inst *inst) { return sub_type()->emit_inst(inst); }

        riscv_isa_instruction_map(_riscv_isa_emit_instruction)

#undef _riscv_isa_emit_instruction
    };

    template<typename SubT>
    class JITCompiler : public _JITCompiler<SubT> {
    private:
        SubT *sub_type() { return static_cast<SubT *>(this); }

    public:
        using SuperT = _JITCompiler<SubT>;

        using RetT = typename SuperT::RetT;
        using XLenT = typename SuperT::XLenT;
        using UXLenT = typename SuperT::UXLenT;
        using IntRegT = typename SuperT::IntRegT;
        using CSRRegT = typename SuperT::CSRRegT;

        RetT illegal_instruction(riscv_isa_unused const Instruction *inst) { return false; }

    };
}


#endif //NEUTRON_JIT_COMPILER_HPP
