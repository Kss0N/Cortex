#include "pch.h"
#include "ctx_string.h"

inline CtxInt16 resolve_endian(CtxInt16 x)
{
	union Bytes {
		CtxInt16 integer;
		CtxUint8 bytes[2];
	};

	auto a = Bytes{ x };
	return Bytes{ .bytes = {a.bytes[1], a.bytes[0]} }.integer;
}

inline CtxInt32 resolve_endian(CtxInt32 x)
{
	union Bytes {
		CtxInt32 integer;
		CtxUint8 bytes[4];
	};

	auto a = Bytes{ x };
	return Bytes{ .bytes = {a.bytes[3], a.bytes[2], a.bytes[1], a.bytes[0]}}.integer;
}

namespace strings
{

/*
* Sizeof
*/

TEST(SizeOf, BasicCase)
{
	const char zStr[] = "Hello World!";

	EXPECT_EQ(ctx_string_sizeof(zStr, sizeof zStr), sizeof "Hello World!");
}

TEST(SizeOf, Empty)
{
	const char zStr[] = "";
	EXPECT_EQ(ctx_string_sizeof(zStr, sizeof zStr), sizeof "");
}

TEST(SizeOf, Null)
{
	EXPECT_EQ(ctx_string_sizeof(NULL, ctxSIZE_MAX), 0);
}

TEST(SizeOf, MaxReached)
{
	const char zStr[] = "Hello World";

	EXPECT_EQ(ctx_string_sizeof(zStr, sizeof"ello World"), 0);
}

TEST(SizeOf, IgnoreContent)
{
	const char zStr1[] = "AAAAAAAAAAAA";
	const char zStr2[] = "😄😄😄";
	static_assert(sizeof zStr1 == sizeof zStr2);

	auto size1 = ctx_string_sizeof(zStr1, sizeof zStr1);
	auto size2 = ctx_string_sizeof(zStr2, sizeof zStr2);

	EXPECT_EQ(size1, size2);
}

TEST(SizeOf, PrematureNul)
{
	const char zStr[] = "Hello\0World!";

	EXPECT_EQ(ctx_string_sizeof(zStr, sizeof zStr), sizeof "Hello");
}

/*
* Next
*/

TEST(Next, iteration)
{
	const char zStr[] = "Hi! åäö ᚠᚢᚦ 😄😂🤣";

	// The sequence is iterated through, hence ASSERT instead of EXPECT

	const CtxChar* it = zStr;
	ASSERT_EQ(*it, 'H');

	it = ctx_string_next(zStr, sizeof zStr, it);
	ASSERT_EQ(*it, 'i');

	it = ctx_string_next(zStr, sizeof zStr, it);
	ASSERT_EQ(*it, '!');

	it = ctx_string_next(zStr, sizeof zStr, it);
	ASSERT_EQ(*it, ' ');
	
	it = ctx_string_next(zStr, sizeof zStr, it);
	ASSERT_EQ(*(short*)it, resolve_endian((short)'å'));
	
	it = ctx_string_next(zStr, sizeof zStr, it);
	ASSERT_EQ(*(short*)it, resolve_endian((short)'ä'));
	
	it = ctx_string_next(zStr, sizeof zStr, it);
	ASSERT_EQ(*(short*)it, resolve_endian((short)'ö'));
	
	it = ctx_string_next(zStr, sizeof zStr, it);
	ASSERT_EQ(*it, ' ');
	
	// TODO find way to ASSERT the 3 byte chars
	for (int i = 0; i < 3; i++)
		it = ctx_string_next(zStr, sizeof zStr, it);

	it = ctx_string_next(zStr, sizeof zStr, it);
	ASSERT_EQ(*it, ' ');

	it = ctx_string_next(zStr, sizeof zStr, it);
	ASSERT_EQ(*(int*)it, resolve_endian('😄'));

	it = ctx_string_next(zStr, sizeof zStr, it);
	ASSERT_EQ(*(int*)it, resolve_endian('😂'));

	it = ctx_string_next(zStr, sizeof zStr, it);
	ASSERT_EQ(*(int*)it, resolve_endian('🤣'));
	
	it = ctx_string_next(zStr, sizeof zStr, it);
	ASSERT_EQ(it, nullptr);
}

TEST(Next, Null)
{
	const char* zStr = nullptr;

	EXPECT_EQ(ctx_string_next(zStr, ctxSIZE_MAX, zStr), nullptr);

	EXPECT_EQ(ctx_string_next(zStr, ctxSIZE_MAX, zStr + 4), nullptr);
}

TEST(Next, ItIsNull)
{
	const char zStr[] = "Hello World!";

	EXPECT_EQ(ctx_string_next(zStr, sizeof zStr, nullptr), nullptr);
}

TEST(Next, InvalidChar)
{
	const char zStr[] = { 'H', 0xFF, '!', '\0' };

	const CtxChar* it = ctx_string_next(zStr, sizeof zStr, zStr);

	EXPECT_EQ(*it, '!');
}

TEST(Next, InTheMiddle)
{
	const char zStr[] = "😄Hi!";

	const CtxChar* it = ctx_string_next(zStr, sizeof zStr - 1, zStr + 1);

	EXPECT_EQ(*it, 'H');
}

TEST(Next, MaxSearchReached)
{
	const char zStr[] = "Hi";
	const CtxSize maxStr = sizeof zStr - 1;

	//const CtxChar* it = ctx_string_next(zStr, maxStr, zStr);
	//ASSERT_EQ(*it, 'i');

	EXPECT_EQ(ctx_string_next(zStr, maxStr, zStr+1), zStr);
}

TEST(Next, OnlyInvalidCharacters)
{
	const char zStr[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, '\0' };
	
	EXPECT_EQ(ctx_string_next(zStr, sizeof zStr, zStr), nullptr);
}

TEST(Next, InvalidCharactersOutOfBound)
{
	const char zStr[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

	EXPECT_EQ(ctx_string_next(zStr, sizeof zStr, zStr), zStr);
}

TEST(Next, EmojiOutOfBounds)
{
	const char zStr[] = "Hi😄";
	const CtxSize maxStr = sizeof zStr - 2;

	const CtxChar* it = ctx_string_next(zStr, maxStr, zStr);
	//ASSERT_EQ(*it, 'i');

	it = ctx_string_next(zStr, maxStr, it);
	EXPECT_EQ(it, zStr);
}

/*
* Skip
*/

TEST(Skip, EveryOther)
{
	const char zStr[] = "Hello, World!";

	const CtxChar* it = ctx_string_skip(zStr, sizeof zStr, zStr, 2);
	ASSERT_EQ(*it, 'l');

	it = ctx_string_skip(zStr, sizeof zStr, it, 2);
	ASSERT_EQ(*it, 'o');

	it = ctx_string_skip(zStr, sizeof zStr, it, 2);
	ASSERT_EQ(*it, ' ');

	it = ctx_string_skip(zStr, sizeof zStr, it, 2);
	ASSERT_EQ(*it, 'o');

	it = ctx_string_skip(zStr, sizeof zStr, it, 2);
	ASSERT_EQ(*it, 'l');

	it = ctx_string_skip(zStr, sizeof zStr, it, 2);
	ASSERT_EQ(*it, '!');

}

/*
* Length
*/
TEST(Length, Empty)
{
	const char zStr[] = "";

	EXPECT_EQ(ctx_string_length(zStr, sizeof zStr), 0);
}

TEST(Length, Unicode)
{
	const char zStr[] = "Hi! åäö ᚠᚢᚦ 😄😂🤣";

	EXPECT_EQ(ctx_string_length(zStr, sizeof zStr), 15);
}

TEST(Length, InvalidChar)
{
	const char zStr[] = { 'H', 'e', 'l', 'l', 'o', 0xFF, 'W', 'o', 'r', 'l', 'd', '\0'};

	EXPECT_EQ(ctx_string_length(zStr, sizeof zStr), 10);
}

TEST(Length, OutOfBounds)
{
	const char zStr[] = "Hello World!";
	const CtxSize maxSize = sizeof zStr - 1;

	EXPECT_EQ(ctx_string_length(zStr, maxSize), ctxUINT_MAX);
}

TEST(Length, EmojiOutOfBounds)
{
	const char zStr[] = "Hi!😄";
	const CtxSize maxSize = sizeof zStr - 2;

	EXPECT_EQ(ctx_string_length(zStr, maxSize), ctxUINT_MAX);
}

TEST(Length, OnlyInvalidChars)
{
	const char zStr[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, '\0' };

	EXPECT_EQ(ctx_string_length(zStr, sizeof zStr), 0);
}

TEST(Length, InvalidCharsOutOfBounds)
{
	const char zStr[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

	EXPECT_EQ(ctx_string_length(zStr, sizeof zStr), ctxUINT_MAX);
}

TEST(Length, Null)
{
	EXPECT_EQ(ctx_string_length(nullptr, 0), 0);
}

TEST(Length, MaxCount)
{
	const char zStr[] = "😄Hello!";

	EXPECT_EQ(ctx_string_length_max(zStr, sizeof zStr, 3), 3);
}




}
