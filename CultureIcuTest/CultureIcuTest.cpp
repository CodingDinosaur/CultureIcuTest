// CultureIcuTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <vector>

// ICU Includes
#include "unicode/uloc.h"

int main()
{
	std::cout << "Press enter key to begin...";
	std::getchar();

	auto availableLocales = uloc_countAvailable();

	std::vector<std::string> locales;
	for (int i = 0; i < availableLocales; i++)
	{
		locales.push_back(std::string(uloc_getAvailable(i)));
	}
	
	for (auto& locale : locales)
	{
		std::cout << locale << std::endl;
	}
	
	std::cout << "Complete";
	std::getchar();
}
