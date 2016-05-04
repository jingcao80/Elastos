/*
 * Copyright (C) 2011 The Android Open Source Project
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
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Content::IIntent;


using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::Gsm::ISimTlv;
//import Elastos.Droid.internal.telephony.gsm.VoiceMailConstants;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Nio::Charset::ICharset;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;

using static com::Android::Internal::Telephony::Uicc::IccConstants::IEF_DOMAIN;
using static com::Android::Internal::Telephony::Uicc::IccConstants::IEF_IMPI;
using static com::Android::Internal::Telephony::Uicc::IccConstants::IEF_IMPU;
using static com::Android::Internal::Telephony::Uicc::IccConstants::IEF_IST;
using static com::Android::Internal::Telephony::Uicc::IccConstants::IEF_PCSCF;

/**
 * {@hide}
 */
public class IsimUiccRecords extends IccRecords implements IsimRecords {
    protected static const String LOG_TAG = "IsimUiccRecords";

    private static const Boolean DBG = TRUE;
    private static const Boolean DUMP_RECORDS = TRUE;   // Note: PII is logged when this is TRUE
    public static const String INTENT_ISIM_REFRESH = "com.android.intent.isim_refresh";

    private static const Int32 EVENT_APP_READY = 1;
    private static const Int32 EVENT_AKA_AUTHENTICATE_DONE          = 90;

    // ISIM EF Records (see 3GPP TS 31.103)
    private String mIsimImpi;               // IMS private user identity
    private String mIsimDomain;             // IMS home network domain name
    private String[] mIsimImpu;             // IMS public user Identity(s)
    private String mIsimIst;             // IMS Service Table
    private String[] mIsimPcscf;             // IMS Proxy Call Session Control Function
    private String auth_rsp;

    private final Object mLock = new Object();

    private static const Int32 TAG_ISIM_VALUE = 0x80;     // From 3GPP TS 31.103

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "IsimUiccRecords: " + super->ToString()
                + " mIsimImpi=" + mIsimImpi
                + " mIsimDomain=" + mIsimDomain
                + " mIsimImpu=" + mIsimImpu
                + " mIsimIst=" + mIsimIst
                + " mIsimPcscf=" + mIsimPcscf;
    }

    public IsimUiccRecords(UiccCardApplication app, Context c, CommandsInterface ci) {
        Super(app, c, ci);

        mAdnCache = new AdnRecordCache(mFh);

        mRecordsRequested = FALSE;  // No load request is made till SIM ready

        // recordsToLoad is set to 0 because no requests are made yet
        mRecordsToLoad = 0;
        // Start off by setting empty state
        ResetRecords();

        mParentApp->RegisterForReady(this, EVENT_APP_READY, NULL);
        If (DBG) Log("IsimUiccRecords X ctor this=" + this);
    }

    //@Override
    CARAPI Dispose() {
        Log("Disposing " + this);
        //Unregister for all events
        mParentApp->UnregisterForReady(this);
        ResetRecords();
        super->Dispose();
    }

    // ***** Overridden from Handler
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;

        If (mDestroyed->Get()) {
            Rlog->E(LOG_TAG, "Received message " + msg +
                    "[" + msg.what + "] while being destroyed. Ignoring.");
            return;
        }
        Loge("IsimUiccRecords: handleMessage " + msg + "[" + msg.what + "] ");

        try {
            Switch (msg.what) {
                case EVENT_APP_READY:
                    OnReady();
                    break;

                case EVENT_AKA_AUTHENTICATE_DONE:
                    ar = (AsyncResult)msg.obj;
                    Log("EVENT_AKA_AUTHENTICATE_DONE");
                    If (ar.exception != NULL) {
                        Log("Exception ISIM AKA: " + ar.exception);
                    } else {
                        try {
                            auth_rsp = (String)ar.result;
                            Log("ISIM AKA: auth_rsp = " + auth_rsp);
                        } Catch (Exception e) {
                            Log("Failed to parse ISIM AKA contents: " + e);
                        }
                    }
                    Synchronized (mLock) {
                        mLock->NotifyAll();
                    }

                    break;

                default:
                    super->HandleMessage(msg);   // IccRecords handles generic record load responses

            }
        } Catch (RuntimeException exc) {
            // I don't want these exceptions to be fatal
            Rlog->W(LOG_TAG, "Exception parsing SIM record", exc);
        }
    }

    protected void FetchIsimRecords() {
        mRecordsRequested = TRUE;

        mFh->LoadEFTransparent(EF_IMPI, ObtainMessage(
                IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimImpiLoaded()));
        mRecordsToLoad++;

        mFh->LoadEFLinearFixedAll(EF_IMPU, ObtainMessage(
                IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimImpuLoaded()));
        mRecordsToLoad++;

        mFh->LoadEFTransparent(EF_DOMAIN, ObtainMessage(
                IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimDomainLoaded()));
        mRecordsToLoad++;
        mFh->LoadEFTransparent(EF_IST, ObtainMessage(
                    IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimIstLoaded()));
        mRecordsToLoad++;
        mFh->LoadEFLinearFixedAll(EF_PCSCF, ObtainMessage(
                    IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimPcscfLoaded()));
        mRecordsToLoad++;

        If (DBG) Log("fetchIsimRecords " + mRecordsToLoad + " requested: " + mRecordsRequested);
    }

    protected void ResetRecords() {
        // recordsRequested is set to FALSE indicating that the SIM
        // read requests made so far are not valid. This is set to
        // TRUE only when fresh set of read requests are made.
        mIsimImpi = NULL;
        mIsimDomain = NULL;
        mIsimImpu = NULL;
        mIsimIst = NULL;
        mIsimPcscf = NULL;
        auth_rsp = NULL;

        mRecordsRequested = FALSE;
    }

    private class EfIsimImpiLoaded implements IccRecords.IccRecordLoaded {
        public String GetEfName() {
            return "EF_ISIM_IMPI";
        }
        CARAPI OnRecordLoaded(AsyncResult ar) {
            Byte[] data = (Byte[]) ar.result;
            mIsimImpi = IsimTlvToString(data);
            If (DUMP_RECORDS) Log("EF_IMPI=" + mIsimImpi);
        }
    }

    private class EfIsimImpuLoaded implements IccRecords.IccRecordLoaded {
        public String GetEfName() {
            return "EF_ISIM_IMPU";
        }
        CARAPI OnRecordLoaded(AsyncResult ar) {
            ArrayList<Byte[]> impuList = (ArrayList<Byte[]>) ar.result;
            If (DBG) Log("EF_IMPU record count: " + impuList->Size());
            mIsimImpu = new String[impuList->Size()];
            Int32 i = 0;
            For (Byte[] identity : impuList) {
                String impu = IsimTlvToString(identity);
                If (DUMP_RECORDS) Log("EF_IMPU[" + i + "]=" + impu);
                mIsimImpu[i++] = impu;
            }
        }
    }

    private class EfIsimDomainLoaded implements IccRecords.IccRecordLoaded {
        public String GetEfName() {
            return "EF_ISIM_DOMAIN";
        }
        CARAPI OnRecordLoaded(AsyncResult ar) {
            Byte[] data = (Byte[]) ar.result;
            mIsimDomain = IsimTlvToString(data);
            If (DUMP_RECORDS) Log("EF_DOMAIN=" + mIsimDomain);
        }
    }

    private class EfIsimIstLoaded implements IccRecords.IccRecordLoaded {
        public String GetEfName() {
            return "EF_ISIM_IST";
        }
        CARAPI OnRecordLoaded(AsyncResult ar) {
            Byte[] data = (Byte[]) ar.result;
            mIsimIst = IccUtils->BytesToHexString(data);
            If (DUMP_RECORDS) Log("EF_IST=" + mIsimIst);
        }
    }
    private class EfIsimPcscfLoaded implements IccRecords.IccRecordLoaded {
        public String GetEfName() {
            return "EF_ISIM_PCSCF";
        }
        CARAPI OnRecordLoaded(AsyncResult ar) {
            ArrayList<Byte[]> pcscflist = (ArrayList<Byte[]>) ar.result;
            If (DBG) Log("EF_PCSCF record count: " + pcscflist->Size());
            mIsimPcscf = new String[pcscflist->Size()];
            Int32 i = 0;
            For (Byte[] identity : pcscflist) {
                String pcscf = IsimTlvToString(identity);
                If (DUMP_RECORDS) Log("EF_PCSCF[" + i + "]=" + pcscf);
                mIsimPcscf[i++] = pcscf;
            }
        }
    }

    /**
     * ISIM records for IMS are stored inside a Tag-Length-Value record as a UTF-8 string
     * with tag value 0x80.
     * @param record the Byte array containing the IMS data string
     * @return the decoded String value, or NULL if the record can't be decoded
     */
    private static String IsimTlvToString(Byte[] record) {
        SimTlv tlv = new SimTlv(record, 0, record.length);
        do {
            If (tlv->GetTag() == TAG_ISIM_VALUE) {
                return new String(tlv->GetData(), Charset->ForName("UTF-8"));
            }
        } While (tlv->NextObject());

        Rlog->E(LOG_TAG, "[ISIM] can't find TLV tag in ISIM record, returning NULL");
        return NULL;
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
        mRecordsLoadedRegistrants->NotifyRegistrants(
                new AsyncResult(NULL, NULL, NULL));
    }

    //@Override
    protected void HandleFileUpdate(Int32 efid) {
        Switch (efid) {
            case EF_IMPI:
                mFh->LoadEFTransparent(EF_IMPI, ObtainMessage(
                            IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimImpiLoaded()));
                mRecordsToLoad++;
                break;

            case EF_IMPU:
                mFh->LoadEFLinearFixedAll(EF_IMPU, ObtainMessage(
                            IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimImpuLoaded()));
                mRecordsToLoad++;
            break;

            case EF_DOMAIN:
                mFh->LoadEFTransparent(EF_DOMAIN, ObtainMessage(
                            IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimDomainLoaded()));
                mRecordsToLoad++;
            break;

            case EF_IST:
                mFh->LoadEFTransparent(EF_IST, ObtainMessage(
                            IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimIstLoaded()));
                mRecordsToLoad++;
            break;

            case EF_PCSCF:
                mFh->LoadEFLinearFixedAll(EF_PCSCF, ObtainMessage(
                            IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimPcscfLoaded()));
                mRecordsToLoad++;

            default:
                FetchIsimRecords();
                break;
        }
    }

    //@Override
    protected void BroadcastRefresh() {
        Intent intent = new Intent(INTENT_ISIM_REFRESH);
        Log("send ISim REFRESH: " + INTENT_ISIM_REFRESH);
        mContext->SendBroadcast(intent);
    }

    /**
     * Return the IMS private user Identity (IMPI).
     * Returns NULL if the IMPI hasn't been loaded or isn't present on the ISIM.
     * @return the IMS private user identity string, or NULL if not available
     */
    //@Override
    public String GetIsimImpi() {
        return mIsimImpi;
    }

    /**
     * Return the IMS home network domain name.
     * Returns NULL if the IMS domain hasn't been loaded or isn't present on the ISIM.
     * @return the IMS home network domain name, or NULL if not available
     */
    //@Override
    public String GetIsimDomain() {
        return mIsimDomain;
    }

    /**
     * Return an array of IMS public user Identities (IMPU).
     * Returns NULL if the IMPU hasn't been loaded or isn't present on the ISIM.
     * @return an array of IMS public user identity strings, or NULL if not available
     */
    //@Override
    public String[] GetIsimImpu() {
        Return (mIsimImpu != NULL) ? mIsimImpu->Clone() : NULL;
    }

    /**
     * Returns the IMS Service Table (IST) that was loaded from the ISIM.
     * @return IMS Service Table or NULL if not present or not loaded
     */
    //@Override
    public String GetIsimIst() {
        return mIsimIst;
    }

    /**
     * Returns the IMS Proxy Call Session Control Function(PCSCF) that were loaded from the ISIM.
     * @return an array of  PCSCF strings with one PCSCF per string, or NULL if
     *      not present or not loaded
     */
    //@Override
    public String[] GetIsimPcscf() {
        Return (mIsimPcscf != NULL) ? mIsimPcscf->Clone() : NULL;
    }

    /**
     * Returns the response of ISIM Authetification through RIL.
     * Returns NULL if the Authentification hasn't been successed or isn't present iphonesubinfo.
     * @return the response of ISIM Authetification, or NULL if not available
     */
    //@Override
    public String GetIsimChallengeResponse(String nonce){
        If (DBG) Log("getIsimChallengeResponse-nonce:"+nonce);
        try {
            Synchronized(mLock) {
                mCi->RequestIsimAuthentication(nonce,ObtainMessage(EVENT_AKA_AUTHENTICATE_DONE));
                try {
                    mLock->Wait();
                } Catch (InterruptedException e) {
                    Log("interrupted while trying to request Isim Auth");
                }
            }
        } Catch(Exception e) {
            If (DBG) Log( "Fail while trying to request Isim Auth");
            return NULL;
        }

        If (DBG) Log("getIsimChallengeResponse-auth_rsp"+auth_rsp);

        return auth_rsp;
    }

    //@Override
    public Int32 GetDisplayRule(String plmn) {
        // Not applicable to Isim
        return 0;
    }

    //@Override
    CARAPI OnReady() {
        FetchIsimRecords();
    }

    //@Override
    CARAPI OnRefresh(Boolean fileChanged, Int32[] fileList) {
        If (fileChanged) {
            // A future optimization would be to inspect fileList and
            // only reload those files that we care about.  For now,
            // just re-fetch all SIM records that we cache.
            FetchIsimRecords();
        }
    }

    //@Override
    CARAPI SetVoiceMailNumber(String alphaTag, String voiceNumber,
            Message onComplete) {
        // Not applicable to Isim
    }

    //@Override
    CARAPI SetVoiceMessageWaiting(Int32 line, Int32 countWaiting) {
        // Not applicable to Isim
    }

    //@Override
    protected void Log(String s) {
        If (DBG) Rlog->D(LOG_TAG, "[ISIM] " + s);
    }

    //@Override
    protected void Loge(String s) {
        If (DBG) Rlog->E(LOG_TAG, "[ISIM] " + s);
    }

    //@Override
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("IsimRecords: " + this);
        pw->Println(" extends:");
        super->Dump(fd, pw, args);
        pw->Println(" mIsimImpi=" + mIsimImpi);
        pw->Println(" mIsimDomain=" + mIsimDomain);
        pw->Println(" mIsimImpu[]=" + Arrays->ToString(mIsimImpu));
        pw->Println(" mIsimIst" + mIsimIst);
        pw->Println(" mIsimPcscf"+mIsimPcscf);
        pw->Flush();
    }

    //@Override
    public Int32 GetVoiceMessageCount() {
        return 0; // Not applicable to Isim
    }

}
