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

#ifndef __ELASTOS_DROID_PRIVACY_CPRIVACYSETTINGS_H__
#define __ELASTOS_DROID_PRIVACY_CPRIVACYSETTINGS_H__

#include "_Elastos_Droid_Privacy_CPrivacySettings.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Privacy {

CarClass(CPrivacySettings)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPrivacySettings()
        : _id(0)
        , packageName(String(NULL))
        , uid(0)
        , deviceIdSetting(0)
        , deviceId(String(NULL))
        , line1NumberSetting(0)
        , line1Number(String(NULL))
        , locationGpsSetting(0)
        , locationGpsLat(String(NULL))
        , locationGpsLon(String(NULL))
        , locationNetworkSetting(0)
        , locationNetworkLat(String(NULL))
        , locationNetworkLon(String(NULL))
        , networkInfoSetting(0)
        , simInfoSetting(0)
        , simSerialNumberSetting(0)
        , simSerialNumber(String(NULL))
        , subscriberIdSetting(0)
        , subscriberId(String(NULL))
        , accountsSetting(0)
        , accountsAuthTokensSetting(0)
        , outgoingCallsSetting(0)
        , incomingCallsSetting(0)
        , contactsSetting(0)
        , calendarSetting(0)
        , mmsSetting(0)
        , smsSetting(0)
        , callLogSetting(0)
        , bookmarksSetting(0)
        , systemLogsSetting(0)
        , notificationSetting(0)
        , intentBootCompletedSetting(0)
        , cameraSetting(0)
        , recordAudioSetting(0)
        , smsSendSetting(0)
        , phoneCallSetting(0)
        , ipTableProtectSetting(0)
        , iccAccessSetting(0)
        , addOnManagementSetting(0)
        , androidIdSetting(0)
        , androidID(String(NULL))
        , wifiInfoSetting(0)
        , switchConnectivitySetting(0)
        , sendMmsSetting(0)
        , forceOnlineState(0)
        , switchWifiStateSetting(0)
    {}

    CARAPI GetSwitchWifiStateSetting(
        /* [out] */ Byte* setting);

    CARAPI SetSwitchWifiStateSetting(
        /* [in] */ Byte switchWifiStateSetting);

    CARAPI GetForceOnlineState(
        /* [out] */ Byte* setting);

    CARAPI SetForceOnlineState(
        /* [in] */ Byte forceOnlineState);

    CARAPI GetSendMmsSetting(
        /* [out] */ Byte* setting);

    CARAPI SetSendMmsSetting(
        /* [in] */ Byte sendMmsSetting);

    CARAPI GetSwitchConnectivitySetting(
        /* [out] */ Byte* setting);

    CARAPI SetSwitchConnectivitySetting(
        /* [in] */ Byte switchConnectivitySetting);

    CARAPI GetAndroidIdSetting(
        /* [out] */ Byte* setting);

    CARAPI SetAndroidIdSetting(
        /* [in] */ Byte androidIdSetting);

    CARAPI GetAndroidID(
        /* [out] */ String* id);

    CARAPI GetWifiInfoSetting(
        /* [out] */ Byte* setting);

    CARAPI SetWifiInfoSetting(
        /* [in] */ Byte wifiInfoSetting);

    CARAPI SetAndroidID(
        /* [in] */ const String& androidID);

    CARAPI GetIpTableProtectSetting(
        /* [out] */ Byte* setting);

    CARAPI SetIpTableProtectSetting(
        /* [in] */ Byte ipTableProtectSetting);

    CARAPI GetIccAccessSetting(
        /* [out] */ Byte* setting);

    CARAPI SetIccAccessSetting(
        /* [in] */ Byte iccAccessSetting);

    CARAPI GetAddOnManagementSetting(
        /* [out] */ Byte* setting);

    CARAPI SetAddOnManagementSetting(
        /* [in] */ Byte addOnManagementSetting);

    CARAPI GetSmsSendSetting(
        /* [out] */ Byte* setting);

    CARAPI SetSmsSendSetting(
        /* [in] */ Byte smsSendSetting);

    CARAPI GetPhoneCallSetting(
        /* [out] */ Byte* setting);

    CARAPI SetPhoneCallSetting(
        /* [in] */ Byte phoneCallSetting);

    CARAPI GetRecordAudioSetting(
        /* [out] */ Byte* setting);

    CARAPI SetRecordAudioSetting(
        /* [in] */ Byte recordAudioSetting);

    CARAPI GetCameraSetting(
        /* [out] */ Byte* setting);

    CARAPI SetCameraSetting(
        /* [in] */ Byte cameraSetting);

    CARAPI GetId(
        /* [out] */ Int32*);

    CARAPI GetPackageName(
        /* [out] */ String* packageName);

    CARAPI SetPackageName(
        /* [in] */ const String& packageName);

    CARAPI GetUid(
        /* [out] */ Int32* uid);

    CARAPI SetUid(
        /* [in] */ Int32 uid);

    CARAPI GetDeviceIdSetting(
        /* [out] */ Byte* setting);

    CARAPI SetDeviceIdSetting(
        /* [in] */ Byte deviceIdSetting);

    CARAPI GetDeviceId(
        /* [out] */ String* id);

    CARAPI SetDeviceId(
        /* [in] */ const String& deviceId);

    CARAPI GetLine1NumberSetting(
        /* [out] */ Byte* setting);

    CARAPI SetLine1NumberSetting(
        /* [in] */ Byte line1NumberSetting);

    CARAPI GetLine1Number(
        /* [out] */ String* number);

    CARAPI SetLine1Number(
        /* [in] */ const String& line1Number);

    CARAPI GetLocationGpsSetting(
        /* [out] */ Byte* setting);

    CARAPI SetLocationGpsSetting(
        /* [in] */ Byte locationGpsSetting);

    CARAPI GetLocationGpsLat(
        /* [out] */ String* lat);

    CARAPI SetLocationGpsLat(
        /* [in] */ const String& locationGpsLat);

    CARAPI GetLocationGpsLon(
        /* [out] */ String* lng);

    CARAPI SetLocationGpsLon(
        /* [in] */ const String& locationGpsLon);

    CARAPI GetLocationNetworkSetting(
        /* [out] */ Byte* setting);

    CARAPI SetLocationNetworkSetting(
        /* [in] */ Byte locationNetworkSetting);

    CARAPI GetLocationNetworkLat(
        /* [out] */ String* lat);

    CARAPI SetLocationNetworkLat(
        /* [in] */ const String& locationNetworkLat);

    CARAPI GetLocationNetworkLon(
        /* [out] */ String* lng);

    CARAPI SetLocationNetworkLon(
        /* [in] */ const String& locationNetworkLon);

    CARAPI GetNetworkInfoSetting(
        /* [out] */ Byte* setting);

    CARAPI SetNetworkInfoSetting(
        /* [in] */ Byte networkInfoSetting);

    CARAPI GetSimInfoSetting(
        /* [out] */ Byte* setting);

    CARAPI SetSimInfoSetting(
        /* [in] */ Byte simInfoSetting);

    CARAPI GetSimSerialNumberSetting(
        /* [out] */ Byte* setting);

    CARAPI SetSimSerialNumberSetting(
        /* [in] */ Byte simSerialNumberSetting);

    CARAPI GetSimSerialNumber(
        /* [out] */ String* number);

    CARAPI SetSimSerialNumber(
        /* [in] */ const String& simSerialNumber);

    CARAPI GetSubscriberIdSetting(
        /* [out] */ Byte* setting);

    CARAPI SetSubscriberIdSetting(
        /* [in] */ Byte subscriberIdSetting);

    CARAPI GetSubscriberId(
        /* [out] */ String* id);

    CARAPI SetSubscriberId(
        /* [in] */ const String& subscriberId);

    CARAPI GetAccountsSetting(
        /* [out] */ Byte* setting);

    CARAPI SetAccountsSetting(
        /* [in] */ Byte accountsSetting);

    CARAPI GetAccountsAuthTokensSetting(
        /* [out] */ Byte* setting);

    CARAPI SetAccountsAuthTokensSetting(
        /* [in] */ Byte accountsAuthTokensSetting);

    CARAPI GetOutgoingCallsSetting(
        /* [out] */ Byte* setting);

    CARAPI SetOutgoingCallsSetting(
        /* [in] */ Byte outgoingCallsSetting);

    CARAPI GetIncomingCallsSetting(
        /* [out] */ Byte* setting);

    CARAPI SetIncomingCallsSetting(
        /* [in] */ Byte incomingCallsSetting);

    CARAPI GetContactsSetting(
        /* [out] */ Byte* setting);

    CARAPI SetContactsSetting(
        /* [in] */ Byte setting);

    CARAPI GetCalendarSetting(
        /* [out] */ Byte* setting);

    CARAPI SetCalendarSetting(
        /* [in] */ Byte calendarSetting);

    CARAPI GetMmsSetting(
        /* [out] */ Byte* setting);

    CARAPI SetMmsSetting(
        /* [in] */ Byte mmsSetting);

    CARAPI GetSmsSetting(
        /* [out] */ Byte* setting);

    CARAPI SetSmsSetting(
        /* [in] */ Byte smsSetting);

    CARAPI GetCallLogSetting(
        /* [out] */ Byte* setting);

    CARAPI SetCallLogSetting(
        /* [in] */ Byte callLogSetting);

    CARAPI GetBookmarksSetting(
        /* [out] */ Byte* setting);

    CARAPI SetBookmarksSetting(
        /* [in] */ Byte bookmarksSetting);

    CARAPI GetSystemLogsSetting(
        /* [out] */ Byte* setting);

    CARAPI SetSystemLogsSetting(
        /* [in] */ Byte systemLogsSetting);

    CARAPI GetIntentBootCompletedSetting(
        /* [out] */ Byte* setting);

    CARAPI SetIntentBootCompletedSetting(
        /* [in] */ Byte intentBootCompletedSetting);

    CARAPI GetNotificationSetting(
        /* [out] */ Byte* setting);

    CARAPI SetNotificationSetting(
        /* [in] */ Byte notificationSetting);

    CARAPI GetAllowedContacts(
        /* [out, callee] */ ArrayOf<Int32>** contacts);

    CARAPI SetAllowedContacts(
        /* [in] */ ArrayOf<Int32>* allowedContacts);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 id,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid);

    CARAPI constructor(
        /* [in] */ Int32 id,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean allEmpty);

public:
    /**
        * Real value, provided by the unmodified Android framework.
        */
       static const Byte REAL = 0;

       /**
        * Empty or unavailable, depending on setting type. For String settings, it is
        * setter method caller's responsibility to make sure that the corresponding
        * setting field will contain an empty String.
        */
       static const Byte EMPTY = 1;

       /**
        * Custom specified output, appropriate for relevant setting. For String settings,
        * it is setter method caller's responsibility to make sure that the corresponding
        * setting field will contain a custom String.
        */
       static const Byte CUSTOM = 2;

       /**
        * Random output, appropriate for relevant setting. When this option is set, the
        * corresponding getter methods will generate appropriate random values automatically.
        *
        * Device ID: a random string consisting of 15 numeric digits preceded by a "+"
        * Line1Number: a random string consisting of 13 numeric digits
        */
       static const Byte RANDOM = 3;

       static const Byte SETTING_NOTIFY_OFF = 0;
       static const Byte SETTING_NOFITY_ON = 1;

       /** used to create random android ID*/
       static const String ID_PATTERN[];

       // constants for identification of data types transmitted in the notification intent
       static const String DATA_DEVICE_ID;
       static const String DATA_LINE_1_NUMBER;
       static const String DATA_LOCATION_GPS;
       static const String DATA_LOCATION_NETWORK;
       static const String DATA_NETWORK_INFO_CURRENT;
       static const String DATA_NETWORK_INFO_SIM;
       static const String DATA_SIM_SERIAL;
       static const String DATA_SUBSCRIBER_ID;
       static const String DATA_ACCOUNTS_LIST;
       static const String DATA_AUTH_TOKENS;
       static const String DATA_OUTGOING_CALL;
       static const String DATA_INCOMING_CALL;
       static const String DATA_CONTACTS;
       static const String DATA_CALENDAR;
       static const String DATA_MMS;
       static const String DATA_SMS;
       static const String DATA_MMS_SMS;
       static const String DATA_CALL_LOG;
       static const String DATA_BOOKMARKS;
       static const String DATA_SYSTEM_LOGS;
       static const String DATA_INTENT_BOOT_COMPLETED;
       //     static const String DATA_EXTERNAL_STORAGE = "externalStorage";
       static const String DATA_CAMERA;
       static const String DATA_RECORD_AUDIO;
       static const String DATA_SMS_SEND;
       static const String DATA_PHONE_CALL;
       static const String DATA_ANDROID_ID;
       static const String DATA_ICC_ACCESS;
       static const String DATA_WIFI_INFO;
       static const String DATA_IP_TABLES;
       static const String DATA_SWITCH_CONNECTIVITY;
       static const String DATA_SEND_MMS;
       static const String DATA_SWITCH_WIFI_STATE;

private:
    CARAPI_(String) GetRandomCoordinate(
        /* [in] */ Int32 limit);

    CARAPI_(String) GetRandomLat();

    CARAPI_(String) GetRandomLon();

private:
    // Database entry ID
       Int32 _id;

       // Application identifiers
       String packageName;
       Int32 uid;

       //
       // Privacy settings
       //

       Byte deviceIdSetting;
       String deviceId;

       // Phone and Voice Mailbox Number
       Byte line1NumberSetting;
       String line1Number;

       Byte locationGpsSetting;
       String locationGpsLat;
       String locationGpsLon;
       Byte locationNetworkSetting;
       String locationNetworkLat;
       String locationNetworkLon;

       // CountryIso, Operator Code, Operator Name
       Byte networkInfoSetting;
       Byte simInfoSetting;

       Byte simSerialNumberSetting;
       String simSerialNumber;
       Byte subscriberIdSetting;
       String subscriberId;

       Byte accountsSetting;
       Byte accountsAuthTokensSetting;
       Byte outgoingCallsSetting;
       Byte incomingCallsSetting;

       Byte contactsSetting;
       Byte calendarSetting;
       Byte mmsSetting;
       Byte smsSetting;
       Byte callLogSetting;
       Byte bookmarksSetting;// browser bookmarks and history

       Byte systemLogsSetting;

       Byte notificationSetting;

       Byte intentBootCompletedSetting;
       Byte externalStorageSetting;
       Byte cameraSetting;
       Byte recordAudioSetting;
       Byte smsSendSetting;
       Byte phoneCallSetting;

       Byte ipTableProtectSetting;
       Byte iccAccessSetting;
       Byte addOnManagementSetting;

       Byte androidIdSetting;
       String androidID;

       Byte wifiInfoSetting;

       Byte switchConnectivitySetting;

       Byte sendMmsSetting;

       Byte forceOnlineState;//used to fake online state

       Byte switchWifiStateSetting;

       AutoPtr< ArrayOf<Int32> > allowedContacts;

};

}
}
}

#endif // __ELASTOS_DROID_PRIVACY_CPRIVACYSETTINGS_H__
