/*
 * Copyright (C) 2007 The Android Open Source Project
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

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::IPhoneStateListener;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISignalStrength;

using Elastos::Droid::Internal::Telephony::Imsphone::IImsPhone;
using Elastos::Droid::Internal::Telephony::Test::ISimulatedRadioControl;
using Elastos::Droid::Internal::Telephony::Uicc::IIsimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUsimServiceTable;
using Elastos::Droid::Internal::Telephony::PhoneConstants::I*; // ????

using Elastos::Utility::IList;

/**
 * Internal interface used to control the phone; SDK developers cannot
 * obtain this interface.
 *
 * {@hide}
 *
 */
public interface Phone {

    /** used to enable additional debug messages */
    static const Boolean DEBUG_PHONE = TRUE;

    public enum DataActivityState {
        /**
         * The state of a data activity.
         * <ul>
         * <li>NONE = No traffic</li>
         * <li>DATAIN = Receiving IP ppp traffic</li>
         * <li>DATAOUT = Sending IP ppp traffic</li>
         * <li>DATAINANDOUT = Both receiving and sending IP ppp traffic</li>
         * <li>DORMANT = The data connection is still active,
                                     but physical link is down</li>
         * </ul>
         */
        NONE, DATAIN, DATAOUT, DATAINANDOUT, DORMANT;
    }

    enum SuppService {
      UNKNOWN, SWITCH, SEPARATE, TRANSFER, CONFERENCE, REJECT, HANGUP;
    }

    // "Features" accessible through the connectivity manager
    static const String FEATURE_ENABLE_MMS = "enableMMS";
    static const String FEATURE_ENABLE_SUPL = "enableSUPL";
    static const String FEATURE_ENABLE_DUN = "enableDUN";
    static const String FEATURE_ENABLE_HIPRI = "enableHIPRI";
    static const String FEATURE_ENABLE_DUN_ALWAYS = "enableDUNAlways";
    static const String FEATURE_ENABLE_FOTA = "enableFOTA";
    static const String FEATURE_ENABLE_IMS = "enableIMS";
    static const String FEATURE_ENABLE_CBS = "enableCBS";
    static const String FEATURE_ENABLE_EMERGENCY = "enableEmergency";

    /**
     * Optional reasons for disconnect and connect
     */
    static const String REASON_ROAMING_ON = "roamingOn";
    static const String REASON_ROAMING_OFF = "roamingOff";
    static const String REASON_DATA_DISABLED = "dataDisabled";
    static const String REASON_DATA_ENABLED = "dataEnabled";
    static const String REASON_DATA_ATTACHED = "dataAttached";
    static const String REASON_DATA_DETACHED = "dataDetached";
    static const String REASON_CDMA_DATA_ATTACHED = "cdmaDataAttached";
    static const String REASON_CDMA_DATA_DETACHED = "cdmaDataDetached";
    static const String REASON_APN_CHANGED = "apnChanged";
    static const String REASON_APN_SWITCHED = "apnSwitched";
    static const String REASON_APN_FAILED = "apnFailed";
    static const String REASON_RESTORE_DEFAULT_APN = "restoreDefaultApn";
    static const String REASON_RADIO_TURNED_OFF = "radioTurnedOff";
    static const String REASON_PDP_RESET = "pdpReset";
    static const String REASON_VOICE_CALL_ENDED = "2GVoiceCallEnded";
    static const String REASON_VOICE_CALL_STARTED = "2GVoiceCallStarted";
    static const String REASON_PS_RESTRICT_ENABLED = "psRestrictEnabled";
    static const String REASON_PS_RESTRICT_DISABLED = "psRestrictDisabled";
    static const String REASON_SIM_LOADED = "simLoaded";
    static const String REASON_NW_TYPE_CHANGED = "nwTypeChanged";
    static const String REASON_DATA_DEPENDENCY_MET = "dependencyMet";
    static const String REASON_DATA_DEPENDENCY_UNMET = "dependencyUnmet";
    static const String REASON_LOST_DATA_CONNECTION = "lostDataConnection";
    static const String REASON_CONNECTED = "connected";
    static const String REASON_NV_READY = "nvReady";
    static const String REASON_SINGLE_PDN_ARBITRATION = "SinglePdnArbitration";
    static const String REASON_DATA_SPECIFIC_DISABLED = "specificDisabled";
    static const String REASON_IWLAN_AVAILABLE = "iwlanAvailable";

    // Used for band mode selection methods
    static const Int32 BM_UNSPECIFIED = 0; // selected by baseband automatically
    static const Int32 BM_EURO_BAND   = 1; // GSM-900 / DCS-1800 / WCDMA-IMT-2000
    static const Int32 BM_US_BAND     = 2; // GSM-850 / PCS-1900 / WCDMA-850 / WCDMA-PCS-1900
    static const Int32 BM_JPN_BAND    = 3; // WCDMA-800 / WCDMA-IMT-2000
    static const Int32 BM_AUS_BAND    = 4; // GSM-900 / DCS-1800 / WCDMA-850 / WCDMA-IMT-2000
    static const Int32 BM_AUS2_BAND   = 5; // GSM-900 / DCS-1800 / WCDMA-850
    static const Int32 BM_BOUNDARY    = 6; // upper band boundary

    // Used for preferred network type
    // Note NT_* substitute RILConstants.NETWORK_MODE_* above the Phone
    Int32 NT_MODE_WCDMA_PREF   = RILConstants.NETWORK_MODE_WCDMA_PREF;
    Int32 NT_MODE_GSM_ONLY     = RILConstants.NETWORK_MODE_GSM_ONLY;
    Int32 NT_MODE_WCDMA_ONLY   = RILConstants.NETWORK_MODE_WCDMA_ONLY;
    Int32 NT_MODE_GSM_UMTS     = RILConstants.NETWORK_MODE_GSM_UMTS;

    Int32 NT_MODE_CDMA         = RILConstants.NETWORK_MODE_CDMA;

    Int32 NT_MODE_CDMA_NO_EVDO = RILConstants.NETWORK_MODE_CDMA_NO_EVDO;
    Int32 NT_MODE_EVDO_NO_CDMA = RILConstants.NETWORK_MODE_EVDO_NO_CDMA;
    Int32 NT_MODE_GLOBAL       = RILConstants.NETWORK_MODE_GLOBAL;

    Int32 NT_MODE_LTE_CDMA_AND_EVDO        = RILConstants.NETWORK_MODE_LTE_CDMA_EVDO;
    Int32 NT_MODE_LTE_GSM_WCDMA            = RILConstants.NETWORK_MODE_LTE_GSM_WCDMA;
    Int32 NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA  = RILConstants.NETWORK_MODE_LTE_CDMA_EVDO_GSM_WCDMA;
    Int32 NT_MODE_LTE_ONLY                 = RILConstants.NETWORK_MODE_LTE_ONLY;
    Int32 NT_MODE_LTE_WCDMA                = RILConstants.NETWORK_MODE_LTE_WCDMA;
    Int32 PREFERRED_NT_MODE                = RILConstants.PREFERRED_NETWORK_MODE;

    Int32 NT_MODE_TD_SCDMA_ONLY            = RILConstants.NETWORK_MODE_TD_SCDMA_ONLY;
    Int32 NT_MODE_TD_SCDMA_WCDMA           = RILConstants.NETWORK_MODE_TD_SCDMA_WCDMA;
    Int32 NT_MODE_TD_SCDMA_LTE             = RILConstants.NETWORK_MODE_TD_SCDMA_LTE;
    Int32 NT_MODE_TD_SCDMA_GSM             = RILConstants.NETWORK_MODE_TD_SCDMA_GSM;
    Int32 NT_MODE_TD_SCDMA_GSM_LTE         = RILConstants.NETWORK_MODE_TD_SCDMA_GSM_LTE;
    Int32 NT_MODE_TD_SCDMA_GSM_WCDMA       = RILConstants.NETWORK_MODE_TD_SCDMA_GSM_WCDMA;
    Int32 NT_MODE_TD_SCDMA_WCDMA_LTE       = RILConstants.NETWORK_MODE_TD_SCDMA_WCDMA_LTE;
    Int32 NT_MODE_TD_SCDMA_GSM_WCDMA_LTE   = RILConstants.NETWORK_MODE_TD_SCDMA_GSM_WCDMA_LTE;
    Int32 NT_MODE_TD_SCDMA_CDMA_EVDO_GSM_WCDMA =
            RILConstants.NETWORK_MODE_TD_SCDMA_CDMA_EVDO_GSM_WCDMA;
    Int32 NT_MODE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA =
            RILConstants.NETWORK_MODE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA;

    // Used for CDMA roaming mode
    static const Int32 CDMA_RM_HOME        = 0;  // Home Networks only, as defined in PRL
    static const Int32 CDMA_RM_AFFILIATED  = 1;  // Roaming an Affiliated networks, as defined in PRL
    static const Int32 CDMA_RM_ANY         = 2;  // Roaming on Any Network, as defined in PRL

    // Used for CDMA subscription mode
    static const Int32 CDMA_SUBSCRIPTION_UNKNOWN  =-1; // Unknown
    static const Int32 CDMA_SUBSCRIPTION_RUIM_SIM = 0; // RUIM/SIM (default)
    static const Int32 CDMA_SUBSCRIPTION_NV       = 1; // NV -> non-volatile memory

    static const Int32 PREFERRED_CDMA_SUBSCRIPTION = CDMA_SUBSCRIPTION_NV;

    static const Int32 TTY_MODE_OFF = 0;
    static const Int32 TTY_MODE_FULL = 1;
    static const Int32 TTY_MODE_HCO = 2;
    static const Int32 TTY_MODE_VCO = 3;

     /**
     * CDMA OTA PROVISION STATUS, the same as RIL_CDMA_OTA_Status in ril.h
     */

    public static const Int32 CDMA_OTA_PROVISION_STATUS_SPL_UNLOCKED = 0;
    public static const Int32 CDMA_OTA_PROVISION_STATUS_SPC_RETRIES_EXCEEDED = 1;
    public static const Int32 CDMA_OTA_PROVISION_STATUS_A_KEY_EXCHANGED = 2;
    public static const Int32 CDMA_OTA_PROVISION_STATUS_SSD_UPDATED = 3;
    public static const Int32 CDMA_OTA_PROVISION_STATUS_NAM_DOWNLOADED = 4;
    public static const Int32 CDMA_OTA_PROVISION_STATUS_MDN_DOWNLOADED = 5;
    public static const Int32 CDMA_OTA_PROVISION_STATUS_IMSI_DOWNLOADED = 6;
    public static const Int32 CDMA_OTA_PROVISION_STATUS_PRL_DOWNLOADED = 7;
    public static const Int32 CDMA_OTA_PROVISION_STATUS_COMMITTED = 8;
    public static const Int32 CDMA_OTA_PROVISION_STATUS_OTAPA_STARTED = 9;
    public static const Int32 CDMA_OTA_PROVISION_STATUS_OTAPA_STOPPED = 10;
    public static const Int32 CDMA_OTA_PROVISION_STATUS_OTAPA_ABORTED = 11;


    /**
     * Get the current ServiceState. Use
     * <code>registerForServiceStateChanged</code> to be informed of
     * updates.
     */
    ServiceState GetServiceState();

    /**
     * Get the current CellLocation.
     */
    CellLocation GetCellLocation();

    /**
     * @return all available cell information or NULL if none.
     */
    public List<CellInfo> GetAllCellInfo();

    /**
     * Sets the minimum time in milli-seconds between {@link PhoneStateListener#onCellInfoChanged
     * PhoneStateListener.onCellInfoChanged} will be invoked.
     *
     * The default, 0, means invoke onCellInfoChanged when any of the reported
     * information changes. Setting the value to INT_MAX(0x7fffffff) means never issue
     * A onCellInfoChanged.
     *
     * @param rateInMillis the rate
     */
    CARAPI SetCellInfoListRate(Int32 rateInMillis);

    /**
     * Get the current for the default apn DataState. No change notification
     * exists at this interface -- use
     * {@link android.telephony.PhoneStateListener} instead.
     */
    DataState GetDataConnectionState();

    /**
     * Get the current DataState. No change notification exists at this
     * interface -- use
     * {@link android.telephony.PhoneStateListener} instead.
     * @param apnType specify for which apn to get connection state info.
     */
    DataState GetDataConnectionState(String apnType);

    /**
     * Get the current DataActivityState. No change notification exists at this
     * interface -- use
     * {@link android.telephony.TelephonyManager} instead.
     */
    DataActivityState GetDataActivityState();

    /**
     * Gets the context for the phone, as set at initialization time.
     */
    Context GetContext();

    /**
     * Disables the DNS Check (i.e., allows "0.0.0.0").
     * Useful for lab testing environment.
     * @param b TRUE disables the check, FALSE enables.
     */
    void DisableDnsCheck(Boolean b);

    /**
     * Returns TRUE if the DNS check is currently disabled.
     */
    Boolean IsDnsCheckDisabled();

    /**
     * Get current coarse-grained voice call state.
     * Use {@link #RegisterForPreciseCallStateChanged(Handler, Int32, Object)
     * RegisterForPreciseCallStateChanged()} for change notification. <p>
     * If the phone has an active call and call waiting occurs,
     * then the phone state is RINGING not OFFHOOK
     * <strong>Note:</strong>
     * This registration point provides notification of finer-grained
     * changes.<p>
     *
     */
    State GetState();

    /**
     * Returns a string identifier for this phone interface for parties
     *  outside the phone app process.
     *  @return The string name.
     */
    String GetPhoneName();

    /**
     * Return a numerical identifier for the phone radio interface.
     * @return PHONE_TYPE_XXX as defined above.
     */
    Int32 GetPhoneType();

    /**
     * Returns an array of string identifiers for the APN types serviced by the
     * currently active.
     *  @return The string array will always return at least one entry, Phone.APN_TYPE_DEFAULT.
     * TODO: Revisit if we always should return at least one entry.
     */
    String[] GetActiveApnTypes();

    /**
     * Returns string for the active APN host.
     *  @return type as a string or NULL if none.
     */
    String GetActiveApnHost(String apnType);

    /**
     * Return the LinkProperties for the named apn or NULL if not available
     */
    LinkProperties GetLinkProperties(String apnType);

    /**
     * Return the NetworkCapabilities
     */
    NetworkCapabilities GetNetworkCapabilities(String apnType);

    /**
     * Get current signal strength. No change notification available on this
     * interface. Use <code>PhoneStateNotifier</code> or an equivalent.
     * An ASU is 0-31 or -1 if Unknown (for GSM, dBm = -113 - 2 * asu).
     * The following special values are defined:</p>
     * <ul><li>0 means "-113 dBm or less".</li>
     * <li>31 means "-51 dBm or greater".</li></ul>
     *
     * @return Current signal strength as SignalStrength
     */
    SignalStrength GetSignalStrength();

    /**
     * Notifies when a previously untracked non-ringing/waiting connection has appeared.
     * This is likely due to some other Entity (eg, SIM card application) initiating a call.
     */
    void RegisterForUnknownConnection(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for unknown connection notifications.
     */
    void UnregisterForUnknownConnection(Handler h);

    /**
     * Notifies when a Handover happens due to SRVCC or Silent Redial
     */
    void RegisterForHandoverStateChanged(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for handover state notifications
     */
    void UnregisterForHandoverStateChanged(Handler h);
    /**
     * Register for getting notifications for change in the Call State {@link Call.State}
     * This is called PreciseCallState because the call state is more precise than the
     * {@link PhoneConstants.State} which can be obtained using the {@link PhoneStateListener}
     *
     * Resulting events will have an AsyncResult in <code>Message.obj</code>.
     * AsyncResult.userData will be set to the obj argument here.
     * The <em>h</em> parameter is held only by a weak reference.
     */
    void RegisterForPreciseCallStateChanged(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for voice call state change notifications.
     * Extraneous calls are tolerated silently.
     */
    void UnregisterForPreciseCallStateChanged(Handler h);

    /**
     * Notifies when a new ringing or waiting connection has appeared.<p>
     *
     *  Messages received from this:
     *  Message.obj will be an AsyncResult
     *  AsyncResult.userObj = obj
     *  AsyncResult.result = a Connection. <p>
     *  Please check Connection->IsRinging() to make sure the Connection
     *  has not dropped since this message was posted.
     *  If Connection->IsRinging() is TRUE, then
     *   Connection->GetCall() == Phone->GetRingingCall()
     */
    void RegisterForNewRingingConnection(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for new ringing connection notification.
     * Extraneous calls are tolerated silently
     */

    void UnregisterForNewRingingConnection(Handler h);

    /**
     * Notifies when phone's video capabilities changes <p>
     *
     *  Messages received from this:
     *  Message.obj will be an AsyncResult
     *  AsyncResult.userObj = obj
     *  AsyncResult.result = TRUE if phone supports video calling <p>
     */
    CARAPI RegisterForVideoCapabilityChanged(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for video capability changed notification.
     * Extraneous calls are tolerated silently
     */
    CARAPI UnregisterForVideoCapabilityChanged(Handler h);

    /**
     * Notifies when an incoming call rings.<p>
     *
     *  Messages received from this:
     *  Message.obj will be an AsyncResult
     *  AsyncResult.userObj = obj
     *  AsyncResult.result = a Connection. <p>
     */
    void RegisterForIncomingRing(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for ring notification.
     * Extraneous calls are tolerated silently
     */

    void UnregisterForIncomingRing(Handler h);

    /**
     * Notifies when out-band ringback tone is needed.<p>
     *
     *  Messages received from this:
     *  Message.obj will be an AsyncResult
     *  AsyncResult.userObj = obj
     *  AsyncResult.result = Boolean, TRUE to start play ringback tone
     *                       and FALSE to stop. <p>
     */
    void RegisterForRingbackTone(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for ringback tone notification.
     */

    void UnregisterForRingbackTone(Handler h);

    /**
     * Notifies when out-band on-hold tone is needed.<p>
     *
     *  Messages received from this:
     *  Message.obj will be an AsyncResult
     *  AsyncResult.userObj = obj
     *  AsyncResult.result = Boolean, TRUE to start play on-hold tone
     *                       and FALSE to stop. <p>
     */
    void RegisterForOnHoldTone(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for on-hold tone notification.
     */

    void UnregisterForOnHoldTone(Handler h);

    /**
     * Registers the handler to reset the uplink mute state to get
     * uplink audio.
     */
    void RegisterForResendIncallMute(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for resend incall mute notifications.
     */
    void UnregisterForResendIncallMute(Handler h);

    /**
     * Notifies when a voice connection has disconnected, either due to local
     * or remote hangup or error.
     *
     *  Messages received from this will have the following members:<p>
     *  <ul><li>Message.obj will be an AsyncResult</li>
     *  <li>AsyncResult.userObj = obj</li>
     *  <li>AsyncResult.result = a Connection object that is
     *  no longer connected.</li></ul>
     */
    void RegisterForDisconnect(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for voice disconnection notification.
     * Extraneous calls are tolerated silently
     */
    void UnregisterForDisconnect(Handler h);


    /**
     * Register for notifications of initiation of a new MMI code request.
     * MMI codes for GSM are discussed in 3GPP TS 22.030.<p>
     *
     * Example: If Phone.dial is called with "*#31#", then the app will
     * be notified here.<p>
     *
     * The returned <code>Message.obj</code> will contain an AsyncResult.
     *
     * <code>obj.result</code> will be an "MmiCode" object.
     */
    void RegisterForMmiInitiate(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for new MMI initiate notification.
     * Extraneous calls are tolerated silently
     */
    void UnregisterForMmiInitiate(Handler h);

    /**
     * Register for notifications that an MMI request has completed
     * its network activity and is in its final state. This may mean a state
     * of COMPLETE, FAILED, or CANCELLED.
     *
     * <code>Message.obj</code> will contain an AsyncResult.
     * <code>obj.result</code> will be an "MmiCode" object
     */
    void RegisterForMmiComplete(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for MMI complete notification.
     * Extraneous calls are tolerated silently
     */
    void UnregisterForMmiComplete(Handler h);

    /**
     * Registration point for Ecm timer reset
     * @param h handler to notify
     * @param what user-defined message code
     * @param obj placed in Message.obj
     */
    CARAPI RegisterForEcmTimerReset(Handler h, Int32 what, Object obj);

    /**
     * Unregister for notification for Ecm timer reset
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForEcmTimerReset(Handler h);

    /**
     * Returns a list of MMI codes that are pending. (They have initiated
     * but have not yet completed).
     * Presently there is only ever one.
     * Use <code>registerForMmiInitiate</code>
     * and <code>registerForMmiComplete</code> for change notification.
     */
    public List<? extends MmiCode> GetPendingMmiCodes();

    /**
     * Sends user response to a USSD REQUEST message.  An MmiCode instance
     * representing this response is sent to handlers registered with
     * registerForMmiInitiate.
     *
     * @param ussdMessge    Message to send in the response.
     */
    CARAPI SendUssdResponse(String ussdMessge);

    /**
     * Register for ServiceState changed.
     * Message.obj will contain an AsyncResult.
     * AsyncResult.result will be a ServiceState instance
     */
    void RegisterForServiceStateChanged(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for ServiceStateChange notification.
     * Extraneous calls are tolerated silently
     */
    void UnregisterForServiceStateChanged(Handler h);

    /**
     * Register for Supplementary Service notifications from the network.
     * Message.obj will contain an AsyncResult.
     * AsyncResult.result will be a SuppServiceNotification instance.
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForSuppServiceNotification(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for Supplementary Service notifications.
     * Extraneous calls are tolerated silently
     *
     * @param h Handler to be removed from the registrant list.
     */
    void UnregisterForSuppServiceNotification(Handler h);

    /**
     * Register for notifications when a supplementary service attempt fails.
     * Message.obj will contain an AsyncResult.
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForSuppServiceFailed(Handler h, Int32 what, Object obj);

    /**
     * Unregister for notifications when a supplementary service attempt fails.
     * Extraneous calls are tolerated silently
     *
     * @param h Handler to be removed from the registrant list.
     */
    void UnregisterForSuppServiceFailed(Handler h);

    /**
     * Register for notifications when a sInCall VoicePrivacy is enabled
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForInCallVoicePrivacyOn(Handler h, Int32 what, Object obj);

    /**
     * Unegister for notifications when a sInCall VoicePrivacy is enabled
     *
     * @param h Handler to be removed from the registrant list.
     */
    void UnregisterForInCallVoicePrivacyOn(Handler h);

    /**
     * Register for notifications when a sInCall VoicePrivacy is disabled
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForInCallVoicePrivacyOff(Handler h, Int32 what, Object obj);

    /**
     * Unregister for notifications when a sInCall VoicePrivacy is disabled
     *
     * @param h Handler to be removed from the registrant list.
     */
    void UnregisterForInCallVoicePrivacyOff(Handler h);

    /**
     * Register for notifications when CDMA OTA Provision status change
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForCdmaOtaStatusChange(Handler h, Int32 what, Object obj);

    /**
     * Unregister for notifications when CDMA OTA Provision status change
     * @param h Handler to be removed from the registrant list.
     */
    void UnregisterForCdmaOtaStatusChange(Handler h);

    /**
     * Registration point for subscription info ready
     * @param h handler to notify
     * @param what what code of message when delivered
     * @param obj placed in Message.obj
     */
    CARAPI RegisterForSubscriptionInfoReady(Handler h, Int32 what, Object obj);

    /**
     * Unregister for notifications for subscription info
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForSubscriptionInfoReady(Handler h);

    /**
     * Registration point for Sim records loaded
     * @param h handler to notify
     * @param what what code of message when delivered
     * @param obj placed in Message.obj
     */
    CARAPI RegisterForSimRecordsLoaded(Handler h, Int32 what, Object obj);

    /**
     * Unregister for notifications for Sim records loaded
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForSimRecordsLoaded(Handler h);

    /**
     * Returns SIM record load state. Use
     * <code>GetSimCard()->RegisterForReady()</code> for change notification.
     *
     * @return TRUE if records from the SIM have been loaded and are
     * Available (if applicable). If not applicable to the underlying
     * technology, returns TRUE as well.
     */
    Boolean GetIccRecordsLoaded();

    /**
     * Returns the ICC card interface for this phone, or NULL
     * if not applicable to underlying technology.
     */
    IccCard GetIccCard();

    /**
     * Answers a ringing or waiting call. Active calls, if any, go on hold.
     * Answering occurs asynchronously, and final notification occurs via
     * {@link #RegisterForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) RegisterForPreciseCallStateChanged()}.
     *
     * @param videoState The video state in which to answer the call.
     * @exception CallStateException when no call is ringing or waiting
     */
    void AcceptCall(Int32 videoState) throws CallStateException;

    /**
     * Deflects a ringing or waiting IMS call.
     *
     * @param number The number to deflect to.
     * @exception CallStateException when no IMS call is ringing or waiting
     */
    void DeflectCall(String number) throws CallStateException;

    /**
     * Reject (ignore) a ringing call. In GSM, this means UDUB
     * (User Determined User Busy). Reject occurs asynchronously,
     * and final notification occurs via
     * {@link #RegisterForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) RegisterForPreciseCallStateChanged()}.
     *
     * @exception CallStateException when no call is ringing or waiting
     */
    void RejectCall() throws CallStateException;

    /**
     * Places any active calls on hold, and makes any held calls
     *  active. Switch occurs asynchronously and may fail.
     * Final notification occurs via
     * {@link #RegisterForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) RegisterForPreciseCallStateChanged()}.
     *
     * @exception CallStateException if a call is ringing, waiting, or
     * dialing/alerting. In these cases, this operation may not be performed.
     */
    void SwitchHoldingAndActive() throws CallStateException;

    /**
     * Whether or not the phone can conference in the current phone
     * state--that is, one call holding and one call active.
     * @return TRUE if the phone can conference; FALSE otherwise.
     */
    Boolean CanConference();

    /**
     * Conferences holding and active. Conference occurs asynchronously
     * and may fail. Final notification occurs via
     * {@link #RegisterForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) RegisterForPreciseCallStateChanged()}.
     *
     * @exception CallStateException if CanConference() would return FALSE.
     * In these cases, this operation may not be performed.
     */
    void Conference() throws CallStateException;

    /**
     * Enable or disable enhanced Voice Privacy (VP). If enhanced VP is
     * disabled, normal VP is enabled.
     *
     * @param enable whether TRUE or FALSE to enable or disable.
     * @param onComplete a callback message when the action is completed.
     */
    void EnableEnhancedVoicePrivacy(Boolean enable, Message onComplete);

    /**
     * Get the currently set Voice Privacy (VP) mode.
     *
     * @param onComplete a callback message when the action is completed.
     */
    void GetEnhancedVoicePrivacy(Message onComplete);

    /**
     * Whether or not the phone can do explicit call transfer in the current
     * phone state--that is, one call holding and one call active.
     * @return TRUE if the phone can do explicit call transfer; FALSE otherwise.
     */
    Boolean CanTransfer();

    /**
     * Connects the two calls and disconnects the subscriber from both calls
     * Explicit Call Transfer occurs asynchronously
     * and may fail. Final notification occurs via
     * {@link #RegisterForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) RegisterForPreciseCallStateChanged()}.
     *
     * @exception CallStateException if CanTransfer() would return FALSE.
     * In these cases, this operation may not be performed.
     */
    void ExplicitCallTransfer() throws CallStateException;

    /**
     * Clears all DISCONNECTED connections from Call connection lists.
     * Calls that were in the DISCONNECTED state become idle. This occurs
     * synchronously.
     */
    void ClearDisconnected();


    /**
     * Gets the foreground call object, which represents all connections that
     * are dialing or Active (all connections
     * that have their audio path connected).<p>
     *
     * The foreground call is a singleton object. It is constant for the life
     * of this phone. It is never NULL.<p>
     *
     * The foreground call will only ever be in one of these states:
     * IDLE, ACTIVE, DIALING, ALERTING, or DISCONNECTED.
     *
     * State change notification is available via
     * {@link #RegisterForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) RegisterForPreciseCallStateChanged()}.
     */
    Call GetForegroundCall();

    /**
     * Gets the background call object, which represents all connections that
     * are Holding (all connections that have been accepted or connected, but
     * do not have their audio path connected). <p>
     *
     * The background call is a singleton object. It is constant for the life
     * of this phone object . It is never NULL.<p>
     *
     * The background call will only ever be in one of these states:
     * IDLE, HOLDING or DISCONNECTED.
     *
     * State change notification is available via
     * {@link #RegisterForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) RegisterForPreciseCallStateChanged()}.
     */
    Call GetBackgroundCall();

    /**
     * Gets the ringing call object, which represents an incoming
     * Connection (if present) that is pending answer/accept. (This connection
     * may be RINGING or WAITING, and there may be only one.)<p>

     * The ringing call is a singleton object. It is constant for the life
     * of this phone. It is never NULL.<p>
     *
     * The ringing call will only ever be in one of these states:
     * IDLE, INCOMING, WAITING or DISCONNECTED.
     *
     * State change notification is available via
     * {@link #RegisterForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) RegisterForPreciseCallStateChanged()}.
     */
    Call GetRingingCall();

    /**
     * Initiate a new voice connection. This happens asynchronously, so you
     * cannot assume the audio path is Connected (or a call index has been
     * assigned) until PhoneStateChanged notification has occurred.
     *
     * @param dialString The dial string.
     * @param videoState The desired video state for the connection.
     * @exception CallStateException if a new outgoing call is not currently
     * possible because no more call slots exist or a call exists that is
     * dialing, alerting, ringing, or waiting.  Other errors are
     * handled asynchronously.
     */
    Connection Dial(String dialString, Int32 videoState) throws CallStateException;

    /**
     * Initiate a new voice connection. This happens asynchronously, so you
     * cannot assume the audio path is Connected (or a call index has been
     * assigned) until PhoneStateChanged notification has occurred.
     *
     * @param dialString The dial string.
     * @param videoState The desired video state for the connection.
     * @param extras Additional (meta) call information.
     *        OEMs can pass call Extras (additional call info) Bundle
     *        in the following format:
     *        1) All values in the Bundle must be of type String
     *        2) Keys for the values are defined in ImsCallProfile.java
     * @exception CallStateException if a new outgoing call is not currently
     *                possible because no more call slots exist or a call exists
     *                that is dialing, alerting, ringing, or waiting. Other
     *                errors are handled asynchronously.
     */
    Connection Dial(String dialString, Int32 videoState, Bundle extras) throws CallStateException;

    /**
     * Initiate a new voice connection with supplementary User to User
     * Information. This happens asynchronously, so you cannot assume the audio
     * path is Connected (or a call index has been assigned) until
     * PhoneStateChanged notification has occurred.
     *
     * @param dialString The dial string.
     * @param uusInfo The UUSInfo.
     * @param videoState The desired video state for the connection.
     * @exception CallStateException if a new outgoing call is not currently
     *                possible because no more call slots exist or a call exists
     *                that is dialing, alerting, ringing, or waiting. Other
     *                errors are handled asynchronously.
     */
    Connection Dial(String dialString, UUSInfo uusInfo, Int32 videoState) throws CallStateException;

    /**
     * Initiate to add a participant in an IMS call.
     * This happens asynchronously, so you cannot assume the audio path is
     * Connected (or a call index has been assigned) until PhoneStateChanged
     * notification has occurred.
     *
     * @exception CallStateException if a new outgoing call is not currently
     *                possible because no more call slots exist or a call exists
     *                that is dialing, alerting, ringing, or waiting. Other
     *                errors are handled asynchronously.
     */
    CARAPI AddParticipant(String dialString) throws CallStateException;

    /**
     * Handles PIN MMI Commands (PIN/PIN2/PUK/PUK2), which are initiated
     * without SEND (so <code>dial</code> is not appropriate).
     *
     * @param dialString the MMI command to be executed.
     * @return TRUE if MMI command is executed.
     */
    Boolean HandlePinMmi(String dialString);

    /**
     * Handles in-call MMI commands. While in a call, or while receiving a
     * call, use this to execute MMI commands.
     * see 3GPP 20.030, section 6.5.5.1 for specs on the allowed MMI commands.
     *
     * @param command the MMI command to be executed.
     * @return TRUE if the MMI command is executed.
     * @throws CallStateException
     */
    Boolean HandleInCallMmiCommands(String command) throws CallStateException;

    /**
     * Play a DTMF tone on the active call. Ignored if there is no active call.
     * @param c should be one of 0-9, '*' or '#'. Other values will be
     * silently ignored.
     */
    void SendDtmf(Char32 c);

    /**
     * Start to paly a DTMF tone on the active call. Ignored if there is no active call
     * or there is a playing DTMF tone.
     * @param c should be one of 0-9, '*' or '#'. Other values will be
     * silently ignored.
     */
    void StartDtmf(Char32 c);

    /**
     * Stop the playing DTMF tone. Ignored if there is no playing DTMF
     * tone or no active call.
     */
    void StopDtmf();

    /**
     * send burst DTMF tone, it can send the string as single character or multiple character
     * ignore if there is no active call or not valid digits string.
     * Valid digit means only includes characters ISO-LATIN characters 0-9, *, #
     * The difference between sendDtmf and sendBurstDtmf is sendDtmf only sends one character,
     * this api can send single character and multiple character, also, this api has response
     * back to caller.
     *
     * @param dtmfString is string representing the dialing Digit(s) in the active call
     * @param on the DTMF ON length in milliseconds, or 0 for default
     * @param off the DTMF OFF length in milliseconds, or 0 for default
     * @param onComplete is the callback message when the action is processed by BP
     *
     */
    void SendBurstDtmf(String dtmfString, Int32 on, Int32 off, Message onComplete);

    /**
     * Sets the radio power on/off State (off is sometimes
     * called "airplane mode"). Current state can be gotten via
     * {@link #GetServiceState()}.{@link
     * android.telephony.ServiceState#GetState() GetState()}.
     * <strong>Note: </strong>This request is asynchronous.
     * GetServiceState()->GetState() will not change immediately after this call.
     * RegisterForServiceStateChanged() to find out when the
     * request is complete.
     *
     * @param power TRUE means "on", FALSE means "off".
     */
    void SetRadioPower(Boolean power);

    /**
     * Get voice message waiting indicator status. No change notification
     * available on this interface. Use PhoneStateNotifier or similar instead.
     *
     * @return TRUE if there is a voice message waiting
     */
    Boolean GetMessageWaitingIndicator();

    /**
     * Get voice call forwarding indicator status. No change notification
     * available on this interface. Use PhoneStateNotifier or similar instead.
     *
     * @return TRUE if there is a voice call forwarding
     */
    Boolean GetCallForwardingIndicator();

    /**
     * Get the line 1 phone Number (MSISDN). For CDMA phones, the MDN is returned
     * and {@link #GetMsisdn()} will return the MSISDN on CDMA/LTE phones.<p>
     *
     * @return phone number. May return NULL if not
     * available or the SIM is not ready
     */
    String GetLine1Number();

    /**
     * Returns the alpha tag associated with the msisdn number.
     * If there is no alpha tag associated or the record is not yet available,
     * returns a default localized string. <p>
     */
    String GetLine1AlphaTag();

    /**
     * Sets the MSISDN phone number in the SIM card.
     *
     * @param alphaTag the alpha tag associated with the MSISDN phone number
     *        (see getMsisdnAlphaTag)
     * @param number the new MSISDN phone number to be set on the SIM.
     * @param onComplete a callback message when the action is completed.
     */
    void SetLine1Number(String alphaTag, String number, Message onComplete);

    /**
     * Get the voice mail access phone number. Typically dialed when the
     * user holds the "1" key in the phone app. May return NULL if not
     * available or the SIM is not ready.<p>
     */
    String GetVoiceMailNumber();

    /**
     * Returns unread voicemail count. This count is shown when the  voicemail
     * notification is expanded.<p>
     */
    Int32 GetVoiceMessageCount();

    /**
     * Returns the alpha tag associated with the voice mail number.
     * If there is no alpha tag associated or the record is not yet available,
     * returns a default localized string. <p>
     *
     * Please use this value instead of some other localized string when
     * showing a name for this number in the UI. For example, call log
     * entries should show this alpha tag. <p>
     *
     * Usage of this alpha tag in the UI is a common carrier requirement.
     */
    String GetVoiceMailAlphaTag();

    /**
     * setVoiceMailNumber
     * sets the voicemail number in the SIM card.
     *
     * @param alphaTag the alpha tag associated with the voice mail number
     *        (see getVoiceMailAlphaTag)
     * @param voiceMailNumber the new voicemail number to be set on the SIM.
     * @param onComplete a callback message when the action is completed.
     */
    void SetVoiceMailNumber(String alphaTag,
                            String voiceMailNumber,
                            Message onComplete);

    /**
     * getCallForwardingOptions
     * gets a call forwarding option. The return value of
     * ((AsyncResult)onComplete.obj) is an array of CallForwardInfo.
     *
     * @param commandInterfaceCFReason is one of the valid call forwarding
     *        CF_REASONS, as defined in
     *        <code>com.android.internal.telephony.CommandsInterface.</code>
     * @param onComplete a callback message when the action is completed.
     *        @see com.android.internal.telephony.CallForwardInfo for details.
     */
    void GetCallForwardingOption(Int32 commandInterfaceCFReason,
                                  Message onComplete);

    /**
     * getCallForwardingUncondTimerOptions
     * gets a call forwarding option. The return value of
     * ((AsyncResult)onComplete.obj) is an array of CallForwardInfo.
     *
     * @param commandInterfaceCFReason is one of the valid call forwarding
     *        CF_REASONS, as defined in
     *        <code>com.android.internal.telephony.CommandsInterface.</code>
     * @param onComplete a callback message when the action is completed.
     *        @see com.android.internal.telephony.CallForwardInfo for details.
     */
    void GetCallForwardingUncondTimerOption(Int32 commandInterfaceCFReason,
                                  Message onComplete);

    /**
     * setCallForwardingOptions
     * sets a call forwarding option.
     *
     * @param commandInterfaceCFReason is one of the valid call forwarding
     *        CF_REASONS, as defined in
     *        <code>com.android.internal.telephony.CommandsInterface.</code>
     * @param commandInterfaceCFAction is one of the valid call forwarding
     *        CF_ACTIONS, as defined in
     *        <code>com.android.internal.telephony.CommandsInterface.</code>
     * @param dialingNumber is the target phone number to forward calls to
     * @param timerSeconds is used by CFNRy to indicate the timeout before
     *        forwarding is attempted.
     * @param onComplete a callback message when the action is completed.
     */
    void SetCallForwardingOption(Int32 commandInterfaceCFReason,
                                 Int32 commandInterfaceCFAction,
                                 String dialingNumber,
                                 Int32 timerSeconds,
                                 Message onComplete);

    /**
     * setCallForwardingUncondTimerOptions
     * sets a call forwarding unconditional Timer option.
     *
     * @param startHour indicates starting hour
     * @param startMinute indicates starting minute
     * @param endHour indicates ending hour
     * @param endMinute indicates ending minute
     * @param commandInterfaceCFReason is one of the valid call forwarding
     *        CF_REASONS, as defined in
     *        <code>com.android.internal.telephony.CommandsInterface.</code>
     * @param commandInterfaceCFAction is one of the valid call forwarding
     *        CF_ACTIONS, as defined in
     *        <code>com.android.internal.telephony.CommandsInterface.</code>
     * @param dialingNumber is the target phone number to forward calls to
     * @param onComplete a callback message when the action is completed.
     */
    void SetCallForwardingUncondTimerOption(Int32 startHour, Int32 startMinute,
                                 Int32 endHour, Int32 endMinute,
                                 Int32 commandInterfaceCFReason,
                                 Int32 commandInterfaceCFAction,
                                 String dialingNumber,
                                 Message onComplete);

    /**
     * getOutgoingCallerIdDisplay
     * gets outgoing caller id display. The return value of
     * ((AsyncResult)onComplete.obj) is an array of Int32, with a length of 2.
     *
     * @param onComplete a callback message when the action is completed.
     *        @see com.android.internal.telephony.CommandsInterface#getCLIR for details.
     */
    void GetOutgoingCallerIdDisplay(Message onComplete);

    /**
     * setOutgoingCallerIdDisplay
     * sets a call forwarding option.
     *
     * @param commandInterfaceCLIRMode is one of the valid call CLIR
     *        modes, as defined in
     *        <code>com.android.internal.telephony.CommandsInterface./code>
     * @param onComplete a callback message when the action is completed.
     */
    void SetOutgoingCallerIdDisplay(Int32 commandInterfaceCLIRMode,
                                    Message onComplete);

    /**
     * getCallWaiting
     * gets call waiting activation state. The return value of
     * ((AsyncResult)onComplete.obj) is an array of Int32, with a length of 1.
     *
     * @param onComplete a callback message when the action is completed.
     *        @see com.android.internal.telephony.CommandsInterface#queryCallWaiting for details.
     */
    void GetCallWaiting(Message onComplete);

    /**
     * setCallWaiting
     * sets a call forwarding option.
     *
     * @param enable is a Boolean representing the state that you are
     *        requesting, TRUE for enabled, FALSE for disabled.
     * @param onComplete a callback message when the action is completed.
     */
    void SetCallWaiting(Boolean enable, Message onComplete);

    /**
     * Scan available networks. This method is asynchronous; .
     * On completion, <code>response.obj</code> is set to an AsyncResult with
     * one of the following members:.<p>
     *<ul>
     * <li><code>response.obj.result</code> will be a <code>List</code> of
     * <code>OperatorInfo</code> objects, or</li>
     * <li><code>response.obj.exception</code> will be set with an exception
     * on failure.</li>
     * </ul>
     */
    void GetAvailableNetworks(Message response);

    /**
     * Switches network selection mode to "automatic", re-scanning and
     * re-selecting a network if appropriate.
     *
     * @param response The message to dispatch when the network selection
     * is complete.
     *
     * @see #SelectNetworkManually(OperatorInfo, android.os.Message )
     */
    void SetNetworkSelectionModeAutomatic(Message response);

    /**
     * Manually selects a network. <code>response</code> is
     * dispatched when this is complete.  <code>response.obj</code> will be
     * an AsyncResult, and <code>response.obj.exception</code> will be non-NULL
     * on failure.
     *
     * @see #SetNetworkSelectionModeAutomatic(Message)
     */
    void SelectNetworkManually(OperatorInfo network,
                            Message response);

    /**
     *  Requests to set the preferred network type for searching and registering
     * (CS/PS domain, RAT, and operation mode)
     * @param networkType one of  NT_*_TYPE
     * @param response is callback message
     */
    void SetPreferredNetworkType(Int32 networkType, Message response);

    /**
     *  Query the preferred network type setting
     *
     * @param response is callback message to report one of  NT_*_TYPE
     */
    void GetPreferredNetworkType(Message response);

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
     * Query neighboring cell IDs.  <code>response</code> is dispatched when
     * this is complete.  <code>response.obj</code> will be an AsyncResult,
     * and <code>response.obj.exception</code> will be non-NULL on failure.
     * On success, <code>AsyncResult.result</code> will be a <code>String[]</code>
     * containing the neighboring cell IDs.  Index 0 will contain the count
     * of available cell IDs.  Cell IDs are in hexadecimal format.
     *
     * @param response callback message that is dispatched when the query
     * completes.
     */
    void GetNeighboringCids(Message response);

    /**
     * Sets an event to be fired when the telephony system processes
     * a post-dial character on an outgoing call.<p>
     *
     * Messages of type <code>what</code> will be sent to <code>h</code>.
     * The <code>obj</code> field of these Message's will be instances of
     * <code>AsyncResult</code>. <code>Message.obj.result</code> will be
     * a Connection object.<p>
     *
     * Message.arg1 will be the post dial character being processed,
     * or 0 ('\0') if end of string.<p>
     *
     * If Connection->GetPostDialState() == WAIT,
     * the application must call
     * {@link com.android.internal.telephony.Connection#ProceedAfterWaitChar()
     * Connection->ProceedAfterWaitChar()} or
     * {@link com.android.internal.telephony.Connection#CancelPostDial()
     * Connection->CancelPostDial()}
     * for the telephony system to continue playing the post-dial
     * DTMF sequence.<p>
     *
     * If Connection->GetPostDialState() == WILD,
     * the application must call
     * {@link com.android.internal.telephony.Connection#proceedAfterWildChar
     * Connection->ProceedAfterWildChar()}
     * or
     * {@link com.android.internal.telephony.Connection#CancelPostDial()
     * Connection->CancelPostDial()}
     * for the telephony system to continue playing the
     * post-dial DTMF sequence.<p>
     *
     * Only one post dial character handler may be set. <p>
     * Calling this method with "h" equal to NULL unsets this handler.<p>
     */
    void SetOnPostDialCharacter(Handler h, Int32 what, Object obj);


    /**
     * Mutes or unmutes the microphone for the active call. The microphone
     * is automatically unmuted if a call is answered, dialed, or resumed
     * from a holding state.
     *
     * @param muted TRUE to mute the microphone,
     * FALSE to activate the microphone.
     */

    void SetMute(Boolean muted);

    /**
     * Gets current mute status. Use
     * {@link #RegisterForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) RegisterForPreciseCallStateChanged()}
     * as a change notifcation, although presently phone state changed is not
     * fired when SetMute() is called.
     *
     * @return TRUE is muting, FALSE is unmuting
     */
    Boolean GetMute();

    /**
     * Enables or disables echo suppression.
     */
    void SetEchoSuppressionEnabled();

    /**
     * Invokes RIL_REQUEST_OEM_HOOK_RAW on RIL implementation.
     *
     * @param data The data for the request.
     * @param response <strong>On success</strong>,
     * (Byte[])(((AsyncResult)response.obj).result)
     * <strong>On failure</strong>,
     * (((AsyncResult)response.obj).result) == NULL and
     * (((AsyncResult)response.obj).exception) being an instance of
     * com.android.internal.telephony.gsm.CommandException
     *
     * @see #InvokeOemRilRequestRaw(Byte[], android.os.Message)
     */
    void InvokeOemRilRequestRaw(Byte[] data, Message response);

    /**
     * Invokes RIL_REQUEST_OEM_HOOK_Strings on RIL implementation.
     *
     * @param strings The strings to make available as the request data.
     * @param response <strong>On success</strong>, "response" bytes is
     * made available as:
     * (String[])(((AsyncResult)response.obj).result).
     * <strong>On failure</strong>,
     * (((AsyncResult)response.obj).result) == NULL and
     * (((AsyncResult)response.obj).exception) being an instance of
     * com.android.internal.telephony.gsm.CommandException
     *
     * @see #InvokeOemRilRequestStrings(java.lang.String[], android.os.Message)
     */
    void InvokeOemRilRequestStrings(String[] strings, Message response);

    /**
     * Get the current active Data Call list
     *
     * @param response <strong>On success</strong>, "response" bytes is
     * made available as:
     * (String[])(((AsyncResult)response.obj).result).
     * <strong>On failure</strong>,
     * (((AsyncResult)response.obj).result) == NULL and
     * (((AsyncResult)response.obj).exception) being an instance of
     * com.android.internal.telephony.gsm.CommandException
     */
    void GetDataCallList(Message response);

    /**
     * Update the ServiceState CellLocation for current network registration.
     */
    void UpdateServiceLocation();

    /**
     * Enable location update notifications.
     */
    void EnableLocationUpdates();

    /**
     * Disable location update notifications.
     */
    void DisableLocationUpdates();

    /**
     * For unit tests; don't send notifications to "Phone"
     * mailbox registrants if TRUE.
     */
    void SetUnitTestMode(Boolean f);

    /**
     * @return TRUE If unit test mode is enabled
     */
    Boolean GetUnitTestMode();

    /**
     * Assign a specified band for RF configuration.
     *
     * @param bandMode one of BM_*_BAND
     * @param response is callback message
     */
    void SetBandMode(Int32 bandMode, Message response);

    /**
     * Query the list of band mode supported by RF.
     *
     * @param response is callback message
     *        ((AsyncResult)response.obj).result  is an Int32[] where Int32[0] is
     *        the size of the array and the rest of each element representing
     *        one available BM_*_BAND
     */
    void QueryAvailableBandMode(Message response);

    /**
     * @return TRUE if enable data connection on roaming
     */
    Boolean GetDataRoamingEnabled();

    /**
     * @param enable set TRUE if enable data connection on roaming
     */
    void SetDataRoamingEnabled(Boolean enable);

    /**
     * @return TRUE if user has enabled data
     */
    Boolean GetDataEnabled();

    /**
     * @param @enable set {@code TRUE} if enable data connection
     */
    void SetDataEnabled(Boolean enable);

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
    void SetCdmaSubscription(Int32 cdmaSubscriptionType, Message response);

    /**
     * If this is a simulated phone interface, returns a SimulatedRadioControl.
     * @return SimulatedRadioControl if this is a simulated interface;
     * otherwise, NULL.
     */
    SimulatedRadioControl GetSimulatedRadioControl();

    /**
     * Report on whether data connectivity is allowed.
     */
    Boolean IsDataConnectivityPossible();

    /**
     * Report on whether on-demand data connectivity is allowed.
     */
    Boolean IsOnDemandDataPossible(String apnType);

    /**
     * Report on whether data connectivity is allowed for an APN.
     */
    Boolean IsDataConnectivityPossible(String apnType);

    /**
     * Retrieves the unique device ID, e.g., IMEI for GSM phones and MEID for CDMA phones.
     */
    String GetDeviceId();

    /**
     * Retrieves the software version number for the device, e.g., IMEI/SV
     * for GSM phones.
     */
    String GetDeviceSvn();

    /**
     * Retrieves the unique subscriber ID, e.g., IMSI for GSM phones.
     */
    String GetSubscriberId();

    /**
     * Retrieves the Group Identifier Level1 for GSM phones.
     */
    String GetGroupIdLevel1();

    /**
     * Retrieves the serial number of the ICC, if applicable.
     */
    String GetIccSerialNumber();

    /* CDMA support methods */

    /**
     * Retrieves the MIN for CDMA phones.
     */
    String GetCdmaMin();

    /**
     * Check if subscription data has been assigned to mMin
     *
     * return TRUE if MIN info is ready; FALSE otherwise.
     */
    Boolean IsMinInfoReady();

    /**
     *  Retrieves PRL Version for CDMA phones
     */
    String GetCdmaPrlVersion();

    /**
     * Retrieves the ESN for CDMA phones.
     */
    String GetEsn();

    /**
     * Retrieves MEID for CDMA phones.
     */
    String GetMeid();

    /**
     * Retrieves the MSISDN from the UICC. For GSM/UMTS phones, this is equivalent to
     * {@link #GetLine1Number()}. For CDMA phones, {@link #GetLine1Number()} returns
     * the MDN, so this method is provided to return the MSISDN on CDMA/LTE phones.
     */
    String GetMsisdn();

    /**
     * Retrieves IMEI for phones. Returns NULL if IMEI is not set.
     */
    String GetImei();

    /**
     * Retrieves the PhoneSubInfo of the Phone
     */
    public PhoneSubInfo GetPhoneSubInfo();

    /**
     * Retrieves the IccPhoneBookInterfaceManager of the Phone
     */
    public IccPhoneBookInterfaceManager GetIccPhoneBookInterfaceManager();

    /**
     * setTTYMode
     * sets a TTY mode option.
     * @param ttyMode is a one of the following:
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_OFF}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_FULL}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_HCO}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_VCO}
     * @param onComplete a callback message when the action is completed
     */
    void SetTTYMode(Int32 ttyMode, Message onComplete);

   /**
     * setUiTTYMode
     * sets a TTY mode option.
     * @param ttyMode is a one of the following:
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_OFF}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_FULL}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_HCO}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_VCO}
     * @param onComplete a callback message when the action is completed
     */
    void SetUiTTYMode(Int32 uiTtyMode, Message onComplete);

    /**
     * queryTTYMode
     * query the status of the TTY mode
     *
     * @param onComplete a callback message when the action is completed.
     */
    void QueryTTYMode(Message onComplete);

    /**
     * Activate or deactivate cell broadcast SMS.
     *
     * @param activate
     *            0 = activate, 1 = deactivate
     * @param response
     *            Callback message is empty on completion
     */
    void ActivateCellBroadcastSms(Int32 activate, Message response);

    /**
     * Query the current configuration of cdma cell broadcast SMS.
     *
     * @param response
     *            Callback message is empty on completion
     */
    void GetCellBroadcastSmsConfig(Message response);

    /**
     * Configure cell broadcast SMS.
     *
     * TODO: Change the configValuesArray to a RIL_BroadcastSMSConfig
     *
     * @param response
     *            Callback message is empty on completion
     */
    CARAPI SetCellBroadcastSmsConfig(Int32[] configValuesArray, Message response);

    CARAPI NotifyDataActivity();

    /**
     * Returns the CDMA ERI icon index to display
     */
    public Int32 GetCdmaEriIconIndex();

    /**
     * Returns the CDMA ERI icon mode,
     * 0 - ON
     * 1 - FLASHING
     */
    public Int32 GetCdmaEriIconMode();

    /**
     * Returns the CDMA ERI text,
     */
    public String GetCdmaEriText();

    /**
     * request to exit emergency call back mode
     * the caller should use setOnECMModeExitResponse
     * to receive the emergency callback mode exit response
     */
    void ExitEmergencyCallbackMode();

    /**
     * this decides if the dial number is OTA(Over the air provision) number or not
     * @param dialStr is string representing the dialing Digit(s)
     * @return  TRUE means the dialStr is OTA number, and FALSE means the dialStr is not OTA number
     */
    Boolean IsOtaSpNumber(String dialStr);

    /**
     * Returns TRUE if OTA Service Provisioning needs to be performed.
     */
    Boolean NeedsOtaServiceProvisioning();

    /**
     * Register for notifications when CDMA call waiting comes
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForCallWaiting(Handler h, Int32 what, Object obj);

    /**
     * Unegister for notifications when CDMA Call waiting comes
     * @param h Handler to be removed from the registrant list.
     */
    void UnregisterForCallWaiting(Handler h);


    /**
     * Register for signal information notifications from the network.
     * Message.obj will contain an AsyncResult.
     * AsyncResult.result will be a SuppServiceNotification instance.
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */

    void RegisterForSignalInfo(Handler h, Int32 what, Object obj) ;
    /**
     * Unregisters for signal information notifications.
     * Extraneous calls are tolerated silently
     *
     * @param h Handler to be removed from the registrant list.
     */
    void UnregisterForSignalInfo(Handler h);

    /**
     * Register for display information notifications from the network.
     * Message.obj will contain an AsyncResult.
     * AsyncResult.result will be a SuppServiceNotification instance.
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForDisplayInfo(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for display information notifications.
     * Extraneous calls are tolerated silently
     *
     * @param h Handler to be removed from the registrant list.
     */
    void UnregisterForDisplayInfo(Handler h) ;

    /**
     * Register for CDMA number information record notification from the network.
     * Message.obj will contain an AsyncResult.
     * AsyncResult.result will be a CdmaInformationRecords.CdmaNumberInfoRec
     * instance.
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForNumberInfo(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for number information record notifications.
     * Extraneous calls are tolerated silently
     *
     * @param h Handler to be removed from the registrant list.
     */
    void UnregisterForNumberInfo(Handler h);

    /**
     * Register for CDMA redirected number information record notification
     * from the network.
     * Message.obj will contain an AsyncResult.
     * AsyncResult.result will be a CdmaInformationRecords.CdmaRedirectingNumberInfoRec
     * instance.
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForRedirectedNumberInfo(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for redirected number information record notification.
     * Extraneous calls are tolerated silently
     *
     * @param h Handler to be removed from the registrant list.
     */
    void UnregisterForRedirectedNumberInfo(Handler h);

    /**
     * Register for CDMA line control information record notification
     * from the network.
     * Message.obj will contain an AsyncResult.
     * AsyncResult.result will be a CdmaInformationRecords.CdmaLineControlInfoRec
     * instance.
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForLineControlInfo(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for line control information notifications.
     * Extraneous calls are tolerated silently
     *
     * @param h Handler to be removed from the registrant list.
     */
    void UnregisterForLineControlInfo(Handler h);

    /**
     * Register for CDMA T53 CLIR information record notifications
     * from the network.
     * Message.obj will contain an AsyncResult.
     * AsyncResult.result will be a CdmaInformationRecords.CdmaT53ClirInfoRec
     * instance.
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterFoT53ClirlInfo(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for T53 CLIR information record notification
     * Extraneous calls are tolerated silently
     *
     * @param h Handler to be removed from the registrant list.
     */
    void UnregisterForT53ClirInfo(Handler h);

    /**
     * Register for CDMA T53 audio control information record notifications
     * from the network.
     * Message.obj will contain an AsyncResult.
     * AsyncResult.result will be a CdmaInformationRecords.CdmaT53AudioControlInfoRec
     * instance.
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    void RegisterForT53AudioControlInfo(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for T53 audio control information record notifications.
     * Extraneous calls are tolerated silently
     *
     * @param h Handler to be removed from the registrant list.
     */
    void UnregisterForT53AudioControlInfo(Handler h);

    /**
     * registers for exit emergency call back mode request response
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */

    void SetOnEcbModeExitResponse(Handler h, Int32 what, Object obj);

    /**
     * Unregisters for exit emergency call back mode request response
     *
     * @param h Handler to be removed from the registrant list.
     */
    void UnsetOnEcbModeExitResponse(Handler h);

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
     * Return if the current radio is LTE on GSM
     * @hide
     */
    public Int32 GetLteOnGsmMode();

    /**
     * TODO: Adding a function for each property is not good.
     * A fucntion of type GetPhoneProp(propType) where propType is an
     * enum of GSM+CDMA+LTE props would be a better approach.
     *
     * Get "Restriction of menu options for manual PLMN selection" bit
     * status from EF_CSP data, this belongs to "Value Added Services Group".
     * @return TRUE if this bit is set or EF_CSP data is unavailable,
     * FALSE otherwise
     */
    Boolean IsCspPlmnEnabled();

    /* Checks if manual network selection is allowed
     * @return TRUE if manual network selection is allowed
     * @return FALSE if manual network selection is not allowed
     */
    public Boolean IsManualNetSelAllowed();

    /**
     * Return an interface to retrieve the ISIM records for IMS, if available.
     * @return the interface to retrieve the ISIM records, or NULL if not supported
     */
    IsimRecords GetIsimRecords();

    /**
     * Sets the SIM voice message waiting indicator records.
     * @param line GSM Subscriber Profile Number, one-based. Only '1' is supported
     * @param countWaiting The number of messages waiting, if known. Use
     *                     -1 to indicate that an unknown number of
     *                      messages are waiting
     */
    void SetVoiceMessageWaiting(Int32 line, Int32 countWaiting);

    /**
     * Gets the USIM service table from the UICC, if present and available.
     * @return an interface to the UsimServiceTable record, or NULL if not available
     */
    UsimServiceTable GetUsimServiceTable();

    /**
     * Gets the Uicc card corresponding to this phone.
     * @return the UiccCard object corresponding to the phone ID.
     */
    UiccCard GetUiccCard();

    /**
     * Unregister from all events it registered for and dispose objects
     * created by this object.
     */
    void Dispose();

    /**
     * Remove references to external object stored in this object.
     */
    void RemoveReferences();

    void GetCallBarringOption(String facility, String password, Message onComplete);

    void SetCallBarringOption(String facility, Boolean lockState, String password,
            Message onComplete);

    void RequestChangeCbPsw(String facility, String oldPwd, String newPwd, Message result);

    /**
     * Update the phone object if the voice radio technology has changed
     *
     * @param voiceRadioTech The new voice radio technology
     */
    void UpdatePhoneObject(Int32 voiceRadioTech);

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

    /*
     * Returns the subscription id.
     */
    public Int64 GetSubId();

    /*
     * Returns the phone id.
     */
    public Int32 GetPhoneId();

    /**
     * Get P-CSCF address from PCO after data connection is established or modified.
     * @param apnType the apnType, "ims" for IMS APN, "emergency" for EMERGENCY APN
     */
    public String[] GetPcscfAddress(String apnType);

    /**
     * Set IMS registration state
     */
    CARAPI SetImsRegistrationState(Boolean registered);

    /**
     * Return the ImsPhone phone co-managed with this phone
     * @return an instance of an ImsPhone phone
     */
    public Phone GetImsPhone();

    /**
     * Release the local instance of the ImsPhone and disconnect from
     * the phone.
     * @return the instance of the ImsPhone phone previously owned
     */
    public ImsPhone RelinquishOwnershipOfImsPhone();

    /**
     * Take ownership and wire-up the input ImsPhone
     * @param imsPhone ImsPhone to be used.
     */
    CARAPI AcquireOwnershipOfImsPhone(ImsPhone imsPhone);

    /**
     * Return the service state of mImsPhone if it is STATE_IN_SERVICE
     * otherwise return the current voice service state
     */
    Int32 GetVoicePhoneServiceState();

    /**
     * Override the service provider name and the operator name for the current ICCID.
     */
    public Boolean SetOperatorBrandOverride(String brand);

    /**
     * Is Radio Present on the device and is it accessible
     */
    public Boolean IsRadioAvailable();

    /**
     * shutdown Radio gracefully
     */
    CARAPI ShutdownRadio();

    /** Request to update the current local call hold state.
     * @param lchStatus, TRUE if call is in lch state
     */
    CARAPI SetLocalCallHold(Int32 lchStatus);

    public Boolean IsImsVtCallPresent();
}
