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

#include "elastos/droid/wifi/CWifiScannerChannelSpec.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CWifiScannerChannelSpec, Object, IWifiScannerChannelSpec)

CAR_OBJECT_IMPL(CWifiScannerChannelSpec)

ECode CWifiScannerChannelSpec::constructor(
    /* [in] */ Int32 frequency)
{
    mFrequency = frequency;
    mPassive = FALSE;
    mDwellTimeMS = 0;
    return NOERROR;
}

/**
 * channel frequency in MHz; for example channel 1 is specified as 2412
 */
ECode CWifiScannerChannelSpec::GetFrequency(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFrequency;
    return NOERROR;
}

ECode CWifiScannerChannelSpec::SetFrequency(
    /* [in] */ Int32 frequency)
{
    mFrequency = frequency;
    return NOERROR;
}

/**
 * if true, scan this channel in passive fashion.
 * This flag is ignored on DFS channel specification.
 * @hide
 */
ECode CWifiScannerChannelSpec::GetPassive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPassive;
    return NOERROR;
}

ECode CWifiScannerChannelSpec::SetPassive(
    /* [in] */ Boolean passive)
{
    mPassive = passive;
    return NOERROR;
}

/**
 * how long to dwell on this channel
 * @hide
 */
ECode CWifiScannerChannelSpec::GetDwellTimeMS(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDwellTimeMS;
    return NOERROR;
}

ECode CWifiScannerChannelSpec::SetDwellTimeMS(
    /* [in] */ Int32 dwellTimeMS)
{
    mDwellTimeMS = dwellTimeMS;
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
