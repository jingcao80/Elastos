/*
  * Copyright (C) 2010 The Android Open Source Project
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

#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFISERVICE_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFISERVICE_H__

#include "elastos/droid/ext/frameworkext.h"

// package com.android.server.wifi;
// import android.content.Context;
// import android.net.wifi.ScanResult;
// import android.net.wifi.WifiScanner;
// import android.util.Log;
// import com.android.server.SystemService;
// import java.util.List;

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

class WifiService
    : public Object
    , public ISystemService
{
public:
    WifiService(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

public:
    /*const*/ AutoPtr<WifiServiceImpl> mImpl;

private:
    static const String TAG;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFISERVICE_H__

