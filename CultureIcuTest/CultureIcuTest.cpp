#include "pch.h"

// ICU Includes
#include "unicode/uloc.h"
#include "unicode/locid.h"

int main()
{
	std::cout << "Press enter key to begin..." << std::endl;
	std::getchar();

	get_available_cpp_api_test();
	get_available_c_api_test();

	std::cout << "Complete";
	std::getchar();
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
