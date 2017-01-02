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

#ifndef __ELASTOS_DROID_SETTINGS_DEVICEINFO_CSTATUS_H__
#define __ELASTOS_DROID_SETTINGS_DEVICEINFO_CSTATUS_H__

#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Wifi.h"
#include "_Elastos_Droid_Settings_Deviceinfo_CStatus.h"
#include "elastos/droid/preference/PreferenceActivity.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/telephony/PhoneStateListener.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneStateIntentReceiver;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::PreferenceActivity;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::PhoneStateListener;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemLongClickListener;


namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

/**
 * Display the following information
 * # Phone Number
 * # Network
 * # Roaming
 * # Device Id (IMEI in GSM and MEID in CDMA)
 * # Network type
 * # Operator info (area info cell broadcast for Brazil)
 * # Signal Strength
 * # Battery Strength  : TODO
 * # Uptime
 * # Awake Time
 * # XMPP/buzz/tickle status : TODO
 *
 */
CarClass(CStatus)
    , public PreferenceActivity
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CStatus::MyHandler")

        MyHandler(
            /* [in] */ CStatus* activity);

        CARAPI constructor();

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        // WeakReference<CStatus> mStatus;
        AutoPtr<IWeakReference> mStatus;
    };

    class InitBatteryInfoReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CStatus::InitBatteryInfoReceiver")

        InitBatteryInfoReceiver(
            /* [in] */ CStatus* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CStatus* mHost;
    };

    class InitPhoneStateListener
        : public PhoneStateListener
    {
    public:
        TO_STRING_IMPL("CStatus::InitPhoneStateListener")

        InitPhoneStateListener(
            /* [in] */ CStatus* host);

        //@Override
        CARAPI OnDataConnectionStateChanged(
            /* [in] */ Int32 state);

    private:
        CStatus* mHost;
    };

    class InitAreaInfoReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CStatus::InitAreaInfoReceiver")

        InitAreaInfoReceiver(
            /* [in] */ CStatus* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CStatus* mHost;
    };

    class InitConnectivityReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CStatus::InitConnectivityReceiver")

        InitConnectivityReceiver(
            /* [in] */ CStatus* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CStatus* mHost;
    };

    class OnCreateAdapterViewOnItemLongClickListener
        : public Object
        , public IAdapterViewOnItemLongClickListener
    {
    public:
        TO_STRING_IMPL("CStatus::OnCreateAdapterViewOnItemLongClickListener");

        CAR_INTERFACE_DECL()

        OnCreateAdapterViewOnItemLongClickListener(
            /* [in] */ CStatus* host);

        //@Override
        CARAPI OnItemLongClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id,
            /* [out] */ Boolean* result);

    private:
        CStatus* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CStatus();

    CARAPI constructor();

    //@Override
    CARAPI OnPause();

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnResume();

    virtual CARAPI_(void) UpdateSignalStrength();

    virtual CARAPI_(void) UpdateConnectivity();

    virtual CARAPI_(void) UpdateTimes();

private:
    CARAPI_(Boolean) HasBluetooth();

    CARAPI_(Boolean) HasWimax();

    /**
     * Removes the specified preference, if it exists.
     * @param key the key for the Preference item
     */
    CARAPI_(void) RemovePreferenceFromScreen(
        /* [in] */ const String& key);

    /**
     * @param preference The key for the Preference item
     * @param property The system property to fetch
     * @param alt The default value, if the property doesn't exist
     */
    CARAPI_(void) SetSummary(
        /* [in] */ const String& preference,
        /* [in] */ const String& property,
        /* [in] */ const String& alt);

    CARAPI_(void) SetSummaryText(
        /* [in] */ const String& preference,
        /* [in] */ const String& _text);

    CARAPI_(void) UpdateNetworkType();

    CARAPI_(void) UpdateDataState();

    CARAPI_(void) UpdateServiceState(
        /* [in] */ IServiceState* serviceState);

    CARAPI_(void) UpdateAreaInfo(
        /* [in] */ const String& areaInfo);

    CARAPI_(void) SetWimaxStatus();

    CARAPI_(void) SetWifiStatus();

    CARAPI_(void) SetIpAddressStatus();

    CARAPI_(void) SetBtStatus();

    CARAPI_(String) Pad(
        /* [in] */ Int32 n);

    CARAPI_(String) Convert(
        /* [in] */ Int64 t);

    static CARAPI_(AutoPtr< ArrayOf<String> >) InitPHONE_RELATED_ENTRIES();

protected:
    static const String CB_AREA_INFO_RECEIVED_ACTION;

    static const String GET_LATEST_CB_AREA_INFO_ACTION;

    // Require the sender to have this permission to prevent third-party spoofing.
    static const String CB_AREA_INFO_SENDER_PERMISSION;

private:
    static const String KEY_DATA_STATE;
    static const String KEY_SERVICE_STATE;
    static const String KEY_OPERATOR_NAME;
    static const String KEY_ROAMING_STATE;
    static const String KEY_NETWORK_TYPE;
    static const String KEY_LATEST_AREA_INFO;
    static const String KEY_PHONE_NUMBER;
    static const String KEY_IMEI_SV;
    static const String KEY_IMEI;
    static const String KEY_PRL_VERSION;
    static const String KEY_MIN_NUMBER;
    static const String KEY_MEID_NUMBER;
    static const String KEY_SIGNAL_STRENGTH;
    static const String KEY_BATTERY_STATUS;
    static const String KEY_BATTERY_LEVEL;
    static const String KEY_IP_ADDRESS;
    static const String KEY_WIFI_MAC_ADDRESS;
    static const String KEY_BT_ADDRESS;
    static const String KEY_SERIAL_NUMBER;
    static const String KEY_ICC_ID;
    static const String KEY_WIMAX_MAC_ADDRESS;
    static const AutoPtr< ArrayOf<String> > PHONE_RELATED_ENTRIES;

    // Broadcasts to listen to for connectivity changes.
    static const AutoPtr< ArrayOf<String> > CONNECTIVITY_INTENTS;

    static const Int32 EVENT_SIGNAL_STRENGTH_CHANGED;
    static const Int32 EVENT_SERVICE_STATE_CHANGED;

    static const Int32 EVENT_UPDATE_STATS;

    static const Int32 EVENT_UPDATE_CONNECTIVITY;

    AutoPtr<IConnectivityManager> mCM;
    AutoPtr<ITelephonyManager> mTelephonyManager;
    AutoPtr<IWifiManager> mWifiManager;

    AutoPtr<IPhone> mPhone;
    AutoPtr<IPhoneStateIntentReceiver> mPhoneStateReceiver;
    AutoPtr<IResources> mRes;
    Boolean mShowLatestAreaInfo;

    String mUnknown;
    String mUnavailable;

    AutoPtr<IPreference> mSignalStrength;
    AutoPtr<IPreference> mUptime;
    AutoPtr<IPreference> mBatteryStatus;
    AutoPtr<IPreference> mBatteryLevel;
    AutoPtr<IPreference> mBtAddress;
    AutoPtr<IPreference> mIpAddress;
    AutoPtr<IPreference> mWifiMacAddress;
    AutoPtr<IPreference> mWimaxMacAddress;

    AutoPtr<Handler> mHandler;

    AutoPtr<BroadcastReceiver> mBatteryInfoReceiver;


    AutoPtr<PhoneStateListener> mPhoneStateListener;


    AutoPtr<BroadcastReceiver> mAreaInfoReceiver;

    AutoPtr<IIntentFilter> mConnectivityIntentFilter;
    AutoPtr<BroadcastReceiver> mConnectivityReceiver;
};

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DEVICEINFO_CSTATUS_H__
