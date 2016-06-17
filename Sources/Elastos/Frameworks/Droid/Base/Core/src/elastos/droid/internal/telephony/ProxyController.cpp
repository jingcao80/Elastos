
#include "elastos/droid/internal/telephony/ProxyController.h"
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/telephony/CTelephonyManagerHelper.h"

using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IPhoneProxy;
using Elastos::Droid::Internal::Telephony::DataConnection::IDctControllerHelper;
// using Elastos::Droid::Internal::Telephony::DataConnection::CDctControllerHelper;

using Elastos::Core::IThread;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String ProxyController::LOGTAG("ProxyController");

AutoPtr<IProxyController> ProxyController::sProxyController;

AutoPtr<IDctController> ProxyController::mDctController;

CAR_INTERFACE_IMPL(ProxyController, Object, IProxyController)

ECode ProxyController::GetInstance(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<IPhone*>* phoneProxy,
    /* [in] */ IUiccController* uiccController,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci,
    /* [out] */ IProxyController** result)
{
    VALIDATE_NOT_NULL(result)
    if (sProxyController == NULL) {
        assert(0 && "TODO");
        // sProxyController = new ProxyController(context, phoneProxy, uiccController, ci);
    }
    *result = sProxyController;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ProxyController::GetInstance(
    /* [out] */ IProxyController** result)
{
    VALIDATE_NOT_NULL(result)
    *result = sProxyController;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ProxyController::ProxyController(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<IPhone*>* phoneProxy,
    /* [in] */ IUiccController* uiccController,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci)
{
    Logd(String("Constructor - Enter"));

    mContext = context;
    mProxyPhones = phoneProxy;
    mUiccController = uiccController;
    mCi = ci;

    AutoPtr<IHandlerThread> t;
    CHandlerThread::New(String("DctControllerThread"), (IHandlerThread**)&t);
    IThread::Probe(t)->Start();

    AutoPtr<IDctControllerHelper> hlp;
    assert(0 && "TODO");
    // CDctControllerHelper::AcquireSingleton((IDctControllerHelper**)&hlp);
    AutoPtr<ILooper> lp;
    t->GetLooper((ILooper**)&lp);
    hlp->MakeDctController((ArrayOf<IPhoneProxy*>*)phoneProxy, lp, (IDctController**)&mDctController);
    assert(0 && "TODO");
    // mUiccPhoneBookController = new UiccPhoneBookController(mProxyPhones);
    // mPhoneSubInfoController = new PhoneSubInfoController(mProxyPhones);
    // mUiccSmsController = new UiccSmsController(mProxyPhones, context);
    // mSubscriptionManager = SubscriptionManager->GetInstance(context, uiccController, ci);

    Logd(String("Constructor - Exit"));
}

ECode ProxyController::UpdateDataConnectionTracker(
    /* [in] */ Int32 sub)
{
    AutoPtr<IPhone> index = (*mProxyPhones)[sub];
    IPhoneProxy::Probe(index)->UpdateDataConnectionTracker();
    return NOERROR;
}

ECode ProxyController::EnableDataConnectivity(
    /* [in] */ Int32 sub)
{
    AutoPtr<IPhone> index = (*mProxyPhones)[sub];
    IPhoneProxy::Probe(index)->SetInternalDataEnabled(TRUE);
    return NOERROR;
}

ECode ProxyController::DisableDataConnectivity(
    /* [in] */ Int32 sub,
    /* [in] */ IMessage* dataCleanedUpMsg)
{
    AutoPtr<IPhone> index = (*mProxyPhones)[sub];
    IPhoneProxy::Probe(index)->SetInternalDataEnabled(FALSE, dataCleanedUpMsg);
    return NOERROR;
}

ECode ProxyController::UpdateCurrentCarrierInProvider(
    /* [in] */ Int32 sub)
{
    AutoPtr<IPhone> index = (*mProxyPhones)[sub];
    Boolean result = FALSE;
    IPhoneProxy::Probe(index)->UpdateCurrentCarrierInProvider(&result);
    return NOERROR;
}

ECode ProxyController::RegisterForAllDataDisconnected(
    /* [in] */ Int64 subId,
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<ISubscriptionControllerHelper> hlp;
    assert(0 && "TODO");
    // CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlp);
    AutoPtr<ISubscriptionController> sc;
    hlp->GetInstance((ISubscriptionController**)&sc);
    Int32 phoneId = 0;
    IISub::Probe(sc)->GetPhoneId(subId, &phoneId);

    AutoPtr<ITelephonyManagerHelper> tmhlp;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmhlp);
    AutoPtr<ITelephonyManager> tm;
    tmhlp->GetDefault((ITelephonyManager**)&tm);
    Int32 phoneCount = 0;
    tm->GetPhoneCount(&phoneCount);
    if (phoneId >= 0 && phoneId < phoneCount) {
        AutoPtr<IPhone> index = (*mProxyPhones)[phoneId];
        IPhoneProxy::Probe(index)->RegisterForAllDataDisconnected(h, what, obj);
    }
    return NOERROR;
}

ECode ProxyController::UnregisterForAllDataDisconnected(
    /* [in] */ Int64 subId,
    /* [in] */ IHandler* h)
{
    AutoPtr<ISubscriptionControllerHelper> hlp;
    assert(0 && "TODO");
    // CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlp);
    AutoPtr<ISubscriptionController> sc;
    hlp->GetInstance((ISubscriptionController**)&sc);
    Int32 phoneId = 0;
    IISub::Probe(sc)->GetPhoneId(subId, &phoneId);

    AutoPtr<ITelephonyManagerHelper> tmhlp;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmhlp);
    AutoPtr<ITelephonyManager> tm;
    tmhlp->GetDefault((ITelephonyManager**)&tm);
    Int32 phoneCount = 0;
    tm->GetPhoneCount(&phoneCount);
    if (phoneId >= 0 && phoneId < phoneCount) {
        AutoPtr<IPhone> index = (*mProxyPhones)[phoneId];
        IPhoneProxy::Probe(index)->UnregisterForAllDataDisconnected(h);
    }
    return NOERROR;
}

ECode ProxyController::IsDataDisconnected(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISubscriptionControllerHelper> hlp;
    assert(0 && "TODO");
    // CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlp);
    AutoPtr<ISubscriptionController> sc;
    hlp->GetInstance((ISubscriptionController**)&sc);
    Int32 phoneId = 0;
    IISub::Probe(sc)->GetPhoneId(subId, &phoneId);

    AutoPtr<ITelephonyManagerHelper> tmhlp;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmhlp);
    AutoPtr<ITelephonyManager> tm;
    tmhlp->GetDefault((ITelephonyManager**)&tm);
    Int32 phoneCount = 0;
    tm->GetPhoneCount(&phoneCount);
    if (phoneId >= 0 && phoneId < phoneCount) {
        AutoPtr<IPhone> index = (*mProxyPhones)[phoneId];
        AutoPtr<IPhone> activePhone;
        IPhoneProxy::Probe(index)->GetActivePhone((IPhone**)&activePhone);
        assert(0 && "TODO");
        // return IPhoneBase::Probe(activePhone)->mDcTracker->IsDisconnected(result);
        return NOERROR;
    }
    else {
        *result = FALSE;
        return NOERROR;
    }
}

void ProxyController::Logd(
    /* [in] */ String string)
{
    assert(0 && "TODO");
    // Rlog::D(LOGTAG, string);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
