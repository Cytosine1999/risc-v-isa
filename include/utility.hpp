#ifndef RISCV_ISA_UTILITY_HPP
#define RISCV_ISA_UTILITY_HPP


#include <iostream>
#include <zconf.h>
#include <cstdint>
#include <cstddef>
#include <type_traits>


#if !defined(__RV_BIT_WIDTH__)
#error "Bit width not defined!"
#endif

#if __RV_BIT_WIDTH__ != 32 && __RV_BIT_WIDTH__ != 64
#error "Bit width should be 32 or 64!"
#endif

#if defined(__RV_BASE_I__) && defined(__RV_BASE_E__)
#error "Base instruction set I and E cannot be defined at the same time!"
#endif

#if defined(__RV_BASE_E__) && __RV_BIT_WIDTH__ != 32
#error "Base instruction set E only support 32 bits width!"
#endif

#ifndef VENDOR_ID
#define VENDOR_ID 0
#endif

#ifndef ARCH_ID
#define ARCH_ID 0
#endif

#ifndef IMP_ID
#define IMP_ID 0
#endif

namespace riscv_isa {
    void _warn(const char *file, int line, const char *msg) {
        std::cerr << "Warn at file " << file << ", line " << line << ": " << msg << std::endl;
    }

#define riscv_isa_warn(msg) riscv_isa::_warn(__FILE__, __LINE__, msg)

    __attribute__((noreturn)) void _abort(const char *file, int line, const char *msg) {
        std::cerr << "Abort at file " << file << ", line " << line << ": " << msg << std::endl;

        abort();
    }

#define riscv_isa_abort(msg) riscv_isa::_abort(__FILE__, __LINE__, msg)

    __attribute__((noreturn)) void _unreachable(const char *file, int line, const char *msg) {
        std::cerr << "Unreachable at file " << file << ", line " << line << ": " << msg << std::endl;

        abort();
    }

#define riscv_isa_unreachable(msg) riscv_isa::_unreachable(__FILE__, __LINE__, msg)

#define riscv_isa_unused __attribute__((unused))

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

    template<typename T, usize end, usize begin>
    struct bits_mask {
    private:
        using RetT = typename std::enable_if<(std::is_unsigned<T>::value && sizeof(T) * 8 >= end &&
                                              end > begin), T>::type;

    public:
        static constexpr RetT val = ((static_cast<T>(1u) << (end - begin)) - static_cast<T>(1u)) << begin;
    };

    template<typename T, usize begin>
    struct bit_mask {
    public:
        static constexpr T val = bits_mask<T, begin + 1, begin>::val;
    };

    template<typename T, usize end, usize begin, isize offset = 0, bool flag = (begin > offset)>
    struct _get_bits;

    template<typename T, usize end, usize begin, isize offset>
    struct _get_bits<T, end, begin, offset, true> {
    public:
        static constexpr T inner(T val) {
            return (val >> (begin - offset)) & bits_mask<T, end - begin, 0>::val << offset;
        }
    };

    template<typename T, usize end, usize begin, isize offset>
    struct _get_bits<T, end, begin, offset, false> {
    public:
        static constexpr T inner(T val) {
            return (val << (offset - begin)) & bits_mask<T, end - begin, 0>::val << offset;
        }
    };

    template<typename T, usize end, usize begin, isize offset = 0>
    constexpr inline T get_bits(T val) {
        static_assert(sizeof(T) * 8 >= end, "end exceed length");
        static_assert(end > begin, "end need to be bigger than start");
        static_assert(sizeof(T) * 8 >= end - begin + offset, "result exceed length");

        return _get_bits<T, end, begin, offset>::inner(val);
    }

    template<typename T, usize begin, isize offset = 0>
    constexpr inline T get_bit(T val) { return get_bits<T, begin + 1, begin, offset>(val); }

    struct _xlen_32_trait {
    public:
        using XLenT = i32;
        using UXLenT = u32;
        static constexpr usize XLEN_INDEX = 5;
    };

#if __RV_BIT_WIDTH__ == 64
    struct _xlen_64_trait {
    public:
        using XLenT = i64;
        using UXLenT = u64;
        static constexpr usize XLEN_INDEX = 6;
    };
#endif

    template <typename T>
    struct _xlen_trait : public T {
    public:
        static constexpr usize XLEN_BYTE = sizeof(typename T::XLenT);
        static constexpr usize XLEN = XLEN_BYTE * 8;
        static constexpr usize HALF_WIDTH = XLEN_BYTE / 2;
        static constexpr typename T::UXLenT HALF_MASK = bits_mask<typename T::UXLenT, HALF_WIDTH, 0>::val;
        static constexpr typename T::XLenT XLenTMin = static_cast<typename T::XLenT>(1u << (XLEN_BYTE - 1));
    };

    using xlen_32_trait = _xlen_trait<_xlen_32_trait>;
#if __RV_BIT_WIDTH__ == 64
    using xlen_64_trait = _xlen_trait<_xlen_64_trait>;
#endif

#if __RV_BIT_WIDTH__ == 32
    using xlen_trait = xlen_32_trait;
#elif __RV_BIT_WIDTH__ == 64


    using xlen_trait = xlen_64_trait;
#endif

    /// @cite: RVISM - Volume I - V20191213 - P8
    ///
    /// "We use the term IALIGN (measured in bits) to refer to the instruction-address alignment constraint the
    /// implementation enforces. IALIGN is 32 bits in the base ISA, but some ISA extensions, including the compressed
    /// ISA extension, relax IALIGN to 16 bits. IALIGN may not take on any value other than 16 or 32."
    ///
#if defined(__RV_EXTENSION_C__)
#define IALIGN 16
#else
#define IALIGN 32
#endif

    ///  @cite: RVISM - Volume I - V20191213 - P8
    ///
    /// We use the term ILEN (measured in bits) to refer to the maximum instruction length supported by an
    /// implementation, and which is always a multiple of IALIGN. For implementations supporting only a base
    /// instruction set, ILEN is 32 bits. Implementations supporting longer instructions have larger values of ILEN.

#define ILEN 32
    using ILenT = u32;

    template<typename T, typename U>
    bool is_type(U *self);

    template<typename T, typename U, bool flag = std::is_same<typename T::BaseT, U>::value>
    struct _is_type;

    template<typename T, typename U>
    struct _is_type<T, U, true> {
    public:
        static bool inner(U *self) { return T::is_self_type(self); }
    };

    template<typename T, typename U>
    struct _is_type<T, U, false> {
    private:
        using BaseT = typename T::BaseT;

    public:
        static bool inner(U *self) { return is_type<BaseT>(self) && T::is_self_type(reinterpret_cast<BaseT *>(self)); }
    };

    template<typename T, typename U>
    bool is_type(U *self) { return _is_type<T, U>::inner(self); }

    template<typename T, typename U>
    T *dyn_cast(U *self);

    template<typename T, typename U>
    T *dyn_cast(U *self) { return is_type<T>(self) ? reinterpret_cast<T *>(self) : nullptr; }
}


#endif //RISCV_ISA_UTILITY_HPP
