#ifndef __LIBCORE_ICU_IO_LOCALEDATA_H__
#define __LIBCORE_ICU_IO_LOCALEDATA_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include <Object.h>
#include <elastos/utility/etl/HashMap.h>
#include <Mutex.h>
#include "CInteger32.h"

using Elastos::Core::Object;
using Elastos::Core::Mutex;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::ILocale;

namespace Libcore {
namespace ICU {

/**
 * Passes locale-specific from ICU native code to Java.
 * <p>
 * Note that you share these; you must not alter any of the fields, nor their array elements
 * in the case of arrays. If you ever expose any of these things to user code, you must give
 * them a clone rather than the original.
 */
class LocaleData
    : public Object
    , public ILocaleData
{
private:
    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    CAR_INTERFACE_DECL()

    LocaleData();

    ~LocaleData();

    static CARAPI_(AutoPtr<ILocale>) MapInvalidAndNullLocales(
        /* [in] */ ILocale* locale);

    static CARAPI_(AutoPtr<ILocaleData>) Get(
        /* [in] */ ILocale* locale);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetDateFormat(
        /* [in] */ DateFormat style,
        /* [out] */ String* format);

    CARAPI GetTimeFormat(
        /* [in] */ DateFormat style,
        /* [out] */ String* format);

    CARAPI GetFirstDayOfWeek(
        /* [out] */ IInteger32** day);

    CARAPI GetMinimalDaysInFirstWeek(
        /* [out] */ IInteger32** days);

    CARAPI GetAmPm(
        /* [out] */ ArrayOf<String>** amPm);

    CARAPI GetEras(
        /* [out] */ ArrayOf<String>** eras);

    CARAPI GetLongMonthNames(
        /* [out] */ ArrayOf<String>** names);

    CARAPI GetShortMonthNames(
        /* [out] */ ArrayOf<String>** names);

    CARAPI GetTinyMonthNames(
        /* [out] */ ArrayOf<String>** names);

    CARAPI GetLongStandAloneMonthNames(
        /* [out] */ ArrayOf<String>** names);

    CARAPI GetShortStandAloneMonthNames(
        /* [out] */ ArrayOf<String>** names);

    CARAPI GetTinyStandAloneMonthNames(
        /* [out] */ ArrayOf<String>** names);

    CARAPI GetLongWeekdayNames(
        /* [out] */ ArrayOf<String>** names);

    CARAPI GetShortWeekdayNames(
        /* [out] */ ArrayOf<String>** names);

    CARAPI GetTinyWeekdayNames(
        /* [out] */ ArrayOf<String>** names);

    CARAPI GetLongStandAloneWeekdayNames(
        /* [out] */ ArrayOf<String>** names);

    CARAPI GetShortStandAloneWeekdayNames(
        /* [out] */ ArrayOf<String>** names);

    CARAPI GetTinyStandAloneWeekdayNames(
        /* [out] */ ArrayOf<String>** names);

    CARAPI GetYesterday(
        /* [out] */ String* yesterday);

    CARAPI GetToday(
        /* [out] */ String* today);

    CARAPI GetTomorrow(
        /* [out] */ String* tomorrow);

    CARAPI GetFullTimeFormat(
        /* [out] */ String* fullTimeFormat);

    CARAPI GetLongTimeFormat(
        /* [out] */ String* longTimeFormat);

    CARAPI GetMediumTimeFormat(
        /* [out] */ String* mediumTimeFormat);

    CARAPI GetShortTimeFormat(
        /* [out] */ String* shortTimeFormat);

    CARAPI GetFullDateFormat(
        /* [out] */ String* fullDateFormat);

    CARAPI GetLongDateFormat(
        /* [out] */ String* longDateFormat);

    CARAPI GetMediumDateFormat(
        /* [out] */ String* mediumDateFormat);

    CARAPI GetShortDateFormat(
        /* [out] */ String* shortDateFormat);

    CARAPI GetNarrowAm(
        /* [out] */ String* narrowAm);

    CARAPI GetNarrowPm(
        /* [out] */ String* narrowPm);

    CARAPI GetShortDateFormat4(
        /* [out] */ String* shortDateFormat4);

    CARAPI GetZeroDigit(
        /* [out] */ Char32* zeroDigit);

    CARAPI GetDecimalSeparator(
        /* [out] */ Char32* decimalSeparator);

    CARAPI GetGroupingSeparator(
        /* [out] */ Char32* groupingSeparator);

    CARAPI GetPatternSeparator(
        /* [out] */ Char32* patternSeparator);

    CARAPI GetPercent(
        /* [out] */ Char32* percent);

    CARAPI GetPerMill(
        /* [out] */ Char32* perMill);

    CARAPI GetMonetarySeparator(
        /* [out] */ Char32* monetarySeparator);

    CARAPI GetMinusSign(
        /* [out] */ String* minusSign);

    CARAPI GetExponentSeparator(
        /* [out] */ String* exponentSeparator);

    CARAPI GetInfinity(
        /* [out] */ String* infinity);

    CARAPI GetNaN(
        /* [out] */ String* naN);

    CARAPI GetCurrencySymbol(
        /* [out] */ String* currencySymbol);

    CARAPI GetInternationalCurrencySymbol(
        /* [out] */ String* internationalCurrencySymbol);

    CARAPI GetTimeFormat12(
        /* [out] */ String* format); // "hh:mm a"

    CARAPI GetTimeFormat24(
        /* [out] */ String* format); // "HH:mm"

    CARAPI GetNumberPattern(
        /* [out] */ String* numberPattern);

    CARAPI GetIntegerPattern(
        /* [out] */ String* integerPattern);

    CARAPI GetCurrencyPattern(
        /* [out] */ String* currencyPattern);

    CARAPI GetPercentPattern(
        /* [out] */ String* percentPattern);

private:
    static CARAPI_(AutoPtr<ILocaleData>) InitLocaleData(
        /* [in] */ ILocale* locale);

    static Boolean sIsInited;

    static Boolean Init();

public:
    // Used by Calendar.
    AutoPtr<IInteger32> mFirstDayOfWeek;
    AutoPtr<IInteger32> mMinimalDaysInFirstWeek;

    // Used by DateFormatSymbols.
    AutoPtr<ArrayOf<String> > mAmPm; // "AM", "PM".
    AutoPtr<ArrayOf<String> > mEras; // "BC", "AD".

    AutoPtr<ArrayOf<String> > mLongMonthNames; // "January", ...
    AutoPtr<ArrayOf<String> > mShortMonthNames; // "Jan", ...
    AutoPtr<ArrayOf<String> > mTinyMonthNames; // "J", ...
    AutoPtr<ArrayOf<String> > mLongStandAloneMonthNames; // "January", ...
    AutoPtr<ArrayOf<String> > mShortStandAloneMonthNames; // "Jan", ...
    AutoPtr<ArrayOf<String> > mTinyStandAloneMonthNames; // "J", ...

    AutoPtr<ArrayOf<String> > mLongWeekdayNames; // "Sunday", ...
    AutoPtr<ArrayOf<String> > mShortWeekdayNames; // "Sun", ...
    AutoPtr<ArrayOf<String> > mTinyWeekdayNames; // "S", ...
    AutoPtr<ArrayOf<String> > mLongStandAloneWeekdayNames; // "Sunday", ...
    AutoPtr<ArrayOf<String> > mShortStandAloneWeekdayNames; // "Sun", ...
    AutoPtr<ArrayOf<String> > mTinyStandAloneWeekdayNames; // "S", ...

    // Used by frameworks/base DateSorter and DateUtils.
    String mYesterday; // "Yesterday".
    String mToday; // "Today".
    String mTomorrow; // "Tomorrow".

    String mFullTimeFormat;
    String mLongTimeFormat;
    String mMediumTimeFormat;
    String mShortTimeFormat;

    String mFullDateFormat;
    String mLongDateFormat;
    String mMediumDateFormat;
    String mShortDateFormat;

    // Used by TimePicker. Not currently used by UTS#35.
    String mNarrowAm; // "a".
    String mNarrowPm; // "p".

    // shortDateFormat, but guaranteed to have 4-digit years.
    // Used by android.text.format.DateFormat.getDateFormatStringForSetting.
    String mShortDateFormat4;

    // Used by android.text.format.DateFormat.getTimeFormat.
    String mTimeFormat12; // "hh:mm a"
    String mTimeFormat24; // "HH:mm"

    // Used by DecimalFormatSymbols.
    Char32 mZeroDigit;
    Char32 mDecimalSeparator;
    Char32 mGroupingSeparator;
    Char32 mPatternSeparator;
    Char32 mPercent;
    Char32 mPerMill;
    Char32 mMonetarySeparator;
    String mMinusSign;
    String mExponentSeparator;
    String mInfinity;
    String mNaN;
    // Also used by Currency.
    String mCurrencySymbol;
    String mInternationalCurrencySymbol;

    // Used by DecimalFormat and NumberFormat.
    String mNumberPattern;
    String mIntegerPattern;
    String mCurrencyPattern;
    String mPercentPattern;

    // A cache for the locale-specific data.
private:
    static HashMap< String, AutoPtr<ILocaleData> > sLocaleDataCache;
    static Object sLocaleDataCacheLock;
    static StaticInitializer sInitializer;
};

} // namespace ICU
} // namespace Libcore

#endif //__ICU_LOCALEDATA_H__
