#pragma once
class GetLocaleTest
{
public:
	GetLocaleTest();
	~GetLocaleTest();
	// ReSharper disable CppInconsistentNaming
	static int32_t GlobalizationNative_GetLocaleName(const UChar* localeName, UChar* value, int32_t valueLength);
private:
	static UErrorCode u_charsToUChars_safe(const char* str, UChar* value, int32_t valueLength);
	static int32_t UErrorCodeToBool(UErrorCode status);
	static int32_t FixupLocaleName(UChar* value, int32_t valueLength);
	static int32_t GetLocale(const UChar* localeName, char* localeNameResult, int32_t localeNameResultLength, bool canonicalize, UErrorCode* err);
	// ReSharper restore CppInconsistentNaming
};

