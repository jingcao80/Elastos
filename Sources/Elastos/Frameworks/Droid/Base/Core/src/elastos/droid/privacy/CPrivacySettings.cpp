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

#include "elastos/droid/privacy/CPrivacySettings.h"
#include <stdlib.h>
#include <Elastos.CoreLibrary.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::Math;
using Elastos::Math::IBigDecimal;
using Elastos::Math::CBigDecimal;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Privacy {

/** used to create random android ID*/
const String CPrivacySettings::ID_PATTERN[] =
{
String("0"), String("1"), String("2"), String("3"), String("4"), String("5"),
String("6"), String("7"), String("8"), String("9"), String("a"),
String("b"), String("d"), String("e"), String("f") };

// constants for identification of data types transmitted in the notification intent
const String CPrivacySettings::DATA_DEVICE_ID = String("deviceID");
const String CPrivacySettings::DATA_LINE_1_NUMBER = String("line1Number");
const String CPrivacySettings::DATA_LOCATION_GPS = String("locationGPS");
const String CPrivacySettings::DATA_LOCATION_NETWORK = String("locationNetwork");
const String CPrivacySettings::DATA_NETWORK_INFO_CURRENT = String("networkInfoCurrent");
const String CPrivacySettings::DATA_NETWORK_INFO_SIM = String("networkInfoSIM");
const String CPrivacySettings::DATA_SIM_SERIAL = String("simSerial");
const String CPrivacySettings::DATA_SUBSCRIBER_ID = String("subscriberID");
const String CPrivacySettings::DATA_ACCOUNTS_LIST = String("accountsList");
const String CPrivacySettings::DATA_AUTH_TOKENS = String("authTokens");
const String CPrivacySettings::DATA_OUTGOING_CALL = String("outgoingCall");
const String CPrivacySettings::DATA_INCOMING_CALL = String("incomingCall");
const String CPrivacySettings::DATA_CONTACTS = String("contacts");
const String CPrivacySettings::DATA_CALENDAR = String("calendar");
const String CPrivacySettings::DATA_MMS = String("mms");
const String CPrivacySettings::DATA_SMS = String("sms");
const String CPrivacySettings::DATA_MMS_SMS = String("mmsSms");
const String CPrivacySettings::DATA_CALL_LOG = String("callLog");
const String CPrivacySettings::DATA_BOOKMARKS = String("bookmarks");
const String CPrivacySettings::DATA_SYSTEM_LOGS = String("systemLogs");
const String CPrivacySettings::DATA_INTENT_BOOT_COMPLETED = String("intentBootCompleted");
//const String DATA_EXTERNAL_STORAGE = "externalStorage";
const String CPrivacySettings::DATA_CAMERA = String("camera");
const String CPrivacySettings::DATA_RECORD_AUDIO = String("recordAudio");
const String CPrivacySettings::DATA_SMS_SEND = String("SmsSend");
const String CPrivacySettings::DATA_PHONE_CALL = String("phoneCall");
const String CPrivacySettings::DATA_ANDROID_ID = String("android_id");
const String CPrivacySettings::DATA_ICC_ACCESS = String("iccAccess");
const String CPrivacySettings::DATA_WIFI_INFO = String("wifiInfo");
const String CPrivacySettings::DATA_IP_TABLES = String("iptables");
const String CPrivacySettings::DATA_SWITCH_CONNECTIVITY = String("switchconnectivity");
const String CPrivacySettings::DATA_SEND_MMS = String("sendMms");
const String CPrivacySettings::DATA_SWITCH_WIFI_STATE = String("switchWifiState");

const Byte CPrivacySettings::REAL;
const Byte CPrivacySettings::EMPTY;
const Byte CPrivacySettings::CUSTOM;
const Byte CPrivacySettings::RANDOM;
const Byte CPrivacySettings::SETTING_NOTIFY_OFF;
const Byte CPrivacySettings::SETTING_NOFITY_ON;


ECode CPrivacySettings::GetSwitchWifiStateSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = switchWifiStateSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetSwitchWifiStateSetting(
    /* [in] */ Byte switchWifiStateSetting)
{
    this->switchWifiStateSetting = switchWifiStateSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetForceOnlineState(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = forceOnlineState;
    return NOERROR;
}

ECode CPrivacySettings::SetForceOnlineState(
    /* [in] */ Byte forceOnlineState)
{
    this->forceOnlineState = forceOnlineState;
    return NOERROR;
}

ECode CPrivacySettings::GetSendMmsSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = sendMmsSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetSendMmsSetting(
    /* [in] */ Byte sendMmsSetting)
{
    this->sendMmsSetting = sendMmsSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetSwitchConnectivitySetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = switchConnectivitySetting;
    return NOERROR;
}

ECode CPrivacySettings::SetSwitchConnectivitySetting(
    /* [in] */ Byte switchConnectivitySetting)
{
    this->switchConnectivitySetting = switchConnectivitySetting;
    return NOERROR;
}

ECode CPrivacySettings::GetAndroidIdSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = androidIdSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetAndroidIdSetting(
    /* [in] */ Byte androidIdSetting)
{
    this->androidIdSetting = androidIdSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetAndroidID(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = androidID;
    return NOERROR;
}

ECode CPrivacySettings::GetWifiInfoSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = wifiInfoSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetWifiInfoSetting(
    /* [in] */ Byte wifiInfoSetting)
{
    this->wifiInfoSetting = wifiInfoSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetAndroidID(
    /* [in] */ const String& androidID)
{
    this->androidID = androidID;
    return NOERROR;
}

ECode CPrivacySettings::GetIpTableProtectSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = ipTableProtectSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetIpTableProtectSetting(
    /* [in] */ Byte ipTableProtectSetting)
{
    this->ipTableProtectSetting = ipTableProtectSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetIccAccessSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = iccAccessSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetIccAccessSetting(
    /* [in] */ Byte iccAccessSetting)
{
    this->iccAccessSetting = iccAccessSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetAddOnManagementSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = addOnManagementSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetAddOnManagementSetting(
    /* [in] */ Byte addOnManagementSetting)
{
    this->addOnManagementSetting = addOnManagementSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetSmsSendSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = smsSendSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetSmsSendSetting(
    /* [in] */ Byte smsSendSetting)
{
    this->smsSendSetting = smsSendSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetPhoneCallSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = phoneCallSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetPhoneCallSetting(
    /* [in] */ Byte phoneCallSetting)
{
    this->phoneCallSetting = phoneCallSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetRecordAudioSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = recordAudioSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetRecordAudioSetting(
    /* [in] */ Byte recordAudioSetting)
{
    this->recordAudioSetting = recordAudioSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetCameraSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = cameraSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetCameraSetting(
    /* [in] */ Byte cameraSetting)
{
    this->cameraSetting = cameraSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = _id;
    return NOERROR;
}


ECode CPrivacySettings::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = this->packageName;
    return NOERROR;
}

ECode CPrivacySettings::SetPackageName(
    /* [in] */ const String& packageName)
{
    this->packageName = packageName;
    return NOERROR;
}

ECode CPrivacySettings::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid);
    *uid = this->uid;
    return NOERROR;
}

ECode CPrivacySettings::SetUid(
    /* [in] */ Int32 uid)
{
    this->uid = uid;
    return NOERROR;
}

ECode CPrivacySettings::GetDeviceIdSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = deviceIdSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetDeviceIdSetting(
    /* [in] */ Byte deviceIdSetting)
{
    this->deviceIdSetting = deviceIdSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetDeviceId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = deviceId;
    return NOERROR;
}

ECode CPrivacySettings::SetDeviceId(
    /* [in] */ const String& deviceId)
{
    this->deviceId = deviceId;
    return NOERROR;
}

ECode CPrivacySettings::GetLine1NumberSetting(
    /* [out] */ Byte* setting)
{
   VALIDATE_NOT_NULL(setting);
    *setting = line1NumberSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetLine1NumberSetting(
    /* [in] */ Byte line1NumberSetting)
{
    this->line1NumberSetting = line1NumberSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetLine1Number(
    /* [out] */ String* number)
{
    VALIDATE_NOT_NULL(number);
    *number = line1Number;
    return NOERROR;
}

ECode CPrivacySettings::SetLine1Number(
    /* [in] */ const String& line1Number)
{
    this->line1Number = line1Number;
    return NOERROR;
}

ECode CPrivacySettings::GetLocationGpsSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = locationGpsSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetLocationGpsSetting(
    /* [in] */ Byte locationGpsSetting)
{
    this->locationGpsSetting = locationGpsSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetLocationGpsLat(
    /* [out] */ String* lat)
{
    VALIDATE_NOT_NULL(lat);

    if (locationGpsSetting == EMPTY){
        *lat = String("");
    }
    else if (locationGpsSetting == RANDOM){
        *lat = GetRandomLat();
    }
    else{
        *lat = locationGpsLat;
    }
    return NOERROR;
}

ECode CPrivacySettings::SetLocationGpsLat(
    /* [in] */ const String& locationGpsLat)
{
    this->locationGpsLat = locationGpsLat;
    return NOERROR;
}

ECode CPrivacySettings::GetLocationGpsLon(
    /* [out] */ String* lng)
{
    VALIDATE_NOT_NULL(lng);

    if (locationGpsSetting == EMPTY){
        *lng = String("");
    }
    else if (locationGpsSetting == RANDOM){
        *lng = GetRandomLon();
    }
    else{
        *lng = locationGpsLon;
    }

    return NOERROR;
}

ECode CPrivacySettings::SetLocationGpsLon(
    /* [in] */ const String& locationGpsLon)
{
    this->locationGpsLon = locationGpsLon;
    return NOERROR;
}

ECode CPrivacySettings::GetLocationNetworkSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = locationNetworkSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetLocationNetworkSetting(
    /* [in] */ Byte locationNetworkSetting)
{
    this->locationNetworkSetting = locationNetworkSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetLocationNetworkLat(
    /* [out] */ String* lat)
{
    VALIDATE_NOT_NULL(lat);
    *lat = locationNetworkLat;
    return NOERROR;
}

ECode CPrivacySettings::SetLocationNetworkLat(
    /* [in] */ const String& locationNetworkLat)
{
    this->locationNetworkLat = locationNetworkLat;
    return NOERROR;
}

ECode CPrivacySettings::GetLocationNetworkLon(
    /* [out] */ String* lng)
{
    VALIDATE_NOT_NULL(lng);
    *lng = locationNetworkLon;
    return NOERROR;
}

ECode CPrivacySettings::SetLocationNetworkLon(
    /* [in] */ const String& locationNetworkLon)
{
    this->locationNetworkLon = locationNetworkLon;
    return NOERROR;
}

ECode CPrivacySettings::GetNetworkInfoSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = networkInfoSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetNetworkInfoSetting(
    /* [in] */ Byte networkInfoSetting)
{
    this->networkInfoSetting = networkInfoSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetSimInfoSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = simInfoSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetSimInfoSetting(
    /* [in] */ Byte simInfoSetting)
{
    this->simInfoSetting = simInfoSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetSimSerialNumberSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = simSerialNumberSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetSimSerialNumberSetting(
    /* [in] */ Byte simSerialNumberSetting)
{
    this->simSerialNumberSetting = simSerialNumberSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetSimSerialNumber(
    /* [out] */ String* number)
{
    VALIDATE_NOT_NULL(number);
    *number = simSerialNumber;
    return NOERROR;
}

ECode CPrivacySettings::SetSimSerialNumber(
    /* [in] */ const String& simSerialNumber)
{
    this->simSerialNumber = simSerialNumber;
    return NOERROR;
}

ECode CPrivacySettings::GetSubscriberIdSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = subscriberIdSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetSubscriberIdSetting(
    /* [in] */ Byte subscriberIdSetting)
{
    this->subscriberIdSetting = subscriberIdSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetSubscriberId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = subscriberId;
    return NOERROR;
}

ECode CPrivacySettings::SetSubscriberId(
    /* [in] */ const String& subscriberId)
{
    this->subscriberId = subscriberId;
    return NOERROR;
}

ECode CPrivacySettings::GetAccountsSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = accountsSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetAccountsSetting(
    /* [in] */ Byte accountsSetting)
{
    this->accountsSetting = accountsSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetAccountsAuthTokensSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = accountsAuthTokensSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetAccountsAuthTokensSetting(
    /* [in] */ Byte accountsAuthTokensSetting)
{
    this->accountsAuthTokensSetting = accountsAuthTokensSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetOutgoingCallsSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = outgoingCallsSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetOutgoingCallsSetting(
    /* [in] */ Byte outgoingCallsSetting)
{
    this->outgoingCallsSetting = outgoingCallsSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetIncomingCallsSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = incomingCallsSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetIncomingCallsSetting(
    /* [in] */ Byte incomingCallsSetting)
{
    this->incomingCallsSetting = incomingCallsSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetContactsSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = contactsSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetContactsSetting(
    /* [in] */ Byte setting)
{
    contactsSetting = setting;
    return NOERROR;
}

ECode CPrivacySettings::GetCalendarSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = calendarSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetCalendarSetting(
    /* [in] */ Byte calendarSetting)
{
    this->calendarSetting = calendarSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetMmsSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = mmsSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetMmsSetting(
    /* [in] */ Byte mmsSetting)
{
    this->mmsSetting = mmsSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetSmsSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = smsSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetSmsSetting(
    /* [in] */ Byte smsSetting)
{
    this->smsSetting = smsSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetCallLogSetting(
    /* [out] */ Byte* setting)
{
   VALIDATE_NOT_NULL(setting);
    *setting = callLogSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetCallLogSetting(
    /* [in] */ Byte callLogSetting)
{
    this->callLogSetting = callLogSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetBookmarksSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = bookmarksSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetBookmarksSetting(
    /* [in] */ Byte bookmarksSetting)
{
    this->bookmarksSetting = bookmarksSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetSystemLogsSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = systemLogsSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetSystemLogsSetting(
    /* [in] */ Byte systemLogsSetting)
{
    this->systemLogsSetting = systemLogsSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetIntentBootCompletedSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = intentBootCompletedSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetIntentBootCompletedSetting(
    /* [in] */ Byte intentBootCompletedSetting)
{
    this->intentBootCompletedSetting = intentBootCompletedSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetNotificationSetting(
    /* [out] */ Byte* setting)
{
    VALIDATE_NOT_NULL(setting);
    *setting = notificationSetting;
    return NOERROR;
}

ECode CPrivacySettings::SetNotificationSetting(
    /* [in] */ Byte notificationSetting)
{
    this->notificationSetting = notificationSetting;
    return NOERROR;
}

ECode CPrivacySettings::GetAllowedContacts(
    /* [out, callee] */ ArrayOf<Int32>** contacts)
{
    VALIDATE_NOT_NULL(contacts);
    *contacts = allowedContacts;
    REFCOUNT_ADD(*contacts);
    return NOERROR;
}

ECode CPrivacySettings::SetAllowedContacts(
    /* [in] */ ArrayOf<Int32>* allowedContacts)
{
    this->allowedContacts = allowedContacts;
    return NOERROR;
}

ECode CPrivacySettings::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&_id);
    source->ReadString(&packageName);
    source->ReadInt32(&uid);
    source->ReadByte(&locationGpsSetting);
    source->ReadString(&locationGpsLat);
    source->ReadString(&locationGpsLon);
    source->ReadByte(&contactsSetting);
    source->ReadByte(&deviceIdSetting);
    source->ReadString(&deviceId);
    source->ReadByte(&line1NumberSetting);
    source->ReadString(&line1Number);
    source->ReadByte(&locationNetworkSetting);
    source->ReadString(&locationNetworkLat);
    source->ReadString(&locationNetworkLon);
    source->ReadByte(&networkInfoSetting);
    source->ReadByte(&simInfoSetting);
    source->ReadByte(&simSerialNumberSetting);
    source->ReadString(&simSerialNumber);
    source->ReadByte(&subscriberIdSetting);
    source->ReadString(&subscriberId);
    source->ReadByte(&accountsSetting);
    source->ReadByte(&accountsAuthTokensSetting);
    source->ReadByte(&outgoingCallsSetting);
    source->ReadByte(&incomingCallsSetting);
    source->ReadByte(&calendarSetting);
    source->ReadByte(&mmsSetting);
    source->ReadByte(&smsSetting);
    source->ReadByte(&callLogSetting);
    source->ReadByte(&bookmarksSetting);
    source->ReadByte(&systemLogsSetting);
    source->ReadByte(&notificationSetting);
    source->ReadByte(&intentBootCompletedSetting);
    source->ReadByte(&externalStorageSetting);
    source->ReadByte(&cameraSetting);
    source->ReadByte(&recordAudioSetting);
    source->ReadArrayOf((HANDLE*)&allowedContacts);
    source->ReadByte(&smsSendSetting);
    source->ReadByte(&phoneCallSetting);
    source->ReadByte(&ipTableProtectSetting);
    source->ReadByte(&iccAccessSetting);
    source->ReadByte(&addOnManagementSetting);
    source->ReadByte(&androidIdSetting);
    source->ReadString(&androidID);
    source->ReadByte(&wifiInfoSetting);
    source->ReadByte(&switchConnectivitySetting);
    source->ReadByte(&sendMmsSetting);
    source->ReadByte(&forceOnlineState);
    source->ReadByte(&switchWifiStateSetting);
    return NOERROR;
}

ECode CPrivacySettings::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(_id);
    dest->WriteString(packageName);
    dest->WriteInt32(uid);
    dest->WriteByte(locationGpsSetting);
    dest->WriteString(locationGpsLat);
    dest->WriteString(locationGpsLon);
    dest->WriteByte(contactsSetting);
    dest->WriteByte(deviceIdSetting);
    dest->WriteString(deviceId);
    dest->WriteByte(line1NumberSetting);
    dest->WriteString(line1Number);
    dest->WriteByte(locationNetworkSetting);
    dest->WriteString(locationNetworkLat);
    dest->WriteString(locationNetworkLon);
    dest->WriteByte(networkInfoSetting);
    dest->WriteByte(simInfoSetting);
    dest->WriteByte(simSerialNumberSetting);
    dest->WriteString(simSerialNumber);
    dest->WriteByte(subscriberIdSetting);
    dest->WriteString(subscriberId);
    dest->WriteByte(accountsSetting);
    dest->WriteByte(accountsAuthTokensSetting);
    dest->WriteByte(outgoingCallsSetting);
    dest->WriteByte(incomingCallsSetting);
    dest->WriteByte(calendarSetting);
    dest->WriteByte(mmsSetting);
    dest->WriteByte(smsSetting);
    dest->WriteByte(callLogSetting);
    dest->WriteByte(bookmarksSetting);
    dest->WriteByte(systemLogsSetting);
    dest->WriteByte(notificationSetting);
    dest->WriteByte(intentBootCompletedSetting);
    dest->WriteByte(externalStorageSetting);
    dest->WriteByte(cameraSetting);
    dest->WriteByte(recordAudioSetting);
    dest->WriteArrayOf((HANDLE)allowedContacts.Get());
    dest->WriteByte(smsSendSetting);
    dest->WriteByte(phoneCallSetting);
    dest->WriteByte(ipTableProtectSetting);
    dest->WriteByte(iccAccessSetting);
    dest->WriteByte(addOnManagementSetting);
    dest->WriteByte(androidIdSetting);
    dest->WriteString(androidID);
    dest->WriteByte(wifiInfoSetting);
    dest->WriteByte(switchConnectivitySetting);
    dest->WriteByte(sendMmsSetting);
    dest->WriteByte(forceOnlineState);
    dest->WriteByte(switchWifiStateSetting);
    return NOERROR;
}

ECode CPrivacySettings::constructor()
{
    constructor(-1, String(""), 0, TRUE);
    return NOERROR;
}

ECode CPrivacySettings::constructor(
    /* [in] */ Int32 id,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    this->_id = id;
    this->packageName = packageName;
    this->uid = uid;
    this->deviceIdSetting = REAL;
    this->line1NumberSetting = REAL;
    this->locationGpsSetting = REAL;
    this->locationNetworkSetting = REAL;
    this->networkInfoSetting = REAL;
    this->simInfoSetting = REAL;
    this->simSerialNumberSetting = REAL;
    this->subscriberIdSetting = REAL;
    this->accountsSetting = REAL;
    this->accountsAuthTokensSetting = REAL;
    this->outgoingCallsSetting = REAL;
    this->incomingCallsSetting = REAL;
    this->contactsSetting = REAL;
    this->calendarSetting = REAL;
    this->mmsSetting = REAL;
    this->smsSetting = REAL;
    this->callLogSetting = REAL;
    this->bookmarksSetting = REAL;
    this->systemLogsSetting = REAL;
    this->notificationSetting = SETTING_NOTIFY_OFF;
    this->intentBootCompletedSetting = REAL;
    this->externalStorageSetting = REAL;
    this->cameraSetting = REAL;
    this->recordAudioSetting = REAL;
    this->smsSendSetting = REAL;
    this->phoneCallSetting = REAL;
    this->ipTableProtectSetting = REAL;
    this->iccAccessSetting = REAL;
    this->addOnManagementSetting = EMPTY;
    this->androidIdSetting = REAL;
    this->wifiInfoSetting = REAL;
    this->switchConnectivitySetting = REAL;
    this->sendMmsSetting = REAL;
    this->forceOnlineState = EMPTY;
    this->switchWifiStateSetting = REAL;

    return NOERROR;
}

ECode CPrivacySettings::constructor(
    /* [in] */ Int32 id,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean allEmpty)
{
    this->_id = id;

    this->packageName = packageName;
    this->uid = uid;
    if (allEmpty)
    {
        this->deviceIdSetting = EMPTY;
        this->line1NumberSetting = EMPTY;
        this->locationGpsSetting = EMPTY;
        this->locationNetworkSetting = EMPTY;
        this->networkInfoSetting = EMPTY;
        this->simInfoSetting = EMPTY;
        this->simSerialNumberSetting = EMPTY;
        this->subscriberIdSetting = EMPTY;
        this->accountsSetting = EMPTY;
        this->accountsAuthTokensSetting = EMPTY;
        this->outgoingCallsSetting = EMPTY;
        this->incomingCallsSetting = EMPTY;
        this->contactsSetting = EMPTY;
        this->calendarSetting = EMPTY;
        this->mmsSetting = EMPTY;
        this->smsSetting = EMPTY;
        this->callLogSetting = EMPTY;
        this->bookmarksSetting = EMPTY;
        this->systemLogsSetting = EMPTY;
        this->notificationSetting = SETTING_NOTIFY_OFF;
        this->intentBootCompletedSetting = EMPTY;
        this->externalStorageSetting = REAL;
        this->cameraSetting = EMPTY;
        this->recordAudioSetting = EMPTY;
        this->smsSendSetting = EMPTY;
        this->phoneCallSetting = EMPTY;
        this->ipTableProtectSetting = EMPTY;
        this->iccAccessSetting = EMPTY;
        this->addOnManagementSetting = EMPTY;
        this->androidIdSetting = EMPTY;
        this->wifiInfoSetting = EMPTY;
        this->switchConnectivitySetting = EMPTY;
        this->sendMmsSetting = EMPTY;
        this->forceOnlineState = REAL;
        this->switchWifiStateSetting = EMPTY;
    }
    else
    {
        this->deviceIdSetting = RANDOM;
        this->line1NumberSetting = RANDOM;
        this->locationGpsSetting = RANDOM;
        this->locationNetworkSetting = RANDOM;
        this->networkInfoSetting = EMPTY;
        this->simInfoSetting = EMPTY;
        this->simSerialNumberSetting = RANDOM;
        this->subscriberIdSetting = RANDOM;
        this->accountsSetting = EMPTY;
        this->accountsAuthTokensSetting = EMPTY;
        this->outgoingCallsSetting = EMPTY;
        this->incomingCallsSetting = EMPTY;
        this->contactsSetting = EMPTY;
        this->calendarSetting = EMPTY;
        this->mmsSetting = EMPTY;
        this->smsSetting = EMPTY;
        this->callLogSetting = EMPTY;
        this->bookmarksSetting = EMPTY;
        this->systemLogsSetting = EMPTY;
        this->notificationSetting = SETTING_NOTIFY_OFF;
        this->intentBootCompletedSetting = EMPTY;
        this->externalStorageSetting = REAL;
        this->cameraSetting = EMPTY;
        this->recordAudioSetting = EMPTY;
        this->smsSendSetting = EMPTY;
        this->phoneCallSetting = EMPTY;
        this->ipTableProtectSetting = EMPTY;
        this->iccAccessSetting = EMPTY;
        this->addOnManagementSetting = EMPTY;
        this->androidIdSetting = RANDOM;
        this->wifiInfoSetting = EMPTY;
        this->switchConnectivitySetting = EMPTY;
        this->sendMmsSetting = EMPTY;
        this->forceOnlineState = REAL;
        this->switchWifiStateSetting = EMPTY;
    }

    return NOERROR;
}

String CPrivacySettings::GetRandomLat()
{
    return GetRandomCoordinate(180);
}

String CPrivacySettings::GetRandomLon()
{
    return GetRandomCoordinate(360);
}

String CPrivacySettings::GetRandomCoordinate(
    /* [in] */ Int32 limit)
{
    AutoPtr<IBigDecimal> latitude;
    srand((Int32)time(0));
    Double lat = rand() / (RAND_MAX + 1.0) * limit;
    // Double lat = Elastos::Core::Math::Random() * limit;

    if (lat > (limit / 2)) {
        CBigDecimal::New((lat - (limit / 2)), (IBigDecimal**)&latitude);
    }else{
        CBigDecimal::New(-lat, (IBigDecimal**)&latitude);
    }

    AutoPtr<IBigDecimal> latitudeNew;
    latitude->SetScale(6, IBigDecimal::ROUND_HALF_UP, (IBigDecimal**)&latitudeNew);

    String latStr("0");
    if(latitudeNew != NULL){
        latitudeNew->ToString(&latStr);
    }

    return latStr;
}

}
}
}

