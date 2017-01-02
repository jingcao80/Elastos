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

#ifndef __ELASTOS_DROID_NET_WIFI_CSCANRESULTHELPER_H__
#define __ELASTOS_DROID_NET_WIFI_CSCANRESULTHELPER_H__

#include "_Elastos_Droid_Wifi_CScanResultHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CScanResultHelper)
    , public Singleton
    , public IScanResultHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * @hide
     * TODO: makes real freq boundaries
     */
    CARAPI Is24GHz(
        /* [in] */ Int32 freq,
        /* [out] */ Boolean* result);

    /**
     * @hide
     * TODO: makes real freq boundaries
     */
    CARAPI Is5GHz(
        /* [in] */ Int32 freq,
        /* [out] */ Boolean* result);
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CSCANRESULTHELPER_H__
