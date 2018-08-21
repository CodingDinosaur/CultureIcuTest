#include "pch.h"
#include "GetLocaleTest.h"
#include "CultureIcuTest.h"

void get_name_c_api_test()
{
	const auto locale_id = "zh-TW";
	char* locale_name_result = new char[50];
	const auto locale_name_result_length = INT32_MAX;
	auto err = U_ZERO_ERROR;
	//const auto locale_length = uloc_canonicalize(locale_id, locale_name_result, locale_name_result_length, &err);

	const auto locale_length = uloc_getName(locale_id, locale_name_result, locale_name_result_length, &err);

	const auto lang = uloc_getLanguage(locale_id, locale_name_result, locale_name_result_length, &err);

	std::cout << locale_length << std::endl;
	std::cout << locale_name_result << std::endl;
	std::cout << lang << std::endl;
}

void get_available_c_api_test()
{
	const auto count = uloc_countAvailable();
	std::cout << "ICU C API Test - Locale Count: " << count << std::endl;
	std::cout << "(This is how coreclr does it)" << std::endl;

	std::vector<std::string> all_locale_names;
	for (auto i = 0; i < count; i++)
	{
		const auto locale_name = std::string(uloc_getAvailable(i));
		all_locale_names.push_back(locale_name);
	}

	output_zh_locales(all_locale_names);
}

void get_available_cpp_api_test()
{
	int32_t count;
	auto locale = Locale::getAvailableLocales(count);
	
	std::cout << "ICU C++ API Test - Locale Count: " << count << std::endl;

	std::vector<std::string> all_locale_names;
	for (auto i = 0; i < count; i++, locale++)
	{
		all_locale_names.push_back(locale->getName());
	}

	output_zh_locales(all_locale_names);
}

void output_zh_locales(const std::vector<std::string>& all_locale_names)
{
	std::vector<std::string> zh_locales;
	for (auto& locale_name : all_locale_names)
	{
		if (locale_name.substr(0, 2) == "zh")
		{
			zh_locales.push_back(locale_name);
		}
	}

	std::cout << "Chinese locales: ";
	if (zh_locales.begin() == zh_locales.end())
	{
		std::cout << "None";
	}
	else
	{
		std::cout << std::endl;
		for (auto& locale : zh_locales)
		{
			std::cout << locale << std::endl;
		}
		std::cout << std::endl;
	}	
}

void getLocaleCoreClrStyleTest()
{
	std::cout << "CoreCLR GlobalizationNative_GetLocaleName Reproduction Test" << std::endl;
	
	UChar ln[ULOC_FULLNAME_CAPACITY] = { 0 };
	auto name = std::wstring(L"zh-TW");
	name.copy(ln, 5);

	UChar buffer[ULOC_FULLNAME_CAPACITY] = { 0 };
	const auto len = int32_t(ULOC_FULLNAME_CAPACITY);
	GetLocaleTest::GlobalizationNative_GetLocaleName(ln, buffer, len);

	std::wcout << buffer << std::endl;
}

int main()
{
	std::cout << "Press enter key to begin..." << std::endl;
	std::getchar();

	get_available_cpp_api_test();
	get_available_c_api_test();
	get_name_c_api_test();
	getLocaleCoreClrStyleTest();


		std::cout << "Complete";
	std::getchar();
}
