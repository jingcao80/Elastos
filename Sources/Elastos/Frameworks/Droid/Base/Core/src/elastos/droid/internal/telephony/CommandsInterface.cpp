/*
 * Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 *
 * Copyright (C) 2006 The Android Open Source Project
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

package com.android.internal.telephony;

using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSmsBroadcastConfigInfo;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDataProfile;
using Elastos::Droid::Internal::Telephony::Gsm::ISmsBroadcastConfigInfo;
using Elastos::Droid::Internal::Telephony::Uicc::IIccCardStatus;

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandler;

/**
 * {@hide}
 */
public interface CommandsInterface {
    enum RadioState {
        RADIO_OFF,         /* Radio explicitly powered Off (eg CFUN=0) */
        RADIO_UNAVAILABLE, /* Radio Unavailable (eg, resetting or not booted) */
        RADIO_ON;          /* Radio is on */

        public Boolean IsOn() /* and available...*/ {
            return this == RADIO_ON;
        }

        public Boolean IsAvailable() {
            return this != RADIO_UNAVAILABLE;
        }
    }

    //***** Constants

    // Used as parameter to Dial() and SetCLIR() below
    static const Int32 CLIR_DEFAULT = 0;      // "use subscription default value"
    static const Int32 CLIR_INVOCATION = 1;   // (restrict CLI presentation)
    static const Int32 CLIR_SUPPRESSION = 2;  // (allow CLI presentation)


    // Used as parameters for call forward methods below
    static const Int32 CF_ACTION_DISABLE          = 0;
    static const Int32 CF_ACTION_ENABLE           = 1;
//  static const Int32 CF_ACTION_UNUSED           = 2;
    static const Int32 CF_ACTION_REGISTRATION     = 3;
    static const Int32 CF_ACTION_ERASURE          = 4;

    static const Int32 CF_REASON_UNCONDITIONAL    = 0;
    static const Int32 CF_REASON_BUSY             = 1;
    static const Int32 CF_REASON_NO_REPLY         = 2;
    static const Int32 CF_REASON_NOT_REACHABLE    = 3;
    static const Int32 CF_REASON_ALL              = 4;
    static const Int32 CF_REASON_ALL_CONDITIONAL  = 5;

    // Used for call barring methods below
    static const String CB_FACILITY_BAOC         = "AO";
    static const String CB_FACILITY_BAOIC        = "OI";
    static const String CB_FACILITY_BAOICxH      = "OX";
    static const String CB_FACILITY_BAIC         = "AI";
    static const String CB_FACILITY_BAICr        = "IR";
    static const String CB_FACILITY_BA_ALL       = "AB";
    static const String CB_FACILITY_BA_MO        = "AG";
    static const String CB_FACILITY_BA_MT        = "AC";
    static const String CB_FACILITY_BA_SIM       = "SC";
    static const String CB_FACILITY_BA_FD        = "FD";


    // Used for various supp services apis
    // See 27.007 +CCFC or +CLCK
    static const Int32 SERVICE_CLASS_NONE     = 0; // no user input
    static const Int32 SERVICE_CLASS_VOICE    = (1 << 0);
    static const Int32 SERVICE_CLASS_DATA     = (1 << 1); //synonym for 16+32+64+128
    static const Int32 SERVICE_CLASS_FAX      = (1 << 2);
    static const Int32 SERVICE_CLASS_SMS      = (1 << 3);
    static const Int32 SERVICE_CLASS_DATA_SYNC = (1 << 4);
    static const Int32 SERVICE_CLASS_DATA_ASYNC = (1 << 5);
    static const Int32 SERVICE_CLASS_PACKET   = (1 << 6);
    static const Int32 SERVICE_CLASS_PAD      = (1 << 7);
    static const Int32 SERVICE_CLASS_MAX      = (1 << 7); // Max SERVICE_CLASS value

    // Numeric representation of string values returned
    // by messages sent to setOnUSSD handler
    static const Int32 USSD_MODE_NOTIFY       = 0;
    static const Int32 USSD_MODE_REQUEST      = 1;

    // GSM SMS fail cause for acknowledgeLastIncomingSMS. From TS 23.040, 9.2.3.22.
    static const Int32 GSM_SMS_FAIL_CAUSE_MEMORY_CAPACITY_EXCEEDED    = 0xD3;
    static const Int32 GSM_SMS_FAIL_CAUSE_USIM_APP_TOOLKIT_BUSY       = 0xD4;
    static const Int32 GSM_SMS_FAIL_CAUSE_USIM_DATA_DOWNLOAD_ERROR    = 0xD5;
    static const Int32 GSM_SMS_FAIL_CAUSE_UNSPECIFIED_ERROR           = 0xFF;

    // CDMA SMS fail cause for acknowledgeLastIncomingCdmaSms.  From TS N.S0005, 6.5.2.125.
    static const Int32 CDMA_SMS_FAIL_CAUSE_INVALID_TELESERVICE_ID     = 4;
    static const Int32 CDMA_SMS_FAIL_CAUSE_RESOURCE_SHORTAGE          = 35;
    static const Int32 CDMA_SMS_FAIL_CAUSE_OTHER_TERMINAL_PROBLEM     = 39;
    static const Int32 CDMA_SMS_FAIL_CAUSE_ENCODING_PROBLEM           = 96;

    //***** Methods
    RadioState GetRadioState();

    /**
     * response.obj.result is an Int32[2]
     *
     * response.obj.result[0] is IMS registration state
     *                        0 - Not registered
     *                        1 - Registered
     * response.obj.result[1] is of type RILConstants.GSM_PHONE or
     *                                    RILConstants.CDMA_PHONE
     */
    void GetImsRegistrationState(Message result);

    /**
     * Fires on any RadioState transition
     * Always fires immediately as well
     *
     * do not attempt to calculate transitions by storing GetRadioState() values
     * on previous invocations of this notification. Instead, use the other
     * registration methods
     */
    void RegisterForRadioStateChanged(Handler h, Int32 what, Object obj);
    void UnregisterForRadioStateChanged(Handler h);

    void RegisterForVoiceRadioTechChanged(Handler h, Int32 what, Object obj);
    void UnregisterForVoiceRadioTechChanged(Handler h);
    void RegisterForImsNetworkStateChanged(Handler h, Int32 what, Object obj);
    void UnregisterForImsNetworkStateChanged(Handler h);

    /**
     * Fires on any transition into RadioState->IsOn()
     * Fires immediately if currently in that state
     * In general, actions should be idempotent. State may change
     * before event is received.
     */
    void RegisterForOn(Handler h, Int32 what, Object obj);
    void UnregisterForOn(Handler h);

    /**
     * Fires on any transition out of RadioState->IsAvailable()
     * Fires immediately if currently in that state
     * In general, actions should be idempotent. State may change
     * before event is received.
     */
    void RegisterForAvailable(Handler h, Int32 what, Object obj);
    void UnregisterForAvailable(Handler h);

    /**
     * Fires on any transition into !RadioState->IsAvailable()
     * Fires immediately if currently in that state
     * In general, actions should be idempotent. State may change
     * before event is received.
     */
    void RegisterForNotAvailable(Handler h, Int32 what, Object obj);
    void UnregisterForNotAvailable(Handler h);

    /**
     * Fires on any transition into RADIO_OFF or !RadioState->IsAvailable()
     * Fires immediately if currently in that state
     * In general, actions should be idempotent. State may change
     * before event is received.
     */
    void RegisterForOffOrNotAvailable(Handler h, Int32 what, Object obj);
    void UnregisterForOffOrNotAvailable(Handler h);

    /**
     * Fires on any change in ICC status
     */
    void RegisterForIccStatusChanged(Handler h, Int32 what, Object obj);
    void UnregisterForIccStatusChanged(Handler h);

    void RegisterForCallStateChanged(Handler h, Int32 what, Object obj);
    void UnregisterForCallStateChanged(Handler h);
    void RegisterForVoiceNetworkStateChanged(Handler h, Int32 what, Object obj);
    void UnregisterForVoiceNetworkStateChanged(Handler h);
    void RegisterForDataNetworkStateChanged(Handler h, Int32 what, Object obj);
    void UnregisterForDataNetworkStateChanged(Handler h);

    /** InCall voice privacy notifications */
    void RegisterForInCallVoicePrivacyOn(Handler h, Int32 what, Object obj);
    void UnregisterForInCallVoicePrivacyOn(Handler h);
    void RegisterForInCallVoicePrivacyOff(Handler h, Int32 what, Object obj);
    void UnregisterForInCallVoicePrivacyOff(Handler h);

    /** Single Radio Voice Call State progress notifications */
    void RegisterForSrvccStateChanged(Handler h, Int32 what, Object obj);
    void UnregisterForSrvccStateChanged(Handler h);

    /**
     * Handlers for subscription status change indications.
     *
     * @param h Handler for subscription status change messages.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForSubscriptionStatusChanged(Handler h, Int32 what, Object obj);
    void UnregisterForSubscriptionStatusChanged(Handler h);

    /**
     * fires on any change in hardware configuration.
     */
    void RegisterForHardwareConfigChanged(Handler h, Int32 what, Object obj);
    void UnregisterForHardwareConfigChanged(Handler h);

    /**
     * unlike the register* methods, there's only one new 3GPP format SMS handler.
     * if you need to unregister, you should also tell the radio to stop
     * sending SMS's to You (via AT+CNMI)
     *
     * AsyncResult.result is a String containing the SMS PDU
     */
    void SetOnNewGsmSms(Handler h, Int32 what, Object obj);
    void UnSetOnNewGsmSms(Handler h);

    /**
     * unlike the register* methods, there's only one new 3GPP2 format SMS handler.
     * if you need to unregister, you should also tell the radio to stop
     * sending SMS's to You (via AT+CNMI)
     *
     * AsyncResult.result is a String containing the SMS PDU
     */
    void SetOnNewCdmaSms(Handler h, Int32 what, Object obj);
    void UnSetOnNewCdmaSms(Handler h);

    /**
     * Set the handler for SMS Cell Broadcast messages.
     *
     * AsyncResult.result is a Byte array containing the SMS-CB PDU
     */
    void SetOnNewGsmBroadcastSms(Handler h, Int32 what, Object obj);
    void UnSetOnNewGsmBroadcastSms(Handler h);

    /**
     * Register for NEW_SMS_ON_SIM unsolicited message
     *
     * AsyncResult.result is an Int32 array containing the index of new SMS
     */
    void SetOnSmsOnSim(Handler h, Int32 what, Object obj);
    void UnSetOnSmsOnSim(Handler h);

    /**
     * Register for NEW_SMS_STATUS_REPORT unsolicited message
     *
     * AsyncResult.result is a String containing the status report PDU
     */
    void SetOnSmsStatus(Handler h, Int32 what, Object obj);
    void UnSetOnSmsStatus(Handler h);

    /**
     * unlike the register* methods, there's only one NITZ time handler
     *
     * AsyncResult.result is an Object[]
     * ((Object[])AsyncResult.result)[0] is a String containing the NITZ time string
     * ((Object[])AsyncResult.result)[1] is a Long containing the milliseconds since boot as
     *                                   returned by ElapsedRealtime() when this NITZ time
     *                                   was posted.
     *
     * Please note that the delivery of this message may be delayed several
     * seconds on system startup
     */
    void SetOnNITZTime(Handler h, Int32 what, Object obj);
    void UnSetOnNITZTime(Handler h);

    /**
     * unlike the register* methods, there's only one USSD notify handler
     *
     * Represents the arrival of a USSD "notify" message, which may
     * or may not have been triggered by a previous USSD send
     *
     * AsyncResult.result is a String[]
     * ((String[])(AsyncResult.result))[0] contains status code
     *      "0"   USSD-Notify -- text In ((const Char32 **)data)[1]
     *      "1"   USSD-Request -- text In ((const Char32 **)data)[1]
     *      "2"   Session terminated by network
     *      "3"   other local Client (eg, SIM Toolkit) has responded
     *      "4"   Operation not supported
     *      "5"   Network timeout
     *
     * ((String[])(AsyncResult.result))[1] contains the USSD message
     * The numeric representations of these are in USSD_MODE_*
     */

    void SetOnUSSD(Handler h, Int32 what, Object obj);
    void UnSetOnUSSD(Handler h);

    /**
     * unlike the register* methods, there's only one signal strength handler
     * AsyncResult.result is an Int32[2]
     * response.obj.result[0] is received signal Strength (0-31, 99)
     * response.obj.result[1] is  bit error Rate (0-7, 99)
     * as defined in TS 27.007 8.5
     */

    void SetOnSignalStrengthUpdate(Handler h, Int32 what, Object obj);
    void UnSetOnSignalStrengthUpdate(Handler h);

    /**
     * Sets the handler for SIM/RUIM SMS storage full unsolicited message.
     * Unlike the register* methods, there's only one notification handler
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void SetOnIccSmsFull(Handler h, Int32 what, Object obj);
    void UnSetOnIccSmsFull(Handler h);

    /**
     * Sets the handler for SIM Refresh notifications.
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForIccRefresh(Handler h, Int32 what, Object obj);
    void UnregisterForIccRefresh(Handler h);

    void SetOnIccRefresh(Handler h, Int32 what, Object obj);
    void UnsetOnIccRefresh(Handler h);

    /**
     * Sets the handler for RING notifications.
     * Unlike the register* methods, there's only one notification handler
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void SetOnCallRing(Handler h, Int32 what, Object obj);
    void UnSetOnCallRing(Handler h);

    /**
     * Sets the handler for RESTRICTED_STATE changed notification,
     * eg, for Domain Specific Access Control
     * unlike the register* methods, there's only one signal strength handler
     *
     * AsyncResult.result is an Int32[1]
     * response.obj.result[0] is a bitmask of RIL_RESTRICTED_STATE_* values
     */

    void SetOnRestrictedStateChanged(Handler h, Int32 what, Object obj);
    void UnSetOnRestrictedStateChanged(Handler h);

    /**
     * Sets the handler for Supplementary Service Notifications.
     * Unlike the register* methods, there's only one notification handler
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void SetOnSuppServiceNotification(Handler h, Int32 what, Object obj);
    void UnSetOnSuppServiceNotification(Handler h);

    /**
     * Sets the handler for Session End Notifications for CAT.
     * Unlike the register* methods, there's only one notification handler
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void SetOnCatSessionEnd(Handler h, Int32 what, Object obj);
    void UnSetOnCatSessionEnd(Handler h);

    /**
     * Sets the handler for Proactive Commands for CAT.
     * Unlike the register* methods, there's only one notification handler
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void SetOnCatProactiveCmd(Handler h, Int32 what, Object obj);
    void UnSetOnCatProactiveCmd(Handler h);

    /**
     * Sets the handler for Event Notifications for CAT.
     * Unlike the register* methods, there's only one notification handler
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void SetOnCatEvent(Handler h, Int32 what, Object obj);
    void UnSetOnCatEvent(Handler h);

    /**
     * Sets the handler for Call Set Up Notifications for CAT.
     * Unlike the register* methods, there's only one notification handler
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void SetOnCatCallSetUp(Handler h, Int32 what, Object obj);
    void UnSetOnCatCallSetUp(Handler h);

    /**
     * Enables/disbables supplementary service related notifications from
     * the network.
     *
     * @param enable TRUE to enable notifications, FALSE to disable.
     * @param result Message to be posted when command completes.
     */
    void SetSuppServiceNotifications(Boolean enable, Message result);
    //void UnSetSuppServiceNotifications(Handler h);

    /**
     * Sets the handler for Alpha Notification during STK Call Control.
     * Unlike the register* methods, there's only one notification handler
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void SetOnCatCcAlphaNotify(Handler h, Int32 what, Object obj);
    void UnSetOnCatCcAlphaNotify(Handler h);

    /**
     * Sets the handler for notifying Suplementary Services (SS)
     * Data during STK Call Control.
     * Unlike the register* methods, there's only one notification handler
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void SetOnSs(Handler h, Int32 what, Object obj);
    void UnSetOnSs(Handler h);

    /**
     * Sets the handler for Event Notifications for CDMA Display Info.
     * Unlike the register* methods, there's only one notification handler
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForDisplayInfo(Handler h, Int32 what, Object obj);
    void UnregisterForDisplayInfo(Handler h);

    /**
     * Sets the handler for Event Notifications for CallWaiting Info.
     * Unlike the register* methods, there's only one notification handler
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForCallWaitingInfo(Handler h, Int32 what, Object obj);
    void UnregisterForCallWaitingInfo(Handler h);

    /**
     * Sets the handler for Event Notifications for Signal Info.
     * Unlike the register* methods, there's only one notification handler
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForSignalInfo(Handler h, Int32 what, Object obj);
    void UnregisterForSignalInfo(Handler h);

    /**
     * Registers the handler for CDMA number information record
     * Unlike the register* methods, there's only one notification handler
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForNumberInfo(Handler h, Int32 what, Object obj);
    void UnregisterForNumberInfo(Handler h);

    /**
     * Registers the handler for CDMA redirected number Information record
     * Unlike the register* methods, there's only one notification handler
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForRedirectedNumberInfo(Handler h, Int32 what, Object obj);
    void UnregisterForRedirectedNumberInfo(Handler h);

    /**
     * Registers the handler for CDMA line control information record
     * Unlike the register* methods, there's only one notification handler
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForLineControlInfo(Handler h, Int32 what, Object obj);
    void UnregisterForLineControlInfo(Handler h);

    /**
     * Registers the handler for CDMA T53 CLIR information record
     * Unlike the register* methods, there's only one notification handler
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterFoT53ClirlInfo(Handler h, Int32 what, Object obj);
    void UnregisterForT53ClirInfo(Handler h);

    /**
     * Registers the handler for CDMA T53 audio control information record
     * Unlike the register* methods, there's only one notification handler
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForT53AudioControlInfo(Handler h, Int32 what, Object obj);
    void UnregisterForT53AudioControlInfo(Handler h);

    /**
     * Fires on if Modem enters Emergency Callback mode
     */
    void SetEmergencyCallbackMode(Handler h, Int32 what, Object obj);

     /**
      * Fires on any CDMA OTA provision status change
      */
     void RegisterForCdmaOtaProvision(Handler h,Int32 what, Object obj);
     void UnregisterForCdmaOtaProvision(Handler h);

     /**
      * Registers the handler when out-band ringback tone is needed.<p>
      *
      *  Messages received from this:
      *  Message.obj will be an AsyncResult
      *  AsyncResult.userObj = obj
      *  AsyncResult.result = Boolean. <p>
      */
     void RegisterForRingbackTone(Handler h, Int32 what, Object obj);
     void UnregisterForRingbackTone(Handler h);

     /**
      * Registers the handler when mute/unmute need to be resent to get
      * uplink audio during a call.<p>
      *
      * @param h Handler for notification message.
      * @param what User-defined message code.
      * @param obj User object.
      *
      */
     void RegisterForResendIncallMute(Handler h, Int32 what, Object obj);
     void UnregisterForResendIncallMute(Handler h);

     /**
      * Registers the handler for when Cdma subscription changed events
      *
      * @param h Handler for notification message.
      * @param what User-defined message code.
      * @param obj User object.
      *
      */
     void RegisterForCdmaSubscriptionChanged(Handler h, Int32 what, Object obj);
     void UnregisterForCdmaSubscriptionChanged(Handler h);

     /**
      * Registers the handler for when Cdma prl changed events
      *
      * @param h Handler for notification message.
      * @param what User-defined message code.
      * @param obj User object.
      *
      */
     void RegisterForCdmaPrlChanged(Handler h, Int32 what, Object obj);
     void UnregisterForCdmaPrlChanged(Handler h);

     /**
      * Registers the handler for when Cdma prl changed events
      *
      * @param h Handler for notification message.
      * @param what User-defined message code.
      * @param obj User object.
      *
      */
     void RegisterForExitEmergencyCallbackMode(Handler h, Int32 what, Object obj);
     void UnregisterForExitEmergencyCallbackMode(Handler h);

     /**
      * Registers the handler for RIL_UNSOL_RIL_CONNECT events.
      *
      * When ril connects or disconnects a message is sent to the registrant
      * which contains an AsyncResult, ar, in msg.obj. The ar.result is an
      * Integer which is the version of the ril or -1 if the ril disconnected.
      *
      * @param h Handler for notification message.
      * @param what User-defined message code.
      * @param obj User object.
      */
     void RegisterForRilConnected(Handler h, Int32 what, Object obj);
     void UnregisterForRilConnected(Handler h);

    /**
     * Supply the ICC PIN to the ICC card
     *
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  This exception is CommandException with an error of PASSWORD_INCORRECT
     *  if the password is incorrect
     *
     *  ar.result is an optional array of integers where the first entry
     *  is the number of attempts remaining before the ICC will be PUK locked.
     *
     * ar.exception and ar.result are NULL on success
     */

    void SupplyIccPin(String pin, Message result);

    /**
     * Supply the PIN for the app with this AID on the ICC card
     *
     *  AID (Application ID), See ETSI 102.221 8.1 and 101.220 4
     *
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  This exception is CommandException with an error of PASSWORD_INCORRECT
     *  if the password is incorrect
     *
     *  ar.result is an optional array of integers where the first entry
     *  is the number of attempts remaining before the ICC will be PUK locked.
     *
     * ar.exception and ar.result are NULL on success
     */

    void SupplyIccPinForApp(String pin, String aid, Message result);

    /**
     * Supply the ICC PUK and newPin to the ICC card
     *
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  This exception is CommandException with an error of PASSWORD_INCORRECT
     *  if the password is incorrect
     *
     *  ar.result is an optional array of integers where the first entry
     *  is the number of attempts remaining before the ICC is permanently disabled.
     *
     * ar.exception and ar.result are NULL on success
     */

    void SupplyIccPuk(String puk, String newPin, Message result);

    /**
     * Supply the PUK, new pin for the app with this AID on the ICC card
     *
     *  AID (Application ID), See ETSI 102.221 8.1 and 101.220 4
     *
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  This exception is CommandException with an error of PASSWORD_INCORRECT
     *  if the password is incorrect
     *
     *  ar.result is an optional array of integers where the first entry
     *  is the number of attempts remaining before the ICC is permanently disabled.
     *
     * ar.exception and ar.result are NULL on success
     */

    void SupplyIccPukForApp(String puk, String newPin, String aid, Message result);

    /**
     * Supply the ICC PIN2 to the ICC card
     * Only called following operation where ICC_PIN2 was
     * returned as a a failure from a previous operation
     *
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  This exception is CommandException with an error of PASSWORD_INCORRECT
     *  if the password is incorrect
     *
     *  ar.result is an optional array of integers where the first entry
     *  is the number of attempts remaining before the ICC will be PUK locked.
     *
     * ar.exception and ar.result are NULL on success
     */

    void SupplyIccPin2(String pin2, Message result);

    /**
     * Supply the PIN2 for the app with this AID on the ICC card
     * Only called following operation where ICC_PIN2 was
     * returned as a a failure from a previous operation
     *
     *  AID (Application ID), See ETSI 102.221 8.1 and 101.220 4
     *
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  This exception is CommandException with an error of PASSWORD_INCORRECT
     *  if the password is incorrect
     *
     *  ar.result is an optional array of integers where the first entry
     *  is the number of attempts remaining before the ICC will be PUK locked.
     *
     * ar.exception and ar.result are NULL on success
     */

    void SupplyIccPin2ForApp(String pin2, String aid, Message result);

    /**
     * Supply the SIM PUK2 to the SIM card
     * Only called following operation where SIM_PUK2 was
     * returned as a a failure from a previous operation
     *
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  This exception is CommandException with an error of PASSWORD_INCORRECT
     *  if the password is incorrect
     *
     *  ar.result is an optional array of integers where the first entry
     *  is the number of attempts remaining before the ICC is permanently disabled.
     *
     * ar.exception and ar.result are NULL on success
     */

    void SupplyIccPuk2(String puk2, String newPin2, Message result);

    /**
     * Supply the PUK2, newPin2 for the app with this AID on the ICC card
     * Only called following operation where SIM_PUK2 was
     * returned as a a failure from a previous operation
     *
     *  AID (Application ID), See ETSI 102.221 8.1 and 101.220 4
     *
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  This exception is CommandException with an error of PASSWORD_INCORRECT
     *  if the password is incorrect
     *
     *  ar.result is an optional array of integers where the first entry
     *  is the number of attempts remaining before the ICC is permanently disabled.
     *
     * ar.exception and ar.result are NULL on success
     */

    void SupplyIccPuk2ForApp(String puk2, String newPin2, String aid, Message result);

    // TODO: Add java doc and indicate that msg.arg1 contains the number of attempts remaining.
    void ChangeIccPin(String oldPin, String newPin, Message result);
    void ChangeIccPinForApp(String oldPin, String newPin, String aidPtr, Message result);
    void ChangeIccPin2(String oldPin2, String newPin2, Message result);
    void ChangeIccPin2ForApp(String oldPin2, String newPin2, String aidPtr, Message result);

    void ChangeBarringPassword(String facility, String oldPwd, String newPwd, Message result);

    void SupplyDepersonalization(String netpin, String type, Message result);

    /**
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result contains a List of DriverCall
     *      The ar.result List is sorted by DriverCall.index
     */
    void GetCurrentCalls (Message result);

    /**
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result contains a List of DataCallResponse
     *  @deprecated Do not use.
     */
    //@Deprecated
    void GetPDPContextList(Message result);

    /**
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result contains a List of DataCallResponse
     */
    void GetDataCallList(Message result);

    /**
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is NULL on success and failure
     *
     * CLIR_DEFAULT     == on "use subscription default value"
     * CLIR_SUPPRESSION == on "CLIR suppression" (allow CLI presentation)
     * CLIR_INVOCATION  == on "CLIR invocation" (restrict CLI presentation)
     */
    void Dial (String address, Int32 clirMode, Message result);

    /**
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is NULL on success and failure
     *
     * CLIR_DEFAULT     == on "use subscription default value"
     * CLIR_SUPPRESSION == on "CLIR suppression" (allow CLI presentation)
     * CLIR_INVOCATION  == on "CLIR invocation" (restrict CLI presentation)
     */
    void Dial(String address, Int32 clirMode, UUSInfo uusInfo, Message result);

    /**
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is String containing IMSI on success
     */
    void GetIMSI(Message result);

    /**
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is String containing IMSI on success
     */
    void GetIMSIForApp(String aid, Message result);

    /**
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is String containing IMEI on success
     */
    void GetIMEI(Message result);

    /**
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is String containing IMEISV on success
     */
    void GetIMEISV(Message result);

    /**
     * Hang up one individual connection.
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is NULL on success and failure
     *
     *  3GPP 22.030 6.5.5
     *  "Releases a specific active call X"
     */
    void HangupConnection (Int32 gsmIndex, Message result);

    /**
     * 3GPP 22.030 6.5.5
     *  "Releases all held calls or sets User Determined User Busy (UDUB)
     *   for a waiting call."
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is NULL on success and failure
     */
    void HangupWaitingOrBackground (Message result);

    /**
     * 3GPP 22.030 6.5.5
     * "Releases all active Calls (if any exist) and accepts
     *  the Other (held or waiting) call."
     *
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is NULL on success and failure
     */
    void HangupForegroundResumeBackground (Message result);

    /**
     * 3GPP 22.030 6.5.5
     * "Places all active Calls (if any exist) on hold and accepts
     *  the Other (held or waiting) call."
     *
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is NULL on success and failure
     */
    void SwitchWaitingOrHoldingAndActive (Message result);

    /**
     * 3GPP 22.030 6.5.5
     * "Adds a held call to the conversation"
     *
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is NULL on success and failure
     */
    void Conference (Message result);

    /**
     * Set preferred Voice Privacy (VP).
     *
     * @param enable TRUE is enhanced and FALSE is normal VP
     * @param result is a callback message
     */
    void SetPreferredVoicePrivacy(Boolean enable, Message result);

    /**
     * Get currently set preferred Voice Privacy (VP) mode.
     *
     * @param result is a callback message
     */
    void GetPreferredVoicePrivacy(Message result);

    /**
     * 3GPP 22.030 6.5.5
     * "Places all active calls on hold except call X with which
     *  communication shall be supported."
     */
    void SeparateConnection (Int32 gsmIndex, Message result);

    /**
     *
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is NULL on success and failure
     */
    void AcceptCall (Message result);

    /**
     *  also known as UDUB
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is NULL on success and failure
     */
    void RejectCall (Message result);

    /**
     * 3GPP 22.030 6.5.5
     * "Connects the two calls and disconnects the subscriber from both calls"
     *
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is NULL on success and failure
     */
    void ExplicitCallTransfer (Message result);

    /**
     * cause code returned as Int32[0] in Message.obj.response
     * Returns integer cause code defined in TS 24.008
     * Annex H or closest approximation.
     * Most significant codes:
     * - Any defined in 22.001 F.4 (for generating busy/congestion)
     * - Cause 68: ACM >= ACMMax
     */
    void GetLastCallFailCause (Message result);


    /**
     * Reason for last PDP context deactivate or failure to activate
     * cause code returned as Int32[0] in Message.obj.response
     * returns an integer cause code defined in TS 24.008
     * section 6.1.3.1.3 or close approximation
     * @deprecated Do not use.
     */
    //@Deprecated
    void GetLastPdpFailCause (Message result);

    /**
     * The preferred new alternative to getLastPdpFailCause
     * that is also CDMA-compatible.
     */
    void GetLastDataCallFailCause (Message result);

    void SetMute (Boolean enableMute, Message response);

    void GetMute (Message response);

    /**
     * response.obj is an AsyncResult
     * response.obj.result is an Int32[2]
     * response.obj.result[0] is received signal Strength (0-31, 99)
     * response.obj.result[1] is  bit error Rate (0-7, 99)
     * as defined in TS 27.007 8.5
     */
    void GetSignalStrength (Message response);


    /**
     * response.obj.result is an Int32[3]
     * response.obj.result[0] is registration state 0-5 from TS 27.007 7.2
     * response.obj.result[1] is LAC if registered or -1 if not
     * response.obj.result[2] is CID if registered or -1 if not
     * valid LAC and CIDs are 0x0000 - 0xffff
     *
     * Please note that registration state 4 ("unknown") is treated
     * as "out of service" above
     */
    void GetVoiceRegistrationState (Message response);

    /**
     * response.obj.result is an Int32[3]
     * response.obj.result[0] is registration state 0-5 from TS 27.007 7.2
     * response.obj.result[1] is LAC if registered or -1 if not
     * response.obj.result[2] is CID if registered or -1 if not
     * valid LAC and CIDs are 0x0000 - 0xffff
     *
     * Please note that registration state 4 ("unknown") is treated
     * as "out of service" above
     */
    void GetDataRegistrationState (Message response);

    /**
     * response.obj.result is a String[3]
     * response.obj.result[0] is Int64 alpha or NULL if unregistered
     * response.obj.result[1] is short alpha or NULL if unregistered
     * response.obj.result[2] is numeric or NULL if unregistered
     */
    void GetOperator(Message response);

    /**
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is NULL on success and failure
     */
    void SendDtmf(Char32 c, Message result);


    /**
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is NULL on success and failure
     */
    void StartDtmf(Char32 c, Message result);

    /**
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is NULL on success and failure
     */
    void StopDtmf(Message result);

    /**
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result is NULL on success and failure
     */
    void SendBurstDtmf(String dtmfString, Int32 on, Int32 off, Message result);

    /**
     * smscPDU is smsc address in PDU form GSM BCD format prefixed
     *      by a length Byte (as expected by TS 27.005) or NULL for default SMSC
     * pdu is SMS in PDU format as an ASCII hex string
     *      less the SMSC address
     */
    void SendSMS (String smscPDU, String pdu, Message response);

    /**
     * Send an SMS message, Identical to sendSMS,
     * except that more messages are expected to be sent soon
     * smscPDU is smsc address in PDU form GSM BCD format prefixed
     *      by a length Byte (as expected by TS 27.005) or NULL for default SMSC
     * pdu is SMS in PDU format as an ASCII hex string
     *      less the SMSC address
     */
    void SendSMSExpectMore (String smscPDU, String pdu, Message response);

    /**
     * @param pdu is CDMA-SMS in internal pseudo-PDU format
     * @param response sent when operation completes
     */
    void SendCdmaSms(Byte[] pdu, Message response);

    /**
     * send SMS over IMS with 3GPP/GSM SMS format
     * @param smscPDU is smsc address in PDU form GSM BCD format prefixed
     *      by a length Byte (as expected by TS 27.005) or NULL for default SMSC
     * @param pdu is SMS in PDU format as an ASCII hex string
     *      less the SMSC address
     * @param retry indicates if this is a retry; 0 == not retry, nonzero = retry
     * @param messageRef valid field if retry is set to nonzero.
     *        Contains messageRef from RIL_SMS_Response corresponding to failed MO SMS
     * @param response sent when operation completes
     */
    void SendImsGsmSms (String smscPDU, String pdu, Int32 retry, Int32 messageRef,
            Message response);

    /**
     * send SMS over IMS with 3GPP2/CDMA SMS format
     * @param pdu is CDMA-SMS in internal pseudo-PDU format
     * @param response sent when operation completes
     * @param retry indicates if this is a retry; 0 == not retry, nonzero = retry
     * @param messageRef valid field if retry is set to nonzero.
     *        Contains messageRef from RIL_SMS_Response corresponding to failed MO SMS
     * @param response sent when operation completes
     */
    void SendImsCdmaSms(Byte[] pdu, Int32 retry, Int32 messageRef, Message response);

    /**
     * Deletes the specified SMS record from SIM Memory (EF_SMS).
     *
     * @param index index of the SMS record to delete
     * @param response sent when operation completes
     */
    void DeleteSmsOnSim(Int32 index, Message response);

    /**
     * Deletes the specified SMS record from RUIM Memory (EF_SMS in DF_CDMA).
     *
     * @param index index of the SMS record to delete
     * @param response sent when operation completes
     */
    void DeleteSmsOnRuim(Int32 index, Message response);

    /**
     * Writes an SMS message to SIM Memory (EF_SMS).
     *
     * @param status status of message on SIM.  One of:
     *                  SmsManger.STATUS_ON_ICC_READ
     *                  SmsManger.STATUS_ON_ICC_UNREAD
     *                  SmsManger.STATUS_ON_ICC_SENT
     *                  SmsManger.STATUS_ON_ICC_UNSENT
     * @param pdu message PDU, as hex string
     * @param response sent when operation completes.
     *                  response.obj will be an AsyncResult, and will indicate
     *                  any error that may have Occurred (eg, out of memory).
     */
    void WriteSmsToSim(Int32 status, String smsc, String pdu, Message response);

    void WriteSmsToRuim(Int32 status, String pdu, Message response);

    void SetRadioPower(Boolean on, Message response);

    void AcknowledgeLastIncomingGsmSms(Boolean success, Int32 cause, Message response);

    void AcknowledgeLastIncomingCdmaSms(Boolean success, Int32 cause, Message response);

    /**
     * Acknowledge successful or failed receipt of last incoming SMS,
     * including acknowledgement TPDU to send as the RP-User-Data element
     * of the RP-ACK or RP-ERROR PDU.
     *
     * @param success TRUE to send RP-ACK, FALSE to send RP-ERROR
     * @param ackPdu the acknowledgement TPDU in hexadecimal format
     * @param response sent when operation completes.
     */
    void AcknowledgeIncomingGsmSmsWithPdu(Boolean success, String ackPdu, Message response);

    /**
     * parameters equivalent to 27.007 AT+CRSM command
     * response.obj will be an AsyncResult
     * response.obj.result will be an IccIoResult on success
     */
    void IccIO (Int32 command, Int32 fileid, String path, Int32 p1, Int32 p2, Int32 p3,
            String data, String pin2, Message response);

    /**
     * parameters equivalent to 27.007 AT+CRSM command
     * response.obj will be an AsyncResult
     * response.obj.userObj will be a IccIoResult on success
     */
    void IccIOForApp (Int32 command, Int32 fileid, String path, Int32 p1, Int32 p2, Int32 p3,
            String data, String pin2, String aid, Message response);

    /**
     * (AsyncResult)response.obj).result is an Int32[] with element [0] set to
     * 1 for "CLIP is provisioned", and 0 for "CLIP is not provisioned".
     *
     * @param response is callback message
     */

    void QueryCLIP(Message response);

    /**
     * response.obj will be a an Int32[2]
     *
     * response.obj[0] will be TS 27.007 +CLIR parameter 'n'
     *  0 presentation indicator is used according to the subscription of the CLIR service
     *  1 CLIR invocation
     *  2 CLIR suppression
     *
     * response.obj[1] will be TS 27.007 +CLIR parameter 'm'
     *  0 CLIR not provisioned
     *  1 CLIR provisioned in permanent mode
     *  2 Unknown (e.g. no network, etc.)
     *  3 CLIR temporary mode presentation restricted
     *  4 CLIR temporary mode presentation allowed
     */

    void GetCLIR(Message response);

    /**
     * clirMode is one of the CLIR_* constants above
     *
     * response.obj is NULL
     */

    void SetCLIR(Int32 clirMode, Message response);

    /**
     * (AsyncResult)response.obj).result is an Int32[] with element [0] set to
     * 0 for disabled, 1 for enabled.
     *
     * @param serviceClass is a sum of SERVICE_CLASS_*
     * @param response is callback message
     */

    void QueryCallWaiting(Int32 serviceClass, Message response);

    /**
     * @param enable is TRUE to enable, FALSE to disable
     * @param serviceClass is a sum of SERVICE_CLASS_*
     * @param response is callback message
     */

    void SetCallWaiting(Boolean enable, Int32 serviceClass, Message response);

    /**
     * @param action is one of CF_ACTION_*
     * @param cfReason is one of CF_REASON_*
     * @param serviceClass is a sum of SERVICE_CLASSS_*
     */
    void SetCallForward(Int32 action, Int32 cfReason, Int32 serviceClass,
                String number, Int32 timeSeconds, Message response);

    /**
     * cfReason is one of CF_REASON_*
     *
     * ((AsyncResult)response.obj).result will be an array of
     * CallForwardInfo's
     *
     * An array of length 0 means "disabled for all codes"
     */
    void QueryCallForwardStatus(Int32 cfReason, Int32 serviceClass,
            String number, Message response);

    void SetNetworkSelectionModeAutomatic(Message response);

    void SetNetworkSelectionModeManual(String operatorNumeric, Message response);

    /**
     * Queries whether the current network selection mode is automatic
     * or manual
     *
     * ((AsyncResult)response.obj).result  is an Int32[] with element [0] being
     * a 0 for automatic selection and a 1 for manual selection
     */

    void GetNetworkSelectionMode(Message response);

    /**
     * Queries the currently available networks
     *
     * ((AsyncResult)response.obj).result  is a List of NetworkInfo objects
     */
    void GetAvailableNetworks(Message response);

    void GetBasebandVersion (Message response);


    /**
     * (AsyncResult)response.obj).result will be an Integer representing
     * the sum of enabled service Classes (sum of SERVICE_CLASS_*)
     *
     * @param facility one of CB_FACILTY_*
     * @param password password or "" if not required
     * @param serviceClass is a sum of SERVICE_CLASS_*
     * @param response is callback message
     */

    void QueryFacilityLock (String facility, String password, Int32 serviceClass,
        Message response);

    /**
     * (AsyncResult)response.obj).result will be an Integer representing
     * the sum of enabled service Classes (sum of SERVICE_CLASS_*) for the
     * application with appId.
     *
     * @param facility one of CB_FACILTY_*
     * @param password password or "" if not required
     * @param serviceClass is a sum of SERVICE_CLASS_*
     * @param appId is application Id or NULL if none
     * @param response is callback message
     */

    void QueryFacilityLockForApp(String facility, String password, Int32 serviceClass, String appId,
        Message response);

    /**
     * @param facility one of CB_FACILTY_*
     * @param lockState TRUE means lock, FALSE means unlock
     * @param password password or "" if not required
     * @param serviceClass is a sum of SERVICE_CLASS_*
     * @param response is callback message
     */
    void SetFacilityLock (String facility, Boolean lockState, String password,
        Int32 serviceClass, Message response);

    /**
     * Set the facility lock for the app with this AID on the ICC card.
     *
     * @param facility one of CB_FACILTY_*
     * @param lockState TRUE means lock, FALSE means unlock
     * @param password password or "" if not required
     * @param serviceClass is a sum of SERVICE_CLASS_*
     * @param appId is application Id or NULL if none
     * @param response is callback message
     */
    void SetFacilityLockForApp(String facility, Boolean lockState, String password,
        Int32 serviceClass, String appId, Message response);

    void SendUSSD (String ussdString, Message response);

    /**
     * Cancels a pending USSD session if one exists.
     * @param response callback message
     */
    void CancelPendingUssd (Message response);

    void ResetRadio(Message result);

    /**
     * Assign a specified band for RF configuration.
     *
     * @param bandMode one of BM_*_BAND
     * @param response is callback message
     */
    void SetBandMode (Int32 bandMode, Message response);

    /**
     * Query the list of band mode supported by RF.
     *
     * @param response is callback message
     *        ((AsyncResult)response.obj).result  is an Int32[] where Int32[0] is
     *        the size of the array and the rest of each element representing
     *        one available BM_*_BAND
     */
    void QueryAvailableBandMode (Message response);

    /**
     *  Requests to set the preferred network type for searching and registering
     * (CS/PS domain, RAT, and operation mode)
     * @param networkType one of  NT_*_TYPE
     * @param response is callback message
     */
    void SetPreferredNetworkType(Int32 networkType , Message response);

     /**
     *  Query the preferred network type setting
     *
     * @param response is callback message to report one of  NT_*_TYPE
     */
    void GetPreferredNetworkType(Message response);

    /**
     * Query neighboring cell ids
     *
     * @param response s callback message to cell ids
     */
    void GetNeighboringCids(Message response);

    /**
     * Request to enable/disable network state change notifications when
     * location Information (lac and/or cid) has changed.
     *
     * @param enable TRUE to enable, FALSE to disable
     * @param response callback message
     */
    void SetLocationUpdates(Boolean enable, Message response);

    /**
     * Gets the default SMSC address.
     *
     * @param result Callback message contains the SMSC address.
     */
    void GetSmscAddress(Message result);

    /**
     * Sets the default SMSC address.
     *
     * @param address new SMSC address
     * @param result Callback message is empty on completion
     */
    void SetSmscAddress(String address, Message result);

    /**
     * Indicates whether there is storage available for new SMS messages.
     * @param available TRUE if storage is available
     * @param result callback message
     */
    void ReportSmsMemoryStatus(Boolean available, Message result);

    /**
     * Indicates to the vendor ril that StkService is running
     * and is ready to receive RIL_UNSOL_STK_XXXX commands.
     *
     * @param result callback message
     */
    void ReportStkServiceIsRunning(Message result);

    void InvokeOemRilRequestRaw(Byte[] data, Message response);

    void InvokeOemRilRequestStrings(String[] strings, Message response);

    /**
     * Fires when RIL_UNSOL_OEM_HOOK_RAW is received from the RIL.
     */
    void SetOnUnsolOemHookRaw(Handler h, Int32 what, Object obj);
    void UnSetOnUnsolOemHookRaw(Handler h);

    /**
     * Send TERMINAL RESPONSE to the SIM, after processing a proactive command
     * sent by the SIM.
     *
     * @param contents  String containing SAT/USAT response in hexadecimal
     *                  format starting with first Byte of response data. See
     *                  TS 102 223 for details.
     * @param response  Callback message
     */
    CARAPI SendTerminalResponse(String contents, Message response);

    /**
     * Send ENVELOPE to the SIM, after processing a proactive command sent by
     * the SIM.
     *
     * @param contents  String containing SAT/USAT response in hexadecimal
     *                  format starting with command tag. See TS 102 223 for
     *                  details.
     * @param response  Callback message
     */
    CARAPI SendEnvelope(String contents, Message response);

    /**
     * Send ENVELOPE to the SIM, such as an SMS-PP data download envelope
     * for a SIM data download message. This method has one difference
     * from {@link #sendEnvelope}: The SW1 and SW2 status bytes from the UICC response
     * are returned along with the response data.
     *
     * response.obj will be an AsyncResult
     * response.obj.result will be an IccIoResult on success
     *
     * @param contents  String containing SAT/USAT response in hexadecimal
     *                  format starting with command tag. See TS 102 223 for
     *                  details.
     * @param response  Callback message
     */
    CARAPI SendEnvelopeWithStatus(String contents, Message response);

    /**
     * Accept or reject the call setup request from SIM.
     *
     * @param accept   TRUE if the call is to be accepted, FALSE otherwise.
     * @param response Callback message
     */
    CARAPI HandleCallSetupRequestFromSim(Boolean accept, Message response);

    /**
     * Activate or deactivate cell broadcast SMS for GSM.
     *
     * @param activate
     *            TRUE = activate, FALSE = deactivate
     * @param result Callback message is empty on completion
     */
    CARAPI SetGsmBroadcastActivation(Boolean activate, Message result);

    /**
     * Configure cell broadcast SMS for GSM.
     *
     * @param response Callback message is empty on completion
     */
    CARAPI SetGsmBroadcastConfig(SmsBroadcastConfigInfo[] config, Message response);

    /**
     * Query the current configuration of cell broadcast SMS of GSM.
     *
     * @param response
     *        Callback message contains the configuration from the modem
     *        on completion
     */
    CARAPI GetGsmBroadcastConfig(Message response);

    //***** new Methods for CDMA support

    /**
     * Request the device ESN / MEID / IMEI / IMEISV.
     * "response" is const Char32 **
     *   [0] is IMEI if GSM subscription is available
     *   [1] is IMEISV if GSM subscription is available
     *   [2] is ESN if CDMA subscription is available
     *   [3] is MEID if CDMA subscription is available
     */
    CARAPI GetDeviceIdentity(Message response);

    /**
     * Request the device MDN / H_SID / H_NID / MIN.
     * "response" is const Char32 **
     *   [0] is MDN if CDMA subscription is available
     *   [1] is a comma separated list of H_SID (Home SID) in decimal format
     *       if CDMA subscription is available
     *   [2] is a comma separated list of H_NID (Home NID) in decimal format
     *       if CDMA subscription is available
     *   [3] is MIN (10 digits, MIN2+MIN1) if CDMA subscription is available
     */
    CARAPI GetCDMASubscription(Message response);

    /**
     * Send Flash Code.
     * "response" is is NULL
     *   [0] is a FLASH string
     */
    CARAPI SendCDMAFeatureCode(String FeatureCode, Message response);

    /** Set the Phone type created */
    void SetPhoneType(Int32 phoneType);

    /**
     *  Query the CDMA roaming preference setting
     *
     * @param response is callback message to report one of  CDMA_RM_*
     */
    void QueryCdmaRoamingPreference(Message response);

    /**
     *  Requests to set the CDMA roaming preference
     * @param cdmaRoamingType one of  CDMA_RM_*
     * @param response is callback message
     */
    void SetCdmaRoamingPreference(Int32 cdmaRoamingType, Message response);

    /**
     *  Requests to set the CDMA subscription mode
     * @param cdmaSubscriptionType one of  CDMA_SUBSCRIPTION_*
     * @param response is callback message
     */
    void SetCdmaSubscriptionSource(Int32 cdmaSubscriptionType, Message response);

    /**
     *  Requests to get the CDMA subscription srouce
     * @param response is callback message
     */
    void GetCdmaSubscriptionSource(Message response);

    /**
     *  Set the TTY mode
     *
     * @param ttyMode one of the following:
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_OFF}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_FULL}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_HCO}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_VCO}
     * @param response is callback message
     */
    void SetTTYMode(Int32 ttyMode, Message response);

    /**
     *  Query the TTY mode
     * (AsyncResult)response.obj).result is an Int32[] with element [0] set to
     * tty mode:
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_OFF}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_FULL}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_HCO}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_VCO}
     * @param response is callback message
     */
    void QueryTTYMode(Message response);

    /**
     * Setup a packet data connection On successful completion, the result
     * message will return a {@link com.android.internal.telephony.dataconnection.DataCallResponse}
     * object containing the connection information.
     *
     * @param radioTechnology
     *            indicates whether to setup connection on radio technology CDMA
     *            (0) or GSM/UMTS (1)
     * @param profile
     *            Profile Number or NULL to indicate default profile
     * @param apn
     *            the APN to connect to if radio technology is GSM/UMTS.
     *            Otherwise NULL for CDMA.
     * @param user
     *            the username for APN, or NULL
     * @param password
     *            the password for APN, or NULL
     * @param authType
     *            the PAP / CHAP auth type. Values is one of SETUP_DATA_AUTH_*
     * @param protocol
     *            one of the PDP_type values in TS 27.007 section 10.1.1.
     *            For example, "IP", "IPV6", "IPV4V6", or "PPP".
     * @param result
     *            Callback message
     */
    CARAPI SetupDataCall(String radioTechnology, String profile,
            String apn, String user, String password, String authType,
            String protocol, Message result);

    /**
     * Deactivate packet data connection
     *
     * @param cid
     *            The connection ID
     * @param reason
     *            Data disconnect reason.
     * @param result
     *            Callback message is empty on completion
     */
    CARAPI DeactivateDataCall(Int32 cid, Int32 reason, Message result);

    /**
     * Activate or deactivate cell broadcast SMS for CDMA.
     *
     * @param activate
     *            TRUE = activate, FALSE = deactivate
     * @param result
     *            Callback message is empty on completion
     */
    CARAPI SetCdmaBroadcastActivation(Boolean activate, Message result);

    /**
     * Configure cdma cell broadcast SMS.
     *
     * @param response
     *            Callback message is empty on completion
     */
    CARAPI SetCdmaBroadcastConfig(CdmaSmsBroadcastConfigInfo[] configs, Message response);

    /**
     * Query the current configuration of cdma cell broadcast SMS.
     *
     * @param result
     *            Callback message contains the configuration from the modem on completion
     */
    CARAPI GetCdmaBroadcastConfig(Message result);

    /**
     *  Requests the radio's system selection module to exit emergency callback mode.
     *  This function should only be called from CDMAPHone.java.
     *
     * @param response callback message
     */
    CARAPI ExitEmergencyCallbackMode(Message response);

    /**
     * Request the status of the ICC and UICC cards.
     *
     * @param result
     *          Callback message containing {@link IccCardStatus} structure for the card.
     */
    CARAPI GetIccCardStatus(Message result);

    /**
     * Return if the current radio is LTE on CDMA. This
     * is a tri-state return value as for a period of time
     * the mode may be unknown.
     *
     * @return {@link PhoneConstants#LTE_ON_CDMA_UNKNOWN}, {@link PhoneConstants#LTE_ON_CDMA_FALSE}
     * or {@link PhoneConstants#LTE_ON_CDMA_TRUE}
     */
    public Int32 GetLteOnCdmaMode();

    /**
     * Get the data call profile information from the modem
     *
     * @param appType
     *          Callback message containing the count and the list of {@link
     *          RIL_DataCallProfileInfo}
     *
     * @param result
     *          Callback message
     */
    CARAPI GetDataCallProfile(Int32 appType, Message result);

    /**
     * Return if the current radio is LTE on GSM
     * @hide
     */
    public Int32 GetLteOnGsmMode();

    /**
     * Request the ISIM application on the UICC to perform the AKA
     * challenge/response algorithm for IMS authentication. The nonce string
     * and challenge response are Base64 encoded Strings.
     *
     * @param nonce the nonce string to pass with the ISIM authentication request
     * @param response a callback message with the String response in the obj field
     * @deprecated
     * @see requestIccSimAuthentication
     */
    CARAPI RequestIsimAuthentication(String nonce, Message response);

    /**
     * Request the SIM application on the UICC to perform authentication
     * challenge/response algorithm. The data string and challenge response are
     * Base64 encoded Strings.
     * Can support EAP-SIM, EAP-AKA with results encoded per 3GPP TS 31.102.
     *
     * @param authContext is the P2 parameter that specifies the authentication context per 3GPP TS
     *                    31.102 (Section 7.1.2)
     * @param data authentication challenge data
     * @param aid used to determine which application/slot to send the auth command to. See ETSI
     *            102.221 8.1 and 101.220 4
     * @param response a callback message with the String response in the obj field
     */
    CARAPI RequestIccSimAuthentication(Int32 authContext, String data, String aid, Message response);

    /**
     * Get the current Voice Radio Technology.
     *
     * AsyncResult.result is an Int32 array with the first value
     * being one of the ServiceState.RIL_RADIO_TECHNOLOGY_xxx values.
     *
     * @param result is sent back to handler and result.obj is a AsyncResult
     */
    void GetVoiceRadioTechnology(Message result);

    /**
     * Return the current set of CellInfo records
     *
     * AsyncResult.result is a of Collection<CellInfo>
     *
     * @param result is sent back to handler and result.obj is a AsyncResult
     */
    void GetCellInfoList(Message result);

    /**
     * Sets the minimum time in milli-seconds between when RIL_UNSOL_CELL_INFO_LIST
     * should be invoked.
     *
     * The default, 0, means invoke RIL_UNSOL_CELL_INFO_LIST when any of the reported
     * information changes. Setting the value to INT_MAX(0x7fffffff) means never issue
     * A RIL_UNSOL_CELL_INFO_LIST.
     *
     *

     * @param rateInMillis is sent back to handler and result.obj is a AsyncResult
     * @param response.obj is AsyncResult ar when sent to associated handler
     *                        ar.exception carries exception on failure or NULL on success
     *                        otherwise the error.
     */
    void SetCellInfoListRate(Int32 rateInMillis, Message response);

    /**
     * Fires when RIL_UNSOL_CELL_INFO_LIST is received from the RIL.
     */
    void RegisterForCellInfoList(Handler h, Int32 what, Object obj);
    void UnregisterForCellInfoList(Handler h);

    /**
     * Set Initial Attach Apn
     *
     * @param apn
     *            the APN to connect to if radio technology is GSM/UMTS.
     * @param protocol
     *            one of the PDP_type values in TS 27.007 section 10.1.1.
     *            For example, "IP", "IPV6", "IPV4V6", or "PPP".
     * @param authType
     *            authentication protocol used for this PDP context
     *            (None: 0, PAP: 1, CHAP: 2, PAP&CHAP: 3)
     * @param username
     *            the username for APN, or NULL
     * @param password
     *            the password for APN, or NULL
     * @param result
     *            callback message contains the information of SUCCESS/FAILURE
     */
    CARAPI SetInitialAttachApn(String apn, String protocol, Int32 authType, String username,
            String password, Message result);

    /**
     * Set data profiles in modem
     *
     * @param dps
     *            Array of the data profiles set to modem
     * @param result
     *            callback message contains the information of SUCCESS/FAILURE
     */
    CARAPI SetDataProfile(DataProfile[] dps, Message result);

    /**
     * Notifiy that we are testing an emergency call
     */
    CARAPI TestingEmergencyCall();

    /**
     * Open a logical channel to the SIM.
     *
     * Input parameters equivalent to TS 27.007 AT+CCHO command.
     *
     * @param AID Application id. See ETSI 102.221 and 101.220.
     * @param response Callback message. response.obj will be an Int32 [1] with
     *            element [0] set to the id of the logical channel.
     */
    CARAPI IccOpenLogicalChannel(String AID, Message response);

    /**
     * Close a previously opened logical channel to the SIM.
     *
     * Input parameters equivalent to TS 27.007 AT+CCHC command.
     *
     * @param channel Channel id. Id of the channel to be closed.
     * @param response Callback message.
     */
    CARAPI IccCloseLogicalChannel(Int32 channel, Message response);

    /**
     * Exchange APDUs with the SIM on a logical channel.
     *
     * Input parameters equivalent to TS 27.007 AT+CGLA command.
     *
     * @param channel Channel id of the channel to use for communication. Has to
     *            be greater than zero.
     * @param cla Class of the APDU command.
     * @param instruction Instruction of the APDU command.
     * @param p1 P1 value of the APDU command.
     * @param p2 P2 value of the APDU command.
     * @param p3 P3 value of the APDU command. If p3 is negative a 4 Byte APDU
     *            is sent to the SIM.
     * @param data Data to be sent with the APDU.
     * @param response Callback message. response.obj.userObj will be
     *            an IccIoResult on success.
     */
    CARAPI IccTransmitApduLogicalChannel(Int32 channel, Int32 cla, Int32 instruction,
            Int32 p1, Int32 p2, Int32 p3, String data, Message response);

    /**
     * Exchange APDUs with the SIM on a basic channel.
     *
     * Input parameters equivalent to TS 27.007 AT+CSIM command.
     *
     * @param cla Class of the APDU command.
     * @param instruction Instruction of the APDU command.
     * @param p1 P1 value of the APDU command.
     * @param p2 P2 value of the APDU command.
     * @param p3 P3 value of the APDU command. If p3 is negative a 4 Byte APDU
     *            is sent to the SIM.
     * @param data Data to be sent with the APDU.
     * @param response Callback message. response.obj.userObj will be
     *            an IccIoResult on success.
     */
    CARAPI IccTransmitApduBasicChannel(Int32 cla, Int32 instruction, Int32 p1, Int32 p2,
            Int32 p3, String data, Message response);

    /**
     * Get ATR (Answer To Reset; as per ISO/IEC 7816-4) from SIM card
     *
     * @param response Callback message
     */
    CARAPI GetAtr(Message response);

    /**
     * Read one of the NV items defined in {@link RadioNVItems} / {@code ril_nv_items.h}.
     * Used for device configuration by some CDMA operators.
     *
     * @param itemID the ID of the item to read
     * @param response callback message with the String response in the obj field
     */
    void NvReadItem(Int32 itemID, Message response);

    /**
     * Write one of the NV items defined in {@link RadioNVItems} / {@code ril_nv_items.h}.
     * Used for device configuration by some CDMA operators.
     *
     * @param itemID the ID of the item to read
     * @param itemValue the value to write, as a String
     * @param response Callback message.
     */
    void NvWriteItem(Int32 itemID, String itemValue, Message response);

    /**
     * Update the CDMA Preferred Roaming List (PRL) in the radio NV storage.
     * Used for device configuration by some CDMA operators.
     *
     * @param preferredRoamingList Byte array containing the new PRL
     * @param response Callback message.
     */
    void NvWriteCdmaPrl(Byte[] preferredRoamingList, Message response);

    /**
     * Perform the specified type of NV config reset. The radio will be taken offline
     * and the device must be rebooted after erasing the NV. Used for device
     * configuration by some CDMA operators.
     *
     * @param resetType reset type: 1: reload NV reset, 2: erase NV reset, 3: factory NV reset
     * @param response Callback message.
     */
    void NvResetConfig(Int32 resetType, Message response);

    /**
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the orignal value of result.obj
     *  ar.result contains a List of HardwareConfig
     */
    void GetHardwareConfig (Message result);

    /**
     * @return version of the ril.
     */
    Int32 GetRilVersion();

   /**
     * Sets user selected subscription at Modem.
     *
     * @param slotId
     *          Slot.
     * @param appIndex
     *          Application index in the card.
     * @param subId
     *          Indicates subscription 0 or subscription 1.
     * @param subStatus
     *          Activation status, 1 = activate and 0 = deactivate.
     * @param result
     *          Callback message contains the information of SUCCESS/FAILURE.
     */
    // FIXME Update the doc and consider modifying the request to make more generic.
    CARAPI SetUiccSubscription(Int32 slotId, Int32 appIndex, Int32 subId, Int32 subStatus,
            Message result);

    /**
     * Tells the modem if data is allowed or not.
     *
     * @param allowed
     *          TRUE = allowed, FALSE = not alowed
     * @param result
     *          Callback message contains the information of SUCCESS/FAILURE.
     */
    // FIXME We may need to pass AID and slotid also
    CARAPI SetDataAllowed(Boolean allowed, Message result);

    /**
      * Request to get modem stack capabilities
      * @param response is callback message
      */
    void GetModemCapability(Message response);


    /**
      * Request to update binding status on the stack
      * @param stackId is stack to update binding
      * @param enable is to tell enable or disable binding
      * @param response is callback message
      */
    void UpdateStackBinding(Int32 stackId, Int32 enable, Message response);

    /**
     * Inform RIL that the device is shutting down
     *
     * @param result Callback message contains the information of SUCCESS/FAILURE
     */
    CARAPI RequestShutdown(Message result);

    /**
     * @hide
     * CM-specific: Ask the RIL about the presence of back-compat flags
     */
    public Boolean NeedsOldRilFeature(String feature);

    /**
     * @hide
     * Register/unregister for WWAN and IWLAN coexistence
     * notification.
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForWwanIwlanCoexistence(Handler h, Int32 what, Object obj);
    void UnregisterForWwanIwlanCoexistence(Handler h);

    void RegisterForSimRefreshEvent(Handler h, Int32 what, Object obj);
    void UnregisterForSimRefreshEvent(Handler h);

    /**
     * Request to update the current local call hold state.
     * @param lchStatus, TRUE if call is in lch state
     */
    CARAPI SetLocalCallHold(Int32 lchStatus);

    /**
     * Register/unregister for modem capability oem hook event
     *
     * @param h Handler for notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */

    void RegisterForModemCapEvent(Handler h, Int32 what, Object obj);
    void UnregisterForModemCapEvent(Handler h);

    /**
     * @hide
     * samsung stk service implementation - set up registrant for sending
     * sms send result from Modem(RIL) to catService
     */
    void SetOnCatSendSmsResult(Handler h, Int32 what, Object obj);
    void UnSetOnCatSendSmsResult(Handler h);
}
