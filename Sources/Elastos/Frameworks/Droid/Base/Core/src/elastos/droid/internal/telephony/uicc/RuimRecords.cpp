/*
 * Copyright (C) 2008 The Android Open Source Project
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


using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_ICC_OPERATOR_ISO_COUNTRY;
using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_ICC_OPERATOR_NUMERIC;
using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_APN_RUIM_OPERATOR_NUMERIC;

using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_ICC_OPERATOR_ALPHA;
using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_TEST_CSIM;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::ILocale;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Droid::Internal::Telephony::IMccTable;

using Elastos::Droid::Internal::Telephony::Cdma::Sms::IUserData;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppType;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppState;
using Elastos::Droid::Internal::Telephony::Uicc::IUICCConfig;


/**
 * {@hide}
 */
public class RuimRecords extends IccRecords {
    static const String LOG_TAG = "RuimRecords";

    private Boolean  mOtaCommited=FALSE;

    // ***** Instance Variables

    private String mMyMobileNumber;
    private String mMin2Min1;

    private String mPrlVersion;
    private Boolean mRecordsRequired = FALSE;
    // From CSIM application
    private Byte[] mEFpl = NULL;
    private Byte[] mEFli = NULL;
    Boolean mCsimSpnDisplayCondition = FALSE;
    private String mMdn;
    private String mMin;
    private String mHomeSystemId;
    private String mHomeNetworkId;

    // ***** Ruim constants
    static const Int32 EF_MODEL_FILE_SIZE = 126;
    static const Int32 MODEL_INFORMATION_SIZE = 32;
    static const Int32 MANUFACTURER_NAME_SIZE = 32;
    static const Int32 SOFTWARE_VERSION_INFORMATION_SIZE = 60;
    static const Int32 LANGUAGE_INDICATOR_ENGLISH = 0x01;

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "RuimRecords: " + super->ToString()
                + " m_ota_commited" + mOtaCommited
                + " mMyMobileNumber=" + "xxxx"
                + " mMin2Min1=" + mMin2Min1
                + " mPrlVersion=" + mPrlVersion
                + " mEFpl=" + mEFpl
                + " mEFli=" + mEFli
                + " mCsimSpnDisplayCondition=" + mCsimSpnDisplayCondition
                + " mMdn=" + mMdn
                + " mMin=" + mMin
                + " mHomeSystemId=" + mHomeSystemId
                + " mHomeNetworkId=" + mHomeNetworkId;
    }

    //Constants
    //MNC length in case of CSIM/RUIM IMSI is 2 as per spec C.S0065 section 5.2.2
    private static const Int32 CSIM_IMSI_MNC_LENGTH = 2;

    // ***** Event Constants
    private static const Int32 EVENT_GET_DEVICE_IDENTITY_DONE = 4;
    private static const Int32 EVENT_GET_ICCID_DONE = 5;
    private static const Int32 EVENT_GET_RUIM_CST_DONE = 8;
    private static const Int32 EVENT_GET_CDMA_SUBSCRIPTION_DONE = 10;
    private static const Int32 EVENT_UPDATE_DONE = 14;
    private static const Int32 EVENT_SET_MODEL_DONE = 15;
    private static const Int32 EVENT_GET_SST_DONE = 17;
    private static const Int32 EVENT_GET_ALL_SMS_DONE = 18;
    private static const Int32 EVENT_MARK_SMS_READ_DONE = 19;

    private static const Int32 EVENT_SMS_ON_RUIM = 21;
    private static const Int32 EVENT_GET_SMS_DONE = 22;

    // RUIM ID is 8 bytes data
    private static const Int32 NUM_BYTES_RUIM_ID = 8;

    public RuimRecords(UiccCardApplication app, Context c, CommandsInterface ci) {
        Super(app, c, ci);

        mAdnCache = new AdnRecordCache(mFh);

        mRecordsRequested = FALSE;  // No load request is made till SIM ready

        // recordsToLoad is set to 0 because no requests are made yet
        mRecordsToLoad = 0;

        // NOTE the EVENT_SMS_ON_RUIM is not registered

        // Start off by setting empty state
        ResetRecords();

        mParentApp->RegisterForReady(this, EVENT_APP_READY, NULL);
        If (DBG) Log("RuimRecords X ctor this=" + this);
    }

    //@Override
    CARAPI Dispose() {
        If (DBG) Log("Disposing RuimRecords " + this);
        //Unregister for all events
        mParentApp->UnregisterForReady(this);
        ResetRecords();
        super->Dispose();
    }

    //@Override
    protected void Finalize() {
        If(DBG) Log("RuimRecords finalized");
    }

    protected void ResetRecords() {
        mMncLength = UNINITIALIZED;
        Log("setting0 mMncLength" + mMncLength);
        mIccId = NULL;

        mAdnCache->Reset();

        SetSystemProperty(PROPERTY_APN_RUIM_OPERATOR_NUMERIC, "");

        // Don't clean up PROPERTY_ICC_OPERATOR_ISO_COUNTRY and
        // PROPERTY_ICC_OPERATOR_NUMERIC here. Since not all CDMA
        // devices have RUIM, these properties should keep the original
        // values, e.g. build time settings, when there is no RUIM but
        // set new values when RUIM is available and loaded.

        // recordsRequested is set to FALSE indicating that the SIM
        // read requests made so far are not valid. This is set to
        // TRUE only when fresh set of read requests are made.
        mRecordsRequested = FALSE;
    }

    //@Override
    public String GetIMSI() {
        return mImsi;
    }

    public String GetMdnNumber() {
        return mMyMobileNumber;
    }

    public String GetCdmaMin() {
         return mMin2Min1;
    }

    /** Returns NULL if RUIM is not yet ready */
    public String GetPrlVersion() {
        return mPrlVersion;
    }

    //@Override
    CARAPI SetVoiceMailNumber(String alphaTag, String voiceNumber, Message onComplete){
        // In CDMA this is Operator/OEM dependent
        AsyncResult->ForMessage((onComplete)).exception =
                new IccException("setVoiceMailNumber not implemented");
        onComplete->SendToTarget();
        Loge("method setVoiceMailNumber is not implemented");
    }

    /**
     * Called by CCAT Service when REFRESH is received.
     * @param fileChanged indicates whether any files changed
     * @param fileList if non-NULL, a list of EF files that changed
     */
    //@Override
    CARAPI OnRefresh(Boolean fileChanged, Int32[] fileList) {
        If (fileChanged) {
            // A future optimization would be to inspect fileList and
            // only reload those files that we care about.  For now,
            // just re-fetch all RUIM records that we cache.
            FetchRuimRecords();
        }
    }

    private Int32 DecodeImsiDigits(Int32 digits, Int32 length) {
        // Per C.S0005 section 2.3.1.
        Int32 constant = 0;
        For (Int32 i = 0; i < length; i++ ) {
            constant = (constant * 10) + 1;
        }

        digits += constant;

        For (Int32 i = 0, denominator = 1; i < length; i++) {
            digits = ((digits / denominator) % 10 == 0) ? (digits - (10 * denominator)) : digits;
            denominator *= 10;
        }
        return digits;
    }

    /**
     * Decode utility to decode IMSI from data read from EF_IMSIM
     * Please refer to
     *       // C.S0065 section 5.2.2 for IMSI_M encoding
     *       // C.S0005 section 2.3.1 for MIN encoding in IMSI_M.
     */
    private String DecodeImsi(Byte[] data) {
        // Retrieve the MCC and digits 11 and 12
        Int32 mcc_data = ((0x03 & data[9]) << 8) | (0xFF & data[8]);
        Int32 mcc = DecodeImsiDigits(mcc_data, 3);
        Int32 digits_11_12_data = data[6] & 0x7f;
        Int32 digits_11_12 = DecodeImsiDigits(digits_11_12_data, 2);

        // Retrieve 10 MIN digits
        Int32 first3digits = ((0x03 & data[2]) << 8) + (0xFF & data[1]);
        Int32 second3digits = (((0xFF & data[5]) << 8) | (0xFF & data[4])) >> 6;
        Int32 digit7 = 0x0F & (data[4] >> 2);
        If (digit7 > 0x09) digit7 = 0;
        Int32 last3digits = ((0x03 & data[4]) << 8) | (0xFF & data[3]);

        first3digits = DecodeImsiDigits(first3digits, 3);
        second3digits = DecodeImsiDigits(second3digits, 3);
        last3digits = DecodeImsiDigits(last3digits, 3);

        StringBuilder builder = new StringBuilder();
        builder->Append(String->Format(Locale.US, "%03d", mcc));
        builder->Append(String->Format(Locale.US, "%02d", digits_11_12));
        builder->Append(String->Format(Locale.US, "%03d", first3digits));
        builder->Append(String->Format(Locale.US, "%03d", second3digits));
        builder->Append(String->Format(Locale.US, "%d", digit7));
        builder->Append(String->Format(Locale.US, "%03d", last3digits));
        return  builder->ToString();
    }


    /**
     * Returns the 5 or 6 digit MCC/MNC of the operator that
     *  provided the RUIM card. Returns NULL of RUIM is not yet ready
     */
    public String GetOperatorNumeric() {
        If (mImsi == NULL) {
            return NULL;
        }

        If (SystemProperties->GetBoolean("ro.telephony.get_imsi_from_sim", FALSE)) {
            String imsi = mParentApp->GetUICCConfig()->GetImsi();
            Int32 mnclength = mParentApp->GetUICCConfig()->GetMncLength();

            // If we are LTE over CDMA (Verizon), then pull the correct info from SIMRecords
            If (imsi != NULL) {
                Log("Overriding with Operator Numeric: " + imsi->Substring(0, 3 + mnclength));
                return imsi->Substring(0, 3 + mnclength);
            }
        }


        If (mMncLength != UNINITIALIZED && mMncLength != UNKNOWN) {
            // Length = length of MCC + length of MNC
            // length of mcc = 3 (3GPP2 C.S0005 - Section 2.3)
            return mImsi->Substring(0, 3 + mMncLength);
        }

        // Guess the MNC length based on the MCC if we don't
        // have a valid value in ef[ad]

        Int32 mcc = Integer->ParseInt(mImsi->Substring(0,3));
        return mImsi->Substring(0, 3 + CSIM_IMSI_MNC_LENGTH);
    }

    // Refer to ETSI TS 102.221
    private class EfPlLoaded implements IccRecordLoaded {
        //@Override
        public String GetEfName() {
            return "EF_PL";
        }

        //@Override
        CARAPI OnRecordLoaded(AsyncResult ar) {
            mEFpl = (Byte[]) ar.result;
            If (DBG) Log("EF_PL=" + IccUtils->BytesToHexString(mEFpl));
        }
    }

    // Refer to C.S0065 5.2.26
    private class EfCsimLiLoaded implements IccRecordLoaded {
        //@Override
        public String GetEfName() {
            return "EF_CSIM_LI";
        }

        //@Override
        CARAPI OnRecordLoaded(AsyncResult ar) {
            mEFli = (Byte[]) ar.result;
            // convert csim efli data to iso 639 format
            For (Int32 i = 0; i < mEFli.length; i+=2) {
                Switch(mEFli[i+1]) {
                case 0x01: mEFli[i] = 'e'; mEFli[i+1] = 'n';break;
                case 0x02: mEFli[i] = 'f'; mEFli[i+1] = 'r';break;
                case 0x03: mEFli[i] = 'e'; mEFli[i+1] = 's';break;
                case 0x04: mEFli[i] = 'j'; mEFli[i+1] = 'a';break;
                case 0x05: mEFli[i] = 'k'; mEFli[i+1] = 'o';break;
                case 0x06: mEFli[i] = 'z'; mEFli[i+1] = 'h';break;
                case 0x07: mEFli[i] = 'h'; mEFli[i+1] = 'e';break;
                default: mEFli[i] = ' '; mEFli[i+1] = ' ';
                }
            }

            If (DBG) Log("EF_LI=" + IccUtils->BytesToHexString(mEFli));
        }
    }

    // Refer to C.S0065 5.2.32
    private class EfCsimSpnLoaded implements IccRecordLoaded {
        //@Override
        public String GetEfName() {
            return "EF_CSIM_SPN";
        }

        //@Override
        CARAPI OnRecordLoaded(AsyncResult ar) {
            Byte[] data = (Byte[]) ar.result;
            If (DBG) Log("CSIM_SPN=" +
                         IccUtils->BytesToHexString(data));

            // C.S0065 for EF_SPN decoding
            mCsimSpnDisplayCondition = ((0x01 & data[0]) != 0);

            Int32 encoding = data[1];
            Int32 language = data[2];
            Byte[] spnData = new Byte[32];
            Int32 len = ((data.length - 3) < 32) ? (data.length - 3) : 32;
            System->Arraycopy(data, 3, spnData, 0, len);

            Int32 numBytes;
            For (numBytes = 0; numBytes < spnData.length; numBytes++) {
                If ((spnData[numBytes] & 0xFF) == 0xFF) break;
            }

            If (numBytes == 0) {
                SetServiceProviderName("");
                return;
            }
            try {
                Switch (encoding) {
                case UserData.ENCODING_OCTET:
                case UserData.ENCODING_LATIN:
                    SetServiceProviderName(new String(spnData, 0, numBytes, "ISO-8859-1"));
                    break;
                case UserData.ENCODING_IA5:
                case UserData.ENCODING_GSM_7BIT_ALPHABET:
                    SetServiceProviderName(
                            GsmAlphabet->Gsm7BitPackedToString(spnData, 0, (numBytes*8)/7));
                    break;
                case UserData.ENCODING_7BIT_ASCII:
                    String spn = new String(spnData, 0, numBytes, "US-ASCII");
                    // To address issues with incorrect encoding scheme
                    // programmed in some commercial CSIM cards, the decoded
                    // SPN is checked to have characters in printable ASCII
                    // range. If not, they are decoded with
                    // ENCODING_GSM_7BIT_ALPHABET scheme.
                    If (TextUtils->IsPrintableAsciiOnly(spn)) {
                        SetServiceProviderName(spn);
                    } else {
                        If (DBG) Log("Some corruption in SPN decoding = " + spn);
                        If (DBG) Log("Using ENCODING_GSM_7BIT_ALPHABET scheme...");
                        SetServiceProviderName(
                                GsmAlphabet->Gsm7BitPackedToString(spnData, 0, (numBytes * 8) / 7));
                    }
                break;
                case UserData.ENCODING_UNICODE_16:
                    SetServiceProviderName(new String(spnData, 0, numBytes, "utf-16"));
                    break;
                default:
                    Log("SPN encoding not supported");
                }
            } Catch(Exception e) {
                Log("spn decode error: " + e);
            }
            If (DBG) Log("spn=" + GetServiceProviderName());
            If (DBG) Log("spnCondition=" + mCsimSpnDisplayCondition);
            SystemProperties->Set(PROPERTY_ICC_OPERATOR_ALPHA, GetServiceProviderName());
        }
    }

    private class EfCsimMdnLoaded implements IccRecordLoaded {
        //@Override
        public String GetEfName() {
            return "EF_CSIM_MDN";
        }

        //@Override
        CARAPI OnRecordLoaded(AsyncResult ar) {
            Byte[] data = (Byte[]) ar.result;
            If (DBG) Log("CSIM_MDN=" + IccUtils->BytesToHexString(data));
            // Refer to C.S0065 5.2.35
            Int32 mdnDigitsNum = 0x0F & data[0];
            mMdn = IccUtils->CdmaBcdToString(data, 1, mdnDigitsNum);
            If (DBG) Log("CSIM MDN=" + mMdn);
        }
    }

    private class EfCsimImsimLoaded implements IccRecordLoaded {
        //@Override
        public String GetEfName() {
            return "EF_CSIM_IMSIM";
        }

        //@Override
        CARAPI OnRecordLoaded(AsyncResult ar) {
            Byte[] data = (Byte[]) ar.result;

            If (data == NULL || data.length < 10) {
                Log("Invalid IMSI from EF_CSIM_IMSIM " + IccUtils->BytesToHexString(data));
                mImsi = NULL;
                mMin = NULL;
                return;
            }
            If (DBG) Log("CSIM_IMSIM=" + IccUtils->BytesToHexString(data));

            // C.S0065 section 5.2.2 for IMSI_M encoding
            // C.S0005 section 2.3.1 for MIN encoding in IMSI_M.
            Boolean provisioned = ((data[7] & 0x80) == 0x80);

            If (provisioned) {
                mImsi = DecodeImsi(data);
                If (NULL != mImsi) {
                    mMin = mImsi->Substring(5, 15);
                }
                Log("IMSI: " + mImsi->Substring(0, 5) + "xxxxxxxxx");

            } else {
                If (DBG) Log("IMSI not provisioned in card");
            }

            //Update MccTable with the retrieved IMSI
            String operatorNumeric = GetOperatorNumeric();
            If (operatorNumeric != NULL) {
                If(operatorNumeric->Length() <= 6){
                    MccTable->UpdateMccMncConfiguration(mContext, operatorNumeric, FALSE);
                }
            }

            mImsiReadyRegistrants->NotifyRegistrants();
        }
   }

    private class EfCsimCdmaHomeLoaded implements IccRecordLoaded {
        //@Override
        public String GetEfName() {
            return "EF_CSIM_CDMAHOME";
        }

        //@Override
        CARAPI OnRecordLoaded(AsyncResult ar) {
            // Per C.S0065 section 5.2.8
            ArrayList<Byte[]> dataList = (ArrayList<Byte[]>) ar.result;
            If (DBG) Log("CSIM_CDMAHOME data size=" + dataList->Size());
            If (dataList->IsEmpty()) {
                return;
            }
            StringBuilder sidBuf = new StringBuilder();
            StringBuilder nidBuf = new StringBuilder();

            For (Byte[] data : dataList) {
                If (data.length == 5) {
                    Int32 sid = ((data[1] & 0xFF) << 8) | (data[0] & 0xFF);
                    Int32 nid = ((data[3] & 0xFF) << 8) | (data[2] & 0xFF);
                    sidBuf->Append(sid).Append(',');
                    nidBuf->Append(nid).Append(',');
                }
            }
            // remove trailing ","
            sidBuf->SetLength(sidBuf->Length()-1);
            nidBuf->SetLength(nidBuf->Length()-1);

            mHomeSystemId = sidBuf->ToString();
            mHomeNetworkId = nidBuf->ToString();
        }
    }

    private class EfCsimEprlLoaded implements IccRecordLoaded {
        //@Override
        public String GetEfName() {
            return "EF_CSIM_EPRL";
        }
        //@Override
        CARAPI OnRecordLoaded(AsyncResult ar) {
            OnGetCSimEprlDone(ar);
        }
    }

    private void OnGetCSimEprlDone(AsyncResult ar) {
        // C.S0065 section 5.2.57 for EFeprl encoding
        // C.S0016 section 3.5.5 for PRL format.
        Byte[] data = (Byte[]) ar.result;
        If (DBG) Log("CSIM_EPRL=" + IccUtils->BytesToHexString(data));

        // Only need the first 4 bytes of record
        If (data.length > 3) {
            Int32 prlId = ((data[2] & 0xFF) << 8) | (data[3] & 0xFF);
            mPrlVersion = Integer->ToString(prlId);
        }
        If (DBG) Log("CSIM PRL version=" + mPrlVersion);
    }

    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;

        Byte data[];

        Boolean isRecordLoadResponse = FALSE;

        If (mDestroyed->Get()) {
            Loge("Received message " + msg +
                    "[" + msg.what + "] while being destroyed. Ignoring.");
            return;
        }

        try { Switch (msg.what) {
            case EVENT_APP_READY:
                OnReady();
                break;

            case EVENT_GET_DEVICE_IDENTITY_DONE:
                Log("Event EVENT_GET_DEVICE_IDENTITY_DONE Received");
            break;

            case EVENT_GET_CDMA_SUBSCRIPTION_DONE:
                ar = (AsyncResult)msg.obj;
                String localTemp[] = (String[])ar.result;
                If (ar.exception != NULL) {
                    break;
                }

                mMyMobileNumber = localTemp[0];
                mMin2Min1 = localTemp[3];
                mPrlVersion = localTemp[4];

                Log("MDN: " + mMyMobileNumber + " MIN: " + mMin2Min1);

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

            case EVENT_UPDATE_DONE:
                ar = (AsyncResult)msg.obj;
                If (ar.exception != NULL) {
                    Rlog->I(LOG_TAG, "RuimRecords update failed", ar.exception);
                }
            break;

            case EVENT_GET_ALL_SMS_DONE:
            case EVENT_MARK_SMS_READ_DONE:
            case EVENT_SMS_ON_RUIM:
            case EVENT_GET_SMS_DONE:
                Rlog->W(LOG_TAG, "Event not supported: " + msg.what);
                break;

            // TODO: probably EF_CST should be read instead
            case EVENT_GET_SST_DONE:
                Log("Event EVENT_GET_SST_DONE Received");
            break;
            case EVENT_SET_MODEL_DONE:
                ar = (AsyncResult) msg.obj;
                If (ar.exception != NULL) {
                    Loge("Set EF Model failed" + ar.exception);
                }
                Log("EVENT_SET_MODEL_DONE");
                break;
            case EVENT_GET_RUIM_CST_DONE:
                // CSIM flags
                Boolean omhEnabled = FALSE;
                Boolean mmsicpEnabled = FALSE;
                isRecordLoadResponse = TRUE;
                ar = (AsyncResult) msg.obj;
                If (ar != NULL && ar.exception == NULL) {
                    data = (Byte[]) ar.result;
                    Log("EF CST data: " + IccUtils->BytesToHexString(data));
                    If (data == NULL) {
                        break;
                    }

                    // For CSIM card
                    If (mParentApp != NULL && mParentApp->GetType() == AppType.APPTYPE_CSIM) {
                        // Service n35, Messaging and 3GPD Extensions
                        If (data.length > 4) {
                            omhEnabled = (0x04 == (0x04 & data[4])) ? TRUE : FALSE;
                            If (omhEnabled) {
                                // Service n19, Multimedia Messaging Service (MMS)
                                mmsicpEnabled = (0x04 == (0x04 & data[2])) ? TRUE : FALSE;
                            }
                        } else {
                            Loge("CSIM EF CST data length = " + data.length);
                        }
                    } else {
                        If (data.length > 3) {
                            omhEnabled = (0x30 == (0x30 & data[3])) ? TRUE : FALSE;
                            // Judge the read MMSICP flag is enabled on CST or not
                            If (omhEnabled && data.length > 10) {
                                mmsicpEnabled = (0x0C == (0x0C & data[10])) ? TRUE : FALSE;
                            } else {
                                Loge("OMH EF CST data length = " + data.length);
                            }
                        } else {
                            Loge("OMH EF CST data length = " + data.length);
                        }
                    }
                    Log("mms icp enabled =" + mmsicpEnabled + " omhEnabled " + omhEnabled);
                    SystemProperties->Set(TelephonyProperties.PROPERTY_RUIM_OMH_CARD,
                            omhEnabled ? "TRUE" : "FALSE");
                }
                // We already know if it is an OMH card in this point
                FetchOMHCardRecords(omhEnabled);
                break;
            default:
                super->HandleMessage(msg);   // IccRecords handles generic record load responses

        }}Catch (RuntimeException exc) {
            // I don't want these exceptions to be fatal
            Rlog->W(LOG_TAG, "Exception parsing RUIM record", exc);
        } finally {
            // Count up record load responses even if they are fails
            If (isRecordLoadResponse) {
                OnRecordLoaded();
            }
        }
    }

    private void FetchOMHCardRecords(Boolean isOMHCard) {
        // OMH related events
        If (isOMHCard) {
            // Write device's software version on EF6F90 on startup.
            SetModel();
        }
    }

    /**
     * Returns an array of languages we have assets for.
     *
     * NOTE: This array will have duplicates. If this method will be caused
     * frequently or in a tight loop, it can be rewritten for efficiency.
     */
    private static String[] GetAssetLanguages(Context ctx) {
        final String[] locales = ctx->GetAssets()->GetLocales();
        final String[] localeLangs = new String[locales.length];
        For (Int32 i = 0; i < locales.length; ++i) {
            final String localeStr = locales[i];
            final Int32 separator = localeStr->IndexOf('-');
            If (separator < 0) {
                localeLangs[i] = localeStr;
            } else {
                localeLangs[i] = localeStr->Substring(0, separator);
            }
        }

        return localeLangs;
    }

    private String FindBestLanguage(Byte[] languages) {
        final String[] assetLanguages = GetAssetLanguages(mContext);

        If ((languages == NULL) || (assetLanguages == NULL)) return NULL;

        // Each 2-bytes consists of one language
        For (Int32 i = 0; (i + 1) < languages.length; i += 2) {
            final String lang;
            try {
                lang = new String(languages, i, 2, "ISO-8859-1");
            } Catch(java.io.UnsupportedEncodingException e) {
                Log("Failed to parse SIM language records");
                continue;
            }

            For (Int32 j = 0; j < assetLanguages.length; j++) {
                If (assetLanguages[j].Equals(lang)) {
                    return lang;
                }
            }
        }

        // no match found. return NULL
        return NULL;
    }

    private void SetLocaleFromCsim() {
        String prefLang = NULL;
        // check EFli then EFpl
        prefLang = FindBestLanguage(mEFli);

        If (prefLang == NULL) {
            prefLang = FindBestLanguage(mEFpl);
        }

        If (prefLang != NULL) {
            // check country code from SIM
            String imsi = GetIMSI();
            String country = NULL;
            If (imsi != NULL) {
                country = MccTable->CountryCodeForMcc(
                                    Integer->ParseInt(imsi->Substring(0,3)));
            }
            Log("Setting locale to " + prefLang + "_" + country);
            MccTable->SetSystemLocale(mContext, prefLang, country);
        } else {
            Log ("No suitable CSIM selected locale");
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

        // Further records that can be inserted are Operator/OEM dependent

        String operator = GetOperatorNumeric();
        If (!TextUtils->IsEmpty(operator)) {
            Log("onAllRecordsLoaded set 'gsm.sim.operator.numeric' to operator='" +
                    operator + "'");
            SetSystemProperty(PROPERTY_ICC_OPERATOR_NUMERIC, operator);
            SetSystemProperty(PROPERTY_APN_RUIM_OPERATOR_NUMERIC, operator);
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

        SetLocaleFromCsim();
        mRecordsLoadedRegistrants->NotifyRegistrants(
            new AsyncResult(NULL, NULL, NULL));
    }

    //@Override
    CARAPI OnReady() {
        FetchRuimRecords();

        mCi->GetCDMASubscription(ObtainMessage(EVENT_GET_CDMA_SUBSCRIPTION_DONE));
    }

    /**
     * Called by IccCardProxy before it requests records.
     * We use this as a trigger to read records from the card.
     */
    void RecordsRequired() {
        If (DBG) Log("recordsRequired");
        mRecordsRequired = TRUE;

        // trigger to retrieve all records
        FetchRuimRecords();
    }

    private void FetchRuimRecords() {
        Boolean mESNTrackerEnabled = mContext->GetResources()->GetBoolean(
                R.bool.config_telephony_ESN_Tracker_enabled);
        /* Don't read records if we don't expect
         * anyone to ask for them
         *
         * If we have already requested records OR
         * records are not required by anyone OR
         * the app is not ready
         * then bail
         */
        If (mRecordsRequested || (mESNTrackerEnabled ? FALSE : !mRecordsRequired)
            || AppState.APPSTATE_READY != mParentApp->GetState()) {
            If (DBG) Log("fetchRuimRecords: Abort fetching records rRecordsRequested = "
                            + mRecordsRequested
                            + " state = " + mParentApp->GetState()
                            + " required = " + mRecordsRequired);
            return;
        }

        mRecordsRequested = TRUE;

        If (DBG) Log("fetchRuimRecords " + mRecordsToLoad);

        mFh->LoadEFTransparent(EF_ICCID,
                ObtainMessage(EVENT_GET_ICCID_DONE));
        mRecordsToLoad++;

        mFh->LoadEFTransparent(EF_PL,
                ObtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfPlLoaded()));
        mRecordsToLoad++;

        mFh->LoadEFTransparent(EF_CSIM_LI,
                ObtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfCsimLiLoaded()));
        mRecordsToLoad++;

        mFh->LoadEFTransparent(EF_CSIM_SPN,
                ObtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfCsimSpnLoaded()));
        mRecordsToLoad++;

        mFh->LoadEFLinearFixed(EF_CSIM_MDN, 1,
                ObtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfCsimMdnLoaded()));
        mRecordsToLoad++;

        mFh->LoadEFTransparent(EF_CSIM_IMSIM,
                ObtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfCsimImsimLoaded()));
        mRecordsToLoad++;

        mFh->LoadEFLinearFixedAll(EF_CSIM_CDMAHOME,
                ObtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfCsimCdmaHomeLoaded()));
        mRecordsToLoad++;

        If (mESNTrackerEnabled) {
            mFh->LoadEFTransparent(EF_CSIM_MODEL,
                    ObtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfCsimModelLoaded()));
            mRecordsToLoad++;

            mFh->LoadEFTransparent(EF_MODEL,
                    ObtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfRuimModelLoaded()));
            mRecordsToLoad++;

            // We need to read CST table first to see if it is a CT/OMH/Legacy card
            mFh->LoadEFTransparent(IccConstants.EF_CST,
                    ObtainMessage(EVENT_GET_RUIM_CST_DONE));
            mRecordsToLoad++;

            mFh->LoadEFTransparent(EF_RUIM_ID,
                    ObtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfRuimIdLoaded()));
            mRecordsToLoad++;
        }

        // Entire PRL could be huge. We are only interested in
        // the first 4 bytes of the record.
        mFh->LoadEFTransparent(EF_CSIM_EPRL, 4,
                ObtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfCsimEprlLoaded()));
        mRecordsToLoad++;

        mFh->GetEFLinearRecordSize(EF_SMS, ObtainMessage(EVENT_GET_SMS_RECORD_SIZE_DONE));

        If (DBG) Log("fetchRuimRecords " + mRecordsToLoad + " requested: " + mRecordsRequested);
        // Further records that can be inserted are Operator/OEM dependent
    }

    private class EfCsimModelLoaded implements IccRecordLoaded {
        public String GetEfName() {
            return "EF_CSIM_MODEL";
        }

        CARAPI OnRecordLoaded(AsyncResult ar) {
            Byte[] data = (Byte[]) ar.result;
            If (DBG)
                Log("EF_CSIM_MODEL=" + IccUtils->BytesToHexString(data));
        }
    }

    private class EfRuimModelLoaded implements IccRecordLoaded {
        public String GetEfName() {
            return "EF_RUIM_MODEL";
        }

        CARAPI OnRecordLoaded(AsyncResult ar) {
            Byte[] data = (Byte[]) ar.result;
            If (DBG)
                Log("EF_RUIM_MODEL=" + IccUtils->BytesToHexString(data));
        }
    }

    private class EfRuimIdLoaded implements IccRecordLoaded {
        public String GetEfName() {
            return "EF_RUIM_ID";
        }

        CARAPI OnRecordLoaded(AsyncResult ar) {
            // The first Byte represent the num bytes of valid data for RUIM ID data.
            // It is valid RUIM ID data From the second Byte to num+1 Byte.
            // And the second Byte is the lowest-order Byte, the num+1 Byte is highest-order
            Byte[] data = (Byte[]) ar.result;
            If (DBG)
                Log("RuimId Data=" + IccUtils->BytesToHexString(data));
            If (data == NULL) {
                return;
            }
            Int32 numOfBytes = data[0];
            If (numOfBytes < NUM_BYTES_RUIM_ID) {
                Byte[] decodeData = new Byte[numOfBytes];
                For (Int32 i = 0; i < numOfBytes; i++) {
                    decodeData[i] = data[numOfBytes - i];
                }
                String ruimId = IccUtils->BytesToHexString(decodeData);
                If (DBG)
                    Log("RUIM_ID=" + ruimId);
            }
        }
    }

    // Set software version/manufacturer information into EFmodel elementary
    private void SetModel() {
        Byte[] data = new Byte[EF_MODEL_FILE_SIZE];
        Byte[] model = NULL;
        Byte[] manufacturer = NULL;
        Byte[] softwareVersion = NULL;

        For (Int32 i = 0; i < data.length; i++) {
            data[i] = (Byte) 0xff;
        }
        // EFmodel will be written into UTF-8 characters
        try {
            model = Build.MODEL->GetBytes("UTF-8");
            manufacturer = Build.MANUFACTURER->GetBytes("UTF-8");
            String str = SystemProperties->Get("persist.product.sw.version", Build.DISPLAY);
            softwareVersion = str->GetBytes("UTF-8");
        } Catch (UnsupportedEncodingException e) {
            Loge("BearerData encode failed: " + e);
        }
        data[0] = UserData.ENCODING_OCTET;
        data[1] = LANGUAGE_INDICATOR_ENGLISH;

        // Restrict model length to prevent ArrayIndexOutOfBoundsException
        Int32 offset = 2;
        Int32 modelLen = (model.length > MODEL_INFORMATION_SIZE)
                ? MODEL_INFORMATION_SIZE : model.length;
        System->Arraycopy(model, 0, data, offset, modelLen);
        offset += MODEL_INFORMATION_SIZE;
        // Restrict manufacture length to prevent ArrayIndexOutOfBoundsException
        Int32 manufactureLen = (manufacturer.length > MANUFACTURER_NAME_SIZE)
                ? MANUFACTURER_NAME_SIZE : manufacturer.length;
        System->Arraycopy(manufacturer, 0, data, offset, manufactureLen);
        // We need to restrict versionLength in case it is too Int64 which maybe lead to
        // arrayoutofindex exception
        offset += MANUFACTURER_NAME_SIZE;
        Int32 versionLength = (softwareVersion.length > SOFTWARE_VERSION_INFORMATION_SIZE)
                ? SOFTWARE_VERSION_INFORMATION_SIZE : softwareVersion.length;
        System->Arraycopy(softwareVersion, 0, data, offset, versionLength);

        Log("model: " + IccUtils->BytesToHexString(model) + "manufacturer: "
                + IccUtils->BytesToHexString(manufacturer) + "softwareVersion: "
                + IccUtils->BytesToHexString(softwareVersion));
        Log("EF model write data : " + IccUtils->BytesToHexString(data) + " version length="
                + versionLength);

        If (mParentApp != NULL && mParentApp->GetType() == AppType.APPTYPE_CSIM) {
            Log("CSIM card type, set csim model");
            mFh->UpdateEFTransparent(IccConstants.EF_CSIM_MODEL, data,
                    ObtainMessage(EVENT_SET_MODEL_DONE, IccConstants.EF_CSIM_MODEL));
        } else {
            mFh->UpdateEFTransparent(IccConstants.EF_MODEL, data,
                    ObtainMessage(EVENT_SET_MODEL_DONE, IccConstants.EF_MODEL));
        }
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    public Int32 GetDisplayRule(String plmnNumeric) {
        Int32 rule = 0;

        If ((mContext != NULL) && mContext->GetResources()->GetBoolean(
            R.bool.def_telephony_spn_spec_enabled)) {
            // Always display the SPN only from RUIM
            rule = SPN_RULE_SHOW_SPN;
        } else If (mParentApp != NULL && mParentApp->GetUiccCard() != NULL &&
                mParentApp->GetUiccCard()->GetOperatorBrandOverride() != NULL) {
            // use operator brand override
            rule = SPN_RULE_SHOW_PLMN;
        } else If (TextUtils->IsEmpty(GetServiceProviderName())) {
            // EF_SPN content not found on this RUIM, or not yet loaded
            rule = SPN_RULE_SHOW_PLMN;
        } else If (IsOnMatchingPlmn(plmnNumeric)) {
            // on home network
            If (mCsimSpnDisplayCondition && !TextUtils->IsEmpty(GetServiceProviderName())
                    && (!SystemProperties->GetBoolean("ro.cdma.force_plmn_lookup", FALSE))) {
                // check CSIM SPN Display Condition (applicable on home network),
                // but only if SPN was found on this RUIM
                // if force plmn lookup then plmn should be used
                rule = SPN_RULE_SHOW_SPN;
            } else {
                // CSIM SPN Display does not require a SPN display, or SPN not found on RUIM,
                // then revert to currently registered network
                rule = SPN_RULE_SHOW_PLMN;
            }
        } else {
            // roaming, use the currently registered network
            rule = SPN_RULE_SHOW_PLMN;
        }

        return rule;
    }

    /**
    * Checks if currently registered PLMN is home PLMN
    * (PLMN numeric matches the one reported in CSIM)
    */
    private Boolean IsOnMatchingPlmn(String plmnNumeric) {
        If (plmnNumeric == NULL) return FALSE;
        If (plmnNumeric->Equals(GetOperatorNumeric())) {
            return TRUE;
        }
        return FALSE;
    }

    //@Override
    public Boolean IsProvisioned() {
        // If UICC card has CSIM app, look for MDN and MIN field
        // to determine if the SIM is provisioned.  Otherwise,
        // consider the SIM is provisioned. (for case of ordinal
        // USIM only UICC.)
        // If PROPERTY_TEST_CSIM is defined, bypess provision check
        // and consider the SIM is provisioned.
        If (SystemProperties->GetBoolean(PROPERTY_TEST_CSIM, FALSE)) {
            return TRUE;
        }

        If (mParentApp == NULL) {
            return FALSE;
        }

        If (mParentApp->GetType() == AppType.APPTYPE_CSIM &&
            ((mMdn == NULL) || (mMin == NULL))) {
            return FALSE;
        }
        return TRUE;
    }

    //@Override
    CARAPI SetVoiceMessageWaiting(Int32 line, Int32 countWaiting) {
        // Will be used in future to store voice mail count in UIM
        // C.S0023-D_v1.0 does not have a file id in UIM for MWI
        Log("RuimRecords:setVoiceMessageWaiting - NOP for CDMA");
    }

    //@Override
    public Int32 GetVoiceMessageCount() {
        // Will be used in future to retrieve voice mail count for UIM
        // C.S0023-D_v1.0 does not have a file id in UIM for MWI
        Log("RuimRecords:getVoiceMessageCount - NOP for CDMA");
        return 0;
    }

    //@Override
    protected void HandleFileUpdate(Int32 efid) {
        Switch (efid) {
            case EF_ADN:
                Log("SIM Refresh for EF_ADN");
                mAdnCache->Reset();
                break;
            default:
                mAdnCache->Reset();
                FetchRuimRecords();
                break;
        }
    }

    public String GetMdn() {
        return mMdn;
    }

    public String GetMin() {
        return mMin;
    }

    public String GetSid() {
        return mHomeSystemId;
    }

    public String GetNid() {
        return mHomeNetworkId;
    }

    public Boolean GetCsimSpnDisplayCondition() {
        return mCsimSpnDisplayCondition;
    }
    //@Override
    protected void Log(String s) {
        Rlog->D(LOG_TAG, "[RuimRecords] " + s);
    }

    //@Override
    protected void Loge(String s) {
        Rlog->E(LOG_TAG, "[RuimRecords] " + s);
    }

    //@Override
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("RuimRecords: " + this);
        pw->Println(" extends:");
        super->Dump(fd, pw, args);
        pw->Println(" mOtaCommited=" + mOtaCommited);
        pw->Println(" mMyMobileNumber=" + mMyMobileNumber);
        pw->Println(" mMin2Min1=" + mMin2Min1);
        pw->Println(" mPrlVersion=" + mPrlVersion);
        pw->Println(" mEFpl[]=" + Arrays->ToString(mEFpl));
        pw->Println(" mEFli[]=" + Arrays->ToString(mEFli));
        pw->Println(" mCsimSpnDisplayCondition=" + mCsimSpnDisplayCondition);
        pw->Println(" mMdn=" + mMdn);
        pw->Println(" mMin=" + mMin);
        pw->Println(" mHomeSystemId=" + mHomeSystemId);
        pw->Println(" mHomeNetworkId=" + mHomeNetworkId);
        pw->Flush();
    }

}
