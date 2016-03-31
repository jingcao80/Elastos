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

#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFICONTROLLER_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFICONTROLLER_H__

#include "elastos/droid/ext/frameworkext.h"

// package com.android.server.wifi;
// import android.app.AlarmManager;
// import android.app.PendingIntent;
// import android.content.BroadcastReceiver;
// import android.content.Context;
// import android.content.Intent;
// import android.content.IntentFilter;
// import android.database.ContentObserver;
// import android.net.ConnectivityManager;
// import android.net.NetworkInfo;
// import android.net.wifi.WifiConfiguration;
// import android.net.wifi.WifiManager;
// import static android.net.wifi.WifiManager.WIFI_MODE_FULL;
// import static android.net.wifi.WifiManager.WIFI_MODE_FULL_HIGH_PERF;
// import static android.net.wifi.WifiManager.WIFI_MODE_SCAN_ONLY;
// import static android.net.wifi.WifiManager.WIFI_STATE_ENABLED;
// import static android.net.wifi.WifiManager.WIFI_STATE_ENABLING;
// import android.os.Handler;
// import android.os.Looper;
// import android.os.Message;
// import android.os.SystemClock;
// import android.os.WorkSource;
// import android.provider.Settings;
// import android.util.Slog;
// import com.android.internal.util.Protocol;
// import com.android.internal.util.State;
// import com.android.internal.util.StateMachine;
// import com.android.server.wifi.WifiServiceImpl.LockList;
// import java.io.FileDescriptor;
// import java.io.PrintWriter;

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::ILooper;
using Elastos::Io::IFileDescriptor;
using Elastos::Io::IPrintWriter;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

class WifiController
    : public Object
    , public IStateMachine
{
public:
    class DefaultState
        : public Object
        , public IState
    {
    public:
        // @Override
        CARAPI_(Boolean) ProcessMessage(
            /* [in] */ IMessage* msg);
    };

    class ApStaDisabledState
        : public Object
        , public IState
    {
    public:
        // @Override
        CARAPI Enter();

        // @Override
        CARAPI_(Boolean) ProcessMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI_(Boolean) DoDeferEnable(
            /* [in] */ IMessage* msg);

    private:
        Int32 mDeferredEnableSerialNumber;
        Boolean mHaveDeferredEnable;
        Int64 mDisabledTimestamp;
    };

    class StaEnabledState
        : public Object
        , public IState
    {
    public:
        // @Override
        CARAPI Enter();

        // @Override
        CARAPI_(Boolean) ProcessMessage(
            /* [in] */ IMessage* msg);
    };

    class StaDisabledWithScanState
        : public Object
        , public IState
    {
    public:
        // @Override
        CARAPI Enter();

        // @Override
        CARAPI_(Boolean) ProcessMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI_(Boolean) DoDeferEnable(
            /* [in] */ IMessage* msg);

    private:
        Int32 mDeferredEnableSerialNumber;
        Boolean mHaveDeferredEnable;
        Int64 mDisabledTimestamp;
    };

    class ApEnabledState
        : public Object
        , public IState
    {
    public:
        // @Override
        CARAPI_(Boolean) ProcessMessage(
            /* [in] */ IMessage* msg);
    };

    class EcmState
        : public Object
        , public IState
    {
    public:
        // @Override
        CARAPI Enter();

        // @Override
        CARAPI_(Boolean) ProcessMessage(
            /* [in] */ IMessage* msg);
    };

    /* Parent: StaEnabledState */
    class DeviceActiveState
        : public Object
        , public IState
    {
    public:
        // @Override
        CARAPI Enter();

        // @Override
        CARAPI_(Boolean) ProcessMessage(
            /* [in] */ IMessage* msg);
    };

    /* Parent: DeviceActiveState. Device is active, and an app is holding a high perf lock. */
    class DeviceActiveHighPerfState
        : public Object
        , public IState
    {
    public:
        // @Override
        CARAPI Enter();
    };

    /* Parent: StaEnabledState */
    class DeviceInactiveState
        : public Object
        , public IState
    {
    public:
        // @Override
        CARAPI_(Boolean) ProcessMessage(
            /* [in] */ IMessage* msg);
    };

    /* Parent: DeviceInactiveState. Device is inactive, but an app is holding a scan only lock. */
    class ScanOnlyLockHeldState
        : public Object
        , public IState
    {
    public:
        // @Override
        CARAPI Enter();
    };

    /* Parent: DeviceInactiveState. Device is inactive, but an app is holding a full lock. */
    class FullLockHeldState
        : public Object
        , public IState
    {
    public:
        // @Override
        CARAPI Enter();
    };

    /* Parent: DeviceInactiveState. Device is inactive, but an app is holding a high perf lock. */
    class FullHighPerfLockHeldState
        : public Object
        , public IState
    {
    public:
        // @Override
        CARAPI Enter();
    };

    /* Parent: DeviceInactiveState. Device is inactive and no app is holding a wifi lock. */
    class NoLockHeldState
        : public Object
        , public IState
    {
    public:
        // @Override
        CARAPI Enter();
    };

private:
    class InnerBroadcastReceiver1
        : public Object
        , public IBroadcastReceiver
    {
    public:
        InnerBroadcastReceiver1(
            /* [in] */ WifiController* owner);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WifiController* mOwner;
    };

    class InnerContentObserver1
        : public Object
        , public IContentObserver
    {
    public:
        InnerContentObserver1(
            /* [in] */ WifiController* owner);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        WifiController* mOwner;
    };

    class InnerContentObserver3
        : public Object
        , public IContentObserver
    {
    public:
        InnerContentObserver3(
            /* [in] */ WifiController* owner);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        WifiController* mOwner;
    };

    class InnerContentObserver5
        : public Object
        , public IContentObserver
    {
    public:
        InnerContentObserver5(
            /* [in] */ WifiController* owner);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        WifiController* mOwner;
    };

public:
    WifiController(
        /* [in] */ IContext* context,
        /* [in] */ WifiServiceImpl* service,
        /* [in] */ ILooper* looper);

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(void) InitializeAndRegisterForSettingsChange(
        /* [in] */ ILooper* looper);

    CARAPI_(void) ReadStayAwakeConditions();

    CARAPI_(void) ReadWifiIdleTime();

    CARAPI_(void) ReadWifiSleepPolicy();

    CARAPI_(void) ReadWifiReEnableDelay();

    /**
      * Observes settings changes to scan always mode.
      */
    CARAPI_(void) RegisterForStayAwakeModeChange(
        /* [in] */ IHandler* handler);

    /**
      * Observes settings changes to scan always mode.
      */
    CARAPI_(void) RegisterForWifiIdleTimeChange(
        /* [in] */ IHandler* handler);

    /**
      * Observes changes to wifi sleep policy
      */
    CARAPI_(void) RegisterForWifiSleepPolicyChange(
        /* [in] */ IHandler* handler);

    /**
      * Determines whether the Wi-Fi chipset should stay awake or be put to
      * sleep. Looks at the setting for the sleep policy and the current
      * conditions.
      *
      * @see #shouldDeviceStayAwake(int)
      */
    CARAPI_(Boolean) ShouldWifiStayAwake(
        /* [in] */ Int32 pluggedType);

    /**
      * Determine whether the bit value corresponding to {@code pluggedType} is set in
      * the bit string mStayAwakeConditions. This determines whether the device should
      * stay awake based on the current plugged type.
      *
      * @param pluggedType the type of plug (USB, AC, or none) for which the check is
      * being made
      * @return {@code true} if {@code pluggedType} indicates that the device is
      * supposed to stay awake, {@code false} otherwise.
      */
    CARAPI_(Boolean) ShouldDeviceStayAwake(
        /* [in] */ Int32 pluggedType);

    CARAPI_(void) UpdateBatteryWorkSource();

    CARAPI_(void) CheckLocksAndTransitionWhenDeviceActive();

    CARAPI_(void) CheckLocksAndTransitionWhenDeviceIdle();

public:
    AutoPtr<INetworkInfo> mNetworkInfo;
    /* References to values tracked in WifiService */
    /*const*/ AutoPtr<WifiStateMachine> mWifiStateMachine;
    /*const*/ AutoPtr<WifiSettingsStore> mSettingsStore;
    /*const*/ AutoPtr<ILockList> mLocks;
    static const Int32 CMD_EMERGENCY_MODE_CHANGED = BASE + 1;
    static const Int32 CMD_SCREEN_ON = BASE + 2;
    static const Int32 CMD_SCREEN_OFF = BASE + 3;
    static const Int32 CMD_BATTERY_CHANGED = BASE + 4;
    static const Int32 CMD_DEVICE_IDLE = BASE + 5;
    static const Int32 CMD_LOCKS_CHANGED = BASE + 6;
    static const Int32 CMD_SCAN_ALWAYS_MODE_CHANGED = BASE + 7;
    static const Int32 CMD_WIFI_TOGGLED = BASE + 8;
    static const Int32 CMD_AIRPLANE_TOGGLED = BASE + 9;
    static const Int32 CMD_SET_AP = BASE + 10;
    static const Int32 CMD_DEFERRED_TOGGLE = BASE + 11;
    static const Int32 CMD_USER_PRESENT = BASE + 12;

private:
    static const String TAG;
    static const Boolean DBG;
    AutoPtr<IContext> mContext;
    Boolean mScreenOff;
    Boolean mDeviceIdle;
    Int32 mPluggedType;
    Int32 mStayAwakeConditions;
    Int64 mIdleMillis;
    Int32 mSleepPolicy;
    Boolean mFirstUserSignOnSeen;
    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<IPendingIntent> mIdleIntent;
    static const Int32 IDLE_REQUEST = 0;
    /**
      * See {@link Settings.Global#WIFI_IDLE_MS}. This is the default value if a
      * Settings.Global value is not present. This timeout value is chosen as
      * the approximate point at which the battery drain caused by Wi-Fi
      * being enabled but not active exceeds the battery drain caused by
      * re-establishing a connection to the mobile data network.
      */
    static const Int64 DEFAULT_IDLE_MS = 15 * 60 * 1000;
    /* 15 minutes */
    /**
      * See {@link Settings.Global#WIFI_REENABLE_DELAY_MS}.  This is the default value if a
      * Settings.Global value is not present.  This is the minimum time after wifi is disabled
      * we'll act on an enable.  Enable requests received before this delay will be deferred.
      */
    static const Int64 DEFAULT_REENABLE_DELAY_MS = 500;
    // finding that delayed messages can sometimes be delivered earlier than expected
    // probably rounding errors..  add a margin to prevent problems
    static const Int64 DEFER_MARGIN_MS = 5;
    static const String ACTION_DEVICE_IDLE;
    /**
      * Temporary for computing UIDS that are responsible for starting WIFI.
      * Protected by mWifiStateTracker lock.
      */
    /*const*/ AutoPtr<IWorkSource> mTmpWorkSource;
    Int64 mReEnableDelayMillis;
    static const Int32 BASE = Protocol.BASE_WIFI_CONTROLLER;
    AutoPtr<DefaultState> mDefaultState;
    AutoPtr<StaEnabledState> mStaEnabledState;
    AutoPtr<ApStaDisabledState> mApStaDisabledState;
    AutoPtr<StaDisabledWithScanState> mStaDisabledWithScanState;
    AutoPtr<ApEnabledState> mApEnabledState;
    AutoPtr<DeviceActiveState> mDeviceActiveState;
    AutoPtr<DeviceActiveHighPerfState> mDeviceActiveHighPerfState;
    AutoPtr<DeviceInactiveState> mDeviceInactiveState;
    AutoPtr<ScanOnlyLockHeldState> mScanOnlyLockHeldState;
    AutoPtr<FullLockHeldState> mFullLockHeldState;
    AutoPtr<FullHighPerfLockHeldState> mFullHighPerfLockHeldState;
    AutoPtr<NoLockHeldState> mNoLockHeldState;
    AutoPtr<EcmState> mEcmState;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFICONTROLLER_H__

