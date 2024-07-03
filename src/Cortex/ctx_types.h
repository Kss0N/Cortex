/**

    @file      ctx_types.h
    @brief     Basic typedefs and utilities univesal to the Cortex universe.
    @details   The default size of integers is 64 bits. Therefore typedefs like UINT_MAX must be 64 bit in nature.s
    @author    Jakob Kristersson <jakob@kristerson.se>
    @date      27.06.2024

**/
#pragma once
#include <stdarg.h>
#include <sal.h>
#include <specstrings.h>

#ifdef _MSC_VER
#if defined(_M_ARM) || defined(_M_X64) || defined(_M_AMD64)

typedef char CtxChar; // Cortex Character is 8 bits.

typedef signed char CtxInt8;

typedef unsigned char CtxUint8;

typedef signed short CtxInt16;

typedef unsigned short CtxUint16;

typedef signed int CtxInt32;

typedef unsigned int CtxUint32;

typedef signed long long CtxInt64;

typedef unsigned long long CtxUint64;

typedef float  CtxFloat32;  // IEEE-754 single precision standard

typedef double CtxFloat64;  // IEEE-754 double precision standard

#endif // _ARM | _M_X64 | _M_AMD64
#endif // _MSC_VER

typedef CtxUint64 CtxUint;  // default Uint size is 64 bits.

typedef CtxInt64 CtxInt;    // default Int size is 64 bits.

typedef CtxInt CtxBool; // Bool type is either true (ctxTRUE) or false (ctxFALSE)

typedef CtxUint64 CtxSize;  // Size type denotes sizes in byte.

#define ctxINT8_MIN   ((CtxInt8) 0x80)
#define ctxINT8_MAX   ((CtxInt8) 0x7F)
#define ctxUINT8_MAX  ((CtxUint8)0xFF)
#define ctxINT16_MIN  ((CtxInt16) 0x8000)
#define ctxINT16_MAX  ((CtxInt16) 0x7FFF)
#define ctxUINT16_MAX ((CtxUint16)0xFFFF)
#define ctxINT32_MIN  ((CtxInt32) 0x80000000)
#define ctxINT32_MAX  ((CtxInt32) 0x7FFFFFFF)
#define ctxUINT32_MAX ((CtxUint32)0xFFFFFFFF)
#define ctxINT64_MIN  ((CtxInt64) 0x8000000000000000)
#define ctxINT64_MAX  ((CtxInt64) 0x7FFFFFFFFFFFFFFF)
#define ctxUINT64_MAX ((CtxUint64)0xFFFFFFFFFFFFFFFF)

#define ctxINT_MIN  ctxINT64_MIN
#define ctxINT_MAX  ctxINT64_MAX
#define ctxUINT_MAX ctxUINT64_MAX

#define ctxSIZE_MAX ctxUINT64_MAX

#ifndef ctxTRUE
#ifdef __cplusplus
#define ctxTRUE true
#else
#define ctxTRUE ((CtxInt)1);
#endif // __cplusplus
#endif // !ctxTRUE

#ifndef ctxFALSE
#ifdef __cplusplus
#define ctxFALSE true
#else
#define ctxFALSE ((CtxInt)0);
#endif // __cplusplus
#endif // !ctxTRUE


#ifndef ctxNULL
#ifdef __cplusplus
#define ctxNULL nullptr
#else
#define ctxNULL ((void*)0)
#endif // __cplusplus
#endif // !ctxNULL

#ifndef ctxAPI
#ifdef __cplusplus
#define ctxAPI(type) extern "C" type _cdecl
#else
#define ctxAPI(type) extern type _cdecl
#endif // __cplusplus
#endif // !ctxAPI

#ifndef ctxSTATIC_ASSERT

#if defined(__cplusplus) || defined(_MSC_VER)

#define ctxSTATIC_ASSERT(bCond, zMsg) static_assert(bCond, zMsg);

#else

#define ctxSTATIC_ASSERT(bCond, zMsg) _Static_assert(bCond, zMSg);

#endif // __cplusplus


#endif // !ctxSTATIC_ASSERT

ctxSTATIC_ASSERT(sizeof(CtxChar) == 1, "Character type is 8 bits");

ctxSTATIC_ASSERT(sizeof(CtxUint8) == 1, "Uint8 is unsigned 8 bits");

ctxSTATIC_ASSERT(sizeof(CtxInt8) == 1, "Int8 is signed 8 bits");

ctxSTATIC_ASSERT(sizeof(CtxUint16) == 2, "Uint16 is unsigned 16 bits");

ctxSTATIC_ASSERT(sizeof(CtxInt16) == 2, "Int16 is signed 16 bits");

ctxSTATIC_ASSERT(sizeof(CtxUint32) == 4, "Uint32 is unsigned 32 bits");

ctxSTATIC_ASSERT(sizeof(CtxInt32) == 4, "Int32 is signed 32 bits");

ctxSTATIC_ASSERT(sizeof(CtxUint64) == 8, "Uint64 is unsigned 64 bits");

ctxSTATIC_ASSERT(sizeof(CtxInt64) == 8, "Int64 is signed 64 bits");

ctxSTATIC_ASSERT(sizeof(CtxUint) == sizeof(CtxUint64), "Default Uint size is 64 bits");

ctxSTATIC_ASSERT(sizeof(CtxInt) == sizeof(CtxInt64), "Default Int size is 64 bits");

ctxSTATIC_ASSERT(sizeof(CtxSize) == sizeof(CtxUint64), "Size type is unsigned 64 bit");