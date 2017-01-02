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

#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFINETWORKSCORECACHE_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFINETWORKSCORECACHE_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Net::IScoredNetwork;
using Elastos::Droid::Net::IINetworkScoreCache;
using Elastos::Droid::Wifi::IScanResult;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

class WifiNetworkScoreCache
    : public Object
    , public IINetworkScoreCache
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    WifiNetworkScoreCache();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI UpdateScores(
        /* [in] */ IList* networks);//android::net::ScoredNetwork

    // @Override
    CARAPI ClearScores();

    CARAPI IsScoredNetwork(
        /* [in] */ IScanResult* result,
        /* [out] */ Boolean* isScoreNetwork);

    CARAPI GetNetworkScore(
        /* [in] */ IScanResult* result,
        /* [out] */ Int32* rValue);

    CARAPI GetNetworkScore(
        /* [in] */ IScanResult* result,
        /* [in] */ Int32 rssiBoost,
        /* [out] */ Int32* rValue);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info)
        return Object::ToString(info);
    }

protected:
    // @Override
    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(String) BuildNetworkKey(
        /* [in] */ IScoredNetwork* network);

    CARAPI_(String) BuildNetworkKey(
        /* [in] */ IScanResult* result);

public:
    // A Network scorer returns a score in the range [-127, +127]
    static Int32 INVALID_NETWORK_SCORE;// = 100000;

private:
    static String TAG;
    Boolean DBG;
    AutoPtr<IContext> mContext;
    // The key is of the form "<ssid>"<bssid>
    // TODO: What about SSIDs that can't be encoded as UTF-8?
    AutoPtr<IMap> mNetworkCache;//String, ScoredNetwork
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFINETWORKSCORECACHE_H__
