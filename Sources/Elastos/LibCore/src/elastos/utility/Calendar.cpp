
#include "Calendar.h"
#include "CDateFormatSymbolsHelper.h"
#include "TimeZone.h"
#include "CGregorianCalendar.h"
#include "CInteger32.h"
#include "CLocaleHelper.h"
#include "CLocaleDataHelper.h"
#include "CLocaleData.h"
#include "ICUUtil.h"
#include "StringUtils.h"
#include "StringBuilder.h"
#include "CHashMap.h"
#include "AutoLock.h"
#include "CoreUtils.h"
#include <unistd.h>

using Elastos::IO::EIID_ISerializable;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::ICloneable;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Core::EIID_IComparable;
using Elastos::Core::EIID_ICloneable;
using Elastos::Text::IDateFormatSymbols;
using Elastos::Text::IDateFormatSymbolsHelper;
using Elastos::Text::CDateFormatSymbolsHelper;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::CHashMap;
using Libcore::ICU::ILocaleDataHelper;
using Libcore::ICU::CLocaleDataHelper;
using Libcore::ICU::ILocaleData;
using Libcore::ICU::CLocaleData;
using Libcore::ICU::ICUUtil;

namespace Elastos{
namespace Utility{

Object Calendar::sLock;
const String Calendar::FIELD_NAMES[] = {String("ERA"), String("YEAR"), String("MONTH"),
        String("WEEK_OF_YEAR"), String("WEEK_OF_MONTH"), String("DAY_OF_MONTH"), String("DAY_OF_YEAR"),
        String("DAY_OF_WEEK"), String("DAY_OF_WEEK_IN_MONTH"), String("AM_PM"), String("HOUR"),
        String("HOUR_OF_DAY"), String("MINUTE"), String("SECOND"), String("MILLISECOND"),
        String("ZONE_OFFSET"), String("DST_OFFSET") };

CAR_INTERFACE_IMPL_4(Calendar, Object, ICalendar, ISerializable, ICloneable, IComparable)

Calendar::Calendar()
    : mAreFieldsSet(FALSE)
    , mIsTimeSet(FALSE)
    , mTime(0)
    , mLenient(FALSE)
    , mFirstDayOfWeek(0)
    , mMinimalDaysInFirstWeek(0)
    , mLastTimeFieldSet(0)
    , mLastDateFieldSet(0)
{
}

AutoPtr<ILocale> Calendar::GetDefaultLocale()
{
    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> locale;
    localeHelper->GetDefault((ILocale**)&locale);
    return locale;
}

ECode Calendar::constructor()
{
    AutoPtr<ILocale> locale = GetDefaultLocale();
    AutoPtr<ITimeZone> timezone = TimeZone::GetDefault();

    return constructor(timezone.Get(), locale.Get());
}

ECode Calendar::constructor(
    /* [in] */ ITimeZone* timezone)
{
    mFields = ArrayOf<Int32>::Alloc(ICalendar::FIELD_COUNT);
    mIsSet = ArrayOf<Boolean>::Alloc(ICalendar::FIELD_COUNT);
    mAreFieldsSet = mIsTimeSet = FALSE;
    FAIL_RETURN(SetLenient(TRUE));
    FAIL_RETURN(SetTimeZone(timezone));
    return NOERROR;
}

ECode Calendar::constructor(
    /* [in] */ ITimeZone* timezone,
    /* [in] */ ILocale* l)
{
    FAIL_RETURN(constructor(timezone));

    AutoPtr<ILocaleDataHelper> localeDataHelper;
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&localeDataHelper);

    AutoPtr<ILocale> locale;
    localeDataHelper->MapInvalidAndNullLocales(l, (ILocale**)&locale);

    AutoPtr<ILocaleData> localeData;
    localeDataHelper->Get(locale, (ILocaleData**)&localeData);

    AutoPtr<IInteger32> ifdw, imdifw;
    localeData->GetFirstDayOfWeek((IInteger32**)&ifdw);
    localeData->GetMinimalDaysInFirstWeek((IInteger32**)&imdifw);
    Int32 fdw = 0, mdifw = 0;
    if (ifdw) {
        ifdw->GetValue(&fdw);
    }
    if (imdifw) {
        imdifw->GetValue(&mdifw);
    }

    FAIL_RETURN(SetFirstDayOfWeek(fdw));
    FAIL_RETURN(SetMinimalDaysInFirstWeek(mdifw));
    return NOERROR;
}

ECode Calendar::IsAfter(
    /* [in */ ICalendar* calendar,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (calendar == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    Int64 lhs, rhs;
    GetTimeInMillis(&lhs);
    calendar->GetTimeInMillis(&rhs);
    *result = (lhs > rhs);
    return NOERROR;
}

ECode Calendar::IsBefore(
    /* [in */ ICalendar* calendar,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (calendar == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    Int64 lhs, rhs;
    GetTimeInMillis(&lhs);
    calendar->GetTimeInMillis(&rhs);
    *result = (lhs < rhs);
    return NOERROR;
}

ECode Calendar::Clear()
{
    for (Int32 i = 0; i < ICalendar::FIELD_COUNT; i++) {
        (*mFields)[i] = 0;
        (*mIsSet)[i] = FALSE;
    }
    mAreFieldsSet = mIsTimeSet = FALSE;

    return NOERROR;
}

ECode Calendar::Clear(
    /* [in] */ Int32 field)
{
    (*mFields)[field] = 0;
    (*mIsSet)[field] = FALSE;
    mAreFieldsSet = mIsTimeSet = FALSE;
    return NOERROR;
}

ECode Calendar::Complete()
{
    if (!mIsTimeSet) {
        ComputeTime();
        mIsTimeSet = TRUE;
    }
    if (!mAreFieldsSet) {
        ComputeFields();
        mAreFieldsSet = TRUE;
    }
    return NOERROR;
}

ECode Calendar::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(other);

    return Equals(ICalendar::Probe(other), result);
}

ECode Calendar::Equals(
    /* [in] */ ICalendar* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(other);

    Calendar* cal = (Calendar*)other;
    if (cal == this) {
        *result = TRUE;
        return NOERROR;
    }

    Int64 ltim, rtim;
    GetTimeInMillis(&ltim);
    cal->GetTimeInMillis(&rtim);

    Boolean ll, rl;
    IsLenient(&ll);
    cal->IsLenient(&rl);

    Int32 lfdw, rfdw;
    GetFirstDayOfWeek(&lfdw);
    cal->GetFirstDayOfWeek(&rfdw);

    Int32 lmdfw, rmdfw;
    GetMinimalDaysInFirstWeek(&lmdfw);
    cal->GetMinimalDaysInFirstWeek(&rmdfw);

    AutoPtr<ITimeZone> ltz, rtz;
    GetTimeZone((ITimeZone**)&ltz);
    cal->GetTimeZone((ITimeZone**)&rtz);
    Boolean isEqual;
    ltz->Equals(rtz, &isEqual);

    *result = (ltim == rtim && ll == rl && lfdw == rfdw && lmdfw == rmdfw
            && isEqual);
    return NOERROR;
}

ECode Calendar::Get(
    /* [in] */ Int32 field,
    /* [out] */ Int32* pValue)
{
    VALIDATE_NOT_NULL(pValue);
    if (field < 0 || field >= mFields->GetLength()) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Complete();
    *pValue = (*mFields)[field];
    return NOERROR;
}

ECode Calendar::GetActualMaximum(
    /* [in] */ Int32 field,
    /* [out] */ Int32* pValue)
{
    VALIDATE_NOT_NULL(pValue);
    if (field < 0 || field >= mFields->GetLength()) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 value, next;
    Int32 max, lMax;
    GetMaximum(field, &max);
    GetLeastMaximum(field, &lMax);
    if ( max == (next = lMax) ) {
        *pValue = next;
        return NOERROR;
    }

    Complete();

    Int64 orgTime = mTime;
    Set(field, next);
    do {
        value = next;
        Roll(field, TRUE);
        Get(field, &next);
    }
    while (next > value);

    mTime = orgTime;
    mAreFieldsSet = FALSE;
    *pValue = value;
    return NOERROR;
}

ECode Calendar::GetActualMinimum(
    /* [in] */ Int32 field,
    /* [out] */ Int32* pValue)
{
    VALIDATE_NOT_NULL(pValue);
    if (field < 0 || field >= mFields->GetLength()) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 value, next;
    Int32 min, gMin;
    GetMinimum(field, &min);
    GetGreatestMinimum(field, &gMin);

    next = gMin;
    if (min == next) {
        *pValue = next;
        return NOERROR;
    }

    Complete();

    Int64 orgTime = mTime;
    Set(field, next);
    do {
        value = next;
        Roll(field, FALSE);
        Get(field, &next);
    }
    while (next < value);

    mTime = orgTime;
    mAreFieldsSet = FALSE;
    *pValue = value;
    return NOERROR;
}

ECode Calendar::GetAvailableLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** ppLocales)
{
    AutoLock lock(sLock);
    return ICUUtil::GetAvailableCalendarLocales(ppLocales);
}

ECode Calendar::GetFirstDayOfWeek(
     /* [out] */ Int32* pDay)
{
    VALIDATE_NOT_NULL(pDay);
    *pDay = mFirstDayOfWeek;
    return NOERROR;
}

ECode Calendar::GetInstance(
    /* [out] */ ICalendar** calendar)
{
    VALIDATE_NOT_NULL(calendar);
    AutoLock lock(sLock);
    return CGregorianCalendar::New(calendar);
}

ECode Calendar::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ ICalendar** calendar)
{
    VALIDATE_NOT_NULL(calendar);
    AutoLock lock(sLock);
    return CGregorianCalendar::New(locale, calendar);
}

ECode Calendar::GetInstance(
    /* [in] */ ITimeZone* timezone,
    /* [out] */ ICalendar** calendar)
{
    VALIDATE_NOT_NULL(calendar);
    AutoLock lock(sLock);
    return CGregorianCalendar::New(timezone, calendar);
}

ECode Calendar::GetInstance(
    /* [in] */ ITimeZone* timezone,
    /* [in] */ ILocale* locale,
    /* [out] */ ICalendar** calendar)
{
    VALIDATE_NOT_NULL(calendar);
    AutoLock lock(sLock);
    return CGregorianCalendar::New(timezone, locale, calendar);
}

ECode Calendar::GetMinimalDaysInFirstWeek(
    /* [out] */ Int32* pDay)
{
    VALIDATE_NOT_NULL(pDay);
    *pDay = mMinimalDaysInFirstWeek;
    return NOERROR;
}

ECode Calendar::GetTime(
    /* [out] */ IDate** ppDate)
{
    VALIDATE_NOT_NULL(ppDate);
    Int64 tim;
    GetTimeInMillis(&tim);
    return CDate::New(tim, ppDate);
}

ECode Calendar::GetTimeInMillis(
    /* [out] */ Int64* millis)
{
    VALIDATE_NOT_NULL(millis);
    if (!mIsTimeSet) {
        ComputeTime();
        mIsTimeSet = TRUE;
    }
    *millis = mTime;
    return NOERROR;
}

ECode Calendar::GetTimeZone(
    /* [out] */ ITimeZone** ppZone)
{
    VALIDATE_NOT_NULL(ppZone);
    *ppZone = mZone;
    REFCOUNT_ADD(*ppZone);
    return NOERROR;
}

Int32 Calendar::InternalGet(
    /* [in] */ Int32 field)
{
    assert(field >= 0 && field < mFields->GetLength());
    return (*mFields)[field];
}

ECode Calendar::IsLenient(
    /* [out] */ Boolean* isLenient)
{
    VALIDATE_NOT_NULL(isLenient);
    *isLenient = mLenient;
    return NOERROR;
}

ECode Calendar::IsSet(
    /* [in] */ Int32 field,
    /* [out] */ Boolean* isSet)
{
    VALIDATE_NOT_NULL(isSet);
    *isSet = FALSE;

    if (field < 0 || field >= mIsSet->GetLength()) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *isSet = (*mIsSet)[field];
    return NOERROR;
}

ECode Calendar::Roll(
    /* [in] */ Int32 field,
    /* [in] */ Int32 value)
{
    Boolean increment = value >= 0;
    Int32 count = increment ? value : -value;
    for (Int32 i = 0; i < count; i++) {
        Roll(field, increment);
    }
    return NOERROR;
}

ECode Calendar::Set(
    /* [in] */ Int32 field,
    /* [in] */ Int32 value)
{
    if (field < 0 || field >= mIsSet->GetLength()) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    (*mFields)[field] = value;
    (*mIsSet)[field] = TRUE;
    mAreFieldsSet = mIsTimeSet = FALSE;

    if (field > ICalendar::MONTH && field < ICalendar::AM_PM) {
        mLastDateFieldSet = field;
    }
    if (field == ICalendar::HOUR || field == ICalendar::HOUR_OF_DAY) {
        mLastTimeFieldSet = field;
    }
    if (field == ICalendar::AM_PM) {
        mLastTimeFieldSet = ICalendar::HOUR;
    }
    return NOERROR;
}

ECode Calendar::Set(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day)
{
    Set(ICalendar::YEAR, year);
    Set(ICalendar::MONTH, month);
    Set(ICalendar::DATE, day);
    return NOERROR;
}

ECode Calendar::Set(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day,
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute)
{
    Set(year, month, day);
    Set(ICalendar::HOUR_OF_DAY, hourOfDay);
    Set(ICalendar::MINUTE, minute);
    return NOERROR;
}

ECode Calendar::Set(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day,
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute,
    /* [in] */ Int32 second)
{
    Set(year, month, day, hourOfDay, minute);
    Set(ICalendar::SECOND, second);
    return NOERROR;
}

ECode Calendar::SetFirstDayOfWeek(
    /* [in] */ Int32 value)
{
    mFirstDayOfWeek = value;
    return NOERROR;
}

ECode Calendar::SetLenient(
    /* [in] */ Boolean value)
{
    mLenient = value;
    return NOERROR;
}

ECode Calendar::SetMinimalDaysInFirstWeek(
    /* [in] */ Int32 value)
{
    mMinimalDaysInFirstWeek = value;
    return NOERROR;
}

ECode Calendar::SetTime(
    /* [in] */ IDate* date)
{
    VALIDATE_NOT_NULL(date);

    Int64 time = 0;
    date->GetTime(&time);
    SetTimeInMillis(time);
    return NOERROR;
}

ECode Calendar::SetTimeInMillis(
    /* [in] */ Int64 milliseconds)
{
    if (!mIsTimeSet || !mAreFieldsSet || mTime != milliseconds) {
        mTime = milliseconds;
        mIsTimeSet = TRUE;
        mAreFieldsSet = FALSE;
        Complete();
    }
    return NOERROR;
}

ECode Calendar::SetTimeZone(
    /* [in] */ ITimeZone* timezone)
{
    VALIDATE_NOT_NULL(timezone);

    mZone = timezone;
    mAreFieldsSet = FALSE;
    return NOERROR;
}

ECode Calendar::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (!ICalendar::Probe(other)) {
        return E_NULL_POINTER_EXCEPTION;
    }

    Int64 ltim, rtim;
    GetTimeInMillis(&ltim);
    ICalendar::Probe(other)->GetTimeInMillis(&rtim);
    if (ltim > rtim) {
        *result = 1;
    }
    else if (ltim == rtim) {
        *result = 0;
    }
    else {
        *result = -1;
    }
    return NOERROR;
}

ECode Calendar::GetDisplayName(
    /* [in] */ Int32 field,
    /* [in] */ Int32 style,
    /* [in] */ ILocale* locale,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    if (style == ICalendar::ALL_STYLES) {
        style = ICalendar::SHORT;
    }
    AutoPtr<ArrayOf<String> > array;
    GetDisplayNameArray(field, style, locale, (ArrayOf<String>**)&array);

    Int32 value;
    Get(field, &value);

    if(array.Get() != NULL) {
        *name = (*array)[value];
    }
    else {
        *name = String(NULL);
    }
    return NOERROR;
}

ECode Calendar::GetDisplayNameArray(
    /* [in] */ Int32 field,
    /* [in] */ Int32 style,
    /* [in] */ ILocale* locale,
    /* [out, callee] */ ArrayOf<String>** array)
{
    VALIDATE_NOT_NULL(array);
    *array = NULL;

    if (field < 0 || field >= ICalendar::FIELD_COUNT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(CheckStyle(style));

    AutoPtr<IDateFormatSymbols> dfs;
    AutoPtr<IDateFormatSymbolsHelper> dfsh;
    FAIL_RETURN(CDateFormatSymbolsHelper::AcquireSingleton((IDateFormatSymbolsHelper**)&dfsh));
    dfsh->GetInstance(locale, (IDateFormatSymbols**)&dfs);
    switch (field) {
        case ICalendar::AM_PM:
            return dfs->GetAmPmStrings(array);
        case ICalendar::DAY_OF_WEEK:
            if (style == ICalendar::LONG) {
                return dfs->GetWeekdays(array);
            }
            else {
                return dfs->GetShortWeekdays(array);
            }
        case ICalendar::ERA:
            return dfs->GetEras(array);
        case ICalendar::MONTH:
            if (style == ICalendar::LONG) {
                return dfs->GetMonths(array);
            }
            else {
                return dfs->GetShortMonths(array);
            }
    }
    return NOERROR;
}

ECode Calendar::CheckStyle(
    /* [in] */ Int32 style)
{
    if (style != ICalendar::ALL_STYLES
            && style != ICalendar::SHORT
            && style != ICalendar::LONG) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode Calendar::GetDisplayNames(
    /* [in] */ Int32 field,
    /* [in] */ Int32 style,
    /* [in] */ ILocale* locale,
    /* [out] */ IMap** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(CheckStyle(style));
    Complete();

    AutoPtr<IMap> array;
    CHashMap::New((IMap**)&array);

    AutoPtr<ArrayOf<String> > nameArray;
    if (style == ICalendar::SHORT || style == ICalendar::ALL_STYLES) {
        GetDisplayNameArray(field, ICalendar::SHORT, locale,
                (ArrayOf<String>**)&nameArray);
        InsertValuesInMap(array, *nameArray);
    }
    if (style == ICalendar::LONG || style == ICalendar::ALL_STYLES) {
        GetDisplayNameArray(field, ICalendar::LONG, locale,
                (ArrayOf<String>**)&nameArray);
        InsertValuesInMap(array, *nameArray);
    }

    Int32 size;
    array->GetSize(&size);
    if (size > 0) {
        *result = array;
        REFCOUNT_ADD(*result);
    }

    return NOERROR;
}

void Calendar::InsertValuesInMap(
    /* [in] */ IMap* array,
    /* [in] */ ArrayOf<String>& values)
{
    if (array == NULL) {
        return;
    }
    for (Int32 i = 0; i < values.GetLength(); ++i) {
        if (!values[i].IsNullOrEmpty()) {
            array->Put(CoreUtils::Convert(values[i]), CoreUtils::Convert(i));
        }
    }
}

ECode Calendar::CloneImpl(
    /* [in] */ ICalendar* _clone)
{
    Calendar* clone = (Calendar*)_clone;

    clone->mFields = mFields->Clone();
    clone->mIsSet = mIsSet->Clone();
    AutoPtr<IInterface> cloneZone;
    FAIL_RETURN(ICloneable::Probe(mZone)->Clone((IInterface**)&cloneZone));
    clone->mZone = ITimeZone::Probe(cloneZone);

    return NOERROR;
}

Int32 Calendar::GetHashCode()
{
    return (mLenient ? 1237 : 1231) + mFirstDayOfWeek
        + mMinimalDaysInFirstWeek + Object::GetHashCode(mZone);
}

String Calendar::ToString()
{
    StringBuilder sb("Calendar");
    sb += "[time=";
    sb += (mIsTimeSet ? StringUtils::ToString(mTime) : "?");
    sb += ",areFieldsSet=";
    sb += mAreFieldsSet;
    sb += ",lenient=";
    sb += mLenient;
    sb += ",zone=";
    String id;
    sb += (mZone->GetID(&id), id);
    sb += ",firstDayOfWeek=";
    sb += mFirstDayOfWeek;
    sb += ",minimalDaysInFirstWeek=";
    sb += mMinimalDaysInFirstWeek;
    for (Int32 i = 0; i < ICalendar::FIELD_COUNT; i++) {
        sb.AppendChar(',');
        sb.Append(FIELD_NAMES[i]);
        sb.AppendChar('=');
        if ((*mIsSet)[i]) {
            sb.Append((*mFields)[i]);
        } else {
            sb.AppendChar('?');
        }
    }
    sb.AppendChar(']');
    return sb.ToString();
}

} // Utility
} // Elastos

