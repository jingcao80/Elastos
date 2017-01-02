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

#ifndef __ELASTOS_SQL_CTIME_H__
#define __ELASTOS_SQL_CTIME_H__

#include "_Elastos_Sql_CTime.h"
#include "Date.h"

using Elastos::Utility::Date;

namespace Elastos {
namespace Sql {

CarClass(CTime)
    , public Date
    , public ITime
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetDate(
        /* [out] */ Int32 * pDate);

    CARAPI GetDay(
        /* [out] */ Int32 * pDay);

    CARAPI GetMonth(
        /* [out] */ Int32 * pMonth);

    CARAPI GetYear(
        /* [out] */ Int32 * pYear);

    CARAPI SetDate(
        /* [in] */ Int32 day);

    CARAPI SetMonth(
        /* [in] */ Int32 month);

    CARAPI SetTime(
        /* [in] */ Int64 milliseconds);

    CARAPI SetYear(
        /* [in] */ Int32 year);

    CARAPI ToString(
        /* [out] */ String * pLocaleStr);

    CARAPI constructor(
        /* [in] */ Int32 theHour,
        /* [in] */ Int32 theMinute,
        /* [in] */ Int32 theSecond);

    CARAPI constructor(
        /* [in] */ Int64 theTime);

public:
    static CARAPI ValueOf(
        /* [in] */ const String& timeString,
        /* [out] */ ITime** time);

private:
    CARAPI_(void) Format(
        /* [in] */ Int32 date,
        /* [in] */ Int32 digits,
        /* [in] */ StringBuilder* sb);

private:
    const static String PADDING;
};

} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_CTIME_H__
