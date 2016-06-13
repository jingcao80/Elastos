#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IsimUiccRecords.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                  IsimUiccRecords::EfIsimImpiLoaded
//=====================================================================
CAR_INTERFACE_IMPL(IsimUiccRecords::EfIsimImpiLoaded, Object, IIccRecordLoaded);

String IsimUiccRecords::EfIsimImpiLoaded::GetEfName()
{
    // ==================before translated======================
    // return "EF_ISIM_IMPI";
    assert(0);
    return String("");
}

ECode IsimUiccRecords::EfIsimImpiLoaded::OnRecordLoaded(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // byte[] data = (byte[]) ar.result;
    // mIsimImpi = isimTlvToString(data);
    // if (DUMP_RECORDS) log("EF_IMPI=" + mIsimImpi);
    assert(0);
    return NOERROR;
}

//=====================================================================
//                  IsimUiccRecords::EfIsimImpuLoaded
//=====================================================================
CAR_INTERFACE_IMPL(IsimUiccRecords::EfIsimImpuLoaded, Object, IIccRecordLoaded);

String IsimUiccRecords::EfIsimImpuLoaded::GetEfName()
{
    // ==================before translated======================
    // return "EF_ISIM_IMPU";
    assert(0);
    return String("");
}

ECode IsimUiccRecords::EfIsimImpuLoaded::OnRecordLoaded(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // ArrayList<byte[]> impuList = (ArrayList<byte[]>) ar.result;
    // if (DBG) log("EF_IMPU record count: " + impuList.size());
    // mIsimImpu = new String[impuList.size()];
    // int i = 0;
    // for (byte[] identity : impuList) {
    //     String impu = isimTlvToString(identity);
    //     if (DUMP_RECORDS) log("EF_IMPU[" + i + "]=" + impu);
    //     mIsimImpu[i++] = impu;
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                 IsimUiccRecords::EfIsimDomainLoaded
//=====================================================================
CAR_INTERFACE_IMPL(IsimUiccRecords::EfIsimDomainLoaded, Object, IIccRecordLoaded);

String IsimUiccRecords::EfIsimDomainLoaded::GetEfName()
{
    // ==================before translated======================
    // return "EF_ISIM_DOMAIN";
    assert(0);
    return String("");
}

ECode IsimUiccRecords::EfIsimDomainLoaded::OnRecordLoaded(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // byte[] data = (byte[]) ar.result;
    // mIsimDomain = isimTlvToString(data);
    // if (DUMP_RECORDS) log("EF_DOMAIN=" + mIsimDomain);
    assert(0);
    return NOERROR;
}

//=====================================================================
//                   IsimUiccRecords::EfIsimIstLoaded
//=====================================================================
CAR_INTERFACE_IMPL(IsimUiccRecords::EfIsimIstLoaded, Object, IIccRecordLoaded);

String IsimUiccRecords::EfIsimIstLoaded::GetEfName()
{
    // ==================before translated======================
    // return "EF_ISIM_IST";
    assert(0);
    return String("");
}

ECode IsimUiccRecords::EfIsimIstLoaded::OnRecordLoaded(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // byte[] data = (byte[]) ar.result;
    // mIsimIst = IccUtils.bytesToHexString(data);
    // if (DUMP_RECORDS) log("EF_IST=" + mIsimIst);
    assert(0);
    return NOERROR;
}

//=====================================================================
//                  IsimUiccRecords::EfIsimPcscfLoaded
//=====================================================================
CAR_INTERFACE_IMPL(IsimUiccRecords::EfIsimPcscfLoaded, Object, IIccRecordLoaded);

String IsimUiccRecords::EfIsimPcscfLoaded::GetEfName()
{
    // ==================before translated======================
    // return "EF_ISIM_PCSCF";
    assert(0);
    return String("");
}

ECode IsimUiccRecords::EfIsimPcscfLoaded::OnRecordLoaded(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // ArrayList<byte[]> pcscflist = (ArrayList<byte[]>) ar.result;
    // if (DBG) log("EF_PCSCF record count: " + pcscflist.size());
    // mIsimPcscf = new String[pcscflist.size()];
    // int i = 0;
    // for (byte[] identity : pcscflist) {
    //     String pcscf = isimTlvToString(identity);
    //     if (DUMP_RECORDS) log("EF_PCSCF[" + i + "]=" + pcscf);
    //     mIsimPcscf[i++] = pcscf;
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                           IsimUiccRecords
//=====================================================================
CAR_INTERFACE_IMPL(IsimUiccRecords, IccRecords, IIsimRecords);

const String IsimUiccRecords::LOGTAG("IsimUiccRecords");
const Boolean IsimUiccRecords::DBG = TRUE;
const Boolean IsimUiccRecords::DUMP_RECORDS = TRUE;
const Int32 IsimUiccRecords::EVENT_APP_READY;
const Int32 IsimUiccRecords::EVENT_AKA_AUTHENTICATE_DONE;
const Int32 IsimUiccRecords::TAG_ISIM_VALUE;

IsimUiccRecords::IsimUiccRecords()
{
}

ECode IsimUiccRecords::constructor(
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
    // // Start off by setting empty state
    // resetRecords();
    //
    // mParentApp.registerForReady(this, EVENT_APP_READY, null);
    // if (DBG) log("IsimUiccRecords X ctor this=" + this);
    return NOERROR;
}

ECode IsimUiccRecords::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "IsimUiccRecords: " + super.toString()
    //         + " mIsimImpi=" + mIsimImpi
    //         + " mIsimDomain=" + mIsimDomain
    //         + " mIsimImpu=" + mIsimImpu
    //         + " mIsimIst=" + mIsimIst
    //         + " mIsimPcscf=" + mIsimPcscf;
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::Dispose()
{
    // ==================before translated======================
    // log("Disposing " + this);
    // //Unregister for all events
    // mParentApp.unregisterForReady(this);
    // resetRecords();
    // super.dispose();
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // AsyncResult ar;
    //
    // if (mDestroyed.get()) {
    //     Rlog.e(LOGTAG, "Received message " + msg +
    //             "[" + msg.what + "] while being destroyed. Ignoring.");
    //     return;
    // }
    // loge("IsimUiccRecords: handleMessage " + msg + "[" + msg.what + "] ");
    //
    // try {
    //     switch (msg.what) {
    //         case EVENT_APP_READY:
    //             onReady();
    //             break;
    //
    //         case EVENT_AKA_AUTHENTICATE_DONE:
    //             ar = (AsyncResult)msg.obj;
    //             log("EVENT_AKA_AUTHENTICATE_DONE");
    //             if (ar.exception != null) {
    //                 log("Exception ISIM AKA: " + ar.exception);
    //             } else {
    //                 try {
    //                     auth_rsp = (String)ar.result;
    //                     log("ISIM AKA: auth_rsp = " + auth_rsp);
    //                 } catch (Exception e) {
    //                     log("Failed to parse ISIM AKA contents: " + e);
    //                 }
    //             }
    //             synchronized (mLock) {
    //                 mLock.notifyAll();
    //             }
    //
    //             break;
    //
    //         default:
    //             super.handleMessage(msg);   // IccRecords handles generic record load responses
    //
    //     }
    // } catch (RuntimeException exc) {
    //     // I don't want these exceptions to be fatal
    //     Rlog.w(LOGTAG, "Exception parsing SIM record", exc);
    // }
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::GetIsimImpi(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIsimImpi;
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::GetIsimDomain(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIsimDomain;
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::GetIsimImpu(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (mIsimImpu != null) ? mIsimImpu.clone() : null;
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::GetIsimIst(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIsimIst;
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::GetIsimPcscf(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (mIsimPcscf != null) ? mIsimPcscf.clone() : null;
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::GetIsimChallengeResponse(
    /* [in] */ const String& nonce,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("getIsimChallengeResponse-nonce:"+nonce);
    // try {
    //     synchronized(mLock) {
    //         mCi.requestIsimAuthentication(nonce,obtainMessage(EVENT_AKA_AUTHENTICATE_DONE));
    //         try {
    //             mLock.wait();
    //         } catch (InterruptedException e) {
    //             log("interrupted while trying to request Isim Auth");
    //         }
    //     }
    // } catch(Exception e) {
    //     if (DBG) log( "Fail while trying to request Isim Auth");
    //     return null;
    // }
    //
    // if (DBG) log("getIsimChallengeResponse-auth_rsp"+auth_rsp);
    //
    // return auth_rsp;
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::GetDisplayRule(
    /* [in] */ const String& plmn,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Not applicable to Isim
    // return 0;
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::OnReady()
{
    // ==================before translated======================
    // fetchIsimRecords();
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::OnRefresh(
    /* [in] */ Boolean fileChanged,
    /* [in] */ ArrayOf<Int32>* fileList)
{
    // ==================before translated======================
    // if (fileChanged) {
    //     // A future optimization would be to inspect fileList and
    //     // only reload those files that we care about.  For now,
    //     // just re-fetch all SIM records that we cache.
    //     fetchIsimRecords();
    // }
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::SetVoiceMailNumber(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& voiceNumber,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // // Not applicable to Isim
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::SetVoiceMessageWaiting(
    /* [in] */ Int32 line,
    /* [in] */ Int32 countWaiting)
{
    // ==================before translated======================
    // // Not applicable to Isim
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    // ==================before translated======================
    // pw.println("IsimRecords: " + this);
    // pw.println(" extends:");
    // super.dump(fd, pw, args);
    // pw.println(" mIsimImpi=" + mIsimImpi);
    // pw.println(" mIsimDomain=" + mIsimDomain);
    // pw.println(" mIsimImpu[]=" + Arrays.toString(mIsimImpu));
    // pw.println(" mIsimIst" + mIsimIst);
    // pw.println(" mIsimPcscf"+mIsimPcscf);
    // pw.flush();
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::GetVoiceMessageCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return 0; // Not applicable to Isim
    assert(0);
    return NOERROR;
}

void IsimUiccRecords::FetchIsimRecords()
{
    // ==================before translated======================
    // mRecordsRequested = true;
    //
    // mFh.loadEFTransparent(EF_IMPI, obtainMessage(
    //         IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimImpiLoaded()));
    // mRecordsToLoad++;
    //
    // mFh.loadEFLinearFixedAll(EF_IMPU, obtainMessage(
    //         IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimImpuLoaded()));
    // mRecordsToLoad++;
    //
    // mFh.loadEFTransparent(EF_DOMAIN, obtainMessage(
    //         IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimDomainLoaded()));
    // mRecordsToLoad++;
    // mFh.loadEFTransparent(EF_IST, obtainMessage(
    //             IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimIstLoaded()));
    // mRecordsToLoad++;
    // mFh.loadEFLinearFixedAll(EF_PCSCF, obtainMessage(
    //             IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimPcscfLoaded()));
    // mRecordsToLoad++;
    //
    // if (DBG) log("fetchIsimRecords " + mRecordsToLoad + " requested: " + mRecordsRequested);
    assert(0);
}

void IsimUiccRecords::ResetRecords()
{
    // ==================before translated======================
    // // recordsRequested is set to false indicating that the SIM
    // // read requests made so far are not valid. This is set to
    // // true only when fresh set of read requests are made.
    // mIsimImpi = null;
    // mIsimDomain = null;
    // mIsimImpu = null;
    // mIsimIst = null;
    // mIsimPcscf = null;
    // auth_rsp = null;
    //
    // mRecordsRequested = false;
    assert(0);
}

ECode IsimUiccRecords::OnRecordLoaded()
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

ECode IsimUiccRecords::OnAllRecordsLoaded()
{
    // ==================before translated======================
    // if (DBG) log("record load complete");
    //  mRecordsLoadedRegistrants.notifyRegistrants(
    //          new AsyncResult(null, null, null));
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::HandleFileUpdate(
    /* [in] */ Int32 efid)
{
    // ==================before translated======================
    // switch (efid) {
    //     case EF_IMPI:
    //         mFh.loadEFTransparent(EF_IMPI, obtainMessage(
    //                     IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimImpiLoaded()));
    //         mRecordsToLoad++;
    //         break;
    //
    //     case EF_IMPU:
    //         mFh.loadEFLinearFixedAll(EF_IMPU, obtainMessage(
    //                     IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimImpuLoaded()));
    //         mRecordsToLoad++;
    //     break;
    //
    //     case EF_DOMAIN:
    //         mFh.loadEFTransparent(EF_DOMAIN, obtainMessage(
    //                     IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimDomainLoaded()));
    //         mRecordsToLoad++;
    //     break;
    //
    //     case EF_IST:
    //         mFh.loadEFTransparent(EF_IST, obtainMessage(
    //                     IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimIstLoaded()));
    //         mRecordsToLoad++;
    //     break;
    //
    //     case EF_PCSCF:
    //         mFh.loadEFLinearFixedAll(EF_PCSCF, obtainMessage(
    //                     IccRecords.EVENT_GET_ICC_RECORD_DONE, new EfIsimPcscfLoaded()));
    //         mRecordsToLoad++;
    //
    //     default:
    //         fetchIsimRecords();
    //         break;
    // }
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::BroadcastRefresh()
{
    // ==================before translated======================
    // Intent intent = new Intent(INTENT_ISIM_REFRESH);
    // log("send ISim REFRESH: " + INTENT_ISIM_REFRESH);
    // mContext.sendBroadcast(intent);
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::Log(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // if (DBG) Rlog.d(LOGTAG, "[ISIM] " + s);
    assert(0);
    return NOERROR;
}

ECode IsimUiccRecords::Loge(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // if (DBG) Rlog.e(LOGTAG, "[ISIM] " + s);
    assert(0);
    return NOERROR;
}

String IsimUiccRecords::IsimTlvToString(
    /* [in] */ ArrayOf<Byte>* record)
{
    // ==================before translated======================
    // SimTlv tlv = new SimTlv(record, 0, record.length);
    // do {
    //     if (tlv.getTag() == TAG_ISIM_VALUE) {
    //         return new String(tlv.getData(), Charset.forName("UTF-8"));
    //     }
    // } while (tlv.nextObject());
    //
    // Rlog.e(LOGTAG, "[ISIM] can't find TLV tag in ISIM record, returning null");
    // return null;
    assert(0);
    return String("");
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
