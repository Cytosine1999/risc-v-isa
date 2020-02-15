#ifndef RISCV_ISA_DUMP_HPP
#define RISCV_ISA_DUMP_HPP


#include <ostream>

#include "utility.hpp"
#include "instruction/instruction_visitor.hpp"


namespace riscv_isa {
    class Dump : public InstructionVisitor<Dump, usize> {
    public:
        using RetT = usize;

    private:
        template<typename InstT>
        RetT dump_inst(InstT *inst) {
            stream << *inst;
            return InstT::INST_WIDTH;
        }

    public:
        std::ostream &stream;

        explicit Dump(std::ostream &stream) : stream{stream} {}

        RetT illegal_instruction(riscv_isa_unused Instruction *inst) {
            stream << "[unknown instruction]";
            return 0;
        }

#define _riscv_isa_dump_instruction(NAME, name) \
        RetT visit_##name##_inst(NAME##Inst *inst) { return dump_inst(inst); }

        riscv_isa_instruction_map(_riscv_isa_dump_instruction)
    };

    std::ostream &operator<<(std::ostream &stream, const Instruction &inst) {
        Dump dump{stream};
        dump.visit(const_cast<Instruction *>(&inst));
        return stream;
    }
}


#endif //RISCV_ISA_DUMP_HPP
