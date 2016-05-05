
#include "DateFormatSymbols.h"
#include "CDateFormatSymbols.h"
#include "CString.h"
#include "CLocaleHelper.h"
#include "Arrays.h"
#include "CLocaleDataHelper.h"
#include "ICUUtil.h"
#include "TimeZoneNames.h"
#include "AutoLock.h"
#include "StringBuilder.h"
#include "CoreUtils.h"
#include "TimeZone.h"
#include "CArrayOf.h"
#include "LocaleData.h"

using Elastos::IO::EIID_ISerializable;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::CoreUtils;
using Elastos::Utility::TimeZone;
using Elastos::Utility::Arrays;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Libcore::ICU::ILocaleDataHelper;
using Libcore::ICU::CLocaleDataHelper;
using Libcore::ICU::ILocaleData;
using Libcore::ICU::LocaleData;
using Libcore::ICU::ICUUtil;
using Libcore::ICU::TimeZoneNames;

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL_3(DateFormatSymbols, Object, IDateFormatSymbols, ICloneable, ISerializable)

DateFormatSymbols::DateFormatSymbols()
{
}

DateFormatSymbols::~DateFormatSymbols()
{
}

AutoPtr<ArrayOf<IArrayOf*> > DateFormatSymbols::InternalZoneStrings()
{
    AutoLock lock(mLock);
    if (mZoneStrings == NULL) {
        TimeZoneNames::GetZoneStrings(mLocale, (ArrayOf<IArrayOf*>**)&mZoneStrings);
    }
    return mZoneStrings;
}

ECode DateFormatSymbols::constructor()
{
    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> locale;
    localeHelper->GetDefault((ILocale**)&locale);
    return constructor(locale);
}

ECode DateFormatSymbols::constructor(
    /* [in] */ ILocale* locale)
{
    mLocale = LocaleData::MapInvalidAndNullLocales(locale);;
    mLocalPatternChars = ISimpleDateFormat_PATTERN_CHARS;

    AutoPtr<ILocaleDataHelper> localeDataHelper;
    FAIL_RETURN(CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&localeDataHelper));
    AutoPtr<ILocaleData> localeData;
    FAIL_RETURN(localeDataHelper->Get(locale, (ILocaleData**)&localeData));

    mLocaleData = localeData;
    localeData->GetAmPm((ArrayOf<String>**)&mAmpms);
    localeData->GetEras((ArrayOf<String>**)&mEras);
    localeData->GetLongMonthNames((ArrayOf<String>**)&mMonths);
    localeData->GetShortMonthNames((ArrayOf<String>**)&mShortMonths);
    localeData->GetLongWeekdayNames((ArrayOf<String>**)&mWeekdays);
    localeData->GetShortWeekdayNames((ArrayOf<String>**)&mShortWeekdays);

    return NOERROR;
}

ECode DateFormatSymbols::GetInstance(
    /* [out] */ IDateFormatSymbols** instance)
{
    AutoPtr<ILocaleHelper> pLocaleHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&pLocaleHelper);
    AutoPtr<ILocale> pLocale;
    pLocaleHelper->GetDefault((ILocale**)&pLocale);
    return GetInstance(pLocale, instance);
}

ECode DateFormatSymbols::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ IDateFormatSymbols** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = NULL;
    VALIDATE_NOT_NULL(locale);

    return CDateFormatSymbols::New(locale, instance);
}

ECode DateFormatSymbols::GetAvailableLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** arrayOfLocales)
{
    return ICUUtil::GetAvailableDateFormatSymbolsLocales(arrayOfLocales);
}

// private void readObject(ObjectInputStream ois) throws IOException, ClassNotFoundException {
//     ois.defaultReadObject();

//     Locale locale = this.locale;
//     if (locale == null) {
//         // Before the L release Android did not serialize the locale. Handle its absence.
//         locale = Locale.getDefault();
//     }
//     this.localeData = LocaleData.get(locale);
// }

// private void writeObject(ObjectOutputStream oos) throws IOException {
//     internalZoneStrings();
//     oos.defaultWriteObject();
// }

ECode DateFormatSymbols::Clone(
    /* [out] */ IInterface** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = NULL;

    AutoPtr<IDateFormatSymbols> outfs;
    if (mLocale) {
        FAIL_RETURN(CDateFormatSymbols::New(mLocale, (IDateFormatSymbols**)&outfs));
    }
    else {
        FAIL_RETURN(CDateFormatSymbols::New((IDateFormatSymbols**)&outfs));
    }

    CloneImpl(outfs);

    *instance = outfs;
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode DateFormatSymbols::CloneImpl(
    /* [in] */ IDateFormatSymbols* instance)
{
    assert(instance);
    instance->SetAmPmStrings(mAmpms);
    instance->SetEras(mEras);
    instance->SetLocalPatternChars(mLocalPatternChars);
    instance->SetMonths(mMonths);
    instance->SetShortMonths(mShortMonths);
    instance->SetShortWeekdays(mShortWeekdays);
    instance->SetWeekdays(mWeekdays);

    //DateFormatSymbols* ds = (DateFormatSymbols*)instance;
    instance->SetZoneStrings(mZoneStrings);
    return NOERROR;
}

ECode DateFormatSymbols::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    IDateFormatSymbols * other = IDateFormatSymbols::Probe(object);
    if (this == other) {
        *result = TRUE;
        return NOERROR;
    }

    if (other == NULL) {
        return NOERROR;
    }

    DateFormatSymbols * rhs = (DateFormatSymbols*)other;
    *result = mLocalPatternChars.Equals(rhs->mLocalPatternChars) &&
            Arrays::Equals(mAmpms, rhs->mAmpms) &&
            Arrays::Equals(mEras, rhs->mEras) &&
            Arrays::Equals(mMonths, rhs->mMonths) &&
            Arrays::Equals(mShortMonths, rhs->mShortMonths) &&
            Arrays::Equals(mShortWeekdays, rhs->mShortWeekdays) &&
            Arrays::Equals(mWeekdays, rhs->mWeekdays) &&
            TimeZoneStringsEqual(this, rhs);
    return NOERROR;
}

Boolean DateFormatSymbols::TimeZoneStringsEqual(
    /* [in] */ IDateFormatSymbols *lhs,
    /* [in] */ IDateFormatSymbols *rhs)
{
    if (lhs == rhs) {
        return TRUE;
    }
    if (lhs == NULL || rhs == NULL) {
        return FALSE;
    }

    DateFormatSymbols* l = (DateFormatSymbols*)lhs;
    DateFormatSymbols* r = (DateFormatSymbols*)rhs;
    // Quick check that may keep us from having to load the zone strings.
    // Note that different locales may have the same strings, so the opposite check isn't valid.
    if (l->mZoneStrings == NULL && r->mZoneStrings == NULL && Object::Equals(l->mLocale, r->mLocale)) {
        return TRUE;
    }

    // Make sure zone strings are loaded, then check.
    AutoPtr<ArrayOf<IArrayOf*> > lzs, rzs;
    lzs = l->InternalZoneStrings();
    rzs = r->InternalZoneStrings();
    return Arrays::DeepEquals(lzs, rzs);
}

ECode DateFormatSymbols::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    // 'locale' isn't part of the externally-visible state.
    // 'zoneStrings' is so large, we just print a representative value.

    StringBuilder sb;
    sb.Append("DateFormatSymbols");
    sb.Append("[amPmStrings=");
    sb.Append(Arrays::ToString(mAmpms));
    sb.Append(",eras=");
    sb.Append(Arrays::ToString(mEras));
    sb.Append(",localPatternChars=");
    sb.Append(mLocalPatternChars);
    sb.Append(",months=");
    sb.Append(Arrays::ToString(mMonths));
    sb.Append(",shortMonths=");
    sb.Append(Arrays::ToString(mShortMonths));
    sb.Append(",shortWeekdays=");
    sb.Append(Arrays::ToString(mShortWeekdays));
    sb.Append(",weekdays=");
    sb.Append(Arrays::ToString(mWeekdays));
    sb.Append(",zoneStrings=[");
    AutoPtr<ArrayOf<IArrayOf*> > lzs = InternalZoneStrings();
    sb.Append(Object::ToString((*lzs)[0]));
    sb.Append("...]]");

    *str = sb.ToString();
    return NOERROR;
}

ECode DateFormatSymbols::GetLocale(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)

    *locale = mLocale;
    REFCOUNT_ADD(*locale)
    return NOERROR;
}

ECode DateFormatSymbols::GetAmPmStrings(
    /* [out, callee] */ ArrayOf<String>** arrayOfStrings)
{
    VALIDATE_NOT_NULL(arrayOfStrings)

    AutoPtr<ArrayOf<String> > temp = mAmpms->Clone();
    *arrayOfStrings = temp;
    REFCOUNT_ADD(*arrayOfStrings)
    return NOERROR;
}

ECode DateFormatSymbols::GetEras(
    /* [out, callee] */ ArrayOf<String>** arrayOfStrings)
{
    VALIDATE_NOT_NULL(arrayOfStrings)

    AutoPtr<ArrayOf<String> > temp = mEras->Clone();
    *arrayOfStrings = temp;
    REFCOUNT_ADD(*arrayOfStrings)
    return NOERROR;
}

ECode DateFormatSymbols::GetLocalPatternChars(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);

    *string = mLocalPatternChars;
    return NOERROR;
}

ECode DateFormatSymbols::GetMonths(
    /* [out, callee] */ ArrayOf<String>** arrayOfStrings)
{
    VALIDATE_NOT_NULL(arrayOfStrings)

    AutoPtr<ArrayOf<String> > temp = mMonths->Clone();
    *arrayOfStrings = temp;
    REFCOUNT_ADD(*arrayOfStrings)
    return NOERROR;
}

ECode DateFormatSymbols::GetShortMonths(
    /* [out, callee] */ ArrayOf<String>** arrayOfStrings)
{
    VALIDATE_NOT_NULL(arrayOfStrings)

    AutoPtr<ArrayOf<String> > temp = mShortMonths->Clone();
    *arrayOfStrings = temp;
    REFCOUNT_ADD(*arrayOfStrings)
    return NOERROR;
}

ECode DateFormatSymbols::GetShortWeekdays(
    /* [out, callee] */ ArrayOf<String>** arrayOfStrings)
{
    VALIDATE_NOT_NULL(arrayOfStrings)

    AutoPtr<ArrayOf<String> > temp = mShortWeekdays->Clone();
    *arrayOfStrings = temp;
    REFCOUNT_ADD(*arrayOfStrings)
    return NOERROR;
}

ECode DateFormatSymbols::GetWeekdays(
    /* [out, callee] */ ArrayOf<String>** arrayOfStrings)
{
    VALIDATE_NOT_NULL(arrayOfStrings)

    AutoPtr<ArrayOf<String> > temp = mWeekdays->Clone();
    *arrayOfStrings = temp;
    REFCOUNT_ADD(*arrayOfStrings)
    return NOERROR;
}

ECode DateFormatSymbols::GetZoneStrings(
    /* [out, callee] */ ArrayOf<IArrayOf*>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<IArrayOf*> > strings = InternalZoneStrings();
    AutoPtr<ArrayOf<IArrayOf*> > copy = Clone2dStringArray(strings);

    // If icu4c doesn't have a name, our array contains a null. TimeZone.getDisplayName
    // knows how to format GMT offsets (and, unlike icu4c, has accurate data). http://b/8128460.
    Int32 size = copy->GetLength();
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IArrayOf> zone = (*copy)[i];
        AutoPtr<IInterface> obj;
        zone->Get(0, (IInterface**)&obj);

        String id = Object::ToString(obj);
        AutoPtr<ITimeZone> tz;
        TimeZone::GetTimeZone(id, (ITimeZone**)&tz);

        obj = NULL;
        zone->Get(1, (IInterface**)&obj);
        if (obj == NULL) {
            String name;
            tz->GetDisplayName(FALSE, ITimeZone::LONG, mLocale, &name);
            AutoPtr<ICharSequence> csq = CoreUtils::Convert(name);
            zone->Set(1, csq);
        }

        obj = NULL;
        zone->Get(2, (IInterface**)&obj);
        if (obj == NULL) {
            String name;
            tz->GetDisplayName(FALSE, ITimeZone::SHORT, mLocale, &name);
            AutoPtr<ICharSequence> csq = CoreUtils::Convert(name);
            zone->Set(2, csq);
        }

        obj = NULL;
        zone->Get(3, (IInterface**)&obj);
        if (obj == NULL) {
            String name;
            tz->GetDisplayName(TRUE, ITimeZone::LONG, mLocale, &name);
            AutoPtr<ICharSequence> csq = CoreUtils::Convert(name);
            zone->Set(3, csq);
        }


        obj = NULL;
        zone->Get(4, (IInterface**)&obj);
        if (obj == NULL) {
            String name;
            tz->GetDisplayName(TRUE, ITimeZone::SHORT, mLocale, &name);
            AutoPtr<ICharSequence> csq = CoreUtils::Convert(name);
            zone->Set(4, csq);
        }
    }

    *result = copy;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<ArrayOf<IArrayOf*> > DateFormatSymbols::Clone2dStringArray(
    /* [in] */ ArrayOf<IArrayOf*>* inArray)
{
    assert(inArray);

    Int32 size = inArray->GetLength();
    String string;
    AutoPtr<ArrayOf<IArrayOf*> > result = ArrayOf<IArrayOf*>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IArrayOf> bc = (*inArray)[i];
        if (bc == NULL) {
            result->Set(i, NULL);
            continue;
        }

        Int32 subSize;
        bc->GetLength(&subSize);
        AutoPtr<IArrayOf> newBc;
        CArrayOf::New(EIID_IInterface, subSize, (IArrayOf**)&newBc);
        for (Int32 j = 0; j < subSize; ++j) {
            AutoPtr<IInterface> obj;
            bc->Get(j, (IInterface**)&obj);
            newBc->Set(j, obj);
        }

        result->Set(i, newBc);
    }
    return result;
}

ECode DateFormatSymbols::SetAmPmStrings(
    /* [in] */ ArrayOf<String> * data)
{
    VALIDATE_NOT_NULL(data)
    mAmpms = data->Clone();
    return NOERROR;
}

ECode DateFormatSymbols::SetEras(
    /* [in] */ ArrayOf<String> * data)
{
    VALIDATE_NOT_NULL(data)
    mEras = data->Clone();
    return NOERROR;
}

ECode DateFormatSymbols::SetLocalPatternChars(
    /* [in] */ const String& data)
{
    if (data.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mLocalPatternChars = data;
    return NOERROR;
}

ECode DateFormatSymbols::SetMonths(
    /* [in] */ ArrayOf<String> * data)
{
    VALIDATE_NOT_NULL(data)
    mMonths = data->Clone();
    return NOERROR;
}

ECode DateFormatSymbols::SetShortMonths(
    /* [in] */ ArrayOf<String> * data)
{
    VALIDATE_NOT_NULL(data)
    mShortMonths = data->Clone();
    return NOERROR;
}

ECode DateFormatSymbols::SetShortWeekdays(
    /* [in] */ ArrayOf<String> * data)
{
    VALIDATE_NOT_NULL(data)
    mShortWeekdays = data->Clone();
    return NOERROR;
}

ECode DateFormatSymbols::SetWeekdays(
    /* [in] */ ArrayOf<String> * data)
{
    VALIDATE_NOT_NULL(data)
    mWeekdays = data->Clone();
    return NOERROR;
}

ECode DateFormatSymbols::SetZoneStrings(
    /* [in] */ ArrayOf<IArrayOf*>* zoneStrings)
{
    if (zoneStrings == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 length;
    for (Int32 i = 0; i < zoneStrings->GetLength(); i++) {
        AutoPtr<IArrayOf> bc = (*zoneStrings)[i];
        if (bc == NULL) return E_ILLEGAL_ARGUMENT_EXCEPTION;

        bc->GetLength(&length);
        if (length < 5) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    mZoneStrings = Clone2dStringArray(zoneStrings);
    return NOERROR;
}

ECode DateFormatSymbols::GetTimeZoneDisplayName(
    /* [in] */ ITimeZone* tz,
    /* [in] */ Boolean daylight,
    /* [in] */ Int32 style,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (style != ITimeZone::SHORT && style != ITimeZone::LONG) {
        //throw new IllegalArgumentException("Bad style: " + style);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // If custom zone strings have been set with setZoneStrings() we use those. Otherwise we
    // use the ones from LocaleData.
    String id;
    tz->GetID(&id);
    AutoPtr<ArrayOf<IArrayOf*> > zoneStrings = InternalZoneStrings();
    return TimeZoneNames::GetDisplayName(zoneStrings,id, daylight, style, result);
}

} // namespace Text
} // namespace Elastos
