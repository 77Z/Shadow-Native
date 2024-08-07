#ifndef SHADOW_NATIVE_TYPES_HPP
#define SHADOW_NATIVE_TYPES_HPP

#include <cinttypes>

// Size of a static C-style array. Don't use on pointers!
#define SHADOW_ARRAYSIZE(ARR) ((int)(sizeof(ARR) / sizeof(*(ARR))))

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef std::size_t usize;
// typedef __ssize_t isize;

typedef float f32;
typedef double f64;

#endif /* SHADOW_NATIVE_TYPES_HPP */
