#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccRecords.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                              IccRecords
//=====================================================================
CAR_INTERFACE_IMPL_2(IccRecords, Handler, IIccConstants, IIccRecords);

const Boolean IccRecords::DBG = TRUE;
const Int32 IccRecords::UNINITIALIZED;
const Int32 IccRecords::UNKNOWN;
const Int32 IccRecords::EVENT_SET_MSISDN_DONE;
const Int32 IccRecords::EVENT_GET_SMS_RECORD_SIZE_DONE;
const Int32 IccRecords::EVENT_APP_READY;
const Int32 IccRecords::EVENT_AKA_AUTHENTICATE_DONE;

IccRecords::IccRecords()
{
}

ECode IccRecords::constructor(
    /* [in] */ IUiccCardApplication* app,
    /* [in] */ IContext* c,
    /* [in] */ ICommandsInterface* ci)
{
    // ==================before translated======================
    // mContext = c;
    // mCi = ci;
    // mFh = app.getIccFileHandler();
    // mParentApp = app;
    // mOEMHookSimRefresh = mContext.getResources().getBoolean(
    //         com.android.internal.R.bool.config_sim_refresh_for_dual_mode_card);
    // if (mOEMHookSimRefresh) {
    //     mCi.registerForSimRefreshEvent(this, EVENT_REFRESH_OEM, null);
    // } else {
    //     mCi.registerForIccRefresh(this, EVENT_REFRESH, null);
    // }
    return NOERROR;
}

ECode IccRecords::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "mDestroyed=" + mDestroyed
    //         + " mContext=" + mContext
    //         + " mCi=" + mCi
    //         + " mFh=" + mFh
    //         + " mParentApp=" + mParentApp
    //         + " recordsLoadedRegistrants=" + mRecordsLoadedRegistrants
    //         + " mImsiReadyRegistrants=" + mImsiReadyRegistrants
    //         + " mRecordsEventsRegistrants=" + mRecordsEventsRegistrants
    //         + " mNewSmsRegistrants=" + mNewSmsRegistrants
    //         + " mNetworkSelectionModeAutomaticRegistrants="
    //                 + mNetworkSelectionModeAutomaticRegistrants
    //         + " recordsToLoad=" + mRecordsToLoad
    //         + " adnCache=" + mAdnCache
    //         + " recordsRequested=" + mRecordsRequested
    //         + " iccid=" + mIccId
    //         + " msisdn=" + mMsisdn
    //         + " msisdnTag=" + mMsisdnTag
    //         + " voiceMailNum=" + mVoiceMailNum
    //         + " voiceMailTag=" + mVoiceMailTag
    //         + " newVoiceMailNum=" + mNewVoiceMailNum
    //         + " newVoiceMailTag=" + mNewVoiceMailTag
    //         + " isVoiceMailFixed=" + mIsVoiceMailFixed
    //         + " mImsi=" + mImsi
    //         + " mncLength=" + mMncLength
    //         + " mailboxIndex=" + mMailboxIndex
    //         + " spn=" + mSpn;
    assert(0);
    return NOERROR;
}

ECode IccRecords::Dispose()
{
    // ==================before translated======================
    // mDestroyed.set(true);
    // if (mOEMHookSimRefresh) {
    //     mCi.unregisterForSimRefreshEvent(this);
    // } else {
    //     mCi.unregisterForIccRefresh(this);
    // }
    // mParentApp = null;
    // mFh = null;
    // mCi = null;
    // mContext = null;
    assert(0);
    return NOERROR;
}

ECode IccRecords::RecordsRequired()
{
    // ==================before translated======================
    // return;
    assert(0);
    return NOERROR;
}

ECode IccRecords::GetAdnCache(
    /* [out] */ IAdnRecordCache** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAdnCache;
    assert(0);
    return NOERROR;
}

ECode IccRecords::GetIccId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIccId;
    assert(0);
    return NOERROR;
}

ECode IccRecords::RegisterForRecordsLoaded(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // if (mDestroyed.get()) {
    //     return;
    // }
    //
    // for (int i = mRecordsLoadedRegistrants.size() - 1; i >= 0 ; i--) {
    //     Registrant  r = (Registrant) mRecordsLoadedRegistrants.get(i);
    //     Handler rH = r.getHandler();
    //
    //     if (rH != null && rH == h) {
    //         return;
    //     }
    // }
    // Registrant r = new Registrant(h, what, obj);
    // mRecordsLoadedRegistrants.add(r);
    //
    // if (mRecordsToLoad == 0 && mRecordsRequested == true) {
    //     r.notifyRegistrant(new AsyncResult(null, null, null));
    // }
    assert(0);
    return NOERROR;
}

ECode IccRecords::UnregisterForRecordsLoaded(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mRecordsLoadedRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode IccRecords::RegisterForImsiReady(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // if (mDestroyed.get()) {
    //     return;
    // }
    //
    // Registrant r = new Registrant(h, what, obj);
    // mImsiReadyRegistrants.add(r);
    //
    // if (mImsi != null) {
    //     r.notifyRegistrant(new AsyncResult(null, null, null));
    // }
    assert(0);
    return NOERROR;
}

ECode IccRecords::UnregisterForImsiReady(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mImsiReadyRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode IccRecords::RegisterForRecordsEvents(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // Registrant r = new Registrant (h, what, obj);
    // mRecordsEventsRegistrants.add(r);
    //
    // /* Notify registrant of all the possible events. This is to make sure registrant is
    // notified even if event occurred in the past. */
    // r.notifyResult(EVENT_MWI);
    // r.notifyResult(EVENT_CFI);
    assert(0);
    return NOERROR;
}

ECode IccRecords::UnregisterForRecordsEvents(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mRecordsEventsRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode IccRecords::RegisterForNewSms(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // Registrant r = new Registrant (h, what, obj);
    // mNewSmsRegistrants.add(r);
    assert(0);
    return NOERROR;
}

ECode IccRecords::UnregisterForNewSms(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mNewSmsRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode IccRecords::RegisterForNetworkSelectionModeAutomatic(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // Registrant r = new Registrant (h, what, obj);
    // mNetworkSelectionModeAutomaticRegistrants.add(r);
    assert(0);
    return NOERROR;
}

ECode IccRecords::UnregisterForNetworkSelectionModeAutomatic(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mNetworkSelectionModeAutomaticRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode IccRecords::GetIMSI(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode IccRecords::SetImsi(
    /* [in] */ const String& imsi)
{
    // ==================before translated======================
    // mImsi = imsi;
    // mImsiReadyRegistrants.notifyRegistrants();
    assert(0);
    return NOERROR;
}

ECode IccRecords::GetMsisdnNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMsisdn;
    assert(0);
    return NOERROR;
}

ECode IccRecords::GetGid1(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode IccRecords::SetMsisdnNumber(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    //
    // mMsisdn = number;
    // mMsisdnTag = alphaTag;
    //
    // if (DBG) log("Set MSISDN: " + mMsisdnTag +" " + mMsisdn);
    //
    //
    // AdnRecord adn = new AdnRecord(mMsisdnTag, mMsisdn);
    //
    // new AdnRecordLoader(mFh).updateEF(adn, EF_MSISDN, EF_EXT1, 1, null,
    //         obtainMessage(EVENT_SET_MSISDN_DONE, onComplete));
    assert(0);
    return NOERROR;
}

ECode IccRecords::GetMsisdnAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMsisdnTag;
    assert(0);
    return NOERROR;
}

ECode IccRecords::GetVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mVoiceMailNum;
    assert(0);
    return NOERROR;
}

ECode IccRecords::GetServiceProviderName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // String providerName = mSpn;
    //
    // // Check for null pointers, mParentApp can be null after dispose,
    // // which did occur after removing a SIM.
    // UiccCardApplication parentApp = mParentApp;
    // if (parentApp != null) {
    //     UiccCard card = parentApp.getUiccCard();
    //     if (card != null) {
    //         String brandOverride = card.getOperatorBrandOverride();
    //         if (brandOverride != null) {
    //             log("getServiceProviderName: override");
    //             providerName = brandOverride;
    //         } else {
    //             log("getServiceProviderName: no brandOverride");
    //         }
    //     } else {
    //         log("getServiceProviderName: card is null");
    //     }
    // } else {
    //     log("getServiceProviderName: mParentApp is null");
    // }
    // log("getServiceProviderName: providerName=" + providerName);
    // return providerName;
    assert(0);
    return NOERROR;
}

ECode IccRecords::GetVoiceMailAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mVoiceMailTag;
    assert(0);
    return NOERROR;
}

ECode IccRecords::GetRecordsLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mRecordsToLoad == 0 && mRecordsRequested == true) {
    //     return true;
    // } else {
    //     return false;
    // }
    assert(0);
    return NOERROR;
}

ECode IccRecords::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // AsyncResult ar;
    // switch (msg.what) {
    //     case EVENT_GET_ICC_RECORD_DONE:
    //         try {
    //             ar = (AsyncResult) msg.obj;
    //             IccRecordLoaded recordLoaded = (IccRecordLoaded) ar.userObj;
    //             if (DBG) log(recordLoaded.getEfName() + " LOADED");
    //
    //             if (ar.exception != null) {
    //                 loge("Record Load Exception: " + ar.exception);
    //             } else {
    //                 recordLoaded.onRecordLoaded(ar);
    //             }
    //         }catch (RuntimeException exc) {
    //             // I don't want these exceptions to be fatal
    //             loge("Exception parsing SIM record: " + exc);
    //         } finally {
    //             // Count up record load responses even if they are fails
    //             onRecordLoaded();
    //         }
    //         break;
    //     case EVENT_REFRESH:
    //         ar = (AsyncResult)msg.obj;
    //         if (DBG) log("Card REFRESH occurred: ");
    //         if (ar.exception == null) {
    //              broadcastRefresh();
    //              handleRefresh((IccRefreshResponse)ar.result);
    //         } else {
    //             loge("Icc refresh Exception: " + ar.exception);
    //         }
    //         break;
    //     case EVENT_REFRESH_OEM:
    //         ar = (AsyncResult)msg.obj;
    //         if (DBG) log("Card REFRESH OEM occurred: ");
    //         if (ar.exception == null) {
    //             handleRefreshOem((byte[])ar.result);
    //         } else {
    //             loge("Icc refresh OEM Exception: " + ar.exception);
    //         }
    //         break;
    //
    //     case EVENT_AKA_AUTHENTICATE_DONE:
    //         ar = (AsyncResult)msg.obj;
    //         auth_rsp = null;
    //         if (DBG) log("EVENT_AKA_AUTHENTICATE_DONE");
    //         if (ar.exception != null) {
    //             loge("Exception ICC SIM AKA: " + ar.exception);
    //         } else {
    //             try {
    //                 auth_rsp = (IccIoResult)ar.result;
    //                 if (DBG) log("ICC SIM AKA: auth_rsp = " + auth_rsp);
    //             } catch (Exception e) {
    //                 loge("Failed to parse ICC SIM AKA contents: " + e);
    //             }
    //         }
    //         synchronized (mLock) {
    //             mLock.notifyAll();
    //         }
    //
    //         break;
    //     case EVENT_GET_SMS_RECORD_SIZE_DONE:
    //         ar = (AsyncResult) msg.obj;
    //
    //         if (ar.exception != null) {
    //             loge("Exception in EVENT_GET_SMS_RECORD_SIZE_DONE " + ar.exception);
    //             break;
    //         }
    //
    //         int[] recordSize = (int[])ar.result;
    //         try {
    //             // recordSize[0]  is the record length
    //             // recordSize[1]  is the total length of the EF file
    //             // recordSize[2]  is the number of records in the EF file
    //             mSmsCountOnIcc = recordSize[2];
    //             log("EVENT_GET_SMS_RECORD_SIZE_DONE Size " + recordSize[0]
    //                     + " total " + recordSize[1]
    //                             + " record " + recordSize[2]);
    //         } catch (ArrayIndexOutOfBoundsException exc) {
    //             loge("ArrayIndexOutOfBoundsException in EVENT_GET_SMS_RECORD_SIZE_DONE: "
    //                     + exc.toString());
    //         }
    //         break;
    //
    //     default:
    //         super.handleMessage(msg);
    // }
    assert(0);
    return NOERROR;
}

ECode IccRecords::IsCspPlmnEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode IccRecords::GetOperatorNumeric(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode IccRecords::IsCallForwardStatusStored(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode IccRecords::GetVoiceCallForwardingFlag(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode IccRecords::SetVoiceCallForwardingFlag(
    /* [in] */ Int32 line,
    /* [in] */ Boolean enable,
    /* [in] */ const String& number)
{
    assert(0);
    return NOERROR;
}

ECode IccRecords::IsProvisioned(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return true;
    assert(0);
    return NOERROR;
}

ECode IccRecords::GetIsimRecords(
    /* [out] */ IIsimRecords** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode IccRecords::GetUsimServiceTable(
    /* [out] */ IUsimServiceTable** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode IccRecords::GetIccSimChallengeResponse(
    /* [in] */ Int32 authContext,
    /* [in] */ const String& data,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("getIccSimChallengeResponse:");
    //
    // try {
    //     synchronized(mLock) {
    //         CommandsInterface ci = mCi;
    //         UiccCardApplication parentApp = mParentApp;
    //         if (ci != null && parentApp != null) {
    //             ci.requestIccSimAuthentication(authContext, data,
    //                     parentApp.getAid(),
    //                     obtainMessage(EVENT_AKA_AUTHENTICATE_DONE));
    //             try {
    //                 mLock.wait();
    //             } catch (InterruptedException e) {
    //                 loge("getIccSimChallengeResponse: Fail, interrupted"
    //                         + " while trying to request Icc Sim Auth");
    //                 return null;
    //             }
    //         } else {
    //             loge( "getIccSimChallengeResponse: "
    //                     + "Fail, ci or parentApp is null");
    //             return null;
    //         }
    //     }
    // } catch(Exception e) {
    //     loge( "getIccSimChallengeResponse: "
    //             + "Fail while trying to request Icc Sim Auth");
    //     return null;
    // }
    //
    // if (DBG) log("getIccSimChallengeResponse: return auth_rsp");
    //
    // return android.util.Base64.encodeToString(auth_rsp.payload, android.util.Base64.NO_WRAP);
    assert(0);
    return NOERROR;
}

ECode IccRecords::GetSmsCapacityOnIcc(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("getSmsCapacityOnIcc: " + mSmsCountOnIcc);
    // return mSmsCountOnIcc;
    assert(0);
    return NOERROR;
}

ECode IccRecords::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    // ==================before translated======================
    // pw.println("IccRecords: " + this);
    // pw.println(" mDestroyed=" + mDestroyed);
    // pw.println(" mCi=" + mCi);
    // pw.println(" mFh=" + mFh);
    // pw.println(" mParentApp=" + mParentApp);
    // pw.println(" recordsLoadedRegistrants: size=" + mRecordsLoadedRegistrants.size());
    // for (int i = 0; i < mRecordsLoadedRegistrants.size(); i++) {
    //     pw.println("  recordsLoadedRegistrants[" + i + "]="
    //             + ((Registrant)mRecordsLoadedRegistrants.get(i)).getHandler());
    // }
    // pw.println(" mImsiReadyRegistrants: size=" + mImsiReadyRegistrants.size());
    // for (int i = 0; i < mImsiReadyRegistrants.size(); i++) {
    //     pw.println("  mImsiReadyRegistrants[" + i + "]="
    //             + ((Registrant)mImsiReadyRegistrants.get(i)).getHandler());
    // }
    // pw.println(" mRecordsEventsRegistrants: size=" + mRecordsEventsRegistrants.size());
    // for (int i = 0; i < mRecordsEventsRegistrants.size(); i++) {
    //     pw.println("  mRecordsEventsRegistrants[" + i + "]="
    //             + ((Registrant)mRecordsEventsRegistrants.get(i)).getHandler());
    // }
    // pw.println(" mNewSmsRegistrants: size=" + mNewSmsRegistrants.size());
    // for (int i = 0; i < mNewSmsRegistrants.size(); i++) {
    //     pw.println("  mNewSmsRegistrants[" + i + "]="
    //             + ((Registrant)mNewSmsRegistrants.get(i)).getHandler());
    // }
    // pw.println(" mNetworkSelectionModeAutomaticRegistrants: size="
    //         + mNetworkSelectionModeAutomaticRegistrants.size());
    // for (int i = 0; i < mNetworkSelectionModeAutomaticRegistrants.size(); i++) {
    //     pw.println("  mNetworkSelectionModeAutomaticRegistrants[" + i + "]="
    //             + ((Registrant)mNetworkSelectionModeAutomaticRegistrants.get(i)).getHandler());
    // }
    // pw.println(" mRecordsRequested=" + mRecordsRequested);
    // pw.println(" mRecordsToLoad=" + mRecordsToLoad);
    // pw.println(" mRdnCache=" + mAdnCache);
    // pw.println(" iccid=" + mIccId);
    // pw.println(" mMsisdn=" + mMsisdn);
    // pw.println(" mMsisdnTag=" + mMsisdnTag);
    // pw.println(" mVoiceMailNum=" + mVoiceMailNum);
    // pw.println(" mVoiceMailTag=" + mVoiceMailTag);
    // pw.println(" mNewVoiceMailNum=" + mNewVoiceMailNum);
    // pw.println(" mNewVoiceMailTag=" + mNewVoiceMailTag);
    // pw.println(" mIsVoiceMailFixed=" + mIsVoiceMailFixed);
    // pw.println(" mImsi=" + mImsi);
    // pw.println(" mMncLength=" + mMncLength);
    // pw.println(" mMailboxIndex=" + mMailboxIndex);
    // pw.println(" mSpn=" + mSpn);
    // pw.flush();
    assert(0);
    return NOERROR;
}

void IccRecords::SetServiceProviderName(
    /* [in] */ const String& spn)
{
    // ==================before translated======================
    // mSpn = spn;
    assert(0);
}

void IccRecords::OnIccRefreshInit()
{
    // ==================before translated======================
    // mAdnCache.reset();
    // UiccCardApplication parentApp = mParentApp;
    // if ((parentApp != null) &&
    //         (parentApp.getState() == AppState.APPSTATE_READY)) {
    //     // This will cause files to be reread
    //     sendMessage(obtainMessage(EVENT_APP_READY));
    // }
    assert(0);
}

ECode IccRecords::BroadcastRefresh()
{
    assert(0);
    return NOERROR;
}

void IccRecords::HandleRefresh(
    /* [in] */ IIccRefreshResponse* refreshResponse)
{
    // ==================before translated======================
    // if (refreshResponse == null) {
    //     if (DBG) log("handleRefresh received without input");
    //     return;
    // }
    //
    // if (refreshResponse.aid != null &&
    //         !refreshResponse.aid.equals(mParentApp.getAid())) {
    //     // This is for different app. Ignore.
    //     return;
    // }
    //
    // switch (refreshResponse.refreshResult) {
    //     case IccRefreshResponse.REFRESH_RESULT_FILE_UPDATE:
    //         if (DBG) log("handleRefresh with SIM_FILE_UPDATED");
    //         handleFileUpdate(refreshResponse.efId);
    //         break;
    //     case IccRefreshResponse.REFRESH_RESULT_INIT:
    //         if (DBG) log("handleRefresh with SIM_REFRESH_INIT");
    //         // need to reload all files (that we care about)
    //         if (mAdnCache != null) {
    //             mAdnCache.reset();
    //             //We will re-fetch the records when the app
    //             // goes back to the ready state. Nothing to do here.
    //         }
    //         break;
    //     case IccRefreshResponse.REFRESH_RESULT_RESET:
    //         if (DBG) log("handleRefresh with SIM_REFRESH_RESET");
    //         if (powerOffOnSimReset()) {
    //             mCi.setRadioPower(false, null);
    //             /* Note: no need to call setRadioPower(true).  Assuming the desired
    //             * radio power state is still ON (as tracked by ServiceStateTracker),
    //             * ServiceStateTracker will call setRadioPower when it receives the
    //             * RADIO_STATE_CHANGED notification for the power off.  And if the
    //             * desired power state has changed in the interim, we don't want to
    //             * override it with an unconditional power on.
    //             */
    //         } else {
    //             if(mAdnCache != null) {
    //                 mAdnCache.reset();
    //             }
    //             mRecordsRequested = false;
    //             mImsi = null;
    //         }
    //         //We will re-fetch the records when the app
    //         // goes back to the ready state. Nothing to do here.
    //         break;
    //     default:
    //         // unknown refresh operation
    //         if (DBG) log("handleRefresh with unknown operation");
    //         break;
    // }
    assert(0);
}

Boolean IccRecords::RequirePowerOffOnSimRefreshReset()
{
    // ==================before translated======================
    // return mContext.getResources().getBoolean(
    //     com.android.internal.R.bool.config_requireRadioPowerOffOnSimRefreshReset);
    assert(0);
    return FALSE;
}

Boolean IccRecords::PowerOffOnSimReset()
{
    // ==================before translated======================
    // return !mContext.getResources().getBoolean(
    //         com.android.internal.R.bool.skip_radio_power_off_on_sim_refresh_reset);
    assert(0);
    return FALSE;
}

void IccRecords::SetSystemProperty(
    /* [in] */ const String& property,
    /* [in] */ const String& value)
{
    // ==================before translated======================
    // if (mParentApp == null) return;
    // int slotId = mParentApp.getUiccCard().getSlotId();
    //
    // SubscriptionController subController = SubscriptionController.getInstance();
    // long subId = subController.getSubIdUsingSlotId(slotId)[0];
    //
    // TelephonyManager.setTelephonyProperty(property, subId, value);
    assert(0);
}

void IccRecords::HandleRefreshOem(
    /* [in] */ ArrayOf<Byte>* data)
{
    // ==================before translated======================
    // ByteBuffer payload = ByteBuffer.wrap(data);
    // IccRefreshResponse response = UiccController.parseOemSimRefresh(payload);
    //
    // IccCardApplicationStatus appStatus = new IccCardApplicationStatus();
    // AppType appType = appStatus.AppTypeFromRILInt(payload.getInt());
    // int slotId = (int)payload.get();
    // if ((appType != AppType.APPTYPE_UNKNOWN)
    //     && (appType != mParentApp.getType())) {
    //     // This is for different app. Ignore.
    //     return;
    // }
    //
    // broadcastRefresh();
    // handleRefresh(response);
    //
    // if (response.refreshResult == IccRefreshResponse.REFRESH_RESULT_FILE_UPDATE ||
    //     response.refreshResult == IccRefreshResponse.REFRESH_RESULT_INIT) {
    //     log("send broadcast org.codeaurora.intent.action.ACTION_SIM_REFRESH_UPDATE");
    //     Intent sendIntent = new Intent(
    //             "org.codeaurora.intent.action.ACTION_SIM_REFRESH_UPDATE");
    //     if (TelephonyManager.getDefault().isMultiSimEnabled()){
    //         sendIntent.putExtra(PhoneConstants.SLOT_KEY, slotId);
    //     }
    //     mContext.sendBroadcast(sendIntent, null);
    // }
    assert(0);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
