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

#include "elastos/droid/text/format/CTimeHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/format/CTime.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

CAR_SINGLETON_IMPL(CTimeHelper)

CAR_INTERFACE_IMPL(CTimeHelper, Singleton, ITimeHelper)

ECode CTimeHelper::Compare(
    /* [in] */ ITime* a,
    /* [in] */ ITime* b,
    /* [out] */ Int32* ret)
{
    return CTime::Compare(a, b, ret);
}

ECode CTimeHelper::GetCurrentTimezone(
    /* [out] */ String * ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = CTime::GetCurrentTimezone();
    return NOERROR;
}

ECode CTimeHelper::IsEpoch(
    /* [in] */ ITime* time,
    /* [out] */ Boolean * ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = CTime::IsEpoch(time);
    return NOERROR;
}

ECode CTimeHelper::GetJulianDay(
    /* [in] */ Int64 millis,
    /* [in] */ Int64 gmtoff,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = CTime::GetJulianDay(millis, gmtoff);
    return NOERROR;
}

ECode CTimeHelper::GetWeeksSinceEpochFromJulianDay(
    /* [in] */ Int32 julianDay,
    /* [in] */ Int32 firstDayOfWeek,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = CTime::GetWeeksSinceEpochFromJulianDay(julianDay, firstDayOfWeek);
    return NOERROR;
}

ECode CTimeHelper::GetJulianMondayFromWeeksSinceEpoch(
    /* [in] */ Int32 week,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = CTime::GetJulianMondayFromWeeksSinceEpoch(week);
    return NOERROR;
}





} // namespace Format
} // namespace Text
} // namepsace Droid
} // namespace Elastos

