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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/wifi/CWifiManagerHelper.h"
#include "elastos/droid/wifi/CWifiManager.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CWifiManagerHelper, Singleton, IWifiManagerHelper)

CAR_SINGLETON_IMPL(CWifiManagerHelper)

ECode CWifiManagerHelper::CalculateSignalLevel(
    /* [in] */ Int32 rssi,
    /* [in] */ Int32 numLevels,
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level);
    *level = CWifiManager::CalculateSignalLevel(rssi, numLevels);
    return NOERROR;
}

ECode CWifiManagerHelper::CompareSignalLevel(
    /* [in] */ Int32 rssiA,
    /* [in] */ Int32 rssiB,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CWifiManager::CompareSignalLevel(rssiA, rssiB);
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
