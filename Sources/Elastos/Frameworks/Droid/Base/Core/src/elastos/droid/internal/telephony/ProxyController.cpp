
#include "elastos/droid/internal/telephony/ProxyController.h"
#include "elastos/droid/internal/telephony/CUiccPhoneBookController.h"
#include "elastos/droid/internal/telephony/CPhoneSubInfoController.h"
#include "elastos/droid/internal/telephony/CUiccSmsController.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/internal/telephony/SubscriptionController.h"
#include "elastos/droid/internal/telephony/dataconnection/DctController.h"
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/telephony/CTelephonyManagerHelper.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IPhoneProxy;
using Elastos::Droid::Internal::Telephony::DataConnection::DctController;
using Elastos::Core::IThread;
using Elastos::Utility::Logging::Logger;

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
        sProxyController = new ProxyController(context, phoneProxy, uiccController, ci);
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

    AutoPtr<ILooper> lp;
    t->GetLooper((ILooper**)&lp);
    DctController::MakeDctController((ArrayOf<IPhoneProxy*>*)phoneProxy, lp, (IDctController**)&mDctController);
    CUiccPhoneBookController::New(mProxyPhones, (IUiccPhoneBookController**)&mUiccPhoneBookController);
    CPhoneSubInfoController::New(mProxyPhones, (IPhoneSubInfoController**)&mPhoneSubInfoController);
    CUiccSmsController::New(mProxyPhones, context, (IUiccSmsController**)&mUiccSmsController);
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
    AutoPtr<ISubscriptionController> sc = SubscriptionController::GetInstance();
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
    AutoPtr<ISubscriptionController> sc = SubscriptionController::GetInstance();
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
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISubscriptionController> sc = SubscriptionController::GetInstance();
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

        return ((PhoneBase*)activePhone.Get())->mDcTracker->IsDisconnected(result);
    }
    *result = FALSE;
    return NOERROR;
}

void ProxyController::Logd(
    /* [in] */ String string)
{
    Logger::D(LOGTAG, string);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
