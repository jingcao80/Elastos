
#include "LocaleData.h"
#include "CLocaleData.h"
#include "CLocale.h"
#include "ICUUtil.h"
#include "StringUtils.h"
#include "AutoLock.h"

using Elastos::Core::StringUtils;
using Elastos::Utility::CLocale;

namespace Libcore {
namespace ICU {

LocaleData::StaticInitializer::StaticInitializer()
{
    LocaleData::Get(CLocale::ROOT);
    LocaleData::Get(CLocale::US);
    LocaleData::Get(CLocale::GetDefault());
}

INIT_PROI_4 HashMap< String, AutoPtr<ILocaleData> > LocaleData::sLocaleDataCache;
INIT_PROI_4 Object LocaleData::sLocaleDataCacheLock;
INIT_PROI_4 LocaleData::StaticInitializer LocaleData::sInitializer;

LocaleData::LocaleData()
    : mAmPm(NULL)
    , mEras(NULL)
    , mLongMonthNames(NULL)
    , mShortMonthNames(NULL)
    , mTinyMonthNames(NULL)
    , mLongStandAloneMonthNames(NULL)
    , mShortStandAloneMonthNames(NULL)
    , mTinyStandAloneMonthNames(NULL)
    , mLongWeekdayNames(NULL)
    , mShortWeekdayNames(NULL)
    , mTinyWeekdayNames(NULL)
    , mLongStandAloneWeekdayNames(NULL)
    , mShortStandAloneWeekdayNames(NULL)
    , mTinyStandAloneWeekdayNames(NULL)
{
}

CAR_INTERFACE_IMPL(LocaleData, Object, ILocaleData)

AutoPtr<ILocale> LocaleData::MapInvalidAndNullLocales(
        /* [in] */ ILocale* locale)
{
    AutoPtr<ILocale> rev = locale;
    if (NULL == rev) {
        rev = CLocale::GetDefault();
        return rev;
    }

    String s;
    rev->ToLanguageTag(&s);
    if (s.Equals("und")) rev = CLocale::ROOT;

    return rev;
}

LocaleData::~LocaleData()
{
}

AutoPtr<ILocaleData> LocaleData::Get(
    /* [in] */ ILocale* _locale)
{
    AutoPtr<ILocale> locale = _locale;
    String tmp;
    locale->ToLanguageTag(&tmp);
    const String languageTag = tmp;
    synchronized(sLocaleDataCacheLock) {
        HashMap< String, AutoPtr<ILocaleData> >::Iterator it =
                sLocaleDataCache.Find(languageTag);
        if (it != sLocaleDataCache.End()) {
            return (ILocaleData*)it->mSecond.Get();
        }
    }

    AutoPtr<ILocaleData> newLocaleData = InitLocaleData(locale);
    synchronized(sLocaleDataCacheLock) {
        HashMap< String, AutoPtr<ILocaleData> >::Iterator it =
                sLocaleDataCache.Find(languageTag);
        if (it != sLocaleDataCache.End()) {
            return it->mSecond;
        }
        sLocaleDataCache[languageTag] = newLocaleData;
    }
    return newLocaleData;
}

String LocaleData::ToString()
{
    return String("LocaleData");
    // return Objects.toString(this);
}

ECode LocaleData::GetDateFormat(
    /* [in] */ DateFormat style,
    /* [out] */ String* format)
{
    VALIDATE_NOT_NULL(format);

    switch (style) {
    case DateFormat_SHORT:
        *format = mShortDateFormat;
        return NOERROR;
    case DateFormat_MEDIUM:
        *format = mMediumDateFormat;
        return NOERROR;
    case DateFormat_LONG:
        *format = mLongDateFormat;
        return NOERROR;
    case DateFormat_FULL:
        *format = mFullDateFormat;
        return NOERROR;
    }
    // throw new AssertionError();
    return E_ASSERTION_ERROR;
}

ECode LocaleData::GetTimeFormat(
    /* [in] */ DateFormat style,
    /* [out] */ String* format)
{
    VALIDATE_NOT_NULL(format);

    switch (style) {
    case DateFormat_SHORT:
        *format = mShortTimeFormat;
        return NOERROR;
    case DateFormat_MEDIUM:
        *format = mMediumTimeFormat;
        return NOERROR;
    case DateFormat_LONG:
        *format = mLongTimeFormat;
        return NOERROR;
    case DateFormat_FULL:
        *format = mFullTimeFormat;
        return NOERROR;
    }
    // throw new AssertionError();
    return E_ASSERTION_ERROR;
}

ECode LocaleData::GetFirstDayOfWeek(
    /* [out] */ IInteger32** day)
{
    VALIDATE_NOT_NULL(day);
    *day = mFirstDayOfWeek;
    REFCOUNT_ADD(*day);
    return NOERROR;
}

ECode LocaleData::GetMinimalDaysInFirstWeek(
    /* [out] */ IInteger32** days)
{
    VALIDATE_NOT_NULL(days);
    *days = mMinimalDaysInFirstWeek;
    REFCOUNT_ADD(*days);
    return NOERROR;
}

ECode LocaleData::GetAmPm(
    /* [out] */ ArrayOf<String>** amPm)
{
    VALIDATE_NOT_NULL(amPm);
    *amPm = mAmPm;
    REFCOUNT_ADD(*amPm)
    return NOERROR;
}

ECode LocaleData::GetEras(
    /* [out] */ ArrayOf<String>** eras)
{
    VALIDATE_NOT_NULL(eras);
    *eras = mEras;
    REFCOUNT_ADD(*eras)
    return NOERROR;
}

ECode LocaleData::GetLongMonthNames(
    /* [out] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names);
    *names = mLongMonthNames;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode LocaleData::GetShortMonthNames(
    /* [out] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names);
    *names = mShortMonthNames;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode LocaleData::GetLongStandAloneMonthNames(
    /* [out] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names);
    *names = mLongStandAloneMonthNames;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode LocaleData::GetTinyMonthNames(
    /* [out] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names);
    *names = mTinyMonthNames;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode LocaleData::GetShortStandAloneMonthNames(
    /* [out] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names);
    *names = mShortStandAloneMonthNames;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode LocaleData::GetTinyStandAloneMonthNames(
    /* [out] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names);
    *names = mTinyStandAloneMonthNames;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode LocaleData::GetLongWeekdayNames(
    /* [out] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names);
    *names = mLongWeekdayNames;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode LocaleData::GetShortWeekdayNames(
    /* [out] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names);
    *names = mShortWeekdayNames;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode LocaleData::GetTinyWeekdayNames(
    /* [out] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names);
    *names = mTinyWeekdayNames;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode LocaleData::GetLongStandAloneWeekdayNames(
    /* [out] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names);
    *names = mLongStandAloneWeekdayNames;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode LocaleData::GetShortStandAloneWeekdayNames(
    /* [out] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names);
    *names = mShortStandAloneWeekdayNames;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode LocaleData::GetTinyStandAloneWeekdayNames(
    /* [out] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names);
    *names = mTinyStandAloneWeekdayNames;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode LocaleData::GetYesterday(
    /* [out] */ String* yesterday)
{
    VALIDATE_NOT_NULL(yesterday);
    *yesterday = mYesterday;
    return NOERROR;
}

ECode LocaleData::GetToday(
    /* [out] */ String* today)
{
    VALIDATE_NOT_NULL(today);
    *today = mToday;
    return NOERROR;
}

ECode LocaleData::GetTomorrow(
    /* [out] */ String* tomorrow)
{
    VALIDATE_NOT_NULL(tomorrow);
    *tomorrow = mTomorrow;
    return NOERROR;
}

ECode LocaleData::GetZeroDigit(
    /* [out] */ Char32* zeroDigit)
{
    VALIDATE_NOT_NULL(zeroDigit);
    *zeroDigit = mZeroDigit;
    return NOERROR;
}

ECode LocaleData::GetDecimalSeparator(
    /* [out] */ Char32* decimalSeparator)
{
    VALIDATE_NOT_NULL(decimalSeparator);
    *decimalSeparator = mDecimalSeparator;
    return NOERROR;
}

ECode LocaleData::GetGroupingSeparator(
    /* [out] */ Char32* groupingSeparator)
{
    VALIDATE_NOT_NULL(groupingSeparator);
    *groupingSeparator = mGroupingSeparator;
    return NOERROR;
}

ECode LocaleData::GetPatternSeparator(
    /* [out] */ Char32* patternSeparator)
{
    VALIDATE_NOT_NULL(patternSeparator);
    *patternSeparator = mPatternSeparator;
    return NOERROR;
}

ECode LocaleData::GetPercent(
    /* [out] */ Char32* percent)
{
    VALIDATE_NOT_NULL(percent);
    *percent = mPercent;
    return NOERROR;
}

ECode LocaleData::GetPerMill(
    /* [out] */ Char32* perMill)
{
    VALIDATE_NOT_NULL(perMill);
    *perMill = mPerMill;
    return NOERROR;
}

ECode LocaleData::GetMonetarySeparator(
    /* [out] */ Char32* monetarySeparator)
{
    VALIDATE_NOT_NULL(monetarySeparator);
    *monetarySeparator = mMonetarySeparator;
    return NOERROR;
}

ECode LocaleData::GetMinusSign(
    /* [out] */ String* minusSign)
{
    VALIDATE_NOT_NULL(minusSign);
    *minusSign = mMinusSign;
    return NOERROR;
}

ECode LocaleData::GetExponentSeparator(
    /* [out] */ String* exponentSeparator)
{
    VALIDATE_NOT_NULL(exponentSeparator);
    *exponentSeparator = mExponentSeparator;
    return NOERROR;
}

ECode LocaleData::GetInfinity(
    /* [out] */ String* infinity)
{
    VALIDATE_NOT_NULL(infinity);
    *infinity = mInfinity;
    return NOERROR;
}

ECode LocaleData::GetNaN(
    /* [out] */ String* naN)
{
    VALIDATE_NOT_NULL(naN);
    *naN = mNaN;
    return NOERROR;
}

ECode LocaleData::GetCurrencySymbol(
    /* [out] */ String* currencySymbol)
{
    VALIDATE_NOT_NULL(currencySymbol);
    *currencySymbol = mCurrencySymbol;
    return NOERROR;
}

ECode LocaleData::GetInternationalCurrencySymbol(
    /* [out] */ String* internationalCurrencySymbol)
{
    VALIDATE_NOT_NULL(internationalCurrencySymbol);
    *internationalCurrencySymbol = mInternationalCurrencySymbol;
    return NOERROR;
}

ECode LocaleData::GetTimeFormat12(
    /* [out] */ String* format)
{
    VALIDATE_NOT_NULL(format);
    *format = mTimeFormat12;
    return NOERROR;
}

ECode LocaleData::GetTimeFormat24(
    /* [out] */ String* format)
{
    VALIDATE_NOT_NULL(format);
    *format = mTimeFormat24;
    return NOERROR;
}

ECode LocaleData::GetNumberPattern(
    /* [out] */ String* numberPattern)
{
    VALIDATE_NOT_NULL(numberPattern);
    *numberPattern = mNumberPattern;
    return NOERROR;
}

ECode LocaleData::GetIntegerPattern(
    /* [out] */ String* integerPattern)
{
    VALIDATE_NOT_NULL(integerPattern);
    *integerPattern = mIntegerPattern;
    return NOERROR;
}

ECode LocaleData::GetCurrencyPattern(
    /* [out] */ String* currencyPattern)
{
    VALIDATE_NOT_NULL(currencyPattern);
    *currencyPattern = mCurrencyPattern;
    return NOERROR;
}

ECode LocaleData::GetPercentPattern(
    /* [out] */ String* percentPattern)
{
    VALIDATE_NOT_NULL(percentPattern);
    *percentPattern = mPercentPattern;
    return NOERROR;
}

AutoPtr<ILocaleData> LocaleData::InitLocaleData(
    /* [in] */ ILocale* locale)
{
    AutoPtr<CLocaleData> localeObj;
    CLocaleData::NewByFriend((CLocaleData**)&localeObj);
    LocaleData* localeData = (LocaleData*)localeObj.Get();
    String localeLanguageTag;
    locale->ToLanguageTag(&localeLanguageTag);

    if (!ICUUtil::InitLocaleDataNative(localeLanguageTag, localeData)) {
        return NULL;
    }

    // Get the "h:mm a" and "HH:mm" 12- and 24-hour time format strings.
    ICUUtil::GetBestDateTimePattern(String("hm"), locale, &localeData->mTimeFormat12);
    ICUUtil::GetBestDateTimePattern(String("Hm"), locale, &localeData->mTimeFormat24);

    // Fix up a couple of patterns.
    if (!localeData->mFullTimeFormat.IsNull()) {
        // There are some full time format patterns in ICU that use the pattern character 'v'.
        // Java doesn't accept this, so we replace it with 'z' which has about the same result
        // as 'v', the timezone name.
        // 'v' -> "PT", 'z' -> "PST", v is the generic timezone and z the standard tz
        // "vvvv" -> "Pacific Time", "zzzz" -> "Pacific Standard Time"
        localeData->mFullTimeFormat = (localeData->mFullTimeFormat).Replace('v', 'z');
    }
    if (!localeData->mNumberPattern.IsNull()) {
        // The number pattern might contain positive and negative subpatterns. Arabic, for
        // example, might look like "#,##0.###;#,##0.###-" because the minus sign should be
        // written last. Macedonian supposedly looks something like "#,##0.###;(#,##0.###)".
        // (The negative subpattern is optional, though, and not present in most locales.)
        // By only swallowing '#'es and ','s after the '.', we ensure that we don't
        // accidentally eat too much.
        // localeData.integerPattern = localeData.numberPattern.replaceAll("\\.[#,]*", "");
        StringUtils::ReplaceAll(localeData->mNumberPattern, String("\\.[#,]*"), String("") ,
            &localeData->mIntegerPattern);
    }
    StringUtils::ReplaceAll(localeData->mShortDateFormat, String("\\byy\\b"), String("y"),
        &localeData->mShortDateFormat4);
    return (ILocaleData*)localeObj.Get();
}

} // namespace ICU
} // namespace Libcore
