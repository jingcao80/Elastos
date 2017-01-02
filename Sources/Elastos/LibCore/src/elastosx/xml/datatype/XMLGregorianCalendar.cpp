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

#include "XMLGregorianCalendar.h"

using Elastos::Core::INumber;

namespace Elastosx {
namespace Xml {
namespace Datatype {

CAR_INTERFACE_IMPL(XMLGregorianCalendar, Object, IXMLGregorianCalendar)

ECode XMLGregorianCalendar::SetTime(
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [in] */ Int32 second)
{
    return SetTime(hour, minute, second, (IBigDecimal*)NULL);
}

ECode XMLGregorianCalendar::SetTime(
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute,
    /* [in] */ Int32 second,
    /* [in] */ IBigDecimal* fractional)
{
    SetHour(hour);
    SetMinute(minute);
    SetSecond(second);
    SetFractionalSecond(fractional);
    return NOERROR;
}

ECode XMLGregorianCalendar::SetTime(
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute,
    /* [in] */ Int32 second,
    /* [in] */ Int32 millisecond)
{
    SetHour(hour);
    SetMinute(minute);
    SetSecond(second);
    SetMillisecond(millisecond);
    return NOERROR;
}

ECode XMLGregorianCalendar::GetMillisecond(
    /* [out] */ Int32* second)
{
    *second = IDatatypeConstants::FIELD_UNDEFINED;
    AutoPtr<IBigDecimal> fractionalSeconds;
    GetFractionalSecond((IBigDecimal**)&fractionalSeconds);

    // is field undefined?
    if (fractionalSeconds == NULL) {
        return NOERROR;
    }

    AutoPtr<IBigDecimal> right;
    fractionalSeconds->MovePointRight(3, (IBigDecimal**)&right);

    AutoPtr<INumber> nbTmp = INumber::Probe(right);
    if (nbTmp != NULL) {
        nbTmp->Int32Value(second);
    }
    return NOERROR;
}

ECode XMLGregorianCalendar::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    // Following two dates compare to EQUALS since in different timezones.
    // 2000-01-15T12:00:00-05:00 == 2000-01-15T13:00:00-04:00
    //
    // Must ensure both instances generate same hashcode by normalizing
    // this to UTC timezone.
    Int32 timezone;
    GetTimezone(&timezone);
    if (timezone == IDatatypeConstants::FIELD_UNDEFINED) {
        timezone = 0;
    }

    AutoPtr<IXMLGregorianCalendar> gc = this;
    if (timezone != 0) {
        this->Normalize((IXMLGregorianCalendar**)&gc);
    }

    Int32 year, month, day, hour, minute, second;
    gc->GetYear(&year);
    gc->GetMonth(&month);
    gc->GetDay(&day);
    gc->GetHour(&hour);
    gc->GetMinute(&minute);
    gc->GetSecond(&second);

    *hashCode = year + month + day + hour + minute + second;
    return NOERROR;
}

ECode XMLGregorianCalendar::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    IXMLGregorianCalendar* cdr = IXMLGregorianCalendar::Probe(other);
    if (cdr == (IXMLGregorianCalendar*)this) {
        *result = TRUE;
        return NOERROR;
    }
    if (cdr != NULL) {
        Int32 cpr;
        Compare(cdr, &cpr);
        *result = cpr == IDatatypeConstants::EQUAL;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode XMLGregorianCalendar::ToString(
    /* [out] */ String* info)
{
    return ToXMLFormat(info);
}
} // namespace Datatype
} // namespace Xml
} // namespace Elastosx
