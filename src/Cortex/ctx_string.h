/**

    @file      ctx_strings.h
    @brief     Modernized implementation of Stdlib's string as base. Operates on nul-terminated ASCII/UTF-8 character sequences (zStrings). Has security concerns baked into it. Logical module.
    @details   Functions that considers characters only consider valid UTF-8 characters. Invalid characters are ignored and count to nothing.
    @author    Jakob Kristersson <jakob@kristerson.se>
    @date      27.06.2024

**/
#pragma once
#include "ctx_types.h"


/**
    @brief   Counts byte size of zStr including nul-terminator.
    @details When an invalid input is encountered, function returns zero such that if result is (foolishly) passed into an allocator without checking, the malloc should malfunction.
    @param   zStr   nul-terminated string or NULL
    @param   maxStr maximal size of zStr. If this value is reached before finding nul-terminator, zStr is considered invalid. Pass ctxSIZE_MAX if there's no such limit.
    @returns        byte size of zStr. 0 if maxStr is reached or zStr is NULL
**/
_Success_(return != 0) ctxAPI(CtxSize) ctx_string_sizeof(_In_reads_or_z_opt_(maxStr) const CtxChar* zStr, CtxSize maxStr);


/**
    @brief          Validates `zStr`. A Valid zString is one where there only exist valid UTF-8 characters and nul-terminator comes before the `maxStr` value is reached.
    @details        Return values are based on the metaphor of a doctor running tests on a patient. If a test for some disease returns negative - it means the patient does not suffer from that. There's also the question of NULL as input to `zStr`, while most of the functions in this module treats NULL as valid, this function treats NULL as an invalid input, which always returns 0.
    @param   zStr   Pointer to beginning of block to validate.
    @param   maxStr Maximal size of block within which it's expected the nul-terminator exists
    @returns        If test of `zStr` returns a negative, there is no error. If there's a positive for error then the array index of the errorous character is returned. ctxINT_MAX if nul-terminator cannot be found. 
**/
ctxAPI(CtxInt) ctx_string_validate(_In_reads_or_z_(maxStr) const CtxChar* zStr, CtxSize maxStr);


/**
    @brief   Finds next valid character in zStr seen from position of `it`.
    @details When invalid input is encountered, function returns zStr pointer so that an iteration of invalid string without checking validity hangs execution. Likewise, since NULL indicates that iteration reaches it's end, if `it` is NULL, then that should cause the precondition for a potential infinite loop.
    @param   zStr   nul terminated string or NULL
    @param   maxStr maximal size of zStr. If this value is reached before finding next character, zStr is considered invalid in it's entirety and beginning of zStr is returned.
    @param   it     iterator's current position.
    @returns        On success, pointer to beginning of next valid Character, NULL if `it` points at the character before the nul-terminator or input zStr is NULL or `it` is NULL, beginning of zStr on failure.
**/
_Maybenull_ _Success_(return != zStr) ctxAPI(CtxChar*) ctx_string_next(_In_reads_or_z_opt_(maxStr) const CtxChar* zStr, CtxSize maxStr, _In_opt_z_ const CtxChar* it);


/**
    @brief   Skips over `count` characters from `it`.
    @param   zStr   nul-terminated string or NULL
    @param   maxStr maximal size of zStr. If this value is reached before finding next character, zStris considered invalid and beginning of zStr is returned.
    @param   it     iterator's current position.
    @param   count  number of valid characters to skip
    @returns        pointer to beginning of next valid Character after skipping `count`. NULL if `it` points to nul-terminator, is NULL or if zStr is NULL. zStr pointer replicated on failure.
**/
_Maybenull_ _Success_(return != zStr) inline CtxChar* ctx_string_skip(
    _In_reads_or_z_opt_(maxStr) const CtxChar*  zStr, 
    CtxSize                                     maxStr, 
    _In_opt_z_ const CtxChar*                   it,
    CtxUint                                     count)
{
    for (int i = 0; i < count && it != NULL; i++)
    {
        it = ctx_string_next(zStr, maxStr, it);
        if (it == zStr)
            break;
    }
    return (CtxChar*)it;
}


/**
    @brief   Search for substring `zNeedle` within `zStr`.
    @details There is the possibility that the `zNeedle` immediately is found within `zStr`, That is of course not an error condition, why `zNeedle` is chosen instead as return if there's an error.
    @param   zStr      Nul-terminated string to search through for substring or NULL.
    @param   maxStr    Maximal Size of `zStr`. If this value is reached before `zNeedle` or nul-terminator is encountered, `zStr` is invalid and function fails.
    @param   zNeedle   Substring to find in `zStr`.
    @param   maxNeedle maximal sizeof `zNeedle`.
    @returns           Pointer to beginning of substring within `zStr` if found, NULL if not found. `zNeedle` pointer if error is encountered.
**/
_Maybenull_ _Success_(return != zNeedle) ctxAPI(CtxChar*) ctx_string_find(
    _In_reads_opt_z_(maxStr) const CtxChar*     zStr, 
    CtxSize                                     maxStr, 
    _In_reads_opt_z_(maxNeedle) const CtxChar*  zNeedle,
    CtxSize                                     maxNeedle);

/**
    @brief   Skips valid and invalid characters in `zStr` that are not in set defined by `zSet`.
    @param   zStr   Nul-terminated string to iterate through or NULL.
    @param   maxStr Maximal Size of `zStr`. If this value is reached before a member of the set or Nul-terminator is found, zStr is invalid.
    @param   zSet   Nul-terminated string representing set of characters. Redundancies are ignored.
    @param   maxSet Maximal Size of `zSet`. If this value is reached before Nul-terminator in `zSet`, the Set is invalid and Null is returned.
    @returns        Pointer to first character in `zStr` that is member of `zSet` or NULL if there are none. 
**/
_Maybenull_ _Success_(return != zStr) ctxAPI(CtxChar*) ctx_string_set(
    _In_reads_opt_z_(maxStr) const CtxChar* zStr, 
    CtxSize                                 maxStr, 
    _In_reads_opt_z_(maxStr) const CtxChar* zSet, 
    CtxSize                                 maxSet);


/**
    @brief   Skips valid and invalid characters in `zStr` that are not in the complement of the set defined by `zSet`. (Skips characters that ARE in the set)
    @param   zStr   Nul-terminated string to iterate through or NULL.
    @param   maxStr Maximal Size of `zStr`. If this value is reached before a member of the set or Nul-terminator is found, zStr is invalid.
    @param   zSet   Nul-terminated string representing set of characters. Redundancies are ignored.
    @param   maxSet Maximal Size of `zSet`. If this value is reached before Nul-terminator in `zSet`, the Set is invalid and Null is returned.
    @returns        Pointer to first character in `zStr` that is in complement of `zSet` or NULL if there are none. 
**/
_Maybenull_ _Success_(return != zStr) ctxAPI(CtxChar*) ctx_string_complement_set(
    _In_reads_opt_z_(maxStr) const CtxChar* zStr,
    CtxSize                                 maxStr, 
    _In_reads_opt_z_(maxStr) const CtxChar* zSet, 
    CtxSize                                 maxSet);


/**
    @brief            Counts the number of valid characters from beginning of pointer `zStr` up to, but excluding the nul-terminator. If `maxCount` characters are reached in iteration, it is canceled and `maxCount` is returned.
    @details          Result of this function should not be used for memory allocations. This function does not correspond to size: best case it's sizeof zStr - 1, while worse case, size is (sizeof zStr - 1)/4. To query for size, instead `string_sizeof()` should be used.
    @param   zStr     nul-terminated string or NULL
    @param   maxStr   maximal size of zStr. If this value is reached before finding nul-terminator, zStr is considered invalid. Pass ctxSIZE_MAX if there's no such limit.
    @param   maxCount maximal length before searching is canceled. If this value is reached before finding nul-terminator, zStr may or may not be valid, but regardless the `maxCount` value is returned. Pass `ctxUINT_MAX` if there's no such limit.
    @returns          number of valid characters in `zStr` or maxCount if that value is reached before the nul-terminator. 0 if zStr is NULL and ctxUINT_MAX on failure.
**/
_Success_(return != ctxUINT_MAX) ctxAPI(CtxUint) ctx_string_length_max(_In_reads_or_z_opt_(maxStr) const CtxChar* zStr, CtxSize maxStr, CtxUint maxCount);


/**
    @brief   Counts number of valid characters, beginning at pointer `zStr` until `it` or the nul-terminator, whichever comes first.
    @param   zStr   Nul-terminated string.
    @param   maxStr Maximal size of zStr. If this value is reached before finding nul-terminator or it, `zStr` is invalid. Pass ctxSIZE_MAC if there's no such limit.
    @param   it     Pointer to final character before counting and iteration is terminated.
    @returns        Number of valid characters in `zStr` until it or the nul-terminator. 0 if zStr or `it` is NULL or  and ctxUINT_MAX on failure.
**/
_Success_(return != ctxUINT_MAX) ctxAPI(CtxUint) ctx_string_length_ptr(_In_reads_to_ptr_opt_z_(it) const CtxChar* zStr, CtxSize maxStr, _In_opt_z_ const CtxChar* it);


_Success_(return != ctxUINT_MAX) inline CtxUint ctx_string_length(_In_reads_or_z_opt_(maxStr) const CtxChar* zStr, CtxSize maxStr)
{
    // ctxUINT_MAX makes it physicaly (but not theoreticaly) impossible to reach `maxCount`.
    return ctx_string_length_max(zStr, maxStr, ctxUINT_MAX);
}


/**
    @brief   Encodes a unicode codepoint as a UTF-8 character.
    @param   zDst    pointer to buffer storing UTF-8 character.
    @param   maxDst  max size of destination buffer. Optimally it should be >5.
    @param   unicode code point to encode.  > 0x10FFFF is invalid.
    @returns         zDst pointer replicated on success. NULL on failure.
**/
_Maybenull_ _Success_(return != ctxNULL) ctxAPI(CtxChar*) ctx_string_encode(_Out_writes_z_(maxDst) CtxChar* zDst, CtxSize maxDst, CtxUint unicode);


/**
    @brief   Takes `count` valid characters from `zStr` and copies them sequentialy over to buffer pointed to by `zDst` and appends nul-terminator. If buffer is too small (as indicated by `maxDst`), truncation occurs. If `count` is larger than the actual length of `zStr`, the entire string is copied over.
    @param   zDst   pointer to buffer into which content is copied. If Null, operation will fail.
    @param   maxDst maximal size of string that buffer can contain. If 0, operation will fail. If this is too small to contain all that is desired from `zSrc`, forceful truncation occurs such that nul-terminator also fits.
    @param   zSrc   source string to copy.
    @param   maxSrc maximal size of `zSrc`. If this value is reached before nul-terminator is found in `zSrc`, `zSrc` is invalid & therefore `zDst` is also considered invalid and function fails.
    @param   count  maximal number of characters to take from `zSrc` over to `zDst`. If there's no such upper limit, pass ctxUINT_MAX.
    @returns        Pointer to `zDst` replicated for chained calls. On Failure, Null is returned.
**/
_Maybenull_ _Success_(return != ctxNULL) ctxAPI(CtxChar*) ctx_string_take(
    _Out_writes_z_(maxDst) CtxChar*             zDst, 
    CtxSize                                     maxDst, 
    _In_reads_or_z_opt_(maxSrc) const CtxChar*  zSrc,
    CtxSize                                     maxSrc, 
    CtxUint                                     count);


/**
    @brief   Copies over `zSrc` to buffer pointed to by `zDst` and appends nul-terminator. If buffer is too small (as indicated by `maxDst`), truncation occurs.
    @param   zDst   pointer to buffer into which content is copied. If Null, operation will fail.
    @param   maxDst maximal size of string that buffer can contain. If 0, operation will fail. If this is too small to contain all that is desired from `zSrc`, forceful truncation occurs such that nul-terminator also fits.
    @param   zSrc   source string to copy.
    @param   maxSrc maximal size of `zSrc`. If this value is reached before nul-terminator is found in `zSrc`, `zSrc` is invalid & therefore `zDst` is also considered invalid and function fails.
    @returns        Pointer to `zDst` replicated for chained calls. On Failure, Null is returned.
**/
_Maybenull_ _Success_(return != ctxNULL) inline CtxChar* ctx_string_copy(
    _Out_writes_z_(maxDst) CtxChar* zDst,
    CtxSize maxDst,
    _In_reads_or_z_opt_(maxSrc) const CtxChar * zSrc,
    CtxSize maxSrc)
{
    return ctx_string_take(zDst, maxDst, zSrc, maxSrc, ctxUINT_MAX);
}


/**
    @brief   Skips `count` valid characters from `zStr` and copies the rest sequentialy over to buffer pointed to by `zDst` and appends nul-terminator. If buffer is too small (as indicated by `maxDst`), truncation occurs. If `count` is larger than the actual length of `zStr`, empty string is returned.
    @param   zDst   pointer to buffer into which content is copied. If Null, operation will fail.
    @param   maxDst maximal size of string that buffer can contain. If 0, operation will fail. If this is too small to contain all that is desired from `zSrc`, forceful truncation occurs such that nul-terminator also fits.
    @param   zSrc   source string to copy.
    @param   maxSrc maximal size of `zSrc`. If this value is reached before nul-terminator is found in `zSrc`, `zSrc` is invalid & therefore `zDst` is also considered invalid and function fails.
    @param   count  maximal number of characters to skip before copying from `zSrc` over to `zDst`.
    @returns        Pointer to `zDst` replicated for chained calls. On Failure, Null is returned.
**/
_Maybenull_ _Success_(return != ctxNULL) ctxAPI(CtxChar*) ctx_string_drop(
    _Out_writes_z_(maxDst) CtxChar*             zDst,
    CtxSize                                     maxDst,
    _In_reads_or_z_opt_(maxSrc) const CtxChar*  zSrc,
    CtxSize                                     maxSrc,
    CtxUint                                     count);


/**
    @brief   
    @param   zDst    
    @param   maxDst  
    @param   zSrc    
    @param   maxSrc  
    @param   ixBegin 
    @param   ixEnd   
    @returns         
**/
_Maybenull_ _Success_(return != ctxNULL) ctxAPI(CtxChar*) ctx_string_slice(
    _Out_writes_z_(maxDst) CtxChar*         zDst,
    CtxSize                                 maxDst,
    _In_reads_opt_z_(maxSrc) const CtxChar* zSrc,
    CtxSize                                 maxSrc,
    CtxInt                                  ixBegin,
    CtxInt                                  ixEnd);


/**
    @brief 
    @param   zDst
    @param   maxDst
    @param   zSrc
    @param   maxSrc
    @param   ixBegin
    @param   ixEnd
    @param   step
    @returns
**/
_Maybenull_ _Success_(return != ctxNULL) ctxAPI(CtxChar*) ctx_string_slice_step(
    _Out_writes_z_(maxDst) CtxChar * zDst,
    CtxSize                                 maxDst,
    _In_reads_opt_z_(maxSrc) const CtxChar * zSrc,
    CtxSize                                 maxSrc,
    CtxInt                                  ixBegin,
    CtxInt                                  ixEnd,
    CtxInt                                  step);


/**
    @brief   Copies characters from beginning of `zSrc` to `itFinal` sequentially over to `zDst` and appends nul-terminator. 
    @param   zDst    pointer to beginning of destination buffer.
    @param   maxDst  
    @param   zSrc    
    @param   maxSrc  
    @param   itBegin 
    @param   itFinal 
    @returns         
**/
_Maybenull_ _Success_(return != ctxNULL) ctxAPI(CtxChar*) ctx_string_extract(
    _Out_writes_z_(maxDst) CtxChar*             zDst,
    CtxSize                                     maxDst, 
    _In_reads_(maxSrc) const CtxChar*           zSrc,
    CtxSize                                     maxSrc,
    _In_reads_to_ptr_z_(itFinal) const CtxChar* itBegin,
    _In_z_ const CtxChar*                       itFinal);


ctxAPI(CtxChar*) ctx_string_vprintf(CtxChar* zDst, CtxSize maxDst, const CtxChar* zFormat, va_list);

ctxAPI(CtxChar*) ctx_string_vscanf(const CtxChar* zStr, CtxSize maxStr, const CtxChar* zFormat, va_list);