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

#ifndef __ELASTOS_DROID_SERVER_WIFI_NETWORKUPDATERESULT_H__
#define __ELASTOS_DROID_SERVER_WIFI_NETWORKUPDATERESULT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

class NetworkUpdateResult : public Object
{
public:
    NetworkUpdateResult(
        /* [in] */ Int32 id);

    NetworkUpdateResult(
        /* [in] */ Boolean ip,
        /* [in] */ Boolean proxy);

    CARAPI_(void) SetNetworkId(
        /* [in] */ Int32 id);

    CARAPI_(Int32) GetNetworkId();

    CARAPI_(void) SetIpChanged(
        /* [in] */ Boolean ip);

    CARAPI_(Boolean) HasIpChanged();

    CARAPI_(void) SetProxyChanged(
        /* [in] */ Boolean proxy);

    CARAPI_(Boolean) HasProxyChanged();

    CARAPI_(Boolean) IsNewNetwork();

    CARAPI_(void) SetIsNewNetwork(
        /* [in] */ Boolean isNew);

private:
    Int32 mNetId;
    Boolean mIpChanged;
    Boolean mProxyChanged;
    Boolean mIsNewNetwork;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_NETWORKUPDATERESULT_H__
