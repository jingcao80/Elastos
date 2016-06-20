#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/internal/telephony/uicc/RuimRecords.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                       RuimRecords::EfPlLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfPlLoaded, Object, IIccRecordLoaded);

ECode RuimRecords::EfPlLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "EF_PL";
    assert(0);
    return NOERROR;
}

ECode RuimRecords::EfPlLoaded::OnRecordLoaded(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // mEFpl = (byte[]) ar.result;
    // if (DBG) log("EF_PL=" + IccUtils.bytesToHexString(mEFpl));
    assert(0);
    return NOERROR;
}

//=====================================================================
//                     RuimRecords::EfCsimLiLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfCsimLiLoaded, Object, IIccRecordLoaded);

ECode RuimRecords::EfCsimLiLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "EF_CSIM_LI";
    assert(0);
    return NOERROR;
}

ECode RuimRecords::EfCsimLiLoaded::OnRecordLoaded(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // mEFli = (byte[]) ar.result;
    // // convert csim efli data to iso 639 format
    // for (int i = 0; i < mEFli.length; i+=2) {
    //     switch(mEFli[i+1]) {
    //     case 0x01: mEFli[i] = 'e'; mEFli[i+1] = 'n';break;
    //     case 0x02: mEFli[i] = 'f'; mEFli[i+1] = 'r';break;
    //     case 0x03: mEFli[i] = 'e'; mEFli[i+1] = 's';break;
    //     case 0x04: mEFli[i] = 'j'; mEFli[i+1] = 'a';break;
    //     case 0x05: mEFli[i] = 'k'; mEFli[i+1] = 'o';break;
    //     case 0x06: mEFli[i] = 'z'; mEFli[i+1] = 'h';break;
    //     case 0x07: mEFli[i] = 'h'; mEFli[i+1] = 'e';break;
    //     default: mEFli[i] = ' '; mEFli[i+1] = ' ';
    //     }
    // }
    //
    // if (DBG) log("EF_LI=" + IccUtils.bytesToHexString(mEFli));
    assert(0);
    return NOERROR;
}

//=====================================================================
//                     RuimRecords::EfCsimSpnLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfCsimSpnLoaded, Object, IIccRecordLoaded);

ECode RuimRecords::EfCsimSpnLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "EF_CSIM_SPN";
    assert(0);
    return NOERROR;
}

ECode RuimRecords::EfCsimSpnLoaded::OnRecordLoaded(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // byte[] data = (byte[]) ar.result;
    // if (DBG) log("CSIM_SPN=" +
    //              IccUtils.bytesToHexString(data));
    //
    // // C.S0065 for EF_SPN decoding
    // mCsimSpnDisplayCondition = ((0x01 & data[0]) != 0);
    //
    // int encoding = data[1];
    // int language = data[2];
    // byte[] spnData = new byte[32];
    // int len = ((data.length - 3) < 32) ? (data.length - 3) : 32;
    // System.arraycopy(data, 3, spnData, 0, len);
    //
    // int numBytes;
    // for (numBytes = 0; numBytes < spnData.length; numBytes++) {
    //     if ((spnData[numBytes] & 0xFF) == 0xFF) break;
    // }
    //
    // if (numBytes == 0) {
    //     setServiceProviderName("");
    //     return;
    // }
    // try {
    //     switch (encoding) {
    //     case UserData.ENCODING_OCTET:
    //     case UserData.ENCODING_LATIN:
    //         setServiceProviderName(new String(spnData, 0, numBytes, "ISO-8859-1"));
    //         break;
    //     case UserData.ENCODING_IA5:
    //     case UserData.ENCODING_GSM_7BIT_ALPHABET:
    //         setServiceProviderName(
    //                 GsmAlphabet.gsm7BitPackedToString(spnData, 0, (numBytes*8)/7));
    //         break;
    //     case UserData.ENCODING_7BIT_ASCII:
    //         String spn = new String(spnData, 0, numBytes, "US-ASCII");
    //         // To address issues with incorrect encoding scheme
    //         // programmed in some commercial CSIM cards, the decoded
    //         // SPN is checked to have characters in printable ASCII
    //         // range. If not, they are decoded with
    //         // ENCODING_GSM_7BIT_ALPHABET scheme.
    //         if (TextUtils.isPrintableAsciiOnly(spn)) {
    //             setServiceProviderName(spn);
    //         } else {
    //             if (DBG) log("Some corruption in SPN decoding = " + spn);
    //             if (DBG) log("Using ENCODING_GSM_7BIT_ALPHABET scheme...");
    //             setServiceProviderName(
    //                     GsmAlphabet.gsm7BitPackedToString(spnData, 0, (numBytes * 8) / 7));
    //         }
    //     break;
    //     case UserData.ENCODING_UNICODE_16:
    //         setServiceProviderName(new String(spnData, 0, numBytes, "utf-16"));
    //         break;
    //     default:
    //         log("SPN encoding not supported");
    //     }
    // } catch(Exception e) {
    //     log("spn decode error: " + e);
    // }
    // if (DBG) log("spn=" + getServiceProviderName());
    // if (DBG) log("spnCondition=" + mCsimSpnDisplayCondition);
    // SystemProperties.set(PROPERTY_ICC_OPERATOR_ALPHA, getServiceProviderName());
    assert(0);
    return NOERROR;
}

//=====================================================================
//                     RuimRecords::EfCsimMdnLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfCsimMdnLoaded, Object, IIccRecordLoaded);

ECode RuimRecords::EfCsimMdnLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "EF_CSIM_MDN";
    assert(0);
    return NOERROR;
}

ECode RuimRecords::EfCsimMdnLoaded::OnRecordLoaded(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // byte[] data = (byte[]) ar.result;
    // if (DBG) log("CSIM_MDN=" + IccUtils.bytesToHexString(data));
    // // Refer to C.S0065 5.2.35
    // int mdnDigitsNum = 0x0F & data[0];
    // mMdn = IccUtils.cdmaBcdToString(data, 1, mdnDigitsNum);
    // if (DBG) log("CSIM MDN=" + mMdn);
    assert(0);
    return NOERROR;
}

//=====================================================================
//                    RuimRecords::EfCsimImsimLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfCsimImsimLoaded, Object, IIccRecordLoaded);

ECode RuimRecords::EfCsimImsimLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "EF_CSIM_IMSIM";
    assert(0);
    return NOERROR;
}

ECode RuimRecords::EfCsimImsimLoaded::OnRecordLoaded(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // byte[] data = (byte[]) ar.result;
    //
    // if (data == null || data.length < 10) {
    //     log("Invalid IMSI from EF_CSIM_IMSIM " + IccUtils.bytesToHexString(data));
    //     mImsi = null;
    //     mMin = null;
    //     return;
    // }
    // if (DBG) log("CSIM_IMSIM=" + IccUtils.bytesToHexString(data));
    //
    // // C.S0065 section 5.2.2 for IMSI_M encoding
    // // C.S0005 section 2.3.1 for MIN encoding in IMSI_M.
    // boolean provisioned = ((data[7] & 0x80) == 0x80);
    //
    // if (provisioned) {
    //     mImsi = decodeImsi(data);
    //     if (null != mImsi) {
    //         mMin = mImsi.substring(5, 15);
    //     }
    //     log("IMSI: " + mImsi.substring(0, 5) + "xxxxxxxxx");
    //
    // } else {
    //     if (DBG) log("IMSI not provisioned in card");
    // }
    //
    // //Update MccTable with the retrieved IMSI
    // String operatorNumeric = getOperatorNumeric();
    // if (operatorNumeric != null) {
    //     if(operatorNumeric.length() <= 6){
    //         MccTable.updateMccMncConfiguration(mContext, operatorNumeric, false);
    //     }
    // }
    //
    // mImsiReadyRegistrants.notifyRegistrants();
    assert(0);
    return NOERROR;
}

//=====================================================================
//                  RuimRecords::EfCsimCdmaHomeLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfCsimCdmaHomeLoaded, Object, IIccRecordLoaded);

ECode RuimRecords::EfCsimCdmaHomeLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "EF_CSIM_CDMAHOME";
    assert(0);
    return NOERROR;
}

ECode RuimRecords::EfCsimCdmaHomeLoaded::OnRecordLoaded(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // // Per C.S0065 section 5.2.8
    // ArrayList<byte[]> dataList = (ArrayList<byte[]>) ar.result;
    // if (DBG) log("CSIM_CDMAHOME data size=" + dataList.size());
    // if (dataList.isEmpty()) {
    //     return;
    // }
    // StringBuilder sidBuf = new StringBuilder();
    // StringBuilder nidBuf = new StringBuilder();
    //
    // for (byte[] data : dataList) {
    //     if (data.length == 5) {
    //         int sid = ((data[1] & 0xFF) << 8) | (data[0] & 0xFF);
    //         int nid = ((data[3] & 0xFF) << 8) | (data[2] & 0xFF);
    //         sidBuf.append(sid).append(',');
    //         nidBuf.append(nid).append(',');
    //     }
    // }
    // // remove trailing ","
    // sidBuf.setLength(sidBuf.length()-1);
    // nidBuf.setLength(nidBuf.length()-1);
    //
    // mHomeSystemId = sidBuf.toString();
    // mHomeNetworkId = nidBuf.toString();
    assert(0);
    return NOERROR;
}

//=====================================================================
//                    RuimRecords::EfCsimEprlLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfCsimEprlLoaded, Object, IIccRecordLoaded);

ECode RuimRecords::EfCsimEprlLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "EF_CSIM_EPRL";
    assert(0);
    return NOERROR;
}

ECode RuimRecords::EfCsimEprlLoaded::OnRecordLoaded(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // onGetCSimEprlDone(ar);
    assert(0);
    return NOERROR;
}

//=====================================================================
//                    RuimRecords::EfCsimModelLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfCsimModelLoaded, Object, IIccRecordLoaded);

ECode RuimRecords::EfCsimModelLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    // ==================before translated======================
    // return "EF_CSIM_MODEL";
    assert(0);
    return NOERROR;
}

ECode RuimRecords::EfCsimModelLoaded::OnRecordLoaded(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // byte[] data = (byte[]) ar.result;
    // if (DBG)
    //     log("EF_CSIM_MODEL=" + IccUtils.bytesToHexString(data));
    assert(0);
    return NOERROR;
}

//=====================================================================
//                    RuimRecords::EfRuimModelLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfRuimModelLoaded, Object, IIccRecordLoaded);

ECode RuimRecords::EfRuimModelLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    // ==================before translated======================
    // return "EF_RUIM_MODEL";
    assert(0);
    return NOERROR;
}

ECode RuimRecords::EfRuimModelLoaded::OnRecordLoaded(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // byte[] data = (byte[]) ar.result;
    // if (DBG)
    //     log("EF_RUIM_MODEL=" + IccUtils.bytesToHexString(data));
    assert(0);
    return NOERROR;
}

//=====================================================================
//                     RuimRecords::EfRuimIdLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfRuimIdLoaded, Object, IIccRecordLoaded);

ECode RuimRecords::EfRuimIdLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    // ==================before translated======================
    // return "EF_RUIM_ID";
    assert(0);
    return NOERROR;
}

ECode RuimRecords::EfRuimIdLoaded::OnRecordLoaded(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // // The first byte represent the num bytes of valid data for RUIM ID data.
    // // It is valid RUIM ID data From the second byte to num+1 byte.
    // // And the second byte is the lowest-order byte, the num+1 byte is highest-order
    // byte[] data = (byte[]) ar.result;
    // if (DBG)
    //     log("RuimId Data=" + IccUtils.bytesToHexString(data));
    // if (data == null) {
    //     return;
    // }
    // int numOfBytes = data[0];
    // if (numOfBytes < NUM_BYTES_RUIM_ID) {
    //     byte[] decodeData = new byte[numOfBytes];
    //     for (int i = 0; i < numOfBytes; i++) {
    //         decodeData[i] = data[numOfBytes - i];
    //     }
    //     String ruimId = IccUtils.bytesToHexString(decodeData);
    //     if (DBG)
    //         log("RUIM_ID=" + ruimId);
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                             RuimRecords
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords, IccRecords, IRuimRecords);

const String RuimRecords::LOGTAG("RuimRecords");
const Int32 RuimRecords::EF_MODEL_FILE_SIZE;
const Int32 RuimRecords::MODEL_INFORMATION_SIZE;
const Int32 RuimRecords::MANUFACTURER_NAME_SIZE;
const Int32 RuimRecords::SOFTWARE_VERSION_INFORMATION_SIZE;
const Int32 RuimRecords::LANGUAGE_INDICATOR_ENGLISH;
const Int32 RuimRecords::CSIM_IMSI_MNC_LENGTH;
const Int32 RuimRecords::EVENT_GET_DEVICE_IDENTITY_DONE;
const Int32 RuimRecords::EVENT_GET_ICCID_DONE;
const Int32 RuimRecords::EVENT_GET_RUIM_CST_DONE;
const Int32 RuimRecords::EVENT_GET_CDMA_SUBSCRIPTION_DONE;
const Int32 RuimRecords::EVENT_UPDATE_DONE;
const Int32 RuimRecords::EVENT_SET_MODEL_DONE;
const Int32 RuimRecords::EVENT_GET_SST_DONE;
const Int32 RuimRecords::EVENT_GET_ALL_SMS_DONE;
const Int32 RuimRecords::EVENT_MARK_SMS_READ_DONE;
const Int32 RuimRecords::EVENT_SMS_ON_RUIM;
const Int32 RuimRecords::EVENT_GET_SMS_DONE;
const Int32 RuimRecords::NUM_BYTES_RUIM_ID;

RuimRecords::RuimRecords()
{
}

ECode RuimRecords::constructor(
    /* [in] */ IUiccCardApplication* app,
    /* [in] */ IContext* c,
    /* [in] */ ICommandsInterface* ci)
{
    // ==================before translated======================
    // super(app, c, ci);
    //
    // mAdnCache = new AdnRecordCache(mFh);
    //
    // mRecordsRequested = false;  // No load request is made till SIM ready
    //
    // // recordsToLoad is set to 0 because no requests are made yet
    // mRecordsToLoad = 0;
    //
    // // NOTE the EVENT_SMS_ON_RUIM is not registered
    //
    // // Start off by setting empty state
    // resetRecords();
    //
    // mParentApp.registerForReady(this, EVENT_APP_READY, null);
    // if (DBG) log("RuimRecords X ctor this=" + this);
    return NOERROR;
}

ECode RuimRecords::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "RuimRecords: " + super.toString()
    //         + " m_ota_commited" + mOtaCommited
    //         + " mMyMobileNumber=" + "xxxx"
    //         + " mMin2Min1=" + mMin2Min1
    //         + " mPrlVersion=" + mPrlVersion
    //         + " mEFpl=" + mEFpl
    //         + " mEFli=" + mEFli
    //         + " mCsimSpnDisplayCondition=" + mCsimSpnDisplayCondition
    //         + " mMdn=" + mMdn
    //         + " mMin=" + mMin
    //         + " mHomeSystemId=" + mHomeSystemId
    //         + " mHomeNetworkId=" + mHomeNetworkId;
    assert(0);
    return NOERROR;
}

ECode RuimRecords::Dispose()
{
    // ==================before translated======================
    // if (DBG) log("Disposing RuimRecords " + this);
    // //Unregister for all events
    // mParentApp.unregisterForReady(this);
    // resetRecords();
    // super.dispose();
    assert(0);
    return NOERROR;
}

ECode RuimRecords::GetIMSI(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mImsi;
    assert(0);
    return NOERROR;
}

ECode RuimRecords::GetMdnNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMyMobileNumber;
    assert(0);
    return NOERROR;
}

ECode RuimRecords::GetCdmaMin(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMin2Min1;
    assert(0);
    return NOERROR;
}

ECode RuimRecords::GetPrlVersion(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPrlVersion;
    assert(0);
    return NOERROR;
}

ECode RuimRecords::SetVoiceMailNumber(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& voiceNumber,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // // In CDMA this is Operator/OEM dependent
    // AsyncResult.forMessage((onComplete)).exception =
    //         new IccException("setVoiceMailNumber not implemented");
    // onComplete.sendToTarget();
    // loge("method setVoiceMailNumber is not implemented");
    assert(0);
    return NOERROR;
}

ECode RuimRecords::OnRefresh(
    /* [in] */ Boolean fileChanged,
    /* [in] */ ArrayOf<Int32>* fileList)
{
    // ==================before translated======================
    // if (fileChanged) {
    //     // A future optimization would be to inspect fileList and
    //     // only reload those files that we care about.  For now,
    //     // just re-fetch all RUIM records that we cache.
    //     fetchRuimRecords();
    // }
    assert(0);
    return NOERROR;
}

ECode RuimRecords::GetOperatorNumeric(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mImsi == null) {
    //     return null;
    // }
    //
    // if (SystemProperties.getBoolean("ro.telephony.get_imsi_from_sim", false)) {
    //     String imsi = mParentApp.getUICCConfig().getImsi();
    //     int mnclength = mParentApp.getUICCConfig().getMncLength();
    //
    //     // If we are LTE over CDMA (Verizon), then pull the correct info from SIMRecords
    //     if (imsi != null) {
    //         log("Overriding with Operator Numeric: " + imsi.substring(0, 3 + mnclength));
    //         return imsi.substring(0, 3 + mnclength);
    //     }
    // }
    //
    //
    // if (mMncLength != UNINITIALIZED && mMncLength != UNKNOWN) {
    //     // Length = length of MCC + length of MNC
    //     // length of mcc = 3 (3GPP2 C.S0005 - Section 2.3)
    //     return mImsi.substring(0, 3 + mMncLength);
    // }
    //
    // // Guess the MNC length based on the MCC if we don't
    // // have a valid value in ef[ad]
    //
    // int mcc = Integer.parseInt(mImsi.substring(0,3));
    // return mImsi.substring(0, 3 + CSIM_IMSI_MNC_LENGTH);
    assert(0);
    return NOERROR;
}

ECode RuimRecords::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // AsyncResult ar;
    //
    // byte data[];
    //
    // boolean isRecordLoadResponse = false;
    //
    // if (mDestroyed.get()) {
    //     loge("Received message " + msg +
    //             "[" + msg.what + "] while being destroyed. Ignoring.");
    //     return;
    // }
    //
    // try { switch (msg.what) {
    //     case EVENT_APP_READY:
    //         onReady();
    //         break;
    //
    //     case EVENT_GET_DEVICE_IDENTITY_DONE:
    //         log("Event EVENT_GET_DEVICE_IDENTITY_DONE Received");
    //     break;
    //
    //     case EVENT_GET_CDMA_SUBSCRIPTION_DONE:
    //         ar = (AsyncResult)msg.obj;
    //         String localTemp[] = (String[])ar.result;
    //         if (ar.exception != null) {
    //             break;
    //         }
    //
    //         mMyMobileNumber = localTemp[0];
    //         mMin2Min1 = localTemp[3];
    //         mPrlVersion = localTemp[4];
    //
    //         log("MDN: " + mMyMobileNumber + " MIN: " + mMin2Min1);
    //
    //     break;
    //
    //     case EVENT_GET_ICCID_DONE:
    //         isRecordLoadResponse = true;
    //
    //         ar = (AsyncResult)msg.obj;
    //         data = (byte[])ar.result;
    //
    //         if (ar.exception != null) {
    //             break;
    //         }
    //
    //         mIccId = IccUtils.bcdToString(data, 0, data.length);
    //
    //         log("iccid: " + mIccId);
    //
    //     break;
    //
    //     case EVENT_UPDATE_DONE:
    //         ar = (AsyncResult)msg.obj;
    //         if (ar.exception != null) {
    //             Rlog.i(LOGTAG, "RuimRecords update failed", ar.exception);
    //         }
    //     break;
    //
    //     case EVENT_GET_ALL_SMS_DONE:
    //     case EVENT_MARK_SMS_READ_DONE:
    //     case EVENT_SMS_ON_RUIM:
    //     case EVENT_GET_SMS_DONE:
    //         Rlog.w(LOGTAG, "Event not supported: " + msg.what);
    //         break;
    //
    //     // TODO: probably EF_CST should be read instead
    //     case EVENT_GET_SST_DONE:
    //         log("Event EVENT_GET_SST_DONE Received");
    //     break;
    //     case EVENT_SET_MODEL_DONE:
    //         ar = (AsyncResult) msg.obj;
    //         if (ar.exception != null) {
    //             loge("Set EF Model failed" + ar.exception);
    //         }
    //         log("EVENT_SET_MODEL_DONE");
    //         break;
    //     case EVENT_GET_RUIM_CST_DONE:
    //         // CSIM flags
    //         boolean omhEnabled = false;
    //         boolean mmsicpEnabled = false;
    //         isRecordLoadResponse = true;
    //         ar = (AsyncResult) msg.obj;
    //         if (ar != null && ar.exception == null) {
    //             data = (byte[]) ar.result;
    //             log("EF CST data: " + IccUtils.bytesToHexString(data));
    //             if (data == null) {
    //                 break;
    //             }
    //
    //             // For CSIM card
    //             if (mParentApp != null && mParentApp.getType() == AppType.APPTYPE_CSIM) {
    //                 // Service n35, Messaging and 3GPD Extensions
    //                 if (data.length > 4) {
    //                     omhEnabled = (0x04 == (0x04 & data[4])) ? true : false;
    //                     if (omhEnabled) {
    //                         // Service n19, Multimedia Messaging Service (MMS)
    //                         mmsicpEnabled = (0x04 == (0x04 & data[2])) ? true : false;
    //                     }
    //                 } else {
    //                     loge("CSIM EF CST data length = " + data.length);
    //                 }
    //             } else {
    //                 if (data.length > 3) {
    //                     omhEnabled = (0x30 == (0x30 & data[3])) ? true : false;
    //                     // Judge the read MMSICP flag is enabled on CST or not
    //                     if (omhEnabled && data.length > 10) {
    //                         mmsicpEnabled = (0x0C == (0x0C & data[10])) ? true : false;
    //                     } else {
    //                         loge("OMH EF CST data length = " + data.length);
    //                     }
    //                 } else {
    //                     loge("OMH EF CST data length = " + data.length);
    //                 }
    //             }
    //             log("mms icp enabled =" + mmsicpEnabled + " omhEnabled " + omhEnabled);
    //             SystemProperties.set(TelephonyProperties.PROPERTY_RUIM_OMH_CARD,
    //                     omhEnabled ? "true" : "false");
    //         }
    //         // We already know if it is an OMH card in this point
    //         fetchOMHCardRecords(omhEnabled);
    //         break;
    //     default:
    //         super.handleMessage(msg);   // IccRecords handles generic record load responses
    //
    // }}catch (RuntimeException exc) {
    //     // I don't want these exceptions to be fatal
    //     Rlog.w(LOGTAG, "Exception parsing RUIM record", exc);
    // } finally {
    //     // Count up record load responses even if they are fails
    //     if (isRecordLoadResponse) {
    //         onRecordLoaded();
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode RuimRecords::OnReady()
{
    // ==================before translated======================
    // fetchRuimRecords();
    //
    // mCi.getCDMASubscription(obtainMessage(EVENT_GET_CDMA_SUBSCRIPTION_DONE));
    assert(0);
    return NOERROR;
}

ECode RuimRecords::RecordsRequired()
{
    // ==================before translated======================
    // if (DBG) log("recordsRequired");
    // mRecordsRequired = true;
    //
    // // trigger to retrieve all records
    // fetchRuimRecords();
    assert(0);
    return NOERROR;
}

ECode RuimRecords::GetDisplayRule(
    /* [in] */ const String& plmnNumeric,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // int rule = 0;
    //
    // if ((mContext != null) && mContext.getResources().getBoolean(
    //     com.android.internal.R.bool.def_telephony_spn_spec_enabled)) {
    //     // Always display the SPN only from RUIM
    //     rule = SPN_RULE_SHOW_SPN;
    // } else if (mParentApp != null && mParentApp.getUiccCard() != null &&
    //         mParentApp.getUiccCard().getOperatorBrandOverride() != null) {
    //     // use operator brand override
    //     rule = SPN_RULE_SHOW_PLMN;
    // } else if (TextUtils.isEmpty(getServiceProviderName())) {
    //     // EF_SPN content not found on this RUIM, or not yet loaded
    //     rule = SPN_RULE_SHOW_PLMN;
    // } else if (isOnMatchingPlmn(plmnNumeric)) {
    //     // on home network
    //     if (mCsimSpnDisplayCondition && !TextUtils.isEmpty(getServiceProviderName())
    //             && (!SystemProperties.getBoolean("ro.cdma.force_plmn_lookup", false))) {
    //         // check CSIM SPN Display Condition (applicable on home network),
    //         // but only if SPN was found on this RUIM
    //         // if force plmn lookup then plmn should be used
    //         rule = SPN_RULE_SHOW_SPN;
    //     } else {
    //         // CSIM SPN Display does not require a SPN display, or SPN not found on RUIM,
    //         // then revert to currently registered network
    //         rule = SPN_RULE_SHOW_PLMN;
    //     }
    // } else {
    //     // roaming, use the currently registered network
    //     rule = SPN_RULE_SHOW_PLMN;
    // }
    //
    // return rule;
    assert(0);
    return NOERROR;
}

ECode RuimRecords::IsProvisioned(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // If UICC card has CSIM app, look for MDN and MIN field
    // // to determine if the SIM is provisioned.  Otherwise,
    // // consider the SIM is provisioned. (for case of ordinal
    // // USIM only UICC.)
    // // If PROPERTY_TEST_CSIM is defined, bypess provision check
    // // and consider the SIM is provisioned.
    // if (SystemProperties.getBoolean(PROPERTY_TEST_CSIM, false)) {
    //     return true;
    // }
    //
    // if (mParentApp == null) {
    //     return false;
    // }
    //
    // if (mParentApp.getType() == AppType.APPTYPE_CSIM &&
    //     ((mMdn == null) || (mMin == null))) {
    //     return false;
    // }
    // return true;
    assert(0);
    return NOERROR;
}

ECode RuimRecords::SetVoiceMessageWaiting(
    /* [in] */ Int32 line,
    /* [in] */ Int32 countWaiting)
{
    // ==================before translated======================
    // // Will be used in future to store voice mail count in UIM
    // // C.S0023-D_v1.0 does not have a file id in UIM for MWI
    // log("RuimRecords:setVoiceMessageWaiting - NOP for CDMA");
    assert(0);
    return NOERROR;
}

ECode RuimRecords::GetVoiceMessageCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Will be used in future to retrieve voice mail count for UIM
    // // C.S0023-D_v1.0 does not have a file id in UIM for MWI
    // log("RuimRecords:getVoiceMessageCount - NOP for CDMA");
    // return 0;
    assert(0);
    return NOERROR;
}

ECode RuimRecords::GetMdn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMdn;
    assert(0);
    return NOERROR;
}

ECode RuimRecords::GetMin(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMin;
    assert(0);
    return NOERROR;
}

ECode RuimRecords::GetSid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mHomeSystemId;
    assert(0);
    return NOERROR;
}

ECode RuimRecords::GetNid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mHomeNetworkId;
    assert(0);
    return NOERROR;
}

ECode RuimRecords::GetCsimSpnDisplayCondition(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCsimSpnDisplayCondition;
    assert(0);
    return NOERROR;
}

ECode RuimRecords::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    // ==================before translated======================
    // pw.println("RuimRecords: " + this);
    // pw.println(" extends:");
    // super.dump(fd, pw, args);
    // pw.println(" mOtaCommited=" + mOtaCommited);
    // pw.println(" mMyMobileNumber=" + mMyMobileNumber);
    // pw.println(" mMin2Min1=" + mMin2Min1);
    // pw.println(" mPrlVersion=" + mPrlVersion);
    // pw.println(" mEFpl[]=" + Arrays.toString(mEFpl));
    // pw.println(" mEFli[]=" + Arrays.toString(mEFli));
    // pw.println(" mCsimSpnDisplayCondition=" + mCsimSpnDisplayCondition);
    // pw.println(" mMdn=" + mMdn);
    // pw.println(" mMin=" + mMin);
    // pw.println(" mHomeSystemId=" + mHomeSystemId);
    // pw.println(" mHomeNetworkId=" + mHomeNetworkId);
    // pw.flush();
    assert(0);
    return NOERROR;
}

ECode RuimRecords::Finalize()
{
    // ==================before translated======================
    // if(DBG) log("RuimRecords finalized");
    assert(0);
    return NOERROR;
}

void RuimRecords::ResetRecords()
{
    // ==================before translated======================
    // mMncLength = UNINITIALIZED;
    // log("setting0 mMncLength" + mMncLength);
    // mIccId = null;
    //
    // mAdnCache.reset();
    //
    // setSystemProperty(PROPERTY_APN_RUIM_OPERATOR_NUMERIC, "");
    //
    // // Don't clean up PROPERTY_ICC_OPERATOR_ISO_COUNTRY and
    // // PROPERTY_ICC_OPERATOR_NUMERIC here. Since not all CDMA
    // // devices have RUIM, these properties should keep the original
    // // values, e.g. build time settings, when there is no RUIM but
    // // set new values when RUIM is available and loaded.
    //
    // // recordsRequested is set to false indicating that the SIM
    // // read requests made so far are not valid. This is set to
    // // true only when fresh set of read requests are made.
    // mRecordsRequested = false;
    assert(0);
}

ECode RuimRecords::OnRecordLoaded()
{
    // ==================before translated======================
    // // One record loaded successfully or failed, In either case
    // // we need to update the recordsToLoad count
    // mRecordsToLoad -= 1;
    // if (DBG) log("onRecordLoaded " + mRecordsToLoad + " requested: " + mRecordsRequested);
    //
    // if (mRecordsToLoad == 0 && mRecordsRequested == true) {
    //     onAllRecordsLoaded();
    // } else if (mRecordsToLoad < 0) {
    //     loge("recordsToLoad <0, programmer error suspected");
    //     mRecordsToLoad = 0;
    // }
    assert(0);
    return NOERROR;
}

ECode RuimRecords::OnAllRecordsLoaded()
{
    // ==================before translated======================
    // if (DBG) log("record load complete");
    //
    // // Further records that can be inserted are Operator/OEM dependent
    //
    // String operator = getOperatorNumeric();
    // if (!TextUtils.isEmpty(operator)) {
    //     log("onAllRecordsLoaded set 'gsm.sim.operator.numeric' to operator='" +
    //             operator + "'");
    //     setSystemProperty(PROPERTY_ICC_OPERATOR_NUMERIC, operator);
    //     setSystemProperty(PROPERTY_APN_RUIM_OPERATOR_NUMERIC, operator);
    // } else {
    //     log("onAllRecordsLoaded empty 'gsm.sim.operator.numeric' skipping");
    // }
    //
    // if (!TextUtils.isEmpty(mImsi)) {
    //     log("onAllRecordsLoaded set mcc imsi=" + mImsi);
    //     setSystemProperty(PROPERTY_ICC_OPERATOR_ISO_COUNTRY,
    //             MccTable.countryCodeForMcc(Integer.parseInt(mImsi.substring(0,3))));
    // } else {
    //     log("onAllRecordsLoaded empty imsi skipping setting mcc");
    // }
    //
    // setLocaleFromCsim();
    // mRecordsLoadedRegistrants.notifyRegistrants(
    //     new AsyncResult(null, null, null));
    assert(0);
    return NOERROR;
}

ECode RuimRecords::HandleFileUpdate(
    /* [in] */ Int32 efid)
{
    // ==================before translated======================
    // switch (efid) {
    //     case EF_ADN:
    //         log("SIM Refresh for EF_ADN");
    //         mAdnCache.reset();
    //         break;
    //     default:
    //         mAdnCache.reset();
    //         fetchRuimRecords();
    //         break;
    // }
    assert(0);
    return NOERROR;
}

ECode RuimRecords::Log(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "[RuimRecords] " + s);
    assert(0);
    return NOERROR;
}

ECode RuimRecords::Loge(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "[RuimRecords] " + s);
    assert(0);
    return NOERROR;
}

Int32 RuimRecords::DecodeImsiDigits(
    /* [in] */ Int32 digits,
    /* [in] */ Int32 length)
{
    // ==================before translated======================
    // // Per C.S0005 section 2.3.1.
    // int constant = 0;
    // for (int i = 0; i < length; i++ ) {
    //     constant = (constant * 10) + 1;
    // }
    //
    // digits += constant;
    //
    // for (int i = 0, denominator = 1; i < length; i++) {
    //     digits = ((digits / denominator) % 10 == 0) ? (digits - (10 * denominator)) : digits;
    //     denominator *= 10;
    // }
    // return digits;
    assert(0);
    return 0;
}

String RuimRecords::DecodeImsi(
    /* [in] */ ArrayOf<Byte>* data)
{
    // ==================before translated======================
    // // Retrieve the MCC and digits 11 and 12
    // int mcc_data = ((0x03 & data[9]) << 8) | (0xFF & data[8]);
    // int mcc = decodeImsiDigits(mcc_data, 3);
    // int digits_11_12_data = data[6] & 0x7f;
    // int digits_11_12 = decodeImsiDigits(digits_11_12_data, 2);
    //
    // // Retrieve 10 MIN digits
    // int first3digits = ((0x03 & data[2]) << 8) + (0xFF & data[1]);
    // int second3digits = (((0xFF & data[5]) << 8) | (0xFF & data[4])) >> 6;
    // int digit7 = 0x0F & (data[4] >> 2);
    // if (digit7 > 0x09) digit7 = 0;
    // int last3digits = ((0x03 & data[4]) << 8) | (0xFF & data[3]);
    //
    // first3digits = decodeImsiDigits(first3digits, 3);
    // second3digits = decodeImsiDigits(second3digits, 3);
    // last3digits = decodeImsiDigits(last3digits, 3);
    //
    // StringBuilder builder = new StringBuilder();
    // builder.append(String.format(Locale.US, "%03d", mcc));
    // builder.append(String.format(Locale.US, "%02d", digits_11_12));
    // builder.append(String.format(Locale.US, "%03d", first3digits));
    // builder.append(String.format(Locale.US, "%03d", second3digits));
    // builder.append(String.format(Locale.US, "%d", digit7));
    // builder.append(String.format(Locale.US, "%03d", last3digits));
    // return  builder.toString();
    assert(0);
    return String("");
}

void RuimRecords::OnGetCSimEprlDone(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // // C.S0065 section 5.2.57 for EFeprl encoding
    // // C.S0016 section 3.5.5 for PRL format.
    // byte[] data = (byte[]) ar.result;
    // if (DBG) log("CSIM_EPRL=" + IccUtils.bytesToHexString(data));
    //
    // // Only need the first 4 bytes of record
    // if (data.length > 3) {
    //     int prlId = ((data[2] & 0xFF) << 8) | (data[3] & 0xFF);
    //     mPrlVersion = Integer.toString(prlId);
    // }
    // if (DBG) log("CSIM PRL version=" + mPrlVersion);
    assert(0);
}

void RuimRecords::FetchOMHCardRecords(
    /* [in] */ Boolean isOMHCard)
{
    // ==================before translated======================
    // // OMH related events
    // if (isOMHCard) {
    //     // Write device's software version on EF6F90 on startup.
    //     setModel();
    // }
    assert(0);
}

AutoPtr< ArrayOf<String> > RuimRecords::GetAssetLanguages(
    /* [in] */ IContext* ctx)
{
    // ==================before translated======================
    // final String[] locales = ctx.getAssets().getLocales();
    // final String[] localeLangs = new String[locales.length];
    // for (int i = 0; i < locales.length; ++i) {
    //     final String localeStr = locales[i];
    //     final int separator = localeStr.indexOf('-');
    //     if (separator < 0) {
    //         localeLangs[i] = localeStr;
    //     } else {
    //         localeLangs[i] = localeStr.substring(0, separator);
    //     }
    // }
    //
    // return localeLangs;
    assert(0);
    AutoPtr< ArrayOf<String> > empty;
    return empty;
}

String RuimRecords::FindBestLanguage(
    /* [in] */ ArrayOf<Byte>* languages)
{
    // ==================before translated======================
    // final String[] assetLanguages = getAssetLanguages(mContext);
    //
    // if ((languages == null) || (assetLanguages == null)) return null;
    //
    // // Each 2-bytes consists of one language
    // for (int i = 0; (i + 1) < languages.length; i += 2) {
    //     final String lang;
    //     try {
    //         lang = new String(languages, i, 2, "ISO-8859-1");
    //     } catch(java.io.UnsupportedEncodingException e) {
    //         log("Failed to parse SIM language records");
    //         continue;
    //     }
    //
    //     for (int j = 0; j < assetLanguages.length; j++) {
    //         if (assetLanguages[j].equals(lang)) {
    //             return lang;
    //         }
    //     }
    // }
    //
    // // no match found. return null
    // return null;
    assert(0);
    return String("");
}

void RuimRecords::SetLocaleFromCsim()
{
    // ==================before translated======================
    // String prefLang = null;
    // // check EFli then EFpl
    // prefLang = findBestLanguage(mEFli);
    //
    // if (prefLang == null) {
    //     prefLang = findBestLanguage(mEFpl);
    // }
    //
    // if (prefLang != null) {
    //     // check country code from SIM
    //     String imsi = getIMSI();
    //     String country = null;
    //     if (imsi != null) {
    //         country = MccTable.countryCodeForMcc(
    //                             Integer.parseInt(imsi.substring(0,3)));
    //     }
    //     log("Setting locale to " + prefLang + "_" + country);
    //     MccTable.setSystemLocale(mContext, prefLang, country);
    // } else {
    //     log ("No suitable CSIM selected locale");
    // }
    assert(0);
}

void RuimRecords::FetchRuimRecords()
{
    // ==================before translated======================
    // boolean mESNTrackerEnabled = mContext.getResources().getBoolean(
    //         com.android.internal.R.bool.config_telephony_ESN_Tracker_enabled);
    // /* Don't read records if we don't expect
    //  * anyone to ask for them
    //  *
    //  * If we have already requested records OR
    //  * records are not required by anyone OR
    //  * the app is not ready
    //  * then bail
    //  */
    // if (mRecordsRequested || (mESNTrackerEnabled ? false : !mRecordsRequired)
    //     || AppState.APPSTATE_READY != mParentApp.getState()) {
    //     if (DBG) log("fetchRuimRecords: Abort fetching records rRecordsRequested = "
    //                     + mRecordsRequested
    //                     + " state = " + mParentApp.getState()
    //                     + " required = " + mRecordsRequired);
    //     return;
    // }
    //
    // mRecordsRequested = true;
    //
    // if (DBG) log("fetchRuimRecords " + mRecordsToLoad);
    //
    // mFh.loadEFTransparent(EF_ICCID,
    //         obtainMessage(EVENT_GET_ICCID_DONE));
    // mRecordsToLoad++;
    //
    // mFh.loadEFTransparent(EF_PL,
    //         obtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfPlLoaded()));
    // mRecordsToLoad++;
    //
    // mFh.loadEFTransparent(EF_CSIM_LI,
    //         obtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfCsimLiLoaded()));
    // mRecordsToLoad++;
    //
    // mFh.loadEFTransparent(EF_CSIM_SPN,
    //         obtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfCsimSpnLoaded()));
    // mRecordsToLoad++;
    //
    // mFh.loadEFLinearFixed(EF_CSIM_MDN, 1,
    //         obtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfCsimMdnLoaded()));
    // mRecordsToLoad++;
    //
    // mFh.loadEFTransparent(EF_CSIM_IMSIM,
    //         obtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfCsimImsimLoaded()));
    // mRecordsToLoad++;
    //
    // mFh.loadEFLinearFixedAll(EF_CSIM_CDMAHOME,
    //         obtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfCsimCdmaHomeLoaded()));
    // mRecordsToLoad++;
    //
    // if (mESNTrackerEnabled) {
    //     mFh.loadEFTransparent(EF_CSIM_MODEL,
    //             obtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfCsimModelLoaded()));
    //     mRecordsToLoad++;
    //
    //     mFh.loadEFTransparent(EF_MODEL,
    //             obtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfRuimModelLoaded()));
    //     mRecordsToLoad++;
    //
    //     // We need to read CST table first to see if it is a CT/OMH/Legacy card
    //     mFh.loadEFTransparent(IccConstants.EF_CST,
    //             obtainMessage(EVENT_GET_RUIM_CST_DONE));
    //     mRecordsToLoad++;
    //
    //     mFh.loadEFTransparent(EF_RUIM_ID,
    //             obtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfRuimIdLoaded()));
    //     mRecordsToLoad++;
    // }
    //
    // // Entire PRL could be huge. We are only interested in
    // // the first 4 bytes of the record.
    // mFh.loadEFTransparent(EF_CSIM_EPRL, 4,
    //         obtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfCsimEprlLoaded()));
    // mRecordsToLoad++;
    //
    // mFh.getEFLinearRecordSize(EF_SMS, obtainMessage(EVENT_GET_SMS_RECORD_SIZE_DONE));
    //
    // if (DBG) log("fetchRuimRecords " + mRecordsToLoad + " requested: " + mRecordsRequested);
    // // Further records that can be inserted are Operator/OEM dependent
    assert(0);
}

void RuimRecords::SetModel()
{
    // ==================before translated======================
    // byte[] data = new byte[EF_MODEL_FILE_SIZE];
    // byte[] model = null;
    // byte[] manufacturer = null;
    // byte[] softwareVersion = null;
    //
    // for (int i = 0; i < data.length; i++) {
    //     data[i] = (byte) 0xff;
    // }
    // // EFmodel will be written into UTF-8 characters
    // try {
    //     model = Build.MODEL.getBytes("UTF-8");
    //     manufacturer = Build.MANUFACTURER.getBytes("UTF-8");
    //     String str = SystemProperties.get("persist.product.sw.version", Build.DISPLAY);
    //     softwareVersion = str.getBytes("UTF-8");
    // } catch (UnsupportedEncodingException e) {
    //     loge("BearerData encode failed: " + e);
    // }
    // data[0] = UserData.ENCODING_OCTET;
    // data[1] = LANGUAGE_INDICATOR_ENGLISH;
    //
    // // Restrict model length to prevent ArrayIndexOutOfBoundsException
    // int offset = 2;
    // int modelLen = (model.length > MODEL_INFORMATION_SIZE)
    //         ? MODEL_INFORMATION_SIZE : model.length;
    // System.arraycopy(model, 0, data, offset, modelLen);
    // offset += MODEL_INFORMATION_SIZE;
    // // Restrict manufacture length to prevent ArrayIndexOutOfBoundsException
    // int manufactureLen = (manufacturer.length > MANUFACTURER_NAME_SIZE)
    //         ? MANUFACTURER_NAME_SIZE : manufacturer.length;
    // System.arraycopy(manufacturer, 0, data, offset, manufactureLen);
    // // We need to restrict versionLength in case it is too long which maybe lead to
    // // arrayoutofindex exception
    // offset += MANUFACTURER_NAME_SIZE;
    // int versionLength = (softwareVersion.length > SOFTWARE_VERSION_INFORMATION_SIZE)
    //         ? SOFTWARE_VERSION_INFORMATION_SIZE : softwareVersion.length;
    // System.arraycopy(softwareVersion, 0, data, offset, versionLength);
    //
    // log("model: " + IccUtils.bytesToHexString(model) + "manufacturer: "
    //         + IccUtils.bytesToHexString(manufacturer) + "softwareVersion: "
    //         + IccUtils.bytesToHexString(softwareVersion));
    // log("EF model write data : " + IccUtils.bytesToHexString(data) + " version length="
    //         + versionLength);
    //
    // if (mParentApp != null && mParentApp.getType() == AppType.APPTYPE_CSIM) {
    //     log("CSIM card type, set csim model");
    //     mFh.updateEFTransparent(IccConstants.EF_CSIM_MODEL, data,
    //             obtainMessage(EVENT_SET_MODEL_DONE, IccConstants.EF_CSIM_MODEL));
    // } else {
    //     mFh.updateEFTransparent(IccConstants.EF_MODEL, data,
    //             obtainMessage(EVENT_SET_MODEL_DONE, IccConstants.EF_MODEL));
    // }
    assert(0);
}

Boolean RuimRecords::IsOnMatchingPlmn(
    /* [in] */ const String& plmnNumeric)
{
    // ==================before translated======================
    // if (plmnNumeric == null) return false;
    // if (plmnNumeric.equals(getOperatorNumeric())) {
    //     return true;
    // }
    // return false;
    assert(0);
    return FALSE;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
