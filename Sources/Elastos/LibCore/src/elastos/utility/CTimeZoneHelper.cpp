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

#include "CTimeZoneHelper.h"
#include "TimeZone.h"

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL(CTimeZoneHelper, Singleton, ITimeZoneHelper)

CAR_SINGLETON_IMPL(CTimeZoneHelper)

ECode CTimeZoneHelper::GetAvailableIDs(
    /* [out, callee] */ ArrayOf<String> ** array)
{
    VALIDATE_NOT_NULL(array);
    AutoPtr<ArrayOf<String> > ids = TimeZone::GetAvailableIDs();
    *array = ids;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode CTimeZoneHelper::GetAvailableIDs(
    /* [in] */ Int32 offsetMillis,
    /* [out, callee] */ ArrayOf<String> ** array)
{
    VALIDATE_NOT_NULL(array);
    AutoPtr<ArrayOf<String> > ids = TimeZone::GetAvailableIDs(offsetMillis);
    *array = ids;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode CTimeZoneHelper::GetDefault(
    /* [out] */ ITimeZone ** timezone)
{
    VALIDATE_NOT_NULL(timezone);
    AutoPtr<ITimeZone> tz = TimeZone::GetDefault();
    *timezone = tz;
    REFCOUNT_ADD(*timezone);

    return NOERROR;
}

ECode CTimeZoneHelper::GetTimeZone(
    /* [in] */ const String& id,
    /* [out] */ ITimeZone ** timezone)
{
    return TimeZone::GetTimeZone(id, timezone);
}

ECode CTimeZoneHelper::SetDefault(
    /* [in] */ ITimeZone* timeZone)
{
    TimeZone::SetDefault(timeZone);
    return NOERROR;
}

ECode CTimeZoneHelper::GetGMT(
    /* [out] */ ITimeZone** timezone)
{
    VALIDATE_NOT_NULL(timezone);
    AutoPtr<ITimeZone> tz = TimeZone::GMT();
    *timezone = tz;
    REFCOUNT_ADD(*timezone);
    return NOERROR;
}

ECode CTimeZoneHelper::GetUTC(
    /* [out] */ ITimeZone** timezone)
{
    VALIDATE_NOT_NULL(timezone);
    AutoPtr<ITimeZone> tz = TimeZone::UTC();
    *timezone = tz;
    REFCOUNT_ADD(*timezone);
    return NOERROR;
}


} // Utility
} // Elastos
