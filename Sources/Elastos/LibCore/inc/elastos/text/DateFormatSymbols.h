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

#ifndef __ELASTOS_TEXT_DATEFORMATSYMBOLS_H__
#define __ELASTOS_TEXT_DATEFORMATSYMBOLS_H__

#include "Elastos.CoreLibrary.Text.h"
#include "Object.h"

using Elastos::Core::ICloneable;
using Elastos::Core::IArrayOf;
using Elastos::Core::Object;
using Elastos::IO::ISerializable;
using Elastos::Utility::ILocale;
using Elastos::Utility::ITimeZone;
using Libcore::ICU::ILocaleData;

namespace Elastos {
namespace Text {

class DateFormatSymbols
    : public Object
    , public IDateFormatSymbols
    , public ICloneable
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    DateFormatSymbols();

    virtual ~DateFormatSymbols();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ILocale* locale);

    static CARAPI GetInstance(
        /* [out] */ IDateFormatSymbols** instance);

    static CARAPI GetInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ IDateFormatSymbols** instance);

    static CARAPI GetAvailableLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** arrayOfLocales);

    CARAPI Clone(
        /* [out] */ IInterface** instance);

    CARAPI CloneImpl(
        /* [out] */ IDateFormatSymbols* instance);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* res);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetLocale(
        /* [out] */ ILocale** locale);

    CARAPI GetAmPmStrings(
        /* [out, callee] */ ArrayOf<String> ** arrayOfStrings);

    CARAPI GetEras(
        /* [out, callee] */ ArrayOf<String> ** arrayOfStrings);

    CARAPI GetLocalPatternChars(
        /* [out] */ String* string);

    CARAPI GetMonths(
        /* [out, callee] */ ArrayOf<String>** arrayOfStrings);

    CARAPI GetShortMonths(
        /* [out, callee] */ ArrayOf<String>** arrayOfStrings);

    CARAPI GetShortWeekdays(
        /* [out, callee] */ ArrayOf<String>** arrayOfStrings);

    CARAPI GetWeekdays(
        /* [out, callee] */ ArrayOf<String>** arrayOfStrings);

    CARAPI GetZoneStrings(
        /* [out, callee] */ ArrayOf<IArrayOf*> ** zoneStrings);

//    @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI SetAmPmStrings(
        /* [in] */ ArrayOf<String> * data);

    CARAPI SetEras(
        /* [in] */ ArrayOf<String> * data);

    CARAPI SetLocalPatternChars(
        /* [in] */ const String& data);

    CARAPI SetMonths(
        /* [in] */ ArrayOf<String> * data);

    CARAPI SetShortMonths(
        /* [in] */ ArrayOf<String> * data);

    CARAPI SetShortWeekdays(
        /* [in] */ ArrayOf<String> * data);

    CARAPI SetWeekdays(
        /* [in] */ ArrayOf<String> * data);

    CARAPI SetZoneStrings(
        /* [in] */ ArrayOf<IArrayOf*>* zoneStrings);

    CARAPI_(AutoPtr<ArrayOf<IArrayOf*> >) InternalZoneStrings();

    CARAPI GetTimeZoneDisplayName(
        /* [in] */ ITimeZone* tz,
        /* [in] */ Boolean daylight,
        /* [in] */ Int32 style,
        /* [out] */ String* result);

private:
//    private void readObject(ObjectInputStream ois);

//    private void writeObject(ObjectOutputStream oos);

    static CARAPI_(Boolean) TimeZoneStringsEqual(
        /* [in] */ IDateFormatSymbols* lhs,
        /* [in] */ IDateFormatSymbols* rhs);

    AutoPtr<ArrayOf<IArrayOf*> > Clone2dStringArray(
        /* [in] */ ArrayOf<IArrayOf*>* array);

public:
    String mLocalPatternChars;

    AutoPtr<ArrayOf<String> > mAmpms, mEras, mMonths, mShortMonths, mShortWeekdays, mWeekdays;

    // This is used to implement parts of Unicode UTS #35 not historically supported.
    /*transient*/ AutoPtr<ILocaleData> mLocaleData;

    // Localized display names.
    AutoPtr<ArrayOf<IArrayOf*> > mZoneStrings;// String[][]

    AutoPtr<ILocale> mLocale;

    Object mLock;
};

} // namespace Text
} // namespace Elastos

#endif //__ELASTOS_TEXT_DATEFORMATSYMBOLS_H__
