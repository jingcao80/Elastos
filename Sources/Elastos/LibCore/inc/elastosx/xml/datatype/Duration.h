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

#ifndef __ELASTOSX_XML_DATATYPE_DURATION_H__
#define __ELASTOSX_XML_DATATYPE_DURATION_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Core::INumber;
using Elastos::Math::IBigDecimal;
using Elastos::Math::IBigInteger;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IDate;
using Elastosx::Xml::Namespace::IQName;

namespace Elastosx {
namespace Xml {
namespace Datatype {

class Duration
    : public Object
    , public IDuration
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetXMLSchemaType(
        /* [out] */ IQName** type);

    CARAPI GetYears(
        /* [out] */ Int32* years);

    CARAPI GetMonths(
        /* [out] */ Int32* months);

    CARAPI GetDays(
        /* [out] */ Int32* days);

    CARAPI GetHours(
        /* [out] */ Int32* hours);

    CARAPI GetMinutes(
        /* [out] */ Int32* minutes);

    CARAPI GetSeconds(
        /* [out] */ Int32* seconds);

    CARAPI GetTimeInMillis(
        /* [in] */ ICalendar* startInstant,
        /* [out] */ Int64* time);

    CARAPI GetTimeInMillis(
        /* [in] */ IDate* startInstant,
        /* [out] */ Int64* time);

    CARAPI AddTo(
        /* [in] */ IDate* date);

    using IDuration::AddTo;

    CARAPI Subtract(
        /* [in] */ IDuration* rhs,
        /* [out] */ IDuration** duration);

    CARAPI Multiply(
        /* [in] */ Int32 factor,
        /* [out] */ IDuration** duration);

    using IDuration::Multiply;

    CARAPI Compare(
        /* [in] */ IDuration* duration,
        /* [out] */ Int32* rst) = 0;

    CARAPI IsLongerThan(
        /* [in] */ IDuration* rhs,
        /* [out] */ Boolean* rst);

    CARAPI IsShorterThan(
        /* [in] */ IDuration* rhs,
        /* [out] */ Boolean* rst);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    Int32 GetFieldValueAsInt(
        /* [in] */ IDatatypeConstantsField* field);

    String ToString(
        /* [in] */ IBigDecimal* bd);

    static Int64 GetCalendarTimeInMillis(
        /* [in] */ ICalendar* cal);
};

} // namespace Datatype
} // namespace Xml
} // namespace Elastosx
#endif
