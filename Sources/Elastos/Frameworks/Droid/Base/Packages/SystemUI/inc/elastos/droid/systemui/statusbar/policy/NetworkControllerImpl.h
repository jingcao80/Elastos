
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_NETWORKCONTROLLERIMPL_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_NETWORKCONTROLLERIMPL_H__

#include "_SystemUI.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Wifi.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/AsyncTask.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telephony::IPhoneStateListener;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::SystemUI::StatusBar::Phone::IStatusBarHeaderView;
using Elastos::Core::Object;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

/** Platform implementation of the network controller. **/
class NetworkControllerImpl
    : public BroadcastReceiver
    , public INetworkControllerImpl
    , public INetworkController
    , public IDemoMode
{
private:
    // ===== Telephony ==============================================================
    class NCPhoneStateListener
        : public Object //PhoneStateListener
    {
    public:
        NCPhoneStateListener(
            /* [in] */ NetworkControllerImpl* host);

        // @Override
        CARAPI OnSignalStrengthsChanged(
            /* [in] */ ISignalStrength* signalStrength);

        // @Override
        CARAPI OnServiceStateChanged(
            /* [in] */ IServiceState* state);

        // @Override
        CARAPI OnCallStateChanged(
            /* [in] */ Int32 state,
            /* [in] */ const String& incomingNumber);

        // @Override
        CARAPI OnDataConnectionStateChanged(
            /* [in] */ Int32 state,
            /* [in] */ Int32 networkType);

        // @Override
        CARAPI OnDataActivity(
            /* [in] */ Int32 direction);

    private:
        NetworkControllerImpl* mHost;
    };

    // ===== Wifi ===================================================================
    class WifiHandler: public Handler
    {
    public:
        WifiHandler(
            /* [in] */ NetworkControllerImpl* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        NetworkControllerImpl* mHost;
    };

    class MobileDataControllerCallback
        : public Object
        , public IMobileDataControllerCallback
    {
    public:
        CAR_INTERFACE_DECL();

        MobileDataControllerCallback(
            /* [in] */ NetworkControllerImpl* host);

        // @Override
        CARAPI OnMobileDataEnabled(
            /* [in] */ Boolean enabled);

    private:
        NetworkControllerImpl* mHost;
    };

    class NCIAsyncTask: public AsyncTask
    {
    public:
        NCIAsyncTask(
            /* [in] */ NetworkControllerImpl* host,
            /* [in] */ Boolean enabled);

    protected:
        // @Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

    private:
        NetworkControllerImpl* mHost;
        Boolean mEnabled;
    };

public:
    CAR_INTERFACE_DECL();

    /**
     * Construct this controller object and register for updates.
     */
    NetworkControllerImpl(
        /* [in] */ IContext* context);

    CARAPI HasMobileDataFeature(
        /* [out] */ Boolean* has);

    CARAPI HasVoiceCallingFeature(
        /* [out] */ Boolean* has);

    CARAPI IsEmergencyOnly(
        /* [out] */ Boolean* result);

    CARAPI AddCombinedLabelView(
        /* [in] */ ITextView* v);

    CARAPI AddMobileLabelView(
        /* [in] */ ITextView* v);

    CARAPI AddWifiLabelView(
        /* [in] */ ITextView* v);

    CARAPI AddEmergencyLabelView(
        /* [in] */ IStatusBarHeaderView* v);

    CARAPI AddSignalCluster(
        /* [in] */ INetworkControllerImplSignalCluster* cluster);

    CARAPI AddNetworkSignalChangedCallback(
        /* [in] */ INetworkSignalChangedCallback* cb);

    CARAPI RemoveNetworkSignalChangedCallback(
        /* [in] */ INetworkSignalChangedCallback* cb);

    // @Override
    CARAPI AddAccessPointCallback(
        /* [in] */ INetworkControllerAccessPointCallback* callback);

    // @Override
    CARAPI RemoveAccessPointCallback(
        /* [in] */ INetworkControllerAccessPointCallback* callback);

    // @Override
    CARAPI ScanForAccessPoints();

    // @Override
    CARAPI Connect(
        /* [in] */ INetworkControllerAccessPoint* ap);

    // @Override
    CARAPI SetWifiEnabled(
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI GetDataUsageInfo(
        /* [out] */ INetworkControllerDataUsageInfo** result);

    // @Override
    CARAPI IsMobileDataSupported(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsMobileDataEnabled(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetMobileDataEnabled(
        /* [in] */ Boolean enabled);

    CARAPI RefreshSignalCluster(
        /* [in] */ INetworkControllerImplSignalCluster* cluster);

    CARAPI_(void) NotifySignalsChangedCallbacks(
        /* [in] */ INetworkSignalChangedCallback* cb);

    CARAPI SetStackedMode(
        /* [in] */ Boolean stacked);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    CARAPI_(Boolean) IsCdmaEri();

    CARAPI_(void) UpdateNetworkName(
        /* [in] */ Boolean showSpn,
        /* [in] */ const String& spn,
        /* [in] */ Boolean showPlmn,
        /* [in] */ const String& plmn);

    // ===== Update the views =======================================================
    CARAPI_(void) RefreshViews();

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    // @Override
    CARAPI DispatchDemoCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle* args);

private:
    CARAPI_(void) NotifyMobileDataEnabled(
        /* [in] */ Boolean enabled);

    CARAPI_(Boolean) IsTypeIconWide(
        /* [in] */ Int32 iconId);

    CARAPI_(Boolean) IsQsTypeIconWide(
        /* [in] */ Int32 iconId);

    CARAPI_(void) UpdateSimState(
        /* [in] */ IIntent* intent);

    CARAPI_(Boolean) IsCdma();

    CARAPI_(Boolean) HasService();

    CARAPI_(void) UpdateAirplaneMode();

    CARAPI_(void) RefreshLocale();

    CARAPI_(void) UpdateTelephonySignalStrength();

    CARAPI_(Int32) InetConditionForNetwork(
        /* [in] */ Int32 networkType);

    CARAPI_(void) UpdateDataNetType();

    CARAPI_(Boolean) IsRoaming();

    CARAPI_(void) UpdateDataIcon();

    CARAPI_(void) UpdateWifiState(
        /* [in] */ IIntent* intent);

    CARAPI_(void) UpdateWifiIcons();

    CARAPI_(String) HuntForSsid(
        /* [in] */ IWifiInfo* info);

    // ===== Wimax ===================================================================
    CARAPI_(void) UpdateWimaxState(
        /* [in] */ IIntent* intent);

    CARAPI_(void) UpdateWimaxIcons();

    // ===== Full or limited Internet connectivity ==================================
    CARAPI_(void) UpdateConnectivity(
        /* [in] */ IIntent* intent);

    CARAPI_(String) GetResourceName(
        /* [in] */ Int32 resId);

protected:
    // debug
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean CHATTY; // additional diagnostics, but not logspew

    // telephony
    Boolean mHspaDataDistinguishable;
    AutoPtr<ITelephonyManager> mPhone;
    Boolean mDataConnected;
    IccCardConstantsState mSimState;
    Int32 mPhoneState;
    Int32 mDataNetType;
    Int32 mDataState;
    Int32 mDataActivity;
    AutoPtr<IServiceState> mServiceState;
    AutoPtr<ISignalStrength> mSignalStrength;
    const Int32* mDataIconList;
    String mNetworkName;
    String mNetworkNameDefault;
    String mNetworkNameSeparator;
    Int32 mPhoneSignalIconId;
    Int32 mQSPhoneSignalIconId;
    Int32 mDataDirectionIconId; // data + data direction on phones
    Int32 mDataSignalIconId;
    Int32 mDataTypeIconId;
    Int32 mQSDataTypeIconId;
    Int32 mAirplaneIconId;
    Boolean mDataActive;
    Boolean mNoSim;
    Int32 mLastSignalLevel;
    Boolean mShowPhoneRSSIForData;
    Boolean mShowAtLeastThreeGees;
    Boolean mAlwaysShowCdmaRssi;

    String mContentDescriptionPhoneSignal;
    String mContentDescriptionWifi;
    String mContentDescriptionWimax;
    String mContentDescriptionCombinedSignal;
    String mContentDescriptionDataType;

    // wifi
    AutoPtr<IWifiManager> mWifiManager;
    AutoPtr<IAsyncChannel> mWifiChannel;
    Boolean mWifiEnabled;
    Boolean mWifiConnected;
    Int32 mWifiRssi;
    Int32 mWifiLevel;
    String mWifiSsid;
    Int32 mWifiIconId;
    Int32 mQSWifiIconId;
    Int32 mWifiActivity;

    // bluetooth
    Boolean mBluetoothTethered;
    Int32 mBluetoothTetherIconId;

private:
    //wimax
    Boolean mWimaxSupported;
    Boolean mIsWimaxEnabled;
    Boolean mWimaxConnected;
    Boolean mWimaxIdle;
    Int32 mWimaxIconId;
    Int32 mWimaxSignal;
    Int32 mWimaxState;
    Int32 mWimaxExtraState;

    // data connectivity (regardless of state, can we access the internet?)
    // state of inet connection - 0 not connected, 100 connected
    Boolean mConnected;
    Int32 mConnectedNetworkType;
    String mConnectedNetworkTypeName;
    Int32 mLastConnectedNetworkType;

    Int32 mInetCondition;
    Int32 mLastInetCondition;
    static const Int32 INET_CONDITION_THRESHOLD;

    Boolean mAirplaneMode;
    Boolean mLastAirplaneMode;

    AutoPtr<ILocale> mLocale;
    AutoPtr<ILocale> mLastLocale;

    // our ui
    AutoPtr<IContext> mContext;
    AutoPtr<IArrayList/*<TextView*/> mCombinedLabelViews;
    AutoPtr<IArrayList/*<TextView*/> mMobileLabelViews;
    AutoPtr<IArrayList/*<TextView*/> mWifiLabelViews;
    AutoPtr<IArrayList/*<StatusBarHeaderView*/> mEmergencyViews;
    AutoPtr<IArrayList/*<SignalCluster*/> mSignalClusters;
    AutoPtr<IArrayList/*<NetworkSignalChangedCallback*/> mSignalsChangedCallbacks;
    Int32 mLastPhoneSignalIconId;
    Int32 mLastDataDirectionIconId;
    Int32 mLastWifiIconId;
    Int32 mLastWimaxIconId;
    Int32 mLastCombinedSignalIconId;
    Int32 mLastDataTypeIconId;
    String mLastCombinedLabel;

    Boolean mHasMobileDataFeature;

    Boolean mDataAndWifiStacked;

    AutoPtr<IWifiAccessPointController> mAccessPoints;
    AutoPtr<IMobileDataController> mMobileDataController;

    Boolean mDemoMode;
    Int32 mDemoInetCondition;
    Int32 mDemoWifiLevel;
    Int32 mDemoDataTypeIconId;
    Int32 mDemoQSDataTypeIconId;
    Int32 mDemoMobileLevel;
    AutoPtr<IPhoneStateListener> mPhoneStateListener;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_NETWORKCONTROLLERIMPL_H__
