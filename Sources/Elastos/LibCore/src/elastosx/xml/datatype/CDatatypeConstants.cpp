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

#include "CDatatypeConstants.h"
#include "CQName.h"

using Elastosx::Xml::Namespace::CQName;

namespace Elastosx {
namespace Xml {
namespace Datatype {

CAR_INTERFACE_IMPL(CDatatypeConstants::Field, Object, IDatatypeConstantsField)

ECode CDatatypeConstants::Field::ToString(
    /* [out] */ String* str)
{
    *str = mStr;
    return NOERROR;
}

ECode CDatatypeConstants::Field::GetId(
    /* [out] */ Int32* id)
{
    *id = mId;
    return NOERROR;
}

CDatatypeConstants::Field::Field(
    /* [in] */ const String& str,
    /* [in] */ Int32 id)
{
    mStr = str;
    mId = id;
}

static AutoPtr<IQName> CreateQName(const String& uri, const String& duration)
{
    AutoPtr<CQName> qName;
    CQName::NewByFriend(uri, duration, (CQName**)&qName);
    return qName;
}

const AutoPtr<IQName> CDatatypeConstants::DATETIME = CreateQName(IXMLConstants::W3C_XML_SCHEMA_NS_URI, String("dateTime"));
const AutoPtr<IQName> CDatatypeConstants::TIME = CreateQName(IXMLConstants::W3C_XML_SCHEMA_NS_URI, String("time"));
const AutoPtr<IQName> CDatatypeConstants::DATE = CreateQName(IXMLConstants::W3C_XML_SCHEMA_NS_URI, String("date"));
const AutoPtr<IQName> CDatatypeConstants::GYEARMONTH = CreateQName(IXMLConstants::W3C_XML_SCHEMA_NS_URI, String("gYearMonth"));
const AutoPtr<IQName> CDatatypeConstants::GMONTHDAY = CreateQName(IXMLConstants::W3C_XML_SCHEMA_NS_URI, String("gMonthDay"));
const AutoPtr<IQName> CDatatypeConstants::GYEAR = CreateQName(IXMLConstants::W3C_XML_SCHEMA_NS_URI, String("gYear"));
const AutoPtr<IQName> CDatatypeConstants::GMONTH = CreateQName(IXMLConstants::W3C_XML_SCHEMA_NS_URI, String("gMonth"));
const AutoPtr<IQName> CDatatypeConstants::GDAY = CreateQName(IXMLConstants::W3C_XML_SCHEMA_NS_URI, String("gDay"));
const AutoPtr<IQName> CDatatypeConstants::DURATION = CreateQName(IXMLConstants::W3C_XML_SCHEMA_NS_URI, String("duration"));
const AutoPtr<IQName> CDatatypeConstants::DURATION_DAYTIME = CreateQName(IXMLConstants::W3C_XPATH_DATATYPE_NS_URI, String("dayTimeDuration"));
const AutoPtr<IQName> CDatatypeConstants::DURATION_YEARMONTH = CreateQName(IXMLConstants::W3C_XPATH_DATATYPE_NS_URI, String("yearMonthDuration"));

const AutoPtr<IDatatypeConstantsField> CDatatypeConstants::YEARS = new CDatatypeConstants::Field(String("YEARS"), 0);
const AutoPtr<IDatatypeConstantsField> CDatatypeConstants::MONTHS = new CDatatypeConstants::Field(String("MONTHS"), 1);
const AutoPtr<IDatatypeConstantsField> CDatatypeConstants::DAYS = new CDatatypeConstants::Field(String("DAYS"), 2);
const AutoPtr<IDatatypeConstantsField> CDatatypeConstants::HOURS = new CDatatypeConstants::Field(String("HOURS"), 3);
const AutoPtr<IDatatypeConstantsField> CDatatypeConstants::MINUTES = new CDatatypeConstants::Field(String("MINUTES"), 4);
const AutoPtr<IDatatypeConstantsField> CDatatypeConstants::SECONDS = new CDatatypeConstants::Field(String("SECONDS"), 5);

CAR_SINGLETON_IMPL(CDatatypeConstants)
CAR_INTERFACE_IMPL(CDatatypeConstants, Singleton, IDatatypeConstants)


ECode CDatatypeConstants::GetQNameForDATETIME(
    /* [out] */ IQName** datetime)
{
    *datetime = DATETIME;
    REFCOUNT_ADD(*datetime);
    return NOERROR;
}

ECode CDatatypeConstants::GetQNameForTIME(
    /* [out] */ IQName** time)
{
    *time = TIME;
    REFCOUNT_ADD(*time);
    return NOERROR;
}

ECode CDatatypeConstants::GetQNameForDATE(
    /* [out] */ IQName** date)
{
    *date = DATE;
    REFCOUNT_ADD(*date);
    return NOERROR;
}

ECode CDatatypeConstants::GetQNameForGYEARMONTH(
    /* [out] */ IQName** gyearmonth)
{
    *gyearmonth = GYEARMONTH;
    REFCOUNT_ADD(*gyearmonth);
    return NOERROR;
}

ECode CDatatypeConstants::GetQNameForGMONTHDAY(
    /* [out] */ IQName** gmonthday)
{
    *gmonthday = GMONTHDAY;
    REFCOUNT_ADD(*gmonthday);
    return NOERROR;
}

ECode CDatatypeConstants::GetQNameForGYEAR(
    /* [out] */ IQName** gyear)
{
    *gyear = GYEAR;
    REFCOUNT_ADD(*gyear);
    return NOERROR;
}

ECode CDatatypeConstants::GetQNameForGMONTH(
    /* [out] */ IQName** gmonth)
{
    *gmonth = GMONTH;
    REFCOUNT_ADD(*gmonth);
    return NOERROR;
}

ECode CDatatypeConstants::GetQNameForGDAY(
    /* [out] */ IQName** gday)
{
    *gday = GDAY;
    REFCOUNT_ADD(*gday);
    return NOERROR;
}

ECode CDatatypeConstants::GetQNameForDURATION(
    /* [out] */ IQName** duration)
{
    *duration = DURATION;
    REFCOUNT_ADD(*duration);
    return NOERROR;
}

ECode CDatatypeConstants::GetQNameForDURATION_DAYTIME(
    /* [out] */ IQName** duration_daytime)
{
    *duration_daytime = DURATION_DAYTIME;
    REFCOUNT_ADD(*duration_daytime);
    return NOERROR;
}

ECode CDatatypeConstants::GetQNameForDURATION_YEARMONTH(
    /* [out] */ IQName** duration_yearmonth)
{
    *duration_yearmonth = DURATION_YEARMONTH;
    REFCOUNT_ADD(*duration_yearmonth);
    return NOERROR;
}

ECode CDatatypeConstants::GetFieldForYEARS(
    /* [out] */ IDatatypeConstantsField** years)
{
    *years = YEARS;
    REFCOUNT_ADD(*years);
    return NOERROR;
}

ECode CDatatypeConstants::GetFieldForMONTHS(
    /* [out] */ IDatatypeConstantsField** months)
{
    *months = MONTHS;
    REFCOUNT_ADD(*months);
    return NOERROR;
}

ECode CDatatypeConstants::GetFieldForDAYS(
    /* [out] */ IDatatypeConstantsField** days)
{
    *days = DAYS;
    REFCOUNT_ADD(*days);
    return NOERROR;
}

ECode CDatatypeConstants::GetFieldForHOURS(
    /* [out] */ IDatatypeConstantsField** hours)
{
    *hours = HOURS;
    REFCOUNT_ADD(*hours);
    return NOERROR;
}

ECode CDatatypeConstants::GetFieldForMINUTES(
    /* [out] */ IDatatypeConstantsField** minutes)
{
    *minutes = MINUTES;
    REFCOUNT_ADD(*minutes);
    return NOERROR;
}

ECode CDatatypeConstants::GetFieldForSECONDS(
    /* [out] */ IDatatypeConstantsField** seconds)
{
    *seconds = SECONDS;
    REFCOUNT_ADD(*seconds);
    return NOERROR;
}

}
}
}
