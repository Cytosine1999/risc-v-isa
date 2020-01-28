#ifndef RISC_V_ISA_UTILITY_HPP
#define RISC_V_ISA_UTILITY_HPP


#include <iostream>
#include <zconf.h>
#include <cstdint>
#include <cstddef>
#include <type_traits>


#if __RV_BIT_WIDTH__ != 32 && __RV_BIT_WIDTH__ != 64
#error "Bit width not defined!"
#endif

namespace risc_v_isa {
    void _warn(const char *file, int line, const char *msg) {
        std::cerr << "Warn at file " << file << ", line " << line << ": " << msg << std::endl;
    }

#define risc_v_isa_warn(msg) risc_v_isa::_warn(__FILE__, __LINE__, msg)

    __attribute__((noreturn)) void _abort(const char *file, int line, const char *msg) {
        std::cerr << "Abort at file " << file << ", line " << line << ": " << msg << std::endl;

        abort();
    }

#define risc_v_isa_abort(msg) risc_v_isa::_abort(__FILE__, __LINE__, msg)

    __attribute__((noreturn)) void _unreachable(const char *file, int line, const char *msg) {
        std::cerr << "Unreachable at file " << file << ", line " << line << ": " << msg << std::endl;

        abort();
    }

#define risc_v_isa_unreachable(msg) risc_v_isa::_unreachable(__FILE__, __LINE__, msg)

#define risc_v_isa_unused __attribute__((unused))

    using i8 = int8_t;
    using u8 = u_int8_t;
    using i16 = int16_t;
    using u16 = u_int16_t;
    using i32 = int32_t;
    using u32 = u_int32_t;
    using i64 = int64_t;
    using u64 = u_int64_t;
#if defined(__x86_64__)
    using isize = int64_t;
    using usize = u_int64_t;
#else
    using isize = int32_t;
    using usize = u_int32_t;
#endif

#if __RV_BIT_WIDTH__ == 32
    using XLenT = i32;
    using UXLenT = u32;
    constexpr usize XLEN_INDEX = 5;
#elif __RV_BIT_WIDTH__ == 64
    using XLenT = i64;
    using UXLenT = u64;
    constexpr usize XLEN_INDEX = 6;
#else
    using XLenT = void;
    using UXLenT = void;
#endif

    constexpr usize XLEN_BYTE = sizeof(XLenT);
    constexpr usize XLEN = XLEN_BYTE * 8;

    /// @cite: RVISM - Volume I - V20191213 - P8
    ///
    /// "We use the term IALIGN (measured in bits) to refer to the instruction-address alignment constraint the
    /// implementation enforces. IALIGN is 32 bits in the base ISA, but some ISA extensions, including the compressed
    /// ISA extension, relax IALIGN to 16 bits. IALIGN may not take on any value other than 16 or 32."
    ///
#if defined(__RV_EXTENSION_C__)
    constexpr usize IALIGN = 16;
#else
    constexpr usize IALIGN = 32;
#endif

    ///  @cite: RVISM - Volume I - V20191213 - P8
    ///
    /// We use the term ILEN (measured in bits) to refer to the maximum instruction length supported by an
    /// implementation, and which is always a multiple of IALIGN. For implementations supporting only a base
    /// instruction set, ILEN is 32 bits. Implementations supporting longer instructions have larger values of ILEN.

    constexpr usize ILEN = 32;
    using ILenT = u32;

    template<typename T, usize end, usize begin>
    constexpr typename std::enable_if<(std::is_unsigned<T>::value && sizeof(T) * 8 >= end && end > begin), T>::type
            BITS_MASK = ((static_cast<T>(1u) << end - begin) - static_cast<T>(1u)) << begin;

    template<typename T, usize end, usize begin, isize offset = 0>
    constexpr inline T get_slice(T val) {
        static_assert(sizeof(T) * 8 >= end && end > begin && sizeof(T) * 8 >= end - begin + offset);

        if constexpr (begin > offset)
            return (val >> (begin - offset)) & BITS_MASK<T, end - begin, 0> << offset;
        else if constexpr (begin < offset)
            return (val << (offset - begin)) & BITS_MASK<T, end - begin, 0> << offset;
        else
            return val & BITS_MASK<T, end, begin>;
    }

    template<typename T, typename U>
    bool is_type(U *self);

    template<typename T, typename U>
    bool is_type(U *self) {
        using BaseT = typename T::BaseT;

        if constexpr (std::is_same<BaseT, U>::value)
            return T::is_self_type(self);
        else
            return is_type < BaseT > (self) && T::is_self_type(reinterpret_cast<BaseT *>(self));
    }

    template<typename T, typename U>
    T *dyn_cast(U *self);

    template<typename T, typename U>
    T *dyn_cast(U *self) { return is_type<T>(self) ? reinterpret_cast<T *>(self) : nullptr; }
}


#endif //RISC_V_ISA_UTILITY_HPP
