/*
  * Copyright (C) 2014 The Android Open Source Project
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *      http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */

#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFINETWORKSCORECACHE_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFINETWORKSCORECACHE_H__

#include "elastos/droid/ext/frameworkext.h"

// package com.android.server.wifi;
// import android.Manifest.permission;
// import android.content.Context;
// import android.net.INetworkScoreCache;
// import android.net.NetworkKey;
// import android.net.ScoredNetwork;
// import android.net.wifi.ScanResult;
// import android.net.wifi.WifiManager;
// import android.util.Log;
// import java.io.FileDescriptor;
// import java.io.PrintWriter;
// import java.util.HashMap;
// import java.util.List;
// import java.util.Map;

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

class WifiNetworkScoreCache
    : public Object
    , public INetworkScoreCache::Stub
{
public:
    WifiNetworkScoreCache(
        /* [in] */ IContext* context);

    // @Override public final void updateScores(List<android.net.ScoredNetwork> networks) {
    Log.e(
        /* [in] */  TAG);

    Synchronized(
        /* [in] */  mNetworkCache);

    );

public:
    // A Network scorer returns a score in the range [-127, +127]
    static Int32 INVALID_NETWORK_SCORE = 100000;

private:
    static String TAG;
    Boolean DBG;
    /*const*/ AutoPtr<IContext> mContext;
    // The key is of the form "<ssid>"<bssid>
    // TODO: What about SSIDs that can't be encoded as UTF-8?
    /*const*/  Map<String;
    /*const*/  ScoredNetwork> mNetworkCache;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFINETWORKSCORECACHE_H__

