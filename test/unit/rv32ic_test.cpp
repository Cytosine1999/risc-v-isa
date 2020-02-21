#include "rv32i_test.hpp"
#include "rv32c_test.hpp"

using namespace riscv_isa;


int main() {
    check_invalid_16_op_code(0b00);
    check_invalid_16_op_code(0b01);
    check_invalid_16_op_code(0b10);

    check_instruction_32_op_00000();
    check_invalid_32_op_code(0b00001);
    check_invalid_32_op_code(0b00010);
    check_instruction_32_op_00011();
    check_instruction_32_op_00100();
    check_instruction_32_op_00101();
    check_invalid_32_op_code(0b00110);
    check_invalid_32_op_code(0b00111);

    check_instruction_32_op_01000();
    check_invalid_32_op_code(0b01001);
    check_invalid_32_op_code(0b01010);
    check_invalid_32_op_code(0b01011);
    check_instruction_32_op_01100();
    check_instruction_32_op_01101();
    check_invalid_32_op_code(0b01110);
    check_invalid_32_op_code(0b01111);

    check_invalid_32_op_code(0b10000);
    check_invalid_32_op_code(0b10001);
    check_invalid_32_op_code(0b10010);
    check_invalid_32_op_code(0b10011);
    check_invalid_32_op_code(0b10100);
    check_invalid_32_op_code(0b10101);
    check_invalid_32_op_code(0b10110);
    check_invalid_32_op_code(0b10111);

    check_instruction_32_op_11000();
    check_instruction_32_op_11001();
    check_invalid_32_op_code(0b11010);
    check_instruction_32_op_11011();
    check_instruction_32_op_11100();
    check_invalid_32_op_code(0b11101);
    check_invalid_32_op_code(0b11110);
    check_invalid_32_op_code(0b11111);
}
