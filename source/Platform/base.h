
#ifndef _BASE_H_
#define _BASE_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Figuring out the Operating System
#if defined(_WIN32)
#  define _ON_WINDOWS_ 1
#elif defined(__linux__)
#  define _ON_LINUX_ 1
#endif

// Figuring out the Compiler
#if defined(__clang__)
#  define _CLANG_COMPILER_ 1
#elif defined(_MSC_VER)
#  define _CL_COMPILER_ 1
#endif

// Resetting the rest of the Macros to 0
#if !defined(_ON_WINDOWS_)
#  define _ON_WINDOWS_ 0
#endif
#if !defined(_ON_LINUX_)
#  define _ON_LINUX_ 0
#endif
#if !defined(_CLANG_COMPILER_)
#  define _CLANG_COMPILER_ 0
#endif
#if !defined(_CL_COMPILER_)
#  define _CL_COMPILER_ 0
#endif

#define ARRAY_COUNT(x) (sizeof(x) / sizeof(*(x)))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define SUCCESS(x) ((x) ? false : true)

#if _ON_WINDOWS_
#define DEBUG_BREAK __debugbreak()
#endif

#define ASSERT(x, m, ...)                       \
    if (x) {                                    \
        printf(m, __VA_ARGS__);                 \
        printf("\n");                           \
        DEBUG_BREAK;                            \
    }  

typedef int8_t    i8;
typedef uint8_t   u8;
typedef int16_t  i16;
typedef uint16_t u16;
typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;
typedef float    f32;
typedef double   f64;

typedef uint32_t Color;

#define global   static 
#define internal static 

#endif // _BASE_H_
