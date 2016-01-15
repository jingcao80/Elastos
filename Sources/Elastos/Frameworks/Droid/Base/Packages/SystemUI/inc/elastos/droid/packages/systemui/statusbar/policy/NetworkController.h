
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_CNETWORKCONTROLLER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_CNETWORKCONTROLLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include "elastos/droid/os/HandlerBase.h"
#include "elastos/droid/utility/AsyncChannel.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/R.h"

using Elastos::Utility::Etl::List;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Internal::Utility::AsyncChannel;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class NetworkController
    : public BroadcastReceiver
    , public INetworkController
{
private:
    class WifiHandler
        : public HandlerBase
    {
    public:

        WifiHandler(
            /* [in] */ NetworkController* host);

        virtual ECode Run();

        virtual ECode HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        NetworkController* mHost;
    };

    // PhoneStateListener mPhoneStateListener = new PhoneStateListener() {
    //     @Override
    //     public void onSignalStrengthsChanged(SignalStrength signalStrength) {
    //         if (DEBUG) {
    //             Slog.d(TAG, "onSignalStrengthsChanged signalStrength=" + signalStrength +
    //                 ((signalStrength == null) ? "" : (" level=" + signalStrength.getLevel())));
    //         }
    //         mSignalStrength = signalStrength;
    //         updateTelephonySignalStrength();
    //         refreshViews();
    //     }

    //     @Override
    //     public void onServiceStateChanged(ServiceState state) {
    //         if (DEBUG) {
    //             Slog.d(TAG, "onServiceStateChanged state=" + state.getState());
    //         }
    //         mServiceState = state;
    //         updateTelephonySignalStrength();
    //         updateDataNetType();
    //         updateDataIcon();
    //         refreshViews();
    //     }

    //     @Override
    //     public void onCallStateChanged(int state, String incomingNumber) {
    //         if (DEBUG) {
    //             Slog.d(TAG, "onCallStateChanged state=" + state);
    //         }
    //         // In cdma, if a voice call is made, RSSI should switch to 1x.
    //         if (isCdma()) {
    //             updateTelephonySignalStrength();
    //             refreshViews();
    //         }
    //     }

    //     @Override
    //     public void onDataConnectionStateChanged(int state, int networkType) {
    //         if (DEBUG) {
    //             Slog.d(TAG, "onDataConnectionStateChanged: state=" + state
    //                     + " type=" + networkType);
    //         }
    //         mDataState = state;
    //         mDataNetType = networkType;
    //         updateDataNetType();
    //         updateDataIcon();
    //         refreshViews();
    //     }

    //     @Override
    //     public void onDataActivity(int direction) {
    //         if (DEBUG) {
    //             Slog.d(TAG, "onDataActivity: direction=" + direction);
    //         }
    //         mDataActivity = direction;
    //         updateDataIcon();
    //         refreshViews();
    //     }
    // };

public:
    NetworkController();

    NetworkController(
        /* [in] */ IContext * pCtx);

    virtual ~NetworkController();

    virtual CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    virtual CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    virtual CARAPI_(UInt32) AddRef();

    virtual CARAPI_(UInt32) Release();

    CARAPI HasMobileDataFeature(
        /* [out] */ Boolean* result);

    CARAPI IsEmergencyOnly(
        /* [out] */ Boolean* result);

    CARAPI AddPhoneSignalIconView(
        /* [in] */ IImageView* v);

    CARAPI AddDataDirectionIconView(
        /* [in] */ IImageView* v);

    CARAPI AddDataDirectionOverlayIconView(
        /* [in] */ IImageView* v);

    CARAPI AddWifiIconView(
        /* [in] */ IImageView* v);

    CARAPI AddWimaxIconView(
        /* [in] */ IImageView* v);

    CARAPI AddCombinedSignalIconView(
        /* [in] */ IImageView* v);

    CARAPI AddDataTypeIconView(
        /* [in] */ IImageView* v);

    CARAPI AddCombinedLabelView(
        /* [in] */ ITextView* v);

    CARAPI AddMobileLabelView(
        /* [in] */ ITextView* v);

    CARAPI AddWifiLabelView(
        /* [in] */ ITextView* v);

    CARAPI AddEmergencyLabelView(
        /* [in] */ ITextView* v);

    CARAPI AddSignalCluster(
        /* [in] */ INetworkControllerSignalCluster* cluster);

    CARAPI AddNetworkSignalChangedCallback(
        /* [in] */ INetworkSignalChangedCallback* cb);

    CARAPI RefreshSignalCluster(
        /* [in] */ INetworkControllerSignalCluster* cluster);

    CARAPI SetStackedMode(
        /* [in] */ Boolean stacked);

    CARAPI NotifySignalsChangedCallbacks(
        /* [in] */ INetworkSignalChangedCallback* cb);

    CARAPI IsCdmaEri(
        /* [out] */ Boolean* result);

    CARAPI UpdateNetworkName(
        /* [in] */ Boolean showSpn,
        /* [in] */ const String& spn,
        /* [in] */ Boolean showPlmn,
        /* [in] */ const String& plmn);

    CARAPI RefreshViews();

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info);
        *info = String("NetworkController: ");
        (*info).AppendFormat("%p", this);
        return NOERROR;
    }
private:
    CARAPI_(void) UpdateSimState(
        /* [in] */ IIntent* intent);

    CARAPI_(Boolean) IsCdma();

    CARAPI_(Boolean) HasService();

    CARAPI_(void) UpdateAirplaneMode();

    CARAPI_(void) UpdateTelephonySignalStrength();

    CARAPI_(void) UpdateDataNetType();

    CARAPI_(void) UpdateDataIcon();

    CARAPI_(void) UpdateWifiState(
        /* [in] */ IIntent* intent);

    CARAPI_(void) UpdateWifiIcons();

    CARAPI_(String) HuntForSsid(
        /* [in] */ IWifiInfo* info);

    CARAPI_(void) UpdateWimaxState(
        /* [in] */ IIntent* intent);

    CARAPI_(void) UpdateWimaxIcons();

    CARAPI_(void) UpdateEthernetState(
        /* [in] */ IIntent* intent);

    CARAPI_(void) UpdateEthernetIcons();

    CARAPI_(void) UpdateConnectivity(
        /* [in] */ IIntent* intent);

    CARAPI_(String) GetResourceName(
        /* [in] */ Int32 resId);

public:
    // debug
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean CHATTY; // additional diagnostics, but not logspew

    // telephony
    Boolean mHspaDataDistinguishable;
    // TelephonyManager mPhone;
    Boolean mDataConnected;
    // IccCardConstants.State mSimState = IccCardConstants.State.READY;
    // Int32 mPhoneState = TelephonyManager.CALL_STATE_IDLE;
    // Int32 mDataNetType = TelephonyManager.NETWORK_TYPE_UNKNOWN;
    // Int32 mDataState = TelephonyManager.DATA_DISCONNECTED;
    // Int32 mDataActivity = TelephonyManager.DATA_ACTIVITY_NONE;
    // ServiceState mServiceState;
    // SignalStrength mSignalStrength;
    // Int32[] mDataIconList = TelephonyIcons.DATA_G[0];
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
    Int32 mMobileActivityIconId; // overlay arrows for data direction
    Int32 mLastSignalLevel;
    Boolean mShowPhoneRSSIForData;
    Boolean mShowAtLeastThreeGees;
    Boolean mAlwaysShowCdmaRssi;

    String mContentDescriptionPhoneSignal;
    String mContentDescriptionWifi;
    String mContentDescriptionWimax;
    String mContentDescriptionEthernet;
    String mContentDescriptionCombinedSignal;
    String mContentDescriptionDataType;

    // wifi
    AutoPtr<IHandler> mWifiHandler;
    AutoPtr<IWifiManager> mWifiManager;
    AutoPtr<AsyncChannel> mWifiChannel;
    Boolean mWifiEnabled;
    Boolean mWifiConnected;
    Int32 mWifiRssi;
    Int32 mWifiLevel;
    String mWifiSsid;
    Int32 mWifiIconId;
    Int32 mQSWifiIconId;
    Int32 mWifiActivityIconId; // overlay arrows for wifi direction
    Int32 mWifiActivity;

    // Ethernet
    Boolean mEthernetConnected;//mShowEthIcon, mEthernetWaitingDHCP;
    Int32 mEthernetIconId;
    Int32 mEthernetActivityIconId;// = 0; // overlay arrows for ethernet direction

    // our ui
    AutoPtr<IContext> mContext;
    List<AutoPtr<IImageView> > mPhoneSignalIconViews;
    List<AutoPtr<IImageView> > mDataDirectionIconViews;
    List<AutoPtr<IImageView> > mDataDirectionOverlayIconViews;
    List<AutoPtr<IImageView> > mWifiIconViews;
    List<AutoPtr<IImageView> > mWimaxIconViews;
    List<AutoPtr<IImageView> > mCombinedSignalIconViews;
    List<AutoPtr<IImageView> > mDataTypeIconViews;
    List<AutoPtr<ITextView> > mCombinedLabelViews;
    List<AutoPtr<ITextView> > mMobileLabelViews;
    List<AutoPtr<ITextView> > mWifiLabelViews;
    List<AutoPtr<ITextView> > mEmergencyLabelViews;
    List<AutoPtr<INetworkControllerSignalCluster> > mSignalClusters;
    List<AutoPtr<INetworkSignalChangedCallback> > mSignalsChangedCallbacks;
    Int32 mLastPhoneSignalIconId;
    Int32 mLastDataDirectionIconId;
    Int32 mLastDataDirectionOverlayIconId;
    Int32 mLastWifiIconId;
    Int32 mLastWimaxIconId;
    Int32 mLastEthernetIconId;
    Int32 mLastCombinedSignalIconId;
    Int32 mLastDataTypeIconId;
    String mLastCombinedLabel;

    Boolean mDataAndWifiStacked;

    // yuck -- stop doing this here and put it in the framework
    // IBatteryStats mBatteryStats;

private:
    // bluetooth
    Boolean mBluetoothTethered;
    Int32 mBluetoothTetherIconId;// = com.android.internal.R.drawable.stat_sys_tether_bluetooth;

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
    Int32 mInetCondition;
    const static Int32 INET_CONDITION_THRESHOLD;

    Boolean mAirplaneMode;
    Boolean mLastAirplaneMode;

    Boolean mHasMobileDataFeature;

    AutoPtr<IHandler> mHandler;
    Boolean mExited;
    Int32 mPreWifiState;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_CNETWORKCONTROLLER_H__
