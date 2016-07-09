
#include "elastos/droid/internal/telephony/uicc/UiccController.h"
#include "elastos/droid/internal/telephony/uicc/CUiccController.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/os/SystemProperties.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                            UiccController
//=====================================================================
CAR_INTERFACE_IMPL(UiccController, Handler, IUiccController);

const Boolean UiccController::DBG = TRUE;
const String UiccController::LOGTAG("UiccController");
const Int32 UiccController::EVENT_ICC_STATUS_CHANGED;
const Int32 UiccController::EVENT_GET_ICC_STATUS_DONE;
const Int32 UiccController::EVENT_RADIO_UNAVAILABLE;
const Int32 UiccController::EVENT_REFRESH;
const Int32 UiccController::EVENT_REFRESH_OEM;
Object UiccController::mLock;
AutoPtr<IUiccController> UiccController::mInstance;

UiccController::UiccController()
{
}

ECode UiccController::constructor(
    /* [in] */ IContext* c,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci)
{
    Handler::constructor();

    if (DBG) Log(String("Creating UiccController"));
    mContext = c;
    mCis = ci;

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetBoolean(Elastos::Droid::R::bool_::config_sim_refresh_for_dual_mode_card, &mOEMHookSimRefresh);
    Boolean tmp = FALSE;
    for (Int32 i = 0; i < mCis->GetLength(); i++) {
        if ((*mCis)[i] == NULL) continue;

        AutoPtr<IInteger32> index;
        CInteger32::New(i, (IInteger32**)&index);
        if (SystemProperties::GetBoolean(String("persist.radio.apm_sim_not_pwdn"), FALSE, &tmp), tmp) {
            // Reading ICC status in airplane mode is only supported in QCOM
            // RILs when this property is set to true
            (*mCis)[i]->RegisterForAvailable(this, EVENT_ICC_STATUS_CHANGED, index);
        }
        else {
            (*mCis)[i]->RegisterForOn(this, EVENT_ICC_STATUS_CHANGED, index);
        }

        (*mCis)[i]->RegisterForIccStatusChanged(this, EVENT_ICC_STATUS_CHANGED, index);
        // TODO remove this once modem correctly notifies the unsols
        (*mCis)[i]->RegisterForNotAvailable(this, EVENT_RADIO_UNAVAILABLE, index);
        if (mOEMHookSimRefresh) {
            (*mCis)[i]->RegisterForSimRefreshEvent(this, EVENT_REFRESH_OEM, index);
        }
        else {
            (*mCis)[i]->RegisterForIccRefresh(this, EVENT_REFRESH, index);
        }
    }
    return NOERROR;
}

AutoPtr<IUiccController> UiccController::Make(
    /* [in] */ IContext* c,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci)
{
    AutoLock lock(mLock);
    if (mInstance != NULL) {
        // throw new RuntimeException("MSimUiccController.make() should only be called once");
        assert(0 && "MSimUiccController.make() should only be called once");
    }
    CUiccController::New(c, ci, (IUiccController**)&mInstance);
    return mInstance;
}

AutoPtr<IUiccController> UiccController::GetInstance()
{
    AutoLock lock(mLock);
    if (mInstance == NULL) {
        // throw new RuntimeException(
        //         "UiccController.getInstance can't be called before make()");
        assert(0 && "UiccController.getInstance can't be called before make()");
    }
    return mInstance;
}

ECode UiccController::GetUiccCard(
    /* [out] */ IUiccCard** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getUiccCard(SubscriptionController.getInstance().getPhoneId(SubscriptionController.getInstance().getDefaultSubId()));
    assert(0);
    return NOERROR;
}

ECode UiccController::GetUiccCard(
    /* [in] */ Int32 slotId,
    /* [out] */ IUiccCard** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (isValidCardIndex(slotId)) {
    //         return mUiccCards[slotId];
    //     }
    //     return null;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccController::GetUiccCards(
    /* [out] */ ArrayOf<IUiccCard*>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Return cloned array since we don't want to give out reference
    // // to internal data structure.
    // synchronized (mLock) {
    //     return mUiccCards.clone();
    // }
    assert(0);
    return NOERROR;
}

ECode UiccController::GetUiccCardApplication(
    /* [in] */ Int32 family,
    /* [out] */ IUiccCardApplication** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getUiccCardApplication(SubscriptionController.getInstance().getPhoneId(
    //         SubscriptionController.getInstance().getDefaultSubId()), family);
    assert(0);
    return NOERROR;
}

ECode UiccController::GetIccRecords(
    /* [in] */ Int32 slotId,
    /* [in] */ Int32 family,
    /* [out] */ IIccRecords** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     UiccCardApplication app = getUiccCardApplication(slotId, family);
    //     if (app != null) {
    //         return app.getIccRecords();
    //     }
    //     return null;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccController::GetIccFileHandler(
    /* [in] */ Int32 slotId,
    /* [in] */ Int32 family,
    /* [out] */ IIccFileHandler** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     UiccCardApplication app = getUiccCardApplication(slotId, family);
    //     if (app != null) {
    //         return app.getIccFileHandler();
    //     }
    //     return null;
    // }
    assert(0);
    return NOERROR;
}

Int32 UiccController::GetFamilyFromRadioTechnology(
    /* [in] */ Int32 radioTechnology)
{
    // ==================before translated======================
    // if (ServiceState.isGsm(radioTechnology) ||
    //         radioTechnology == ServiceState.RIL_RADIO_TECHNOLOGY_EHRPD) {
    //     return  UiccController.APP_FAM_3GPP;
    // } else if (ServiceState.isCdma(radioTechnology)) {
    //     return  UiccController.APP_FAM_3GPP2;
    // } else {
    //     // If it is UNKNOWN rat
    //     return UiccController.APP_FAM_UNKNOWN;
    // }
    assert(0);
    return 0;
}

ECode UiccController::RegisterForIccChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    {
        AutoLock lock(mLock);
        AutoPtr<IRegistrant> r;
        CRegistrant::New(h, what, obj, (IRegistrant**)&r);
        mIccChangedRegistrants->Add(r);
        //Notify registrant right after registering, so that it will get the latest ICC status,
        //otherwise which may not happen until there is an actual change in ICC status.
        r->NotifyRegistrant();
    }
    return NOERROR;
}

ECode UiccController::UnregisterForIccChanged(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     mIccChangedRegistrants.remove(h);
    // }
    assert(0);
    return NOERROR;
}

ECode UiccController::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Logger::D("UiccController", "[TODO] ======== HandleMessage ");
    // ==================before translated======================
    // synchronized (mLock) {
    //     Integer index = getCiIndex(msg);
    //
    //     if (index < 0 || index >= mCis.length) {
    //         Rlog.e(LOGTAG, "Invalid index : " + index + " received with event " + msg.what);
    //         return;
    //     }
    //
    //     switch (msg.what) {
    //         case EVENT_ICC_STATUS_CHANGED:
    //             if (DBG) log("Received EVENT_ICC_STATUS_CHANGED, calling getIccCardStatus");
    //             mCis[index].getIccCardStatus(obtainMessage(EVENT_GET_ICC_STATUS_DONE, index));
    //             break;
    //         case EVENT_GET_ICC_STATUS_DONE:
    //             if (DBG) log("Received EVENT_GET_ICC_STATUS_DONE");
    //             AsyncResult ar = (AsyncResult)msg.obj;
    //             onGetIccCardStatusDone(ar, index);
    //             break;
    //         case EVENT_RADIO_UNAVAILABLE:
    //             if (DBG) log("EVENT_RADIO_UNAVAILABLE, dispose card");
    //             if (mUiccCards[index] != null) {
    //                 mUiccCards[index].dispose();
    //             }
    //             mUiccCards[index] = null;
    //             mIccChangedRegistrants.notifyRegistrants(new AsyncResult(null, index, null));
    //             break;
    //         case EVENT_REFRESH:
    //             ar = (AsyncResult)msg.obj;
    //             if (DBG) log("Sim REFRESH received");
    //             if (ar.exception == null) {
    //                 handleRefresh((IccRefreshResponse)ar.result, index);
    //             } else {
    //                 log ("Exception on refresh " + ar.exception);
    //             }
    //             break;
    //         case EVENT_REFRESH_OEM:
    //             ar = (AsyncResult)msg.obj;
    //             if (DBG) log("Sim REFRESH OEM received");
    //             if (ar.exception == null) {
    //                 ByteBuffer payload = ByteBuffer.wrap((byte[])ar.result);
    //                 handleRefresh(parseOemSimRefresh(payload), index);
    //             } else {
    //                 log ("Exception on refresh " + ar.exception);
    //             }
    //             break;
    //         default:
    //             Rlog.e(LOGTAG, " Unknown Event " + msg.what);
    //     }
    // }
    return NOERROR;
}

AutoPtr<IIccRefreshResponse> UiccController::ParseOemSimRefresh(
    /* [in] */ IByteBuffer* payload)
{
    // ==================before translated======================
    // IccRefreshResponse response = new IccRefreshResponse();
    // /* AID maximum size */
    // final int QHOOK_MAX_AID_SIZE = 20*2+1+3;
    //
    // /* parse from payload */
    // payload.order(ByteOrder.nativeOrder());
    //
    // response.refreshResult = payload.getInt();
    // response.efId  = payload.getInt();
    // int aidLen = payload.getInt();
    // byte[] aid = new byte[QHOOK_MAX_AID_SIZE];
    // payload.get(aid, 0, QHOOK_MAX_AID_SIZE);
    // //Read the aid string with QHOOK_MAX_AID_SIZE from payload at first because need
    // //corresponding to the aid array length sent from qcril and need parse the payload
    // //to get app type and sub id in IccRecords.java after called this method.
    // response.aid = (aidLen == 0) ? null : (new String(aid)).substring(0, aidLen);
    //
    // if (DBG){
    //     Rlog.d(LOGTAG, "refresh SIM card " + ", refresh result:" + response.refreshResult
    //             + ", ef Id:" + response.efId + ", aid:" + response.aid);
    // }
    // return response;
    assert(0);
    AutoPtr<IIccRefreshResponse> empty;
    return empty;
}

ECode UiccController::GetUiccCardApplication(
    /* [in] */ Int32 slotId,
    /* [in] */ Int32 family,
    /* [out] */ IUiccCardApplication** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (isValidCardIndex(slotId)) {
    //         UiccCard c = mUiccCards[slotId];
    //         if (c != null) {
    //             return mUiccCards[slotId].getApplication(family);
    //         }
    //     }
    //     return null;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccController::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    // ==================before translated======================
    //         pw.println("UiccController: " + this);
    //         pw.println(" mContext=" + mContext);
    //         pw.println(" mInstance=" + mInstance);
    // //        pw.println(" mCi=" + mCi);
    // //        pw.println(" mUiccCard=" + mUiccCard);
    //         pw.println(" mIccChangedRegistrants: size=" + mIccChangedRegistrants.size());
    //         for (int i = 0; i < mIccChangedRegistrants.size(); i++) {
    //             pw.println("  mIccChangedRegistrants[" + i + "]="
    //                     + ((Registrant)mIccChangedRegistrants.get(i)).getHandler());
    //         }
    //         pw.println();
    //         pw.flush();
    // //        for (int i = 0; i < mUiccCards.length; i++) {
    // //            mUiccCards[i].dump(fd, pw, args);
    // //        }
    assert(0);
    return NOERROR;
}

AutoPtr<ArrayOf<IUiccCard*> > UiccController::MiddleInitMuicccards()
{
    // ==================before translated======================
    // UiccCard[] result = new UiccCard[TelephonyManager.getDefault().getPhoneCount()];
    assert(0);
    AutoPtr<ArrayOf<IUiccCard*> > empty;
    return empty;
}

AutoPtr<IInteger32> UiccController::GetCiIndex(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // AsyncResult ar;
    // Integer index = new Integer(PhoneConstants.DEFAULT_CARD_INDEX);
    //
    // /*
    //  * The events can be come in two ways. By explicitly sending it using
    //  * sendMessage, in this case the user object passed is msg.obj and from
    //  * the CommandsInterface, in this case the user object is msg.obj.userObj
    //  */
    // if (msg != null) {
    //     if (msg.obj != null && msg.obj instanceof Integer) {
    //         index = (Integer)msg.obj;
    //     } else if(msg.obj != null && msg.obj instanceof AsyncResult) {
    //         ar = (AsyncResult)msg.obj;
    //         if (ar.userObj != null && ar.userObj instanceof Integer) {
    //             index = (Integer)ar.userObj;
    //         }
    //     }
    // }
    // return index;
    assert(0);
    AutoPtr<IInteger32> empty;
    return empty;
}

void UiccController::HandleRefresh(
    /* [in] */ IIccRefreshResponse* refreshResponse,
    /* [in] */ Int32 index)
{
    // ==================before translated======================
    // if (refreshResponse == null) {
    //     log("handleRefresh received without input");
    //     return;
    // }
    //
    // // Let the card know right away that a refresh has occurred
    // if (mUiccCards[index] != null) {
    //     mUiccCards[index].onRefresh(refreshResponse);
    // }
    // // The card status could have changed. Get the latest state
    // mCis[index].getIccCardStatus(obtainMessage(EVENT_GET_ICC_STATUS_DONE));
    assert(0);
}

// synchronized
void UiccController::OnGetIccCardStatusDone(
    /* [in] */ AsyncResult* ar,
    /* [in] */ IInteger32* index)
{
    // ==================before translated======================
    //         if (ar.exception != null) {
    //             Rlog.e(LOGTAG,"Error getting ICC status. "
    //                     + "RIL_REQUEST_GET_ICC_STATUS should "
    //                     + "never return an error", ar.exception);
    //             return;
    //         }
    //         if (!isValidCardIndex(index)) {
    //             Rlog.e(LOGTAG,"onGetIccCardStatusDone: invalid index : " + index);
    //             return;
    //         }
    //
    //         IccCardStatus status = (IccCardStatus)ar.result;
    //
    //         if (mUiccCards[index] == null) {
    //             //Create new card
    //             mUiccCards[index] = new UiccCard(mContext, mCis[index], status, index);
    //
    // /*
    //             // Update the UiccCard in base class, so that if someone calls
    //             // UiccManager.getUiccCard(), it will return the default card.
    //             if (index == PhoneConstants.DEFAULT_CARD_INDEX) {
    //                 mUiccCard = mUiccCards[index];
    //             }
    // */
    //         } else {
    //             //Update already existing card
    //             mUiccCards[index].update(mContext, mCis[index] , status);
    //         }
    //
    //         if (DBG) log("Notifying IccChangedRegistrants");
    //         mIccChangedRegistrants.notifyRegistrants(new AsyncResult(null, index, null));
    assert(0);
}

Boolean UiccController::IsValidCardIndex(
    /* [in] */ Int32 index)
{
    // ==================before translated======================
    // return (index >= 0 && index < mUiccCards.length);
    assert(0);
    return FALSE;
}

void UiccController::Log(
    /* [in] */ const String& string)
{
    Logger::D(LOGTAG, string);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
