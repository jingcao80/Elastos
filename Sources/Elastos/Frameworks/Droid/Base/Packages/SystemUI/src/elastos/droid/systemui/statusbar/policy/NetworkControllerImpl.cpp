
#include "elastos/droid/systemui/statusbar/policy/NetworkControllerImpl.h"
#include "elastos/droid/systemui/statusbar/policy/CNetworkControllerBroadcastReceiver.h"
#include "elastos/droid/systemui/statusbar/policy/AccessibilityContentDescriptions.h"
#include "elastos/droid/systemui/statusbar/policy/MobileDataController.h"
#include "elastos/droid/systemui/statusbar/policy/TelephonyIcons.h"
#include "elastos/droid/systemui/statusbar/policy/WifiAccessPointController.h"
#include "elastos/droid/systemui/statusbar/policy/WifiIcons.h"
#include "elastos/droid/systemui/statusbar/policy/WimaxIcons.h"
#include "../R.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/provider/Settings.h>
#include <elastos/droid/R.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Internal::Telephony::IIccCardConstants;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_ABSENT;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PIN_REQUIRED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PUK_REQUIRED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PERSO_LOCKED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_READY;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_UNKNOWN;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Utility::CAsyncChannel;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::Wimax::IWimaxManagerConstants;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::View::IView;
using Elastos::Droid::Wifi::CWifiManagerHelper;
using Elastos::Droid::Wifi::IWifiManagerHelper;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const String NetworkControllerImpl::TAG("StatusBar.NetworkController");
const Boolean NetworkControllerImpl::DEBUG = FALSE;
const Boolean NetworkControllerImpl::CHATTY = FALSE; // additional diagnostics, but not logspew
const Int32 NetworkControllerImpl::INET_CONDITION_THRESHOLD = 50;

NetworkControllerImpl::NCPhoneStateListener::NCPhoneStateListener(
    /* [in] */ NetworkControllerImpl* host)
    : mHost(host)
{
    PhoneStateListener::constructor();
}

ECode NetworkControllerImpl::NCPhoneStateListener::OnSignalStrengthsChanged(
    /* [in] */ ISignalStrength* signalStrength)
{
    if (DEBUG) {
        if (signalStrength == NULL) {
            Logger::D(mHost->TAG, "onSignalStrengthsChanged signalStrength=NULL");
        }
        else {
            Int32 level = 0;
            signalStrength->GetLevel(&level);
            Logger::D(mHost->TAG, "onSignalStrengthsChanged signalStrength=%p level=%d", signalStrength, level);
        }
    }
    mHost->mSignalStrength = signalStrength;
    mHost->UpdateTelephonySignalStrength();
    mHost->RefreshViews();
    return NOERROR;
}

ECode NetworkControllerImpl::NCPhoneStateListener::OnServiceStateChanged(
    /* [in] */ IServiceState* state)
{
    if (DEBUG) {
        Int32 s1 = 0, s2 = 0;
        state->GetVoiceRegState(&s1);
        state->GetDataRegState(&s2);
        Logger::D(mHost->TAG, "onServiceStateChanged voiceState=%d dataState=%d", s1, s2);
    }
    mHost->mServiceState = state;
    mHost->UpdateTelephonySignalStrength();
    mHost->UpdateDataNetType();
    mHost->UpdateDataIcon();
    mHost->RefreshViews();
    return NOERROR;
}

ECode NetworkControllerImpl::NCPhoneStateListener::OnCallStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ const String& incomingNumber)
{
    if (DEBUG) {
        Logger::D(mHost->TAG, "onCallStateChanged state=%d", state);
    }
    // In cdma, if a voice call is made, RSSI should switch to 1x.
    if (mHost->IsCdma()) {
        mHost->UpdateTelephonySignalStrength();
        mHost->RefreshViews();
    }
    return NOERROR;
}

ECode NetworkControllerImpl::NCPhoneStateListener::OnDataConnectionStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ Int32 networkType)
{
    if (DEBUG) {
        Logger::D(mHost->TAG, "onDataConnectionStateChanged: state=%d, type=%d", state, networkType);
    }
    mHost->mDataState = state;
    mHost->mDataNetType = networkType;
    mHost->UpdateDataNetType();
    mHost->UpdateDataIcon();
    mHost->RefreshViews();
    return NOERROR;
}

ECode NetworkControllerImpl::NCPhoneStateListener::OnDataActivity(
    /* [in] */ Int32 direction)
{
    if (DEBUG) {
        Logger::D(mHost->TAG, "onDataActivity: direction=%d", direction);
    }
    mHost->mDataActivity = direction;
    mHost->UpdateDataIcon();
    mHost->RefreshViews();
    return NOERROR;
}

NetworkControllerImpl::WifiHandler::WifiHandler(
    /* [in] */ NetworkControllerImpl* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode NetworkControllerImpl::WifiHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case IAsyncChannel::CMD_CHANNEL_HALF_CONNECTED: {
            Int32 arg1 = 0;
            msg->GetArg1(&arg1);
            if (arg1 == IAsyncChannel::STATUS_SUCCESSFUL) {
                AutoPtr<IMessageHelper> mh;
                CMessageHelper::AcquireSingleton((IMessageHelper**)&mh);
                AutoPtr<IMessage> m;
                mh->Obtain(this, IAsyncChannel::CMD_CHANNEL_FULL_CONNECTION, (IMessage**)&m);
                mHost->mWifiChannel->SendMessage(m);
            }
            else {
                Logger::E(mHost->TAG, "Failed to connect to wifi");
            }
            break;
        }
        case IWifiManager::DATA_ACTIVITY_NOTIFICATION: {
            Int32 arg1 = 0;
            msg->GetArg1(&arg1);
            if (arg1 != mHost->mWifiActivity) {
                mHost->mWifiActivity = arg1;
                mHost->RefreshViews();
            }
            break;
        }
        default:
            //Ignore
            break;
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(NetworkControllerImpl::MobileDataControllerCallback, Object, IMobileDataControllerCallback)
NetworkControllerImpl::MobileDataControllerCallback::MobileDataControllerCallback(
    /* [in] */ NetworkControllerImpl* host)
    : mHost(host)
{}

ECode NetworkControllerImpl::MobileDataControllerCallback::OnMobileDataEnabled(
    /* [in] */ Boolean enabled)
{
    mHost->NotifyMobileDataEnabled(enabled);
    return NOERROR;
}

NetworkControllerImpl::NCIAsyncTask::NCIAsyncTask(
    /* [in] */ NetworkControllerImpl* host,
    /* [in] */ Boolean enabled)
    : mHost(host)
    , mEnabled(enabled)
{}

ECode NetworkControllerImpl::NCIAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // Disable tethering if enabling Wifi
    if (mHost->mWifiManager != NULL) {
        Boolean tmp = FALSE;
        Int32 wifiApState = 0;
        mHost->mWifiManager->GetWifiApState(&wifiApState);
        if (mEnabled && ((wifiApState == IWifiManager::WIFI_AP_STATE_ENABLING) ||
                       (wifiApState == IWifiManager::WIFI_AP_STATE_ENABLED))) {
            mHost->mWifiManager->SetWifiApEnabled(NULL, FALSE, &tmp);
        }

        mHost->mWifiManager->SetWifiEnabled(mEnabled, &tmp);
    }

    *result = NULL;
    return NOERROR;
}

CAR_INTERFACE_IMPL_3(NetworkControllerImpl, Object, INetworkControllerImpl, INetworkController, IDemoMode);
NetworkControllerImpl::NetworkControllerImpl(
    /* [in] */ IContext* context)
    : mHspaDataDistinguishable(FALSE)
    , mDataConnected(FALSE)
    , mSimState(IccCardConstantsState_READY)
    , mPhoneState(ITelephonyManager::CALL_STATE_IDLE)
    , mDataNetType(ITelephonyManager::NETWORK_TYPE_UNKNOWN)
    , mDataState(ITelephonyManager::DATA_DISCONNECTED)
    , mDataActivity(ITelephonyManager::DATA_ACTIVITY_NONE)
    , mDataIconList(TelephonyIcons::DATA_G[0])
    , mPhoneSignalIconId(0)
    , mQSPhoneSignalIconId(0)
    , mDataDirectionIconId(0)
    , mDataSignalIconId(0)
    , mDataTypeIconId(0)
    , mQSDataTypeIconId(0)
    , mAirplaneIconId(0)
    , mDataActive(FALSE)
    , mNoSim(FALSE)
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
    , mWifiActivity(IWifiManager::DATA_ACTIVITY_NONE)
    , mBluetoothTethered(FALSE)
    , mBluetoothTetherIconId(Elastos::Droid::R::drawable::stat_sys_tether_bluetooth)
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
    , mLastConnectedNetworkType(IConnectivityManager::TYPE_NONE)
    , mInetCondition(0)
    , mLastInetCondition(0)
    , mAirplaneMode(FALSE)
    , mLastAirplaneMode(TRUE)
    , mLastPhoneSignalIconId(-1)
    , mLastDataDirectionIconId(-1)
    , mLastWifiIconId(-1)
    , mLastWimaxIconId(-1)
    , mLastCombinedSignalIconId(-1)
    , mLastDataTypeIconId(-1)
    , mLastCombinedLabel("")
    , mHasMobileDataFeature(FALSE)
    , mDataAndWifiStacked(FALSE)
    , mDemoMode(FALSE)
    , mDemoInetCondition(0)
    , mDemoWifiLevel(0)
    , mDemoDataTypeIconId(0)
    , mDemoQSDataTypeIconId(0)
    , mDemoMobileLevel(0)
{
    mHasMobileDataFeature = TRUE;
    Logger::D(TAG, "TODO : Set mHasMobileDataFeature = TRUE to show the signal image.");
    CArrayList::New((IArrayList**)&mCombinedLabelViews);
    CArrayList::New((IArrayList**)&mMobileLabelViews);
    CArrayList::New((IArrayList**)&mWifiLabelViews);
    CArrayList::New((IArrayList**)&mEmergencyViews);
    CArrayList::New((IArrayList**)&mSignalClusters);
    CArrayList::New((IArrayList**)&mSignalsChangedCallbacks);
    mPhoneStateListener = new NCPhoneStateListener(this);

    mContext = context;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    AutoPtr<IInterface> obj;
    Logger::D(TAG, "TODO: CONNECTIVITY_SERVICE.");
    // mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    // AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(obj);
    // cm->IsNetworkSupported(IConnectivityManager::TYPE_MOBILE, &mHasMobileDataFeature);

    res->GetBoolean(R::bool_::config_showPhoneRSSIForData, &mShowPhoneRSSIForData);
    res->GetBoolean(R::bool_::config_showMin3G, &mShowAtLeastThreeGees);
    res->GetBoolean(Elastos::Droid::R::bool_::config_alwaysUseCdmaRssi, &mAlwaysShowCdmaRssi);

    // set up the default wifi icon, used when no radios have ever appeared
    UpdateWifiIcons();
    UpdateWimaxIcons();

    // telephony
    obj = NULL;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    mPhone = ITelephonyManager::Probe(obj);
    Logger::D(TAG, "TODO: TELEPHONY_SERVICE not ready.");
    // mPhone->Listen(mPhoneStateListener,
    //                   IPhoneStateListener::LISTEN_SERVICE_STATE
    //                 | IPhoneStateListener::LISTEN_SIGNAL_STRENGTHS
    //                 | IPhoneStateListener::LISTEN_CALL_STATE
    //                 | IPhoneStateListener::LISTEN_DATA_CONNECTION_STATE
    //                 | IPhoneStateListener::LISTEN_DATA_ACTIVITY);

    res->GetBoolean(R::bool_::config_hspa_data_distinguishable, &mHspaDataDistinguishable);
    mContext->GetString(R::string::status_bar_network_name_separator, &mNetworkNameSeparator);
    mContext->GetString(Elastos::Droid::R::string::lockscreen_carrier_default, &mNetworkNameDefault);
    mNetworkName = mNetworkNameDefault;

    // wifi
    obj = NULL;
    Logger::D(TAG, "TODO: WIFI_SERVICE not ready.");
    // context->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&obj);
    mWifiManager = IWifiManager::Probe(obj);
    AutoPtr<IHandler> handler = new WifiHandler(this);
    CAsyncChannel::New((IAsyncChannel**)&mWifiChannel);
    AutoPtr<IMessenger> wifiMessenger;
    if (mWifiManager != NULL) {
        mWifiManager->GetWifiServiceMessenger((IMessenger**)&wifiMessenger);
    }
    if (wifiMessenger != NULL) {
        mWifiChannel->Connect(mContext, handler, wifiMessenger);
    }

    // broadcasts
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IWifiManager::RSSI_CHANGED_ACTION);
    filter->AddAction(IWifiManager::WIFI_STATE_CHANGED_ACTION);
    filter->AddAction(IWifiManager::NETWORK_STATE_CHANGED_ACTION);
    filter->AddAction(ITelephonyIntents::ACTION_SIM_STATE_CHANGED);
    filter->AddAction(ITelephonyIntents::SPN_STRINGS_UPDATED_ACTION);
    filter->AddAction(IConnectivityManager::CONNECTIVITY_ACTION_IMMEDIATE);
    filter->AddAction(IConnectivityManager::INET_CONDITION_ACTION);
    filter->AddAction(IIntent::ACTION_CONFIGURATION_CHANGED);
    filter->AddAction(IIntent::ACTION_AIRPLANE_MODE_CHANGED);
    res->GetBoolean(Elastos::Droid::R::bool_::config_wimaxEnabled, &mWimaxSupported);
    if(mWimaxSupported) {
        filter->AddAction(IWimaxManagerConstants::WIMAX_NETWORK_STATE_CHANGED_ACTION);
        filter->AddAction(IWimaxManagerConstants::SIGNAL_LEVEL_CHANGED_ACTION);
        filter->AddAction(IWimaxManagerConstants::NET_4G_STATE_CHANGED_ACTION);
    }

    AutoPtr<IIntent> i;
    AutoPtr<IBroadcastReceiver> b;
    CNetworkControllerBroadcastReceiver::New(this, (IBroadcastReceiver**)&b);
    context->RegisterReceiver(b, filter, (IIntent**)&i);

    // AIRPLANE_MODE_CHANGED is sent at boot; we've probably already missed it
    UpdateAirplaneMode();

    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    config->GetLocale((ILocale**)&mLastLocale);
    mAccessPoints = new WifiAccessPointController(mContext);
    mMobileDataController = new MobileDataController(mContext);
    AutoPtr<MobileDataControllerCallback> cb = new MobileDataControllerCallback(this);
    mMobileDataController->SetCallback(cb);
}

void NetworkControllerImpl::NotifyMobileDataEnabled(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IIterator> ator;
    mSignalsChangedCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> cb;
        ator->GetNext((IInterface**)&cb);
        INetworkSignalChangedCallback::Probe(cb)->OnMobileDataEnabled(enabled);
    }
}

ECode NetworkControllerImpl::HasMobileDataFeature(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    *has = mHasMobileDataFeature;
    return NOERROR;
}

ECode NetworkControllerImpl::HasVoiceCallingFeature(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    Int32 type = 0;
    if (mPhone != NULL) {
        mPhone->GetPhoneType(&type);
    }
    *has = type != ITelephonyManager::PHONE_TYPE_NONE;
    return NOERROR;
}

ECode NetworkControllerImpl::IsEmergencyOnly(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    *result = (mServiceState != NULL && (mServiceState->IsEmergencyOnly(&tmp), tmp));
    return NOERROR;
}

ECode NetworkControllerImpl::AddCombinedLabelView(
    /* [in] */ ITextView* v)
{
    mCombinedLabelViews->Add(v);
    return NOERROR;
}

ECode NetworkControllerImpl::AddMobileLabelView(
    /* [in] */ ITextView* v)
{
    mMobileLabelViews->Add(v);
    return NOERROR;
}

ECode NetworkControllerImpl::AddWifiLabelView(
    /* [in] */ ITextView* v)
{
    mWifiLabelViews->Add(v);
    return NOERROR;
}

ECode NetworkControllerImpl::AddEmergencyLabelView(
    /* [in] */ IStatusBarHeaderView* v)
{
    mEmergencyViews->Add(v);
    return NOERROR;
}

ECode NetworkControllerImpl::AddSignalCluster(
    /* [in] */ INetworkControllerImplSignalCluster* cluster)
{
    mSignalClusters->Add(cluster);
    RefreshSignalCluster(cluster);
    return NOERROR;
}

ECode NetworkControllerImpl::AddNetworkSignalChangedCallback(
    /* [in] */ INetworkSignalChangedCallback* cb)
{
    mSignalsChangedCallbacks->Add(cb);
    NotifySignalsChangedCallbacks(cb);
    return NOERROR;
}

ECode NetworkControllerImpl::RemoveNetworkSignalChangedCallback(
    /* [in] */ INetworkSignalChangedCallback* cb)
{
    mSignalsChangedCallbacks->Remove(cb);
    return NOERROR;
}

ECode NetworkControllerImpl::AddAccessPointCallback(
    /* [in] */ INetworkControllerAccessPointCallback* callback)
{
    mAccessPoints->AddCallback(callback);
    return NOERROR;
}

ECode NetworkControllerImpl::RemoveAccessPointCallback(
    /* [in] */ INetworkControllerAccessPointCallback* callback)
{
    mAccessPoints->RemoveCallback(callback);
    return NOERROR;
}

ECode NetworkControllerImpl::ScanForAccessPoints()
{
    mAccessPoints->Scan();
    return NOERROR;
}

ECode NetworkControllerImpl::Connect(
    /* [in] */ INetworkControllerAccessPoint* ap)
{
    mAccessPoints->Connect(ap);
    return NOERROR;
}

ECode NetworkControllerImpl::SetWifiEnabled(
    /* [in] */ Boolean enabled)
{
    AutoPtr<NCIAsyncTask> task = new NCIAsyncTask(this, enabled);
    task->Execute((ArrayOf<IInterface*>*)NULL);
    return NOERROR;
}

ECode NetworkControllerImpl::GetDataUsageInfo(
    /* [out] */ INetworkControllerDataUsageInfo** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<INetworkControllerDataUsageInfo> info;
    mMobileDataController->GetDataUsageInfo((INetworkControllerDataUsageInfo**)&info);
    if (info != NULL) {
        info->SetCarrier(mNetworkName);
    }
    *result = info;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkControllerImpl::IsMobileDataSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mMobileDataController->IsMobileDataSupported(result);
}

ECode NetworkControllerImpl::IsMobileDataEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mMobileDataController->IsMobileDataEnabled(result);
}

ECode NetworkControllerImpl::SetMobileDataEnabled(
    /* [in] */ Boolean enabled)
{
    mMobileDataController->SetMobileDataEnabled(enabled);
    return NOERROR;
}

Boolean NetworkControllerImpl::IsTypeIconWide(
    /* [in] */ Int32 iconId)
{
    return TelephonyIcons::ICON_LTE == iconId || TelephonyIcons::ICON_1X == iconId
            || TelephonyIcons::ICON_3G == iconId || TelephonyIcons::ICON_4G == iconId;
}

Boolean NetworkControllerImpl::IsQsTypeIconWide(
    /* [in] */ Int32 iconId)
{
    return TelephonyIcons::QS_ICON_LTE == iconId || TelephonyIcons::QS_ICON_1X == iconId
            || TelephonyIcons::QS_ICON_3G == iconId || TelephonyIcons::QS_ICON_4G == iconId;
}

ECode NetworkControllerImpl::RefreshSignalCluster(
    /* [in] */ INetworkControllerImplSignalCluster* cluster)
{
    if (mDemoMode) return NOERROR;
    cluster->SetWifiIndicators(
            // only show wifi in the cluster if connected or if wifi-only
            mWifiEnabled && (mWifiConnected || !mHasMobileDataFeature),
            mWifiIconId,
            mContentDescriptionWifi);

    if (mIsWimaxEnabled && mWimaxConnected) {
        // wimax is special
        cluster->SetMobileDataIndicators(
                TRUE,
                mAlwaysShowCdmaRssi ? mPhoneSignalIconId : mWimaxIconId,
                mDataTypeIconId,
                mContentDescriptionWimax,
                mContentDescriptionDataType,
                mDataTypeIconId == TelephonyIcons::ROAMING_ICON,
                FALSE /* IsTypeIconWide */ );
    }
    else {
        // normal mobile data
        cluster->SetMobileDataIndicators(
                mHasMobileDataFeature,
                mShowPhoneRSSIForData ? mPhoneSignalIconId : mDataSignalIconId,
                mDataTypeIconId,
                mContentDescriptionPhoneSignal,
                mContentDescriptionDataType,
                mDataTypeIconId == TelephonyIcons::ROAMING_ICON,
                IsTypeIconWide(mDataTypeIconId));
    }
    cluster->SetIsAirplaneMode(mAirplaneMode, mAirplaneIconId);
    return NOERROR;
}

void NetworkControllerImpl::NotifySignalsChangedCallbacks(
    /* [in] */ INetworkSignalChangedCallback* cb)
{
    // only show wifi in the cluster if connected or if wifi-only
    Boolean wifiEnabled = mWifiEnabled && (mWifiConnected || !mHasMobileDataFeature);
    String wifiDesc = wifiEnabled ?
            mWifiSsid : String(NULL);
    Boolean wifiIn = wifiEnabled && mWifiSsid != NULL
            && (mWifiActivity == IWifiManager::DATA_ACTIVITY_INOUT
            || mWifiActivity == IWifiManager::DATA_ACTIVITY_IN);
    Boolean wifiOut = wifiEnabled && mWifiSsid != NULL
            && (mWifiActivity == IWifiManager::DATA_ACTIVITY_INOUT
            || mWifiActivity == IWifiManager::DATA_ACTIVITY_OUT);
    cb->OnWifiSignalChanged(mWifiEnabled, mWifiConnected, mQSWifiIconId, wifiIn, wifiOut,
            mContentDescriptionWifi, wifiDesc);

    Boolean mobileIn = mDataConnected && (mDataActivity == ITelephonyManager::DATA_ACTIVITY_INOUT
            || mDataActivity == ITelephonyManager::DATA_ACTIVITY_IN);
    Boolean mobileOut = mDataConnected && (mDataActivity == ITelephonyManager::DATA_ACTIVITY_INOUT
            || mDataActivity == ITelephonyManager::DATA_ACTIVITY_OUT);

    Boolean tmp = FALSE;
    if (IsEmergencyOnly(&tmp), &tmp) {
        cb->OnMobileDataSignalChanged(FALSE, mQSPhoneSignalIconId,
                mContentDescriptionPhoneSignal, mQSDataTypeIconId, mobileIn, mobileOut,
                mContentDescriptionDataType, String(NULL), mNoSim, IsQsTypeIconWide(mQSDataTypeIconId));
    }
    else {
        if (mIsWimaxEnabled && mWimaxConnected) {
            // Wimax is special
            cb->OnMobileDataSignalChanged(TRUE, mQSPhoneSignalIconId,
                    mContentDescriptionPhoneSignal, mQSDataTypeIconId, mobileIn, mobileOut,
                    mContentDescriptionDataType, mNetworkName, mNoSim,
                    IsQsTypeIconWide(mQSDataTypeIconId));
        }
        else {
            // Normal mobile data
            cb->OnMobileDataSignalChanged(mHasMobileDataFeature, mQSPhoneSignalIconId,
                    mContentDescriptionPhoneSignal, mQSDataTypeIconId, mobileIn, mobileOut,
                    mContentDescriptionDataType, mNetworkName, mNoSim,
                    IsQsTypeIconWide(mQSDataTypeIconId));
        }
    }
    cb->OnAirplaneModeChanged(mAirplaneMode);
}

ECode NetworkControllerImpl::SetStackedMode(
    /* [in] */ Boolean stacked)
{
    mDataAndWifiStacked = TRUE;
    return NOERROR;
}

void NetworkControllerImpl::UpdateSimState(
    /* [in] */ IIntent* intent)
{
    String stateExtra;
    intent->GetStringExtra(IIccCardConstants::INTENT_KEY_ICC_STATE, &stateExtra);
    if (IIccCardConstants::INTENT_VALUE_ICC_ABSENT.Equals(stateExtra)) {
        mSimState = IccCardConstantsState_ABSENT;
    }
    else if (IIccCardConstants::INTENT_VALUE_ICC_READY.Equals(stateExtra)) {
        mSimState = IccCardConstantsState_READY;
    }
    else if (IIccCardConstants::INTENT_VALUE_ICC_LOCKED.Equals(stateExtra)) {
        String lockedReason;
        intent->GetStringExtra(IIccCardConstants::INTENT_KEY_LOCKED_REASON, &lockedReason);
        if (IIccCardConstants::INTENT_VALUE_LOCKED_ON_PIN.Equals(lockedReason)) {
            mSimState = IccCardConstantsState_PIN_REQUIRED;
        }
        else if (IIccCardConstants::INTENT_VALUE_LOCKED_ON_PUK.Equals(lockedReason)) {
            mSimState = IccCardConstantsState_PUK_REQUIRED;
        }
        else {
            mSimState = IccCardConstantsState_PERSO_LOCKED;
        }
    }
    else {
        mSimState = IccCardConstantsState_UNKNOWN;
    }
    if (DEBUG) Logger::D(TAG, "updateSimState: mSimState=%d", mSimState);
}

Boolean NetworkControllerImpl::IsCdma()
{
    Boolean tmp = FALSE;
    return (mSignalStrength != NULL) && (mSignalStrength->IsGsm(&tmp), !tmp);
}

Boolean NetworkControllerImpl::HasService()
{
    Boolean retVal;
    if (mServiceState != NULL) {
        // Consider the device to be in service if either voice or data service is available.
        // Some SIM cards are marketed as data-only and do not support voice service, and on
        // these SIM cards, we want to show signal bars for data service as well as the "no
        // service" or "emergency calls only" text that indicates that voice is not available.
        Int32 state = 0;
        switch(mServiceState->GetVoiceRegState(&state), state) {
            case IServiceState::STATE_POWER_OFF:
                retVal = FALSE;
                break;
            case IServiceState::STATE_OUT_OF_SERVICE:
            case IServiceState::STATE_EMERGENCY_ONLY: {
                retVal = (mServiceState->GetDataRegState(&state), state) == IServiceState::STATE_IN_SERVICE;
                break;
            }
            default:
                retVal = TRUE;
        }
    }
    else {
        retVal = FALSE;
    }
    if (DEBUG) Logger::D(TAG, "HasService: mServiceState=%p retVal=%d", mServiceState.Get(), retVal);
    return retVal;
}

void NetworkControllerImpl::UpdateAirplaneMode()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 v = 0;
    Elastos::Droid::Provider::Settings::Global::GetInt32(cr, ISettingsGlobal::AIRPLANE_MODE_ON, 0, &v);
    mAirplaneMode = (v == 1);
}

void NetworkControllerImpl::RefreshLocale()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);

    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    config->GetLocale((ILocale**)&mLocale);
}

void NetworkControllerImpl::UpdateTelephonySignalStrength()
{
    if (DEBUG) {
        Logger::D(TAG, "UpdateTelephonySignalStrength: HasService=%d ss=%p", HasService(), mSignalStrength.Get());
    }
    if (!HasService()) {
        if (CHATTY) Logger::D(TAG, "UpdateTelephonySignalStrength: !HasService()");
        mPhoneSignalIconId = R::drawable::stat_sys_signal_null;
        mQSPhoneSignalIconId = R::drawable::ic_qs_signal_no_signal;
        mDataSignalIconId = R::drawable::stat_sys_signal_null;
        mContext->GetString(AccessibilityContentDescriptions::PHONE_SIGNAL_STRENGTH[0],
                &mContentDescriptionPhoneSignal);
    }
    else {
        if (mSignalStrength == NULL) {
            if (CHATTY) Logger::D(TAG, "UpdateTelephonySignalStrength: mSignalStrength == NULL");
            mPhoneSignalIconId = R::drawable::stat_sys_signal_null;
            mQSPhoneSignalIconId = R::drawable::ic_qs_signal_no_signal;
            mDataSignalIconId = R::drawable::stat_sys_signal_null;
            mContext->GetString(AccessibilityContentDescriptions::PHONE_SIGNAL_STRENGTH[0],
                    &mContentDescriptionPhoneSignal);
        }
        else {
            Int32 iconLevel = 0;
            const Int32* iconList;
            if (IsCdma() && mAlwaysShowCdmaRssi) {
                mSignalStrength->GetCdmaLevel(&iconLevel);
                mLastSignalLevel = iconLevel;
                if (DEBUG) {
                    Int32 l = 0;
                    mSignalStrength->GetLevel(&l);
                    Logger::D(TAG, "UpdateTelephonySignalStrength: mAlwaysShowCdmaRssi=%d set to cdmaLevel=%d instead of level=%d"
                        , mAlwaysShowCdmaRssi, iconLevel, l);
                }
            }
            else {
                mSignalStrength->GetLevel(&iconLevel);
                mLastSignalLevel = iconLevel;
            }

            if (IsRoaming()) {
                iconList = TelephonyIcons::TELEPHONY_SIGNAL_STRENGTH_ROAMING[mInetCondition];
            }
            else {
                iconList = TelephonyIcons::TELEPHONY_SIGNAL_STRENGTH[mInetCondition];
            }
            mPhoneSignalIconId = iconList[iconLevel];
            mQSPhoneSignalIconId =
                    TelephonyIcons::QS_TELEPHONY_SIGNAL_STRENGTH[mInetCondition][iconLevel];
            mContext->GetString(AccessibilityContentDescriptions::PHONE_SIGNAL_STRENGTH[iconLevel]
                        , &mContentDescriptionPhoneSignal);
            mDataSignalIconId = TelephonyIcons::DATA_SIGNAL_STRENGTH[mInetCondition][iconLevel];
            if (DEBUG) Logger::D(TAG, "UpdateTelephonySignalStrength: iconLevel=%d", iconLevel);
        }
    }
}

Int32 NetworkControllerImpl::InetConditionForNetwork(
    /* [in] */ Int32 networkType)
{
    return (mInetCondition == 1 && mConnectedNetworkType == networkType) ? 1 : 0;
}

void NetworkControllerImpl::UpdateDataNetType()
{
    Int32 inetCondition;
    mDataTypeIconId = mQSDataTypeIconId = 0;
    if (mIsWimaxEnabled && mWimaxConnected) {
        // wimax is a special 4g network not handled by telephony
        inetCondition = InetConditionForNetwork(IConnectivityManager::TYPE_WIMAX);
        mDataIconList = TelephonyIcons::DATA_4G[inetCondition];
        mDataTypeIconId = R::drawable::stat_sys_data_fully_connected_4g;
        mQSDataTypeIconId = TelephonyIcons::QS_DATA_4G[inetCondition];
        mContext->GetString(R::string::accessibility_data_connection_4g, &mContentDescriptionDataType);
    }
    else {
        inetCondition = InetConditionForNetwork(IConnectivityManager::TYPE_MOBILE);
        Boolean showDataTypeIcon = (inetCondition > 0);
        switch (mDataNetType) {
            case ITelephonyManager::NETWORK_TYPE_UNKNOWN:
                if (!mShowAtLeastThreeGees) {
                    mDataIconList = TelephonyIcons::DATA_G[inetCondition];
                    mContentDescriptionDataType = "";
                    break;
                }
                else {
                    // fall through
                }
            case ITelephonyManager::NETWORK_TYPE_EDGE:
                if (!mShowAtLeastThreeGees) {
                    mDataIconList = TelephonyIcons::DATA_E[inetCondition];
                    mDataTypeIconId = showDataTypeIcon ?
                            R::drawable::stat_sys_data_fully_connected_e : 0;
                    mQSDataTypeIconId = TelephonyIcons::QS_DATA_E[inetCondition];
                    mContext->GetString(R::string::accessibility_data_connection_edge, &mContentDescriptionDataType);
                    break;
                }
                else {
                    // fall through
                }
            case ITelephonyManager::NETWORK_TYPE_UMTS:
                mDataIconList = TelephonyIcons::DATA_3G[inetCondition];
                mDataTypeIconId = showDataTypeIcon ?
                            R::drawable::stat_sys_data_fully_connected_3g : 0;
                mQSDataTypeIconId = TelephonyIcons::QS_DATA_3G[inetCondition];
                mContext->GetString(R::string::accessibility_data_connection_3g, &mContentDescriptionDataType);
                break;
            case ITelephonyManager::NETWORK_TYPE_HSDPA:
            case ITelephonyManager::NETWORK_TYPE_HSUPA:
            case ITelephonyManager::NETWORK_TYPE_HSPA:
            case ITelephonyManager::NETWORK_TYPE_HSPAP:
                if (mHspaDataDistinguishable) {
                    mDataIconList = TelephonyIcons::DATA_H[inetCondition];
                    mDataTypeIconId = showDataTypeIcon ?
                            R::drawable::stat_sys_data_fully_connected_h : 0;
                    mQSDataTypeIconId = TelephonyIcons::QS_DATA_H[inetCondition];
                    mContext->GetString(R::string::accessibility_data_connection_3_5g, &mContentDescriptionDataType);
                }
                else {
                    mDataIconList = TelephonyIcons::DATA_3G[inetCondition];
                    mDataTypeIconId = showDataTypeIcon ?
                            R::drawable::stat_sys_data_fully_connected_3g : 0;
                    mQSDataTypeIconId = TelephonyIcons::QS_DATA_3G[inetCondition];
                    mContext->GetString(R::string::accessibility_data_connection_3g, &mContentDescriptionDataType);
                }
                break;
            case ITelephonyManager::NETWORK_TYPE_CDMA:
                if (!mShowAtLeastThreeGees) {
                    // display 1xRTT for IS95A/B
                    mDataIconList = TelephonyIcons::DATA_1X[inetCondition];
                    mDataTypeIconId = showDataTypeIcon ?
                            R::drawable::stat_sys_data_fully_connected_1x : 0;
                    mQSDataTypeIconId = TelephonyIcons::QS_DATA_1X[inetCondition];
                    mContext->GetString(R::string::accessibility_data_connection_cdma, &mContentDescriptionDataType);
                    break;
                }
                else {
                    // fall through
                }
            case ITelephonyManager::NETWORK_TYPE_1xRTT:
                if (!mShowAtLeastThreeGees) {
                    mDataIconList = TelephonyIcons::DATA_1X[inetCondition];
                    mDataTypeIconId = showDataTypeIcon ?
                            R::drawable::stat_sys_data_fully_connected_1x : 0;
                    mQSDataTypeIconId = TelephonyIcons::QS_DATA_1X[inetCondition];
                    mContext->GetString(R::string::accessibility_data_connection_cdma, &mContentDescriptionDataType);
                    break;
                }
                else {
                    // fall through
                }
            case ITelephonyManager::NETWORK_TYPE_EVDO_0: //fall through
            case ITelephonyManager::NETWORK_TYPE_EVDO_A:
            case ITelephonyManager::NETWORK_TYPE_EVDO_B:
            case ITelephonyManager::NETWORK_TYPE_EHRPD:
                mDataIconList = TelephonyIcons::DATA_3G[inetCondition];
                mDataTypeIconId = showDataTypeIcon ?
                            R::drawable::stat_sys_data_fully_connected_3g : 0;
                mQSDataTypeIconId = TelephonyIcons::QS_DATA_3G[inetCondition];
                mContext->GetString(R::string::accessibility_data_connection_3g, &mContentDescriptionDataType);
                break;
            case ITelephonyManager::NETWORK_TYPE_LTE: {
                AutoPtr<IResources> res;
                mContext->GetResources((IResources**)&res);
                Boolean show4GforLTE = FALSE;
                res->GetBoolean(R::bool_::config_show4GForLTE, &show4GforLTE);
                if (show4GforLTE) {
                    mDataIconList = TelephonyIcons::DATA_4G[inetCondition];
                    mDataTypeIconId = showDataTypeIcon ?
                            R::drawable::stat_sys_data_fully_connected_4g : 0;
                    mQSDataTypeIconId = TelephonyIcons::QS_DATA_4G[inetCondition];
                    mContext->GetString(R::string::accessibility_data_connection_4g, &mContentDescriptionDataType);
                }
                else {
                    mDataIconList = TelephonyIcons::DATA_LTE[inetCondition];
                    mDataTypeIconId = showDataTypeIcon ? TelephonyIcons::ICON_LTE : 0;
                    mQSDataTypeIconId = TelephonyIcons::QS_DATA_LTE[inetCondition];
                    mContext->GetString(R::string::accessibility_data_connection_lte, &mContentDescriptionDataType);
                }
                break;
            }
            default:
                if (!mShowAtLeastThreeGees) {
                    mDataIconList = TelephonyIcons::DATA_G[inetCondition];
                    mDataTypeIconId = showDataTypeIcon ?
                            R::drawable::stat_sys_data_fully_connected_g : 0;
                    mQSDataTypeIconId = TelephonyIcons::QS_DATA_G[inetCondition];
                    mContext->GetString(R::string::accessibility_data_connection_gprs, &mContentDescriptionDataType);
                }
                else {
                    mDataIconList = TelephonyIcons::DATA_3G[inetCondition];
                    mDataTypeIconId = showDataTypeIcon ?
                            R::drawable::stat_sys_data_fully_connected_3g : 0;
                    mQSDataTypeIconId = TelephonyIcons::QS_DATA_3G[inetCondition];
                    mContext->GetString(R::string::accessibility_data_connection_3g, &mContentDescriptionDataType);
                }
                break;
        }
    }

    if (IsRoaming()) {
        mDataTypeIconId = TelephonyIcons::ROAMING_ICON;
        mQSDataTypeIconId = TelephonyIcons::QS_DATA_R[mInetCondition];
    }
}

Boolean NetworkControllerImpl::IsCdmaEri()
{
    if (mServiceState != NULL) {
        Int32 iconIndex = 0;
        mServiceState->GetCdmaEriIconIndex(&iconIndex);
        assert(0 && "TODO");
        // if (iconIndex != EriInfo.ROAMING_INDICATOR_OFF) {
        //     Int32 iconMode = 0;
        //     mServiceState->GetCdmaEriIconMode(&iconMode);
        //     if (iconMode == EriInfo.ROAMING_ICON_MODE_NORMAL
        //             || iconMode == EriInfo.ROAMING_ICON_MODE_FLASH) {
        //         return TRUE;
        //     }
        // }
    }
    return FALSE;
}

Boolean NetworkControllerImpl::IsRoaming()
{
    if (IsCdma()) {
        return IsCdmaEri();
    }
    Boolean tmp = FALSE;
    return mServiceState != NULL && (mServiceState->GetRoaming(&tmp), tmp);
}

void NetworkControllerImpl::UpdateDataIcon()
{
    Int32 iconId;
    Boolean visible = TRUE;

    if (!IsCdma()) {
        // GSM case, we have to check also the sim state
        if (mSimState == IccCardConstantsState_READY ||
                mSimState == IccCardConstantsState_UNKNOWN) {
            mNoSim = FALSE;
            if (HasService() && mDataState == ITelephonyManager::DATA_CONNECTED) {
                switch (mDataActivity) {
                    case ITelephonyManager::DATA_ACTIVITY_IN:
                        iconId = mDataIconList[1];
                        break;
                    case ITelephonyManager::DATA_ACTIVITY_OUT:
                        iconId = mDataIconList[2];
                        break;
                    case ITelephonyManager::DATA_ACTIVITY_INOUT:
                        iconId = mDataIconList[3];
                        break;
                    default:
                        iconId = mDataIconList[0];
                        break;
                }
                mDataDirectionIconId = iconId;
            }
            else {
                iconId = 0;
                visible = FALSE;
            }
        }
        else {
            iconId = 0;
            mNoSim = TRUE;
            visible = FALSE; // no SIM? no data
        }
    }
    else {
        // CDMA case, mDataActivity can be also DATA_ACTIVITY_DORMANT
        if (HasService() && mDataState == ITelephonyManager::DATA_CONNECTED) {
            switch (mDataActivity) {
                case ITelephonyManager::DATA_ACTIVITY_IN:
                    iconId = mDataIconList[1];
                    break;
                case ITelephonyManager::DATA_ACTIVITY_OUT:
                    iconId = mDataIconList[2];
                    break;
                case ITelephonyManager::DATA_ACTIVITY_INOUT:
                    iconId = mDataIconList[3];
                    break;
                case ITelephonyManager::DATA_ACTIVITY_DORMANT:
                default:
                    iconId = mDataIconList[0];
                    break;
            }
        }
        else {
            iconId = 0;
            visible = FALSE;
        }
    }

    mDataDirectionIconId = iconId;
    mDataConnected = visible;
}

void NetworkControllerImpl::UpdateNetworkName(
    /* [in] */ Boolean showSpn,
    /* [in] */ const String& spn,
    /* [in] */ Boolean showPlmn,
    /* [in] */ const String& plmn)
{
    if (FALSE) {
        Logger::D("CarrierLabel", "updateNetworkName showSpn=%d spn=%s showPlmn=%d plmn=%s", showSpn, spn.string()
                , showPlmn, plmn.string());
    }
    StringBuilder str;
    Boolean something = FALSE;
    if (showPlmn && plmn != NULL) {
        str.Append(plmn);
        something = TRUE;
    }
    if (showSpn && spn != NULL) {
        if (something) {
            str.Append(mNetworkNameSeparator);
        }
        str.Append(spn);
        something = TRUE;
    }
    if (something) {
        str.ToString(&mNetworkName);
    }
    else {
        mNetworkName = mNetworkNameDefault;
    }
}

void NetworkControllerImpl::UpdateWifiState(
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IWifiManager::WIFI_STATE_CHANGED_ACTION)) {
        Int32 v = 0;
        intent->GetInt32Extra(IWifiManager::EXTRA_WIFI_STATE, IWifiManager::WIFI_STATE_UNKNOWN, &v);
        mWifiEnabled = v == IWifiManager::WIFI_STATE_ENABLED;
    }
    else if (action.Equals(IWifiManager::NETWORK_STATE_CHANGED_ACTION)) {
        AutoPtr<IParcelable> p;
        intent->GetParcelableExtra(IWifiManager::EXTRA_NETWORK_INFO, (IParcelable**)&p);
        AutoPtr<INetworkInfo> networkInfo = INetworkInfo::Probe(p);
        Boolean wasConnected = mWifiConnected, tmp = FALSE;
        mWifiConnected = networkInfo != NULL && (networkInfo->IsConnected(&tmp), tmp);
        // If Connected grab the signal strength and ssid
        if (mWifiConnected) {
            // try getting it out of the intent first
            p = NULL;
            intent->GetParcelableExtra(IWifiManager::EXTRA_WIFI_INFO, (IParcelable**)&p);
            AutoPtr<IWifiInfo> info = IWifiInfo::Probe(p);
            if (info == NULL) {
                if (mWifiManager != NULL) {
                    mWifiManager->GetConnectionInfo((IWifiInfo**)&info);
                }
            }
            if (info != NULL) {
                mWifiSsid = HuntForSsid(info);
            }
            else {
                mWifiSsid = NULL;
            }
        }
        else if (!mWifiConnected) {
            mWifiSsid = NULL;
        }
    }
    else if (action.Equals(IWifiManager::RSSI_CHANGED_ACTION)) {
        intent->GetInt32Extra(IWifiManager::EXTRA_NEW_RSSI, -200, &mWifiRssi);

        AutoPtr<IWifiManagerHelper> helper;
        CWifiManagerHelper::AcquireSingleton((IWifiManagerHelper**)&helper);
        helper->CalculateSignalLevel(mWifiRssi, WifiIcons::WIFI_LEVEL_COUNT, &mWifiLevel);
    }

    UpdateWifiIcons();
}

void NetworkControllerImpl::UpdateWifiIcons()
{
    Int32 inetCondition = InetConditionForNetwork(IConnectivityManager::TYPE_WIFI);
    if (mWifiConnected) {
        mWifiIconId = WifiIcons::WIFI_SIGNAL_STRENGTH[inetCondition][mWifiLevel];
        mQSWifiIconId = WifiIcons::QS_WIFI_SIGNAL_STRENGTH[inetCondition][mWifiLevel];
        mContext->GetString(AccessibilityContentDescriptions::WIFI_CONNECTION_STRENGTH[mWifiLevel],
                &mContentDescriptionWifi);
    }
    else {
        if (mDataAndWifiStacked) {
            mWifiIconId = 0;
            mQSWifiIconId = 0;
        }
        else {
            mWifiIconId = mWifiEnabled ? R::drawable::stat_sys_wifi_signal_null : 0;
            mQSWifiIconId = mWifiEnabled ? R::drawable::ic_qs_wifi_no_network : 0;
        }
        mContext->GetString(R::string::accessibility_no_wifi, &mContentDescriptionWifi);
    }
}

String NetworkControllerImpl::HuntForSsid(
    /* [in] */ IWifiInfo* info)
{
    String ssid;
    info->GetSSID(&ssid);
    if (ssid != NULL) {
        return ssid;
    }
    // OK, it's not in the connectionInfo; we have to go hunting for it
    AutoPtr<IList> networks;  /*<WifiConfiguration*/
    if (mWifiManager != NULL) {
        mWifiManager->GetConfiguredNetworks((IList**)&networks);

        AutoPtr<IIterator> ator;
        networks->GetIterator((IIterator**)&ator);
        Boolean has = FALSE;
        while (ator->HasNext(&has), has) {
            AutoPtr<IInterface> obj;
            ator->GetNext((IInterface**)&obj);
            AutoPtr<IWifiConfiguration> net = IWifiConfiguration::Probe(obj);
            Int32 networkId = 0, id = 0;
            net->GetNetworkId(&networkId);
            if (networkId == (info->GetNetworkId(&id), id)) {
                net->GetSSID(&ssid);
                return ssid;
            }
        }
    }

    return String(NULL);
}


// ===== Wimax ===================================================================
void NetworkControllerImpl::UpdateWimaxState(
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Boolean wasConnected = mWimaxConnected;
    if (action.Equals(IWimaxManagerConstants::NET_4G_STATE_CHANGED_ACTION)) {
        Int32 wimaxStatus = 0;
        intent->GetInt32Extra(IWimaxManagerConstants::EXTRA_4G_STATE,
                IWimaxManagerConstants::NET_4G_STATE_UNKNOWN, &wimaxStatus);
        mIsWimaxEnabled = (wimaxStatus ==
                IWimaxManagerConstants::NET_4G_STATE_ENABLED);
    }
    else if (action.Equals(IWimaxManagerConstants::SIGNAL_LEVEL_CHANGED_ACTION)) {
        intent->GetInt32Extra(IWimaxManagerConstants::EXTRA_NEW_SIGNAL_LEVEL, 0, &mWimaxSignal);
    }
    else if (action.Equals(IWimaxManagerConstants::WIMAX_NETWORK_STATE_CHANGED_ACTION)) {
        intent->GetInt32Extra(IWimaxManagerConstants::EXTRA_WIMAX_STATE,
                IWimaxManagerConstants::NET_4G_STATE_UNKNOWN, &mWimaxState);
        intent->GetInt32Extra(IWimaxManagerConstants::EXTRA_WIMAX_STATE_DETAIL,
                IWimaxManagerConstants::NET_4G_STATE_UNKNOWN, &mWimaxExtraState);
        mWimaxConnected = (mWimaxState ==
                IWimaxManagerConstants::WIMAX_STATE_CONNECTED);
        mWimaxIdle = (mWimaxExtraState == IWimaxManagerConstants::WIMAX_IDLE);
    }
    UpdateDataNetType();
    UpdateWimaxIcons();
}

void NetworkControllerImpl::UpdateWimaxIcons()
{
    if (mIsWimaxEnabled) {
        if (mWimaxConnected) {
            Int32 inetCondition = InetConditionForNetwork(IConnectivityManager::TYPE_WIMAX);
            if (mWimaxIdle) {
                mWimaxIconId = WimaxIcons::WIMAX_IDLE;
            }
            else {
                mWimaxIconId = WimaxIcons::WIMAX_SIGNAL_STRENGTH[inetCondition][mWimaxSignal];
            }

            mContext->GetString(AccessibilityContentDescriptions::WIMAX_CONNECTION_STRENGTH[mWimaxSignal],
                    &mContentDescriptionWimax);
        }
        else {
            mWimaxIconId = WimaxIcons::WIMAX_DISCONNECTED;
            mContext->GetString(R::string::accessibility_no_wimax,
                    &mContentDescriptionWimax);
        }
    }
    else {
        mWimaxIconId = 0;
    }
}

// ===== Full or limited Internet connectivity ==================================
void NetworkControllerImpl::UpdateConnectivity(
    /* [in] */ IIntent* intent)
{
    if (CHATTY) {
        Logger::D(TAG, "updateConnectivity: intent=%p", intent);
    }

    Logger::D(TAG, "TODO [UpdateConnectivity] : CONNECTIVITY_SERVICE.");
    // AutoPtr<IInterface> obj;
    // mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    // AutoPtr<IConnectivityManager> connManager = IConnectivityManager::Probe(obj);

    // AutoPtr<INetworkInfo> info;
    // connManager->GetActiveNetworkInfo((INetworkInfo**)&info);

    // // Are we connected at all, by any interface?
    // Boolean tmp = FALSE;
    // mConnected = info != NULL && (info->IsConnected(&tmp), tmp);
    // if (mConnected) {
    //     info->GetType(&mConnectedNetworkType);
    //     info->GetTypeName(&mConnectedNetworkTypeName);
    // }
    // else {
    //     mConnectedNetworkType = IConnectivityManager::TYPE_NONE;
    //     mConnectedNetworkTypeName = NULL;
    // }

    // Int32 connectionStatus = 0;
    // intent->GetInt32Extra(IConnectivityManager::EXTRA_INET_CONDITION, 0, &connectionStatus);

    // if (CHATTY) {
    //     Logger::D(TAG, "updateConnectivity: networkInfo=%p", info.Get());
    //     Logger::D(TAG, "updateConnectivity: connectionStatus=%d", connectionStatus);
    // }

    // mInetCondition = (connectionStatus > INET_CONDITION_THRESHOLD ? 1 : 0);

    // Int32 type = 0;
    // if (info != NULL && (info->GetType(&type), type) == IConnectivityManager::TYPE_BLUETOOTH) {
    //     info->IsConnected(&mBluetoothTethered);
    // }
    // else {
    //     mBluetoothTethered = FALSE;
    // }

    // // We want to update all the icons, all at once, for any condition change
    UpdateDataNetType();
    UpdateWimaxIcons();
    UpdateDataIcon();
    UpdateTelephonySignalStrength();
    UpdateWifiIcons();
}


// ===== Update the views =======================================================
void NetworkControllerImpl::RefreshViews()
{
    AutoPtr<IContext> context = mContext;

    Int32 combinedSignalIconId = 0;
    String combinedLabel("");
    String wifiLabel("");
    String mobileLabel("");
    Int32 N = 0;
    Boolean emergencyOnly = FALSE;
    IsEmergencyOnly(&emergencyOnly);

    if (!mHasMobileDataFeature) {
        mDataSignalIconId = mPhoneSignalIconId = 0;
        mQSPhoneSignalIconId = 0;
        mobileLabel = "";
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
            }
            else {
                // Tablets, basically
                mobileLabel = "";
            }
        }
        else {
            context->GetString(R::string::status_bar_settings_signal_meter_disconnected, &mobileLabel);
        }

        // Now for things that should only be shown when actually using mobile data.
        if (mDataConnected) {
            combinedSignalIconId = mDataSignalIconId;

            combinedLabel = mobileLabel;
            combinedSignalIconId = mDataSignalIconId; // set by UpdateDataIcon()
            mContentDescriptionCombinedSignal = mContentDescriptionDataType;
        }
    }

    if (mWifiConnected) {
        if (mWifiSsid == NULL) {
            context->GetString(R::string::status_bar_settings_signal_meter_wifi_nossid, &wifiLabel);
        }
        else {
            wifiLabel = mWifiSsid;
            if (DEBUG) {
                wifiLabel += "xxxxXXXXxxxxXXXX";
            }
        }

        combinedLabel = wifiLabel;
        combinedSignalIconId = mWifiIconId; // set by UpdateWifiIcons()
        mContentDescriptionCombinedSignal = mContentDescriptionWifi;
    }
    else {
        if (mHasMobileDataFeature) {
            wifiLabel = "";
        }
        else {
            context->GetString(R::string::status_bar_settings_signal_meter_disconnected, &wifiLabel);
        }
    }

    if (mBluetoothTethered) {
        mContext->GetString(R::string::bluetooth_tethered, &combinedLabel);
        combinedSignalIconId = mBluetoothTetherIconId;
        mContext->GetString(R::string::accessibility_bluetooth_tether, &mContentDescriptionCombinedSignal);
    }

    const Boolean ethernetConnected = (mConnectedNetworkType == IConnectivityManager::TYPE_ETHERNET);
    if (ethernetConnected) {
        context->GetString(R::string::ethernet_label, &combinedLabel);
    }

    Boolean tmp = FALSE;
    if (mAirplaneMode &&
            (mServiceState == NULL || (!HasService() && (mServiceState->IsEmergencyOnly(&tmp), !tmp)))) {
        // Only display the flight-mode icon if not in "emergency calls only" mode.

        // look again; your radios are now airplanes
        mContext->GetString(R::string::accessibility_airplane_mode, &mContentDescriptionPhoneSignal);
        mAirplaneIconId = TelephonyIcons::FLIGHT_MODE_ICON;
        mPhoneSignalIconId = mDataSignalIconId = mDataTypeIconId = mQSDataTypeIconId = 0;
        mQSPhoneSignalIconId = 0;

        // combined values from connected wifi take precedence over airplane mode
        if (mWifiConnected) {
            // Suppress "No internet connection." from mobile if wifi connected.
            mobileLabel = "";
        }
        else {
            if (mHasMobileDataFeature) {
                // let the mobile icon show "No internet connection."
                wifiLabel = "";
            }
            else {
                context->GetString(R::string::status_bar_settings_signal_meter_disconnected, &wifiLabel);
                combinedLabel = wifiLabel;
            }
            mContentDescriptionCombinedSignal = mContentDescriptionPhoneSignal;
            combinedSignalIconId = mDataSignalIconId;
        }
    }
    else if (!mDataConnected && !mWifiConnected && !mBluetoothTethered && !mWimaxConnected && !ethernetConnected) {
        // pretty much totally disconnected

        context->GetString(R::string::status_bar_settings_signal_meter_disconnected, &combinedLabel);
        // On devices without mobile radios, we want to show the wifi icon
        combinedSignalIconId =
            mHasMobileDataFeature ? mDataSignalIconId : mWifiIconId;
        mContentDescriptionCombinedSignal = mHasMobileDataFeature
            ? mContentDescriptionDataType : mContentDescriptionWifi;

        Int32 inetCondition = InetConditionForNetwork(IConnectivityManager::TYPE_MOBILE);

        mDataTypeIconId = 0;
        mQSDataTypeIconId = 0;
        if (IsRoaming()) {
            mDataTypeIconId = TelephonyIcons::ROAMING_ICON;
            mQSDataTypeIconId = TelephonyIcons::QS_DATA_R[mInetCondition];
        }
    }

    if (mDemoMode) {
        mQSWifiIconId = mDemoWifiLevel < 0 ? R::drawable::ic_qs_wifi_no_network
                : WifiIcons::QS_WIFI_SIGNAL_STRENGTH[mDemoInetCondition][mDemoWifiLevel];
        mQSPhoneSignalIconId = mDemoMobileLevel < 0 ? R::drawable::ic_qs_signal_no_signal :
                TelephonyIcons::QS_TELEPHONY_SIGNAL_STRENGTH[mDemoInetCondition][mDemoMobileLevel];
        mQSDataTypeIconId = mDemoQSDataTypeIconId;
    }

    if (DEBUG) {
        assert(0 && "TODO");
        // Logger::D(TAG, "RefreshViews connected={"
        //         + (mWifiConnected?" wifi":"")
        //         + (mDataConnected?" data":"")
        //         + " } level="
        //         + ((mSignalStrength == NULL)?"??":Integer.toString(mSignalStrength.getLevel()))
        //         + " combinedSignalIconId=0x"
        //         + Integer.toHexString(combinedSignalIconId)
        //         + "/" + GetResourceName(combinedSignalIconId)
        //         + " mobileLabel=" + mobileLabel
        //         + " wifiLabel=" + wifiLabel
        //         + " emergencyOnly=" + emergencyOnly
        //         + " combinedLabel=" + combinedLabel
        //         + " mAirplaneMode=" + mAirplaneMode
        //         + " mDataActivity=" + mDataActivity
        //         + " mPhoneSignalIconId=0x" + Integer.toHexString(mPhoneSignalIconId)
        //         + " mQSPhoneSignalIconId=0x" + Integer.toHexString(mQSPhoneSignalIconId)
        //         + " mDataDirectionIconId=0x" + Integer.toHexString(mDataDirectionIconId)
        //         + " mDataSignalIconId=0x" + Integer.toHexString(mDataSignalIconId)
        //         + " mDataTypeIconId=0x" + Integer.toHexString(mDataTypeIconId)
        //         + " mQSDataTypeIconId=0x" + Integer.toHexString(mQSDataTypeIconId)
        //         + " mWifiIconId=0x" + Integer.toHexString(mWifiIconId)
        //         + " mQSWifiIconId=0x" + Integer.toHexString(mQSWifiIconId)
        //         + " mBluetoothTetherIconId=0x" + Integer.toHexString(mBluetoothTetherIconId));
    }

    // update QS
    AutoPtr<IIterator> ator;
    mSignalsChangedCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> cb;
        ator->GetNext((IInterface**)&cb);
        NotifySignalsChangedCallbacks(INetworkSignalChangedCallback::Probe(cb));
    }

    if (mLastPhoneSignalIconId          != mPhoneSignalIconId
         || mLastWifiIconId                 != mWifiIconId
         || mLastInetCondition              != mInetCondition
         || mLastWimaxIconId                != mWimaxIconId
         || mLastDataTypeIconId             != mDataTypeIconId
         || mLastAirplaneMode               != mAirplaneMode
         || mLastLocale                     != mLocale
         || mLastConnectedNetworkType       != mConnectedNetworkType)
    {
        // NB: the mLast*s will be updated later
        ator = NULL;
        mSignalClusters->GetIterator((IIterator**)&ator);
        while (ator->HasNext(&has), has) {
            AutoPtr<IInterface> cluster;
            ator->GetNext((IInterface**)&cluster);
            RefreshSignalCluster(INetworkControllerImplSignalCluster::Probe(cluster));
        }
    }

    if (mLastAirplaneMode != mAirplaneMode) {
        mLastAirplaneMode = mAirplaneMode;
    }

    if (mLastLocale != mLocale) {
        mLastLocale = mLocale;
    }

    // the phone icon on phones
    if (mLastPhoneSignalIconId != mPhoneSignalIconId) {
        mLastPhoneSignalIconId = mPhoneSignalIconId;
    }

    // the data icon on phones
    if (mLastDataDirectionIconId != mDataDirectionIconId) {
        mLastDataDirectionIconId = mDataDirectionIconId;
    }

    // the wifi icon on phones
    if (mLastWifiIconId != mWifiIconId) {
        mLastWifiIconId = mWifiIconId;
    }

    if (mLastInetCondition != mInetCondition) {
        mLastInetCondition = mInetCondition;
    }

    if (mLastConnectedNetworkType != mConnectedNetworkType) {
        mLastConnectedNetworkType = mConnectedNetworkType;
    }

    // the wimax icon on phones
    if (mLastWimaxIconId != mWimaxIconId) {
        mLastWimaxIconId = mWimaxIconId;
    }
    // the combined data signal icon
    if (mLastCombinedSignalIconId != combinedSignalIconId) {
        mLastCombinedSignalIconId = combinedSignalIconId;
    }

    // the data network type overlay
    if (mLastDataTypeIconId != mDataTypeIconId) {
        mLastDataTypeIconId = mDataTypeIconId;
    }

    // the combinedLabel in the notification panel
    if (!mLastCombinedLabel.Equals(combinedLabel)) {
        mLastCombinedLabel = combinedLabel;
        mCombinedLabelViews->GetSize(&N);
        for (Int32 i = 0; i < N; i ++) {
            AutoPtr<IInterface> o;
            mCombinedLabelViews->Get(i, (IInterface**)&o);
            AutoPtr<ITextView> v = ITextView::Probe(o);
            AutoPtr<ICharSequence> cs;
            CString::New(combinedLabel, (ICharSequence**)&cs);
            v->SetText(cs);
        }
    }

    // wifi label
    mWifiLabelViews->GetSize(&N);
    for (Int32 i = 0; i < N; i ++) {
        AutoPtr<IInterface> o;
        mWifiLabelViews->Get(i, (IInterface**)&o);
        AutoPtr<ITextView> v = ITextView::Probe(o);

        AutoPtr<ICharSequence> cs;
        CString::New(wifiLabel, (ICharSequence**)&cs);
        v->SetText(cs);
        if (wifiLabel.Equals("")) {
            IView::Probe(v)->SetVisibility(IView::GONE);
        }
        else {
            IView::Probe(v)->SetVisibility(IView::VISIBLE);
        }
    }

    // mobile label
    mMobileLabelViews->GetSize(&N);
    for (Int32 i = 0; i < N; i ++) {
        AutoPtr<IInterface> o;
        mMobileLabelViews->Get(i, (IInterface**)&o);
        AutoPtr<ITextView> v = ITextView::Probe(o);

        AutoPtr<ICharSequence> cs;
        CString::New(mobileLabel, (ICharSequence**)&cs);
        v->SetText(cs);
        if (mobileLabel.Equals("")) {
            IView::Probe(v)->SetVisibility(IView::GONE);
        }
        else {
            IView::Probe(v)->SetVisibility(IView::VISIBLE);
        }
    }

    // e-call label
    mEmergencyViews->GetSize(&N);
    for (Int32 i = 0; i < N; i ++) {
        AutoPtr<IInterface> o;
        mEmergencyViews->Get(i, (IInterface**)&o);
        AutoPtr<IStatusBarHeaderView> v = IStatusBarHeaderView::Probe(o);
        v->SetShowEmergencyCallsOnly(emergencyOnly);
    }
}

ECode NetworkControllerImpl::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("NetworkController state:"));
    String format;
    format.AppendFormat("  %s network type %d (%s)",
            mConnected? "CONNECTED" : "DISCONNECTED",
            mConnectedNetworkType, mConnectedNetworkTypeName.string());
    pw->Println(format);
    pw->Println(String("  - telephony ------"));
    pw->Print(String("  hasVoiceCallingFeature()="));

    Boolean tmp = FALSE;
    pw->Println((HasVoiceCallingFeature(&tmp), tmp));
    pw->Print(String("  HasService()="));
    pw->Println(HasService());
    pw->Print(String("  mHspaDataDistinguishable="));
    pw->Println(mHspaDataDistinguishable);
    pw->Print(String("  mDataConnected="));
    pw->Println(mDataConnected);
    pw->Print(String("  mSimState="));
    pw->Println(mSimState);
    pw->Print(String("  mPhoneState="));
    pw->Println(mPhoneState);
    pw->Print(String("  mDataState="));
    pw->Println(mDataState);
    pw->Print(String("  mDataActivity="));
    pw->Println(mDataActivity);
    pw->Print(String("  mDataNetType="));
    pw->Print(mDataNetType);
    pw->Print(String("/"));

    String value;
    AutoPtr<ITelephonyManagerHelper> helper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&helper);
    helper->GetNetworkTypeName(mDataNetType, &value);
    pw->Println(value);
    pw->Print(String("  mServiceState="));
    pw->Println(mServiceState);
    pw->Print(String("  mSignalStrength="));
    pw->Println(mSignalStrength);
    pw->Print(String("  mLastSignalLevel="));
    pw->Println(mLastSignalLevel);
    pw->Print(String("  mNetworkName="));
    pw->Println(mNetworkName);
    pw->Print(String("  mNetworkNameDefault="));
    pw->Println(mNetworkNameDefault);
    pw->Print(String("  mNetworkNameSeparator="));

    StringUtils::Replace(mNetworkNameSeparator, "\n","\\n", &value);
    pw->Println(value);
    pw->Print(String("  mPhoneSignalIconId=0x"));
    pw->Print(StringUtils::ToHexString(mPhoneSignalIconId));
    pw->Print(String("/"));
    pw->Print(String("  mQSPhoneSignalIconId=0x"));
    pw->Print(StringUtils::ToHexString(mQSPhoneSignalIconId));
    pw->Print(String("/"));
    pw->Println(GetResourceName(mPhoneSignalIconId));
    pw->Print(String("  mDataDirectionIconId="));
    pw->Print(StringUtils::ToHexString(mDataDirectionIconId));
    pw->Print(String("/"));
    pw->Println(GetResourceName(mDataDirectionIconId));
    pw->Print(String("  mDataSignalIconId="));
    pw->Print(StringUtils::ToHexString(mDataSignalIconId));
    pw->Print(String("/"));
    pw->Println(GetResourceName(mDataSignalIconId));
    pw->Print(String("  mDataTypeIconId="));
    pw->Print(StringUtils::ToHexString(mDataTypeIconId));
    pw->Print(String("/"));
    pw->Println(GetResourceName(mDataTypeIconId));
    pw->Print(String("  mQSDataTypeIconId="));
    pw->Print(StringUtils::ToHexString(mQSDataTypeIconId));
    pw->Print(String("/"));
    pw->Println(GetResourceName(mQSDataTypeIconId));

    pw->Println(String("  - wifi ------"));
    pw->Print(String("  mWifiEnabled="));
    pw->Println(mWifiEnabled);
    pw->Print(String("  mWifiConnected="));
    pw->Println(mWifiConnected);
    pw->Print(String("  mWifiRssi="));
    pw->Println(mWifiRssi);
    pw->Print(String("  mWifiLevel="));
    pw->Println(mWifiLevel);
    pw->Print(String("  mWifiSsid="));
    pw->Println(mWifiSsid);

    format = NULL;
    format.AppendFormat("  mWifiIconId=0x%08x/%s",
                mWifiIconId, GetResourceName(mWifiIconId).string());
    pw->Println(format);

    format = NULL;
    format.AppendFormat("  mQSWifiIconId=0x%08x/%s",
                mQSWifiIconId, GetResourceName(mQSWifiIconId).string());
    pw->Println(format);
    pw->Print(String("  mWifiActivity="));
    pw->Println(mWifiActivity);

    if (mWimaxSupported) {
        pw->Println(String("  - wimax ------"));
        pw->Print(String("  mIsWimaxEnabled="));
        pw->Println(mIsWimaxEnabled);
        pw->Print(String("  mWimaxConnected="));
        pw->Println(mWimaxConnected);
        pw->Print(String("  mWimaxIdle="));
        pw->Println(mWimaxIdle);

        format = NULL;
        format.AppendFormat("  mWimaxIconId=0x%08x/%s",
                    mWimaxIconId, GetResourceName(mWimaxIconId).string());
        pw->Println(format);
        format = NULL;
        format.AppendFormat("  mWimaxSignal=%d", mWimaxSignal);
        pw->Println(format);
        format = NULL;
        format.AppendFormat("  mWimaxState=%d", mWimaxState);
        pw->Println(format);
        format = NULL;
        format.AppendFormat("  mWimaxExtraState=%d", mWimaxExtraState);
        pw->Println(format);
    }

    pw->Println(String("  - Bluetooth ----"));
    pw->Print(String("  mBtReverseTethered="));
    pw->Println(mBluetoothTethered);

    pw->Println(String("  - connectivity ------"));
    pw->Print(String("  mInetCondition="));
    pw->Println(mInetCondition);

    pw->Println(String("  - icons ------"));
    pw->Print(String("  mLastPhoneSignalIconId=0x"));
    pw->Print(StringUtils::ToHexString(mLastPhoneSignalIconId));
    pw->Print(String("/"));
    pw->Println(GetResourceName(mLastPhoneSignalIconId));
    pw->Print(String("  mLastDataDirectionIconId=0x"));
    pw->Print(StringUtils::ToHexString(mLastDataDirectionIconId));
    pw->Print(String("/"));
    pw->Println(GetResourceName(mLastDataDirectionIconId));
    pw->Print(String("  mLastWifiIconId=0x"));
    pw->Print(StringUtils::ToHexString(mLastWifiIconId));
    pw->Print(String("/"));
    pw->Println(GetResourceName(mLastWifiIconId));
    pw->Print(String("  mLastCombinedSignalIconId=0x"));
    pw->Print(StringUtils::ToHexString(mLastCombinedSignalIconId));
    pw->Print(String("/"));
    pw->Println(GetResourceName(mLastCombinedSignalIconId));
    pw->Print(String("  mLastDataTypeIconId=0x"));
    pw->Print(StringUtils::ToHexString(mLastDataTypeIconId));
    pw->Print(String("/"));
    pw->Println(GetResourceName(mLastDataTypeIconId));
    pw->Print(String("  mLastCombinedLabel="));
    pw->Print(mLastCombinedLabel);
    pw->Println(String(""));
    return NOERROR;
}

String NetworkControllerImpl::GetResourceName(
    /* [in] */ Int32 resId)
{
    if (resId != 0) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        String value;
        if (FAILED(res->GetResourceName(resId, &value))) {
            return String("unknown");
        }
        return value;
        // try {
        //     return res->GetResourceName(resId);
        // } catch (android.content.res.Resources.NotFoundException ex) {
        //     return "(unknown)";
        // }
    }
    return String("(NULL)");
}

ECode NetworkControllerImpl::DispatchDemoCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle* args)
{
    if (!mDemoMode && command.Equals(COMMAND_ENTER)) {
        mDemoMode = TRUE;
        mDemoWifiLevel = mWifiLevel;
        mDemoInetCondition = mInetCondition;
        mDemoDataTypeIconId = mDataTypeIconId;
        mDemoQSDataTypeIconId = mQSDataTypeIconId;
        mDemoMobileLevel = mLastSignalLevel;
    }
    else if (mDemoMode && command.Equals(COMMAND_EXIT)) {
        mDemoMode = FALSE;
        AutoPtr<IIterator> ator;
        mSignalClusters->GetIterator((IIterator**)&ator);
        Boolean has = FALSE;
        while (ator->HasNext(&has), has) {
            AutoPtr<IInterface> cluster;
            ator->GetNext((IInterface**)&cluster);
            RefreshSignalCluster(INetworkControllerImplSignalCluster::Probe(cluster));
        }
        RefreshViews();
    }
    else if (mDemoMode && command.Equals(COMMAND_NETWORK)) {
        String airplane;
        args->GetString(String("airplane"), &airplane);
        if (airplane != NULL) {
            Boolean show = airplane.Equals("show");

            AutoPtr<IIterator> ator;
            mSignalClusters->GetIterator((IIterator**)&ator);
            Boolean has = FALSE;
            while (ator->HasNext(&has), has) {
                AutoPtr<IInterface> cluster;
                ator->GetNext((IInterface**)&cluster);
                INetworkControllerImplSignalCluster::Probe(cluster)->SetIsAirplaneMode(show, TelephonyIcons::FLIGHT_MODE_ICON);
            }
        }
        String fully;
        args->GetString(String("fully"), &fully);
        if (fully != NULL) {
            mDemoInetCondition = StringUtils::ParseBoolean(fully) ? 1 : 0;
        }
        String wifi;
        args->GetString(String("wifi"), &wifi);
        if (wifi != NULL) {
            Boolean show = wifi.Equals("show");
            String level;
            args->GetString(String("level"), &level);
            if (level != NULL) {
                mDemoWifiLevel = level.Equals("NULL") ? -1
                        : Elastos::Core::Math::Min(StringUtils::ParseInt32(level), WifiIcons::WIFI_LEVEL_COUNT - 1);
            }
            Int32 iconId = mDemoWifiLevel < 0 ? R::drawable::stat_sys_wifi_signal_null
                    : WifiIcons::WIFI_SIGNAL_STRENGTH[mDemoInetCondition][mDemoWifiLevel];

            AutoPtr<IIterator> ator;
            mSignalClusters->GetIterator((IIterator**)&ator);
            Boolean has = FALSE;
            while (ator->HasNext(&has), has) {
                AutoPtr<IInterface> cluster;
                ator->GetNext((IInterface**)&cluster);
                INetworkControllerImplSignalCluster::Probe(cluster)->SetWifiIndicators(show, iconId, String("Demo"));
            }

            RefreshViews();
        }
        String mobile;
        args->GetString(String("mobile"), &mobile);
        if (mobile != NULL) {
            Boolean show = mobile.Equals("show");
            String datatype;
            args->GetString(String("datatype"), &datatype);
            if (datatype != NULL) {
                mDemoDataTypeIconId =
                        datatype.Equals("1x") ? TelephonyIcons::ICON_1X :
                        datatype.Equals("3g") ? TelephonyIcons::ICON_3G :
                        datatype.Equals("4g") ? TelephonyIcons::ICON_4G :
                        datatype.Equals("e") ? R::drawable::stat_sys_data_fully_connected_e :
                        datatype.Equals("g") ? R::drawable::stat_sys_data_fully_connected_g :
                        datatype.Equals("h") ? R::drawable::stat_sys_data_fully_connected_h :
                        datatype.Equals("lte") ? TelephonyIcons::ICON_LTE :
                        datatype.Equals("roam") ? TelephonyIcons::ROAMING_ICON :
                        0;
                mDemoQSDataTypeIconId =
                        datatype.Equals("1x") ? TelephonyIcons::QS_ICON_1X :
                        datatype.Equals("3g") ? TelephonyIcons::QS_ICON_3G :
                        datatype.Equals("4g") ? TelephonyIcons::QS_ICON_4G :
                        datatype.Equals("e") ? R::drawable::ic_qs_signal_e :
                        datatype.Equals("g") ? R::drawable::ic_qs_signal_g :
                        datatype.Equals("h") ? R::drawable::ic_qs_signal_h :
                        datatype.Equals("lte") ? TelephonyIcons::QS_ICON_LTE :
                        datatype.Equals("roam") ? R::drawable::ic_qs_signal_r :
                        0;
            }
            // Int32[][] icons = TelephonyIcons::TELEPHONY_SIGNAL_STRENGTH;
            String level;
            args->GetString(String("level"), &level);
            if (level != NULL) {
                mDemoMobileLevel = level.Equals("NULL") ? -1
                        : Elastos::Core::Math::Min(StringUtils::ParseInt32(level), /*icons[0].length*/5 - 1);
            }
            Int32 iconId = mDemoMobileLevel < 0 ? R::drawable::stat_sys_signal_null :
                    TelephonyIcons::TELEPHONY_SIGNAL_STRENGTH/*icons*/[mDemoInetCondition][mDemoMobileLevel];

            AutoPtr<IIterator> ator;
            mSignalClusters->GetIterator((IIterator**)&ator);
            Boolean has = FALSE;
            while (ator->HasNext(&has), has) {
                AutoPtr<IInterface> cluster;
                ator->GetNext((IInterface**)&cluster);
                INetworkControllerImplSignalCluster::Probe(cluster)->SetMobileDataIndicators(
                        show,
                        iconId,
                        mDemoDataTypeIconId,
                        String("Demo"),
                        String("Demo"),
                        mDemoDataTypeIconId == TelephonyIcons::ROAMING_ICON,
                        IsTypeIconWide(mDemoDataTypeIconId));
            }
            RefreshViews();
        }
    }
    return NOERROR;
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
