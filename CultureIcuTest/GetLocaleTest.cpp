#include "pch.h"
#include "GetLocaleTest.h"

GetLocaleTest::GetLocaleTest() = default;
GetLocaleTest::~GetLocaleTest() = default;

// ReSharper disable CppInconsistentNaming
extern "C" int32_t GetLocaleTest::GlobalizationNative_GetLocaleName(const UChar* localeName, UChar* value, int32_t valueLength)
{
	UErrorCode status = U_ZERO_ERROR;

	char localeNameBuffer[ULOC_FULLNAME_CAPACITY];
	GetLocale(localeName, localeNameBuffer, ULOC_FULLNAME_CAPACITY, true, &status);

	if (U_SUCCESS(status))
	{
		status = u_charsToUChars_safe(localeNameBuffer, value, valueLength);

		if (U_SUCCESS(status))
		{
			FixupLocaleName(value, valueLength);
		}
	}

	return UErrorCodeToBool(status);
}

int32_t GetLocaleTest::GetLocale(
	const UChar* localeName, char* localeNameResult, int32_t localeNameResultLength, bool canonicalize, UErrorCode* err)
{
	char localeNameTemp[ULOC_FULLNAME_CAPACITY] = { 0 };
	int32_t localeLength;

	// Convert ourselves instead of doing u_UCharsToChars as that function considers '@' a variant and stops.
	for (int i = 0; i < ULOC_FULLNAME_CAPACITY - 1; i++)
	{
		UChar c = localeName[i];

		if (c > (UChar)0x7F)
		{
			*err = U_ILLEGAL_ARGUMENT_ERROR;
			return ULOC_FULLNAME_CAPACITY;
		}

		localeNameTemp[i] = (char) c;

		if (c == (UChar) 0x0)
		{
			break;
		}
	}

	if (canonicalize)
	{
		localeLength = uloc_canonicalize(localeNameTemp, localeNameResult, localeNameResultLength, err);
	}
	else
	{
		localeLength = uloc_getName(localeNameTemp, localeNameResult, localeNameResultLength, err);
	}

	if (U_SUCCESS(*err))
	{
		// Make sure the "language" part of the locale is reasonable (i.e. we can fetch it and it is within range).
		// This mimics how the C++ ICU API determines if a locale is "bogus" or not.

		char language[ULOC_LANG_CAPACITY];
		uloc_getLanguage(localeNameTemp, language, ULOC_LANG_CAPACITY, err);

		if (*err == U_BUFFER_OVERFLOW_ERROR || *err == U_STRING_NOT_TERMINATED_WARNING)
		{
			// ULOC_LANG_CAPACITY includes the null terminator, so if we couldn't extract the language with the null
			// terminator, the language must be invalid.

			*err = U_ILLEGAL_ARGUMENT_ERROR;
		}
	}

	return localeLength;
}


int32_t GetLocaleTest::FixupLocaleName(UChar* value, int32_t valueLength)
{
	int32_t i = 0;
	for (; i < valueLength; i++)
	{
		if (value[i] == (UChar)'\0')
		{
			break;
		}
		else if (value[i] == (UChar)'_')
		{
			value[i] = (UChar)'-';
		}
	}

	return i;
}

int32_t GetLocaleTest::UErrorCodeToBool(UErrorCode status)
{
	if (U_SUCCESS(status))
	{
		return 1;
	}

	// add possible SetLastError support here

	return 0;
}

UErrorCode GetLocaleTest::u_charsToUChars_safe(const char* str, UChar* value, int32_t valueLength)
{
	int len = strlen(str);

	if (len >= valueLength)
	{
		return U_BUFFER_OVERFLOW_ERROR;
	}

	u_charsToUChars(str, value, len + 1);
	return U_ZERO_ERROR;
}

// ReSharper restore CppInconsistentNaming

