#ifndef RISCV_ISA_TEST_HPP
#define RISCV_ISA_TEST_HPP


#include <iostream>
#include "riscv_isa_utility.hpp"


template<typename T>
riscv_isa_static_inline void _assert(T expr, riscv_isa_unused const char *file, riscv_isa_unused int line,
             riscv_isa_unused const char *msg) {
    if (!expr) {
#if defined(__DEBUG__)
        std::cout << "Assert failed at file " << file << ", line " << line << ":\n\t"
                  << msg << " = " << expr << std::endl;
        abort();
#else
        std::cout << 'X' << std::flush;
#endif
    } else {
#if !defined(__DEBUG__)
        std::cout << '.' << std::flush;
#endif
    }
}

#define ASSERT(expr) _assert(expr, __FILE__, __LINE__, #expr)

template<typename T, typename U>
riscv_isa_static_inline void _assert_eq(T expr1, U expr2, riscv_isa_unused const char *file, riscv_isa_unused int line,
                riscv_isa_unused const char *msg1, riscv_isa_unused const char *msg2) {
    if (expr1 != expr2) {
#if defined(__DEBUG__)
        std::cout << "Assert euqal failed at file " << file << ", line " << line << ":\n\t"
                  << msg1 << " = " << expr1 << "\n\t"
                  << msg2 << " = " << expr2 << std::endl;
        abort();
#else
        std::cout << 'X' << std::flush;
#endif
    } else {
#if !defined(__DEBUG__)
        std::cout << '.' << std::flush;
#endif
    }
}

#define ASSERT_EQ(expr1, expr2) _assert_eq(expr1, expr2, __FILE__, __LINE__, #expr1, #expr2)

template<typename T, typename U>
riscv_isa_static_inline void _assert_ne(T expr1, U expr2, riscv_isa_unused const char *file, riscv_isa_unused int line,
                riscv_isa_unused const char *msg1, riscv_isa_unused const char *msg2) {
    if (expr1 == expr2) {
#if defined(__DEBUG__)
        std::cout << "Assert not equal failed at file " << file << ", line " << line << ":\n\t"
                  << msg1 << " = " << expr1 << "\n\t"
                  << msg2 << " = " << expr2 << std::endl;
        abort();
#else
        std::cout << 'X' << std::flush;
#endif
    } else {
#if !defined(__DEBUG__)
        std::cout << '.' << std::flush;
#endif
    }
}

#define ASSERT_NE(expr1, expr2) _assert_ne(expr1, expr2, __FILE__, __LINE__, #expr1, #expr2)


#endif //RISCV_ISA_TEST_HPP
