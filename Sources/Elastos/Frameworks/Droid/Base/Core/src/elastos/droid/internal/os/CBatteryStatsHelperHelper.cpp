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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/os/CBatteryStatsHelperHelper.h"
#include "elastos/droid/internal/os/CBatteryStatsHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CAR_INTERFACE_IMPL(CBatteryStatsHelperHelper, Singleton, IBatteryStatsHelperHelper)

CAR_SINGLETON_IMPL(CBatteryStatsHelperHelper)

ECode CBatteryStatsHelperHelper::CheckWifiOnly(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = CBatteryStatsHelper::CheckWifiOnly(context);
    return NOERROR;
}

ECode CBatteryStatsHelperHelper::StatsFromFile(
    /* [in] */ IContext* context,
    /* [in] */ const String& fname,
    /* [out] */ IBatteryStats** stats)
{
    VALIDATE_NOT_NULL(stats)
    AutoPtr<IBatteryStats> bs = CBatteryStatsHelper::StatsFromFile(context, fname);
    *stats = bs;
    REFCOUNT_ADD(*stats)
    return NOERROR;
}

ECode CBatteryStatsHelperHelper::DropFile(
    /* [in] */ IContext* context,
    /* [in] */ const String& fname)
{
    CBatteryStatsHelper::DropFile(context, fname);
    return NOERROR;
}

ECode CBatteryStatsHelperHelper::MakemAh(
    /* [in] */ Double power,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = CBatteryStatsHelper::MakemAh(power);
    return NOERROR;
}

ECode CBatteryStatsHelperHelper::ReadFully(
    /* [in] */ IFileInputStream* stream,
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes)
    *bytes = CBatteryStatsHelper::ReadFully(stream);
    REFCOUNT_ADD(*bytes)
    return NOERROR;
}

ECode CBatteryStatsHelperHelper::ReadFully(
    /* [in] */ IFileInputStream* stream,
    /* [in] */ Int32 avail,
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes)
    *bytes = CBatteryStatsHelper::ReadFully(stream, avail);
    REFCOUNT_ADD(*bytes)
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Internal
} // namespace Elastos
