#ifndef RISC_V_ISA_TEST_HPP
#define RISC_V_ISA_TEST_HPP


#include <iostream>
#include "utility.hpp"


template<typename T>
void _assert(T expr, risc_v_isa_unused const char *file, risc_v_isa_unused int line,
        risc_v_isa_unused const char *msg) {
    if (!expr) {
#if defined(DEBUG)
        std::cout << "Assert failed at file " << file << ", line " << line << ":\n\t"
                  << msg << " = " << expr << std::endl;
        abort();
#else
        std::cout << 'X' << std::flush;
#endif
    } else {
#if !defined(DEBUG)
        std::cout << '.' << std::flush;
#endif
    }
}

#define ASSERT(expr) _assert(expr, __FILE__, __LINE__, #expr)

template<typename T, typename U>
void _assert_eq(T expr1, U expr2, risc_v_isa_unused const char *file, risc_v_isa_unused int line,
        risc_v_isa_unused const char *msg1, risc_v_isa_unused const char *msg2) {
    if (expr1 != expr2) {
#if defined(DEBUG)
        std::cout << "Assert euqal failed at file " << file << ", line " << line << ":\n\t"
                  << msg1 << " = " << expr1 << "\n\t"
                  << msg2 << " = " << expr2 << std::endl;
        abort();
#else
        std::cout << 'X' << std::flush;
#endif
    } else {
#if !defined(DEBUG)
        std::cout << '.' << std::flush;
#endif
    }
}

#define ASSERT_EQ(expr1, expr2) _assert_eq(expr1, expr2, __FILE__, __LINE__, #expr1, #expr2)

template<typename T, typename U>
void _assert_ne(T expr1, U expr2, risc_v_isa_unused const char *file, risc_v_isa_unused int line,
        risc_v_isa_unused const char *msg1, risc_v_isa_unused const char *msg2) {
    if (expr1 == expr2) {
#if defined(DEBUG)
        std::cout << "Assert not equal failed at file " << file << ", line " << line << ":\n\t"
                  << msg1 << " = " << expr1 << "\n\t"
                  << msg2 << " = " << expr2 << std::endl;
        abort();
#else
        std::cout << 'X' << std::flush;
#endif
    } else {
#if !defined(DEBUG)
        std::cout << '.' << std::flush;
#endif
    }
}

#define ASSERT_NE(expr1, expr2) _assert_ne(expr1, expr2, __FILE__, __LINE__, #expr1, #expr2)


#endif //RISC_V_ISA_TEST_HPP
