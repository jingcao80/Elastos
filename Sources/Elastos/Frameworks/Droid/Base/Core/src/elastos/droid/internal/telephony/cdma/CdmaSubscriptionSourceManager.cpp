
#include "elastos/droid/internal/telephony/cdma/CdmaSubscriptionSourceManager.h"
#include "elastos/droid/provider/Settings.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"

using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::Settings;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                    CdmaSubscriptionSourceManager
//=====================================================================
CAR_INTERFACE_IMPL(CdmaSubscriptionSourceManager, Handler, ICdmaSubscriptionSourceManager);

const String CdmaSubscriptionSourceManager::LOGTAG("CdmaSSM");
const Int32 CdmaSubscriptionSourceManager::EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED;
const Int32 CdmaSubscriptionSourceManager::EVENT_GET_CDMA_SUBSCRIPTION_SOURCE;
const Int32 CdmaSubscriptionSourceManager::EVENT_RADIO_ON;
const Int32 CdmaSubscriptionSourceManager::EVENT_SUBSCRIPTION_STATUS_CHANGED;
const Int32 CdmaSubscriptionSourceManager::SUBSCRIPTION_ACTIVATED;
AutoPtr<ICdmaSubscriptionSourceManager> CdmaSubscriptionSourceManager::sInstance;
AutoPtr<IObject> CdmaSubscriptionSourceManager::sReferenceCountMonitor = new Object();
Int32 CdmaSubscriptionSourceManager::sReferenceCount = 0;

AutoPtr<ICdmaSubscriptionSourceManager> CdmaSubscriptionSourceManager::GetInstance(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // synchronized (sReferenceCountMonitor) {
    //     if (null == sInstance) {
    //         sInstance = new CdmaSubscriptionSourceManager(context, ci);
    //     }
    //     CdmaSubscriptionSourceManager.sReferenceCount++;
    // }
    // sInstance.registerForCdmaSubscriptionSourceChanged(h, what, obj);
    // return sInstance;
    assert(0);
    AutoPtr<CdmaSubscriptionSourceManager> empty;
    return (ICdmaSubscriptionSourceManager*)empty->Probe(EIID_ICdmaSubscriptionSourceManager);
}

ECode CdmaSubscriptionSourceManager::Dispose(
    /* [in] */ IHandler* h)
{
    VALIDATE_NOT_NULL(h);
    // ==================before translated======================
    // mCdmaSubscriptionSourceChangedRegistrants.remove(h);
    // synchronized (sReferenceCountMonitor) {
    //     sReferenceCount--;
    //     if (sReferenceCount <= 0) {
    //         mCi.unregisterForCdmaSubscriptionChanged(this);
    //         mCi.unregisterForOn(this);
    //         mCi.unregisterForSubscriptionStatusChanged(this);
    //         sInstance = null;
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaSubscriptionSourceManager::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    // ==================before translated======================
    // AsyncResult ar;
    // switch (msg.what) {
    //     case EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED:
    //     case EVENT_GET_CDMA_SUBSCRIPTION_SOURCE:
    //     {
    //         log("CDMA_SUBSCRIPTION_SOURCE event = " + msg.what);
    //         ar = (AsyncResult) msg.obj;
    //         handleGetCdmaSubscriptionSource(ar);
    //     }
    //     break;
    //     case EVENT_RADIO_ON: {
    //         mCi.getCdmaSubscriptionSource(obtainMessage(EVENT_GET_CDMA_SUBSCRIPTION_SOURCE));
    //     }
    //     break;
    //     case EVENT_SUBSCRIPTION_STATUS_CHANGED: {
    //         log("EVENT_SUBSCRIPTION_STATUS_CHANGED");
    //         ar = (AsyncResult)msg.obj;
    //         if (ar.exception == null) {
    //             Int32 actStatus = ((Int32[])ar.result)[0];
    //             log("actStatus = " + actStatus);
    //             if (actStatus == SUBSCRIPTION_ACTIVATED) { // Subscription Activated
    //                 // In case of multi-SIM, framework should wait for the subscription ready
    //                 // to send any request to RIL.  Otherwise it will return failure.
    //                 Rlog.v(LOGTAG,"get Cdma Subscription Source");
    //                 mCi.getCdmaSubscriptionSource(
    //                         obtainMessage(EVENT_GET_CDMA_SUBSCRIPTION_SOURCE));
    //             }
    //         } else {
    //             logw("EVENT_SUBSCRIPTION_STATUS_CHANGED, Exception:" + ar.exception);
    //         }
    //     }
    //     break;
    //     default:
    //         super.handleMessage(msg);
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaSubscriptionSourceManager::GetCdmaSubscriptionSource(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCdmaSubscriptionSource.get();
    assert(0);
    return NOERROR;
}

Int32 CdmaSubscriptionSourceManager::GetDefault(
    /* [in] */ IContext* context)
{
    // Get the default value from the Settings
    Int32 subscriptionSource = 0;
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Settings::Global::GetInt32(cr, ISettingsGlobal::CDMA_SUBSCRIPTION_MODE
            , ICdmaSubscriptionSourceManager::PREFERRED_CDMA_SUBSCRIPTION, &subscriptionSource);
    return subscriptionSource;
}

CdmaSubscriptionSourceManager::CdmaSubscriptionSourceManager(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci)
{
    // ==================before translated======================
    // mContext = context;
    // mCi = ci;
    // mCi.registerForCdmaSubscriptionChanged(this, EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, null);
    // mCi.registerForOn(this, EVENT_RADIO_ON, null);
    // Int32 subscriptionSource = getDefault(context);
    // mCdmaSubscriptionSource.set(subscriptionSource);
    // mCi.registerForSubscriptionStatusChanged(this, EVENT_SUBSCRIPTION_STATUS_CHANGED, null);
}

void CdmaSubscriptionSourceManager::RegisterForCdmaSubscriptionSourceChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // Registrant r = new Registrant (h, what, obj);
    // mCdmaSubscriptionSourceChangedRegistrants.add(r);
    assert(0);
}

void CdmaSubscriptionSourceManager::HandleGetCdmaSubscriptionSource(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // if ((ar.exception == null) && (ar.result != null)) {
    //     Int32 newSubscriptionSource = ((Int32[]) ar.result)[0];
    //
    //     if (newSubscriptionSource != mCdmaSubscriptionSource.get()) {
    //         log("Subscription Source Changed : " + mCdmaSubscriptionSource + " >> "
    //                 + newSubscriptionSource);
    //         mCdmaSubscriptionSource.set(newSubscriptionSource);
    //
    //         // Notify registrants of the new CDMA subscription source
    //         mCdmaSubscriptionSourceChangedRegistrants.notifyRegistrants(new AsyncResult(null,
    //                 null, null));
    //     }
    // } else {
    //     // GET_CDMA_SUBSCRIPTION is returning Failure. Probably
    //     // because modem created GSM Phone. If modem created
    //     // GSMPhone, then PhoneProxy will trigger a change in
    //     // Phone objects and this object will be destroyed.
    //     logw("Unable to get CDMA Subscription Source, Exception: " + ar.exception
    //             + ", result: " + ar.result);
    // }
    assert(0);
}

void CdmaSubscriptionSourceManager::Log(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, s);
    assert(0);
}

void CdmaSubscriptionSourceManager::Logw(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Rlog.w(LOGTAG, s);
    assert(0);
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
