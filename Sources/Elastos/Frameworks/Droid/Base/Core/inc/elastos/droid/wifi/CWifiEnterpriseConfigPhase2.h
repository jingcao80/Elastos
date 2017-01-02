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

#ifndef __ELASTOS_DROID_NET_WIFI_CWIFIENTERPRISECONFIGPHASE2_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFIENTERPRISECONFIGPHASE2_H__

#include "_Elastos_Droid_Wifi_CWifiEnterpriseConfigPhase2.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiEnterpriseConfigPhase2)
    , public Singleton
    , public IWifiEnterpriseConfigPhase2
{
    friend class CWifiEnterpriseConfig;
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CWifiEnterpriseConfigPhase2();

    /** @hide */
    CARAPI GetStrings(
        /* [out, callee] */ ArrayOf<String>** result);

private:
    static const String PREFIX;
    AutoPtr< ArrayOf<String> > mStrings;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFIENTERPRISECONFIGPHASE2_H__
