#include "pch.h"
#include "CultureIcuTest.h"

int main()
{
	std::cout << "Press enter key to begin..." << std::endl;
	std::getchar();

	getAvailableCppApiTest();
	getAvailableCApiTest();

	std::cout << "Complete";
	std::getchar();
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
