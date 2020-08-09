#ifndef RISCV_ISA_DUMP_HPP
#define RISCV_ISA_DUMP_HPP


#include <cstring>
#include <ostream>

#include "riscv_isa_utility.hpp"
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
        ILenT inst_buffer;

        explicit Dump(std::ostream &stream) : stream{stream}, inst_buffer{0} {}

        RetT visit(Instruction *inst, usize length) {
            inst_buffer = 0;

            if (length < 2) stream << "[instruction exceed length limit]";
            memcpy(reinterpret_cast<u8 *>(&inst_buffer) + 0, reinterpret_cast<u8 *>(inst) + 0, 2);

            if ((this->inst_buffer & bits_mask<u16, 2, 0>::val) != bits_mask<u16, 2, 0>::val) {
#if defined(__RV_EXTENSION_C__)
                return this->visit_16(reinterpret_cast<Instruction16 *>(&this->inst_buffer));
#else
                return illegal_instruction(reinterpret_cast<Instruction *>(&this->inst_buffer));
#endif // defined(__RV_EXTENSION_C__)
            } else if ((this->inst_buffer & bits_mask<u16, 5, 2>::val) != bits_mask<u16, 5, 2>::val) {
                if (length < 4) stream << "[instruction exceed length limit]";
                memcpy(reinterpret_cast<u8 *>(&inst_buffer) + 2, reinterpret_cast<u8 *>(inst) + 2, 2);
                return this->visit_32(reinterpret_cast<Instruction32 *>(&this->inst_buffer));
            } else {
                return illegal_instruction(reinterpret_cast<Instruction *>(&this->inst_buffer));
            }
        }

        RetT illegal_instruction(riscv_isa_unused Instruction *inst) {
            stream << "[unknown instruction]";
            return 0;
        }

#define _riscv_isa_dump_instruction(NAME, name) \
        RetT visit_##name##_inst(NAME##Inst *inst) { return dump_inst(inst); }
        riscv_isa_instruction_map(_riscv_isa_dump_instruction)
#undef _riscv_isa_dump_instruction
    };

    std::ostream &operator<<(std::ostream &stream, const Instruction &inst) {
        Dump dump{stream};
        dump.visit(const_cast<Instruction *>(&inst), RISCV_ILEN / 8);
        return stream;
    }
}


#endif //RISCV_ISA_DUMP_HPP
