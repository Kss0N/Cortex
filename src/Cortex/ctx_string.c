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

_Maybenull_ _Success_(return != zStr) ctxAPI(CtxChar*)
ctx_string_next(_In_reads_or_z_opt_(maxStr) const CtxChar * zStr, CtxSize maxStr, _In_opt_z_ const CtxChar * it)
{
	if (zStr == ctxNULL || it == ctxNULL) 
		return ctxNULL;

	CtxSize size = getUtf8CharSize(*it);
	const CtxChar* pNext = size != 0 ? it + size : skipInvalidChars(zStr, maxStr, it);

	if (pNext == zStr) 
		return zStr;

	pNext = skipInvalidChars(zStr, maxStr, pNext);

	ptrdiff_t diff = pNext - zStr;

	if (pNext == zStr)
		return zStr;

	if (*pNext == '\0')
		return ctxNULL;

	const CtxChar nextSize = getUtf8CharSize(*pNext);

	return pNext - zStr < maxStr - nextSize ? pNext : zStr;
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
