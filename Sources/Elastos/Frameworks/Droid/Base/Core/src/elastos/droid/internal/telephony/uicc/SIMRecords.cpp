#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/SIMRecords.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                        SIMRecords::EfPlLoaded
//=====================================================================
CAR_INTERFACE_IMPL(SIMRecords::EfPlLoaded, Object, IIccRecordLoaded);

ECode SIMRecords::EfPlLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    // ==================before translated======================
    // return "EF_PL";
    assert(0);
    return NOERROR;
}

ECode SIMRecords::EfPlLoaded::OnRecordLoaded(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // mEfPl = (byte[]) ar.result;
    // if (DBG) log("EF_PL=" + IccUtils.bytesToHexString(mEfPl));
    assert(0);
    return NOERROR;
}

//=====================================================================
//                      SIMRecords::EfUsimLiLoaded
//=====================================================================
CAR_INTERFACE_IMPL(SIMRecords::EfUsimLiLoaded, Object, IIccRecordLoaded);

ECode SIMRecords::EfUsimLiLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    // ==================before translated======================
    // return "EF_LI";
    assert(0);
    return NOERROR;
}

ECode SIMRecords::EfUsimLiLoaded::OnRecordLoaded(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // mEfLi = (byte[]) ar.result;
    // if (DBG) log("EF_LI=" + IccUtils.bytesToHexString(mEfLi));
    assert(0);
    return NOERROR;
}

//=====================================================================
//                              SIMRecords
//=====================================================================
const Int32 SIMRecords::TAG_SPDI;
const Int32 SIMRecords::TAG_SPDI_PLMN_LIST;
const Int32 SIMRecords::TAG_FULL_NETWORK_NAME;
const Int32 SIMRecords::TAG_SHORT_NETWORK_NAME;
const Int32 SIMRecords::CFF_UNCONDITIONAL_ACTIVE;
const Int32 SIMRecords::CFF_UNCONDITIONAL_DEACTIVE;
const Int32 SIMRecords::CFF_LINE1_MASK;
const Int32 SIMRecords::CFF_LINE1_RESET;
const String SIMRecords::LOGTAG("SIMRecords");
const Int32 SIMRecords::EVENT_GET_AD_DONE;
const Int32 SIMRecords::EVENT_GET_MSISDN_DONE;
const Int32 SIMRecords::EVENT_GET_SST_DONE;
const Boolean SIMRecords::CRASH_RIL = FALSE;
const Int32 SIMRecords::CPHS_SST_MBN_MASK;
const Int32 SIMRecords::CPHS_SST_MBN_ENABLED;
const Int32 SIMRecords::CFIS_BCD_NUMBER_LENGTH_OFFSET;
const Int32 SIMRecords::CFIS_TON_NPI_OFFSET;
const Int32 SIMRecords::CFIS_ADN_CAPABILITY_ID_OFFSET;
const Int32 SIMRecords::CFIS_ADN_EXTENSION_ID_OFFSET;
const Int32 SIMRecords::EVENT_GET_IMSI_DONE;
const Int32 SIMRecords::EVENT_GET_ICCID_DONE;
const Int32 SIMRecords::EVENT_GET_MBI_DONE;
const Int32 SIMRecords::EVENT_GET_MBDN_DONE;
const Int32 SIMRecords::EVENT_GET_MWIS_DONE;
const Int32 SIMRecords::EVENT_GET_VOICE_MAIL_INDICATOR_CPHS_DONE;
const Int32 SIMRecords::EVENT_GET_CPHS_MAILBOX_DONE;
const Int32 SIMRecords::EVENT_GET_SPN_DONE;
const Int32 SIMRecords::EVENT_GET_SPDI_DONE;
const Int32 SIMRecords::EVENT_UPDATE_DONE;
const Int32 SIMRecords::EVENT_GET_PNN_DONE;
const Int32 SIMRecords::EVENT_GET_ALL_SMS_DONE;
const Int32 SIMRecords::EVENT_MARK_SMS_READ_DONE;
const Int32 SIMRecords::EVENT_SET_MBDN_DONE;
const Int32 SIMRecords::EVENT_GET_SMS_DONE;
const Int32 SIMRecords::EVENT_GET_CFF_DONE;
const Int32 SIMRecords::EVENT_SET_CPHS_MAILBOX_DONE;
const Int32 SIMRecords::EVENT_GET_INFO_CPHS_DONE;
const Int32 SIMRecords::EVENT_GET_CFIS_DONE;
const Int32 SIMRecords::EVENT_GET_CSP_CPHS_DONE;
const Int32 SIMRecords::EVENT_GET_GID1_DONE;
const Int32 SIMRecords::EVENT_APP_LOCKED;
AutoPtr<ArrayOf<String> > SIMRecords::MCCMNC_CODES_HAVING_3DIGITS_MNC = SIMRecords::MiddleInitMccmncCodesHaving3digitsMnc();

SIMRecords::SIMRecords()
{
}

ECode SIMRecords::constructor(
    /* [in] */ IUiccCardApplication* app,
    /* [in] */ IContext* c,
    /* [in] */ ICommandsInterface* ci)
{
    // ==================before translated======================
    // super(app, c, ci);
    //
    // mAdnCache = new AdnRecordCache(mFh);
    //
    // mVmConfig = new VoiceMailConstants();
    // mSpnOverride = new SpnOverride();
    //
    // mRecordsRequested = false;  // No load request is made till SIM ready
    //
    // // recordsToLoad is set to 0 because no requests are made yet
    // mRecordsToLoad = 0;
    //
    // // Start off by setting empty state
    // resetRecords();
    // mParentApp.registerForReady(this, EVENT_APP_READY, null);
    // mParentApp.registerForLocked(this, EVENT_APP_LOCKED, null);
    // if (DBG) log("SIMRecords X ctor this=" + this);
    return NOERROR;
}

ECode SIMRecords::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "SimRecords: " + super.toString()
    //         + " mVmConfig" + mVmConfig
    //         + " mSpnOverride=" + "mSpnOverride"
    //         + " callForwardingEnabled=" + mCallForwardingEnabled
    //         + " spnState=" + mSpnState
    //         + " mCphsInfo=" + mCphsInfo
    //         + " mCspPlmnEnabled=" + mCspPlmnEnabled
    //         + " efMWIS=" + mEfMWIS
    //         + " efCPHS_MWI=" + mEfCPHS_MWI
    //         + " mEfCff=" + mEfCff
    //         + " mEfCfis=" + mEfCfis
    //         + " getOperatorNumeric=" + getOperatorNumeric();
    assert(0);
    return NOERROR;
}

ECode SIMRecords::Dispose()
{
    // ==================before translated======================
    // if (DBG) log("Disposing SIMRecords this=" + this);
    // //Unregister for all events
    // mParentApp.unregisterForReady(this);
    // mParentApp.unregisterForLocked(this);
    // resetRecords();
    // super.dispose();
    assert(0);
    return NOERROR;
}

ECode SIMRecords::GetIMSI(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mImsi;
    assert(0);
    return NOERROR;
}

ECode SIMRecords::GetMsisdnNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMsisdn;
    assert(0);
    return NOERROR;
}

ECode SIMRecords::GetGid1(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mGid1;
    assert(0);
    return NOERROR;
}

ECode SIMRecords::GetUsimServiceTable(
    /* [out] */ IUsimServiceTable** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mUsimServiceTable;
    assert(0);
    return NOERROR;
}

ECode SIMRecords::SetMsisdnNumber(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    //
    // mMsisdn = number;
    // mMsisdnTag = alphaTag;
    //
    // if(DBG) log("Set MSISDN: " + mMsisdnTag + " " + /*mMsisdn*/ "xxxxxxx");
    //
    // AdnRecord adn = new AdnRecord(mMsisdnTag, mMsisdn);
    //
    // new AdnRecordLoader(mFh).updateEF(adn, EF_MSISDN, getExtFromEf(EF_MSISDN), 1, null,
    //         obtainMessage(EVENT_SET_MSISDN_DONE, onComplete));
    assert(0);
    return NOERROR;
}

ECode SIMRecords::GetMsisdnAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMsisdnTag;
    assert(0);
    return NOERROR;
}

ECode SIMRecords::GetVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mVoiceMailNum;
    assert(0);
    return NOERROR;
}

ECode SIMRecords::SetVoiceMailNumber(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& voiceNumber,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // if (mIsVoiceMailFixed) {
    //     AsyncResult.forMessage((onComplete)).exception =
    //             new IccVmFixedException("Voicemail number is fixed by operator");
    //     onComplete.sendToTarget();
    //     return;
    // }
    //
    // mNewVoiceMailNum = voiceNumber;
    // mNewVoiceMailTag = alphaTag;
    //
    // AdnRecord adn = new AdnRecord(mNewVoiceMailTag, mNewVoiceMailNum);
    //
    // if (mMailboxIndex != 0 && mMailboxIndex != 0xff) {
    //
    //     new AdnRecordLoader(mFh).updateEF(adn, EF_MBDN, EF_EXT6,
    //             mMailboxIndex, null,
    //             obtainMessage(EVENT_SET_MBDN_DONE, onComplete));
    //
    // } else if (isCphsMailboxEnabled()) {
    //
    //     new AdnRecordLoader(mFh).updateEF(adn, EF_MAILBOX_CPHS,
    //             EF_EXT1, 1, null,
    //             obtainMessage(EVENT_SET_CPHS_MAILBOX_DONE, onComplete));
    //
    // } else {
    //     AsyncResult.forMessage((onComplete)).exception =
    //             new IccVmNotSupportedException("Update SIM voice mailbox error");
    //     onComplete.sendToTarget();
    // }
    assert(0);
    return NOERROR;
}

ECode SIMRecords::GetVoiceMailAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mVoiceMailTag;
    assert(0);
    return NOERROR;
}

ECode SIMRecords::SetVoiceMessageWaiting(
    /* [in] */ Int32 line,
    /* [in] */ Int32 countWaiting)
{
    // ==================before translated======================
    // if (line != 1) {
    //     // only profile 1 is supported
    //     return;
    // }
    //
    // try {
    //     if (mEfMWIS != null) {
    //         // TS 51.011 10.3.45
    //
    //         // lsb of byte 0 is 'voicemail' status
    //         mEfMWIS[0] = (byte)((mEfMWIS[0] & 0xfe)
    //                             | (countWaiting == 0 ? 0 : 1));
    //
    //         // byte 1 is the number of voice messages waiting
    //         if (countWaiting < 0) {
    //             // The spec does not define what this should be
    //             // if we don't know the count
    //             mEfMWIS[1] = 0;
    //         } else {
    //             mEfMWIS[1] = (byte) countWaiting;
    //         }
    //
    //         mFh.updateEFLinearFixed(
    //             EF_MWIS, 1, mEfMWIS, null,
    //             obtainMessage (EVENT_UPDATE_DONE, EF_MWIS, 0));
    //     }
    //
    //     if (mEfCPHS_MWI != null) {
    //             // Refer CPHS4_2.WW6 B4.2.3
    //         mEfCPHS_MWI[0] = (byte)((mEfCPHS_MWI[0] & 0xf0)
    //                     | (countWaiting == 0 ? 0x5 : 0xa));
    //         mFh.updateEFTransparent(
    //             EF_VOICE_MAIL_INDICATOR_CPHS, mEfCPHS_MWI,
    //             obtainMessage (EVENT_UPDATE_DONE, EF_VOICE_MAIL_INDICATOR_CPHS));
    //     }
    // } catch (ArrayIndexOutOfBoundsException ex) {
    //     logw("Error saving voice mail state to SIM. Probably malformed SIM record", ex);
    // }
    assert(0);
    return NOERROR;
}

ECode SIMRecords::GetVoiceMessageCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean voiceMailWaiting = false;
    // int countVoiceMessages = -1;
    // if (mEfMWIS != null) {
    //     // Use this data if the EF[MWIS] exists and
    //     // has been loaded
    //     // Refer TS 51.011 Section 10.3.45 for the content description
    //     voiceMailWaiting = ((mEfMWIS[0] & 0x01) != 0);
    //     countVoiceMessages = mEfMWIS[1] & 0xff;
    //
    //     if (voiceMailWaiting && countVoiceMessages == 0) {
    //         // Unknown count = -1
    //         countVoiceMessages = -1;
    //     }
    //     if(DBG) log(" VoiceMessageCount from SIM MWIS = " + countVoiceMessages);
    // } else if (mEfCPHS_MWI != null) {
    //     // use voice mail count from CPHS
    //     int indicator = (int) (mEfCPHS_MWI[0] & 0xf);
    //
    //     // Refer CPHS4_2.WW6 B4.2.3
    //     if (indicator == 0xA) {
    //         // Unknown count = -1
    //         countVoiceMessages = -1;
    //     } else if (indicator == 0x5) {
    //         countVoiceMessages = 0;
    //     }
    //     if(DBG) log(" VoiceMessageCount from SIM CPHS = " + countVoiceMessages);
    // }
    // return countVoiceMessages;
    assert(0);
    return NOERROR;
}

ECode SIMRecords::IsCallForwardStatusStored(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (mEfCfis != null) || (mEfCff != null);
    assert(0);
    return NOERROR;
}

ECode SIMRecords::GetVoiceCallForwardingFlag(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCallForwardingEnabled;
    assert(0);
    return NOERROR;
}

ECode SIMRecords::SetVoiceCallForwardingFlag(
    /* [in] */ Int32 line,
    /* [in] */ Boolean enable,
    /* [in] */ const String& dialNumber)
{
    // ==================before translated======================
    //
    // if (line != 1) return; // only line 1 is supported
    //
    // mCallForwardingEnabled = enable;
    //
    // mRecordsEventsRegistrants.notifyResult(EVENT_CFI);
    //
    // try {
    //     if (validEfCfis(mEfCfis)) {
    //         // lsb is of byte 1 is voice status
    //         if (enable) {
    //             mEfCfis[1] |= 1;
    //         } else {
    //             mEfCfis[1] &= 0xfe;
    //         }
    //
    //         log("setVoiceCallForwardingFlag: enable=" + enable
    //                 + " mEfCfis=" + IccUtils.bytesToHexString(mEfCfis));
    //
    //         // Update dialNumber if not empty and CFU is enabled.
    //         // Spec reference for EF_CFIS contents, TS 51.011 section 10.3.46.
    //         if (enable && !TextUtils.isEmpty(dialNumber)) {
    //             log("EF_CFIS: updating cf number, " + dialNumber);
    //             byte[] bcdNumber = PhoneNumberUtils.numberToCalledPartyBCD(dialNumber);
    //
    //             System.arraycopy(bcdNumber, 0, mEfCfis, CFIS_TON_NPI_OFFSET, bcdNumber.length);
    //
    //             mEfCfis[CFIS_BCD_NUMBER_LENGTH_OFFSET] = (byte) (bcdNumber.length);
    //             mEfCfis[CFIS_ADN_CAPABILITY_ID_OFFSET] = (byte) 0xFF;
    //             mEfCfis[CFIS_ADN_EXTENSION_ID_OFFSET] = (byte) 0xFF;
    //         }
    //
    //         mFh.updateEFLinearFixed(
    //                 EF_CFIS, 1, mEfCfis, null,
    //                 obtainMessage (EVENT_UPDATE_DONE, EF_CFIS));
    //     } else {
    //         log("setVoiceCallForwardingFlag: ignoring enable=" + enable
    //                 + " invalid mEfCfis=" + IccUtils.bytesToHexString(mEfCfis));
    //     }
    //
    //     if (mEfCff != null) {
    //         if (enable) {
    //             mEfCff[0] = (byte) ((mEfCff[0] & CFF_LINE1_RESET)
    //                     | CFF_UNCONDITIONAL_ACTIVE);
    //         } else {
    //             mEfCff[0] = (byte) ((mEfCff[0] & CFF_LINE1_RESET)
    //                     | CFF_UNCONDITIONAL_DEACTIVE);
    //         }
    //
    //         mFh.updateEFTransparent(
    //                 EF_CFF_CPHS, mEfCff,
    //                 obtainMessage (EVENT_UPDATE_DONE, EF_CFF_CPHS));
    //     }
    // } catch (ArrayIndexOutOfBoundsException ex) {
    //     logw("Error saving call forwarding flag to SIM. "
    //                     + "Probably malformed SIM record", ex);
    //
    // }
    assert(0);
    return NOERROR;
}

ECode SIMRecords::OnRefresh(
    /* [in] */ Boolean fileChanged,
    /* [in] */ ArrayOf<Int32>* fileList)
{
    // ==================before translated======================
    // if (fileChanged) {
    //     // A future optimization would be to inspect fileList and
    //     // only reload those files that we care about.  For now,
    //     // just re-fetch all SIM records that we cache.
    //     fetchSimRecords();
    // }
    assert(0);
    return NOERROR;
}

ECode SIMRecords::GetOperatorNumeric(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mImsi == null) {
    //     log("getOperatorNumeric: IMSI == null");
    //     return null;
    // }
    // if (mMncLength == UNINITIALIZED || mMncLength == UNKNOWN) {
    //     log("getSIMOperatorNumeric: bad mncLength");
    //     return null;
    // }
    //
    // // Length = length of MCC + length of MNC
    // // length of mcc = 3 (TS 23.003 Section 2.2)
    // return mImsi.substring(0, 3 + mMncLength);
    assert(0);
    return NOERROR;
}

ECode SIMRecords::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // AsyncResult ar;
    // AdnRecord adn;
    //
    // byte data[];
    //
    // boolean isRecordLoadResponse = false;
    //
    // if (mDestroyed.get()) {
    //     loge("Received message " + msg + "[" + msg.what + "] " +
    //             " while being destroyed. Ignoring.");
    //     return;
    // }
    //
    // try { switch (msg.what) {
    //     case EVENT_APP_READY:
    //         onReady();
    //         break;
    //
    //     case EVENT_APP_LOCKED:
    //         onLocked();
    //         break;
    //
    //     /* IO events */
    //     case EVENT_GET_IMSI_DONE:
    //         isRecordLoadResponse = true;
    //
    //         ar = (AsyncResult)msg.obj;
    //
    //         if (ar.exception != null) {
    //             loge("Exception querying IMSI, Exception:" + ar.exception);
    //             break;
    //         }
    //
    //         mImsi = (String) ar.result;
    //
    //         // IMSI (MCC+MNC+MSIN) is at least 6 digits, but not more
    //         // than 15 (and usually 15).
    //         if (mImsi != null && (mImsi.length() < 6 || mImsi.length() > 15)) {
    //             loge("invalid IMSI " + mImsi);
    //             mImsi = null;
    //         }
    //
    //         log("IMSI: mMncLength=" + mMncLength);
    //         log("IMSI: " + mImsi.substring(0, 6) + "xxxxxxx");
    //
    //         if (((mMncLength == UNKNOWN) || (mMncLength == 2)) &&
    //                 ((mImsi != null) && (mImsi.length() >= 6))) {
    //             String mccmncCode = mImsi.substring(0, 6);
    //             for (String mccmnc : MCCMNC_CODES_HAVING_3DIGITS_MNC) {
    //                 if (mccmnc.equals(mccmncCode)) {
    //                     mMncLength = 3;
    //                     log("IMSI: setting1 mMncLength=" + mMncLength);
    //                     break;
    //                 }
    //             }
    //         }
    //
    //         if (mMncLength == UNKNOWN) {
    //             // the SIM has told us all it knows, but it didn't know the mnc length.
    //             // guess using the mcc
    //             try {
    //                 int mcc = Integer.parseInt(mImsi.substring(0,3));
    //                 mMncLength = MccTable.smallestDigitsMccForMnc(mcc);
    //                 log("setting2 mMncLength=" + mMncLength);
    //             } catch (NumberFormatException e) {
    //                 mMncLength = UNKNOWN;
    //                 loge("Corrupt IMSI! setting3 mMncLength=" + mMncLength);
    //             }
    //         }
    //
    //         mParentApp.getUICCConfig().setImsi(mImsi);
    //         if (mMncLength == UNKNOWN || mMncLength == UNINITIALIZED) {
    //             // We need to default to something that seems common
    //             mParentApp.getUICCConfig().setMncLength(3);
    //         } else {
    //             mParentApp.getUICCConfig().setMncLength(mMncLength);
    //         }
    //
    //         if (mMncLength != UNKNOWN && mMncLength != UNINITIALIZED) {
    //             log("update mccmnc=" + mImsi.substring(0, 3 + mMncLength));
    //             // finally have both the imsi and the mncLength and can parse the imsi properly
    //             MccTable.updateMccMncConfiguration(mContext,
    //                     mImsi.substring(0, 3 + mMncLength), false);
    //         }
    //         mImsiReadyRegistrants.notifyRegistrants();
    //     break;
    //
    //     case EVENT_GET_MBI_DONE:
    //         boolean isValidMbdn;
    //         isRecordLoadResponse = true;
    //
    //         ar = (AsyncResult)msg.obj;
    //         data = (byte[]) ar.result;
    //
    //         isValidMbdn = false;
    //         if (ar.exception == null) {
    //             // Refer TS 51.011 Section 10.3.44 for content details
    //             log("EF_MBI: " + IccUtils.bytesToHexString(data));
    //
    //             // Voice mail record number stored first
    //             mMailboxIndex = data[0] & 0xff;
    //
    //             // check if dailing numbe id valid
    //             if (mMailboxIndex != 0 && mMailboxIndex != 0xff) {
    //                 log("Got valid mailbox number for MBDN");
    //                 isValidMbdn = true;
    //             }
    //         }
    //
    //         // one more record to load
    //         mRecordsToLoad += 1;
    //
    //         if (isValidMbdn) {
    //             // Note: MBDN was not included in NUM_OF_SIM_RECORDS_LOADED
    //             new AdnRecordLoader(mFh).loadFromEF(EF_MBDN, EF_EXT6,
    //                     mMailboxIndex, obtainMessage(EVENT_GET_MBDN_DONE));
    //         } else {
    //             // If this EF not present, try mailbox as in CPHS standard
    //             // CPHS (CPHS4_2.WW6) is a european standard.
    //             new AdnRecordLoader(mFh).loadFromEF(EF_MAILBOX_CPHS,
    //                     EF_EXT1, 1,
    //                     obtainMessage(EVENT_GET_CPHS_MAILBOX_DONE));
    //         }
    //
    //         break;
    //     case EVENT_GET_CPHS_MAILBOX_DONE:
    //     case EVENT_GET_MBDN_DONE:
    //         //Resetting the voice mail number and voice mail tag to null
    //         //as these should be updated from the data read from EF_MBDN.
    //         //If they are not reset, incase of invalid data/exception these
    //         //variables are retaining their previous values and are
    //         //causing invalid voice mailbox info display to user.
    //         mVoiceMailNum = null;
    //         mVoiceMailTag = null;
    //         isRecordLoadResponse = true;
    //
    //         ar = (AsyncResult)msg.obj;
    //
    //         if (ar.exception != null) {
    //
    //             log("Invalid or missing EF"
    //                 + ((msg.what == EVENT_GET_CPHS_MAILBOX_DONE) ? "[MAILBOX]" : "[MBDN]"));
    //
    //             // Bug #645770 fall back to CPHS
    //             // FIXME should use SST to decide
    //
    //             if (msg.what == EVENT_GET_MBDN_DONE) {
    //                 //load CPHS on fail...
    //                 // FIXME right now, only load line1's CPHS voice mail entry
    //
    //                 mRecordsToLoad += 1;
    //                 new AdnRecordLoader(mFh).loadFromEF(
    //                         EF_MAILBOX_CPHS, EF_EXT1, 1,
    //                         obtainMessage(EVENT_GET_CPHS_MAILBOX_DONE));
    //             }
    //             break;
    //         }
    //
    //         adn = (AdnRecord)ar.result;
    //
    //         log("VM: " + adn +
    //                 ((msg.what == EVENT_GET_CPHS_MAILBOX_DONE) ? " EF[MAILBOX]" : " EF[MBDN]"));
    //
    //         if (adn.isEmpty() && msg.what == EVENT_GET_MBDN_DONE) {
    //             // Bug #645770 fall back to CPHS
    //             // FIXME should use SST to decide
    //             // FIXME right now, only load line1's CPHS voice mail entry
    //             mRecordsToLoad += 1;
    //             new AdnRecordLoader(mFh).loadFromEF(
    //                     EF_MAILBOX_CPHS, EF_EXT1, 1,
    //                     obtainMessage(EVENT_GET_CPHS_MAILBOX_DONE));
    //
    //             break;
    //         }
    //
    //         mVoiceMailNum = adn.getNumber();
    //         mVoiceMailTag = adn.getAlphaTag();
    //     break;
    //
    //     case EVENT_GET_MSISDN_DONE:
    //         isRecordLoadResponse = true;
    //
    //         ar = (AsyncResult)msg.obj;
    //
    //         if (ar.exception != null) {
    //             log("Invalid or missing EF[MSISDN]");
    //             break;
    //         }
    //
    //         adn = (AdnRecord)ar.result;
    //
    //         mMsisdn = adn.getNumber();
    //         mMsisdnTag = adn.getAlphaTag();
    //
    //         log("MSISDN: " + /*mMsisdn*/ "xxxxxxx");
    //     break;
    //
    //     case EVENT_SET_MSISDN_DONE:
    //         isRecordLoadResponse = false;
    //         ar = (AsyncResult)msg.obj;
    //
    //         if (ar.userObj != null) {
    //             AsyncResult.forMessage(((Message) ar.userObj)).exception
    //                     = ar.exception;
    //             ((Message) ar.userObj).sendToTarget();
    //         }
    //         break;
    //
    //     case EVENT_GET_MWIS_DONE:
    //         isRecordLoadResponse = true;
    //
    //         ar = (AsyncResult)msg.obj;
    //         data = (byte[])ar.result;
    //
    //         if(DBG) log("EF_MWIS : " + IccUtils.bytesToHexString(data));
    //
    //         if (ar.exception != null) {
    //             if(DBG) log("EVENT_GET_MWIS_DONE exception = "
    //                     + ar.exception);
    //             break;
    //         }
    //
    //         if ((data[0] & 0xff) == 0xff) {
    //             if(DBG) log("SIMRecords: Uninitialized record MWIS");
    //             break;
    //         }
    //
    //         mEfMWIS = data;
    //         break;
    //
    //     case EVENT_GET_VOICE_MAIL_INDICATOR_CPHS_DONE:
    //         isRecordLoadResponse = true;
    //
    //         ar = (AsyncResult)msg.obj;
    //         data = (byte[])ar.result;
    //
    //         if(DBG) log("EF_CPHS_MWI: " + IccUtils.bytesToHexString(data));
    //
    //         if (ar.exception != null) {
    //             if(DBG) log("EVENT_GET_VOICE_MAIL_INDICATOR_CPHS_DONE exception = "
    //                     + ar.exception);
    //             break;
    //         }
    //
    //         mEfCPHS_MWI = data;
    //         break;
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
    //
    //     case EVENT_GET_AD_DONE:
    //         try {
    //             isRecordLoadResponse = true;
    //
    //             ar = (AsyncResult)msg.obj;
    //             data = (byte[])ar.result;
    //
    //             if (ar.exception != null) {
    //                 break;
    //             }
    //
    //             log("EF_AD: " + IccUtils.bytesToHexString(data));
    //
    //             if (data.length < 3) {
    //                 log("Corrupt AD data on SIM");
    //                 break;
    //             }
    //
    //             if (data.length == 3) {
    //                 log("MNC length not present in EF_AD");
    //                 break;
    //             }
    //
    //             mMncLength = data[3] & 0xf;
    //             log("setting4 mMncLength=" + mMncLength);
    //
    //             if (mMncLength == 0xf) {
    //                 mMncLength = UNKNOWN;
    //                 log("setting5 mMncLength=" + mMncLength);
    //             } else {
    //                 mParentApp.getUICCConfig().setMncLength(mMncLength);
    //             }
    //
    //         } finally {
    //             if (((mMncLength == UNINITIALIZED) || (mMncLength == UNKNOWN) ||
    //                     (mMncLength == 2)) && ((mImsi != null) && (mImsi.length() >= 6))) {
    //                 String mccmncCode = mImsi.substring(0, 6);
    //                 log("mccmncCode=" + mccmncCode);
    //                 for (String mccmnc : MCCMNC_CODES_HAVING_3DIGITS_MNC) {
    //                     if (mccmnc.equals(mccmncCode)) {
    //                         mMncLength = 3;
    //                         log("setting6 mMncLength=" + mMncLength);
    //                         break;
    //                     }
    //                 }
    //             }
    //
    //             if (mMncLength == UNKNOWN || mMncLength == UNINITIALIZED) {
    //                 if (mImsi != null) {
    //                     try {
    //                         int mcc = Integer.parseInt(mImsi.substring(0,3));
    //
    //                         mMncLength = MccTable.smallestDigitsMccForMnc(mcc);
    //                         log("setting7 mMncLength=" + mMncLength);
    //                     } catch (NumberFormatException e) {
    //                         mMncLength = UNKNOWN;
    //                         loge("Corrupt IMSI! setting8 mMncLength=" + mMncLength);
    //                     }
    //                 } else {
    //                     // Indicate we got this info, but it didn't contain the length.
    //                     mMncLength = UNKNOWN;
    //                     log("MNC length not present in EF_AD setting9 mMncLength=" + mMncLength);
    //                 }
    //             }
    //             if (mImsi != null && mMncLength != UNKNOWN) {
    //                 // finally have both imsi and the length of the mnc and can parse
    //                 // the imsi properly
    //                 log("update mccmnc=" + mImsi.substring(0, 3 + mMncLength));
    //                 MccTable.updateMccMncConfiguration(mContext,
    //                         mImsi.substring(0, 3 + mMncLength), false);
    //             }
    //         }
    //     break;
    //
    //     case EVENT_GET_SPN_DONE:
    //         isRecordLoadResponse = true;
    //         ar = (AsyncResult) msg.obj;
    //         getSpnFsm(false, ar);
    //     break;
    //
    //     case EVENT_GET_CFF_DONE:
    //         isRecordLoadResponse = true;
    //
    //         ar = (AsyncResult) msg.obj;
    //         data = (byte[]) ar.result;
    //
    //         if (ar.exception != null) {
    //             break;
    //         }
    //
    //         log("EF_CFF_CPHS: " + IccUtils.bytesToHexString(data));
    //         mEfCff = data;
    //
    //         // if EF_CFIS is valid, prefer it to EF_CFF_CPHS
    //         if (!validEfCfis(mEfCfis)) {
    //             mCallForwardingEnabled =
    //                 ((data[0] & CFF_LINE1_MASK) == CFF_UNCONDITIONAL_ACTIVE);
    //
    //             mRecordsEventsRegistrants.notifyResult(EVENT_CFI);
    //         } else {
    //             log("EVENT_GET_CFF_DONE: EF_CFIS is valid, ignoring EF_CFF_CPHS");
    //         }
    //         break;
    //
    //     case EVENT_GET_SPDI_DONE:
    //         isRecordLoadResponse = true;
    //
    //         ar = (AsyncResult)msg.obj;
    //         data = (byte[])ar.result;
    //
    //         if (ar.exception != null) {
    //             break;
    //         }
    //
    //         parseEfSpdi(data);
    //     break;
    //
    //     case EVENT_UPDATE_DONE:
    //         ar = (AsyncResult)msg.obj;
    //         if (ar.exception != null) {
    //             logw("update failed. ", ar.exception);
    //         }
    //     break;
    //
    //     case EVENT_GET_PNN_DONE:
    //         isRecordLoadResponse = true;
    //
    //         ar = (AsyncResult)msg.obj;
    //         data = (byte[])ar.result;
    //
    //         if (ar.exception != null) {
    //             break;
    //         }
    //
    //         SimTlv tlv = new SimTlv(data, 0, data.length);
    //
    //         for ( ; tlv.isValidObject() ; tlv.nextObject()) {
    //             if (tlv.getTag() == TAG_FULL_NETWORK_NAME) {
    //                 mPnnHomeName
    //                     = IccUtils.networkNameToString(
    //                         tlv.getData(), 0, tlv.getData().length);
    //                 break;
    //             }
    //         }
    //     break;
    //
    //     case EVENT_GET_ALL_SMS_DONE:
    //         isRecordLoadResponse = true;
    //
    //         ar = (AsyncResult)msg.obj;
    //         if (ar.exception != null)
    //             break;
    //
    //         handleSmses((ArrayList<byte []>) ar.result);
    //         break;
    //
    //     case EVENT_MARK_SMS_READ_DONE:
    //         Rlog.i("ENF", "marked read: sms " + msg.arg1);
    //         break;
    //
    //     case EVENT_GET_SMS_DONE:
    //         isRecordLoadResponse = false;
    //         ar = (AsyncResult)msg.obj;
    //         if (ar.exception == null) {
    //             handleSms((byte[])ar.result);
    //         } else {
    //             loge("Error on GET_SMS with exp " + ar.exception);
    //         }
    //         break;
    //     case EVENT_GET_SST_DONE:
    //         isRecordLoadResponse = true;
    //
    //         ar = (AsyncResult)msg.obj;
    //         data = (byte[])ar.result;
    //
    //         if (ar.exception != null) {
    //             break;
    //         }
    //
    //         mUsimServiceTable = new UsimServiceTable(data);
    //         if (DBG) log("SST: " + mUsimServiceTable);
    //         break;
    //
    //     case EVENT_GET_INFO_CPHS_DONE:
    //         isRecordLoadResponse = true;
    //
    //         ar = (AsyncResult)msg.obj;
    //
    //         if (ar.exception != null) {
    //             break;
    //         }
    //
    //         mCphsInfo = (byte[])ar.result;
    //
    //         if (DBG) log("iCPHS: " + IccUtils.bytesToHexString(mCphsInfo));
    //     break;
    //
    //     case EVENT_SET_MBDN_DONE:
    //         isRecordLoadResponse = false;
    //         ar = (AsyncResult)msg.obj;
    //
    //         if (DBG) log("EVENT_SET_MBDN_DONE ex:" + ar.exception);
    //         if (ar.exception == null) {
    //             mVoiceMailNum = mNewVoiceMailNum;
    //             mVoiceMailTag = mNewVoiceMailTag;
    //         }
    //
    //         if (isCphsMailboxEnabled()) {
    //             adn = new AdnRecord(mVoiceMailTag, mVoiceMailNum);
    //             Message onCphsCompleted = (Message) ar.userObj;
    //
    //             /* write to cphs mailbox whenever it is available but
    //             * we only need notify caller once if both updating are
    //             * successful.
    //             *
    //             * so if set_mbdn successful, notify caller here and set
    //             * onCphsCompleted to null
    //             */
    //             if (ar.exception == null && ar.userObj != null) {
    //                 AsyncResult.forMessage(((Message) ar.userObj)).exception
    //                         = null;
    //                 ((Message) ar.userObj).sendToTarget();
    //
    //                 if (DBG) log("Callback with MBDN successful.");
    //
    //                 onCphsCompleted = null;
    //             }
    //
    //             new AdnRecordLoader(mFh).
    //                     updateEF(adn, EF_MAILBOX_CPHS, EF_EXT1, 1, null,
    //                     obtainMessage(EVENT_SET_CPHS_MAILBOX_DONE,
    //                             onCphsCompleted));
    //         } else {
    //             if (ar.userObj != null) {
    //                 Resources resource = Resources.getSystem();
    //                 if (ar.exception != null && resource.getBoolean(com.android.internal.
    //                             R.bool.editable_voicemailnumber)) {
    //                     // GSMPhone will store vm number on device
    //                     // when IccVmNotSupportedException occurred
    //                     AsyncResult.forMessage(((Message) ar.userObj)).exception
    //                         = new IccVmNotSupportedException(
    //                                 "Update SIM voice mailbox error");
    //                 } else {
    //                     AsyncResult.forMessage(((Message) ar.userObj)).exception
    //                         = ar.exception;
    //                 }
    //                 ((Message) ar.userObj).sendToTarget();
    //             }
    //         }
    //         break;
    //     case EVENT_SET_CPHS_MAILBOX_DONE:
    //         isRecordLoadResponse = false;
    //         ar = (AsyncResult)msg.obj;
    //         if(ar.exception == null) {
    //             mVoiceMailNum = mNewVoiceMailNum;
    //             mVoiceMailTag = mNewVoiceMailTag;
    //         } else {
    //             if (DBG) log("Set CPHS MailBox with exception: "
    //                     + ar.exception);
    //         }
    //         if (ar.userObj != null) {
    //             if (DBG) log("Callback with CPHS MB successful.");
    //             AsyncResult.forMessage(((Message) ar.userObj)).exception
    //                     = ar.exception;
    //             ((Message) ar.userObj).sendToTarget();
    //         }
    //         break;
    //     case EVENT_GET_CFIS_DONE:
    //         isRecordLoadResponse = true;
    //
    //         ar = (AsyncResult)msg.obj;
    //         data = (byte[])ar.result;
    //
    //         if (ar.exception != null) {
    //             break;
    //         }
    //
    //         log("EF_CFIS: " + IccUtils.bytesToHexString(data));
    //
    //         if (validEfCfis(data)) {
    //             mEfCfis = data;
    //
    //             // Refer TS 51.011 Section 10.3.46 for the content description
    //             mCallForwardingEnabled = ((data[1] & 0x01) != 0);
    //             log("EF_CFIS: callForwardingEnabled=" + mCallForwardingEnabled);
    //
    //             mRecordsEventsRegistrants.notifyResult(EVENT_CFI);
    //         } else {
    //             log("EF_CFIS: invalid data=" + IccUtils.bytesToHexString(data));
    //         }
    //         break;
    //
    //     case EVENT_GET_CSP_CPHS_DONE:
    //         isRecordLoadResponse = true;
    //
    //         ar = (AsyncResult)msg.obj;
    //
    //         if (ar.exception != null) {
    //             loge("Exception in fetching EF_CSP data " + ar.exception);
    //             break;
    //         }
    //
    //         data = (byte[])ar.result;
    //
    //         log("EF_CSP: " + IccUtils.bytesToHexString(data));
    //         handleEfCspData(data);
    //         break;
    //
    //     case EVENT_GET_GID1_DONE:
    //         isRecordLoadResponse = true;
    //
    //         ar = (AsyncResult)msg.obj;
    //         data =(byte[])ar.result;
    //
    //         if (ar.exception != null) {
    //             loge("Exception in get GID1 " + ar.exception);
    //             mGid1 = null;
    //             break;
    //         }
    //         mGid1 = IccUtils.bytesToHexString(data);
    //         log("GID1: " + mGid1);
    //
    //         break;
    //
    //     default:
    //         super.handleMessage(msg);   // IccRecords handles generic record load responses
    //
    // }}catch (RuntimeException exc) {
    //     // I don't want these exceptions to be fatal
    //     logw("Exception parsing SIM record", exc);
    // } finally {
    //     // Count up record load responses even if they are fails
    //     if (isRecordLoadResponse) {
    //         onRecordLoaded();
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode SIMRecords::HandleSmsOnIcc(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    //
    // int[] index = (int[])ar.result;
    //
    // if (ar.exception != null || index.length != 1) {
    //     loge(" Error on SMS_ON_SIM with exp "
    //            + ar.exception + " length " + index.length);
    // } else {
    //     log("READ EF_SMS RECORD index= " + index[0]);
    //     mFh.loadEFLinearFixed(EF_SMS,index[0],
    //                     obtainMessage(EVENT_GET_SMS_DONE));
    // }
    assert(0);
    return NOERROR;
}

ECode SIMRecords::OnReady()
{
    // ==================before translated======================
    // fetchSimRecords();
    assert(0);
    return NOERROR;
}

ECode SIMRecords::GetDisplayRule(
    /* [in] */ const String& plmn,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // int rule;
    // if ((mContext != null) && mContext.getResources().getBoolean(
    //         com.android.internal.R.bool.def_telephony_spn_spec_enabled)) {
    //     rule = SPN_RULE_SHOW_SPN;
    //     return rule;
    // }
    //
    // if (mParentApp != null && mParentApp.getUiccCard() != null &&
    //     mParentApp.getUiccCard().getOperatorBrandOverride() != null) {
    // // If the operator has been overridden, treat it as the SPN file on the SIM did not exist.
    //     rule = SPN_RULE_SHOW_PLMN;
    // } else if (TextUtils.isEmpty(getServiceProviderName()) || mSpnDisplayCondition == -1) {
    //     // No EF_SPN content was found on the SIM, or not yet loaded.  Just show ONS.
    //     rule = SPN_RULE_SHOW_PLMN;
    // } else if (isOnMatchingPlmn(plmn)) {
    //     rule = SPN_RULE_SHOW_SPN;
    //     if ((mSpnDisplayCondition & 0x01) == 0x01) {
    //         // ONS required when registered to HPLMN or PLMN in EF_SPDI
    //         rule |= SPN_RULE_SHOW_PLMN;
    //     }
    // } else {
    //     rule = SPN_RULE_SHOW_PLMN;
    //     if ((mSpnDisplayCondition & 0x02) == 0x00) {
    //         // SPN required if not registered to HPLMN or PLMN in EF_SPDI
    //         rule |= SPN_RULE_SHOW_SPN;
    //     }
    // }
    // return rule;
    assert(0);
    return NOERROR;
}

ECode SIMRecords::IsCspPlmnEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCspPlmnEnabled;
    assert(0);
    return NOERROR;
}

ECode SIMRecords::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    // ==================before translated======================
    // pw.println("SIMRecords: " + this);
    // pw.println(" extends:");
    // super.dump(fd, pw, args);
    // pw.println(" mVmConfig=" + mVmConfig);
    // pw.println(" mSpnOverride=" + mSpnOverride);
    // pw.println(" mCallForwardingEnabled=" + mCallForwardingEnabled);
    // pw.println(" mSpnState=" + mSpnState);
    // pw.println(" mCphsInfo=" + mCphsInfo);
    // pw.println(" mCspPlmnEnabled=" + mCspPlmnEnabled);
    // pw.println(" mEfMWIS[]=" + Arrays.toString(mEfMWIS));
    // pw.println(" mEfCPHS_MWI[]=" + Arrays.toString(mEfCPHS_MWI));
    // pw.println(" mEfCff[]=" + Arrays.toString(mEfCff));
    // pw.println(" mEfCfis[]=" + Arrays.toString(mEfCfis));
    // pw.println(" mSpnDisplayCondition=" + mSpnDisplayCondition);
    // pw.println(" mSpdiNetworks[]=" + mSpdiNetworks);
    // pw.println(" mPnnHomeName=" + mPnnHomeName);
    // pw.println(" mUsimServiceTable=" + mUsimServiceTable);
    // pw.println(" mGid1=" + mGid1);
    // pw.flush();
    assert(0);
    return NOERROR;
}

ECode SIMRecords::Finalize()
{
    // ==================before translated======================
    // if(DBG) log("finalized");
    assert(0);
    return NOERROR;
}

void SIMRecords::ResetRecords()
{
    // ==================before translated======================
    // mImsi = null;
    // mMsisdn = null;
    // mVoiceMailNum = null;
    // mMncLength = UNINITIALIZED;
    // log("setting0 mMncLength" + mMncLength);
    // mIccId = null;
    // // -1 means no EF_SPN found; treat accordingly.
    // mSpnDisplayCondition = -1;
    // mEfMWIS = null;
    // mEfCPHS_MWI = null;
    // mSpdiNetworks = null;
    // mPnnHomeName = null;
    // mGid1 = null;
    //
    // mAdnCache.reset();
    //
    // log("SIMRecords: onRadioOffOrNotAvailable set 'gsm.sim.operator.numeric' to operator=null");
    // log("update icc_operator_numeric=" + null);
    // setSystemProperty(PROPERTY_ICC_OPERATOR_NUMERIC, null);
    // setSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, null);
    // setSystemProperty(PROPERTY_ICC_OPERATOR_ISO_COUNTRY, null);
    // mParentApp.getUICCConfig().setImsi(mImsi);
    // mParentApp.getUICCConfig().setMncLength(mMncLength);
    //
    // // recordsRequested is set to false indicating that the SIM
    // // read requests made so far are not valid. This is set to
    // // true only when fresh set of read requests are made.
    // mRecordsRequested = false;
    assert(0);
}

ECode SIMRecords::HandleFileUpdate(
    /* [in] */ Int32 efid)
{
    // ==================before translated======================
    // switch(efid) {
    //     case EF_MBDN:
    //         mRecordsToLoad++;
    //         new AdnRecordLoader(mFh).loadFromEF(EF_MBDN, EF_EXT6,
    //                 mMailboxIndex, obtainMessage(EVENT_GET_MBDN_DONE));
    //         break;
    //     case EF_MAILBOX_CPHS:
    //         mRecordsToLoad++;
    //         new AdnRecordLoader(mFh).loadFromEF(EF_MAILBOX_CPHS, EF_EXT1,
    //                 1, obtainMessage(EVENT_GET_CPHS_MAILBOX_DONE));
    //         break;
    //     case EF_CSP_CPHS:
    //         mRecordsToLoad++;
    //         log("[CSP] SIM Refresh for EF_CSP_CPHS");
    //         mFh.loadEFTransparent(EF_CSP_CPHS,
    //                 obtainMessage(EVENT_GET_CSP_CPHS_DONE));
    //         break;
    //     case EF_FDN:
    //         if (DBG) log("SIM Refresh called for EF_FDN");
    //         mParentApp.queryFdn();
    //     case EF_MSISDN:
    //         mRecordsToLoad++;
    //         log("SIM Refresh called for EF_MSISDN");
    //         new AdnRecordLoader(mFh).loadFromEF(EF_MSISDN, getExtFromEf(EF_MSISDN), 1,
    //                 obtainMessage(EVENT_GET_MSISDN_DONE));
    //         break;
    //     case EF_CFIS:
    //         mRecordsToLoad++;
    //         log("SIM Refresh called for EF_CFIS");
    //         mFh.loadEFLinearFixed(EF_CFIS,
    //                 1, obtainMessage(EVENT_GET_CFIS_DONE));
    //         break;
    //     case EF_CFF_CPHS:
    //         mRecordsToLoad++;
    //         log("SIM Refresh called for EF_CFF_CPHS");
    //         mFh.loadEFTransparent(EF_CFF_CPHS,
    //                 obtainMessage(EVENT_GET_CFF_DONE));
    //         break;
    //     case EF_ADN:
    //         log("SIM Refresh for EF_ADN");
    //         mAdnCache.reset();
    //         break;
    //     default:
    //         // For now, fetch all records if this is not a
    //         // voicemail number.
    //         // TODO: Handle other cases, instead of fetching all.
    //         mAdnCache.reset();
    //         fetchSimRecords();
    //         break;
    // }
    assert(0);
    return NOERROR;
}

ECode SIMRecords::OnRecordLoaded()
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

ECode SIMRecords::OnAllRecordsLoaded()
{
    // ==================before translated======================
    // if (DBG) log("record load complete");
    //
    // setLocaleFromUsim();
    //
    // if (mParentApp.getState() == AppState.APPSTATE_PIN ||
    //        mParentApp.getState() == AppState.APPSTATE_PUK) {
    //     // reset recordsRequested, since sim is not loaded really
    //     mRecordsRequested = false;
    //     // lock state, only update language
    //     return ;
    // }
    //
    // // Some fields require more than one SIM record to set
    //
    // String operator = getOperatorNumeric();
    // if (!TextUtils.isEmpty(operator)) {
    //     log("onAllRecordsLoaded set 'gsm.sim.operator.numeric' to operator='" +
    //             operator + "'");
    //     log("update icc_operator_numeric=" + operator);
    //     setSystemProperty(PROPERTY_ICC_OPERATOR_NUMERIC, operator);
    //     final SubscriptionController subController = SubscriptionController.getInstance();
    //     subController.setMccMnc(operator, subController.getDefaultSmsSubId());
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
    // setVoiceMailByCountry(operator);
    // setSpnFromConfig(operator);
    //
    // mRecordsLoadedRegistrants.notifyRegistrants(
    //     new AsyncResult(null, null, null));
    assert(0);
    return NOERROR;
}

void SIMRecords::FetchSimRecords()
{
    // ==================before translated======================
    // mRecordsRequested = true;
    //
    // if (DBG) log("fetchSimRecords " + mRecordsToLoad);
    //
    // mCi.getIMSIForApp(mParentApp.getAid(), obtainMessage(EVENT_GET_IMSI_DONE));
    // mRecordsToLoad++;
    //
    // mFh.loadEFTransparent(EF_ICCID, obtainMessage(EVENT_GET_ICCID_DONE));
    // mRecordsToLoad++;
    //
    // // FIXME should examine EF[MSISDN]'s capability configuration
    // // to determine which is the voice/data/fax line
    // new AdnRecordLoader(mFh).loadFromEF(EF_MSISDN, getExtFromEf(EF_MSISDN), 1,
    //             obtainMessage(EVENT_GET_MSISDN_DONE));
    // mRecordsToLoad++;
    //
    // // Record number is subscriber profile
    // mFh.loadEFLinearFixed(EF_MBI, 1, obtainMessage(EVENT_GET_MBI_DONE));
    // mRecordsToLoad++;
    //
    // mFh.loadEFTransparent(EF_AD, obtainMessage(EVENT_GET_AD_DONE));
    // mRecordsToLoad++;
    //
    // // Record number is subscriber profile
    // mFh.loadEFLinearFixed(EF_MWIS, 1, obtainMessage(EVENT_GET_MWIS_DONE));
    // mRecordsToLoad++;
    //
    //
    // // Also load CPHS-style voice mail indicator, which stores
    // // the same info as EF[MWIS]. If both exist, both are updated
    // // but the EF[MWIS] data is preferred
    // // Please note this must be loaded after EF[MWIS]
    // mFh.loadEFTransparent(
    //         EF_VOICE_MAIL_INDICATOR_CPHS,
    //         obtainMessage(EVENT_GET_VOICE_MAIL_INDICATOR_CPHS_DONE));
    // mRecordsToLoad++;
    //
    // // Same goes for Call Forward Status indicator: fetch both
    // // EF[CFIS] and CPHS-EF, with EF[CFIS] preferred.
    // mFh.loadEFLinearFixed(EF_CFIS, 1, obtainMessage(EVENT_GET_CFIS_DONE));
    // mRecordsToLoad++;
    // mFh.loadEFTransparent(EF_CFF_CPHS, obtainMessage(EVENT_GET_CFF_DONE));
    // mRecordsToLoad++;
    //
    //
    // getSpnFsm(true, null);
    //
    // mFh.loadEFTransparent(EF_SPDI, obtainMessage(EVENT_GET_SPDI_DONE));
    // mRecordsToLoad++;
    //
    // mFh.loadEFLinearFixed(EF_PNN, 1, obtainMessage(EVENT_GET_PNN_DONE));
    // mRecordsToLoad++;
    //
    // mFh.loadEFTransparent(EF_SST, obtainMessage(EVENT_GET_SST_DONE));
    // mRecordsToLoad++;
    //
    // mFh.loadEFTransparent(EF_INFO_CPHS, obtainMessage(EVENT_GET_INFO_CPHS_DONE));
    // mRecordsToLoad++;
    //
    // mFh.loadEFTransparent(EF_CSP_CPHS,obtainMessage(EVENT_GET_CSP_CPHS_DONE));
    // mRecordsToLoad++;
    //
    // mFh.loadEFTransparent(EF_GID1, obtainMessage(EVENT_GET_GID1_DONE));
    // mRecordsToLoad++;
    //
    // loadEfLiAndEfPl();
    // mFh.getEFLinearRecordSize(EF_SMS, obtainMessage(EVENT_GET_SMS_RECORD_SIZE_DONE));
    //
    // // XXX should seek instead of examining them all
    // if (false) { // XXX
    //     mFh.loadEFLinearFixedAll(EF_SMS, obtainMessage(EVENT_GET_ALL_SMS_DONE));
    //     mRecordsToLoad++;
    // }
    //
    // if (CRASH_RIL) {
    //     String sms = "0107912160130310f20404d0110041007030208054832b0120"
    //                  + "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
    //                  + "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
    //                  + "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
    //                  + "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
    //                  + "ffffffffffffffffffffffffffffff";
    //     byte[] ba = IccUtils.hexStringToBytes(sms);
    //
    //     mFh.updateEFLinearFixed(EF_SMS, 1, ba, null,
    //                     obtainMessage(EVENT_MARK_SMS_READ_DONE, 1));
    // }
    // if (DBG) log("fetchSimRecords " + mRecordsToLoad + " requested: " + mRecordsRequested);
    assert(0);
}

ECode SIMRecords::Log(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "[SIMRecords] " + s);
    assert(0);
    return NOERROR;
}

ECode SIMRecords::Loge(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "[SIMRecords] " + s);
    assert(0);
    return NOERROR;
}

void SIMRecords::Logw(
    /* [in] */ const String& s)
    ///* [in] */ Throwable* tr)
{
    // ==================before translated======================
    // Rlog.w(LOGTAG, "[SIMRecords] " + s, tr);
    assert(0);
}

void SIMRecords::Logv(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Rlog.v(LOGTAG, "[SIMRecords] " + s);
    assert(0);
}

AutoPtr<ArrayOf<String> > SIMRecords::MiddleInitMccmncCodesHaving3digitsMnc()
{
    // ==================before translated======================
    // ->WWZ_SIGN: ARRAY_INIT_START {
    // "302370", "302720", "310260",
    //          "405025", "405026", "405027", "405028", "405029", "405030", "405031", "405032",
    //          "405033", "405034", "405035", "405036", "405037", "405038", "405039", "405040",
    //          "405041", "405042", "405043", "405044", "405045", "405046", "405047", "405750",
    //          "405751", "405752", "405753", "405754", "405755", "405756", "405799", "405800",
    //          "405801", "405802", "405803", "405804", "405805", "405806", "405807", "405808",
    //          "405809", "405810", "405811", "405812", "405813", "405814", "405815", "405816",
    //          "405817", "405818", "405819", "405820", "405821", "405822", "405823", "405824",
    //          "405825", "405826", "405827", "405828", "405829", "405830", "405831", "405832",
    //          "405833", "405834", "405835", "405836", "405837", "405838", "405839", "405840",
    //          "405841", "405842", "405843", "405844", "405845", "405846", "405847", "405848",
    //          "405849", "405850", "405851", "405852", "405853", "405875", "405876", "405877",
    //          "405878", "405879", "405880", "405881", "405882", "405883", "405884", "405885",
    //          "405886", "405908", "405909", "405910", "405911", "405912", "405913", "405914",
    //          "405915", "405916", "405917", "405918", "405919", "405920", "405921", "405922",
    //          "405923", "405924", "405925", "405926", "405927", "405928", "405929", "405930",
    //          "405931", "405932", "502142", "502143", "502145", "502146", "502147", "502148"
    // ->WWZ_SIGN: ARRAY_INIT_END }
    assert(0);
    AutoPtr<ArrayOf<String> > empty;
    return empty;
}

Int32 SIMRecords::GetExtFromEf(
    /* [in] */ Int32 ef)
{
    // ==================before translated======================
    // int ext;
    // switch (ef) {
    //     case EF_MSISDN:
    //         /* For USIM apps use EXT5. (TS 31.102 Section 4.2.37) */
    //         if (mParentApp.getType() == AppType.APPTYPE_USIM) {
    //             ext = EF_EXT5;
    //         } else {
    //             ext = EF_EXT1;
    //         }
    //         break;
    //     default:
    //         ext = EF_EXT1;
    // }
    // return ext;
    assert(0);
    return 0;
}

Boolean SIMRecords::ValidEfCfis(
    /* [in] */ ArrayOf<Byte>* data)
{
    // ==================before translated======================
    // return ((data != null) && (data[0] >= 1) && (data[0] <= 4));
    assert(0);
    return FALSE;
}

Int32 SIMRecords::DispatchGsmMessage(
    /* [in] */ ISmsMessage* message)
{
    // ==================before translated======================
    // mNewSmsRegistrants.notifyResult(message);
    // return 0;
    assert(0);
    return 0;
}

void SIMRecords::HandleSms(
    /* [in] */ ArrayOf<Byte>* ba)
{
    // ==================before translated======================
    // if (ba[0] != 0)
    //     Rlog.d("ENF", "status : " + ba[0]);
    //
    // // 3GPP TS 51.011 v5.0.0 (20011-12)  10.5.3
    // // 3 == "received by MS from network; message to be read"
    // if (ba[0] == 3) {
    //     int n = ba.length;
    //
    //     // Note: Data may include trailing FF's.  That's OK; message
    //     // should still parse correctly.
    //     byte[] pdu = new byte[n - 1];
    //     System.arraycopy(ba, 1, pdu, 0, n - 1);
    //     SmsMessage message = SmsMessage.createFromPdu(pdu, SmsConstants.FORMAT_3GPP);
    //
    //     dispatchGsmMessage(message);
    // }
    assert(0);
}

void SIMRecords::HandleSmses(
    /* [in] */ IArrayList/*<ArrayOf<Byte>>*/* messages)
{
    // ==================before translated======================
    // int count = messages.size();
    //
    // for (int i = 0; i < count; i++) {
    //     byte[] ba = messages.get(i);
    //
    //     if (ba[0] != 0)
    //         Rlog.i("ENF", "status " + i + ": " + ba[0]);
    //
    //     // 3GPP TS 51.011 v5.0.0 (20011-12)  10.5.3
    //     // 3 == "received by MS from network; message to be read"
    //
    //     if (ba[0] == 3) {
    //         int n = ba.length;
    //
    //         // Note: Data may include trailing FF's.  That's OK; message
    //         // should still parse correctly.
    //         byte[] pdu = new byte[n - 1];
    //         System.arraycopy(ba, 1, pdu, 0, n - 1);
    //         SmsMessage message = SmsMessage.createFromPdu(pdu, SmsConstants.FORMAT_3GPP);
    //
    //         dispatchGsmMessage(message);
    //
    //         // 3GPP TS 51.011 v5.0.0 (20011-12)  10.5.3
    //         // 1 == "received by MS from network; message read"
    //
    //         ba[0] = 1;
    //
    //         if (false) { // FIXME: writing seems to crash RdoServD
    //             mFh.updateEFLinearFixed(EF_SMS,
    //                     i, ba, null, obtainMessage(EVENT_MARK_SMS_READ_DONE, i));
    //         }
    //     }
    // }
    assert(0);
}

String SIMRecords::FindBestLanguage(
    /* [in] */ ArrayOf<Byte>* languages)
{
    // ==================before translated======================
    // String bestMatch = null;
    // String[] locales = mContext.getAssets().getLocales();
    //
    // if ((languages == null) || (locales == null)) return null;
    //
    // // Each 2-bytes consists of one language
    // for (int i = 0; (i + 1) < languages.length; i += 2) {
    //     try {
    //         String lang = new String(languages, i, 2, "ISO-8859-1");
    //         if (DBG) log ("languages from sim = " + lang);
    //         for (int j = 0; j < locales.length; j++) {
    //             if (locales[j] != null && locales[j].length() >= 2 &&
    //                     locales[j].substring(0, 2).equalsIgnoreCase(lang)) {
    //                 return lang;
    //             }
    //         }
    //         if (bestMatch != null) break;
    //     } catch(java.io.UnsupportedEncodingException e) {
    //         log ("Failed to parse USIM language records" + e);
    //     }
    // }
    // // no match found. return null
    // return null;
    assert(0);
    return String("");
}

void SIMRecords::SetLocaleFromUsim()
{
    // ==================before translated======================
    // String prefLang = null;
    // // check EFli then EFpl
    // prefLang = findBestLanguage(mEfLi);
    //
    // if (prefLang == null) {
    //     prefLang = findBestLanguage(mEfPl);
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
    //     if (DBG) log("Setting locale to " + prefLang + "_" + country);
    //     MccTable.setSystemLocale(mContext, prefLang, country);
    // } else {
    //     if (DBG) log ("No suitable USIM selected locale");
    // }
    assert(0);
}

void SIMRecords::SetSpnFromConfig(
    /* [in] */ const String& carrier)
{
    // ==================before translated======================
    // if (mSpnOverride.containsCarrier(carrier)) {
    //     setServiceProviderName(mSpnOverride.getSpn(carrier));
    //     SystemProperties.set(PROPERTY_ICC_OPERATOR_ALPHA, getServiceProviderName());
    // }
    assert(0);
}

void SIMRecords::SetVoiceMailByCountry(
    /* [in] */ const String& spn)
{
    // ==================before translated======================
    // if (mVmConfig.containsCarrier(spn)) {
    //     mIsVoiceMailFixed = true;
    //     mVoiceMailNum = mVmConfig.getVoiceMailNumber(spn);
    //     mVoiceMailTag = mVmConfig.getVoiceMailTag(spn);
    // }
    assert(0);
}

void SIMRecords::OnLocked()
{
    // ==================before translated======================
    // if (DBG) log("only fetch EF_LI and EF_PL in lock state");
    // loadEfLiAndEfPl();
    assert(0);
}

void SIMRecords::LoadEfLiAndEfPl()
{
    // ==================before translated======================
    // if (mParentApp.getType() == AppType.APPTYPE_USIM) {
    //     mRecordsRequested = true;
    //     mFh.loadEFTransparent(EF_LI,
    //             obtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfUsimLiLoaded()));
    //     mRecordsToLoad++;
    //
    //     mFh.loadEFTransparent(EF_PL,
    //             obtainMessage(EVENT_GET_ICC_RECORD_DONE, new EfPlLoaded()));
    //     mRecordsToLoad++;
    // }
    assert(0);
}

Boolean SIMRecords::IsOnMatchingPlmn(
    /* [in] */ const String& plmn)
{
    // ==================before translated======================
    // if (plmn == null) return false;
    //
    // if (plmn.equals(getOperatorNumeric())) {
    //     return true;
    // }
    //
    // if (mSpdiNetworks != null) {
    //     for (String spdiNet : mSpdiNetworks) {
    //         if (plmn.equals(spdiNet)) {
    //             return true;
    //         }
    //     }
    // }
    // return false;
    assert(0);
    return FALSE;
}

void SIMRecords::GetSpnFsm(
    /* [in] */ Boolean start,
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // byte[] data;
    //
    // if (start) {
    //     // Check previous state to see if there is outstanding
    //     // SPN read
    //     if(mSpnState == GetSpnFsmState.READ_SPN_3GPP ||
    //        mSpnState == GetSpnFsmState.READ_SPN_CPHS ||
    //        mSpnState == GetSpnFsmState.READ_SPN_SHORT_CPHS ||
    //        mSpnState == GetSpnFsmState.INIT) {
    //         // Set INIT then return so the INIT code
    //         // will run when the outstanding read done.
    //         mSpnState = GetSpnFsmState.INIT;
    //         return;
    //     } else {
    //         mSpnState = GetSpnFsmState.INIT;
    //     }
    // }
    //
    // switch(mSpnState){
    //     case INIT:
    //         setServiceProviderName(null);
    //
    //         mFh.loadEFTransparent(EF_SPN,
    //                 obtainMessage(EVENT_GET_SPN_DONE));
    //         mRecordsToLoad++;
    //
    //         mSpnState = GetSpnFsmState.READ_SPN_3GPP;
    //         break;
    //     case READ_SPN_3GPP:
    //         if (ar != null && ar.exception == null) {
    //             data = (byte[]) ar.result;
    //             mSpnDisplayCondition = 0xff & data[0];
    //             setServiceProviderName(IccUtils.adnStringFieldToString(
    //                     data, 1, data.length - 1));
    //
    //             if (DBG) log("Load EF_SPN: " + getServiceProviderName()
    //                     + " spnDisplayCondition: " + mSpnDisplayCondition);
    //             setSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, getServiceProviderName());
    //
    //             mSpnState = GetSpnFsmState.IDLE;
    //         } else {
    //             mFh.loadEFTransparent( EF_SPN_CPHS,
    //                     obtainMessage(EVENT_GET_SPN_DONE));
    //             mRecordsToLoad++;
    //
    //             mSpnState = GetSpnFsmState.READ_SPN_CPHS;
    //
    //             // See TS 51.011 10.3.11.  Basically, default to
    //             // show PLMN always, and SPN also if roaming.
    //             mSpnDisplayCondition = -1;
    //         }
    //         break;
    //     case READ_SPN_CPHS:
    //         if (ar != null && ar.exception == null) {
    //             data = (byte[]) ar.result;
    //             setServiceProviderName(IccUtils.adnStringFieldToString(data, 0, data.length));
    //
    //             if (DBG) log("Load EF_SPN_CPHS: " + getServiceProviderName());
    //             setSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, getServiceProviderName());
    //
    //             mSpnState = GetSpnFsmState.IDLE;
    //         } else {
    //             mFh.loadEFTransparent(
    //                     EF_SPN_SHORT_CPHS, obtainMessage(EVENT_GET_SPN_DONE));
    //             mRecordsToLoad++;
    //
    //             mSpnState = GetSpnFsmState.READ_SPN_SHORT_CPHS;
    //         }
    //         break;
    //     case READ_SPN_SHORT_CPHS:
    //         if (ar != null && ar.exception == null) {
    //             data = (byte[]) ar.result;
    //             setServiceProviderName(IccUtils.adnStringFieldToString(data, 0, data.length));
    //
    //             if (DBG) log("Load EF_SPN_SHORT_CPHS: " + getServiceProviderName());
    //             setSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, getServiceProviderName());
    //         }else {
    //             if (DBG) log("No SPN loaded in either CHPS or 3GPP");
    //             if (mPnnHomeName != null && mSpn == null) {
    //                 if (DBG) log("Falling back to home network name for SPN");
    //                 mSpn = mPnnHomeName;
    //                 setSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, mSpn);
    //                 mRecordsEventsRegistrants.notifyResult(EVENT_SPN);
    //             }
    //         }
    //
    //         mSpnState = GetSpnFsmState.IDLE;
    //         break;
    //     default:
    //         mSpnState = GetSpnFsmState.IDLE;
    // }
    assert(0);
}

void SIMRecords::ParseEfSpdi(
    /* [in] */ ArrayOf<Byte>* data)
{
    // ==================before translated======================
    // SimTlv tlv = new SimTlv(data, 0, data.length);
    //
    // byte[] plmnEntries = null;
    //
    // for ( ; tlv.isValidObject() ; tlv.nextObject()) {
    //     // Skip SPDI tag, if existant
    //     if (tlv.getTag() == TAG_SPDI) {
    //       tlv = new SimTlv(tlv.getData(), 0, tlv.getData().length);
    //     }
    //     // There should only be one TAG_SPDI_PLMN_LIST
    //     if (tlv.getTag() == TAG_SPDI_PLMN_LIST) {
    //         plmnEntries = tlv.getData();
    //         break;
    //     }
    // }
    //
    // if (plmnEntries == null) {
    //     return;
    // }
    //
    // mSpdiNetworks = new ArrayList<String>(plmnEntries.length / 3);
    //
    // for (int i = 0 ; i + 2 < plmnEntries.length ; i += 3) {
    //     String plmnCode;
    //     plmnCode = IccUtils.bcdToString(plmnEntries, i, 3);
    //
    //     // Valid operator codes are 5 or 6 digits
    //     if (plmnCode.length() >= 5) {
    //         log("EF_SPDI network: " + plmnCode);
    //         mSpdiNetworks.add(plmnCode);
    //     }
    // }
    assert(0);
}

Boolean SIMRecords::IsCphsMailboxEnabled()
{
    // ==================before translated======================
    // if (mCphsInfo == null)  return false;
    // return ((mCphsInfo[1] & CPHS_SST_MBN_MASK) == CPHS_SST_MBN_ENABLED );
    assert(0);
    return FALSE;
}

void SIMRecords::HandleEfCspData(
    /* [in] */ ArrayOf<Byte>* data)
{
    // ==================before translated======================
    // // As per spec CPHS4_2.WW6, CPHS B.4.7.1, EF_CSP contains CPHS defined
    // // 18 bytes (i.e 9 service groups info) and additional data specific to
    // // operator. The valueAddedServicesGroup is not part of standard
    // // services. This is operator specific and can be programmed any where.
    // // Normally this is programmed as 10th service after the standard
    // // services.
    // int usedCspGroups = data.length / 2;
    // // This is the "Service Group Number" of "Value Added Services Group".
    // byte valueAddedServicesGroup = (byte)0xC0;
    //
    // mCspPlmnEnabled = true;
    // for (int i = 0; i < usedCspGroups; i++) {
    //      if (data[2 * i] == valueAddedServicesGroup) {
    //          log("[CSP] found ValueAddedServicesGroup, value " + data[(2 * i) + 1]);
    //          if ((data[(2 * i) + 1] & 0x80) == 0x80) {
    //              // Bit 8 is for
    //              // "Restriction of menu options for manual PLMN selection".
    //              // Operator Selection menu should be enabled.
    //              mCspPlmnEnabled = true;
    //          } else {
    //              mCspPlmnEnabled = false;
    //              // Operator Selection menu should be disabled.
    //              // Operator Selection Mode should be set to Automatic.
    //              log("[CSP] Set Automatic Network Selection");
    //              mNetworkSelectionModeAutomaticRegistrants.notifyRegistrants();
    //          }
    //          return;
    //      }
    // }
    //
    // log("[CSP] Value Added Service Group (0xC0), not found!");
    assert(0);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
