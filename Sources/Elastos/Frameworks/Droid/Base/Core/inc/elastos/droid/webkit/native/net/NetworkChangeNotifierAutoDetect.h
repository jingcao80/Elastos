// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_NET_NETWORKCHANGENOTIFIERAUTODETECT_H_
#define _ELASTOS_DROID_WEBKIT_NET_NETWORKCHANGENOTIFIERAUTODETECT_H_

#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/content/IntentFilter.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/base/ApplicationStatus.h"

// package org.chromium.net;
// import android.content.BroadcastReceiver;
// import android.content.Context;
// import android.content.Intent;
// import android.content.IntentFilter;
// import android.net.ConnectivityManager;
// import android.net.wifi.WifiInfo;
// import android.net.wifi.WifiManager;
// import android.telephony.TelephonyManager;
// import android.util.Log;
// import org.chromium.base.ApplicationState;
// import org.chromium.base.ApplicationStatus;

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IntentFilter;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Webkit::Base::ApplicationStatus;
using Elastos::Droid::Wifi::IWifiManager;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Net {

/**
  * Used by the NetworkChangeNotifier to listens to platform changes in connectivity.
  * Note that use of this class requires that the app have the platform
  * ACCESS_NETWORK_STATE permission.
  */
class NetworkChangeNotifierAutoDetect
    : public BroadcastReceiver
    , public ApplicationStatus::ApplicationStateListener
{
public:
    CAR_INTERFACE_DECL();

    /** Queries the ConnectivityManager for information about the current connection. */
    class ConnectivityManagerDelegate
        : public Object
    {
    public:
        ConnectivityManagerDelegate(
            /* [in] */ IContext* context);

        // For testing.
        ConnectivityManagerDelegate();

        virtual CARAPI_(Boolean) ActiveNetworkExists();

        virtual CARAPI_(Boolean) IsConnected();

        virtual CARAPI_(Int32) GetNetworkType();

        virtual CARAPI_(Int32) GetNetworkSubtype();

    private:
        /*const*/ AutoPtr<IConnectivityManager> mConnectivityManager;
    };

    /** Queries the WifiManager for SSID of the current Wifi connection. */
    class WifiManagerDelegate
        : public Object
    {
    public:
        WifiManagerDelegate(
            /* [in] */ IContext* context);

        // For testing.
        WifiManagerDelegate();

        virtual CARAPI_(String) GetWifiSSID();

    private:
        /*const*/ AutoPtr<IWifiManager> mWifiManager;
    };

    /**
      * Observer notified on the UI thread whenever a new connection type was detected.
      */
    class Observer
        : public Object
    {
    public:
        virtual CARAPI OnConnectionTypeChanged(
            /* [in] */ Int32 newConnectionType) = 0;
    };

private:
    class NetworkConnectivityIntentFilter
        : public IntentFilter
    {
    public:
        NetworkConnectivityIntentFilter();
    };

public:
    NetworkChangeNotifierAutoDetect(
        /* [in] */ Observer* observer,
        /* [in] */ IContext* context);

    /**
      * Allows overriding the ConnectivityManagerDelegate for tests.
      */
    virtual CARAPI SetConnectivityManagerDelegateForTests(
        /* [in] */ ConnectivityManagerDelegate* delegate);

    /**
      * Allows overriding the WifiManagerDelegate for tests.
      */
    virtual CARAPI SetWifiManagerDelegateForTests(
        /* [in] */ WifiManagerDelegate* delegate);

    virtual CARAPI Destroy();

    virtual CARAPI_(Int32) GetCurrentConnectionType();

    // BroadcastReceiver
    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    // ApplicationStatus.ApplicationStateListener
    // @Override
    // Inconsistent with ApplicationStatus.ApplicationStateListener, note it temporarily
    //CARAPI OnApplicationStateChange(
    //    /* [in] */ Int32 newState);
    virtual CARAPI_(void) OnApplicationStateChange(
        /* [in] */ Int32 newState);

private:
    /**
      * Register a BroadcastReceiver in the given context.
      */
    CARAPI RegisterReceiver();

    /**
      * Unregister the BroadcastReceiver in the given context.
      */
    CARAPI UnregisterReceiver();

    CARAPI_(String) GetCurrentWifiSSID();

    CARAPI ConnectionTypeChanged();

private:
    static const String TAG;
    /*const*/ AutoPtr<NetworkConnectivityIntentFilter> mIntentFilter;
    /*const*/ AutoPtr<Observer> mObserver;
    /*const*/ AutoPtr<IContext> mContext;
    AutoPtr<ConnectivityManagerDelegate> mConnectivityManagerDelegate;
    AutoPtr<WifiManagerDelegate> mWifiManagerDelegate;
    Boolean mRegistered;
    Int32 mConnectionType;
    String mWifiSSID;
};

} // namespace Net
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_NET_NETWORKCHANGENOTIFIERAUTODETECT_H_

