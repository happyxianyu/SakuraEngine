#pragma once
#include "SkrBase/config.h"
#include "SkrBase/tools/is_detected.hpp"
#include <type_traits>

namespace skr
{
template <typename T>
struct Hash;
namespace detail
{
template <typename T>
using has_skr_hash = decltype(T::_skr_hash(std::declval<const T&>()));
}
template <typename T>
inline constexpr bool has_skr_hash_v = is_detected_v<detail::has_skr_hash, T>;

template <typename T>
struct Hash {
    SKR_INLINE size_t operator()(const T& v) const
    {
        if constexpr (std::is_enum_v<T>) // enum case
        {
            using UnderlyingType = std::underlying_type_t<T>;
            return Hash<UnderlyingType>()(static_cast<UnderlyingType>(v));
        }
        else // struct case
        {
            static_assert(has_skr_hash_v<T>, "T::_skr_hash(const T&) is not defined");
            return T::_skr_hash(v);
        }
    }
};

// hash combine
SKR_INLINE size_t hash_combine(size_t seed, size_t combine)
{
    return seed ^ (combine + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

} // namespace skr

// primitive type hash
namespace skr
{
#define SKR_IMPL_HASH_CAST(type)                     \
    template <>                                      \
    struct Hash<type> {                              \
        SKR_INLINE size_t operator()(type val) const \
        {                                            \
            return static_cast<size_t>(val);         \
        }                                            \
    };

// helper
SKR_INLINE size_t fold_if_needed(uint64_t v)
{
    if constexpr (sizeof(size_t) == 4)
    {
        return static_cast<size_t>(v ^ (v >> 32));
    }
    else if constexpr (sizeof(size_t) == 8)
    {
        return static_cast<size_t>(v);
    }
}

// impl for pointer
template <typename T>
struct Hash<T*> {
    SKR_INLINE size_t operator()(const T* val) const
    {
        return static_cast<size_t>(reinterpret_cast<const uintptr_t>(val));
    }
};

// impl for boolean
SKR_IMPL_HASH_CAST(bool)

// impl for char
SKR_IMPL_HASH_CAST(char)
SKR_IMPL_HASH_CAST(signed char)
SKR_IMPL_HASH_CAST(unsigned char)
SKR_IMPL_HASH_CAST(wchar_t)
SKR_IMPL_HASH_CAST(char16_t)
SKR_IMPL_HASH_CAST(char32_t)

// impl for int
SKR_IMPL_HASH_CAST(int16_t)
SKR_IMPL_HASH_CAST(uint16_t)
SKR_IMPL_HASH_CAST(int32_t)
SKR_IMPL_HASH_CAST(uint32_t)
SKR_IMPL_HASH_CAST(int64_t)
SKR_IMPL_HASH_CAST(uint64_t)

// impl for float
template <>
struct Hash<float> {
    SKR_INLINE size_t operator()(float val) const noexcept
    {
        // -0.0 and 0.0 should return same hash
        uint32_t* as_int = reinterpret_cast<uint32_t*>(&val);
        return (val == 0) ? static_cast<size_t>(0) :
                            static_cast<size_t>(*as_int);
    }
};

// impl for double
template <>
struct Hash<double> {
    SKR_INLINE size_t operator()(double val) const noexcept
    {
        // -0.0 and 0.0 should return same hash
        uint64_t* as_int = reinterpret_cast<uint64_t*>(&val);
        return (val == 0) ? static_cast<size_t>(0) :
                            fold_if_needed(*as_int);
    }
};

#undef SKR_IMPL_HASH_CAST
} // namespace skr

// impl for string
namespace skr
{
// char 8
template <>
struct Hash<char*> {
    SKR_INLINE size_t operator()(const char* p) const
    {
        uint32_t c, result = 2166136261U; // FNV1 hash. Perhaps the best string hash. Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
        while ((c = (uint8_t)*p++) != 0)  // Using '!=' disables compiler warnings.
            result = (result * 16777619) ^ c;
        return (size_t)result;
    }
};
template <>
struct Hash<const char*> {
    SKR_INLINE size_t operator()(const char* p) const
    {
        return Hash<char*>()(p);
    }
};

// char 16
template <>
struct Hash<char16_t*> {
    SKR_INLINE size_t operator()(const char16_t* p) const
    {
        uint32_t c, result = 2166136261U; // Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
        while ((c = (uint16_t)*p++) != 0) // cast to unsigned 16 bit.
            result = (result * 16777619) ^ c;
        return (size_t)result;
    }
};
template <>
struct Hash<const char16_t*> {
    SKR_INLINE size_t operator()(const char16_t* p) const
    {
        return Hash<char16_t*>()(p);
    }
};

// char 32
template <>
struct Hash<char32_t*> {
    SKR_INLINE size_t operator()(const char32_t* p) const
    {
        uint32_t c, result = 2166136261U; // Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
        while ((c = (uint32_t)*p++) != 0) // cast to unsigned 32 bit.
            result = (result * 16777619) ^ c;
        return (size_t)result;
    }
};
template <>
struct Hash<const char32_t*> {
    SKR_INLINE size_t operator()(const char32_t* p) const
    {
        return Hash<char32_t*>()(p);
    }
};
} // namespace skr