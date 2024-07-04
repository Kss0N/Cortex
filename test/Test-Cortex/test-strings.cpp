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
* Validate
*/

TEST(Validate, Null)
{
	EXPECT_EQ(ctx_string_validate(nullptr, 0), 0);
}

TEST(Validate, EmptyString)
{
	EXPECT_TRUE(ctx_string_valid("", sizeof ""));
}

TEST(Validate, InvalidCharacter)
{
	const char zStr[] = { 'H', 'i', 0xFF, '!', '\0' };

	EXPECT_EQ(ctx_string_validate(zStr, sizeof zStr), 2);
}

TEST(Validate, LackingNulTerminator)
{
	const char zStr[] = "Hello World!";

	ASSERT_TRUE(ctx_string_valid(zStr, sizeof zStr));

	EXPECT_EQ(ctx_string_validate(zStr, sizeof zStr - 1), ctxINT_MAX);
}

TEST(Validate, LastCharInvalid)
{
	const char zStr[] = { 'H', 'i', 0xFF };

	EXPECT_EQ(ctx_string_validate(zStr, sizeof zStr), 2);
}

TEST(Validate, InTheMiddle)
{
	const char zStr[] = "😄Hi!";

	EXPECT_EQ(ctx_string_validate(zStr + 1, sizeof zStr - 1), 0);
}

TEST(Validate, PrematureNul)
{
	const char zStr[] = { 'H', 'i', '\0', 0xFF };

	EXPECT_TRUE(ctx_string_valid(zStr, sizeof zStr));

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
	ASSERT_EQ(*it, '\0');

	it = ctx_string_next(zStr, sizeof zStr, it);
	ASSERT_EQ(it, nullptr);
}

TEST(Next, Empty)
{
	const char zStr[] = "";

	EXPECT_EQ(ctx_string_next(zStr, sizeof zStr, zStr), nullptr);
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

TEST(Next, OneInvalid)
{
	const char zStr[] = { 0xFF };

	EXPECT_EQ(ctx_string_next(zStr, sizeof zStr, zStr), zStr);

}

TEST(Next, OnlyInvalidCharacters)
{
	const char zStr[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, '\0' };
	
	EXPECT_EQ(*ctx_string_next(zStr, sizeof zStr, zStr), '\0');
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
	ASSERT_EQ(*it, 'i');

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

TEST(Skip, Null)
{
	EXPECT_EQ(ctx_string_skip(nullptr, 0, nullptr, 2), nullptr);
}

TEST(Skip, Empty)
{
	const char zStr[] = "";

	EXPECT_EQ(ctx_string_skip(zStr, sizeof zStr, zStr, 2), nullptr);
}

TEST(Skip, Everything)
{
	const char zStr[] = "Hello World";

	EXPECT_EQ(ctx_string_skip(zStr, sizeof zStr, zStr, 12), nullptr);
}

TEST(Skip, InvalidCharacters)
{
	const char zStr[] = {'H', 0xFF, 0xFF, 0xFF, 'e', 0xFF, 'l', 'l', 'o', '\0'};

	EXPECT_EQ(*ctx_string_skip(zStr, sizeof zStr, zStr, 4), 'o');
}

TEST(Skip, Backwards)
{
	const char zStr[] = "Hi!😄";

	const char* it = zStr + sizeof zStr - 1;
	ASSERT_EQ(*it, '\0');

	it = ctx_string_skip(zStr, sizeof zStr, it, -1);
	ASSERT_EQ(*(int*)it, resolve_endian('😄'));

	it = ctx_string_skip(zStr, sizeof zStr, it, -1);
	ASSERT_EQ(*it, '!');

	it = ctx_string_skip(zStr, sizeof zStr, it, -1);
	ASSERT_EQ(*it, 'i');

	it = ctx_string_skip(zStr, sizeof zStr, it, -1);
	ASSERT_EQ(*it, 'H');

	// Keep in mind: it's not an error condition because iteration goes backwards
	ASSERT_EQ(it, zStr);

	it = ctx_string_skip(zStr, sizeof zStr, it, -1);
	ASSERT_EQ(it, nullptr);
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

	ASSERT_GT(ctx_string_length(zStr, sizeof zStr), 3);

	EXPECT_EQ(ctx_string_length_max(zStr, sizeof zStr, 3), 3);
}




}
