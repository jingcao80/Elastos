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

#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFIAUTOJOINCONTROLLER_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFIAUTOJOINCONTROLLER_H__

#include "elastos/droid/ext/frameworkext.h"

// package com.android.server.wifi;
// import android.content.Context;
// import android.net.NetworkKey;
// import android.net.NetworkScoreManager;
// import android.net.WifiKey;
// import android.net.wifi.*;
// import android.os.SystemClock;
// import android.os.Process;
// import android.text.TextUtils;
// import android.util.Log;
// import java.util.ArrayList;
// import java.util.Iterator;
// import java.util.HashMap;
// import java.util.List;

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

/**
  * AutoJoin controller is responsible for WiFi Connect decision
  *
  * It runs in the thread context of WifiStateMachine
  *
  */
class WifiAutoJoinController
    : public Object
{
public:
    WifiAutoJoinController(
        /* [in] */ IContext* c,
        /* [in] */ WifiStateMachine* w,
        /* [in] */ WifiConfigStore* s,
        /* [in] */ WifiConnectionStatistics* st,
        /* [in] */ WifiNative* n);

    virtual CARAPI EnableVerboseLogging(
        /* [in] */ Int32 verbose);

    virtual CARAPI AddToScanCache(
        /* [in] */ IList<ScanResult*>* scanList,
        /* [out] */ Int32* result);

    virtual CARAPI LogDbg(
        /* [in] */ String message);

    virtual CARAPI LogDbg(
        /* [in] */ String message,
        /* [in] */ Boolean stackTrace);

    // Called directly from WifiStateMachine
    virtual CARAPI NewSupplicantResults(
        /* [in] */ Boolean doAutoJoin,
        /* [out] */ Int32* result);

    /**
      * Not used at the moment
      * should be a call back from WifiScanner HAL ??
      * this function is not hooked and working yet, it will receive scan results from WifiScanners
      * with the list of IEs,then populate the capabilities by parsing the IEs and inject the scan
      * results as normal.
      */
    virtual CARAPI NewHalScanResults();

    /**
      *  network link quality changed, called directly from WifiTrafficPoller,
      * or by listening to Link Quality intent
      */
    virtual CARAPI LinkQualitySignificantChange();

    /**
      * update the network history fields fo that configuration
      * - if userTriggered, we mark the configuration as "non selfAdded" since the user has seen it
      * and took over management
      * - if it is a "connect", remember which network were there at the point of the connect, so
      * as those networks get a relative lower score than the selected configuration
      *
      * @param netId
      * @param userTriggered : if the update come from WiFiManager
      * @param connect : if the update includes a connect
      */
    virtual CARAPI UpdateConfigurationHistory(
        /* [in] */ Int32 netId,
        /* [in] */ Boolean userTriggered,
        /* [in] */ Boolean connect);

    virtual CARAPI GetConnectChoice(
        /* [in] */ WifiConfiguration* source,
        /* [in] */ WifiConfiguration* target,
        /* [out] */ Int32* result);

    virtual CARAPI GetScoreFromVisibility(
        /* [in] */ WifiConfiguration* ::Visibility* visibility,
        /* [in] */ Int32 rssiBoost,
        /* [in] */ String dbg,
        /* [out] */ Int32* result);

    // Compare WifiConfiguration by RSSI, and return a comparison value in the range [-50, +50]
    // The result represents "approximately" an RSSI difference measured in dBM
    // Adjusted with various parameters:
    // +) current network gets a +15 boost
    // +) 5GHz signal, if they are strong enough, get a +15 or +25 boost, representing the
    // fact that at short range we prefer 5GHz band as it is cleaner of interference and
    // provides for wider channels
    virtual CARAPI CompareWifiConfigurationsRSSI(
        /* [in] */ WifiConfiguration* a,
        /* [in] */ WifiConfiguration* b,
        /* [in] */ String currentConfiguration,
        /* [out] */ Int32* result);

    virtual CARAPI CompareWifiConfigurationsWithScorer(
        /* [in] */ WifiConfiguration* a,
        /* [in] */ WifiConfiguration* b,
        /* [out] */ Int32* result);

    virtual CARAPI CompareWifiConfigurations(
        /* [in] */ WifiConfiguration* a,
        /* [in] */ WifiConfiguration* b,
        /* [out] */ Int32* result);

    virtual CARAPI IsBadCandidate(
        /* [in] */ Int32 rssi5,
        /* [in] */ Int32 rssi24,
        /* [out] */ Boolean* result);

    virtual CARAPI CompareWifiConfigurationsTop(
        /* [in] */ WifiConfiguration* a,
        /* [in] */ WifiConfiguration* b,
        /* [out] */ Int32* result);

    virtual CARAPI RssiBoostFrom5GHzRssi(
        /* [in] */ Int32 rssi,
        /* [in] */ String dbg,
        /* [out] */ Int32* result);

    /**
          * attemptRoam() function implements the core of the same SSID switching algorithm
          *
          * Run thru all recent scan result of a WifiConfiguration and select the
          * best one.
          */
    virtual CARAPI AttemptRoam(
        /* [in] */ ScanResult* a,
        /* [in] */ WifiConfiguration* current,
        /* [in] */ Int32 age,
        /* [in] */ String currentBSSID,
        /* [out] */ ScanResult** result);

    /**
      * getNetworkScore()
      *
      * if scorer is present, get the network score of a WifiConfiguration
      *
      * Note: this should be merge with setVisibility
      *
      * @param config
      * @return score
      */
    virtual CARAPI GetConfigNetworkScore(
        /* [in] */ WifiConfiguration* config,
        /* [in] */ Int32 age,
        /* [in] */ Int32 rssiBoost,
        /* [out] */ Int32* result);

    virtual CARAPI HandleBSSIDBlackList(
        /* [in] */ Boolean enable,
        /* [in] */ String bssid,
        /* [in] */ Int32 reason);

    virtual CARAPI IsBlacklistedBSSID(
        /* [in] */ String bssid,
        /* [out] */ Boolean* result);

    /**
      * attemptAutoJoin() function implements the core of the a network switching algorithm
      */
    virtual CARAPI AttemptAutoJoin();

private:
    /**
      * Flush out scan results older than mScanResultMaximumAge
      *
      */
    CARAPI_(void) AgeScanResultsOut(
        /* [in] */ Int32 delay);

    /**
      * compare a WifiConfiguration against the current network, return a delta score
      * If not associated, and the candidate will always be better
      * For instance if the candidate is a home network versus an unknown public wifi,
      * the delta will be infinite, else compare Kepler scores etc…
      * Negatve return values from this functions are meaningless per se, just trying to
      * keep them distinct for debug purpose (i.e. -1, -2 etc...)
      */
    CARAPI_(Int32) CompareNetwork(
        /* [in] */ WifiConfiguration* candidate,
        /* [in] */ String lastSelectedConfiguration);

public:
    static Int32 mScanResultMaximumAge = 40000;
    /* for debug purpose only : the untrusted SSID we would be connected to if we had VPN */
    String lastUntrustedBSSID;
    /* For debug purpose only: if the scored override a score */
    Boolean didOverride;
    static const Int32 AUTO_JOIN_IDLE = 0;
    static const Int32 AUTO_JOIN_ROAMING = 1;
    static const Int32 AUTO_JOIN_EXTENDED_ROAMING = 2;
    static const Int32 AUTO_JOIN_OUT_OF_NETWORK_ROAMING = 3;
    static const Int32 HIGH_THRESHOLD_MODIFIER = 5;
    // Below are AutoJoin wide parameters indicating if we should be aggressive before joining
    // weak network. Note that we cannot join weak network that are going to be marked as unanted by
    // ConnectivityService because this will trigger link flapping.
    /**
      * There was a non-blacklisted configuration that we bailed from because of a weak signal
      */
    Boolean didBailDueToWeakRssi;
    /**
      * number of time we consecutively bailed out of an eligible network because its signal
      * was too weak
      */
    Int32 weakRssiBailCount;

private:
    AutoPtr<IContext> mContext;
    AutoPtr<WifiStateMachine> mWifiStateMachine;
    AutoPtr<WifiConfigStore> mWifiConfigStore;
    AutoPtr<WifiNative> mWifiNative;
    AutoPtr<INetworkScoreManager> scoreManager;
    AutoPtr<WifiNetworkScoreCache> mNetworkScoreCache;
    static const String TAG;
    static Boolean DBG;
    static Boolean VDBG;
    static const Boolean mStaStaSupported;
    static const Int32 SCAN_RESULT_CACHE_SIZE = 80;
    /* milliseconds unit */
    String mCurrentConfigurationKey;
    //used by autojoin
    AutoPtr< IHashMap<String, ScanResult> > scanResultCache;
    AutoPtr< IArrayList<String> > mBlacklistedBssids;
    AutoPtr<WifiConnectionStatistics> mWifiConnectionStatistics;
    // Lose the non-auth failure blacklisting after 8 hours
    static const Int64 loseBlackListHardMilli = 1000 * 60 * 60 * 8;
    // Lose some temporary blacklisting after 30 minutes
    static const Int64 loseBlackListSoftMilli = 1000 * 60 * 30;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFIAUTOJOINCONTROLLER_H__

