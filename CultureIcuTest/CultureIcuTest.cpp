#include "pch.h"
#include "GetLocaleTest.h"
#include "CultureIcuTest.h"

int main()
{
	std::cout << "Press enter key to begin..." << std::endl;
	std::getchar();

	getAvailableCppApiTest();
	getAvailableCApiTest();
	getNameCApiTest();
	getLocaleCoreClrStyleTest();

	std::cout << "Complete";
	std::getchar();
}

void getNameCApiTest()
{
	std::cout << "ICU C API GetName / Get Language Test" << std::endl;
	
	const auto localeId = "zh-TW";
	const auto localeNameResult = new char[50];
	const auto localeNameResultLength = INT32_MAX;
	auto err = U_ZERO_ERROR;

	const auto localeLength = uloc_getName(localeId, localeNameResult, localeNameResultLength, &err);
	std::cout << "uloc_getName Result: " << localeNameResult << std::endl;

	const auto lang = uloc_getLanguage(localeId, localeNameResult, localeNameResultLength, &err);
	std::cout << "uloc_getLanguage Result: " << localeNameResult << std::endl;
	
}

void getLocaleCoreClrStyleTest()
{
	std::cout << std::endl << "CoreCLR GlobalizationNative_GetLocaleName Reproduction Test" << std::endl;

	UChar ln[ULOC_FULLNAME_CAPACITY] = { 0 };
	auto name = std::wstring(L"zh-TW");
	name.copy(ln, 5);

	UChar buffer[ULOC_FULLNAME_CAPACITY] = { 0 };
	const auto len = int32_t(ULOC_FULLNAME_CAPACITY);
	GetLocaleTest::GlobalizationNative_GetLocaleName(ln, buffer, len);

	std::wcout << buffer << std::endl;
}

void getAvailableCApiTest()
{
	const auto count = uloc_countAvailable();
	std::cout << "ICU C API Test - Locale Count: " << count << std::endl;
	std::cout << "(This is how coreclr does it)" << std::endl;

	std::vector<std::string> allLocaleNames;
	for (auto i = 0; i < count; i++)
	{
		const auto localeName = std::string(uloc_getAvailable(i));
		allLocaleNames.push_back(localeName);
	}

	outputZhLocales(allLocaleNames);
}

void getAvailableCppApiTest()
{
	int32_t count;
	auto locale = Locale::getAvailableLocales(count);
	
	std::cout << "ICU C++ API Test - Locale Count: " << count << std::endl;

	std::vector<std::string> allLocaleNames;
	for (auto i = 0; i < count; i++, locale++)
	{
		allLocaleNames.push_back(locale->getName());
	}

	outputZhLocales(allLocaleNames);
}

void outputZhLocales(const std::vector<std::string>& all_locale_names)
{
	std::vector<std::string> zhLocales;
	for (auto& localeName : all_locale_names)
	{
		if (localeName.substr(0, 2) == "zh")
		{
			zhLocales.push_back(localeName);
		}
	}

	std::cout << "Chinese locales: ";
	if (zhLocales.begin() == zhLocales.end())
	{
		std::cout << "None";
	}
	else
	{
		std::cout << std::endl;
		for (auto& locale : zhLocales)
		{
			std::cout << locale << std::endl;
		}
		std::cout << std::endl;
	}	
}
