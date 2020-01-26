#ifndef RISC_V_ISA_TEST_HPP
#define RISC_V_ISA_TEST_HPP


#include <iostream>


#define ASSERT(_expr) \
    do { \
        auto expr = (_expr); \
        if (!expr) { \
            std::cout << "Assert failed at file " << __FILE__ << ", line " << __LINE__ << ":\n" \
                      << "\t"#_expr" = " << expr << std::endl; \
        } else { \
            std::cout << '.'; \
        } \
    } while (false)


#define ASSERT_EQ(_expr1, _expr2) \
    do { \
        auto expr1 = (_expr1); \
        auto expr2 = (_expr2); \
        if (expr1 != expr2) { \
            std::cout << "Assert equal failed at file " << __FILE__ << ", line " << __LINE__ << ":\n" \
                      << "\t"#_expr1" = " << expr1 << '\n' \
                      << "\t"#_expr2" = " << expr2 << std::endl; \
        } else { \
            std::cout << '.'; \
        } \
    } while (false)

#define ASSERT_NE(_expr1, _expr2) \
    do { \
        auto expr1 = (_expr1); \
        auto expr2 = (_expr2); \
        if (expr1 == expr2) { \
            std::cout << "Assert equal failed at file " << __FILE__ << ", line " << __LINE__ << ":\n" \
                      << "\t"#_expr1" = " << expr1 << '\n' \
                      << "\t"#_expr2" = " << expr2 << std::endl; \
        } else { \
            std::cout << '.'; \
        } \
    } while (false)


#endif //RISC_V_ISA_TEST_HPP
