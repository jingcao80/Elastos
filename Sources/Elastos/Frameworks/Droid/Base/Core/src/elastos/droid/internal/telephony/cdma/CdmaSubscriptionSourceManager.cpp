
#include "elastos/droid/internal/telephony/cdma/CdmaSubscriptionSourceManager.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/os/CRegistrant.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::Settings;
using Elastos::Core::IArrayOf;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Logging::Logger;

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
Object CdmaSubscriptionSourceManager::sReferenceCountMonitor;
Int32 CdmaSubscriptionSourceManager::sReferenceCount = 0;

AutoPtr<ICdmaSubscriptionSourceManager> CdmaSubscriptionSourceManager::GetInstance(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    {
        AutoLock lock(sReferenceCountMonitor);
        if (NULL == sInstance) {
            sInstance = new CdmaSubscriptionSourceManager(context, ci);
        }
        CdmaSubscriptionSourceManager::sReferenceCount++;
    }
    ((CdmaSubscriptionSourceManager*)sInstance.Get())->RegisterForCdmaSubscriptionSourceChanged(h, what, obj);
    return sInstance;
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
    //         sInstance = NULL;
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaSubscriptionSourceManager::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    AsyncResult* ar = NULL;
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED:
        case EVENT_GET_CDMA_SUBSCRIPTION_SOURCE:
        {
            Log(String("CDMA_SUBSCRIPTION_SOURCE event = ") + StringUtils::ToString(what));
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            HandleGetCdmaSubscriptionSource(ar);
        }
        break;
        case EVENT_RADIO_ON: {
            AutoPtr<IMessage> m;
            ObtainMessage(EVENT_GET_CDMA_SUBSCRIPTION_SOURCE, (IMessage**)&m);
            mCi->GetCdmaSubscriptionSource(m);
        }
        break;
        case EVENT_SUBSCRIPTION_STATUS_CHANGED: {
            Log(String("EVENT_SUBSCRIPTION_STATUS_CHANGED"));
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException == NULL) {
                AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
                assert(array != NULL);
                AutoPtr<IInterface> o;
                array->Get(0, (IInterface**)&o);
                Int32 actStatus = 0;
                IInteger32::Probe(o)->GetValue(&actStatus);
                Log(String("actStatus = ") + StringUtils::ToString(actStatus));
                if (actStatus == SUBSCRIPTION_ACTIVATED) { // Subscription Activated
                    // In case of multi-SIM, framework should wait for the subscription ready
                    // to send any request to RIL.  Otherwise it will return failure.
                    Logger::V(LOGTAG,"get Cdma Subscription Source");
                    AutoPtr<IMessage> m;
                    ObtainMessage(EVENT_GET_CDMA_SUBSCRIPTION_SOURCE, (IMessage**)&m);
                    mCi->GetCdmaSubscriptionSource(m);
                }
            }
            else {
                Logw(String("EVENT_SUBSCRIPTION_STATUS_CHANGED, Exception:") + TO_CSTR(ar->mException));
            }
        }
        break;
        default: {
            Handler::HandleMessage(msg);
        }
    }
    return NOERROR;
}

ECode CdmaSubscriptionSourceManager::GetCdmaSubscriptionSource(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mCdmaSubscriptionSource->Get(result);
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
    Handler::constructor();

    mCdmaSubscriptionSourceChangedRegistrants = new RegistrantList();
    CAtomicInteger32::New(SUBSCRIPTION_FROM_NV, (IAtomicInteger32**)&mCdmaSubscriptionSource);

    mContext = context;
    mCi = ci;
    mCi->RegisterForCdmaSubscriptionChanged(this, EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, NULL);
    mCi->RegisterForOn(this, EVENT_RADIO_ON, NULL);
    Int32 subscriptionSource = GetDefault(context);
    mCdmaSubscriptionSource->Set(subscriptionSource);
    mCi->RegisterForSubscriptionStatusChanged(this, EVENT_SUBSCRIPTION_STATUS_CHANGED, NULL);
}

ECode CdmaSubscriptionSourceManager::ToString(
    /* [out] */ String* result)
{
    return Object::ToString(result);
}

void CdmaSubscriptionSourceManager::RegisterForCdmaSubscriptionSourceChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mCdmaSubscriptionSourceChangedRegistrants->Add(r);
}

void CdmaSubscriptionSourceManager::HandleGetCdmaSubscriptionSource(
    /* [in] */ AsyncResult* ar)
{
    if ((ar->mException == NULL) && (ar->mResult != NULL)) {
        AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
        AutoPtr<IInterface> obj;
        array->Get(0, (IInterface**)&obj);
        Int32 newSubscriptionSource = 0;
        IInteger32::Probe(obj)->GetValue(&newSubscriptionSource);

        Int32 v = 0;
        mCdmaSubscriptionSource->Get(&v);
        if (newSubscriptionSource != v) {
            Log(String("Subscription Source Changed : ") + TO_CSTR(mCdmaSubscriptionSource) + " >> "
                    + StringUtils::ToString(newSubscriptionSource));
            mCdmaSubscriptionSource->Set(newSubscriptionSource);

            // Notify registrants of the new CDMA subscription source
            AutoPtr<AsyncResult> ar = new AsyncResult(NULL, NULL, NULL);
            mCdmaSubscriptionSourceChangedRegistrants->NotifyRegistrants(ar);
        }
    }
    else {
        // GET_CDMA_SUBSCRIPTION is returning Failure. Probably
        // because modem created GSM Phone. If modem created
        // GSMPhone, then PhoneProxy will trigger a change in
        // Phone objects and this object will be destroyed.
        String e;
        if (ar->mException != NULL) {
            e = TO_CSTR(ar->mException);
        }
        String r;
        if (ar->mResult != NULL) {
            r = TO_CSTR(ar->mResult);
        }
        Logw(String("Unable to get CDMA Subscription Source, Exception: ") + e + ", result: " + r);
    }
}

void CdmaSubscriptionSourceManager::Log(
    /* [in] */ const String& s)
{
    Logger::D(LOGTAG, s);
}

void CdmaSubscriptionSourceManager::Logw(
    /* [in] */ const String& s)
{
    Logger::W(LOGTAG, s);
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
