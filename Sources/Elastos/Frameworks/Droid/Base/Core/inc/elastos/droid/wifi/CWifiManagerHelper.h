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

#ifndef __ELASTOS_DROID_NET_WIFI_CWIFIMANAGERHELPER_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFIMANAGERHELPER_H__

#include "_Elastos_Droid_Wifi_CWifiManagerHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiManagerHelper)
    , public Singleton
    , public IWifiManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CalculateSignalLevel(
        /* [in] */ Int32 rssi,
        /* [in] */ Int32 numLevels,
        /* [out] */ Int32* level);

    CARAPI CompareSignalLevel(
        /* [in] */ Int32 rssiA,
        /* [in] */ Int32 rssiB,
        /* [out] */ Int32* result);
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_NET_WIFI_CWIFIMANAGERHELPER_H__
