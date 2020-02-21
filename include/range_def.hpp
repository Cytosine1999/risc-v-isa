#ifndef RISCV_ISA_RAGE_DEF_HPP
#define RISCV_ISA_RAGE_DEF_HPP

#define _riscv_isa_range_0_16(func, ...) \
    func(__VA_ARGS__, 0) \
    func(__VA_ARGS__, 1) \
    func(__VA_ARGS__, 2) \
    func(__VA_ARGS__, 3) \
    func(__VA_ARGS__, 4) \
    func(__VA_ARGS__, 5) \
    func(__VA_ARGS__, 6) \
    func(__VA_ARGS__, 7) \
    func(__VA_ARGS__, 8) \
    func(__VA_ARGS__, 9) \
    func(__VA_ARGS__, 10) \
    func(__VA_ARGS__, 11) \
    func(__VA_ARGS__, 12) \
    func(__VA_ARGS__, 13) \
    func(__VA_ARGS__, 14) \
    func(__VA_ARGS__, 15)

#define _riscv_isa_range_3_32(func, ...) \
    func(__VA_ARGS__, 3) \
    func(__VA_ARGS__, 4) \
    func(__VA_ARGS__, 5) \
    func(__VA_ARGS__, 6) \
    func(__VA_ARGS__, 7) \
    func(__VA_ARGS__, 8) \
    func(__VA_ARGS__, 9) \
    func(__VA_ARGS__, 10) \
    func(__VA_ARGS__, 11) \
    func(__VA_ARGS__, 12) \
    func(__VA_ARGS__, 13) \
    func(__VA_ARGS__, 14) \
    func(__VA_ARGS__, 15) \
    func(__VA_ARGS__, 16) \
    func(__VA_ARGS__, 17) \
    func(__VA_ARGS__, 18) \
    func(__VA_ARGS__, 19) \
    func(__VA_ARGS__, 20) \
    func(__VA_ARGS__, 21) \
    func(__VA_ARGS__, 22) \
    func(__VA_ARGS__, 23) \
    func(__VA_ARGS__, 24) \
    func(__VA_ARGS__, 25) \
    func(__VA_ARGS__, 26) \
    func(__VA_ARGS__, 27) \
    func(__VA_ARGS__, 28) \
    func(__VA_ARGS__, 29) \
    func(__VA_ARGS__, 30) \
    func(__VA_ARGS__, 31)

#define _riscv_isa_range(begin, end, func, ...) \
    _riscv_isa_range_##begin##_##end(func, __VA_ARGS__)

#endif // RISCV_ISA_RAGE_DEF_HPP