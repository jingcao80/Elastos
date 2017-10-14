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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMAPHONE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMAPHONE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/os/Registrant.h"
#include "elastos/droid/os/RegistrantList.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"

// import android.app.ActivityManagerNative;
// import android.content.ContentValues;
// import android.content.Intent;
// import android.content.SharedPreferences;
// import android.database.SQLException;
// import android.net.Uri;
// import android.os.AsyncResult;
//import android.os.PowerManager;
// import android.os.SystemProperties;
// import android.os.UserHandle;
// import android.preference.PreferenceManager;
// import android.provider.Settings;
// import android.provider.Telephony;
// import android.telephony.CellLocation;
// import android.telephony.PhoneNumberUtils;
// import android.telephony.ServiceState;
// import android.telephony.SubscriptionManager;
// import android.telephony.cdma.CdmaCellLocation;
// import android.text.TextUtils;
// import android.telephony.Rlog;
// import com.android.ims.ImsManager;
// import com.android.internal.telephony.CallStateException;
// import com.android.internal.telephony.CallTracker;
// import com.android.internal.telephony.CommandException;
// import com.android.internal.telephony.CommandsInterface;
// import com.android.internal.telephony.IccPhoneBookInterfaceManager;
// import com.android.internal.telephony.MccTable;
// import com.android.internal.telephony.MmiCode;
// import com.android.internal.telephony.PhoneConstants;
// import com.android.internal.telephony.PhoneFactory;
// import com.android.internal.telephony.PhoneProxy;
// import com.android.internal.telephony.SubInfoRecordUpdater;
// import com.android.internal.telephony.SubscriptionController;
// import com.android.internal.telephony.TelephonyIntents;
// import com.android.internal.telephony.TelephonyProperties;
// import com.android.internal.telephony.dataconnection.DcTracker;
// import com.android.internal.telephony.imsphone.ImsPhone;
// import com.android.internal.telephony.uicc.IccException;
// import com.android.internal.telephony.uicc.IccRecords;
// import com.android.internal.telephony.uicc.RuimRecords;
// import com.android.internal.telephony.uicc.UiccCard;
// import com.android.internal.telephony.uicc.UiccController;
// import java.util.regex.Matcher;
// import static com.android.internal.telephony.TelephonyProperties.PROPERTY_ICC_OPERATOR_ALPHA;
// import static com.android.internal.telephony.TelephonyProperties.PROPERTY_ICC_OPERATOR_ISO_COUNTRY;
// import static com.android.internal.telephony.TelephonyProperties.PROPERTY_ICC_OPERATOR_NUMERIC;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::PhoneBase;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState;
using Elastos::Droid::Internal::Telephony::IPhoneDataActivityState;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IPhoneNotifier;
using Elastos::Droid::Internal::Telephony::ICallTracker;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IServiceStateTracker;
using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IUUSInfo;
using Elastos::Droid::Internal::Telephony::IOperatorInfo;
using Elastos::Droid::Internal::Telephony::IPhoneSubInfo;
using Elastos::Droid::Internal::Telephony::IIccPhoneBookInterfaceManager;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::Registrant;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

/**
  * {@hide}
  */
class CDMAPhone
    : public PhoneBase
    , public ICDMAPhone
{
private:
    class InnerRunnable
        : public Runnable
    {
    public:
        InnerRunnable(
            /* [in] */ CDMAPhone* owner);

        // @Override
        CARAPI Run();

    private:
        CDMAPhone* mOwner;
    };

public:
    TO_STRING_IMPL("CDMAPhone")

    CAR_INTERFACE_DECL();

    CDMAPhone();

    virtual ~CDMAPhone();

    // Constructors
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IPhoneNotifier* notifier);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IPhoneNotifier* notifier,
        /* [in] */ Int32 phoneId);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IPhoneNotifier* notifier,
        /* [in] */ Boolean unitTestMode);

    // @Override
    CARAPI Dispose();

    // @Override
    CARAPI RemoveReferences();

    // @Override
    CARAPI GetServiceState(
        /* [out] */ IServiceState** result);

    // @Override
    CARAPI GetCallTracker(
        /* [out] */ ICallTracker** result);

    // @Override
    CARAPI GetState(
        /* [out] */ PhoneConstantsState* result);

    // @Override
    CARAPI GetServiceStateTracker(
        /* [out] */ IServiceStateTracker** result);

    // @Override
    CARAPI GetPhoneType(
        /* [out] */ Int32* result);

    // @Override
    CARAPI CanTransfer(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetRingingCall(
        /* [out] */ ICall** result);

    // @Override
    CARAPI SetUiTTYMode(
        /* [in] */ Int32 uiTtyMode,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetMute(
        /* [in] */ Boolean muted);

    // @Override
    CARAPI GetMute(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI Conference();

    // @Override
    CARAPI EnableEnhancedVoicePrivacy(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI GetEnhancedVoicePrivacy(
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI ClearDisconnected();

    // @Override
    CARAPI GetDataActivityState(
        /* [out] */ IPhoneDataActivityState* result);

    // @Override
    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 videoState,
        /* [out] */ IConnection** result);

    // @Override
    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 videoState,
        /* [in] */ IBundle* extras,
        /* [out] */ IConnection** result);

    virtual CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ IUUSInfo* uusInfo,
        /* [in] */ Int32 videoState,
        /* [in] */ IBundle* extras,
        /* [out] */ IConnection** result);

    // @Override
    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ IUUSInfo* uusInfo,
        /* [in] */ Int32 videoState,
        /* [out] */ IConnection** result);

    // @Override IList List<? extends MmiCode>
    CARAPI GetPendingMmiCodes(
        /* [out] */ IList** result);

    // @Override
    CARAPI RegisterForSuppServiceNotification(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI GetBackgroundCall(
        /* [out] */ ICall** result);

    // @Override
    CARAPI HandleInCallMmiCommands(
        /* [in] */ const String& dialString,
        /* [out] */ Boolean* result);

    virtual CARAPI IsInCall(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetNetworkSelectionModeAutomatic(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI UnregisterForSuppServiceNotification(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI AcceptCall(
        /* [in] */ Int32 videoState);

    // @Override
    CARAPI DeflectCall(
        /* [in] */ const String& number);

    // @Override
    CARAPI RejectCall();

    // @Override
    CARAPI SwitchHoldingAndActive();

    // @Override
    CARAPI GetIccSerialNumber(
        /* [out] */ String* result);

    // @Override
    CARAPI GetLine1Number(
        /* [out] */ String* result);

    // @Override
    CARAPI GetCdmaPrlVersion(
        /* [out] */ String* result);

    // @Override
    CARAPI GetCdmaMin(
        /* [out] */ String* result);

    // @Override
    CARAPI IsMinInfoReady(
        /* [out] */ Boolean* resutl);

    // @Override
    CARAPI GetCallWaiting(
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetRadioPower(
        /* [in] */ Boolean power);

    // @Override
    CARAPI GetEsn(
        /* [out] */ String* result);

    // @Override
    CARAPI GetMeid(
        /* [out] */ String* result);

    //returns MEID or ESN in CDMA
    // @Override
    CARAPI GetDeviceId(
        /* [out] */ String* result);

    // @Override
    CARAPI GetDeviceSvn(
        /* [out] */ String* result);

    // @Override
    CARAPI GetSubscriberId(
        /* [out] */ String* result);

    // @Override
    CARAPI GetGroupIdLevel1(
        /* [out] */ String* result);

    // @Override
    CARAPI GetImei(
        /* [out] */ String* result);

    // @Override
    CARAPI CanConference(
        /* [out] */ Boolean* resutl);

    // @Override
    CARAPI GetCellLocation(
        /* [out] */ ICellLocation** result);

    // @Override
    CARAPI GetForegroundCall(
        /* [out] */ ICall** result);

    // @Override
    CARAPI SelectNetworkManually(
        /* [in] */ IOperatorInfo* network,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetOnPostDialCharacter(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI HandlePinMmi(
        /* [in] */ const String& dialString,
        /* [out] */ Boolean* result);

    /**
      * Removes the given MMI from the pending list and notifies registrants that
      * it is complete.
      *
      * @param mmi MMI that is done
      */
    virtual CARAPI OnMMIDone(
        /* [in] */ ICdmaMmiCode* mmi);

    // @Override
    CARAPI SetLine1Number(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& number,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetCallWaiting(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI UpdateServiceLocation();

    // @Override
    CARAPI SetDataRoamingEnabled(
        /* [in] */ Boolean enable);

    // @Override
    CARAPI RegisterForCdmaOtaStatusChange(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForCdmaOtaStatusChange(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForSubscriptionInfoReady(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForSubscriptionInfoReady(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnEcbModeExitResponse(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnsetOnEcbModeExitResponse(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForCallWaiting(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForCallWaiting(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI GetNeighboringCids(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetDataConnectionState(
        /* [in] */ const String& apnType,
        /* [out] */ PhoneConstantsState* result);

    // @Override
    CARAPI SendUssdResponse(
        /* [in] */ const String& ussdMessge);

    // @Override
    CARAPI SendDtmf(
        /* [in] */ Char32 c);

    // @Override
    CARAPI StartDtmf(
        /* [in] */ Char32 c);

    // @Override
    CARAPI StopDtmf();

    // @Override
    CARAPI SendBurstDtmf(
        /* [in] */ const String& dtmfString,
        /* [in] */ Int32 on,
        /* [in] */ Int32 off,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI GetAvailableNetworks(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetOutgoingCallerIdDisplay(
        /* [in] */ Int32 commandInterfaceCLIRMode,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI EnableLocationUpdates();

    // @Override
    CARAPI DisableLocationUpdates();

    // @Override
    CARAPI GetDataCallList(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetDataRoamingEnabled(
        /* [out] */ Boolean* resutl);

    // @Override
    CARAPI SetDataEnabled(
        /* [in] */ Boolean enable);

    // @Override
    CARAPI GetDataEnabled(
        /* [out] */ Boolean* resutl);

    // @Override
    CARAPI SetVoiceMailNumber(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& voiceMailNumber,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI GetVoiceMailNumber(
        /* [out] */ String* result);

    // @Override
    CARAPI GetVoiceMailAlphaTag(
        /* [out] */ String* result);

    // @Override
    CARAPI GetCallForwardingOption(
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetCallForwardingOption(
        /* [in] */ Int32 commandInterfaceCFAction,
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ const String& dialingNumber,
        /* [in] */ Int32 timerSeconds,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI GetCallForwardingUncondTimerOption(
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetCallForwardingUncondTimerOption(
        /* [in] */ Int32 startHour,
        /* [in] */ Int32 startMinute,
        /* [in] */ Int32 endHour,
        /* [in] */ Int32 endMinute,
        /* [in] */ Int32 commandInterfaceCFAction,
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ const String& dialingNumber,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI GetOutgoingCallerIdDisplay(
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI GetCallForwardingIndicator(
        /* [out] */ Boolean* resutl);

    // @Override
    CARAPI ExplicitCallTransfer();

    // @Override
    CARAPI GetLine1AlphaTag(
        /* [out] */ String* result);

    /**
      * Notify any interested party of a Phone state change
      * {@link com.android.internal.telephony.PhoneConstants.State}
      */
    /*package*/
    virtual CARAPI NotifyPhoneStateChanged();

    /**
      * Notify registrants of a change in the call state. This notifies changes in
      * {@link com.android.internal.telephony.Call.State}. Use this when changes
      * in the precise call state are needed, else use notifyPhoneStateChanged.
      */
    /*package*/
    virtual CARAPI NotifyPreciseCallStateChanged();

    virtual CARAPI NotifyServiceStateChanged(
        /* [in] */ IServiceState* ss);

    virtual CARAPI NotifyLocationChanged();

    virtual CARAPI NotifyNewRingingConnection(
        /* [in] */ IConnection* c);

    /*package*/
    virtual CARAPI NotifyDisconnect(
        /* [in] */ IConnection* cn);

    virtual CARAPI NotifyUnknownConnection(
        /* [in] */ IConnection* connection);

    // @Override
    CARAPI IsInEmergencyCall(
        /* [out] */ Boolean* resutl);

    // @Override
    CARAPI IsInEcm(
        /* [out] */ Boolean* resutl);

    virtual CARAPI SendEmergencyCallbackModeChange();

    // @Override
    CARAPI ExitEmergencyCallbackMode();

    /**
      * Handle to cancel or restart Ecm timer in emergency call back mode
      * if action is CANCEL_ECM_TIMER, cancel Ecm timer and notify apps the timer is canceled;
      * otherwise, restart Ecm timer and notify apps the timer is restarted.
      */
    virtual CARAPI HandleTimerInEmergencyCallbackMode(
        /* [in] */ Int32 action);

    virtual CARAPI NotifyEcbmTimerReset(
        /* [in] */ Boolean flag);

    /**
      * Registration point for Ecm timer reset
      * @param h handler to notify
      * @param what User-defined message code
      * @param obj placed in Message.obj
      */
    // @Override
    CARAPI RegisterForEcmTimerReset(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForEcmTimerReset(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    /**
      * Retrieves the PhoneSubInfo of the CDMAPhone
      */
    // @Override
    CARAPI GetPhoneSubInfo(
        /* [out] */ IPhoneSubInfo** result);

    /**
      * Retrieves the IccPhoneBookInterfaceManager of the CDMAPhone
      */
    // @Override
    CARAPI GetIccPhoneBookInterfaceManager(
        /* [out] */ IIccPhoneBookInterfaceManager** result);

    virtual CARAPI RegisterForEriFileLoaded(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForEriFileLoaded(
        /* [in] */ IHandler* h);

    // override for allowing access from other classes of this package
    /**
      * {@inheritDoc}
      */
    // @Override
    CARAPI SetSystemProperty(
        /* [in] */ const String& property,
        /* [in] */ const String& value);

    // override for allowing access from other classes of this package
    /**
      * {@inheritDoc}
      */
    // @Override
    CARAPI GetSystemProperty(
        /* [in] */ const String& property,
        /* [in] */ const String& defValue,
        /* [out] */ String* result);

    /**
      * Activate or deactivate cell broadcast SMS.
      *
      * @param activate 0 = activate, 1 = deactivate
      * @param response Callback message is empty on completion
      */
    // @Override
    CARAPI ActivateCellBroadcastSms(
        /* [in] */ Int32 activate,
        /* [in] */ IMessage* response);

    /**
      * Query the current configuration of cdma cell broadcast SMS.
      *
      * @param response Callback message is empty on completion
      */
    // @Override
    CARAPI GetCellBroadcastSmsConfig(
        /* [in] */ IMessage* response);

    /**
      * Configure cdma cell broadcast SMS.
      *
      * @param response Callback message is empty on completion
      */
    // @Override
    CARAPI SetCellBroadcastSmsConfig(
        /* [in] */ ArrayOf<Int32>* configValuesArray,
        /* [in] */ IMessage* response);

    /**
      * Returns true if OTA Service Provisioning needs to be performed.
      */
    // @Override
    CARAPI NeedsOtaServiceProvisioning(
        /* [out] */ Boolean* resutl);

    /**
      * isOTASPNumber: checks a given number against the IS-683A OTASP dial string and carrier
      * OTASP dial string.
      *
      * @param dialStr the number to look up.
      * @return true if the number is in IS-683A OTASP dial string or carrier OTASP dial string
      */
    // @Override
    CARAPI IsOtaSpNumber(
        /* [in] */ const String& dialStr,
        /* [out] */ Boolean* resutl);

    // @Override
    CARAPI GetCdmaEriIconIndex(
        /* [out] */ Int32* result);

    /**
      * Returns the CDMA ERI icon mode,
      * 0 - ON
      * 1 - FLASHING
      */
    // @Override
    CARAPI GetCdmaEriIconMode(
        /* [out] */ Int32* result);

    /**
      * Returns the CDMA ERI text,
      */
    // @Override
    CARAPI GetCdmaEriText(
        /* [out] */ String* result);

    /**
      * Sets the "current" field in the telephony provider according to the
      * build-time operator numeric property
      *
      * @return true for success; false otherwise.
      */
    virtual CARAPI UpdateCurrentCarrierInProvider(
        /* [in] */ const String& operatorNumeric,
        /* [out] */ Boolean* result);

    /**
      * Sets the "current" field in the telephony provider according to the SIM's operator.
      * Implemented in {@link CDMALTEPhone} for CDMA/LTE devices.
      *
      * @return true for success; false otherwise.
      */
    virtual CARAPI UpdateCurrentCarrierInProvider(
        /* [out] */ Boolean* result);

    virtual CARAPI PrepareEri();

    virtual CARAPI IsEriFileLoaded(
        /* [out] */ Boolean* result);

    /**
      * Sets the SIM voice message count
      * @param line Subscriber Profile Number, one-based. Only '1' is supported
      * @param countWaiting The number of messages waiting, if known. Use
      *                     -1 to indicate that an unknown number of
      *                      messages are waiting
      * This is a wrapper function for setVoiceMessageCount
      */
    // @Override
    CARAPI SetVoiceMessageWaiting(
        /* [in] */ Int32 line,
        /* [in] */ Int32 countWaiting);

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    // @Override
    CARAPI SetOperatorBrandOverride(
        /* [in] */ const String& brand,
        /* [out] */ Boolean* resutl);

protected:
    virtual CARAPI_(void) InitSstIcc();

    virtual CARAPI_(void) Init(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneNotifier* notifier);

    // @Override
    CARAPI_(void) Finalize();

    // @Override
    CARAPI DialInternal(
        /* [in] */ const String& dialString,
        /* [in] */ IUUSInfo* uusInfo,
        /* [in] */ Int32 videoState,
        /* [out] */ IConnection** connection);

    virtual CARAPI_(AutoPtr<IUiccCardApplication>) GetUiccCardApplication();

    // Set the Card into the Phone Book.
    // @Override
    CARAPI_(void) SetCardInPhoneBook();

    // @Override
    CARAPI OnUpdateIccAvailability();

    /**
      * Sets PROPERTY_ICC_OPERATOR_ISO_COUNTRY property
      *
      */
    virtual CARAPI_(void) SetIsoCountryProperty(
        /* [in] */ const String& operatorNumeric);

    virtual CARAPI_(void) RegisterForRuimRecordEvents();

    virtual CARAPI_(void) UnregisterForRuimRecordEvents();

    virtual CARAPI_(void) Log(
        /* [in] */ const String& s);

    //virtual CARAPI_(void) Loge(
    //    /* [in] */ const String& s,
    //    /* [in] */ Exception* e);

private:
    // pending voice mail count updated after phone creation
    CARAPI_(void) UpdateVoiceMail();

    /** gets the voice mail count from preferences */
    CARAPI_(Int32) GetStoredVoiceMessageCount();

    CARAPI_(void) HandleEnterEmergencyCallbackMode(
        /* [in] */ IMessage* msg);

    CARAPI_(void) HandleExitEmergencyCallbackMode(
        /* [in] */ IMessage* msg);

    /**
      * Handles the call to get the subscription source
      *
      * @param newSubscriptionSource holds the new CDMA subscription source value
      */
    CARAPI_(void) HandleCdmaSubscriptionSource(
        /* [in] */ Int32 newSubscriptionSource);

    static CARAPI_(Boolean) IsIs683OtaSpDialStr(
        /* [in] */ const String& dialStr);

    /**
      * This function extracts the system selection code from the dial string.
      */
    static CARAPI_(Int32) ExtractSelCodeFromOtaSpNum(
        /* [in] */ const String& dialStr);

    /**
      * This function checks if the system selection code extracted from
      * the dial string "sysSelCodeInt' is the system selection code specified
      * in the carrier ota sp number schema "sch".
      */
    static CARAPI_(Boolean) CheckOtaSpNumBasedOnSysSelCode(
        /* [in] */ Int32 sysSelCodeInt,
        /* [in] */ ArrayOf<String>* sch);

    /**
      * The following function checks if a dial string is a carrier specified
      * OTASP number or not by checking against the OTASP number schema stored
      * in PROPERTY_OTASP_NUM_SCHEMA.
      *
      * Currently, there are 2 schemas for carriers to specify the OTASP number:
      * 1) Use system selection code:
      *    The schema is:
      *    SELC,the # of code pairs,min1,max1,min2,max2,...
      *    e.g "SELC,3,10,20,30,40,60,70" indicates that there are 3 pairs of
      *    selection codes, and they are {10,20}, {30,40} and {60,70} respectively.
      *
      * 2) Use feature code:
      *    The schema is:
      *    "FC,length of feature code,feature code".
      *     e.g "FC,2,*2" indicates that the length of the feature code is 2,
      *     and the code itself is "*2".
      */
    CARAPI_(Boolean) IsCarrierOtaSpNum(
        /* [in] */ const String& dialStr);

    /**
      * Store the voicemail number in preferences
      */
    CARAPI_(void) StoreVoiceMailNumber(
        /* [in] */ const String& number);

public:
    static const String LOGTAG;
    static const Int32 RESTART_ECM_TIMER = 0;
    // restart Ecm timer
    static const Int32 CANCEL_ECM_TIMER = 1;
    // cancel Ecm timer
    // Instance Variables
    AutoPtr<ICdmaCallTracker> mCT;
    AutoPtr<ICdmaServiceStateTracker> mSST;
    AutoPtr<ICdmaSubscriptionSourceManager> mCdmaSSM;
    AutoPtr<IArrayList> mPendingMmis; // CdmaMmiCode
    AutoPtr<IRuimPhoneBookInterfaceManager> mRuimPhoneBookInterfaceManager;
    Int32 mCdmaSubscriptionSource;
    AutoPtr<IPhoneSubInfo> mSubInfo;
    AutoPtr<IEriManager> mEriManager;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    AutoPtr<IRegistrant> mPostDialHandler;

protected:
    // mEcmExitRespRegistrant is informed after the phone has been exited
    //the emergency callback mode
    //keep track of if phone is in emergency callback mode
    Boolean mIsPhoneInEcmState;
    String mImei;
    String mImeiSv;
    // string to define how the carrier specifies its own ota sp number
    String mCarrierOtaSpNumSchema;

private:
    static const Boolean DBG;
    static const Boolean VDBG;
    /* STOP SHIP if true */
    // Default Emergency Callback Mode exit timer
    static const Int32 DEFAULT_ECM_EXIT_TIMER_VALUE = 300000;
    static const String VM_NUMBER_CDMA;
    String mVmNumber;
    // mEriFileLoadedRegistrants are informed after the ERI text has been loaded
    AutoPtr<RegistrantList> mEriFileLoadedRegistrants;
    // mEcmTimerResetRegistrants are informed after Ecm timer is canceled or re-started
    AutoPtr<RegistrantList> mEcmTimerResetRegistrants;
    AutoPtr<IRegistrant> mEcmExitRespRegistrant;
    String mEsn;
    String mMeid;
    // A runnable which is used to automatically exit from Ecm after a period of time.
    AutoPtr<Runnable> mExitEcmRunnable;
    static const String IS683A_FEATURE_CODE;
    static const Int32 IS683A_FEATURE_CODE_NUM_DIGITS = 4;
    static const Int32 IS683A_SYS_SEL_CODE_NUM_DIGITS = 2;
    static const Int32 IS683A_SYS_SEL_CODE_OFFSET = 4;
    static const Int32 IS683_CONST_800MHZ_A_BAND = 0;
    static const Int32 IS683_CONST_800MHZ_B_BAND = 1;
    static const Int32 IS683_CONST_1900MHZ_A_BLOCK = 2;
    static const Int32 IS683_CONST_1900MHZ_B_BLOCK = 3;
    static const Int32 IS683_CONST_1900MHZ_C_BLOCK = 4;
    static const Int32 IS683_CONST_1900MHZ_D_BLOCK = 5;
    static const Int32 IS683_CONST_1900MHZ_E_BLOCK = 6;
    static const Int32 IS683_CONST_1900MHZ_F_BLOCK = 7;
    static const Int32 INVALID_SYSTEM_SELECTION_CODE = -1;
    // Define the pattern/format for carrier specified OTASP number schema.
    // It separates by comma and/or whitespace.
    static AutoPtr<IPattern> pOtaSpNumSchema;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMAPHONE_H__
