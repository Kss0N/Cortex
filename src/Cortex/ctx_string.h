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
inline CtxChar* ctx_string_skip(
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

