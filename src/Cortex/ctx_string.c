#include "ctx_string.h"

//
// 1 byte:  0*******
// 2 bytes: 110***** 10******
// 3 bytes: 1110**** 10****** 10******
// 4 bytes: 11110*** 10****** 10****** 10******
//
static CtxSize getUtf8CharSize(CtxChar c)
{
	
	if ((c & 0x80) == 0x00) return 1;
	if ((c & 0xE0) == 0xC0) return 2;
	if ((c & 0xF0) == 0xE0) return 3;
	if ((c & 0xF8) == 0xF0) return 4;

	return 0;
}

inline CtxInt isValidUtf8(CtxChar c)
{
	return getUtf8CharSize(c) != 0;
}

inline CtxChar* skipInvalidChars(const CtxChar* zStr, CtxSize maxStr, const CtxChar* it)
{
	while (!isValidUtf8(*it) && it - zStr < maxStr)
		it++;
	return it - zStr >= maxStr ? zStr : it;
}

inline CtxChar* goBackToPrevChar(const CtxChar* zStr, CtxSize maxStr, const CtxChar* it)
{
	if(it != ctxNULL) while (it - zStr > 0)
	{
		const CtxChar* prev = it-1;

		if (isValidUtf8(*prev))
			return prev;

		it--;
	}

	return ctxNULL;
}

inline CtxInt maximal(CtxInt a, CtxInt b)
{
	return a > b ? a : b;
}

inline CtxInt absolute(CtxInt x)
{
	return maximal(x, -x);
}



_Success_(return != 0) ctxAPI(CtxSize) 
ctx_string_sizeof(_In_reads_or_z_opt_(maxStr) const CtxChar * zStr, CtxSize maxStr)
{
	CtxSize size = 0;
	if (zStr != NULL)
		while (*zStr++ != '\0' && maxStr-- != 0) 
			size++;
	else 
		return 0;
	return maxStr > 0 ? size + 1 : 0;
}


ctxAPI(CtxInt) 
ctx_string_validate(_In_reads_or_z_(maxStr) const CtxChar * zStr, CtxSize maxStr)
{
	if (!zStr)
		return 0;

	const CtxChar* it = zStr;
	while (it - zStr < maxStr && *it != '\0')
	{
		CtxSize size = getUtf8CharSize(*it);
		if (size == 0)
			return it - zStr;

		it += size;
	}

	if (it - zStr >= maxStr || *it != '\0')
		return ctxINT_MAX;

	return -1;
}


_Maybenull_ _Success_(return != zStr) ctxAPI(CtxChar*)
ctx_string_next(_In_reads_or_z_opt_(maxStr) const CtxChar * zStr, CtxSize maxStr, _In_opt_z_ const CtxChar * it)
{
	if (zStr == ctxNULL || it == ctxNULL || *it == '\0')
		return ctxNULL;

	const CtxChar* pNext = skipInvalidChars(zStr, maxStr, it + getUtf8CharSize(*it));

	if (pNext == zStr) 
		return zStr;

	return pNext - zStr <= maxStr - getUtf8CharSize(*pNext) ? pNext : zStr;
}


_Maybenull_ ctxAPI(CtxChar*) 
ctx_string_skip(
	_In_reads_or_z_opt_(maxStr) const CtxChar*  zStr,
	CtxSize                                     maxStr,
	_In_opt_z_ const CtxChar*					it,
	CtxInt										count)
{
	for (int i = 0; i < absolute(count) && it != NULL; i++)
	{
		it = count > 0 ? 
			ctx_string_next(zStr, maxStr, it) : 
			goBackToPrevChar(zStr, maxStr, it);
		if (it == zStr)
			break;
	}
	return (CtxChar*)it;
}


_Maybenull_ _Success_(return != zNeedle) ctxAPI(CtxChar*)
ctx_string_find(
	_In_reads_opt_z_(maxStr) const CtxChar*		zStr,
	CtxSize                                     maxStr,
	_In_reads_opt_z_(maxNeedle) const CtxChar*  zNeedle,
	CtxSize                                     maxNeedle)
{
	return ctxNULL;
}


_Maybenull_ _Success_(return != zStr) ctxAPI(CtxChar*)
ctx_string_set(
	_In_reads_opt_z_(maxStr) const CtxChar* zStr,
	CtxSize                                 maxStr,
	_In_reads_opt_z_(maxStr) const CtxChar* zSet,
	CtxSize                                 maxSet)
{
	return ctxNULL;
}

_Maybenull_ _Success_(return != zStr) ctxAPI(CtxChar*)
ctx_string_complement_set(
	_In_reads_opt_z_(maxStr) const CtxChar* zStr,
	CtxSize                                 maxStr,
	_In_reads_opt_z_(maxStr) const CtxChar* zSet,
	CtxSize                                 maxSet)
{
	return ctxNULL;
}

_Success_(return != ctxUINT_MAX) ctxAPI(CtxUint) 
ctx_string_length_max(_In_reads_or_z_opt_(maxStr) const CtxChar* zStr, CtxSize maxStr, CtxUint maxCount)
{
	if (zStr == ctxNULL)
		return 0;

	CtxUint count = 0;
	for (CtxChar* it = isValidUtf8(*zStr) ? zStr : ctx_string_next(zStr, maxStr, zStr);
		it != NULL && maxCount > 0 && *it != '\0';
		it = ctx_string_next(zStr, maxStr, it), maxCount--)
	{
		if (count > 0 && it == zStr)
			return ctxUINT_MAX;

		count++;
	}

	return count;
}

_Success_(return != ctxUINT_MAX) ctxAPI(CtxUint)
ctx_string_length_ptr(_In_reads_opt_z_(maxStr) const CtxChar * zStr, CtxSize maxStr, _In_z_ const CtxChar * it)
{
	return ctxUINT_MAX;
}

_Success_(return != ctxNULL) ctxAPI(CtxChar*)
ctx_string_encode(_Out_writes_z_(maxDst) CtxChar * zDst, CtxSize maxDst, CtxUint unicode)
{
	return ctxNULL;
}

_Success_(return != ctxNULL) ctxAPI(CtxChar*) 
ctx_string_take(
	_Out_writes_z_(maxDst) CtxChar * zDst,
	CtxSize maxDst,
	_In_reads_or_z_opt_(maxSrc) const CtxChar * zSrc,
	CtxSize maxSrc,
	CtxUint count)
{
	return ctxNULL;
}

_Maybenull_ _Success_(return != ctxNULL) ctxAPI(CtxChar*) 
ctx_string_drop(
	_Out_writes_z_(maxDst) CtxChar * zDst,
	CtxSize                                     maxDst,
	_In_reads_or_z_opt_(maxSrc) const CtxChar * zSrc,
	CtxSize                                     maxSrc,
	CtxUint                                     count)
{
	return ctxNULL;
}