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

#ifndef __ELASTOS_UTILITY_CLOCALEHELPER_H__
#define __ELASTOS_UTILITY_CLOCALEHELPER_H__

#include "_Elastos_Utility_CLocaleHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Utility {

CarClass(CLocaleHelper)
    , public Singleton
    , public ILocaleHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns a locale for a given BCP-47 language tag. This method is more
     * lenient than {@link Builder#setLanguageTag}. For a given language tag, parsing
     * will proceed up to the first malformed subtag. All subsequent tags are discarded.
     * Note that language tags use {@code -} rather than {@code _}, for example {@code en-US}.
     *
     * @throws NullPointerException if {@code languageTag} is {@code null}.
     *
     * @since 1.7
     */
    CARAPI ForLanguageTag(
        /* [in] */ const String& languageTag,
        /* [out] */ ILocale** locale);

    /**
     * Returns the system's installed locales. This array always includes {@code
     * Locale.US}, and usually several others. Most locale-sensitive classes
     * offer their own {@code getAvailableLocales} method, which should be
     * preferred over this general purpose method.
     *
     * @see java.text.BreakIterator#getAvailableLocales()
     * @see java.text.Collator#getAvailableLocales()
     * @see java.text.DateFormat#getAvailableLocales()
     * @see java.text.DateFormatSymbols#getAvailableLocales()
     * @see java.text.DecimalFormatSymbols#getAvailableLocales()
     * @see java.text.NumberFormat#getAvailableLocales()
     * @see java.util.Calendar#getAvailableLocales()
     */
    CARAPI GetAvailableLocales(
        /* [out] */ ArrayOf<ILocale*>** locales);

    /**
     * Returns the user's preferred locale. This may have been overridden for
     * this process with {@link #setDefault}.
     *
     * <p>Since the user's locale changes dynamically, avoid caching this value.
     * Instead, use this method to look it up for each use.
     */
    CARAPI GetDefault(
        /* [out] */ ILocale** defaultLocale);

    CARAPI GetLocale(
        /* [in] */ const String& localeName,
        /* [out] */ ILocale** locale);

    /**
     * Gets the list of two letter ISO country codes which can be used as the
     * country code for a {@code Locale}.
     *
     * @return an array of strings.
     */
    CARAPI GetISOCountries(
        /* [out] */ ArrayOf<String>** codes);

    /**
     * Gets the list of two letter ISO language codes which can be used as the
     * language code for a {@code Locale}.
     *
     * @return an array of strings.
     */
    CARAPI GetISOLanguages(
        /* [out] */ ArrayOf<String>** codes);

    /**
     * Overrides the default locale. This does not affect system configuration,
     * and attempts to override the system-provided default locale may
     * themselves be overridden by actual changes to the system configuration.
     * Code that calls this method is usually incorrect, and should be fixed by
     * passing the appropriate locale to each locale-sensitive method that's
     * called.
     */
    CARAPI SetDefault(
        /* [in] */ ILocale* locale);

    /**
     * Locale constant for en_CA.
     */
    CARAPI GetCANADA(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for fr_CA.
     */
    CARAPI GetCANADA_FRENCH(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for zh_CN.
     */
    CARAPI GetCHINA(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for zh.
     */
    CARAPI GetCHINESE(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for en.
     */
    CARAPI GetENGLISH(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for fr_FR.
     */
    CARAPI GetFRANCE(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for fr.
     */
    CARAPI GetFRENCH(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for de.
     */
    CARAPI GetGERMAN(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for de_DE.
     */
    CARAPI GetGERMANY(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for it.
     */
    CARAPI GetITALIAN(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for it_IT.
     */
    CARAPI GetITALY(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for ja_JP.
     */
    CARAPI GetJAPAN(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for ja.
     */
    CARAPI GetJAPANESE(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for ko_KR.
     */
    CARAPI GetKOREA(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for ko.
     */
    CARAPI GetKOREAN(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for zh_CN.
     */
    CARAPI GetPRC(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for the root locale. The root locale has an empty language,
     * country, and variant.
     *
     * @since 1.6
     */
    CARAPI GetROOT(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for zh_CN.
     */
    CARAPI GetSIMPLIFIED_CHINESE(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for zh_TW.
     */
    CARAPI GetTAIWAN(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for zh_TW.
     */
    CARAPI GetTRADITIONAL_CHINESE(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for en_GB.
     */
    CARAPI GetUK(
        /* [out] */ ILocale** locale);

    /**
     * Locale constant for en_US.
     */
    CARAPI GetUS(
        /* [out] */ ILocale** locale);

    /**
     * @hide for internal use only.
     */
    CARAPI AdjustLanguageCode(
        /* [in] */ const String& languageCode,
        /* [out] */ String* result);
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CLOCALEHELPER_H__
