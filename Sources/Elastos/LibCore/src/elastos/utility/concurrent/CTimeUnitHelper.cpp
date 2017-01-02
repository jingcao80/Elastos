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

#include "CTimeUnitHelper.h"
#include "TimeUnit.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

CAR_INTERFACE_IMPL(CTimeUnitHelper, Singleton, ITimeUnitHelper)

CAR_SINGLETON_IMPL(CTimeUnitHelper)

ECode CTimeUnitHelper::GetNANOSECONDS(
    /* [out] */ ITimeUnit** unit)
{
    VALIDATE_NOT_NULL(unit);
    *unit = new NANOSECONDS();
    REFCOUNT_ADD(*unit);
    return NOERROR;
}

ECode CTimeUnitHelper::GetMICROSECONDS(
    /* [out] */ ITimeUnit** unit)
{
    VALIDATE_NOT_NULL(unit);
    *unit = new MICROSECONDS();
    REFCOUNT_ADD(*unit);
    return NOERROR;
}

ECode CTimeUnitHelper::GetMILLISECONDS(
    /* [out] */ ITimeUnit** unit)
{
    VALIDATE_NOT_NULL(unit);
    *unit = new MILLISECONDS();
    REFCOUNT_ADD(*unit);
    return NOERROR;
}

ECode CTimeUnitHelper::GetSECONDS(
    /* [out] */ ITimeUnit** unit)
{
    VALIDATE_NOT_NULL(unit);
    *unit = new SECONDS();
    REFCOUNT_ADD(*unit);
    return NOERROR;
}

ECode CTimeUnitHelper::GetMINUTES(
    /* [out] */ ITimeUnit** unit)
{
    VALIDATE_NOT_NULL(unit);
    *unit = new MINUTES();
    REFCOUNT_ADD(*unit);
    return NOERROR;
}

ECode CTimeUnitHelper::GetHOURS(
    /* [out] */ ITimeUnit** unit)
{
    VALIDATE_NOT_NULL(unit);
    *unit = new HOURS();
    REFCOUNT_ADD(*unit);
    return NOERROR;
}

ECode CTimeUnitHelper::GetDAYS(
    /* [out] */ ITimeUnit** unit)
{
    VALIDATE_NOT_NULL(unit);
    *unit = new DAYS();
    REFCOUNT_ADD(*unit);
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
