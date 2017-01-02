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

#include "Elastos.Droid.Bluetooth.h"
#include "elastos/droid/settings/deviceinfo/CStatus.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/text/TextUtils.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothAdapterHelper;
using Elastos::Droid::Bluetooth::CBluetoothAdapterHelper;
using Elastos::Droid::Content::IClipboardManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::CPhoneFactory;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::CPhoneStateIntentReceiver;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Preference::IPreferenceActivity;
using Elastos::Droid::Preference::EIID_IPreferenceActivity;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Telephony::ICellBroadcastMessage;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::IPhoneStateListener;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemLongClickListener;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

const String CStatus::CB_AREA_INFO_RECEIVED_ACTION("android.cellbroadcastreceiver.CB_AREA_INFO_RECEIVED");

const String CStatus::GET_LATEST_CB_AREA_INFO_ACTION("android.cellbroadcastreceiver.GET_LATEST_CB_AREA_INFO");

const String CStatus::CB_AREA_INFO_SENDER_PERMISSION("android.permission.RECEIVE_EMERGENCY_BROADCAST");

const String CStatus::KEY_DATA_STATE("data_state");
const String CStatus::KEY_SERVICE_STATE("service_state");
const String CStatus::KEY_OPERATOR_NAME("operator_name");
const String CStatus::KEY_ROAMING_STATE("roaming_state");
const String CStatus::KEY_NETWORK_TYPE("network_type");
const String CStatus::KEY_LATEST_AREA_INFO("latest_area_info");
const String CStatus::KEY_PHONE_NUMBER("number");
const String CStatus::KEY_IMEI_SV("imei_sv");
const String CStatus::KEY_IMEI("imei");
const String CStatus::KEY_PRL_VERSION("prl_version");
const String CStatus::KEY_MIN_NUMBER("min_number");
const String CStatus::KEY_MEID_NUMBER("meid_number");
const String CStatus::KEY_SIGNAL_STRENGTH("signal_strength");
const String CStatus::KEY_BATTERY_STATUS("battery_status");
const String CStatus::KEY_BATTERY_LEVEL("battery_level");
const String CStatus::KEY_IP_ADDRESS("wifi_ip_address");
const String CStatus::KEY_WIFI_MAC_ADDRESS("wifi_mac_address");
const String CStatus::KEY_BT_ADDRESS("bt_address");
const String CStatus::KEY_SERIAL_NUMBER("serial_number");
const String CStatus::KEY_ICC_ID("icc_id");
const String CStatus::KEY_WIMAX_MAC_ADDRESS("wimax_mac_address");

AutoPtr< ArrayOf<String> > CStatus::InitPHONE_RELATED_ENTRIES()
{
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(14);
    (*args)[0] = CStatus::KEY_DATA_STATE;
    (*args)[1] = CStatus::KEY_SERVICE_STATE;
    (*args)[2] = CStatus::KEY_OPERATOR_NAME;
    (*args)[3] = CStatus::KEY_ROAMING_STATE;
    (*args)[4] = CStatus::KEY_NETWORK_TYPE;
    (*args)[5] = CStatus::KEY_LATEST_AREA_INFO;
    (*args)[6] = CStatus::KEY_PHONE_NUMBER;
    (*args)[7] = CStatus::KEY_IMEI;
    (*args)[8] = CStatus::KEY_IMEI_SV;
    (*args)[9] = CStatus::KEY_PRL_VERSION;
    (*args)[10] = CStatus::KEY_MIN_NUMBER;
    (*args)[11] = CStatus::KEY_MEID_NUMBER;
    (*args)[12] = CStatus::KEY_SIGNAL_STRENGTH;
    (*args)[13] = CStatus::KEY_ICC_ID;

    return args;
}

const AutoPtr< ArrayOf<String> > CStatus::PHONE_RELATED_ENTRIES = InitPHONE_RELATED_ENTRIES();

AutoPtr< ArrayOf<String> > InitCONNECTIVITY_INTENTS()
{
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(4);
    (*args)[0] = IBluetoothAdapter::ACTION_STATE_CHANGED;
    (*args)[1] = IConnectivityManager::CONNECTIVITY_ACTION_IMMEDIATE;
    (*args)[2] = IWifiManager::LINK_CONFIGURATION_CHANGED_ACTION;
    (*args)[3] = IWifiManager::NETWORK_STATE_CHANGED_ACTION;

    return args;
}

const AutoPtr< ArrayOf<String> > CStatus::CONNECTIVITY_INTENTS = InitCONNECTIVITY_INTENTS();

const Int32 CStatus::EVENT_SIGNAL_STRENGTH_CHANGED = 200;
const Int32 CStatus::EVENT_SERVICE_STATE_CHANGED = 300;

const Int32 CStatus::EVENT_UPDATE_STATS = 500;

const Int32 CStatus::EVENT_UPDATE_CONNECTIVITY = 600;

//===============================================================================
//                  CStatus::MyHandler
//===============================================================================

CStatus::MyHandler::MyHandler(
    /* [in] */ CStatus* activity)
{
    // mStatus = new WeakReference<CStatus>(activity);
    activity->GetWeakReference((IWeakReference**)&mStatus);
}

ECode CStatus::MyHandler::constructor()
{
    return Handler::constructor();
}

ECode CStatus::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // CStatus status = mStatus->Get();
    AutoPtr<IPreferenceActivity> obj;
    mStatus->Resolve(EIID_IPreferenceActivity, (IInterface**)&obj);
    CStatus* status = (CStatus*)obj.Get();

    if (status == NULL) {
        return NOERROR;
    }

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_SIGNAL_STRENGTH_CHANGED:
            status->UpdateSignalStrength();
            break;

        case EVENT_SERVICE_STATE_CHANGED: {
            AutoPtr<IServiceState> serviceState;
            status->mPhoneStateReceiver->GetServiceState((IServiceState**)&serviceState);
            status->UpdateServiceState(serviceState);
            break;
        }

        case EVENT_UPDATE_STATS: {
            status->UpdateTimes();
            Boolean res;
            SendEmptyMessageDelayed(EVENT_UPDATE_STATS, 1000, &res);
            break;
        }

        case EVENT_UPDATE_CONNECTIVITY:
            status->UpdateConnectivity();
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  CStatus::InitBatteryInfoReceiver
//===============================================================================

CStatus::InitBatteryInfoReceiver::InitBatteryInfoReceiver(
    /* [in] */ CStatus* host)
    : mHost(host)
{}

ECode CStatus::InitBatteryInfoReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_BATTERY_CHANGED.Equals(action)) {
        mHost->mBatteryLevel->SetSummary(CoreUtils::Convert(Utils::GetBatteryPercentage(intent)));
        AutoPtr<IResources> resources;
        mHost->GetResources((IResources**)&resources);
        mHost->mBatteryStatus->SetSummary(CoreUtils::Convert(Utils::GetBatteryStatus(resources, intent)));
    }
    return NOERROR;
}

//===============================================================================
//                  CStatus::InitPhoneStateListener
//===============================================================================

CStatus::InitPhoneStateListener::InitPhoneStateListener(
    /* [in] */ CStatus* host)
    : mHost(host)
{}

ECode CStatus::InitPhoneStateListener::OnDataConnectionStateChanged(
    /* [in] */ Int32 state)
{
    mHost->UpdateDataState();
    mHost->UpdateNetworkType();
    return NOERROR;
}

//===============================================================================
//                  CStatus::InitAreaInfoReceiver
//===============================================================================

CStatus::InitAreaInfoReceiver::InitAreaInfoReceiver(
    /* [in] */ CStatus* host)
    : mHost(host)
{}

ECode CStatus::InitAreaInfoReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (CB_AREA_INFO_RECEIVED_ACTION.Equals(action)) {
        AutoPtr<IBundle> extras;
        intent->GetExtras((IBundle**)&extras);
        if (extras == NULL) {
            return NOERROR;
        }

        AutoPtr<IParcelable> value;
        extras->GetParcelable(String("message"), (IParcelable**)&value);
        ICellBroadcastMessage* cbMessage = ICellBroadcastMessage::Probe(value);
        Int32 data;
        if (cbMessage != NULL && (cbMessage->GetServiceCategory(&data), data) == 50) {
            String latestAreaInfo;
            cbMessage->GetMessageBody(&latestAreaInfo);
            mHost->UpdateAreaInfo(latestAreaInfo);
        }
    }
    return NOERROR;
}

//===============================================================================
//                  CStatus::InitConnectivityReceiver
//===============================================================================

CStatus::InitConnectivityReceiver::InitConnectivityReceiver(
    /* [in] */ CStatus* host)
    : mHost(host)
{}

ECode CStatus::InitConnectivityReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (ArrayUtils::Contains(CONNECTIVITY_INTENTS.Get(), action)) {
        Boolean res;
        mHost->mHandler->SendEmptyMessage(EVENT_UPDATE_CONNECTIVITY, &res);
    }
    return NOERROR;
}

//===============================================================================
//                  CStatus::OnCreateAdapterViewOnItemLongClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CStatus::OnCreateAdapterViewOnItemLongClickListener, Object, IAdapterViewOnItemLongClickListener)

CStatus::OnCreateAdapterViewOnItemLongClickListener::OnCreateAdapterViewOnItemLongClickListener(
    /* [in] */ CStatus* host)
    : mHost(host)
{}

ECode CStatus::OnCreateAdapterViewOnItemLongClickListener::OnItemLongClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IAdapter> listAdapter;
    parent->GetAdapter((IAdapter**)&listAdapter);

    AutoPtr<IInterface> obj;
    listAdapter->GetItem(position, (IInterface**)&obj);
    AutoPtr<IPreference> pref = IPreference::Probe(obj);

    obj = NULL;
    mHost->GetSystemService(IContext::CLIPBOARD_SERVICE, (IInterface**)&obj);
    IClipboardManager* cm = IClipboardManager::Probe(obj);
    AutoPtr<ICharSequence> cs;
    pref->GetSummary((ICharSequence**)&cs);
    cm->SetText(cs);
    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    AutoPtr<IToast> toast;
    helper->MakeText(mHost, Elastos::Droid::R::string::text_copied, IToast::LENGTH_SHORT, (IToast**)&toast);
    toast->Show();
    *result = TRUE;
    return NOERROR;
}

//===============================================================================
//                  CStatus
//===============================================================================

CAR_OBJECT_IMPL(CStatus)

CStatus::CStatus()
    : mShowLatestAreaInfo(FALSE)
{}

ECode CStatus::constructor()
{
    mBatteryInfoReceiver = new InitBatteryInfoReceiver(this);

    mPhoneStateListener = new InitPhoneStateListener(this);

    mAreaInfoReceiver = new InitAreaInfoReceiver(this);

    mConnectivityReceiver = new InitConnectivityReceiver(this);

    return PreferenceActivity::constructor();
}

Boolean CStatus::HasBluetooth()
{
    AutoPtr<IBluetoothAdapterHelper> helper;
    CBluetoothAdapterHelper::AcquireSingleton((IBluetoothAdapterHelper**)&helper);
    AutoPtr<IBluetoothAdapter> adapter;
    helper->GetDefaultAdapter((IBluetoothAdapter**)&adapter);
    return adapter != NULL;
}

Boolean CStatus::HasWimax()
{
    AutoPtr<INetworkInfo> info;
    mCM->GetNetworkInfo(IConnectivityManager::TYPE_WIMAX, (INetworkInfo**)&info);
    return info != NULL;
}

ECode CStatus::OnCreate(
    /* [in] */ IBundle* icicle)
{
    PreferenceActivity::OnCreate(icicle);

    mHandler = new MyHandler(this);
    mHandler->constructor();

    AutoPtr<IInterface> obj;
    GetSystemService(CONNECTIVITY_SERVICE, (IInterface**)&obj);
    mCM = IConnectivityManager::Probe(obj);
    obj = NULL;
    GetSystemService(TELEPHONY_SERVICE, (IInterface**)&obj);
    mTelephonyManager = ITelephonyManager::Probe(obj);
    obj = NULL;
    GetSystemService(WIFI_SERVICE, (IInterface**)&obj);
    mWifiManager = IWifiManager::Probe(obj);

    AddPreferencesFromResource(R::xml::device_info_status);
    FindPreference(KEY_BATTERY_LEVEL, (IPreference**)&mBatteryLevel);
    FindPreference(KEY_BATTERY_STATUS, (IPreference**)&mBatteryStatus);
    FindPreference(KEY_BT_ADDRESS, (IPreference**)&mBtAddress);
    FindPreference(KEY_WIFI_MAC_ADDRESS, (IPreference**)&mWifiMacAddress);
    FindPreference(KEY_WIMAX_MAC_ADDRESS, (IPreference**)&mWimaxMacAddress);
    FindPreference(KEY_IP_ADDRESS, (IPreference**)&mIpAddress);

    GetResources((IResources**)&mRes);
    mRes->GetString(R::string::device_info_default, &mUnknown);
    mRes->GetString(R::string::status_unavailable, &mUnavailable);

    if (UserHandle::GetMyUserId() == IUserHandle::USER_OWNER) {
        AutoPtr<IPhoneFactory> pf;
        CPhoneFactory::AcquireSingleton((IPhoneFactory**)&pf);
        pf->GetDefaultPhone((IPhone**)&mPhone);
    }
    // Note - missing in zaku build, be careful later...
    FindPreference(KEY_SIGNAL_STRENGTH, (IPreference**)&mSignalStrength);
    FindPreference(String("up_time"), (IPreference**)&mUptime);

    Boolean res;
    AutoPtr<IContext> context;
    if (mPhone == NULL || Utils::IsWifiOnly((GetApplicationContext((IContext**)&context), context))) {
        for (Int32 i = 0; i < PHONE_RELATED_ENTRIES->GetLength(); i++) {
            String key = (*PHONE_RELATED_ENTRIES)[i];
            RemovePreferenceFromScreen(key);
        }
    }
    else {
        // NOTE "imei" is the "Device ID" since it represents
        //  the IMEI in GSM and the MEID in CDMA
        String phoneName;
        mPhone->GetPhoneName(&phoneName);
        String str;
        if (phoneName.Equals("CDMA")) {
            SetSummaryText(KEY_MEID_NUMBER, (mPhone->GetMeid(&str), str));
            SetSummaryText(KEY_MIN_NUMBER, (mPhone->GetCdmaMin(&str), str));
            AutoPtr<IResources> resources;
            GetResources((IResources**)&resources);
            resources->GetBoolean(R::bool_::config_msid_enable, &res);
            if (res) {
                AutoPtr<IPreference> tmp;
                FindPreference(KEY_MIN_NUMBER, (IPreference**)&tmp);
                tmp->SetTitle(R::string::status_msid_number);
            }

            SetSummaryText(KEY_PRL_VERSION, (mPhone->GetCdmaPrlVersion(&str), str));
            RemovePreferenceFromScreen(KEY_IMEI_SV);

            Int32 data;
            if ((mPhone->GetLteOnCdmaMode(&data), data) == IPhoneConstants::LTE_ON_CDMA_TRUE) {
                // Show ICC ID and IMEI for LTE device
                SetSummaryText(KEY_ICC_ID, (mPhone->GetIccSerialNumber(&str), str));
                SetSummaryText(KEY_IMEI, (mPhone->GetImei(&str), str));
            }
            else {
                // device is not GSM/UMTS, do not display GSM/UMTS features
                // check Null in case no specified preference in overlay xml
                RemovePreferenceFromScreen(KEY_IMEI);
                RemovePreferenceFromScreen(KEY_ICC_ID);
            }
        }
        else {
            SetSummaryText(KEY_IMEI, (mPhone->GetDeviceId(&str), str));

            obj = NULL;
            GetSystemService(TELEPHONY_SERVICE, (IInterface**)&obj);
            SetSummaryText(KEY_IMEI_SV,
                    (ITelephonyManager::Probe(obj)->GetDeviceSoftwareVersion(&str), str));

            // device is not CDMA, do not display CDMA features
            // check Null in case no specified preference in overlay xml
            RemovePreferenceFromScreen(KEY_PRL_VERSION);
            RemovePreferenceFromScreen(KEY_MEID_NUMBER);
            RemovePreferenceFromScreen(KEY_MIN_NUMBER);
            RemovePreferenceFromScreen(KEY_ICC_ID);

            // only show area info when SIM country is Brazil
            if (String("br").Equals((mTelephonyManager->GetSimCountryIso(&str), str))) {
                mShowLatestAreaInfo = TRUE;
            }
        }

        String rawNumber;
        mTelephonyManager->GetLine1Number(&rawNumber);  // may be NULL or empty
        String formattedNumber;
        if (!TextUtils::IsEmpty(rawNumber)) {
            AutoPtr<IPhoneNumberUtils> pnu;
            CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&pnu);
            pnu->FormatNumber(rawNumber, &formattedNumber);
        }
        // If formattedNumber is NULL or empty, it'll display as "Unknown".
        SetSummaryText(KEY_PHONE_NUMBER, formattedNumber);

        CPhoneStateIntentReceiver::New(this, mHandler, (IPhoneStateIntentReceiver**)&mPhoneStateReceiver);
        mPhoneStateReceiver->NotifySignalStrength(EVENT_SIGNAL_STRENGTH_CHANGED);
        mPhoneStateReceiver->NotifyServiceState(EVENT_SERVICE_STATE_CHANGED);

        if (!mShowLatestAreaInfo) {
            RemovePreferenceFromScreen(KEY_LATEST_AREA_INFO);
        }
    }

    if (!HasBluetooth()) {
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        IPreferenceGroup::Probe(screen)->RemovePreference(mBtAddress, &res);
        mBtAddress = NULL;
    }

    if (!HasWimax()) {
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        IPreferenceGroup::Probe(screen)->RemovePreference(mWimaxMacAddress, &res);
        mWimaxMacAddress = NULL;
    }

    CIntentFilter::New((IIntentFilter**)&mConnectivityIntentFilter);
    for (Int32 i = 0; i < CONNECTIVITY_INTENTS->GetLength(); i++) {
        String intent = (*CONNECTIVITY_INTENTS)[i];
        mConnectivityIntentFilter->AddAction(intent);
    }

    UpdateConnectivity();

    String serial = Build::SERIAL;
    if (!serial.IsNull() && !serial.Equals("")) {
        SetSummaryText(KEY_SERIAL_NUMBER, serial);
    }
    else {
        RemovePreferenceFromScreen(KEY_SERIAL_NUMBER);
    }

    // Make every pref on this screen copy its data to the clipboard on longpress.
    // Super convenient for capturing the IMEI, MAC addr, serial, etc.

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    AutoPtr<IAdapterViewOnItemLongClickListener> listener = new OnCreateAdapterViewOnItemLongClickListener(this);
    IAdapterView::Probe(listView)->SetOnItemLongClickListener(listener);

    return NOERROR;
}

ECode CStatus::OnResume()
{
    PreferenceActivity::OnResume();

    AutoPtr<IIntent> stickyIntent;
    AutoPtr<IContext> context;
    if (mPhone != NULL && !Utils::IsWifiOnly((GetApplicationContext((IContext**)&context), context))) {
        mPhoneStateReceiver->RegisterIntent();

        UpdateSignalStrength();
        AutoPtr<IServiceState> serviceState;
        mPhone->GetServiceState((IServiceState**)&serviceState);
        UpdateServiceState(serviceState);
        UpdateDataState();
        mTelephonyManager->Listen(mPhoneStateListener,
                IPhoneStateListener::LISTEN_DATA_CONNECTION_STATE);
        if (mShowLatestAreaInfo) {
            AutoPtr<IIntentFilter> filter;
            CIntentFilter::New(CB_AREA_INFO_RECEIVED_ACTION, (IIntentFilter**)&filter);
            RegisterReceiver(mAreaInfoReceiver, filter,
                    CB_AREA_INFO_SENDER_PERMISSION, NULL, (IIntent**)&stickyIntent);
            // Ask CellBroadcastReceiver to broadcast the latest area info received
            AutoPtr<IIntent> getLatestIntent;
            CIntent::New(GET_LATEST_CB_AREA_INFO_ACTION, (IIntent**)&getLatestIntent);
            SendBroadcastAsUser(getLatestIntent, UserHandle::ALL,
                    CB_AREA_INFO_SENDER_PERMISSION);
        }
    }
    stickyIntent = NULL;
    RegisterReceiver(mConnectivityReceiver, mConnectivityIntentFilter,
                    Manifest::permission::CHANGE_NETWORK_STATE, NULL, (IIntent**)&stickyIntent);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_BATTERY_CHANGED, (IIntentFilter**)&filter);
    stickyIntent = NULL;
    RegisterReceiver(mBatteryInfoReceiver, filter, (IIntent**)&stickyIntent);
    Boolean res;
    mHandler->SendEmptyMessage(EVENT_UPDATE_STATS, &res);
    return NOERROR;
}

ECode CStatus::OnPause()
{
    PreferenceActivity::OnPause();

    AutoPtr<IContext> context;
    if (mPhone != NULL && !Utils::IsWifiOnly((GetApplicationContext((IContext**)&context), context))) {
        mPhoneStateReceiver->UnregisterIntent();
        mTelephonyManager->Listen(mPhoneStateListener, IPhoneStateListener::LISTEN_NONE);
    }
    if (mShowLatestAreaInfo) {
        UnregisterReceiver(mAreaInfoReceiver);
    }
    UnregisterReceiver(mBatteryInfoReceiver);
    UnregisterReceiver(mConnectivityReceiver);
    mHandler->RemoveMessages(EVENT_UPDATE_STATS);
    return NOERROR;
}

void CStatus::RemovePreferenceFromScreen(
    /* [in] */ const String& key)
{
    AutoPtr<IPreference> pref;
    FindPreference(key, (IPreference**)&pref);
    if (pref != NULL) {
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        Boolean res;
        IPreferenceGroup::Probe(screen)->RemovePreference(pref, &res);
    }
}

void CStatus::SetSummary(
    /* [in] */ const String& preference,
    /* [in] */ const String& property,
    /* [in] */ const String& alt)
{
    // try {
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String str;
    sp->Get(property, alt, &str);
    AutoPtr<IPreference> pref;
    FindPreference(preference, (IPreference**)&pref);
    pref->SetSummary(CoreUtils::Convert(str));
    // } catch (RuntimeException e) {

    // }
}

void CStatus::SetSummaryText(
    /* [in] */ const String& preference,
    /* [in] */ const String& _text)
{
    String text = _text;
    if (TextUtils::IsEmpty(text)) {
       text = mUnknown;
    }
    // some preferences may be missing
    AutoPtr<IPreference> pref;
    FindPreference(preference, (IPreference**)&pref);
    if (pref!= NULL) {
        pref->SetSummary(CoreUtils::Convert(text));
    }
}

void CStatus::UpdateNetworkType()
{
    // Whether EDGE, UMTS, etc...
    String networktype;
    Int32 val;
    if (ITelephonyManager::NETWORK_TYPE_UNKNOWN != (mTelephonyManager->GetNetworkType(&val), val)) {
        mTelephonyManager->GetNetworkTypeName(&networktype);
    }
    SetSummaryText(KEY_NETWORK_TYPE, networktype);
}

void CStatus::UpdateDataState()
{
    Int32 state;
    mTelephonyManager->GetDataState(&state);
    String display;
    mRes->GetString(R::string::radioInfo_unknown, &display);

    switch (state) {
        case ITelephonyManager::DATA_CONNECTED:
            mRes->GetString(R::string::radioInfo_data_connected, &display);
            break;
        case ITelephonyManager::DATA_SUSPENDED:
            mRes->GetString(R::string::radioInfo_data_suspended, &display);
            break;
        case ITelephonyManager::DATA_CONNECTING:
            mRes->GetString(R::string::radioInfo_data_connecting, &display);
            break;
        case ITelephonyManager::DATA_DISCONNECTED:
            mRes->GetString(R::string::radioInfo_data_disconnected, &display);
            break;
    }

    SetSummaryText(KEY_DATA_STATE, display);
}

void CStatus::UpdateServiceState(
    /* [in] */ IServiceState* serviceState)
{
    Int32 state;
    serviceState->GetState(&state);
    String display;
    mRes->GetString(R::string::radioInfo_unknown, &display);

    switch (state) {
        case IServiceState::STATE_IN_SERVICE:
            mRes->GetString(R::string::radioInfo_service_in, &display);
            break;
        case IServiceState::STATE_OUT_OF_SERVICE:
        case IServiceState::STATE_EMERGENCY_ONLY:
            mRes->GetString(R::string::radioInfo_service_out, &display);
            break;
        case IServiceState::STATE_POWER_OFF:
            mRes->GetString(R::string::radioInfo_service_off, &display);
            break;
    }

    SetSummaryText(KEY_SERVICE_STATE, display);

    Boolean roaming;
    String str;
    if (serviceState->GetRoaming(&roaming), roaming) {
        mRes->GetString(R::string::radioInfo_roaming_in, &str);
        SetSummaryText(KEY_ROAMING_STATE, str);
    }
    else {
        mRes->GetString(R::string::radioInfo_roaming_not, &str);
        SetSummaryText(KEY_ROAMING_STATE, str);
    }
    serviceState->GetOperatorAlphaLong(&str);
    SetSummaryText(KEY_OPERATOR_NAME, str);
}

void CStatus::UpdateAreaInfo(
    /* [in] */ const String& areaInfo)
{
    if (!areaInfo.IsNull()) {
        SetSummaryText(KEY_LATEST_AREA_INFO, areaInfo);
    }
}

void CStatus::UpdateSignalStrength()
{
    // TODO PhoneStateIntentReceiver is deprecated and PhoneStateListener
    // should probably used instead.

    // not loaded in some versions of the code (e.g., zaku)
    if (mSignalStrength != NULL) {
        AutoPtr<IServiceState> serviceState;
        mPhoneStateReceiver->GetServiceState((IServiceState**)&serviceState);
        Int32 state;
        serviceState->GetState(&state);
        AutoPtr<IResources> r;
        GetResources((IResources**)&r);

        if ((IServiceState::STATE_OUT_OF_SERVICE == state) ||
                (IServiceState::STATE_POWER_OFF == state)) {
            mSignalStrength->SetSummary(CoreUtils::Convert("0"));
            return;
        }

        Int32 signalDbm;
        mPhoneStateReceiver->GetSignalStrengthDbm(&signalDbm);

        if (-1 == signalDbm) signalDbm = 0;

        Int32 signalAsu;
        mPhoneStateReceiver->GetSignalStrengthLevelAsu(&signalAsu);

        if (-1 == signalAsu) signalAsu = 0;

        StringBuilder builder;
        builder += signalDbm;
        builder += " ";
        String str;
        r->GetString(R::string::radioInfo_display_dbm, &str);
        builder += str;
        builder += "   ";
        builder += signalAsu;
        builder += " ";
        r->GetString(R::string::radioInfo_display_asu, &str);
        builder += str;

        mSignalStrength->SetSummary(builder.ToCharSequence());
    }
}

void CStatus::SetWimaxStatus()
{
    if (mWimaxMacAddress != NULL) {
        AutoPtr<ISystemProperties> sp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
        String macAddress;
        sp->Get(String("net.wimax.mac.address"), mUnavailable, &macAddress);
        mWimaxMacAddress->SetSummary(CoreUtils::Convert(macAddress));
    }
}

void CStatus::SetWifiStatus()
{
    AutoPtr<IWifiInfo> wifiInfo;
    mWifiManager->GetConnectionInfo((IWifiInfo**)&wifiInfo);
    String macAddress;
    if (wifiInfo != NULL) {
        wifiInfo->GetMacAddress(&macAddress);
    }
    mWifiMacAddress->SetSummary(CoreUtils::Convert(!TextUtils::IsEmpty(macAddress) ? macAddress : mUnavailable));
}

void CStatus::SetIpAddressStatus()
{
    String ipAddress = Utils::GetDefaultIpAddresses(mCM);
    if (!ipAddress.IsNull()) {
        mIpAddress->SetSummary(CoreUtils::Convert(ipAddress));
    }
    else {
        mIpAddress->SetSummary(CoreUtils::Convert(mUnavailable));
    }
}

void CStatus::SetBtStatus()
{
    AutoPtr<IBluetoothAdapterHelper> helper;
    CBluetoothAdapterHelper::AcquireSingleton((IBluetoothAdapterHelper**)&helper);
    AutoPtr<IBluetoothAdapter> bluetooth;
    helper->GetDefaultAdapter((IBluetoothAdapter**)&bluetooth);
    if (bluetooth != NULL && mBtAddress != NULL) {
        String address;
        Boolean res;
        bluetooth->IsEnabled(&res);
        if (res) {
            bluetooth->GetAddress(&address);
        }
        if (!TextUtils::IsEmpty(address)) {
           // Convert the address to lowercase for consistency with the wifi MAC address.
            mBtAddress->SetSummary(CoreUtils::Convert(address.ToLowerCase()));
        }
        else {
            mBtAddress->SetSummary(CoreUtils::Convert(mUnavailable));
        }
    }
}

void CStatus::UpdateConnectivity()
{
    SetWimaxStatus();
    SetWifiStatus();
    SetBtStatus();
    SetIpAddressStatus();
}

void CStatus::UpdateTimes()
{
    // Int64 at = SystemClock::GetUptimeMillis() / 1000;
    Int64 ut = SystemClock::GetElapsedRealtime() / 1000;

    if (ut == 0) {
        ut = 1;
    }

    mUptime->SetSummary(CoreUtils::Convert(Convert(ut)));
}

String CStatus::Pad(
    /* [in] */ Int32 n)
{
    if (n >= 10) {
        return StringUtils::ToString(n);
    }
    else {
        return String("0") + StringUtils::ToString(n);
    }
}

String CStatus::Convert(
    /* [in] */ Int64 t)
{
    Int32 s = (Int32)(t % 60);
    Int32 m = (Int32)((t / 60) % 60);
    Int32 h = (Int32)((t / 3600));

    StringBuilder builder;
    builder += h;
    builder += ":";
    builder += Pad(m);
    builder += ":";
    builder += Pad(s);
    return builder.ToString();
}

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos