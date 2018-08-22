# Demonstration of ICU C/C++ API to Obtain a List of Available Locales

This repo is a sample for dotnet/coreclr issue here: (TBD)
See also, the main demonstration repo here: (TBD)

This repo contains C/C++ code that utilizes the ICU C & C++ APIs to show that ICU does not return aliases when requesting a list of available locales.

- [Issue Description](#issue-description)
- [Issue Cause Summarized](#issue-cause-summarized)
    - [CultureInfo & The Culture List](#cultureinfo--the-culture-list)
    - [ICU's Data Source](#icus-data-source)
    - [ICU Locale Aliases](#icu-locale-aliases)
        - [ICU Locale Alias List](#icu-locale-alias-list)
- [Running the Test Application](#running-the-test-application)

# Issue Description
Certain valid locales cannot be used for localization in .NET Core on Unix-based environments, because they are not recognized by CultureInfo and its surrounding classes.  Although not the only affected locale, this is most easily reproduced with zh-TW (Chinese, Taiwan).

This affects any locale which is an "aliased" locale in ICU [ICU (International Components for Unicode)](http://site.icu-project.org/home).

For a more thorough discussion of the issue, check the main demo repo here: (TBD)

# Issue Cause Summarized
To summarize what appears to be the cause of the issue:
- Microsoft's mscorlib in dotnet/coreclr uses the 3rd party library [ICU (International Components for Unicode)](http://site.icu-project.org/home) to support non-Windows, platform agnostic localization features such as knowing if a given locale is valid and details about the locale (collation, numeric formatting, etc).
  -  ICU is only used in non-Windows runtimes of .NET Core
- To save on data space while providing a robust parenting algorithm, ICU defines certain locales as "aliases" of others.
- ICU aliases are intentionally not returned by ICU when requesting a list of locales.
- Some Culture concerns in mscorlib currently depend on getting an up-front list of all available cultures.
- **`CultureData.EnumCultures`, which in turn via native interop calls the ICU C API `uloc_getAvailable`, therefore fails to obtain any cultures which are defined as aliases in ICU, and these cultures become invalid or are populated with incorrect data in various contexts.**

Further details can be found in the main demo repo here: (TBD)

## CultureInfo & The Culture List

In .NET Core, all roads lead to the CultureData class in [dotnet/coreclr](https://github.com/dotnet/coreclr).

When calling CultureInfo.GetCultures:

[CultureInfo.GetCultures](https://github.com/dotnet/coreclr/blob/master/src/System.Private.CoreLib/src/System/Globalization/CultureInfo.cs#L536-L545) ->
[CultureData.GetCultures](https://github.com/dotnet/coreclr/blob/master/src/System.Private.CoreLib/shared/System/Globalization/CultureData.cs#L407-L440) ->
[CultureData(Unix).EnumCultures](https://github.com/dotnet/coreclr/blob/master/src/System.Private.CoreLib/shared/System/Globalization/CultureData.Unix.cs#L359-L408) ->
[System.Globalization.Native/locale.cpp:GlobalizationNative_GetLocales](https://github.com/dotnet/coreclr/blob/master/src/corefx/System.Globalization.Native/locale.cpp#L158-L199)

Ultimately ending here in *System.Globalization.Native/locale.cpp*:
```cpp
    int32_t localeCount = uloc_countAvailable();
    
    if (localeCount <=  0)
        return -1; // failed
    
    for (int32_t i = 0; i < localeCount; i++)
    {
        const char *pLocaleName = uloc_getAvailable(i);
        if (pLocaleName[0] == 0) // unexpected empty name
            return -2;
```

This is when the stack enters the C API for the [ICU - the International Components for Unicode](http://site.icu-project.org/home) library.

## ICU's Data Source

The *uloc_countAvailable* and *uloc_getAvailable* calls are part of the ICU C API.  (Mostly functionally equivalent calls exist in the C++ API in the Locale class, however in my testing the exhibit the same behavior as the C API calls.)

We can directly test that ICU isn't returning zh-TW in both the C API method used above and the C++ API, as I have done in this repo.

To better understand why ICU isn't returning zh_TW and some other locales, we need to better understand how ICU's data works.

ICU's data comes from the [CLDR - Unicode Common Locale Data Repository](http://cldr.unicode.org/).  This is part of what makes ICU attractive -- it is intentionally completely platform agnostic and does not depend on the hosting environment for locale information in any way.  [From the ICU FAQ](http://userguide.icu-project.org/icufaq#TOC-What-is-the-relationship-between-ICU-locale-data-and-system-locale-data-):

>**What is the relationship between ICU locale data and system locale data?**<br/>There is no relationship. ICU is not dependent on the operating system for the locale data.<br/>This also means that uloc_setDefault() does not affect the operating system. The function uloc_setDefault() only sets ICU's default locale. Normally the default locale for ICU is whatever the operating system says is the default locale.

For that reason, ICU includes all of its own data from CLDR.  However, database size has been an ongoing concern, and some locales which shared the same collation data but didn't have a parent / child relationship meant that said collation data would be duplicated.  To de-duplicate this data, ICU added the concept of locale "aliases".  [From ICU's documentation on ICU resource bundles](http://userguide.icu-project.org/locale/localizing#TOC-.txt-resource-bundles):

>A value can also be an "alias", which is simply a reference to another bundle's item. This is to save space by storing large data pieces only once when they cannot be inherited along the locale ID hierarchy (e.g., collation data in ICU shared among zh_HK and zh_TW).

## ICU Locale Aliases
When a locale name is defined as an alias in ICU, then from the standpoint of ICU it isn't a first-class locale -- merely a pointer to a "real" locale when requested.  As a result, **ICU does not return aliases when getting a list of locales** -- whether with `uloc_getAvailable` or `Locale::getAvailableLocales` (and `uloc_countAvailable` does not include them in its count).

That ICU does not return the aliases in this manner **appears to be intentional**, both based on the numerous references to [a lack of alias mapping in the uloc documentation](http://www.icu-project.org/apiref/icu4c/uloc_8h.html), and the following bug:  

https://unicode-org.atlassian.net/browse/ICU-4309
> uloc_getAvailable returns sr_YU, even though it is an %%ALIAS locale. None of the other %%ALIAS locales are returned.

> TracBot made changes - 01/Jul/18 1:59 PM<br/>Resolution		Fixed [ 10004 ]<br/>Status	Done [ 10002 ]	Done [ 10002 ]

**That this bug was fixed is a further and very strong indication that ICU not returning locale aliases is intentional.**

### ICU Locale Alias List
These are all the locales that have aliases for the purposes of locale identification.  Other data types in ICU (such as collation and rule-based number formatting) have additional aliases.

|Locale|Aliases|
---|---
ar_SA|ars
az_Latn_AZ|az_AZ
bs_Latn_BA|bs_BA
en_VU|en_NH
en_ZW|en_RH
fil|tl
fil_PH|tl_PH
he|iw
he_IL|iw_IL
id|in
id_ID|in_ID
nb|no
nb_NO|no_NO
nn_NO|no_NO_NY
pa_Arab_PK|pa_PK
pa_Guru_IN|pa_IN
ro_MD|mo
shi_Tfng_MA|shi_MA
sr_Cyrl_BA|sr_BA
sr_Cyrl_RS|sr_CS, sr_Cyrl_CS, sr_Cyrl_YU, sr_RS, sr_YU
sr_Cyrl_XK|sr_XK
sr_Latn|sh
sr_Latn_BA|sh_BA
sr_Latn_ME|sr_ME
sr_Latn_RS|sh_CS, sh_YU, sr_Latn_CS, sr_Latn_YU
uz_Arab_AF|uz_AF
uz_Latn_UZ|uz_UZ
vai_Vaii_LR|vai_LR
yue_Hans_CN|yue_CN
yue_Hant_HK|yue_HK
zh_Hans_CN|zh_CN
zh_Hans_SG|zh_SG
zh_Hant_HK|zh_HK
zh_Hant_MO|zh_MO
zh_Hant_TW|zh_TW

# Running the Test Application
To run the test, you will need:
- Visual Studio with the C++ workflow installed

Or, obtain the non-Windows ICU binaries and re-compile in your desired environment.

Just build & run.  The results are very simple -- using both the C and C++ ICU APIs, the documented methods to obtain a list of locales are executed, of which the method involving the C API is the same used by .NET Core's mscorlib (dotnet/coreclr).  For both the list is filtered to only Chinese language locales (starting with *zh*) and the resulting list is displayed.

As you can see, zh_CN, zh_TW, zh_SG, zh_HK, and zh_MO are all missing -- and as discussed above, according to ICU's documentation and bug history, this does appear to be intentional.

The purpose of this test app, then, is to provide verification that this is the case, and correcting the overall issue may need to come from the dotnet/coreclr side.
