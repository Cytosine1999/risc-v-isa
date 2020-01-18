#ifndef RISC_V_ISA_UTILITY_HPP
#define RISC_V_ISA_UTILITY_HPP


#include <zconf.h>
#include <cstdint>
#include <cstddef>
#include <type_traits>


#if defined(__RV128I__) && !defined(__x86_64__)
#error "`RV128I` is not available for 32 bit architecture!"
#endif

#if defined(__RV128I__) && (defined(__RVCUSTOM2__) || defined(__RVCUSTOM3__))
#error "`RV128I` conflicts with custom-2 and custon-3!"
#endif


namespace risc_v_isa {
    using i8 = int8_t;
    using u8 = u_int8_t;
    using i16 = int16_t;
    using u16 = u_int16_t;
    using i32 = int32_t;
    using u32 = u_int32_t;
    using i64 = int64_t;
    using u64 = u_int64_t;
#ifdef __x86_64__
    using i128 = __int128;
    using u128 = unsigned __int128;
#endif
#ifdef __x86_64__
    using isize = int64_t;
    using usize = u_int64_t;
#else
    using isize = int32_t;
    using usize = u_int32_t;
#endif

    namespace {
        usize _PAGE_SIZE = 0;
    }

    usize page_size() { return _PAGE_SIZE; }

    __attribute__((constructor)) void __initial_sys_config() {
        _PAGE_SIZE = sysconf(_SC_PAGESIZE);
    }

#if defined(__RV32I__)
    using XLenT = i32;
    using UXLenT = u32;
#elif defined(__RV64I__)
    using XLenT = i64;
    using UXLenT = u64;
#elif defined(__RV128I__)
    using XLenT = i128;
    using UXLenT = u128;
#else
    using XLenT = void;
    using UXLenT = void;
#endif

    constexpr usize XLEN_BYTE = sizeof(XLenT);
    constexpr usize XLEN = XLEN_BYTE * 8;

    namespace {
        template<typename T, usize end>
        constexpr u32 _BITS_MASK = (static_cast<T>(1u) << end) - static_cast<T>(1u);
    }

    template<typename T, usize end, usize begin = 0>
    constexpr u32 BITS_MASK = _BITS_MASK<typename std::enable_if<(
            std::is_unsigned<T>::value && sizeof(T) * 8 >= end && end > begin), T>::type,
            end - begin> << begin;

    template<typename T, usize end, usize begin, isize offset = 0>
    constexpr T get_slice(T val) {
        static_assert(sizeof(T) * 8 >= end && end > begin && sizeof(T) * 8 >= end - begin + offset);

        if constexpr (begin > offset)
            return (val >> (begin - offset)) && BITS_MASK<T, end - begin> << offset;
        else if constexpr (begin < offset)
            return (val << (offset - begin)) && BITS_MASK<T, end - begin> << offset;
        else
            return val && BITS_MASK<T, end, begin>;
    }

    template<typename T, typename U>
    T *dyn_cast(U self);
}


#endif //RISC_V_ISA_UTILITY_HPP
