/*
  * Copyright (C) 2013 The Android Open Source Project
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

#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFITRAFFICPOLLER_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFITRAFFICPOLLER_H__

#include "elastos/droid/ext/frameworkext.h"

// package com.android.server.wifi;
// import android.content.BroadcastReceiver;
// import android.content.Context;
// import android.content.Intent;
// import android.content.IntentFilter;
// import android.net.NetworkInfo;
// import static android.net.NetworkInfo.DetailedState.CONNECTED;
// import android.net.TrafficStats;
// import android.net.wifi.WifiManager;
// import android.os.Messenger;
// import android.os.RemoteException;
// import android.os.Handler;
// import android.os.Message;
// import android.util.Log;
// import java.io.FileDescriptor;
// import java.io.PrintWriter;
// import java.util.ArrayList;
// import java.util.List;
// import java.util.concurrent.atomic.AtomicBoolean;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Io::IFileDescriptor;
using Elastos::Io::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

/* Polls for traffic stats and notifies the clients */
class WifiTrafficPoller
    : public Object
{
private:
    class InnerBroadcastReceiver1
        : public Object
        , public IBroadcastReceiver
    {
    public:
        InnerBroadcastReceiver1(
            /* [in] */ WifiTrafficPoller* owner);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WifiTrafficPoller* mOwner;
    };

    class TrafficHandler
        : public Object
        , public IHandler
    {
    public:
        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    };

public:
    WifiTrafficPoller(
        /* [in] */ IContext* context,
        /* [in] */ String iface);

    virtual CARAPI AddClient(
        /* [in] */ IMessenger* client);

    virtual CARAPI RemoveClient(
        /* [in] */ IMessenger* client);

    virtual CARAPI EnableVerboseLogging(
        /* [in] */ Int32 verbose);

    virtual CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(void) EvaluateTrafficStatsPolling();

    CARAPI_(void) NotifyOnDataActivity();

private:
    Boolean DBG;
    Boolean VDBG;
    /*const*/ String TAG;
    /**
      * Interval in milliseconds between polling for traffic
      * statistics
      */
    static const Int32 POLL_TRAFFIC_STATS_INTERVAL_MSECS = 1000;
    static const Int32 ENABLE_TRAFFIC_STATS_POLL = 1;
    static const Int32 TRAFFIC_STATS_POLL = 2;
    static const Int32 ADD_CLIENT = 3;
    static const Int32 REMOVE_CLIENT = 4;
    Boolean mEnableTrafficStatsPoll;
    Int32 mTrafficStatsPollToken;
    Int64 mTxPkts;
    Int64 mRxPkts;
    /* Tracks last reported data activity */
    Int32 mDataActivity;
    /*const*/ AutoPtr< IList<IMessenger> > mClients;
    // err on the side of updating at boot since screen on broadcast may be missed
    // the first time
    AutoPtr<IAtomicBoolean> mScreenOn;
    /*const*/ AutoPtr<TrafficHandler> mTrafficHandler;
    AutoPtr<INetworkInfo> mNetworkInfo;
    /*const*/ String mInterface;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFITRAFFICPOLLER_H__

