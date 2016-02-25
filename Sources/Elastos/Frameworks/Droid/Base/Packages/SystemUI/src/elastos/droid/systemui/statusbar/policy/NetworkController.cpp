#include "elastos/droid/systemui/statusbar/policy/NetworkController.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/net/wifi/CWifiManager.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include <elastos/core/StringBuilder.h>
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/systemui/statusbar/policy/WifiIcons.h"
#include "elastos/droid/systemui/statusbar/policy/AccessibilityContentDescriptions.h"
#include <cutils/properties.h>

using Elastos::Droid::R;
using Elastos::Core::StringBuilder;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::View::IView;
using Elastos::Droid::SystemUI::SystemUIR;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::Ethernet::IEthernetManager;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Droid::Wifi::CWifiManager;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

//========================================================================================
//                      NetworkController::WifiHandler
//========================================================================================

NetworkController::WifiHandler::WifiHandler(
    /* [in] */ NetworkController* host)
    : mHost(host)
{}

ECode NetworkController::WifiHandler::Run()
{
    return NOERROR;
}

ECode NetworkController::WifiHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);

    if (NetworkController::DEBUG) {
        Slogger::D(TAG, " >> NetworkController::WifiHandler::HandleMessage: what=%d, arg1=%d", what, arg1);
    }

    switch (what) {
        case AsyncChannel::CMD_CHANNEL_HALF_CONNECTED:
            //Slogger::D(TAG, " >> NetworkController::WifiHandler::HandleMessage: CMD_CHANNEL_HALF_CONNECTED");
            if (arg1 == AsyncChannel::STATUS_SUCCESSFUL) {
                AutoPtr<IMessage> message;
                ObtainMessage(AsyncChannel::CMD_CHANNEL_FULL_CONNECTION, (IMessage**)&message);
                mHost->mWifiChannel->SendMessage(message);
            } else {
                Slogger::D("NetworkController::WifiHandler", "Failed to connect to wifi");
            }
            break;
        case IWifiManager::DATA_ACTIVITY_NOTIFICATION:
            if (arg1 != mHost->mWifiActivity) {
                mHost->mWifiActivity = arg1;
                mHost->RefreshViews();
            }
            break;
        default:
            //Ignore
            break;
    }
    return NOERROR;
}

//========================================================================================
//                      NetworkController
//========================================================================================
const String NetworkController::TAG("StatusBar.NetworkController");
const Boolean NetworkController::DEBUG = FALSE;
const Boolean NetworkController::CHATTY = FALSE;
const Int32 NetworkController::INET_CONDITION_THRESHOLD = 50;


NetworkController::NetworkController()
    : mHspaDataDistinguishable(FALSE)
    , mDataConnected(FALSE)
    // , mPhoneState(ITelephonyManager::CALL_STATE_IDLE)
    // , mDataNetType(ITelephonyManager::NETWORK_TYPE_UNKNOWN)
    // , mDataState(ITelephonyManager::DATA_DISCONNECTED)
    // , mDataActivity(ITelephonyManager::DATA_ACTIVITY_NONE)
    , mPhoneSignalIconId(0)
    , mQSPhoneSignalIconId(0)
    , mDataDirectionIconId(0)
    , mDataSignalIconId(0)
    , mDataTypeIconId(0)
    , mQSDataTypeIconId(0)
    , mAirplaneIconId(0)
    , mDataActive(FALSE)
    , mMobileActivityIconId(0)
    , mLastSignalLevel(0)
    , mShowPhoneRSSIForData(FALSE)
    , mShowAtLeastThreeGees(FALSE)
    , mAlwaysShowCdmaRssi(FALSE)
    , mWifiEnabled(FALSE)
    , mWifiConnected(FALSE)
    , mWifiRssi(0)
    , mWifiLevel(0)
    , mWifiIconId(0)
    , mQSWifiIconId(0)
    , mWifiActivityIconId(0)
    , mWifiActivity(IWifiManager::DATA_ACTIVITY_NONE)
    , mEthernetConnected(FALSE)
    , mEthernetIconId(SystemUIR::drawable::stat_sys_ethernet_error)
    , mEthernetActivityIconId(0)
    , mLastPhoneSignalIconId(-1)
    , mLastDataDirectionIconId(-1)
    , mLastDataDirectionOverlayIconId(-1)
    , mLastWifiIconId(-1)
    , mLastWimaxIconId(-1)
    , mLastEthernetIconId(-1)
    , mLastCombinedSignalIconId(-1)
    , mLastDataTypeIconId(-1)
    , mLastCombinedLabel("")
    , mDataAndWifiStacked(FALSE)
    , mBluetoothTethered(FALSE)
    , mBluetoothTetherIconId(R::drawable::stat_sys_tether_bluetooth)
    , mWimaxSupported(FALSE)
    , mIsWimaxEnabled(FALSE)
    , mWimaxConnected(FALSE)
    , mWimaxIdle(FALSE)
    , mWimaxIconId(0)
    , mWimaxSignal(0)
    , mWimaxState(0)
    , mWimaxExtraState(0)
    , mConnected(FALSE)
    , mConnectedNetworkType(IConnectivityManager::TYPE_NONE)
    , mInetCondition(0)
    , mAirplaneMode(FALSE)
    , mLastAirplaneMode(TRUE)
    , mHasMobileDataFeature(FALSE)
{
    mExited = FALSE;
    mPreWifiState = -1;
}

NetworkController::NetworkController(
    /* [in] */ IContext * context)
    : mHspaDataDistinguishable(FALSE)
    , mDataConnected(FALSE)
    // , mPhoneState(ITelephonyManager::CALL_STATE_IDLE)
    // , mDataNetType(ITelephonyManager::NETWORK_TYPE_UNKNOWN)
    // , mDataState(ITelephonyManager::DATA_DISCONNECTED)
    // , mDataActivity(ITelephonyManager::DATA_ACTIVITY_NONE)
    , mPhoneSignalIconId(0)
    , mQSPhoneSignalIconId(0)
    , mDataDirectionIconId(0)
    , mDataSignalIconId(0)
    , mDataTypeIconId(0)
    , mQSDataTypeIconId(0)
    , mAirplaneIconId(0)
    , mDataActive(FALSE)
    , mMobileActivityIconId(0)
    , mLastSignalLevel(0)
    , mShowPhoneRSSIForData(FALSE)
    , mShowAtLeastThreeGees(FALSE)
    , mAlwaysShowCdmaRssi(FALSE)
    , mWifiEnabled(FALSE)
    , mWifiConnected(FALSE)
    , mWifiRssi(0)
    , mWifiLevel(0)
    , mWifiIconId(0)
    , mQSWifiIconId(0)
    , mWifiActivityIconId(0)
    , mWifiActivity(IWifiManager::DATA_ACTIVITY_NONE)
    , mEthernetConnected(FALSE)
    , mEthernetIconId(SystemUIR::drawable::stat_sys_ethernet_error)
    , mEthernetActivityIconId(0)
    , mLastPhoneSignalIconId(-1)
    , mLastDataDirectionIconId(-1)
    , mLastDataDirectionOverlayIconId(-1)
    , mLastWifiIconId(-1)
    , mLastWimaxIconId(-1)
    , mLastEthernetIconId(-1)
    , mLastCombinedSignalIconId(-1)
    , mLastDataTypeIconId(-1)
    , mLastCombinedLabel("")
    , mDataAndWifiStacked(FALSE)
    , mBluetoothTethered(FALSE)
    , mBluetoothTetherIconId(R::drawable::stat_sys_tether_bluetooth)
    , mWimaxSupported(FALSE)
    , mIsWimaxEnabled(FALSE)
    , mWimaxConnected(FALSE)
    , mWimaxIdle(FALSE)
    , mWimaxIconId(0)
    , mWimaxSignal(0)
    , mWimaxState(0)
    , mWimaxExtraState(0)
    , mConnected(FALSE)
    , mConnectedNetworkType(IConnectivityManager::TYPE_NONE)
    , mInetCondition(0)
    , mAirplaneMode(FALSE)
    , mLastAirplaneMode(TRUE)
    , mHasMobileDataFeature(FALSE)
{
    mContext = context;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    AutoPtr<IInterface> tmpObj;
    mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&tmpObj);
    AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(tmpObj.Get());
    assert(cm != NULL);

    cm->IsNetworkSupported(IConnectivityManager::TYPE_MOBILE, &mHasMobileDataFeature);

    res->GetBoolean(SystemUIR::bool_::config_showPhoneRSSIForData, &mShowPhoneRSSIForData);
    res->GetBoolean(SystemUIR::bool_::config_showMin3G, &mShowAtLeastThreeGees);
    res->GetBoolean(R::bool_::config_alwaysUseCdmaRssi, &mAlwaysShowCdmaRssi);

    // set up the default wifi icon, used when no radios have ever appeared
    UpdateWifiIcons();
    UpdateWimaxIcons();

    // telephony
    // context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&mPhone);
    // mPhone->Listen(mPhoneStateListener,
    //                   IPhoneStateListener::LISTEN_SERVICE_STATE
    //                 | IPhoneStateListener::LISTEN_SIGNAL_STRENGTHS
    //                 | IPhoneStateListener::LISTEN_CALL_STATE
    //                 | IPhoneStateListener::LISTEN_DATA_CONNECTION_STATE
    //                 | IPhoneStateListener::LISTEN_DATA_ACTIVITY);

    res->GetBoolean(SystemUIR::bool_::config_hspa_data_distinguishable, &mHspaDataDistinguishable);
    mContext->GetString(SystemUIR::string::status_bar_network_name_separator, &mNetworkNameSeparator);
    mContext->GetString(R::string::lockscreen_carrier_default, &mNetworkNameDefault);
    mNetworkName = mNetworkNameDefault;

    // wifi
    tmpObj = NULL;
    context->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&tmpObj);
    mWifiManager = IWifiManager::Probe(tmpObj.Get());
    assert(mWifiManager != NULL);

    mWifiHandler = new WifiHandler(this);
    mWifiChannel = new AsyncChannel();
    AutoPtr<IMessenger> wifiMessenger;
    mWifiManager->GetWifiServiceMessenger((IMessenger**)&wifiMessenger);
    if (wifiMessenger != NULL) {
        mWifiChannel->Connect(mContext, mWifiHandler, wifiMessenger);
    }

    // broadcasts
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IWifiManager::RSSI_CHANGED_ACTION);
    filter->AddAction(IWifiManager::WIFI_STATE_CHANGED_ACTION);
    filter->AddAction(IWifiManager::NETWORK_STATE_CHANGED_ACTION);
    // filter->AddAction(ITelephonyIntents::ACTION_SIM_STATE_CHANGED);
    // filter->AddAction(ITelephonyIntents::SPN_STRINGS_UPDATED_ACTION);
    filter->AddAction(IConnectivityManager::CONNECTIVITY_ACTION);
    filter->AddAction(IConnectivityManager::INET_CONDITION_ACTION);
    filter->AddAction(IIntent::ACTION_CONFIGURATION_CHANGED);
    filter->AddAction(IIntent::ACTION_AIRPLANE_MODE_CHANGED);
    filter->AddAction(IEthernetManager::ETHERNET_STATE_CHANGED_ACTION);
    filter->AddAction(IEthernetManager::NETWORK_STATE_CHANGED_ACTION);

    res->GetBoolean(R::bool_::config_wimaxEnabled, &mWimaxSupported);
    if (mWimaxSupported) {
        // filter->AddAction(IWimaxManagerConstants::WIMAX_NETWORK_STATE_CHANGED_ACTION);
        // filter->AddAction(IWimaxManagerConstants::SIGNAL_LEVEL_CHANGED_ACTION);
        // filter->AddAction(IWimaxManagerConstants::NET_4G_STATE_CHANGED_ACTION);
    }
    AutoPtr<IIntent> in;
    context->RegisterReceiver(this, filter, (IIntent**)&in);

    // AIRPLANE_MODE_CHANGED is sent at boot; we've probably already missed it
    UpdateAirplaneMode();

    // yuck
    // mBatteryStats = BatteryStatsService.getService();
}

NetworkController::~NetworkController()
{
    mExited = TRUE;
}

PInterface NetworkController::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)(INetworkController*)this;
    }
    else if (riid == EIID_IObject ) {
        return (IObject*)this;
    }
    else if (riid == EIID_IBroadcastReceiver) {
        return (IBroadcastReceiver*)this;
    }
    else if (riid == EIID_INetworkController) {
        return (INetworkController*)this;
    }
    return NULL;
}

ECode NetworkController::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);
    if (object == (IInterface*)(INetworkController *)this) {
        *iid = EIID_INetworkController;
    }
    else if (object == (IInterface*)(IBroadcastReceiver *)this) {
        *iid = EIID_IBroadcastReceiver;
    }
    else if (object == (IInterface*)(IObject *)this) {
        *iid = EIID_IObject;
    }
    else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

UInt32 NetworkController::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 NetworkController::Release()
{
    return ElRefBase::Release();
}

ECode NetworkController::HasMobileDataFeature(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHasMobileDataFeature;
    return NOERROR;
}

ECode NetworkController::IsEmergencyOnly(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // *result = (mServiceState != NULL && mServiceState->IsEmergencyOnly());
    *result = FALSE;
    return E_NOT_IMPLEMENTED;
}

ECode NetworkController::AddPhoneSignalIconView(
    /* [in] */ IImageView* v)
{
    AutoPtr<IImageView> tmp = v;
    mPhoneSignalIconViews.PushBack(tmp);
    return NOERROR;
}

ECode NetworkController::AddDataDirectionIconView(
    /* [in] */ IImageView* v)
{
    AutoPtr<IImageView> tmp = v;
    mDataDirectionIconViews.PushBack(tmp);
    return NOERROR;
}

ECode NetworkController::AddDataDirectionOverlayIconView(
    /* [in] */ IImageView* v)
{
    AutoPtr<IImageView> tmp = v;
    mDataDirectionOverlayIconViews.PushBack(tmp);
    return NOERROR;
}

ECode NetworkController::AddWifiIconView(
    /* [in] */ IImageView* v)
{
    AutoPtr<IImageView> tmp = v;
    mWifiIconViews.PushBack(tmp);
    return NOERROR;
}

ECode NetworkController::AddWimaxIconView(
    /* [in] */ IImageView* v)
{
    AutoPtr<IImageView> tmp = v;
    mWimaxIconViews.PushBack(tmp);
    return NOERROR;
}

ECode NetworkController::AddCombinedSignalIconView(
    /* [in] */ IImageView* v)
{
    AutoPtr<IImageView> tmp = v;
    mCombinedSignalIconViews.PushBack(tmp);
    return NOERROR;
}

ECode NetworkController::AddDataTypeIconView(
    /* [in] */ IImageView* v)
{
    AutoPtr<IImageView> tmp = v;
    mDataTypeIconViews.PushBack(tmp);
    return NOERROR;
}

ECode NetworkController::AddCombinedLabelView(
    /* [in] */ ITextView* v)
{
    AutoPtr<ITextView> tmp = v;
    mCombinedLabelViews.PushBack(tmp);
    return NOERROR;
}

ECode NetworkController::AddMobileLabelView(
    /* [in] */ ITextView* v)
{
    AutoPtr<ITextView> tmp = v;
    mMobileLabelViews.PushBack(tmp);
    return NOERROR;
}

ECode NetworkController::AddWifiLabelView(
    /* [in] */ ITextView* v)
{
    AutoPtr<ITextView> tmp = v;
    mWifiLabelViews.PushBack(tmp);
    return NOERROR;
}

ECode NetworkController::AddEmergencyLabelView(
    /* [in] */ ITextView* v)
{
    AutoPtr<ITextView> tmp = v;
    mEmergencyLabelViews.PushBack(tmp);
    return NOERROR;
}

ECode NetworkController::AddSignalCluster(
    /* [in] */ INetworkControllerSignalCluster* cluster)
{
    AutoPtr<INetworkControllerSignalCluster> tmp = cluster;
    mSignalClusters.PushBack(tmp);
    RefreshSignalCluster(tmp);
    return NOERROR;
}

ECode NetworkController::AddNetworkSignalChangedCallback(
    /* [in] */ INetworkSignalChangedCallback* cb)
{
    AutoPtr<INetworkSignalChangedCallback> tmp = cb;
    mSignalsChangedCallbacks.PushBack(tmp);
    NotifySignalsChangedCallbacks(tmp);
    return NOERROR;
}

ECode NetworkController::RefreshSignalCluster(
    /* [in] */ INetworkControllerSignalCluster* cluster)
{
    assert(cluster);

    cluster->SetWifiIndicators(
        // only show wifi in the cluster if connected or if wifi-only
        mWifiEnabled && (mWifiConnected || !mHasMobileDataFeature),
        mWifiIconId,
        mWifiActivityIconId,
        mContentDescriptionWifi);

    cluster->SetEthernetIndicators(
        mEthernetConnected,
        mEthernetIconId,
        mEthernetActivityIconId,
        mContentDescriptionEthernet);

    if (mIsWimaxEnabled && mWimaxConnected) {
        // wimax is special
        cluster->SetMobileDataIndicators(
                TRUE,
                mAlwaysShowCdmaRssi ? mPhoneSignalIconId : mWimaxIconId,
                mMobileActivityIconId,
                mDataTypeIconId,
                mContentDescriptionWimax,
                mContentDescriptionDataType);
    } else {
        // normal mobile data
        cluster->SetMobileDataIndicators(
                mHasMobileDataFeature,
                mShowPhoneRSSIForData ? mPhoneSignalIconId : mDataSignalIconId,
                mMobileActivityIconId,
                mDataTypeIconId,
                mContentDescriptionPhoneSignal,
                mContentDescriptionDataType);
    }

    cluster->SetIsAirplaneMode(mAirplaneMode, mAirplaneIconId);
    return NOERROR;
}

ECode NetworkController::NotifySignalsChangedCallbacks(
    /* [in] */ INetworkSignalChangedCallback* cb)
{
    String nullStr;
    // only show wifi in the cluster if connected or if wifi-only
    Boolean wifiEnabled = mWifiEnabled && (mWifiConnected || !mHasMobileDataFeature);
    String wifiDesc = wifiEnabled ? mWifiSsid : nullStr;
    cb->OnWifiSignalChanged(wifiEnabled, mQSWifiIconId, mContentDescriptionWifi, wifiDesc);

    Boolean b;
    IsEmergencyOnly(&b);
    if (b) {
        cb->OnMobileDataSignalChanged(FALSE, mQSPhoneSignalIconId,
                mContentDescriptionPhoneSignal, mQSDataTypeIconId, mContentDescriptionDataType,
                nullStr);
    } else {
        if (mIsWimaxEnabled && mWimaxConnected) {
            // Wimax is special
            cb->OnMobileDataSignalChanged(TRUE, mQSPhoneSignalIconId,
                    mContentDescriptionPhoneSignal, mQSDataTypeIconId,
                    mContentDescriptionDataType, mNetworkName);
        } else {
            // Normal mobile data
            cb->OnMobileDataSignalChanged(mHasMobileDataFeature, mQSPhoneSignalIconId,
                    mContentDescriptionPhoneSignal, mQSDataTypeIconId,
                    mContentDescriptionDataType, mNetworkName);
        }
    }
    cb->OnAirplaneModeChanged(mAirplaneMode);
    return NOERROR;
}

ECode NetworkController::SetStackedMode(
    /* [in] */ Boolean stacked)
{
    mDataAndWifiStacked = TRUE;
    return NOERROR;
}

ECode NetworkController::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);

    String action;
    intent->GetAction(&action);

    if (DEBUG) {
        Slogger::D(TAG, "NetworkController::OnReceive %s", action.string());
    }

    if (action.Equals(IWifiManager::RSSI_CHANGED_ACTION)
            || action.Equals(IWifiManager::WIFI_STATE_CHANGED_ACTION)
            || action.Equals(IWifiManager::NETWORK_STATE_CHANGED_ACTION)) {
        UpdateWifiState(intent);
        RefreshViews();
    }
    // else if (action.Equals(TelephonyIntents.ACTION_SIM_STATE_CHANGED)) {
    //     updateSimState(intent);
    //     updateDataIcon();
    //     refreshViews();
    // } else if (action.Equals(TelephonyIntents.SPN_STRINGS_UPDATED_ACTION)) {
    //     updateNetworkName(intent.getBooleanExtra(TelephonyIntents.EXTRA_SHOW_SPN, false),
    //                 intent.getStringExtra(TelephonyIntents.EXTRA_SPN),
    //                 intent.getBooleanExtra(TelephonyIntents.EXTRA_SHOW_PLMN, false),
    //                 intent.getStringExtra(TelephonyIntents.EXTRA_PLMN));
    //     refreshViews();
    // }
    else if (action.Equals(IConnectivityManager::CONNECTIVITY_ACTION) ||
             action.Equals(IConnectivityManager::INET_CONDITION_ACTION)) {
        UpdateConnectivity(intent);
        RefreshViews();
    }
    else if (action.Equals(IIntent::ACTION_CONFIGURATION_CHANGED)) {
        RefreshViews();
    }
    else if (action.Equals(IIntent::ACTION_AIRPLANE_MODE_CHANGED)) {
        UpdateAirplaneMode();
        RefreshViews();
    }
    // else if (action.Equals(WimaxManagerConstants.NET_4G_STATE_CHANGED_ACTION) ||
    //         action.Equals(WimaxManagerConstants.SIGNAL_LEVEL_CHANGED_ACTION) ||
    //         action.Equals(WimaxManagerConstants.WIMAX_NETWORK_STATE_CHANGED_ACTION)) {
    //     updateWimaxState(intent);
    //     RefreshViews();
    // }
    else if (action.Equals(IEthernetManager::NETWORK_STATE_CHANGED_ACTION) ||
            action.Equals(IEthernetManager::ETHERNET_STATE_CHANGED_ACTION)) {
        UpdateEthernetState(intent);
        RefreshViews();
    }
    return NOERROR;
}

// ===== Telephony ==============================================================

void NetworkController::UpdateSimState(
    /* [in] */ IIntent* intent)
{
    // String stateExtra = intent.getStringExtra(IccCardConstants.INTENT_KEY_ICC_STATE);
    // if (IccCardConstants.INTENT_VALUE_ICC_ABSENT.equals(stateExtra)) {
    //     mSimState = IccCardConstants.State.ABSENT;
    // }
    // else if (IccCardConstants.INTENT_VALUE_ICC_READY.equals(stateExtra)) {
    //     mSimState = IccCardConstants.State.READY;
    // }
    // else if (IccCardConstants.INTENT_VALUE_ICC_LOCKED.equals(stateExtra)) {
    //     final String lockedReason =
    //             intent.getStringExtra(IccCardConstants.INTENT_KEY_LOCKED_REASON);
    //     if (IccCardConstants.INTENT_VALUE_LOCKED_ON_PIN.equals(lockedReason)) {
    //         mSimState = IccCardConstants.State.PIN_REQUIRED;
    //     }
    //     else if (IccCardConstants.INTENT_VALUE_LOCKED_ON_PUK.equals(lockedReason)) {
    //         mSimState = IccCardConstants.State.PUK_REQUIRED;
    //     }
    //     else {
    //         mSimState = IccCardConstants.State.NETWORK_LOCKED;
    //     }
    // } else {
    //     mSimState = IccCardConstants.State.UNKNOWN;
    // }
}

Boolean NetworkController::IsCdma()
{
    // return (mSignalStrength != NULL) && !mSignalStrength.isGsm();
    return FALSE;
}

Boolean NetworkController::HasService()
{
    // if (mServiceState != null) {
    //     switch (mServiceState.getState()) {
    //         case ServiceState.STATE_OUT_OF_SERVICE:
    //         case ServiceState.STATE_POWER_OFF:
    //             return FALSE;
    //         default:
    //             return TRUE;
    //     }
    // } else {
    //     return FALSE;
    // }
    return FALSE;
}

void NetworkController::UpdateAirplaneMode()
{
    // Int32 value = 0;
    // AutoPtr<IContentResolver> cr;
    // mContext->GetContentResolver((IContentResolver**)&cr);
    // Settings::Global::GetInt32(cr, ISettingsGlobal::AIRPLANE_MODE_ON, 0, &value);
    // mAirplaneMode = (value == 1);
}

void NetworkController::UpdateTelephonySignalStrength()
{
    // if (!HasService()) {
    //     if (CHATTY) Slogger::D(TAG, "updateTelephonySignalStrength: !hasService()");
    //     mPhoneSignalIconId = R::drawable::stat_sys_signal_null;
    //     mQSPhoneSignalIconId = R::drawable::ic_qs_signal_no_signal;
    //     mDataSignalIconId = R::drawable::stat_sys_signal_null;
    // } else {
    //     if (mSignalStrength == NULL) {
    //         if (CHATTY) Slogger::D(TAG, "updateTelephonySignalStrength: mSignalStrength == NULL");
    //         mPhoneSignalIconId = R::drawable::stat_sys_signal_null;
    //         mQSPhoneSignalIconId = R::drawable::ic_qs_signal_no_signal;
    //         mDataSignalIconId = R::drawable::stat_sys_signal_null;
    //         mContext.getString(
    //                 AccessibilityContentDescriptions::PHONE_SIGNAL_STRENGTH[0], &mContentDescriptionPhoneSignal);
    //     } else {
    //         Int32 iconLevel;
    //         int[] iconList;
    //         if (isCdma() && mAlwaysShowCdmaRssi) {
    //             mLastSignalLevel = iconLevel = mSignalStrength.getCdmaLevel();
    //             // if(DEBUG) Slogger::D(TAG, "mAlwaysShowCdmaRssi=" + mAlwaysShowCdmaRssi
    //             //         + " set to cdmaLevel=" + mSignalStrength.getCdmaLevel()
    //             //         + " instead of level=" + mSignalStrength.getLevel());
    //         } else {
    //             mLastSignalLevel = iconLevel = mSignalStrength.getLevel();
    //         }

    //         if (isCdma()) {
    //             if (isCdmaEri()) {
    //                 iconList = TelephonyIcons.TELEPHONY_SIGNAL_STRENGTH_ROAMING[mInetCondition];
    //             } else {
    //                 iconList = TelephonyIcons.TELEPHONY_SIGNAL_STRENGTH[mInetCondition];
    //             }
    //         } else {
    //             // Though mPhone is a Manager, this call is not an IPC
    //             if (mPhone.isNetworkRoaming()) {
    //                 iconList = TelephonyIcons.TELEPHONY_SIGNAL_STRENGTH_ROAMING[mInetCondition];
    //             } else {
    //                 iconList = TelephonyIcons.TELEPHONY_SIGNAL_STRENGTH[mInetCondition];
    //             }
    //         }
    //         mPhoneSignalIconId = iconList[iconLevel];
    //         mQSPhoneSignalIconId =
    //                 TelephonyIcons.QS_TELEPHONY_SIGNAL_STRENGTH[mInetCondition][iconLevel];
    //         mContentDescriptionPhoneSignal = mContext.getString(
    //                 AccessibilityContentDescriptions::PHONE_SIGNAL_STRENGTH[iconLevel]);
    //         mDataSignalIconId = TelephonyIcons.DATA_SIGNAL_STRENGTH[mInetCondition][iconLevel];
    //     }
    // }
}

void NetworkController::UpdateDataNetType()
{
    // if (mIsWimaxEnabled && mWimaxConnected) {
    //     // wimax is a special 4g network not handled by telephony
    //     mDataIconList = TelephonyIcons.DATA_4G[mInetCondition];
    //     mDataTypeIconId = R.drawable.stat_sys_data_connected_4g;
    //     mQSDataTypeIconId = R.drawable.ic_qs_signal_4g;
    //     mContentDescriptionDataType = mContext.getString(
    //             R.string.accessibility_data_connection_4g);
    // } else {
    //     switch (mDataNetType) {
    //         case TelephonyManager.NETWORK_TYPE_UNKNOWN:
    //             if (!mShowAtLeastThreeGees) {
    //                 mDataIconList = TelephonyIcons.DATA_G[mInetCondition];
    //                 mDataTypeIconId = 0;
    //                 mQSDataTypeIconId = 0;
    //                 mContentDescriptionDataType = mContext.getString(
    //                         R.string.accessibility_data_connection_gprs);
    //                 break;
    //             } else {
    //                 // fall through
    //             }
    //         case TelephonyManager.NETWORK_TYPE_EDGE:
    //             if (!mShowAtLeastThreeGees) {
    //                 mDataIconList = TelephonyIcons.DATA_E[mInetCondition];
    //                 mDataTypeIconId = R.drawable.stat_sys_data_connected_e;
    //                 mQSDataTypeIconId = R.drawable.ic_qs_signal_e;
    //                 mContentDescriptionDataType = mContext.getString(
    //                         R.string.accessibility_data_connection_edge);
    //                 break;
    //             } else {
    //                 // fall through
    //             }
    //         case TelephonyManager.NETWORK_TYPE_UMTS:
    //             mDataIconList = TelephonyIcons.DATA_3G[mInetCondition];
    //             mDataTypeIconId = R.drawable.stat_sys_data_connected_3g;
    //             mQSDataTypeIconId = R.drawable.ic_qs_signal_3g;
    //             mContentDescriptionDataType = mContext.getString(
    //                     R.string.accessibility_data_connection_3g);
    //             break;
    //         case TelephonyManager.NETWORK_TYPE_HSDPA:
    //         case TelephonyManager.NETWORK_TYPE_HSUPA:
    //         case TelephonyManager.NETWORK_TYPE_HSPA:
    //         case TelephonyManager.NETWORK_TYPE_HSPAP:
    //             if (mHspaDataDistinguishable) {
    //                 mDataIconList = TelephonyIcons.DATA_H[mInetCondition];
    //                 mDataTypeIconId = R.drawable.stat_sys_data_connected_h;
    //                 mQSDataTypeIconId = R.drawable.ic_qs_signal_h;
    //                 mContentDescriptionDataType = mContext.getString(
    //                         R.string.accessibility_data_connection_3_5g);
    //             } else {
    //                 mDataIconList = TelephonyIcons.DATA_3G[mInetCondition];
    //                 mDataTypeIconId = R.drawable.stat_sys_data_connected_3g;
    //                 mQSDataTypeIconId = R.drawable.ic_qs_signal_3g;
    //                 mContentDescriptionDataType = mContext.getString(
    //                         R.string.accessibility_data_connection_3g);
    //             }
    //             break;
    //         case TelephonyManager.NETWORK_TYPE_CDMA:
    //             if (!mShowAtLeastThreeGees) {
    //                 // display 1xRTT for IS95A/B
    //                 mDataIconList = TelephonyIcons.DATA_1X[mInetCondition];
    //                 mDataTypeIconId = R.drawable.stat_sys_data_connected_1x;
    //                 mQSDataTypeIconId = R.drawable.ic_qs_signal_1x;
    //                 mContentDescriptionDataType = mContext.getString(
    //                         R.string.accessibility_data_connection_cdma);
    //                 break;
    //             } else {
    //                 // fall through
    //             }
    //         case TelephonyManager.NETWORK_TYPE_1xRTT:
    //             if (!mShowAtLeastThreeGees) {
    //                 mDataIconList = TelephonyIcons.DATA_1X[mInetCondition];
    //                 mDataTypeIconId = R.drawable.stat_sys_data_connected_1x;
    //                 mQSDataTypeIconId = R.drawable.ic_qs_signal_1x;
    //                 mContentDescriptionDataType = mContext.getString(
    //                         R.string.accessibility_data_connection_cdma);
    //                 break;
    //             } else {
    //                 // fall through
    //             }
    //         case TelephonyManager.NETWORK_TYPE_EVDO_0: //fall through
    //         case TelephonyManager.NETWORK_TYPE_EVDO_A:
    //         case TelephonyManager.NETWORK_TYPE_EVDO_B:
    //         case TelephonyManager.NETWORK_TYPE_EHRPD:
    //             mDataIconList = TelephonyIcons.DATA_3G[mInetCondition];
    //             mDataTypeIconId = R.drawable.stat_sys_data_connected_3g;
    //             mQSDataTypeIconId = R.drawable.ic_qs_signal_3g;
    //             mContentDescriptionDataType = mContext.getString(
    //                     R.string.accessibility_data_connection_3g);
    //             break;
    //         case TelephonyManager.NETWORK_TYPE_LTE:
    //             mDataIconList = TelephonyIcons.DATA_4G[mInetCondition];
    //             mDataTypeIconId = R.drawable.stat_sys_data_connected_4g;
    //             mQSDataTypeIconId = R.drawable.ic_qs_signal_4g;
    //             mContentDescriptionDataType = mContext.getString(
    //                     R.string.accessibility_data_connection_4g);
    //             break;
    //         default:
    //             if (!mShowAtLeastThreeGees) {
    //                 mDataIconList = TelephonyIcons.DATA_G[mInetCondition];
    //                 mDataTypeIconId = R.drawable.stat_sys_data_connected_g;
    //                 mQSDataTypeIconId = R.drawable.ic_qs_signal_g;
    //                 mContentDescriptionDataType = mContext.getString(
    //                         R.string.accessibility_data_connection_gprs);
    //             } else {
    //                 mDataIconList = TelephonyIcons.DATA_3G[mInetCondition];
    //                 mDataTypeIconId = R.drawable.stat_sys_data_connected_3g;
    //                 mQSDataTypeIconId = R.drawable.ic_qs_signal_3g;
    //                 mContentDescriptionDataType = mContext.getString(
    //                         R.string.accessibility_data_connection_3g);
    //             }
    //             break;
    //     }
    // }

    // if (isCdma()) {
    //     if (isCdmaEri()) {
    //         mDataTypeIconId = R.drawable.stat_sys_data_connected_roam;
    //         mQSDataTypeIconId = R.drawable.ic_qs_signal_r;
    //     }
    // } else if (mPhone.isNetworkRoaming()) {
    //         mDataTypeIconId = R.drawable.stat_sys_data_connected_roam;
    //         mQSDataTypeIconId = R.drawable.ic_qs_signal_r;
    // }
}

ECode NetworkController::IsCdmaEri(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    //TODO
    // if (mServiceState != NULL) {
    //     Int32 iconIndex = mServiceState.getCdmaEriIconIndex();
    //     if (iconIndex != EriInfo.ROAMING_INDICATOR_OFF) {
    //         Int32 iconMode = mServiceState.getCdmaEriIconMode();
    //         if (iconMode == EriInfo.ROAMING_ICON_MODE_NORMAL
    //                 || iconMode == EriInfo.ROAMING_ICON_MODE_FLASH) {
    //             *result = TRUE;
    //             return NOERROR;
    //         }
    //     }
    // }
    // *result = FALSE;
    return NOERROR;
}

void NetworkController::UpdateDataIcon()
{
    // Int32 iconId;
    // Boolean visible = TRUE;

    // if (!IsCdma()) {
    //     // GSM case, we have to check also the sim state
    //     if (mSimState == IccCardConstants.State.READY ||
    //             mSimState == IccCardConstants.State.UNKNOWN) {
    //         if (HasService() && mDataState == TelephonyManager.DATA_CONNECTED) {
    //             switch (mDataActivity) {
    //                 case TelephonyManager.DATA_ACTIVITY_IN:
    //                     iconId = mDataIconList[1];
    //                     break;
    //                 case TelephonyManager.DATA_ACTIVITY_OUT:
    //                     iconId = mDataIconList[2];
    //                     break;
    //                 case TelephonyManager.DATA_ACTIVITY_INOUT:
    //                     iconId = mDataIconList[3];
    //                     break;
    //                 default:
    //                     iconId = mDataIconList[0];
    //                     break;
    //             }
    //             mDataDirectionIconId = iconId;
    //         } else {
    //             iconId = 0;
    //             visible = FALSE;
    //         }
    //     } else {
    //         iconId = R.drawable.stat_sys_no_sim;
    //         visible = FALSE; // no SIM? no data
    //     }
    // } else {
    //     // CDMA case, mDataActivity can be also DATA_ACTIVITY_DORMANT
    //     if (HasService() && mDataState == TelephonyManager.DATA_CONNECTED) {
    //         switch (mDataActivity) {
    //             case TelephonyManager.DATA_ACTIVITY_IN:
    //                 iconId = mDataIconList[1];
    //                 break;
    //             case TelephonyManager.DATA_ACTIVITY_OUT:
    //                 iconId = mDataIconList[2];
    //                 break;
    //             case TelephonyManager.DATA_ACTIVITY_INOUT:
    //                 iconId = mDataIconList[3];
    //                 break;
    //             case TelephonyManager.DATA_ACTIVITY_DORMANT:
    //             default:
    //                 iconId = mDataIconList[0];
    //                 break;
    //         }
    //     } else {
    //         iconId = 0;
    //         visible = FALSE;
    //     }
    // }

    // // yuck - this should NOT be done by the status bar
    // Int64 ident = Binder::ClearCallingIdentity();
    // // try {
    //     mBatteryStats->NotePhoneDataConnectionState(mPhone.getNetworkType(), visible);
    // // } catch (RemoteException e) {
    // // } finally {
    //     Binder::RestoreCallingIdentity(ident);
    // // }

    // mDataDirectionIconId = iconId;
    // mDataConnected = visible;
}

ECode NetworkController::UpdateNetworkName(
    /* [in] */ Boolean showSpn,
    /* [in] */ const String& spn,
    /* [in] */ Boolean showPlmn,
    /* [in] */ const String& plmn)
{
    if (DEBUG) {
        Slogger::D(String("CarrierLabel"), "updateNetworkName showSpn=%d, spn=%s, showPlmn=%d, plmn=%s",
            showSpn, spn.string(), showPlmn, plmn.string());
    }

    StringBuilder str;
    Boolean something = FALSE;
    if (showPlmn && !plmn.IsNull()) {
        str.AppendString(plmn);
        something = TRUE;
    }
    if (showSpn && !spn.IsNull()) {
        if (something) {
            str.AppendString(mNetworkNameSeparator);
        }
        str.AppendString(spn);
        something = TRUE;
    }
    if (something) {
        mNetworkName = str.ToString();
    } else {
        mNetworkName = mNetworkNameDefault;
    }
    return NOERROR;
}

// ===== Wifi ===================================================================

void NetworkController::UpdateWifiState(
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (action.Equals(IWifiManager::WIFI_STATE_CHANGED_ACTION)) {
        Int32 value;
        intent->GetInt32Extra(IWifiManager::EXTRA_WIFI_STATE, IWifiManager::WIFI_STATE_UNKNOWN, &value);
        mWifiEnabled = value == IWifiManager::WIFI_STATE_ENABLED;
    }
    else if (action.Equals(IWifiManager::NETWORK_STATE_CHANGED_ACTION)) {
        AutoPtr<IParcelable> parcelable;
        intent->GetParcelableExtra(IWifiManager::EXTRA_NETWORK_INFO, (IParcelable**)&parcelable);
        AutoPtr<INetworkInfo> networkInfo = INetworkInfo::Probe(parcelable.Get());
        Boolean wasConnected = mWifiConnected;
        mWifiConnected = FALSE;
        if (networkInfo != NULL) {
            networkInfo->IsConnected(&mWifiConnected);
        }
        if (DEBUG)
            Slogger::D(TAG, "UpdateWifiState NETWORK_STATE_CHANGED_ACTION connected: %d", mWifiConnected);
        // If we just connected, grab the inintial signal strength and ssid
        if (mWifiConnected && !wasConnected) {
            // try getting it out of the intent first
            AutoPtr<IParcelable> wifiParcelable;
            intent->GetParcelableExtra(IWifiManager::EXTRA_WIFI_INFO, (IParcelable**)&wifiParcelable);
            AutoPtr<IWifiInfo> info = IWifiInfo::Probe(wifiParcelable.Get());
            if (info == NULL) {
                mWifiManager->GetConnectionInfo((IWifiInfo**)&info);
            }
            if (info != NULL) {
                mWifiSsid = HuntForSsid(info);
            } else {
                mWifiSsid = NULL;
            }
        } else if (!mWifiConnected) {
            mWifiSsid = NULL;
        }
    }
    else if (action.Equals(IWifiManager::RSSI_CHANGED_ACTION)) {
        intent->GetInt32Extra(IWifiManager::EXTRA_NEW_RSSI, -200, &mWifiRssi);
        mWifiLevel = CWifiManager::CalculateSignalLevel(mWifiRssi, WifiIcons::WIFI_LEVEL_COUNT);
        if (DEBUG)
            Slogger::D(TAG, "UpdateWifiState RSSI_CHANGED_ACTION mWifiLevel: %d",  mWifiLevel);
    }

    UpdateWifiIcons();
}

void NetworkController::UpdateWifiIcons()
{
    if (mWifiConnected) {
        mWifiIconId = WifiIcons::WIFI_SIGNAL_STRENGTH[mInetCondition][mWifiLevel];
        mQSWifiIconId = WifiIcons::QS_WIFI_SIGNAL_STRENGTH[mInetCondition][mWifiLevel];
        // TODO
        mContentDescriptionWifi = String("WIFI");
        // mContext->GetString(
        //     AccessibilityContentDescriptions::WIFI_CONNECTION_STRENGTH[mWifiLevel],
        //     &mContentDescriptionWifi);
    }
    else {
        if (mDataAndWifiStacked) {
            mWifiIconId = 0;
            mQSWifiIconId = 0;
        }
        else {
            mWifiIconId = mWifiEnabled ? SystemUIR::drawable::stat_sys_wifi_signal_null : 0;
            mQSWifiIconId = mWifiEnabled ? SystemUIR::drawable::ic_qs_wifi_no_network : 0;
        }
        // TODO
        mContentDescriptionWifi = String("WIFI");
        // mContext->GetString(SystemUIR::string::accessibility_no_wifi,
        //     &mContentDescriptionWifi);
    }
}

String NetworkController::HuntForSsid(
    /* [in] */ IWifiInfo* info)
{
    assert(info != NULL);
    String ssid;
    info->GetSSID(&ssid);
    if (ssid.IsNull()) {
        return ssid;
    }

    // OK, it's not in the connectionInfo; we have to go hunting for it
    AutoPtr<IObjectContainer> networks;
    mWifiManager->GetConfiguredNetworks((IObjectContainer**)&networks);

    if (networks != NULL) {
        Int32 id, networkId;
        info->GetNetworkId(&id);

        AutoPtr<IWifiConfiguration> config;
        AutoPtr<IObjectEnumerator> enumerator;
        networks->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
        Boolean hasNext = FALSE;
        while(enumerator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IInterface> tmpObj;
            enumerator->Current((IInterface**)&tmpObj);
            config = IWifiConfiguration::Probe(tmpObj.Get());
            if (config) {
                config->GetNetworkId(&networkId);
                if(networkId == id) {
                    config->GetSSID(&ssid);
                    return ssid;
                }
            }
        }
    }

    return String(NULL);
}

// ===== Ethernet State =========================================================

void NetworkController::UpdateEthernetState(
    /* [in] */ IIntent* intent)
{
    assert(intent != NULL);

    String action;
    intent->GetAction(&action);

    if (action.Equals(IEthernetManager::NETWORK_STATE_CHANGED_ACTION)) {
        AutoPtr<IParcelable> parcelable;
        intent->GetParcelableExtra(IWifiManager::EXTRA_NETWORK_INFO, (IParcelable**)&parcelable);
        AutoPtr<INetworkInfo> networkInfo = INetworkInfo::Probe(parcelable.Get());

        mEthernetConnected = FALSE;
        if (networkInfo) {
            networkInfo->IsConnected(&mEthernetConnected);
        }
    }

    UpdateEthernetIcons();
}

void NetworkController::UpdateEthernetIcons()
{
    if (mEthernetConnected) {
        mEthernetIconId = SystemUIR::drawable::stat_sys_ethernet_established;
        mContext->GetString(SystemUIR::string::ethernet_description, &mContentDescriptionEthernet);
    }
    else {
        mEthernetIconId = SystemUIR::drawable::stat_sys_ethernet_error;
        //mContentDescriptionEthernet = "DISCONNECTED";
    }
}

// ===== Wimax ===================================================================

void NetworkController::UpdateWimaxState(
    /* [in] */ IIntent* intent)
{
    // String action;
    // intent->GetAction(&action);
    // Boolean wasConnected = mWimaxConnected;
    // if (action.Equals(WimaxManagerConstants.NET_4G_STATE_CHANGED_ACTION)) {
    //     int wimaxStatus = intent.getIntExtra(WimaxManagerConstants.EXTRA_4G_STATE,
    //             WimaxManagerConstants.NET_4G_STATE_UNKNOWN);
    //     mIsWimaxEnabled = (wimaxStatus ==
    //             WimaxManagerConstants.NET_4G_STATE_ENABLED);
    // } else if (action.equals(WimaxManagerConstants.SIGNAL_LEVEL_CHANGED_ACTION)) {
    //     mWimaxSignal = intent.getIntExtra(WimaxManagerConstants.EXTRA_NEW_SIGNAL_LEVEL, 0);
    // } else if (action.equals(WimaxManagerConstants.WIMAX_NETWORK_STATE_CHANGED_ACTION)) {
    //     mWimaxState = intent.getIntExtra(WimaxManagerConstants.EXTRA_WIMAX_STATE,
    //             WimaxManagerConstants.NET_4G_STATE_UNKNOWN);
    //     mWimaxExtraState = intent.getIntExtra(
    //             WimaxManagerConstants.EXTRA_WIMAX_STATE_DETAIL,
    //             WimaxManagerConstants.NET_4G_STATE_UNKNOWN);
    //     mWimaxConnected = (mWimaxState ==
    //             WimaxManagerConstants.WIMAX_STATE_CONNECTED);
    //     mWimaxIdle = (mWimaxExtraState == WimaxManagerConstants.WIMAX_IDLE);
    // }
    // UpdateDataNetType();
    // UpdateWimaxIcons();
}

void NetworkController::UpdateWimaxIcons()
{
    // if (mIsWimaxEnabled) {
    //     if (mWimaxConnected) {
    //         if (mWimaxIdle)
    //             mWimaxIconId = WimaxIcons.WIMAX_IDLE;
    //         else
    //             mWimaxIconId = WimaxIcons.WIMAX_SIGNAL_STRENGTH[mInetCondition][mWimaxSignal];
    //         mContentDescriptionWimax = mContext.getString(
    //                 AccessibilityContentDescriptions::WIMAX_CONNECTION_STRENGTH[mWimaxSignal]);
    //     } else {
    //         mWimaxIconId = WimaxIcons.WIMAX_DISCONNECTED;
    //         mContentDescriptionWimax = mContext.getString(R.string.accessibility_no_wimax);
    //     }
    // } else {
    //     mWimaxIconId = 0;
    // }
}

// ===== Full or limited Internet connectivity ==================================

void NetworkController::UpdateConnectivity(
    /* [in] */ IIntent* intent)
{
    if (CHATTY) {
        String str;
        intent->ToString(&str);
        Slogger::D(TAG, "updateConnectivity: intent=%s", str.string());
    }

    AutoPtr<IInterface> tmpObj;
    mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&tmpObj);
    AutoPtr<IConnectivityManager> connManager = IConnectivityManager::Probe(tmpObj.Get());
    assert(connManager != NULL);

    AutoPtr<INetworkInfo> info;
    connManager->GetActiveNetworkInfo((INetworkInfo**)&info);

    // Are we connected at all, by any interface?
    mConnected = FALSE;
    if (info != NULL) {
        info->IsConnected(&mConnected);
    }

    if (mConnected) {
        info->GetType(&mConnectedNetworkType);
        info->GetTypeName(&mConnectedNetworkTypeName);
    }
    else {
        mConnectedNetworkType = IConnectivityManager::TYPE_NONE;
        mConnectedNetworkTypeName = NULL;
    }

    Int32 connectionStatus;
    intent->GetInt32Extra(IConnectivityManager::EXTRA_INET_CONDITION, 0, &connectionStatus);

    if (CHATTY) {
        String str;
        if (info != NULL)   info->ToString(&str);
        Slogger::D(TAG, "updateConnectivity: networkInfo=%p", str.string());
        Slogger::D(TAG, "updateConnectivity: connectionStatus=%d", connectionStatus);
    }

    mInetCondition = (connectionStatus > INET_CONDITION_THRESHOLD ? 1 : 0);

    mBluetoothTethered = FALSE;
    if (info != NULL) {
        Int32 type;
        info->GetType(&type);
        if (type == IConnectivityManager::TYPE_BLUETOOTH) {
            info->IsConnected(&mBluetoothTethered);
        }
    }

    // We want to update all the icons, all at once, for any condition change
    UpdateDataNetType();
    UpdateWimaxIcons();
    UpdateDataIcon();
    UpdateTelephonySignalStrength();
    UpdateWifiIcons();
    UpdateEthernetIcons();
}

// ===== Update the views =======================================================

ECode NetworkController::RefreshViews()
{
    AutoPtr<IContext> context = mContext;

    Int32 combinedSignalIconId = 0;
    Int32 combinedActivityIconId = 0;
    String combinedLabel = String("");
    String wifiLabel = String("");
    String mobileLabel = String("");
    Boolean emergencyOnly;
    IsEmergencyOnly(&emergencyOnly);

    if (!mHasMobileDataFeature) {
        mDataSignalIconId = mPhoneSignalIconId = 0;
        mQSPhoneSignalIconId = 0;
        mobileLabel = String("");
    }
    else {
        // We want to show the carrier name if in service and either:
        //   - We are connected to mobile data, or
        //   - We are not connected to mobile data, as long as the *reason* packets are not
        //     being routed over that link is that we have better connectivity via wifi.
        // If data is disconnected for some other reason but wifi (or ethernet/bluetooth)
        // is connected, we show nothing.
        // Otherwise (nothing connected) we show "No internet connection".

        if (mDataConnected) {
            mobileLabel = mNetworkName;
        }
        else if (mConnected || emergencyOnly) {
            if (HasService() || emergencyOnly) {
                // The isEmergencyOnly test covers the case of a phone with no SIM
                mobileLabel = mNetworkName;
            } else {
                // Tablets, basically
                mobileLabel = "";
            }
        }
        else {
            context->GetString(SystemUIR::string::status_bar_settings_signal_meter_disconnected, &mobileLabel);
        }

        // Now for things that should only be shown when actually using mobile data.
        if (mDataConnected) {
            combinedSignalIconId = mDataSignalIconId;
            // switch (mDataActivity) {
            //     case ITelephonyManager::DATA_ACTIVITY_IN:
            //         mMobileActivityIconId = SystemUIR::drawable::stat_sys_signal_in;
            //         break;
            //     case ITelephonyManager::DATA_ACTIVITY_OUT:
            //         mMobileActivityIconId = SystemUIR::drawable::stat_sys_signal_out;
            //         break;
            //     case ITelephonyManager::DATA_ACTIVITY_INOUT:
            //         mMobileActivityIconId = SystemUIR::drawable::stat_sys_signal_inout;
            //         break;
            //     default:
            //         mMobileActivityIconId = 0;
            //         break;
            // }

            combinedLabel = mobileLabel;
            combinedActivityIconId = mMobileActivityIconId;
            combinedSignalIconId = mDataSignalIconId; // set by updateDataIcon()
            mContentDescriptionCombinedSignal = mContentDescriptionDataType;
        } else {
            mMobileActivityIconId = 0;
        }
    }

    if (mWifiConnected) {
        if (mWifiSsid.IsNull()) {
            context->GetString(SystemUIR::string::status_bar_settings_signal_meter_wifi_nossid, &wifiLabel);
            mWifiActivityIconId = 0; // no wifis, no bits
        }
        else {
            wifiLabel = mWifiSsid;
            if (DEBUG) {
                wifiLabel += "xxxxXXXXxxxxXXXX";
            }
            switch (mWifiActivity) {
                case IWifiManager::DATA_ACTIVITY_IN:
                    mWifiActivityIconId = SystemUIR::drawable::stat_sys_wifi_in;
                    break;
                case IWifiManager::DATA_ACTIVITY_OUT:
                    mWifiActivityIconId = SystemUIR::drawable::stat_sys_wifi_out;
                    break;
                case IWifiManager::DATA_ACTIVITY_INOUT:
                    mWifiActivityIconId = SystemUIR::drawable::stat_sys_wifi_inout;
                    break;
                case IWifiManager::DATA_ACTIVITY_NONE:
                    mWifiActivityIconId = 0;
                    break;
            }
        }

        combinedActivityIconId = mWifiActivityIconId;
        combinedLabel = wifiLabel;
        combinedSignalIconId = mWifiIconId; // set by updateWifiIcons()
        mContentDescriptionCombinedSignal = mContentDescriptionWifi;
    }
    else {
        if (mHasMobileDataFeature) {
            wifiLabel = "";
        } else {
            context->GetString(SystemUIR::string::status_bar_settings_signal_meter_disconnected, &wifiLabel);
        }
    }

    if (mEthernetConnected) {
        mContext->GetString(SystemUIR::string::ethernet_link, &combinedLabel);
        combinedActivityIconId = mEthernetIconId;
        combinedSignalIconId = mEthernetIconId;
        mContentDescriptionCombinedSignal = mContentDescriptionEthernet;
    }

    // if (mBluetoothTethered) {
    //     mContext->GetString(SystemUIR::string::bluetooth_tethered, &combinedLabel);
    //     combinedSignalIconId = mBluetoothTetherIconId;
    //     mContext->GetString(SystemUIR::string::accessibility_bluetooth_tether, &mContentDescriptionCombinedSignal);
    // }

    Boolean ethernetConnected = (mConnectedNetworkType == IConnectivityManager::TYPE_ETHERNET);
    if (ethernetConnected) {
        context->GetString(SystemUIR::string::ethernet_label, &combinedLabel);
    }

//TODO: Need mServiceState
    // mServiceState->IsEmergencyOnly(&b);
    // if (mAirplaneMode && (mServiceState == NULL || (!HasService() && !b))) {
    //     // Only display the flight-mode icon if not in "emergency calls only" mode.

    //     // look again; your radios are now airplanes
    //     mContext->GetString(SystemUIR::string::accessibility_airplane_mode, &mContentDescriptionPhoneSignal);
    //     mAirplaneIconId = SystemUIR::drawable::stat_sys_signal_flightmode;
    //     mPhoneSignalIconId = mDataSignalIconId = mDataTypeIconId = mQSDataTypeIconId = 0;
    //     mQSPhoneSignalIconId = 0;

    //     // combined values from connected wifi take precedence over airplane mode
    //     if (mWifiConnected || mShowEthIcon) {
    //         // Suppress "No internet connection." from mobile if wifi connected.
    //         mobileLabel = "";
    //     } else {
    //         if (mHasMobileDataFeature) {
    //             // let the mobile icon show "No internet connection."
    //             wifiLabel = "";
    //         } else {
    //             context->GetString(SystemUIR::string::status_bar_settings_signal_meter_disconnected, &wifiLabel);
    //             combinedLabel = wifiLabel;
    //         }
    //         mContentDescriptionCombinedSignal = mContentDescriptionPhoneSignal;
    //         combinedSignalIconId = mDataSignalIconId;
    //     }
    // }
    // else if (!mDataConnected && !mWifiConnected && !mBluetoothTethered && !mWimaxConnected && !mEthernetConnected && !mShowEthIcon) {
    //     // pretty much totally disconnected

    //     context->GetString(SystemUIR::string::status_bar_settings_signal_meter_disconnected, &combinedLabel);
    //     // On devices without mobile radios, we want to show the wifi icon
    //     combinedSignalIconId = mHasMobileDataFeature ? mDataSignalIconId : mWifiIconId;
    //     mContentDescriptionCombinedSignal = mHasMobileDataFeature ? mContentDescriptionDataType : mContentDescriptionWifi;

    //     mDataTypeIconId = 0;
    //     mQSDataTypeIconId = 0;

    //     Boolean b1,b2;
    //     IsCdmaEri(&b1);
    //     // mPhone->IsNetworkRoaming(&b2);
    //     if (IsCdma()) {
    //         if (b1) {
    //             mDataTypeIconId = SystemUIR::drawable::stat_sys_data_connected_roam;
    //             mQSDataTypeIconId = SystemUIR::drawable::ic_qs_signal_r;
    //         }
    //     } else if (b2) {
    //         mDataTypeIconId =SystemUIR::drawable::stat_sys_data_connected_roam;
    //         mQSDataTypeIconId = SystemUIR::drawable::ic_qs_signal_r;
    //     }
    // }

    // if (DEBUG) {
    //     Slog.d(TAG, "refreshViews connected={"
    //             + (mWifiConnected?" wifi":"")
    //             + (mDataConnected?" data":"")
    //             + " } level="
    //             + ((mSignalStrength == null)?"??":Integer.toString(mSignalStrength.getLevel()))
    //             + " combinedSignalIconId=0x"
    //             + Integer.toHexString(combinedSignalIconId)
    //             + "/" + getResourceName(combinedSignalIconId)
    //             + " combinedActivityIconId=0x" + Integer.toHexString(combinedActivityIconId)
    //             + " mobileLabel=" + mobileLabel
    //             + " wifiLabel=" + wifiLabel
    //             + " emergencyOnly=" + emergencyOnly
    //             + " combinedLabel=" + combinedLabel
    //             + " mAirplaneMode=" + mAirplaneMode
    //             + " mDataActivity=" + mDataActivity
    //             + " mPhoneSignalIconId=0x" + Integer.toHexString(mPhoneSignalIconId)
    //             + " mQSPhoneSignalIconId=0x" + Integer.toHexString(mQSPhoneSignalIconId)
    //             + " mDataDirectionIconId=0x" + Integer.toHexString(mDataDirectionIconId)
    //             + " mDataSignalIconId=0x" + Integer.toHexString(mDataSignalIconId)
    //             + " mDataTypeIconId=0x" + Integer.toHexString(mDataTypeIconId)
    //             + " mQSDataTypeIconId=0x" + Integer.toHexString(mQSDataTypeIconId)
    //             + " mWifiIconId=0x" + Integer.toHexString(mWifiIconId)
    //             + " mQSWifiIconId=0x" + Integer.toHexString(mQSWifiIconId)
    //             + " mBluetoothTetherIconId=0x" + Integer.toHexString(mBluetoothTetherIconId));
    // }

    if (mLastPhoneSignalIconId          != mPhoneSignalIconId
     || mLastDataDirectionOverlayIconId != combinedActivityIconId
     || mLastWifiIconId                 != mWifiIconId
     || mLastWimaxIconId                != mWimaxIconId
     || mLastDataTypeIconId             != mDataTypeIconId
     || mLastAirplaneMode               != mAirplaneMode)
    {
        // NB: the mLast*s will be updated later
        List<AutoPtr<INetworkControllerSignalCluster> >::Iterator it = mSignalClusters.Begin();
        for(; it != mSignalClusters.End(); it++) {
            AutoPtr<INetworkControllerSignalCluster> cluster = *it;
            assert(cluster);
            RefreshSignalCluster(cluster);
        }

        List<AutoPtr<INetworkSignalChangedCallback> >::Iterator ite = mSignalsChangedCallbacks.Begin();
        for(; ite != mSignalsChangedCallbacks.End(); ite++) {
            NotifySignalsChangedCallbacks(*ite);
        }
    }

    if (mLastAirplaneMode != mAirplaneMode) {
        mLastAirplaneMode = mAirplaneMode;
    }

    // the phone icon on phones
    if (mLastPhoneSignalIconId != mPhoneSignalIconId) {
        mLastPhoneSignalIconId = mPhoneSignalIconId;

        List<AutoPtr<IImageView> >::Iterator it = mPhoneSignalIconViews.Begin();
        for(; it != mPhoneSignalIconViews.End(); ++it) {
            AutoPtr<IImageView> v = *it;
            if (mPhoneSignalIconId == 0) {
                v->SetVisibility(IView::GONE);
            } else {
                v->SetVisibility(IView::VISIBLE);
                v->SetImageResource(mPhoneSignalIconId);
                AutoPtr<ICharSequence> cs;
                CString::New(mContentDescriptionPhoneSignal, (ICharSequence**)&cs);
                v->SetContentDescription(cs);
            }
        }
    }

    // the data icon on phones
    if (mLastDataDirectionIconId != mDataDirectionIconId) {
        mLastDataDirectionIconId = mDataDirectionIconId;
        List<AutoPtr<IImageView> >::Iterator it = mDataDirectionIconViews.Begin();
        for(; it != mDataDirectionIconViews.End(); ++it) {
            AutoPtr<IImageView> v = *it;
            v->SetImageResource(mDataDirectionIconId);
            AutoPtr<ICharSequence> cs;
            CString::New(mContentDescriptionDataType, (ICharSequence**)&cs);
            v->SetContentDescription(cs);
        }
    }

    // the wifi icon on phones
    if (mLastWifiIconId != mWifiIconId) {
        mLastWifiIconId = mWifiIconId;
        List<AutoPtr<IImageView> >::Iterator it = mWifiIconViews.Begin();
        for(; it != mWifiIconViews.End(); ++it) {
            AutoPtr<IImageView> v = *it;
            if (v) {
                if (mWifiIconId == 0) {
                    v->SetVisibility(IView::GONE);
                } else {
                    v->SetVisibility(IView::VISIBLE);
                    v->SetImageResource(mWifiIconId);
                    AutoPtr<ICharSequence> cs;
                    CString::New(mContentDescriptionWifi, (ICharSequence**)&cs);
                    v->SetContentDescription(cs);
                }
            }
        }
    }

    // the wimax icon on phones
    if (mLastWimaxIconId != mWimaxIconId) {
        mLastWimaxIconId = mWimaxIconId;
        List<AutoPtr<IImageView> >::Iterator it = mWimaxIconViews.Begin();
        for(; it != mWimaxIconViews.End(); ++it) {
            AutoPtr<IImageView> v = *it;
            if (mWimaxIconId == 0) {
                v->SetVisibility(IView::GONE);
            } else {
                v->SetVisibility(IView::VISIBLE);
                v->SetImageResource(mWimaxIconId);
                AutoPtr<ICharSequence> cs;
                CString::New(mContentDescriptionWimax, (ICharSequence**)&cs);
                v->SetContentDescription(cs);
            }
       }
    }

    // the combined data signal icon
    if (mLastCombinedSignalIconId != combinedSignalIconId) {
        mLastCombinedSignalIconId = combinedSignalIconId;
        List<AutoPtr<IImageView> >::Iterator it = mCombinedSignalIconViews.Begin();
        for(; it != mCombinedSignalIconViews.End(); ++it) {
            AutoPtr<IImageView> v = *it;
            v->SetImageResource(combinedSignalIconId);
            AutoPtr<ICharSequence> cs;
            CString::New(mContentDescriptionCombinedSignal, (ICharSequence**)&cs);
            v->SetContentDescription(cs);
        }
    }

    // the data network type overlay
    if (mLastDataTypeIconId != mDataTypeIconId) {
        mLastDataTypeIconId = mDataTypeIconId;
        List<AutoPtr<IImageView> >::Iterator it = mDataTypeIconViews.Begin();
        for(; it != mDataTypeIconViews.End(); ++it) {
            AutoPtr<IImageView> v = *it;
            if (mDataTypeIconId == 0) {
                v->SetVisibility(IView::GONE);
            } else {
                v->SetVisibility(IView::VISIBLE);
                v->SetImageResource(mDataTypeIconId);
                AutoPtr<ICharSequence> cs;
                CString::New(mContentDescriptionDataType, (ICharSequence**)&cs);
                v->SetContentDescription(cs);
            }
        }
    }

    // the data direction overlay
    if (mLastDataDirectionOverlayIconId != combinedActivityIconId) {
        if (DEBUG) {
            Slogger::D(TAG, "changing data overlay icon id to %d", combinedActivityIconId);
        }
        mLastDataDirectionOverlayIconId = combinedActivityIconId;
        List<AutoPtr<IImageView> >::Iterator it = mDataDirectionOverlayIconViews.Begin();
        for(; it != mDataDirectionOverlayIconViews.End(); ++it) {
            AutoPtr<IImageView> v = *it;
            if (combinedActivityIconId == 0) {
                v->SetVisibility(IView::GONE);
            } else {
                v->SetVisibility(IView::VISIBLE);
                v->SetImageResource(combinedActivityIconId);
                AutoPtr<ICharSequence> cs;
                CString::New(mContentDescriptionDataType, (ICharSequence**)&cs);
                v->SetContentDescription(cs);
            }
        }
    }

    // the combinedLabel in the notification panel
    if (!mLastCombinedLabel.Equals(combinedLabel)) {
        mLastCombinedLabel = combinedLabel;
        List<AutoPtr<ITextView> >::Iterator it = mCombinedLabelViews.Begin();
        for(; it != mCombinedLabelViews.End(); ++it) {
            AutoPtr<ITextView> v = *it;
            AutoPtr<ICharSequence> cs;
            CString::New(combinedLabel, (ICharSequence**)&cs);
            v->SetText(cs);
        }
    }

    // wifi label
    List<AutoPtr<ITextView> >::Iterator it = mWifiLabelViews.Begin();
    for(; it != mWifiLabelViews.End(); ++it) {
        AutoPtr<ITextView> v = *it;
        AutoPtr<ICharSequence> cs;
        CString::New(wifiLabel, (ICharSequence**)&cs);
        v->SetText(cs);
        if (wifiLabel.Equals("")) {
            v->SetVisibility(IView::GONE);
        } else {
            v->SetVisibility(IView::VISIBLE);
        }
    }

    // mobile label
    List<AutoPtr<ITextView> >::Iterator ite = mMobileLabelViews.Begin();
    for(; ite != mMobileLabelViews.End(); ++ite) {
        AutoPtr<ITextView> v = *ite;
        AutoPtr<ICharSequence> cs;
        CString::New(mobileLabel, (ICharSequence**)&cs);
        v->SetText(cs);
        if (mobileLabel.Equals("")) {
            v->SetVisibility(IView::GONE);
        } else {
            v->SetVisibility(IView::VISIBLE);
        }
    }

    // e-call label
    List<AutoPtr<ITextView> >::Iterator iter = mEmergencyLabelViews.Begin();
    for(; iter != mEmergencyLabelViews.End(); ++iter) {
        AutoPtr<ITextView> v = *iter;
        if (!emergencyOnly) {
            v->SetVisibility(IView::GONE);
        } else {
            AutoPtr<ICharSequence> cs;
            CString::New(mobileLabel, (ICharSequence**)&cs);
            v->SetText(cs);
            v->SetVisibility(IView::VISIBLE);
        }
    }

    return NOERROR;
}

String NetworkController::GetResourceName(
    /* [in] */ Int32 resId)
{
    String str("(null)");
    if (resId != 0) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        // try {
        ECode ec = res->GetResourceName(resId, &str);
        if (ec == (ECode)E_NOT_FOUND_EXCEPTION) {
            str = String("(unknown)");
        }
        // } catch (android.content.res.Resources.NotFoundException ex) {
        //     return "(unknown)";
        // }
    }
    return str;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
