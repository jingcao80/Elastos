//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "CLocaleHelper.h"
#include "CLocale.h"

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL(CLocaleHelper, Singleton, ILocaleHelper)

CAR_SINGLETON_IMPL(CLocaleHelper)

ECode CLocaleHelper::ForLanguageTag(
    /* [in] */ const String& languageTag,
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);

    return Locale::ForLanguageTag(languageTag, locale);
}

ECode CLocaleHelper::GetAvailableLocales(
    /* [out] */ ArrayOf<ILocale*>** locales)
{
    VALIDATE_NOT_NULL(locales);

    return Locale::GetAvailableLocales(locales);
}

ECode CLocaleHelper::GetDefault(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);

    AutoPtr<ILocale> l = CLocale::GetDefault();
    *locale = l;
    REFCOUNT_ADD(*locale);

    return NOERROR;
}

ECode CLocaleHelper::GetLocale(
    /* [in] */ const String& localeName,
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);

    AutoPtr<ILocale> temp;
    if (localeName.Equals("CANADA")) {
        temp = CLocale::CANADA;
    }
    else if (localeName.Equals("CANADA_FRENCH")) {
        temp = CLocale::CANADA_FRENCH;
    }
    else if (localeName.Equals("CHINA")) {
        temp = CLocale::CHINA;
    }
    else if (localeName.Equals("CHINESE")) {
        temp = CLocale::CHINESE;
    }
    else if (localeName.Equals("ENGLISH")) {
        temp = CLocale::ENGLISH;
    }
    else if (localeName.Equals("FRANCE")) {
        temp = CLocale::FRANCE;
    }
    else if (localeName.Equals("GERMAN")) {
        temp = CLocale::GERMAN;
    }
    else if (localeName.Equals("ITALIAN")) {
        temp = CLocale::ITALIAN;
    }
    else if (localeName.Equals("ITALY")) {
        temp = CLocale::ITALY;
    }
    else if (localeName.Equals("JAPAN")) {
        temp = CLocale::JAPAN;
    }
    else if (localeName.Equals("JAPANESE")) {
        temp = CLocale::JAPANESE;
    }
    else if (localeName.Equals("KOREA")) {
        temp = CLocale::KOREA;
    }
    else if (localeName.Equals("KOREAN")) {
        temp = CLocale::KOREAN;
    }
    else if (localeName.Equals("PRC")) {
        temp = CLocale::PRC;
    }
    else if (localeName.Equals("SIMPLIFIED_CHINESE")) {
        temp = CLocale::SIMPLIFIED_CHINESE;
    }
    else if (localeName.Equals("TAIWAN")) {
        temp = CLocale::TAIWAN;
    }
    else if (localeName.Equals("TRADITIONAL_CHINESE")) {
        temp = CLocale::TRADITIONAL_CHINESE;
    }
    else if (localeName.Equals("UK")) {
        temp = CLocale::UK;
    }
    else if (localeName.Equals("US")) {
        temp = CLocale::US;
    }
    *locale = temp;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}

ECode CLocaleHelper::GetISOCountries(
    /* [out] */ ArrayOf<String>** codes)
{
    return CLocale::GetISOCountries(codes);
}

ECode CLocaleHelper::GetISOLanguages(
    /* [out] */ ArrayOf<String>** codes)
{
    return CLocale::GetISOLanguages(codes);
}

ECode CLocaleHelper::SetDefault(
    /* [in] */ ILocale* locale)
{
    return CLocale::SetDefault(locale);
}

/**
 * Locale constant for en_CA.
 */
ECode CLocaleHelper::GetCANADA(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);
    *locale = CLocale::CANADA;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}

/**
 * Locale constant for fr_CA.
 */
ECode CLocaleHelper::GetCANADA_FRENCH(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);
    *locale = CLocale::CANADA_FRENCH;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for zh_CN.
 */
ECode CLocaleHelper::GetCHINA(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::CHINA;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for zh.
 */
ECode CLocaleHelper::GetCHINESE(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::CHINESE;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for en.
 */
ECode CLocaleHelper::GetENGLISH(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::ENGLISH;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for fr_FR.
 */
ECode CLocaleHelper::GetFRANCE(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::FRANCE;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for fr.
 */
ECode CLocaleHelper::GetFRENCH(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::FRENCH;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for de.
 */
ECode CLocaleHelper::GetGERMAN(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::GERMAN;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for de_DE.
 */
ECode CLocaleHelper::GetGERMANY(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::GERMANY;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for it.
 */
ECode CLocaleHelper::GetITALIAN(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::ITALIAN;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for it_IT.
 */
ECode CLocaleHelper::GetITALY(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::ITALY;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for ja_JP.
 */
ECode CLocaleHelper::GetJAPAN(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::JAPAN;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for ja.
 */
ECode CLocaleHelper::GetJAPANESE(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::JAPANESE;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for ko_KR.
 */
ECode CLocaleHelper::GetKOREA(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::KOREA;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for ko.
 */
ECode CLocaleHelper::GetKOREAN(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::KOREAN;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for zh_CN.
 */
ECode CLocaleHelper::GetPRC(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::PRC;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for the root locale. The root locale has an empty language,
 * country, and variant.
 *
 * @since 1.6
 */
ECode CLocaleHelper::GetROOT(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::ROOT;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for zh_CN.
 */
ECode CLocaleHelper::GetSIMPLIFIED_CHINESE(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::SIMPLIFIED_CHINESE;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for zh_TW.
 */
ECode CLocaleHelper::GetTAIWAN(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::TAIWAN;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for zh_TW.
 */
ECode CLocaleHelper::GetTRADITIONAL_CHINESE(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::TRADITIONAL_CHINESE;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for en_GB.
 */
ECode CLocaleHelper::GetUK(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::UK;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}
/**
 * Locale constant for en_US.
 */
ECode CLocaleHelper::GetUS(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = CLocale::US;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}

ECode CLocaleHelper::AdjustLanguageCode(
    /* [in] */ const String& languageCode,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = CLocale::AdjustLanguageCode(languageCode);
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
