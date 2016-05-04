/*
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

package com.android.internal.telephony.uicc;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;

using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppState;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppType;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ISubscriptionController;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Nio::IByteBuffer;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

/**
 * {@hide}
 */
public abstract class IccRecords extends Handler implements IccConstants {
    protected static const Boolean DBG = TRUE;

    // ***** Instance Variables
    protected AtomicBoolean mDestroyed = new AtomicBoolean(FALSE);
    protected Context mContext;
    protected CommandsInterface mCi;
    protected IccFileHandler mFh;
    protected UiccCardApplication mParentApp;

    protected RegistrantList mRecordsLoadedRegistrants = new RegistrantList();
    protected RegistrantList mImsiReadyRegistrants = new RegistrantList();
    protected RegistrantList mRecordsEventsRegistrants = new RegistrantList();
    protected RegistrantList mNewSmsRegistrants = new RegistrantList();
    protected RegistrantList mNetworkSelectionModeAutomaticRegistrants = new RegistrantList();

    protected Int32 mRecordsToLoad;  // number of pending load requests

    protected AdnRecordCache mAdnCache;

    // ***** Cached SIM State; cleared on channel close

    protected Boolean mRecordsRequested = FALSE; // TRUE if we've made requests for the sim records

    protected String mIccId;
    protected String mMsisdn = NULL;  // My mobile number
    protected String mMsisdnTag = NULL;
    protected String mVoiceMailNum = NULL;
    protected String mVoiceMailTag = NULL;
    protected String mNewVoiceMailNum = NULL;
    protected String mNewVoiceMailTag = NULL;
    protected Boolean mIsVoiceMailFixed = FALSE;
    protected String mImsi;
    private IccIoResult auth_rsp;

    protected Int32 mMncLength = UNINITIALIZED;
    protected Int32 mMailboxIndex = 0; // 0 is no mailbox dailing number associated

    protected Int32 mSmsCountOnIcc = -1;

    protected String mSpn;

    protected String mGid1;

    private final Object mLock = new Object();

    // ***** Constants

    // Markers for mncLength
    protected static const Int32 UNINITIALIZED = -1;
    protected static const Int32 UNKNOWN = 0;

    // Bitmasks for SPN display rules.
    public static const Int32 SPN_RULE_SHOW_SPN  = 0x01;
    public static const Int32 SPN_RULE_SHOW_PLMN = 0x02;

    // ***** Event Constants
    protected static const Int32 EVENT_SET_MSISDN_DONE = 30;
    public static const Int32 EVENT_MWI = 0; // Message Waiting indication
    public static const Int32 EVENT_CFI = 1; // Call Forwarding indication
    public static const Int32 EVENT_SPN = 2; // Service Provider Name

    public static const Int32 EVENT_GET_ICC_RECORD_DONE = 100;
    public static const Int32 EVENT_REFRESH = 31; // ICC refresh occurred
    protected static const Int32 EVENT_GET_SMS_RECORD_SIZE_DONE = 28;
    public static const Int32 EVENT_REFRESH_OEM = 29;
    protected static const Int32 EVENT_APP_READY = 1;
    private static const Int32 EVENT_AKA_AUTHENTICATE_DONE          = 90;

    private Boolean mOEMHookSimRefresh = FALSE;

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "mDestroyed=" + mDestroyed
                + " mContext=" + mContext
                + " mCi=" + mCi
                + " mFh=" + mFh
                + " mParentApp=" + mParentApp
                + " recordsLoadedRegistrants=" + mRecordsLoadedRegistrants
                + " mImsiReadyRegistrants=" + mImsiReadyRegistrants
                + " mRecordsEventsRegistrants=" + mRecordsEventsRegistrants
                + " mNewSmsRegistrants=" + mNewSmsRegistrants
                + " mNetworkSelectionModeAutomaticRegistrants="
                        + mNetworkSelectionModeAutomaticRegistrants
                + " recordsToLoad=" + mRecordsToLoad
                + " adnCache=" + mAdnCache
                + " recordsRequested=" + mRecordsRequested
                + " iccid=" + mIccId
                + " msisdn=" + mMsisdn
                + " msisdnTag=" + mMsisdnTag
                + " voiceMailNum=" + mVoiceMailNum
                + " voiceMailTag=" + mVoiceMailTag
                + " newVoiceMailNum=" + mNewVoiceMailNum
                + " newVoiceMailTag=" + mNewVoiceMailTag
                + " isVoiceMailFixed=" + mIsVoiceMailFixed
                + " mImsi=" + mImsi
                + " mncLength=" + mMncLength
                + " mailboxIndex=" + mMailboxIndex
                + " spn=" + mSpn;

    }

    /**
     * Generic ICC record loaded callback. Subclasses can call EF load methods on
     * {@link IccFileHandler} passing a Message for onLoaded with the what field set to
     * {@link #EVENT_GET_ICC_RECORD_DONE} and the obj field set to an instance
     * of this interface. The {@link #handleMessage} method in this class will print a
     * log message using {@link #GetEfName()} and decrement {@link #mRecordsToLoad}.
     *
     * If the record load was successful, {@link #onRecordLoaded} will be called with the result.
     * Otherwise, an error log message will be output by {@link #handleMessage} and
     * {@link #onRecordLoaded} will not be called.
     */
    public interface IccRecordLoaded {
        String GetEfName();
        void OnRecordLoaded(AsyncResult ar);
    }

    // ***** Constructor
    public IccRecords(UiccCardApplication app, Context c, CommandsInterface ci) {
        mContext = c;
        mCi = ci;
        mFh = app->GetIccFileHandler();
        mParentApp = app;
        mOEMHookSimRefresh = mContext->GetResources()->GetBoolean(
                R.bool.config_sim_refresh_for_dual_mode_card);
        If (mOEMHookSimRefresh) {
            mCi->RegisterForSimRefreshEvent(this, EVENT_REFRESH_OEM, NULL);
        } else {
            mCi->RegisterForIccRefresh(this, EVENT_REFRESH, NULL);
        }
    }

    /**
     * Call when the IccRecords object is no longer going to be used.
     */
    CARAPI Dispose() {
        mDestroyed->Set(TRUE);
        If (mOEMHookSimRefresh) {
            mCi->UnregisterForSimRefreshEvent(this);
        } else {
            mCi->UnregisterForIccRefresh(this);
        }
        mParentApp = NULL;
        mFh = NULL;
        mCi = NULL;
        mContext = NULL;
    }

    public abstract void OnReady();

    //***** Public Methods

    /*
     * Called to indicate that anyone could request records
     * in the future after this call, once records are loaded and registrants
     * have been notified. This indication could be used
     * to optimize when to actually fetch records from the card. We
     * don't need to fetch records from the card if it is of no use
     * to anyone
     *
     */
    void RecordsRequired() {
        return;
    }

    public AdnRecordCache GetAdnCache() {
        return mAdnCache;
    }

    public String GetIccId() {
        return mIccId;
    }

    CARAPI RegisterForRecordsLoaded(Handler h, Int32 what, Object obj) {
        If (mDestroyed->Get()) {
            return;
        }

        For (Int32 i = mRecordsLoadedRegistrants->Size() - 1; i >= 0 ; i--) {
            Registrant  r = (Registrant) mRecordsLoadedRegistrants->Get(i);
            Handler rH = r->GetHandler();

            If (rH != NULL && rH == h) {
                return;
            }
        }
        Registrant r = new Registrant(h, what, obj);
        mRecordsLoadedRegistrants->Add(r);

        If (mRecordsToLoad == 0 && mRecordsRequested == TRUE) {
            r->NotifyRegistrant(new AsyncResult(NULL, NULL, NULL));
        }
    }
    CARAPI UnregisterForRecordsLoaded(Handler h) {
        mRecordsLoadedRegistrants->Remove(h);
    }

    CARAPI RegisterForImsiReady(Handler h, Int32 what, Object obj) {
        If (mDestroyed->Get()) {
            return;
        }

        Registrant r = new Registrant(h, what, obj);
        mImsiReadyRegistrants->Add(r);

        If (mImsi != NULL) {
            r->NotifyRegistrant(new AsyncResult(NULL, NULL, NULL));
        }
    }
    CARAPI UnregisterForImsiReady(Handler h) {
        mImsiReadyRegistrants->Remove(h);
    }

    CARAPI RegisterForRecordsEvents(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mRecordsEventsRegistrants->Add(r);

        /* Notify registrant of all the possible events. This is to make sure registrant is
        notified even if event occurred in the past. */
        r->NotifyResult(EVENT_MWI);
        r->NotifyResult(EVENT_CFI);
    }
    CARAPI UnregisterForRecordsEvents(Handler h) {
        mRecordsEventsRegistrants->Remove(h);
    }

    CARAPI RegisterForNewSms(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mNewSmsRegistrants->Add(r);
    }
    CARAPI UnregisterForNewSms(Handler h) {
        mNewSmsRegistrants->Remove(h);
    }

    CARAPI RegisterForNetworkSelectionModeAutomatic(
            Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mNetworkSelectionModeAutomaticRegistrants->Add(r);
    }
    CARAPI UnregisterForNetworkSelectionModeAutomatic(Handler h) {
        mNetworkSelectionModeAutomaticRegistrants->Remove(h);
    }

    /**
     * Get the International Mobile Subscriber ID (IMSI) on a SIM
     * for GSM, UMTS and like networks. Default is NULL if IMSI is
     * not supported or unavailable.
     *
     * @return NULL if SIM is not yet ready or unavailable
     */
    public String GetIMSI() {
        return NULL;
    }

    /**
     * Imsi could be set by ServiceStateTrackers in case of cdma
     * @param imsi
     */
    CARAPI SetImsi(String imsi) {
        mImsi = imsi;
        mImsiReadyRegistrants->NotifyRegistrants();
    }

    public String GetMsisdnNumber() {
        return mMsisdn;
    }

    /**
     * Get the Group Identifier Level 1 (GID1) on a SIM for GSM.
     * @return NULL if SIM is not yet ready
     */
    public String GetGid1() {
        return NULL;
    }

    /**
     * Set subscriber number to SIM record
     *
     * The subscriber number is stored in EF_MSISDN (TS 51.011)
     *
     * When the operation is complete, onComplete will be sent to its handler
     *
     * @param alphaTag alpha-tagging of the dailing Nubmer (up to 10 characters)
     * @param number dailing Nubmer (up to 20 digits)
     *        if the number starts with '+', then set to international TOA
     * @param onComplete
     *        onComplete.obj will be an AsyncResult
     *        ((AsyncResult)onComplete.obj).exception == NULL on success
     *        ((AsyncResult)onComplete.obj).exception != NULL on fail
     */
    CARAPI SetMsisdnNumber(String alphaTag, String number,
            Message onComplete) {

        mMsisdn = number;
        mMsisdnTag = alphaTag;

        If (DBG) Log("Set MSISDN: " + mMsisdnTag +" " + mMsisdn);


        AdnRecord adn = new AdnRecord(mMsisdnTag, mMsisdn);

        new AdnRecordLoader(mFh).UpdateEF(adn, EF_MSISDN, EF_EXT1, 1, NULL,
                ObtainMessage(EVENT_SET_MSISDN_DONE, onComplete));
    }

    public String GetMsisdnAlphaTag() {
        return mMsisdnTag;
    }

    public String GetVoiceMailNumber() {
        return mVoiceMailNum;
    }

    /**
     * Return Service Provider Name stored in SIM (EF_SPN=0x6F46) or in RUIM (EF_RUIM_SPN=0x6F41).
     *
     * @return NULL if SIM is not yet ready or no RUIM entry
     */
    public String GetServiceProviderName() {
        String providerName = mSpn;

        // Check for NULL pointers, mParentApp can be NULL after dispose,
        // which did occur after removing a SIM.
        UiccCardApplication parentApp = mParentApp;
        If (parentApp != NULL) {
            UiccCard card = parentApp->GetUiccCard();
            If (card != NULL) {
                String brandOverride = card->GetOperatorBrandOverride();
                If (brandOverride != NULL) {
                    Log("getServiceProviderName: override");
                    providerName = brandOverride;
                } else {
                    Log("getServiceProviderName: no brandOverride");
                }
            } else {
                Log("getServiceProviderName: card is NULL");
            }
        } else {
            Log("getServiceProviderName: mParentApp is NULL");
        }
        Log("getServiceProviderName: providerName=" + providerName);
        return providerName;
    }

    protected void SetServiceProviderName(String spn) {
        mSpn = spn;
    }

    /**
     * Set voice mail number to SIM record
     *
     * The voice mail number can be stored either in EF_MBDN (TS 51.011) or
     * EF_MAILBOX_CPHS (CPHS 4.2)
     *
     * If EF_MBDN is available, store the voice mail number to EF_MBDN
     *
     * If EF_MAILBOX_CPHS is enabled, store the voice mail number to EF_CHPS
     *
     * So the voice mail number will be stored in both EFs if both are available
     *
     * Return error only if both EF_MBDN and EF_MAILBOX_CPHS fail.
     *
     * When the operation is complete, onComplete will be sent to its handler
     *
     * @param alphaTag alpha-tagging of the dailing Nubmer (upto 10 characters)
     * @param voiceNumber dailing Nubmer (upto 20 digits)
     *        if the number is start with '+', then set to international TOA
     * @param onComplete
     *        onComplete.obj will be an AsyncResult
     *        ((AsyncResult)onComplete.obj).exception == NULL on success
     *        ((AsyncResult)onComplete.obj).exception != NULL on fail
     */
    public abstract void SetVoiceMailNumber(String alphaTag, String voiceNumber,
            Message onComplete);

    public String GetVoiceMailAlphaTag() {
        return mVoiceMailTag;
    }

    /**
     * Sets the SIM voice message waiting indicator records
     * @param line GSM Subscriber Profile Number, one-based. Only '1' is supported
     * @param countWaiting The number of messages waiting, if known. Use
     *                     -1 to indicate that an unknown number of
     *                      messages are waiting
     */
    public abstract void SetVoiceMessageWaiting(Int32 line, Int32 countWaiting);

    /**
     * Called by GsmPhone to update VoiceMail count
     */
    public abstract Int32 GetVoiceMessageCount();

    /**
     * Called by STK Service when REFRESH is received.
     * @param fileChanged indicates whether any files changed
     * @param fileList if non-NULL, a list of EF files that changed
     */
    public abstract void OnRefresh(Boolean fileChanged, Int32[] fileList);

    /**
     * Called by Subclasses (SimRecords and RuimRecords) whenever
     * IccRefreshResponse.REFRESH_RESULT_INIT event received
     */
    protected void OnIccRefreshInit() {
        mAdnCache->Reset();
        UiccCardApplication parentApp = mParentApp;
        If ((parentApp != NULL) &&
                (parentApp->GetState() == AppState.APPSTATE_READY)) {
            // This will cause files to be reread
            SendMessage(ObtainMessage(EVENT_APP_READY));
        }
    }

    public Boolean GetRecordsLoaded() {
        If (mRecordsToLoad == 0 && mRecordsRequested == TRUE) {
            return TRUE;
        } else {
            return FALSE;
        }
    }

    //***** Overridden from Handler
    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;
        Switch (msg.what) {
            case EVENT_GET_ICC_RECORD_DONE:
                try {
                    ar = (AsyncResult) msg.obj;
                    IccRecordLoaded recordLoaded = (IccRecordLoaded) ar.userObj;
                    If (DBG) Log(recordLoaded->GetEfName() + " LOADED");

                    If (ar.exception != NULL) {
                        Loge("Record Load Exception: " + ar.exception);
                    } else {
                        recordLoaded->OnRecordLoaded(ar);
                    }
                }Catch (RuntimeException exc) {
                    // I don't want these exceptions to be fatal
                    Loge("Exception parsing SIM record: " + exc);
                } finally {
                    // Count up record load responses even if they are fails
                    OnRecordLoaded();
                }
                break;
            case EVENT_REFRESH:
                ar = (AsyncResult)msg.obj;
                If (DBG) Log("Card REFRESH occurred: ");
                If (ar.exception == NULL) {
                     BroadcastRefresh();
                     HandleRefresh((IccRefreshResponse)ar.result);
                } else {
                    Loge("Icc refresh Exception: " + ar.exception);
                }
                break;
            case EVENT_REFRESH_OEM:
                ar = (AsyncResult)msg.obj;
                If (DBG) Log("Card REFRESH OEM occurred: ");
                If (ar.exception == NULL) {
                    HandleRefreshOem((Byte[])ar.result);
                } else {
                    Loge("Icc refresh OEM Exception: " + ar.exception);
                }
                break;

            case EVENT_AKA_AUTHENTICATE_DONE:
                ar = (AsyncResult)msg.obj;
                auth_rsp = NULL;
                If (DBG) Log("EVENT_AKA_AUTHENTICATE_DONE");
                If (ar.exception != NULL) {
                    Loge("Exception ICC SIM AKA: " + ar.exception);
                } else {
                    try {
                        auth_rsp = (IccIoResult)ar.result;
                        If (DBG) Log("ICC SIM AKA: auth_rsp = " + auth_rsp);
                    } Catch (Exception e) {
                        Loge("Failed to parse ICC SIM AKA contents: " + e);
                    }
                }
                Synchronized (mLock) {
                    mLock->NotifyAll();
                }

                break;
            case EVENT_GET_SMS_RECORD_SIZE_DONE:
                ar = (AsyncResult) msg.obj;

                If (ar.exception != NULL) {
                    Loge("Exception in EVENT_GET_SMS_RECORD_SIZE_DONE " + ar.exception);
                    break;
                }

                Int32[] recordSize = (Int32[])ar.result;
                try {
                    // recordSize[0]  is the record length
                    // recordSize[1]  is the total length of the EF file
                    // recordSize[2]  is the number of records in the EF file
                    mSmsCountOnIcc = recordSize[2];
                    Log("EVENT_GET_SMS_RECORD_SIZE_DONE Size " + recordSize[0]
                            + " total " + recordSize[1]
                                    + " record " + recordSize[2]);
                } Catch (ArrayIndexOutOfBoundsException exc) {
                    Loge("ArrayIndexOutOfBoundsException in EVENT_GET_SMS_RECORD_SIZE_DONE: "
                            + exc->ToString());
                }
                break;

            default:
                super->HandleMessage(msg);
        }
    }

    protected abstract void HandleFileUpdate(Int32 efid);

    protected void BroadcastRefresh() {
    }

    protected void HandleRefresh(IccRefreshResponse refreshResponse){
        If (refreshResponse == NULL) {
            If (DBG) Log("handleRefresh received without input");
            return;
        }

        If (refreshResponse.aid != NULL &&
                !refreshResponse.aid->Equals(mParentApp->GetAid())) {
            // This is for different app. Ignore.
            return;
        }

        Switch (refreshResponse.refreshResult) {
            case IccRefreshResponse.REFRESH_RESULT_FILE_UPDATE:
                If (DBG) Log("handleRefresh with SIM_FILE_UPDATED");
                HandleFileUpdate(refreshResponse.efId);
                break;
            case IccRefreshResponse.REFRESH_RESULT_INIT:
                If (DBG) Log("handleRefresh with SIM_REFRESH_INIT");
                // need to reload all Files (that we care about)
                If (mAdnCache != NULL) {
                    mAdnCache->Reset();
                    //We will re-fetch the records when the app
                    // goes back to the ready state. Nothing to do here.
                }
                break;
            case IccRefreshResponse.REFRESH_RESULT_RESET:
                If (DBG) Log("handleRefresh with SIM_REFRESH_RESET");
                If (PowerOffOnSimReset()) {
                    mCi->SetRadioPower(FALSE, NULL);
                    /* Note: no need to call SetRadioPower(TRUE).  Assuming the desired
                    * radio power state is still ON (as tracked by ServiceStateTracker),
                    * ServiceStateTracker will call setRadioPower when it receives the
                    * RADIO_STATE_CHANGED notification for the power off.  And if the
                    * desired power state has changed in the interim, we don't want to
                    * override it with an unconditional power on.
                    */
                } else {
                    If(mAdnCache != NULL) {
                        mAdnCache->Reset();
                    }
                    mRecordsRequested = FALSE;
                    mImsi = NULL;
                }
                //We will re-fetch the records when the app
                // goes back to the ready state. Nothing to do here.
                break;
            default:
                // unknown refresh operation
                If (DBG) Log("handleRefresh with unknown operation");
                break;
        }
    }

    private void HandleRefreshOem(Byte[] data){
        ByteBuffer payload = ByteBuffer->Wrap(data);
        IccRefreshResponse response = UiccController->ParseOemSimRefresh(payload);

        IccCardApplicationStatus appStatus = new IccCardApplicationStatus();
        AppType appType = appStatus->AppTypeFromRILInt(payload->GetInt());
        Int32 slotId = (Int32)payload->Get();
        If ((appType != AppType.APPTYPE_UNKNOWN)
            && (appType != mParentApp->GetType())) {
            // This is for different app. Ignore.
            return;
        }

        BroadcastRefresh();
        HandleRefresh(response);

        If (response.refreshResult == IccRefreshResponse.REFRESH_RESULT_FILE_UPDATE ||
            response.refreshResult == IccRefreshResponse.REFRESH_RESULT_INIT) {
            Log("send broadcast org.codeaurora.intent.action.ACTION_SIM_REFRESH_UPDATE");
            Intent sendIntent = new Intent(
                    "org.codeaurora.intent.action.ACTION_SIM_REFRESH_UPDATE");
            If (TelephonyManager->GetDefault()->IsMultiSimEnabled()){
                sendIntent->PutExtra(PhoneConstants.SLOT_KEY, slotId);
            }
            mContext->SendBroadcast(sendIntent, NULL);
        }
    }


    protected abstract void OnRecordLoaded();

    protected abstract void OnAllRecordsLoaded();

    /**
     * Returns the SpnDisplayRule based on settings on the SIM and the
     * specified Plmn (currently-registered PLMN).  See TS 22.101 Annex A
     * and TS 51.011 10.3.11 for details.
     *
     * If the SPN is not found on the SIM, the rule is always PLMN_ONLY.
     * Generally used for GSM/UMTS and the like SIMs.
     */
    public abstract Int32 GetDisplayRule(String plmn);

    /**
     * Return TRUE if "Restriction of menu options for manual PLMN selection"
     * bit is set or EF_CSP data is unavailable, return FALSE otherwise.
     * Generally used for GSM/UMTS and the like SIMs.
     */
    public Boolean IsCspPlmnEnabled() {
        return FALSE;
    }

    /**
     * Returns the 5 or 6 digit MCC/MNC of the operator that
     * provided the SIM card. Returns NULL of SIM is not yet ready
     * or is not valid for the type of IccCard. Generally used for
     * GSM/UMTS and the like SIMS
     */
    public String GetOperatorNumeric() {
        return NULL;
    }

    /**
     * Check if call forward info is stored on SIM
     * @return TRUE if call forward info is stored on SIM.
     */
    public Boolean IsCallForwardStatusStored() {
        return FALSE;
    }

    /**
     * Get the current Voice call forwarding flag for GSM/UMTS and the like SIMs
     *
     * @return TRUE if enabled
     */
    public Boolean GetVoiceCallForwardingFlag() {
        return FALSE;
    }

    /**
     * Set the voice call forwarding flag for GSM/UMTS and the like SIMs
     *
     * @param line to enable/disable
     * @param enable
     * @param number to which CFU is enabled
     */
    CARAPI SetVoiceCallForwardingFlag(Int32 line, Boolean enable, String number) {
    }

    /**
     * Indicates wether SIM is in provisioned state or not.
     * Overridden only if SIM can be dynamically provisioned via OTA.
     *
     * @return TRUE if provisioned
     */
    public Boolean IsProvisioned () {
        return TRUE;
    }

    /**
     * Write string to log file
     *
     * @param s is the string to write
     */
    protected abstract void Log(String s);

    /**
     * Write error string to log file.
     *
     * @param s is the string to write
     */
    protected abstract void Loge(String s);

    /**
     * Return an interface to retrieve the ISIM records for IMS, if available.
     * @return the interface to retrieve the ISIM records, or NULL if not supported
     */
    public IsimRecords GetIsimRecords() {
        return NULL;
    }

    public UsimServiceTable GetUsimServiceTable() {
        return NULL;
    }

    /**
     * Returns the response of the SIM application on the UICC to authentication
     * challenge/response algorithm. The data string and challenge response are
     * Base64 encoded Strings.
     * Can support EAP-SIM, EAP-AKA with results encoded per 3GPP TS 31.102.
     *
     * @param authContext parameter P2 that specifies the authentication context per 3GPP TS 31.102 (Section 7.1.2)
     * @param data authentication challenge data
     * @return challenge response
     */
    public String GetIccSimChallengeResponse(Int32 authContext, String data) {
        If (DBG) Log("getIccSimChallengeResponse:");

        try {
            Synchronized(mLock) {
                CommandsInterface ci = mCi;
                UiccCardApplication parentApp = mParentApp;
                If (ci != NULL && parentApp != NULL) {
                    ci->RequestIccSimAuthentication(authContext, data,
                            parentApp->GetAid(),
                            ObtainMessage(EVENT_AKA_AUTHENTICATE_DONE));
                    try {
                        mLock->Wait();
                    } Catch (InterruptedException e) {
                        Loge("getIccSimChallengeResponse: Fail, interrupted"
                                + " while trying to request Icc Sim Auth");
                        return NULL;
                    }
                } else {
                    Loge( "getIccSimChallengeResponse: "
                            + "Fail, ci or parentApp is NULL");
                    return NULL;
                }
            }
        } Catch(Exception e) {
            Loge( "getIccSimChallengeResponse: "
                    + "Fail while trying to request Icc Sim Auth");
            return NULL;
        }

        If (DBG) Log("getIccSimChallengeResponse: return auth_rsp");

        return android.util.Base64->EncodeToString(auth_rsp.payload, android.util.Base64.NO_WRAP);
    }

    protected Boolean RequirePowerOffOnSimRefreshReset() {
        return mContext->GetResources()->GetBoolean(
            R.bool.config_requireRadioPowerOffOnSimRefreshReset);
    }

    /**
     * To get SMS capacity count on ICC card.
     */
    public Int32 GetSmsCapacityOnIcc() {
        If (DBG) Log("getSmsCapacityOnIcc: " + mSmsCountOnIcc);
        return mSmsCountOnIcc;
    }

    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("IccRecords: " + this);
        pw->Println(" mDestroyed=" + mDestroyed);
        pw->Println(" mCi=" + mCi);
        pw->Println(" mFh=" + mFh);
        pw->Println(" mParentApp=" + mParentApp);
        pw->Println(" recordsLoadedRegistrants: size=" + mRecordsLoadedRegistrants->Size());
        For (Int32 i = 0; i < mRecordsLoadedRegistrants->Size(); i++) {
            pw->Println("  recordsLoadedRegistrants[" + i + "]="
                    + ((Registrant)mRecordsLoadedRegistrants->Get(i)).GetHandler());
        }
        pw->Println(" mImsiReadyRegistrants: size=" + mImsiReadyRegistrants->Size());
        For (Int32 i = 0; i < mImsiReadyRegistrants->Size(); i++) {
            pw->Println("  mImsiReadyRegistrants[" + i + "]="
                    + ((Registrant)mImsiReadyRegistrants->Get(i)).GetHandler());
        }
        pw->Println(" mRecordsEventsRegistrants: size=" + mRecordsEventsRegistrants->Size());
        For (Int32 i = 0; i < mRecordsEventsRegistrants->Size(); i++) {
            pw->Println("  mRecordsEventsRegistrants[" + i + "]="
                    + ((Registrant)mRecordsEventsRegistrants->Get(i)).GetHandler());
        }
        pw->Println(" mNewSmsRegistrants: size=" + mNewSmsRegistrants->Size());
        For (Int32 i = 0; i < mNewSmsRegistrants->Size(); i++) {
            pw->Println("  mNewSmsRegistrants[" + i + "]="
                    + ((Registrant)mNewSmsRegistrants->Get(i)).GetHandler());
        }
        pw->Println(" mNetworkSelectionModeAutomaticRegistrants: size="
                + mNetworkSelectionModeAutomaticRegistrants->Size());
        For (Int32 i = 0; i < mNetworkSelectionModeAutomaticRegistrants->Size(); i++) {
            pw->Println("  mNetworkSelectionModeAutomaticRegistrants[" + i + "]="
                    + ((Registrant)mNetworkSelectionModeAutomaticRegistrants->Get(i)).GetHandler());
        }
        pw->Println(" mRecordsRequested=" + mRecordsRequested);
        pw->Println(" mRecordsToLoad=" + mRecordsToLoad);
        pw->Println(" mRdnCache=" + mAdnCache);
        pw->Println(" iccid=" + mIccId);
        pw->Println(" mMsisdn=" + mMsisdn);
        pw->Println(" mMsisdnTag=" + mMsisdnTag);
        pw->Println(" mVoiceMailNum=" + mVoiceMailNum);
        pw->Println(" mVoiceMailTag=" + mVoiceMailTag);
        pw->Println(" mNewVoiceMailNum=" + mNewVoiceMailNum);
        pw->Println(" mNewVoiceMailTag=" + mNewVoiceMailTag);
        pw->Println(" mIsVoiceMailFixed=" + mIsVoiceMailFixed);
        pw->Println(" mImsi=" + mImsi);
        pw->Println(" mMncLength=" + mMncLength);
        pw->Println(" mMailboxIndex=" + mMailboxIndex);
        pw->Println(" mSpn=" + mSpn);
        pw->Flush();
    }

    protected Boolean PowerOffOnSimReset() {
        return !mContext->GetResources()->GetBoolean(
                R.bool.skip_radio_power_off_on_sim_refresh_reset);
    }

    protected void SetSystemProperty(String property, String value) {
        If (mParentApp == NULL) return;
        Int32 slotId = mParentApp->GetUiccCard()->GetSlotId();

        SubscriptionController subController = SubscriptionController->GetInstance();
        Int64 subId = subController->GetSubIdUsingSlotId(slotId)[0];

        TelephonyManager->SetTelephonyProperty(property, subId, value);
    }
}
