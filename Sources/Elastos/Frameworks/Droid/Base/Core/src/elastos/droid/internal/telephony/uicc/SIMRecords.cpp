/*
 * Copyright (c) 2014, Linux Foundation. All rights reserved.
 * Not a Contribution, Apache license notifications and license are retained
 * for attribution purposes only.
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

package com.android.internal.telephony.uicc;

using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_ICC_OPERATOR_ALPHA;
using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_ICC_OPERATOR_ISO_COUNTRY;
using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_ICC_OPERATOR_NUMERIC;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::ISmsMessage;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Content::Res::IResources;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IMccTable;
using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Telephony::ISubscriptionController;
using Elastos::Droid::Internal::Telephony::Gsm::ISimTlv;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppState;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppType;
using Elastos::Droid::Internal::Telephony::Uicc::IUICCConfig;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;

/**
 * {@hide}
 */
public class SIMRecords extends IccRecords {
    protected static const String LOG_TAG = "SIMRecords";

    private static const Boolean CRASH_RIL = FALSE;

    // ***** Instance Variables

    VoiceMailConstants mVmConfig;


    SpnOverride mSpnOverride;

    // ***** Cached SIM State; cleared on channel close

    private Boolean mCallForwardingEnabled;


    /**
     * States only used by getSpnFsm FSM
     */
    private GetSpnFsmState mSpnState;

    /** CPHS service Information (See CPHS 4.2 B.3.1.1)
     *  It will be set in onSimReady if reading GET_CPHS_INFO successfully
     *  mCphsInfo[0] is CPHS Phase
     *  mCphsInfo[1] and mCphsInfo[2] is CPHS Service Table
     */
    private Byte[] mCphsInfo = NULL;
    Boolean mCspPlmnEnabled = TRUE;

    Byte[] mEfMWIS = NULL;
    Byte[] mEfCPHS_MWI =NULL;
    Byte[] mEfCff = NULL;
    Byte[] mEfCfis = NULL;

    Byte[] mEfLi = NULL;
    Byte[] mEfPl = NULL;

    Int32 mSpnDisplayCondition;
    // Numeric network codes listed in TS 51.011 EF[SPDI]
    ArrayList<String> mSpdiNetworks = NULL;

    String mPnnHomeName = NULL;

    UsimServiceTable mUsimServiceTable;

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "SimRecords: " + super->ToString()
                + " mVmConfig" + mVmConfig
                + " mSpnOverride=" + "mSpnOverride"
                + " callForwardingEnabled=" + mCallForwardingEnabled
                + " spnState=" + mSpnState
                + " mCphsInfo=" + mCphsInfo
                + " mCspPlmnEnabled=" + mCspPlmnEnabled
                + " efMWIS=" + mEfMWIS
                + " efCPHS_MWI=" + mEfCPHS_MWI
                + " mEfCff=" + mEfCff
                + " mEfCfis=" + mEfCfis
                + " getOperatorNumeric=" + GetOperatorNumeric();
    }

    // ***** Constants

    // From TS 51.011 EF[SPDI] section
    static const Int32 TAG_SPDI = 0xA3;
    static const Int32 TAG_SPDI_PLMN_LIST = 0x80;

    // Full Name IEI from TS 24.008
    static const Int32 TAG_FULL_NETWORK_NAME = 0x43;

    // Short Name IEI from TS 24.008
    static const Int32 TAG_SHORT_NETWORK_NAME = 0x45;

    // active CFF from CPHS 4.2 B.4.5
    static const Int32 CFF_UNCONDITIONAL_ACTIVE = 0x0a;
    static const Int32 CFF_UNCONDITIONAL_DEACTIVE = 0x05;
    static const Int32 CFF_LINE1_MASK = 0x0f;
    static const Int32 CFF_LINE1_RESET = 0xf0;

    // CPHS Service Table (See CPHS 4.2 B.3.1)
    private static const Int32 CPHS_SST_MBN_MASK = 0x30;
    private static const Int32 CPHS_SST_MBN_ENABLED = 0x30;

    // EF_CFIS related constants
    // Spec reference TS 51.011 section 10.3.46.
    private static const Int32 CFIS_BCD_NUMBER_LENGTH_OFFSET = 2;
    private static const Int32 CFIS_TON_NPI_OFFSET = 3;
    private static const Int32 CFIS_ADN_CAPABILITY_ID_OFFSET = 14;
    private static const Int32 CFIS_ADN_EXTENSION_ID_OFFSET = 15;

    // ***** Event Constants
    private static const Int32 EVENT_GET_IMSI_DONE = 3;
    private static const Int32 EVENT_GET_ICCID_DONE = 4;
    private static const Int32 EVENT_GET_MBI_DONE = 5;
    private static const Int32 EVENT_GET_MBDN_DONE = 6;
    private static const Int32 EVENT_GET_MWIS_DONE = 7;
    private static const Int32 EVENT_GET_VOICE_MAIL_INDICATOR_CPHS_DONE = 8;
    protected static const Int32 EVENT_GET_AD_DONE = 9; // Admin data on SIM
    protected static const Int32 EVENT_GET_MSISDN_DONE = 10;
    private static const Int32 EVENT_GET_CPHS_MAILBOX_DONE = 11;
    private static const Int32 EVENT_GET_SPN_DONE = 12;
    private static const Int32 EVENT_GET_SPDI_DONE = 13;
    private static const Int32 EVENT_UPDATE_DONE = 14;
    private static const Int32 EVENT_GET_PNN_DONE = 15;
    protected static const Int32 EVENT_GET_SST_DONE = 17;
    private static const Int32 EVENT_GET_ALL_SMS_DONE = 18;
    private static const Int32 EVENT_MARK_SMS_READ_DONE = 19;
    private static const Int32 EVENT_SET_MBDN_DONE = 20;
    private static const Int32 EVENT_GET_SMS_DONE = 22;
    private static const Int32 EVENT_GET_CFF_DONE = 24;
    private static const Int32 EVENT_SET_CPHS_MAILBOX_DONE = 25;
    private static const Int32 EVENT_GET_INFO_CPHS_DONE = 26;
    // private static const Int32 EVENT_SET_MSISDN_DONE = 30; Defined in IccRecords as 30
    private static const Int32 EVENT_GET_CFIS_DONE = 32;
    private static const Int32 EVENT_GET_CSP_CPHS_DONE = 33;
    private static const Int32 EVENT_GET_GID1_DONE = 34;
    private static const Int32 EVENT_APP_LOCKED = 35;

    // Lookup table for carriers known to produce SIMs which incorrectly indicate MNC length.

    private static const String[] MCCMNC_CODES_HAVING_3DIGITS_MNC = {
        "302370", "302720", "310260",
        "405025", "405026", "405027", "405028", "405029", "405030", "405031", "405032",
        "405033", "405034", "405035", "405036", "405037", "405038", "405039", "405040",
        "405041", "405042", "405043", "405044", "405045", "405046", "405047", "405750",
        "405751", "405752", "405753", "405754", "405755", "405756", "405799", "405800",
        "405801", "405802", "405803", "405804", "405805", "405806", "405807", "405808",
        "405809", "405810", "405811", "405812", "405813", "405814", "405815", "405816",
        "405817", "405818", "405819", "405820", "405821", "405822", "405823", "405824",
        "405825", "405826", "405827", "405828", "405829", "405830", "405831", "405832",
        "405833", "405834", "405835", "405836", "405837", "405838", "405839", "405840",
        "405841", "405842", "405843", "405844", "405845", "405846", "405847", "405848",
        "405849", "405850", "405851", "405852", "405853", "405875", "405876", "405877",
        "405878", "405879", "405880", "405881", "405882", "405883", "405884", "405885",
        "405886", "405908", "405909", "405910", "405911", "405912", "405913", "405914",
        "405915", "405916", "405917", "405918", "405919", "405920", "405921", "405922",
        "405923", "405924", "405925", "405926", "405927", "405928", "405929", "405930",
        "405931", "405932", "502142", "502143", "502145", "502146", "502147", "502148"
    };

    // ***** Constructor

    public SIMRecords(UiccCardApplication app, Context c, CommandsInterface ci) {
        Super(app, c, ci);

        mAdnCache = new AdnRecordCache(mFh);

        mVmConfig = new VoiceMailConstants();
        mSpnOverride = new SpnOverride();

        mRecordsRequested = FALSE;  // No load request is made till SIM ready

        // recordsToLoad is set to 0 because no requests are made yet
        mRecordsToLoad = 0;

        // Start off by setting empty state
        ResetRecords();
        mParentApp->RegisterForReady(this, EVENT_APP_READY, NULL);
        mParentApp->RegisterForLocked(this, EVENT_APP_LOCKED, NULL);
        If (DBG) Log("SIMRecords X ctor this=" + this);
    }

    //@Override
    CARAPI Dispose() {
        If (DBG) Log("Disposing SIMRecords this=" + this);
        //Unregister for all events
        mParentApp->UnregisterForReady(this);
        mParentApp->UnregisterForLocked(this);
        ResetRecords();
        super->Dispose();
    }

    //@Override
    protected void Finalize() {
        If(DBG) Log("finalized");
    }

    protected void ResetRecords() {
        mImsi = NULL;
        mMsisdn = NULL;
        mVoiceMailNum = NULL;
        mMncLength = UNINITIALIZED;
        Log("setting0 mMncLength" + mMncLength);
        mIccId = NULL;
        // -1 means no EF_SPN found; treat accordingly.
        mSpnDisplayCondition = -1;
        mEfMWIS = NULL;
        mEfCPHS_MWI = NULL;
        mSpdiNetworks = NULL;
        mPnnHomeName = NULL;
        mGid1 = NULL;

        mAdnCache->Reset();

        Log("SIMRecords: onRadioOffOrNotAvailable set 'gsm.sim.operator.numeric' to operator=NULL");
        Log("update icc_operator_numeric=" + NULL);
        SetSystemProperty(PROPERTY_ICC_OPERATOR_NUMERIC, NULL);
        SetSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, NULL);
        SetSystemProperty(PROPERTY_ICC_OPERATOR_ISO_COUNTRY, NULL);
        mParentApp->GetUICCConfig()->SetImsi(mImsi);
        mParentApp->GetUICCConfig()->SetMncLength(mMncLength);

        // recordsRequested is set to FALSE indicating that the SIM
        // read requests made so far are not valid. This is set to
        // TRUE only when fresh set of read requests are made.
        mRecordsRequested = FALSE;
    }


    //***** Public Methods

    /**
     * {@inheritDoc}
     */
    //@Override
    public String GetIMSI() {
        return mImsi;
    }

    //@Override
    public String GetMsisdnNumber() {
        return mMsisdn;
    }

    //@Override
    public String GetGid1() {
        return mGid1;
    }

    //@Override
    public UsimServiceTable GetUsimServiceTable() {
        return mUsimServiceTable;
    }

    private Int32 GetExtFromEf(Int32 ef) {
        Int32 ext;
        Switch (ef) {
            case EF_MSISDN:
                /* For USIM apps use EXT5. (TS 31.102 Section 4.2.37) */
                If (mParentApp->GetType() == AppType.APPTYPE_USIM) {
                    ext = EF_EXT5;
                } else {
                    ext = EF_EXT1;
                }
                break;
            default:
                ext = EF_EXT1;
        }
        return ext;
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
    //@Override
    CARAPI SetMsisdnNumber(String alphaTag, String number,
            Message onComplete) {

        mMsisdn = number;
        mMsisdnTag = alphaTag;

        If(DBG) Log("Set MSISDN: " + mMsisdnTag + " " + /*mMsisdn*/ "xxxxxxx");

        AdnRecord adn = new AdnRecord(mMsisdnTag, mMsisdn);

        new AdnRecordLoader(mFh).UpdateEF(adn, EF_MSISDN, GetExtFromEf(EF_MSISDN), 1, NULL,
                ObtainMessage(EVENT_SET_MSISDN_DONE, onComplete));
    }

    //@Override
    public String GetMsisdnAlphaTag() {
        return mMsisdnTag;
    }

    //@Override
    public String GetVoiceMailNumber() {
        return mVoiceMailNum;
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
    //@Override
    CARAPI SetVoiceMailNumber(String alphaTag, String voiceNumber,
            Message onComplete) {
        If (mIsVoiceMailFixed) {
            AsyncResult->ForMessage((onComplete)).exception =
                    new IccVmFixedException("Voicemail number is fixed by operator");
            onComplete->SendToTarget();
            return;
        }

        mNewVoiceMailNum = voiceNumber;
        mNewVoiceMailTag = alphaTag;

        AdnRecord adn = new AdnRecord(mNewVoiceMailTag, mNewVoiceMailNum);

        If (mMailboxIndex != 0 && mMailboxIndex != 0xff) {

            new AdnRecordLoader(mFh).UpdateEF(adn, EF_MBDN, EF_EXT6,
                    mMailboxIndex, NULL,
                    ObtainMessage(EVENT_SET_MBDN_DONE, onComplete));

        } else If (IsCphsMailboxEnabled()) {

            new AdnRecordLoader(mFh).UpdateEF(adn, EF_MAILBOX_CPHS,
                    EF_EXT1, 1, NULL,
                    ObtainMessage(EVENT_SET_CPHS_MAILBOX_DONE, onComplete));

        } else {
            AsyncResult->ForMessage((onComplete)).exception =
                    new IccVmNotSupportedException("Update SIM voice mailbox error");
            onComplete->SendToTarget();
        }
    }

    //@Override
    public String GetVoiceMailAlphaTag()
    {
        return mVoiceMailTag;
    }

    /**
     * Sets the SIM voice message waiting indicator records
     * @param line GSM Subscriber Profile Number, one-based. Only '1' is supported
     * @param countWaiting The number of messages waiting, if known. Use
     *                     -1 to indicate that an unknown number of
     *                      messages are waiting
     */
    //@Override
    CARAPI
    SetVoiceMessageWaiting(Int32 line, Int32 countWaiting) {
        If (line != 1) {
            // only profile 1 is supported
            return;
        }

        try {
            If (mEfMWIS != NULL) {
                // TS 51.011 10.3.45

                // lsb of Byte 0 is 'voicemail' status
                mEfMWIS[0] = (Byte)((mEfMWIS[0] & 0xfe)
                                    | (countWaiting == 0 ? 0 : 1));

                // Byte 1 is the number of voice messages waiting
                If (countWaiting < 0) {
                    // The spec does not define what this should be
                    // if we don't know the count
                    mEfMWIS[1] = 0;
                } else {
                    mEfMWIS[1] = (Byte) countWaiting;
                }

                mFh->UpdateEFLinearFixed(
                    EF_MWIS, 1, mEfMWIS, NULL,
                    ObtainMessage (EVENT_UPDATE_DONE, EF_MWIS, 0));
            }

            If (mEfCPHS_MWI != NULL) {
                    // Refer CPHS4_2.WW6 B4.2.3
                mEfCPHS_MWI[0] = (Byte)((mEfCPHS_MWI[0] & 0xf0)
                            | (countWaiting == 0 ? 0x5 : 0xa));
                mFh->UpdateEFTransparent(
                    EF_VOICE_MAIL_INDICATOR_CPHS, mEfCPHS_MWI,
                    ObtainMessage (EVENT_UPDATE_DONE, EF_VOICE_MAIL_INDICATOR_CPHS));
            }
        } Catch (ArrayIndexOutOfBoundsException ex) {
            Logw("Error saving voice mail state to SIM. Probably malformed SIM record", ex);
        }
    }

    // Validate data is !NULL and the MSP (Multiple Subscriber Profile)
    // Byte is between 1 and 4. See ETSI TS 131 102 v11.3.0 section 4.2.64.
    private Boolean ValidEfCfis(Byte[] data) {
        Return ((data != NULL) && (data[0] >= 1) && (data[0] <= 4));
    }

    public Int32 GetVoiceMessageCount() {
        Boolean voiceMailWaiting = FALSE;
        Int32 countVoiceMessages = -1;
        If (mEfMWIS != NULL) {
            // Use this data if the EF[MWIS] exists and
            // has been loaded
            // Refer TS 51.011 Section 10.3.45 for the content description
            voiceMailWaiting = ((mEfMWIS[0] & 0x01) != 0);
            countVoiceMessages = mEfMWIS[1] & 0xff;

            If (voiceMailWaiting && countVoiceMessages == 0) {
                // Unknown count = -1
                countVoiceMessages = -1;
            }
            If(DBG) Log(" VoiceMessageCount from SIM MWIS = " + countVoiceMessages);
        } else If (mEfCPHS_MWI != NULL) {
            // use voice mail count from CPHS
            Int32 indicator = (Int32) (mEfCPHS_MWI[0] & 0xf);

            // Refer CPHS4_2.WW6 B4.2.3
            If (indicator == 0xA) {
                // Unknown count = -1
                countVoiceMessages = -1;
            } else If (indicator == 0x5) {
                countVoiceMessages = 0;
            }
            If(DBG) Log(" VoiceMessageCount from SIM CPHS = " + countVoiceMessages);
        }
        return countVoiceMessages;
    }

    /**
     * {@inheritDoc}
     */
     //@Override
     public Boolean IsCallForwardStatusStored() {
         Return (mEfCfis != NULL) || (mEfCff != NULL);
     }

    /**
     * {@inheritDoc}
     */
    //@Override
    public Boolean GetVoiceCallForwardingFlag() {
        return mCallForwardingEnabled;
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SetVoiceCallForwardingFlag(Int32 line, Boolean enable, String dialNumber) {

        If (line != 1) return; // only line 1 is supported

        mCallForwardingEnabled = enable;

        mRecordsEventsRegistrants->NotifyResult(EVENT_CFI);

        try {
            If (ValidEfCfis(mEfCfis)) {
                // lsb is of Byte 1 is voice status
                If (enable) {
                    mEfCfis[1] |= 1;
                } else {
                    mEfCfis[1] &= 0xfe;
                }

                Log("setVoiceCallForwardingFlag: enable=" + enable
                        + " mEfCfis=" + IccUtils->BytesToHexString(mEfCfis));

                // Update dialNumber if not empty and CFU is enabled.
                // Spec reference for EF_CFIS contents, TS 51.011 section 10.3.46.
                If (enable && !TextUtils->IsEmpty(dialNumber)) {
                    Log("EF_CFIS: updating cf number, " + dialNumber);
                    Byte[] bcdNumber = PhoneNumberUtils->NumberToCalledPartyBCD(dialNumber);

                    System->Arraycopy(bcdNumber, 0, mEfCfis, CFIS_TON_NPI_OFFSET, bcdNumber.length);

                    mEfCfis[CFIS_BCD_NUMBER_LENGTH_OFFSET] = (Byte) (bcdNumber.length);
                    mEfCfis[CFIS_ADN_CAPABILITY_ID_OFFSET] = (Byte) 0xFF;
                    mEfCfis[CFIS_ADN_EXTENSION_ID_OFFSET] = (Byte) 0xFF;
                }

                mFh->UpdateEFLinearFixed(
                        EF_CFIS, 1, mEfCfis, NULL,
                        ObtainMessage (EVENT_UPDATE_DONE, EF_CFIS));
            } else {
                Log("setVoiceCallForwardingFlag: ignoring enable=" + enable
                        + " invalid mEfCfis=" + IccUtils->BytesToHexString(mEfCfis));
            }

            If (mEfCff != NULL) {
                If (enable) {
                    mEfCff[0] = (Byte) ((mEfCff[0] & CFF_LINE1_RESET)
                            | CFF_UNCONDITIONAL_ACTIVE);
                } else {
                    mEfCff[0] = (Byte) ((mEfCff[0] & CFF_LINE1_RESET)
                            | CFF_UNCONDITIONAL_DEACTIVE);
                }

                mFh->UpdateEFTransparent(
                        EF_CFF_CPHS, mEfCff,
                        ObtainMessage (EVENT_UPDATE_DONE, EF_CFF_CPHS));
            }
        } Catch (ArrayIndexOutOfBoundsException ex) {
            Logw("Error saving call forwarding flag to SIM. "
                            + "Probably malformed SIM record", ex);

        }
    }

    /**
     * Called by STK Service when REFRESH is received.
     * @param fileChanged indicates whether any files changed
     * @param fileList if non-NULL, a list of EF files that changed
     */
    //@Override
    CARAPI OnRefresh(Boolean fileChanged, Int32[] fileList) {
        If (fileChanged) {
            // A future optimization would be to inspect fileList and
            // only reload those files that we care about.  For now,
            // just re-fetch all SIM records that we cache.
            FetchSimRecords();
        }
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    public String GetOperatorNumeric() {
        If (mImsi == NULL) {
            Log("getOperatorNumeric: IMSI == NULL");
            return NULL;
        }
        If (mMncLength == UNINITIALIZED || mMncLength == UNKNOWN) {
            Log("getSIMOperatorNumeric: bad mncLength");
            return NULL;
        }

        // Length = length of MCC + length of MNC
        // length of mcc = 3 (TS 23.003 Section 2.2)
        return mImsi->Substring(0, 3 + mMncLength);
    }

    // ***** Overridden from Handler
    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;
        AdnRecord adn;

        Byte data[];

        Boolean isRecordLoadResponse = FALSE;

        If (mDestroyed->Get()) {
            Loge("Received message " + msg + "[" + msg.what + "] " +
                    " while being destroyed. Ignoring.");
            return;
        }

        try { Switch (msg.what) {
            case EVENT_APP_READY:
                OnReady();
                break;

            case EVENT_APP_LOCKED:
                OnLocked();
                break;

            /* IO events */
            case EVENT_GET_IMSI_DONE:
                isRecordLoadResponse = TRUE;

                ar = (AsyncResult)msg.obj;

                If (ar.exception != NULL) {
                    Loge("Exception querying IMSI, Exception:" + ar.exception);
                    break;
                }

                mImsi = (String) ar.result;

                // IMSI (MCC+MNC+MSIN) is at least 6 digits, but not more
                // than 15 (and usually 15).
                If (mImsi != NULL && (mImsi->Length() < 6 || mImsi->Length() > 15)) {
                    Loge("invalid IMSI " + mImsi);
                    mImsi = NULL;
                }

                Log("IMSI: mMncLength=" + mMncLength);
                Log("IMSI: " + mImsi->Substring(0, 6) + "xxxxxxx");

                If (((mMncLength == UNKNOWN) || (mMncLength == 2)) &&
                        ((mImsi != NULL) && (mImsi->Length() >= 6))) {
                    String mccmncCode = mImsi->Substring(0, 6);
                    For (String mccmnc : MCCMNC_CODES_HAVING_3DIGITS_MNC) {
                        If (mccmnc->Equals(mccmncCode)) {
                            mMncLength = 3;
                            Log("IMSI: setting1 mMncLength=" + mMncLength);
                            break;
                        }
                    }
                }

                If (mMncLength == UNKNOWN) {
                    // the SIM has told us all it knows, but it didn't know the mnc length.
                    // guess using the mcc
                    try {
                        Int32 mcc = Integer->ParseInt(mImsi->Substring(0,3));
                        mMncLength = MccTable->SmallestDigitsMccForMnc(mcc);
                        Log("setting2 mMncLength=" + mMncLength);
                    } Catch (NumberFormatException e) {
                        mMncLength = UNKNOWN;
                        Loge("Corrupt IMSI! setting3 mMncLength=" + mMncLength);
                    }
                }

                mParentApp->GetUICCConfig()->SetImsi(mImsi);
                If (mMncLength == UNKNOWN || mMncLength == UNINITIALIZED) {
                    // We need to default to something that seems common
                    mParentApp->GetUICCConfig()->SetMncLength(3);
                } else {
                    mParentApp->GetUICCConfig()->SetMncLength(mMncLength);
                }

                If (mMncLength != UNKNOWN && mMncLength != UNINITIALIZED) {
                    Log("update mccmnc=" + mImsi->Substring(0, 3 + mMncLength));
                    // finally have both the imsi and the mncLength and can parse the imsi properly
                    MccTable->UpdateMccMncConfiguration(mContext,
                            mImsi->Substring(0, 3 + mMncLength), FALSE);
                }
                mImsiReadyRegistrants->NotifyRegistrants();
            break;

            case EVENT_GET_MBI_DONE:
                Boolean isValidMbdn;
                isRecordLoadResponse = TRUE;

                ar = (AsyncResult)msg.obj;
                data = (Byte[]) ar.result;

                isValidMbdn = FALSE;
                If (ar.exception == NULL) {
                    // Refer TS 51.011 Section 10.3.44 for content details
                    Log("EF_MBI: " + IccUtils->BytesToHexString(data));

                    // Voice mail record number stored first
                    mMailboxIndex = data[0] & 0xff;

                    // check if dailing numbe id valid
                    If (mMailboxIndex != 0 && mMailboxIndex != 0xff) {
                        Log("Got valid mailbox number for MBDN");
                        isValidMbdn = TRUE;
                    }
                }

                // one more record to load
                mRecordsToLoad += 1;

                If (isValidMbdn) {
                    // Note: MBDN was not included in NUM_OF_SIM_RECORDS_LOADED
                    new AdnRecordLoader(mFh).LoadFromEF(EF_MBDN, EF_EXT6,
                            mMailboxIndex, ObtainMessage(EVENT_GET_MBDN_DONE));
                } else {
                    // If this EF not present, try mailbox as in CPHS standard
                    // CPHS (CPHS4_2.WW6) is a european standard.
                    new AdnRecordLoader(mFh).LoadFromEF(EF_MAILBOX_CPHS,
                            EF_EXT1, 1,
                            ObtainMessage(EVENT_GET_CPHS_MAILBOX_DONE));
                }

                break;
            case EVENT_GET_CPHS_MAILBOX_DONE:
            case EVENT_GET_MBDN_DONE:
                //Resetting the voice mail number and voice mail tag to NULL
                //as these should be updated from the data read from EF_MBDN.
                //If they are not reset, incase of invalid data/exception these
                //variables are retaining their previous values and are
                //causing invalid voice mailbox info display to user.
                mVoiceMailNum = NULL;
                mVoiceMailTag = NULL;
                isRecordLoadResponse = TRUE;

                ar = (AsyncResult)msg.obj;

                If (ar.exception != NULL) {

                    Log("Invalid or missing EF"
                        + ((msg.what == EVENT_GET_CPHS_MAILBOX_DONE) ? "[MAILBOX]" : "[MBDN]"));

                    // Bug #645770 fall back to CPHS
                    // FIXME should use SST to decide

                    If (msg.what == EVENT_GET_MBDN_DONE) {
                        //load CPHS on fail...
                        // FIXME right now, only load line1's CPHS voice mail entry

                        mRecordsToLoad += 1;
                        new AdnRecordLoader(mFh).LoadFromEF(
                                EF_MAILBOX_CPHS, EF_EXT1, 1,
                                ObtainMessage(EVENT_GET_CPHS_MAILBOX_DONE));
                    }
                    break;
                }

                adn = (AdnRecord)ar.result;

                Log("VM: " + adn +
                        ((msg.what == EVENT_GET_CPHS_MAILBOX_DONE) ? " EF[MAILBOX]" : " EF[MBDN]"));

                If (adn->IsEmpty() && msg.what == EVENT_GET_MBDN_DONE) {
                    // Bug #645770 fall back to CPHS
                    // FIXME should use SST to decide
                    // FIXME right now, only load line1's CPHS voice mail entry
                    mRecordsToLoad += 1;
                    new AdnRecordLoader(mFh).LoadFromEF(
                            EF_MAILBOX_CPHS, EF_EXT1, 1,
                            ObtainMessage(EVENT_GET_CPHS_MAILBOX_DONE));

                    break;
                }

                mVoiceMailNum = adn->GetNumber();
                mVoiceMailTag = adn->GetAlphaTag();
            break;

            case EVENT_GET_MSISDN_DONE:
                isRecordLoadResponse = TRUE;

                ar = (AsyncResult)msg.obj;

                If (ar.exception != NULL) {
                    Log("Invalid or missing EF[MSISDN]");
                    break;
                }

                adn = (AdnRecord)ar.result;

                mMsisdn = adn->GetNumber();
                mMsisdnTag = adn->GetAlphaTag();

                Log("MSISDN: " + /*mMsisdn*/ "xxxxxxx");
            break;

            case EVENT_SET_MSISDN_DONE:
                isRecordLoadResponse = FALSE;
                ar = (AsyncResult)msg.obj;

                If (ar.userObj != NULL) {
                    AsyncResult->ForMessage(((Message) ar.userObj)).exception
                            = ar.exception;
                    ((Message) ar.userObj).SendToTarget();
                }
                break;

            case EVENT_GET_MWIS_DONE:
                isRecordLoadResponse = TRUE;

                ar = (AsyncResult)msg.obj;
                data = (Byte[])ar.result;

                If(DBG) Log("EF_MWIS : " + IccUtils->BytesToHexString(data));

                If (ar.exception != NULL) {
                    If(DBG) Log("EVENT_GET_MWIS_DONE exception = "
                            + ar.exception);
                    break;
                }

                If ((data[0] & 0xff) == 0xff) {
                    If(DBG) Log("SIMRecords: Uninitialized record MWIS");
                    break;
                }

                mEfMWIS = data;
                break;

            case EVENT_GET_VOICE_MAIL_INDICATOR_CPHS_DONE:
                isRecordLoadResponse = TRUE;

                ar = (AsyncResult)msg.obj;
                data = (Byte[])ar.result;

                If(DBG) Log("EF_CPHS_MWI: " + IccUtils->BytesToHexString(data));

                If (ar.exception != NULL) {
                    If(DBG) Log("EVENT_GET_VOICE_MAIL_INDICATOR_CPHS_DONE exception = "
                            + ar.exception);
                    break;
                }

                mEfCPHS_MWI = data;
                break;

            case EVENT_GET_ICCID_DONE:
                isRecordLoadResponse = TRUE;

                ar = (AsyncResult)msg.obj;
                data = (Byte[])ar.result;

                If (ar.exception != NULL) {
                    break;
                }

                mIccId = IccUtils->BcdToString(data, 0, data.length);

                Log("iccid: " + mIccId);

            break;


            case EVENT_GET_AD_DONE:
                try {
                    isRecordLoadResponse = TRUE;

                    ar = (AsyncResult)msg.obj;
                    data = (Byte[])ar.result;

                    If (ar.exception != NULL) {
                        break;
                    }

                    Log("EF_AD: " + IccUtils->BytesToHexString(data));

                    If (data.length < 3) {
                        Log("Corrupt AD data on SIM");
                        break;
                    }

                    If (data.length == 3) {
                        Log("MNC length not present in EF_AD");
                        break;
                    }

                    mMncLength = data[3] & 0xf;
                    Log("setting4 mMncLength=" + mMncLength);

                    If (mMncLength == 0xf) {
                        mMncLength = UNKNOWN;
                        Log("setting5 mMncLength=" + mMncLength);
                    } else {
                        mParentApp->GetUICCConfig()->SetMncLength(mMncLength);
                    }

                } finally {
                    If (((mMncLength == UNINITIALIZED) || (mMncLength == UNKNOWN) ||
                            (mMncLength == 2)) && ((mImsi != NULL) && (mImsi->Length() >= 6))) {
                        String mccmncCode = mImsi->Substring(0, 6);
                        Log("mccmncCode=" + mccmncCode);
                        For (String mccmnc : MCCMNC_CODES_HAVING_3DIGITS_MNC) {
                            If (mccmnc->Equals(mccmncCode)) {
                                mMncLength = 3;
                                Log("setting6 mMncLength=" + mMncLength);
                                break;
                            }
                        }
                    }

                    If (mMncLength == UNKNOWN || mMncLength == UNINITIALIZED) {
                        If (mImsi != NULL) {
                            try {
                                Int32 mcc = Integer->ParseInt(mImsi->Substring(0,3));

                                mMncLength = MccTable->SmallestDigitsMccForMnc(mcc);
                                Log("setting7 mMncLength=" + mMncLength);
                            } Catch (NumberFormatException e) {
                                mMncLength = UNKNOWN;
                                Loge("Corrupt IMSI! setting8 mMncLength=" + mMncLength);
                            }
                        } else {
                            // Indicate we got this info, but it didn't contain the length.
                            mMncLength = UNKNOWN;
                            Log("MNC length not present in EF_AD setting9 mMncLength=" + mMncLength);
                        }
                    }
                    If (mImsi != NULL && mMncLength != UNKNOWN) {
                        // finally have both imsi and the length of the mnc and can parse
                        // the imsi properly
                        Log("update mccmnc=" + mImsi->Substring(0, 3 + mMncLength));
                        MccTable->UpdateMccMncConfiguration(mContext,
                                mImsi->Substring(0, 3 + mMncLength), FALSE);
                    }
                }
            break;

            case EVENT_GET_SPN_DONE:
                isRecordLoadResponse = TRUE;
                ar = (AsyncResult) msg.obj;
                GetSpnFsm(FALSE, ar);
            break;

            case EVENT_GET_CFF_DONE:
                isRecordLoadResponse = TRUE;

                ar = (AsyncResult) msg.obj;
                data = (Byte[]) ar.result;

                If (ar.exception != NULL) {
                    break;
                }

                Log("EF_CFF_CPHS: " + IccUtils->BytesToHexString(data));
                mEfCff = data;

                // if EF_CFIS is valid, prefer it to EF_CFF_CPHS
                If (!ValidEfCfis(mEfCfis)) {
                    mCallForwardingEnabled =
                        ((data[0] & CFF_LINE1_MASK) == CFF_UNCONDITIONAL_ACTIVE);

                    mRecordsEventsRegistrants->NotifyResult(EVENT_CFI);
                } else {
                    Log("EVENT_GET_CFF_DONE: EF_CFIS is valid, ignoring EF_CFF_CPHS");
                }
                break;

            case EVENT_GET_SPDI_DONE:
                isRecordLoadResponse = TRUE;

                ar = (AsyncResult)msg.obj;
                data = (Byte[])ar.result;

                If (ar.exception != NULL) {
                    break;
                }

                ParseEfSpdi(data);
            break;

            case EVENT_UPDATE_DONE:
                ar = (AsyncResult)msg.obj;
                If (ar.exception != NULL) {
                    Logw("update failed. ", ar.exception);
                }
            break;

            case EVENT_GET_PNN_DONE:
                isRecordLoadResponse = TRUE;

                ar = (AsyncResult)msg.obj;
                data = (Byte[])ar.result;

                If (ar.exception != NULL) {
                    break;
                }

                SimTlv tlv = new SimTlv(data, 0, data.length);

                For ( ; tlv->IsValidObject() ; tlv->NextObject()) {
                    If (tlv->GetTag() == TAG_FULL_NETWORK_NAME) {
                        mPnnHomeName
                            = IccUtils->NetworkNameToString(
                                tlv->GetData(), 0, tlv->GetData().length);
                        break;
                    }
                }
            break;

            case EVENT_GET_ALL_SMS_DONE:
                isRecordLoadResponse = TRUE;

                ar = (AsyncResult)msg.obj;
                If (ar.exception != NULL)
                    break;

                HandleSmses((ArrayList<Byte []>) ar.result);
                break;

            case EVENT_MARK_SMS_READ_DONE:
                Rlog->I("ENF", "marked read: sms " + msg.arg1);
                break;

            case EVENT_GET_SMS_DONE:
                isRecordLoadResponse = FALSE;
                ar = (AsyncResult)msg.obj;
                If (ar.exception == NULL) {
                    HandleSms((Byte[])ar.result);
                } else {
                    Loge("Error on GET_SMS with exp " + ar.exception);
                }
                break;
            case EVENT_GET_SST_DONE:
                isRecordLoadResponse = TRUE;

                ar = (AsyncResult)msg.obj;
                data = (Byte[])ar.result;

                If (ar.exception != NULL) {
                    break;
                }

                mUsimServiceTable = new UsimServiceTable(data);
                If (DBG) Log("SST: " + mUsimServiceTable);
                break;

            case EVENT_GET_INFO_CPHS_DONE:
                isRecordLoadResponse = TRUE;

                ar = (AsyncResult)msg.obj;

                If (ar.exception != NULL) {
                    break;
                }

                mCphsInfo = (Byte[])ar.result;

                If (DBG) Log("iCPHS: " + IccUtils->BytesToHexString(mCphsInfo));
            break;

            case EVENT_SET_MBDN_DONE:
                isRecordLoadResponse = FALSE;
                ar = (AsyncResult)msg.obj;

                If (DBG) Log("EVENT_SET_MBDN_DONE ex:" + ar.exception);
                If (ar.exception == NULL) {
                    mVoiceMailNum = mNewVoiceMailNum;
                    mVoiceMailTag = mNewVoiceMailTag;
                }

                If (IsCphsMailboxEnabled()) {
                    adn = new AdnRecord(mVoiceMailTag, mVoiceMailNum);
                    Message onCphsCompleted = (Message) ar.userObj;

                    /* write to cphs mailbox whenever it is available but
                    * we only need notify caller once if both updating are
                    * successful.
                    *
                    * so if set_mbdn successful, notify caller here and set
                    * onCphsCompleted to NULL
                    */
                    If (ar.exception == NULL && ar.userObj != NULL) {
                        AsyncResult->ForMessage(((Message) ar.userObj)).exception
                                = NULL;
                        ((Message) ar.userObj).SendToTarget();

                        If (DBG) Log("Callback with MBDN successful.");

                        onCphsCompleted = NULL;
                    }

                    new AdnRecordLoader(mFh).
                            UpdateEF(adn, EF_MAILBOX_CPHS, EF_EXT1, 1, NULL,
                            ObtainMessage(EVENT_SET_CPHS_MAILBOX_DONE,
                                    onCphsCompleted));
                } else {
                    If (ar.userObj != NULL) {
                        Resources resource = Resources->GetSystem();
                        If (ar.exception != NULL && resource->GetBoolean(com.android.internal.
                                    R.bool.editable_voicemailnumber)) {
                            // GSMPhone will store vm number on device
                            // when IccVmNotSupportedException occurred
                            AsyncResult->ForMessage(((Message) ar.userObj)).exception
                                = new IccVmNotSupportedException(
                                        "Update SIM voice mailbox error");
                        } else {
                            AsyncResult->ForMessage(((Message) ar.userObj)).exception
                                = ar.exception;
                        }
                        ((Message) ar.userObj).SendToTarget();
                    }
                }
                break;
            case EVENT_SET_CPHS_MAILBOX_DONE:
                isRecordLoadResponse = FALSE;
                ar = (AsyncResult)msg.obj;
                If(ar.exception == NULL) {
                    mVoiceMailNum = mNewVoiceMailNum;
                    mVoiceMailTag = mNewVoiceMailTag;
                } else {
                    If (DBG) Log("Set CPHS MailBox with exception: "
                            + ar.exception);
                }
                If (ar.userObj != NULL) {
                    If (DBG) Log("Callback with CPHS MB successful.");
                    AsyncResult->ForMessage(((Message) ar.userObj)).exception
                            = ar.exception;
                    ((Message) ar.userObj).SendToTarget();
                }
                break;
            case EVENT_GET_CFIS_DONE:
                isRecordLoadResponse = TRUE;

                ar = (AsyncResult)msg.obj;
                data = (Byte[])ar.result;

                If (ar.exception != NULL) {
                    break;
                }

                Log("EF_CFIS: " + IccUtils->BytesToHexString(data));

                If (ValidEfCfis(data)) {
                    mEfCfis = data;

                    // Refer TS 51.011 Section 10.3.46 for the content description
                    mCallForwardingEnabled = ((data[1] & 0x01) != 0);
                    Log("EF_CFIS: callForwardingEnabled=" + mCallForwardingEnabled);

                    mRecordsEventsRegistrants->NotifyResult(EVENT_CFI);
                } else {
                    Log("EF_CFIS: invalid data=" + IccUtils->BytesToHexString(data));
                }
                break;

            case EVENT_GET_CSP_CPHS_DONE:
                isRecordLoadResponse = TRUE;

                ar = (AsyncResult)msg.obj;

                If (ar.exception != NULL) {
                    Loge("Exception in fetching EF_CSP data " + ar.exception);
                    break;
                }

                data = (Byte[])ar.result;

                Log("EF_CSP: " + IccUtils->BytesToHexString(data));
                HandleEfCspData(data);
                break;

            case EVENT_GET_GID1_DONE:
                isRecordLoadResponse = TRUE;

                ar = (AsyncResult)msg.obj;
                data =(Byte[])ar.result;

                If (ar.exception != NULL) {
                    Loge("Exception in get GID1 " + ar.exception);
                    mGid1 = NULL;
                    break;
                }
                mGid1 = IccUtils->BytesToHexString(data);
                Log("GID1: " + mGid1);

                break;

            default:
                super->HandleMessage(msg);   // IccRecords handles generic record load responses

        }}Catch (RuntimeException exc) {
            // I don't want these exceptions to be fatal
            Logw("Exception parsing SIM record", exc);
        } finally {
            // Count up record load responses even if they are fails
            If (isRecordLoadResponse) {
                OnRecordLoaded();
            }
        }
    }

    private class EfPlLoaded implements IccRecordLoaded {
        public String GetEfName() {
            return "EF_PL";
        }

        CARAPI OnRecordLoaded(AsyncResult ar) {
            mEfPl = (Byte[]) ar.result;
            If (DBG) Log("EF_PL=" + IccUtils->BytesToHexString(mEfPl));
        }
    }

    private class EfUsimLiLoaded implements IccRecordLoaded {
        public String GetEfName() {
            return "EF_LI";
        }

        CARAPI OnRecordLoaded(AsyncResult ar) {
            mEfLi = (Byte[]) ar.result;
            If (DBG) Log("EF_LI=" + IccUtils->BytesToHexString(mEfLi));
        }
    }

    //@Override
    protected void HandleFileUpdate(Int32 efid) {
        Switch(efid) {
            case EF_MBDN:
                mRecordsToLoad++;
                new AdnRecordLoader(mFh).LoadFromEF(EF_MBDN, EF_EXT6,
                        mMailboxIndex, ObtainMessage(EVENT_GET_MBDN_DONE));
                break;
            case EF_MAILBOX_CPHS:
                mRecordsToLoad++;
                new AdnRecordLoader(mFh).LoadFromEF(EF_MAILBOX_CPHS, EF_EXT1,
                        1, ObtainMessage(EVENT_GET_CPHS_MAILBOX_DONE));
                break;
            case EF_CSP_CPHS:
                mRecordsToLoad++;
                Log("[CSP] SIM Refresh for EF_CSP_CPHS");
                mFh->LoadEFTransparent(EF_CSP_CPHS,
                        ObtainMessage(EVENT_GET_CSP_CPHS_DONE));
                break;
            case EF_FDN:
                If (DBG) Log("SIM Refresh called for EF_FDN");
                mParentApp->QueryFdn();
            case EF_MSISDN:
                mRecordsToLoad++;
                Log("SIM Refresh called for EF_MSISDN");
                new AdnRecordLoader(mFh).LoadFromEF(EF_MSISDN, GetExtFromEf(EF_MSISDN), 1,
                        ObtainMessage(EVENT_GET_MSISDN_DONE));
                break;
            case EF_CFIS:
                mRecordsToLoad++;
                Log("SIM Refresh called for EF_CFIS");
                mFh->LoadEFLinearFixed(EF_CFIS,
                        1, ObtainMessage(EVENT_GET_CFIS_DONE));
                break;
            case EF_CFF_CPHS:
                mRecordsToLoad++;
                Log("SIM Refresh called for EF_CFF_CPHS");
                mFh->LoadEFTransparent(EF_CFF_CPHS,
                        ObtainMessage(EVENT_GET_CFF_DONE));
                break;
            case EF_ADN:
                Log("SIM Refresh for EF_ADN");
                mAdnCache->Reset();
                break;
            default:
                // For now, fetch all records if this is not a
                // voicemail number.
                // TODO: Handle other cases, instead of fetching all.
                mAdnCache->Reset();
                FetchSimRecords();
                break;
        }
    }

    /**
     * Dispatch 3GPP format message to Registrant ({@code GSMPhone} or {@code CDMALTEPhone})
     * to pass to the 3GPP SMS dispatcher for delivery.
     */
    private Int32 DispatchGsmMessage(SmsMessage message) {
        mNewSmsRegistrants->NotifyResult(message);
        return 0;
    }

    /*
     * Called when a Class2 SMS is  received.
     *
     * @param ar AsyncResult passed to this function. "ar.result" should
     *           be representing the INDEX of SMS on SIM.
     */
    CARAPI HandleSmsOnIcc(AsyncResult ar) {

        Int32[] index = (Int32[])ar.result;

        If (ar.exception != NULL || index.length != 1) {
            Loge(" Error on SMS_ON_SIM with exp "
                   + ar.exception + " length " + index.length);
        } else {
            Log("READ EF_SMS RECORD index= " + index[0]);
            mFh->LoadEFLinearFixed(EF_SMS,index[0],
                            ObtainMessage(EVENT_GET_SMS_DONE));
        }
    }

    private void HandleSms(Byte[] ba) {
        If (ba[0] != 0)
            Rlog->D("ENF", "status : " + ba[0]);

        // 3GPP TS 51.011 v5.0.0 (20011-12)  10.5.3
        // 3 == "received by MS from network; message to be read"
        If (ba[0] == 3) {
            Int32 n = ba.length;

            // Note: Data may include trailing FF's.  That's OK; message
            // should still parse correctly.
            Byte[] pdu = new Byte[n - 1];
            System->Arraycopy(ba, 1, pdu, 0, n - 1);
            SmsMessage message = SmsMessage->CreateFromPdu(pdu, SmsConstants.FORMAT_3GPP);

            DispatchGsmMessage(message);
        }
    }


    private void HandleSmses(ArrayList<Byte[]> messages) {
        Int32 count = messages->Size();

        For (Int32 i = 0; i < count; i++) {
            Byte[] ba = messages->Get(i);

            If (ba[0] != 0)
                Rlog->I("ENF", "status " + i + ": " + ba[0]);

            // 3GPP TS 51.011 v5.0.0 (20011-12)  10.5.3
            // 3 == "received by MS from network; message to be read"

            If (ba[0] == 3) {
                Int32 n = ba.length;

                // Note: Data may include trailing FF's.  That's OK; message
                // should still parse correctly.
                Byte[] pdu = new Byte[n - 1];
                System->Arraycopy(ba, 1, pdu, 0, n - 1);
                SmsMessage message = SmsMessage->CreateFromPdu(pdu, SmsConstants.FORMAT_3GPP);

                DispatchGsmMessage(message);

                // 3GPP TS 51.011 v5.0.0 (20011-12)  10.5.3
                // 1 == "received by MS from network; message read"

                ba[0] = 1;

                If (FALSE) { // FIXME: writing seems to crash RdoServD
                    mFh->UpdateEFLinearFixed(EF_SMS,
                            i, ba, NULL, ObtainMessage(EVENT_MARK_SMS_READ_DONE, i));
                }
            }
        }
    }

    private String FindBestLanguage(Byte[] languages) {
        String bestMatch = NULL;
        String[] locales = mContext->GetAssets()->GetLocales();

        If ((languages == NULL) || (locales == NULL)) return NULL;

        // Each 2-bytes consists of one language
        For (Int32 i = 0; (i + 1) < languages.length; i += 2) {
            try {
                String lang = new String(languages, i, 2, "ISO-8859-1");
                If (DBG) Log ("languages from sim = " + lang);
                For (Int32 j = 0; j < locales.length; j++) {
                    If (locales[j] != NULL && locales[j].Length() >= 2 &&
                            locales[j].Substring(0, 2).EqualsIgnoreCase(lang)) {
                        return lang;
                    }
                }
                If (bestMatch != NULL) break;
            } Catch(java.io.UnsupportedEncodingException e) {
                Log ("Failed to parse USIM language records" + e);
            }
        }
        // no match found. return NULL
        return NULL;
    }

    private void SetLocaleFromUsim() {
        String prefLang = NULL;
        // check EFli then EFpl
        prefLang = FindBestLanguage(mEfLi);

        If (prefLang == NULL) {
            prefLang = FindBestLanguage(mEfPl);
        }

        If (prefLang != NULL) {
            // check country code from SIM
            String imsi = GetIMSI();
            String country = NULL;
            If (imsi != NULL) {
                country = MccTable->CountryCodeForMcc(
                                    Integer->ParseInt(imsi->Substring(0,3)));
            }
            If (DBG) Log("Setting locale to " + prefLang + "_" + country);
            MccTable->SetSystemLocale(mContext, prefLang, country);
        } else {
            If (DBG) Log ("No suitable USIM selected locale");
        }
    }

    //@Override
    protected void OnRecordLoaded() {
        // One record loaded successfully or failed, In either case
        // we need to update the recordsToLoad count
        mRecordsToLoad -= 1;
        If (DBG) Log("onRecordLoaded " + mRecordsToLoad + " requested: " + mRecordsRequested);

        If (mRecordsToLoad == 0 && mRecordsRequested == TRUE) {
            OnAllRecordsLoaded();
        } else If (mRecordsToLoad < 0) {
            Loge("recordsToLoad <0, programmer error suspected");
            mRecordsToLoad = 0;
        }
    }

    //@Override
    protected void OnAllRecordsLoaded() {
        If (DBG) Log("record load complete");

        SetLocaleFromUsim();

        If (mParentApp->GetState() == AppState.APPSTATE_PIN ||
               mParentApp->GetState() == AppState.APPSTATE_PUK) {
            // reset recordsRequested, since sim is not loaded really
            mRecordsRequested = FALSE;
            // lock state, only update language
            return ;
        }

        // Some fields require more than one SIM record to set

        String operator = GetOperatorNumeric();
        If (!TextUtils->IsEmpty(operator)) {
            Log("onAllRecordsLoaded set 'gsm.sim.operator.numeric' to operator='" +
                    operator + "'");
            Log("update icc_operator_numeric=" + operator);
            SetSystemProperty(PROPERTY_ICC_OPERATOR_NUMERIC, operator);
            final SubscriptionController subController = SubscriptionController->GetInstance();
            subController->SetMccMnc(operator, subController->GetDefaultSmsSubId());
        } else {
            Log("onAllRecordsLoaded empty 'gsm.sim.operator.numeric' skipping");
        }

        If (!TextUtils->IsEmpty(mImsi)) {
            Log("onAllRecordsLoaded set mcc imsi=" + mImsi);
            SetSystemProperty(PROPERTY_ICC_OPERATOR_ISO_COUNTRY,
                    MccTable->CountryCodeForMcc(Integer->ParseInt(mImsi->Substring(0,3))));
        } else {
            Log("onAllRecordsLoaded empty imsi skipping setting mcc");
        }

        SetVoiceMailByCountry(operator);
        SetSpnFromConfig(operator);

        mRecordsLoadedRegistrants->NotifyRegistrants(
            new AsyncResult(NULL, NULL, NULL));
    }

    //***** Private methods

    private void SetSpnFromConfig(String carrier) {
        If (mSpnOverride->ContainsCarrier(carrier)) {
            SetServiceProviderName(mSpnOverride->GetSpn(carrier));
            SystemProperties->Set(PROPERTY_ICC_OPERATOR_ALPHA, GetServiceProviderName());
        }
    }


    private void SetVoiceMailByCountry (String spn) {
        If (mVmConfig->ContainsCarrier(spn)) {
            mIsVoiceMailFixed = TRUE;
            mVoiceMailNum = mVmConfig->GetVoiceMailNumber(spn);
            mVoiceMailTag = mVmConfig->GetVoiceMailTag(spn);
        }
    }

    //@Override
    CARAPI OnReady() {
        FetchSimRecords();
    }

    private void OnLocked() {
        If (DBG) Log("only fetch EF_LI and EF_PL in lock state");
        LoadEfLiAndEfPl();
    }

    private void LoadEfLiAndEfPl() {
        If (mParentApp->GetType() == AppType.APPTYPE_USIM) {
            mRecordsRequested = TRUE;
            mFh->LoadEFTransparent(EF_LI,
                    ObtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfUsimLiLoaded()));
            mRecordsToLoad++;

            mFh->LoadEFTransparent(EF_PL,
                    ObtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfPlLoaded()));
            mRecordsToLoad++;
        }
    }

    protected void FetchSimRecords() {
        mRecordsRequested = TRUE;

        If (DBG) Log("fetchSimRecords " + mRecordsToLoad);

        mCi->GetIMSIForApp(mParentApp->GetAid(), ObtainMessage(EVENT_GET_IMSI_DONE));
        mRecordsToLoad++;

        mFh->LoadEFTransparent(EF_ICCID, ObtainMessage(EVENT_GET_ICCID_DONE));
        mRecordsToLoad++;

        // FIXME should examine EF[MSISDN]'s capability configuration
        // to determine which is the voice/data/fax line
        new AdnRecordLoader(mFh).LoadFromEF(EF_MSISDN, GetExtFromEf(EF_MSISDN), 1,
                    ObtainMessage(EVENT_GET_MSISDN_DONE));
        mRecordsToLoad++;

        // Record number is subscriber profile
        mFh->LoadEFLinearFixed(EF_MBI, 1, ObtainMessage(EVENT_GET_MBI_DONE));
        mRecordsToLoad++;

        mFh->LoadEFTransparent(EF_AD, ObtainMessage(EVENT_GET_AD_DONE));
        mRecordsToLoad++;

        // Record number is subscriber profile
        mFh->LoadEFLinearFixed(EF_MWIS, 1, ObtainMessage(EVENT_GET_MWIS_DONE));
        mRecordsToLoad++;


        // Also load CPHS-style voice mail indicator, which stores
        // the same info as EF[MWIS]. If both exist, both are updated
        // but the EF[MWIS] data is preferred
        // Please note this must be loaded after EF[MWIS]
        mFh->LoadEFTransparent(
                EF_VOICE_MAIL_INDICATOR_CPHS,
                ObtainMessage(EVENT_GET_VOICE_MAIL_INDICATOR_CPHS_DONE));
        mRecordsToLoad++;

        // Same goes for Call Forward Status indicator: fetch both
        // EF[CFIS] and CPHS-EF, with EF[CFIS] preferred.
        mFh->LoadEFLinearFixed(EF_CFIS, 1, ObtainMessage(EVENT_GET_CFIS_DONE));
        mRecordsToLoad++;
        mFh->LoadEFTransparent(EF_CFF_CPHS, ObtainMessage(EVENT_GET_CFF_DONE));
        mRecordsToLoad++;


        GetSpnFsm(TRUE, NULL);

        mFh->LoadEFTransparent(EF_SPDI, ObtainMessage(EVENT_GET_SPDI_DONE));
        mRecordsToLoad++;

        mFh->LoadEFLinearFixed(EF_PNN, 1, ObtainMessage(EVENT_GET_PNN_DONE));
        mRecordsToLoad++;

        mFh->LoadEFTransparent(EF_SST, ObtainMessage(EVENT_GET_SST_DONE));
        mRecordsToLoad++;

        mFh->LoadEFTransparent(EF_INFO_CPHS, ObtainMessage(EVENT_GET_INFO_CPHS_DONE));
        mRecordsToLoad++;

        mFh->LoadEFTransparent(EF_CSP_CPHS,ObtainMessage(EVENT_GET_CSP_CPHS_DONE));
        mRecordsToLoad++;

        mFh->LoadEFTransparent(EF_GID1, ObtainMessage(EVENT_GET_GID1_DONE));
        mRecordsToLoad++;

        LoadEfLiAndEfPl();
        mFh->GetEFLinearRecordSize(EF_SMS, ObtainMessage(EVENT_GET_SMS_RECORD_SIZE_DONE));

        // XXX should seek instead of examining them all
        If (FALSE) { // XXX
            mFh->LoadEFLinearFixedAll(EF_SMS, ObtainMessage(EVENT_GET_ALL_SMS_DONE));
            mRecordsToLoad++;
        }

        If (CRASH_RIL) {
            String sms = "0107912160130310f20404d0110041007030208054832b0120"
                         + "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                         + "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                         + "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                         + "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                         + "ffffffffffffffffffffffffffffff";
            Byte[] ba = IccUtils->HexStringToBytes(sms);

            mFh->UpdateEFLinearFixed(EF_SMS, 1, ba, NULL,
                            ObtainMessage(EVENT_MARK_SMS_READ_DONE, 1));
        }
        If (DBG) Log("fetchSimRecords " + mRecordsToLoad + " requested: " + mRecordsRequested);
    }

    /**
     * Returns the SpnDisplayRule based on settings on the SIM and the
     * specified Plmn (currently-registered PLMN).  See TS 22.101 Annex A
     * and TS 51.011 10.3.11 for details.
     *
     * If the SPN is not found on the SIM or is empty, the rule is
     * always PLMN_ONLY.
     */
    //@Override
    public Int32 GetDisplayRule(String plmn) {
        Int32 rule;
        If ((mContext != NULL) && mContext->GetResources()->GetBoolean(
                R.bool.def_telephony_spn_spec_enabled)) {
            rule = SPN_RULE_SHOW_SPN;
            return rule;
        }

        If (mParentApp != NULL && mParentApp->GetUiccCard() != NULL &&
            mParentApp->GetUiccCard()->GetOperatorBrandOverride() != NULL) {
        // If the operator has been overridden, treat it as the SPN file on the SIM did not exist.
            rule = SPN_RULE_SHOW_PLMN;
        } else If (TextUtils->IsEmpty(GetServiceProviderName()) || mSpnDisplayCondition == -1) {
            // No EF_SPN content was found on the SIM, or not yet loaded.  Just show ONS.
            rule = SPN_RULE_SHOW_PLMN;
        } else If (IsOnMatchingPlmn(plmn)) {
            rule = SPN_RULE_SHOW_SPN;
            If ((mSpnDisplayCondition & 0x01) == 0x01) {
                // ONS required when registered to HPLMN or PLMN in EF_SPDI
                rule |= SPN_RULE_SHOW_PLMN;
            }
        } else {
            rule = SPN_RULE_SHOW_PLMN;
            If ((mSpnDisplayCondition & 0x02) == 0x00) {
                // SPN required if not registered to HPLMN or PLMN in EF_SPDI
                rule |= SPN_RULE_SHOW_SPN;
            }
        }
        return rule;
    }

    /**
     * Checks if plmn is HPLMN or on the spdiNetworks list.
     */
    private Boolean IsOnMatchingPlmn(String plmn) {
        If (plmn == NULL) return FALSE;

        If (plmn->Equals(GetOperatorNumeric())) {
            return TRUE;
        }

        If (mSpdiNetworks != NULL) {
            For (String spdiNet : mSpdiNetworks) {
                If (plmn->Equals(spdiNet)) {
                    return TRUE;
                }
            }
        }
        return FALSE;
    }

    /**
     * States of Get SPN Finite State Machine which only used by GetSpnFsm()
     */
    private enum GetSpnFsmState {
        IDLE,               // No initialized
        INIT,               // Start FSM
        READ_SPN_3GPP,      // Load EF_SPN firstly
        READ_SPN_CPHS,      // Load EF_SPN_CPHS secondly
        READ_SPN_SHORT_CPHS // Load EF_SPN_SHORT_CPHS last
    }

    /**
     * Finite State Machine to load Service Provider Name , which can be stored
     * in either EF_SPN (3GPP), EF_SPN_CPHS, or EF_SPN_SHORT_CPHS (CPHS4.2)
     *
     * After starting, FSM will search SPN EFs in order and stop after finding
     * the first valid SPN
     *
     * If the FSM gets restart while waiting for one of
     * SPN EFs Results (i.e. a SIM refresh occurs after issuing
     * read EF_CPHS_SPN), it will re-initialize only after
     * receiving and discarding the unfinished SPN EF result.
     *
     * @param start set TRUE only for initialize loading
     * @param ar the AsyncResult from loadEFTransparent
     *        ar.exception holds exception in error
     *        ar.result is Byte[] for data in success
     */
    private void GetSpnFsm(Boolean start, AsyncResult ar) {
        Byte[] data;

        If (start) {
            // Check previous state to see if there is outstanding
            // SPN read
            If(mSpnState == GetSpnFsmState.READ_SPN_3GPP ||
               mSpnState == GetSpnFsmState.READ_SPN_CPHS ||
               mSpnState == GetSpnFsmState.READ_SPN_SHORT_CPHS ||
               mSpnState == GetSpnFsmState.INIT) {
                // Set INIT then return so the INIT code
                // will run when the outstanding read done.
                mSpnState = GetSpnFsmState.INIT;
                return;
            } else {
                mSpnState = GetSpnFsmState.INIT;
            }
        }

        Switch(mSpnState){
            case INIT:
                SetServiceProviderName(NULL);

                mFh->LoadEFTransparent(EF_SPN,
                        ObtainMessage(EVENT_GET_SPN_DONE));
                mRecordsToLoad++;

                mSpnState = GetSpnFsmState.READ_SPN_3GPP;
                break;
            case READ_SPN_3GPP:
                If (ar != NULL && ar.exception == NULL) {
                    data = (Byte[]) ar.result;
                    mSpnDisplayCondition = 0xff & data[0];
                    SetServiceProviderName(IccUtils->AdnStringFieldToString(
                            data, 1, data.length - 1));

                    If (DBG) Log("Load EF_SPN: " + GetServiceProviderName()
                            + " spnDisplayCondition: " + mSpnDisplayCondition);
                    SetSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, GetServiceProviderName());

                    mSpnState = GetSpnFsmState.IDLE;
                } else {
                    mFh->LoadEFTransparent( EF_SPN_CPHS,
                            ObtainMessage(EVENT_GET_SPN_DONE));
                    mRecordsToLoad++;

                    mSpnState = GetSpnFsmState.READ_SPN_CPHS;

                    // See TS 51.011 10.3.11.  Basically, default to
                    // show PLMN always, and SPN also if roaming.
                    mSpnDisplayCondition = -1;
                }
                break;
            case READ_SPN_CPHS:
                If (ar != NULL && ar.exception == NULL) {
                    data = (Byte[]) ar.result;
                    SetServiceProviderName(IccUtils->AdnStringFieldToString(data, 0, data.length));

                    If (DBG) Log("Load EF_SPN_CPHS: " + GetServiceProviderName());
                    SetSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, GetServiceProviderName());

                    mSpnState = GetSpnFsmState.IDLE;
                } else {
                    mFh->LoadEFTransparent(
                            EF_SPN_SHORT_CPHS, ObtainMessage(EVENT_GET_SPN_DONE));
                    mRecordsToLoad++;

                    mSpnState = GetSpnFsmState.READ_SPN_SHORT_CPHS;
                }
                break;
            case READ_SPN_SHORT_CPHS:
                If (ar != NULL && ar.exception == NULL) {
                    data = (Byte[]) ar.result;
                    SetServiceProviderName(IccUtils->AdnStringFieldToString(data, 0, data.length));

                    If (DBG) Log("Load EF_SPN_SHORT_CPHS: " + GetServiceProviderName());
                    SetSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, GetServiceProviderName());
                }else {
                    If (DBG) Log("No SPN loaded in either CHPS or 3GPP");
                    If (mPnnHomeName != NULL && mSpn == NULL) {
                        If (DBG) Log("Falling back to home network name for SPN");
                        mSpn = mPnnHomeName;
                        SetSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, mSpn);
                        mRecordsEventsRegistrants->NotifyResult(EVENT_SPN);
                    }
                }

                mSpnState = GetSpnFsmState.IDLE;
                break;
            default:
                mSpnState = GetSpnFsmState.IDLE;
        }
    }

    /**
     * Parse TS 51.011 EF[SPDI] record
     * This record contains the list of numeric network IDs that
     * are treated specially when determining SPN display
     */
    private void
    ParseEfSpdi(Byte[] data) {
        SimTlv tlv = new SimTlv(data, 0, data.length);

        Byte[] plmnEntries = NULL;

        For ( ; tlv->IsValidObject() ; tlv->NextObject()) {
            // Skip SPDI tag, if existant
            If (tlv->GetTag() == TAG_SPDI) {
              tlv = new SimTlv(tlv->GetData(), 0, tlv->GetData().length);
            }
            // There should only be one TAG_SPDI_PLMN_LIST
            If (tlv->GetTag() == TAG_SPDI_PLMN_LIST) {
                plmnEntries = tlv->GetData();
                break;
            }
        }

        If (plmnEntries == NULL) {
            return;
        }

        mSpdiNetworks = new ArrayList<String>(plmnEntries.length / 3);

        For (Int32 i = 0 ; i + 2 < plmnEntries.length ; i += 3) {
            String plmnCode;
            plmnCode = IccUtils->BcdToString(plmnEntries, i, 3);

            // Valid operator codes are 5 or 6 digits
            If (plmnCode->Length() >= 5) {
                Log("EF_SPDI network: " + plmnCode);
                mSpdiNetworks->Add(plmnCode);
            }
        }
    }

    /**
     * check to see if Mailbox Number is allocated and activated in CPHS SST
     */
    private Boolean IsCphsMailboxEnabled() {
        If (mCphsInfo == NULL)  return FALSE;
        Return ((mCphsInfo[1] & CPHS_SST_MBN_MASK) == CPHS_SST_MBN_ENABLED );
    }

    //@Override
    protected void Log(String s) {
        Rlog->D(LOG_TAG, "[SIMRecords] " + s);
    }

    //@Override
    protected void Loge(String s) {
        Rlog->E(LOG_TAG, "[SIMRecords] " + s);
    }

    protected void Logw(String s, Throwable tr) {
        Rlog->W(LOG_TAG, "[SIMRecords] " + s, tr);
    }

    protected void Logv(String s) {
        Rlog->V(LOG_TAG, "[SIMRecords] " + s);
    }

    /**
     * Return TRUE if "Restriction of menu options for manual PLMN selection"
     * bit is set or EF_CSP data is unavailable, return FALSE otherwise.
     */
    //@Override
    public Boolean IsCspPlmnEnabled() {
        return mCspPlmnEnabled;
    }

    /**
     * Parse EF_CSP data and check if
     * "Restriction of menu options for manual PLMN selection" is
     * Enabled/Disabled
     *
     * @param data EF_CSP hex data.
     */
    private void HandleEfCspData(Byte[] data) {
        // As per spec CPHS4_2.WW6, CPHS B.4.7.1, EF_CSP contains CPHS defined
        // 18 Bytes (i.e 9 service groups info) and additional data specific to
        // operator. The valueAddedServicesGroup is not part of standard
        // services. This is operator specific and can be programmed any where.
        // Normally this is programmed as 10th service after the standard
        // services.
        Int32 usedCspGroups = data.length / 2;
        // This is the "Service Group Number" of "Value Added Services Group".
        Byte valueAddedServicesGroup = (Byte)0xC0;

        mCspPlmnEnabled = TRUE;
        For (Int32 i = 0; i < usedCspGroups; i++) {
             If (data[2 * i] == valueAddedServicesGroup) {
                 Log("[CSP] found ValueAddedServicesGroup, value " + data[(2 * i) + 1]);
                 If ((data[(2 * i) + 1] & 0x80) == 0x80) {
                     // Bit 8 is for
                     // "Restriction of menu options for manual PLMN selection".
                     // Operator Selection menu should be enabled.
                     mCspPlmnEnabled = TRUE;
                 } else {
                     mCspPlmnEnabled = FALSE;
                     // Operator Selection menu should be disabled.
                     // Operator Selection Mode should be set to Automatic.
                     Log("[CSP] Set Automatic Network Selection");
                     mNetworkSelectionModeAutomaticRegistrants->NotifyRegistrants();
                 }
                 return;
             }
        }

        Log("[CSP] Value Added Service Group (0xC0), not found!");
    }

    //@Override
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("SIMRecords: " + this);
        pw->Println(" extends:");
        super->Dump(fd, pw, args);
        pw->Println(" mVmConfig=" + mVmConfig);
        pw->Println(" mSpnOverride=" + mSpnOverride);
        pw->Println(" mCallForwardingEnabled=" + mCallForwardingEnabled);
        pw->Println(" mSpnState=" + mSpnState);
        pw->Println(" mCphsInfo=" + mCphsInfo);
        pw->Println(" mCspPlmnEnabled=" + mCspPlmnEnabled);
        pw->Println(" mEfMWIS[]=" + Arrays->ToString(mEfMWIS));
        pw->Println(" mEfCPHS_MWI[]=" + Arrays->ToString(mEfCPHS_MWI));
        pw->Println(" mEfCff[]=" + Arrays->ToString(mEfCff));
        pw->Println(" mEfCfis[]=" + Arrays->ToString(mEfCfis));
        pw->Println(" mSpnDisplayCondition=" + mSpnDisplayCondition);
        pw->Println(" mSpdiNetworks[]=" + mSpdiNetworks);
        pw->Println(" mPnnHomeName=" + mPnnHomeName);
        pw->Println(" mUsimServiceTable=" + mUsimServiceTable);
        pw->Println(" mGid1=" + mGid1);
        pw->Flush();
    }

}
