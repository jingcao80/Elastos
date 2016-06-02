
#include "elastos/droid/app/trust/TrustManager.h"
#include "elastos/droid/app/trust/CTrustManagerTrustListener.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/utility/CArrayMap.h"

using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Utility::CArrayMap;

namespace Elastos {
namespace Droid {
namespace App {
namespace Trust {

const Int32 TrustManager::MSG_TRUST_CHANGED = 1;
const Int32 TrustManager::MSG_TRUST_MANAGED_CHANGED = 2;

const String TrustManager::TAG("TrustManager");
const String TrustManager::DATA_INITIATED_BY_USER("initiatedByUser");

//======================================================================================
// TrustManager::TrustManagerTrustListener
//======================================================================================
CAR_INTERFACE_IMPL_2(TrustManager::TrustManagerTrustListener, Object, IITrustListener, IBinder)

ECode TrustManager::TrustManagerTrustListener::constructor(
    /* [in] */ ITrustListener* trustListener,
    /* [ini] */ IHandler* handle)
{
    mTrustListener = trustListener;
    mHandler = handle;
    return NOERROR;
}

ECode TrustManager::TrustManagerTrustListener::OnTrustChanged(
    /* [in] */ Boolean enabled,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean initiatedByUser)
{
    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(TrustManager::MSG_TRUST_CHANGED,
        (enabled ? 1 : 0), userId, mTrustListener.Get(), (IMessage**)&m);
    if (initiatedByUser) {
        AutoPtr<IBundle> data;
        m->GetData((IBundle**)&data);
        data->PutBoolean(TrustManager::DATA_INITIATED_BY_USER, initiatedByUser);
    }
    return m->SendToTarget();
}

ECode TrustManager::TrustManagerTrustListener::OnTrustManagedChanged(
    /* [in] */ Boolean managed,
    /* [in] */ Int32 userId)
{
    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(TrustManager::MSG_TRUST_MANAGED_CHANGED,
        (managed ? 1 : 0), userId, mTrustListener.Get(), (IMessage**)&m);
    return m->SendToTarget();
}

ECode TrustManager::TrustManagerTrustListener::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//======================================================================================
// TrustManager::MyHandler
//======================================================================================
TrustManager::MyHandler::MyHandler()
{}

//@Override
ECode TrustManager::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    ITrustListener* listener = ITrustListener::Probe(obj);
    switch(what) {
        case TrustManager::MSG_TRUST_CHANGED: {
            Boolean initiatedByUser = FALSE;

            AutoPtr<IBundle> data;
            msg->PeekData((IBundle**)&data);
            if (data != NULL) {
                data->GetBoolean(TrustManager::DATA_INITIATED_BY_USER, &initiatedByUser);
            }
            listener->OnTrustChanged(arg1 != 0, arg2, initiatedByUser);
            break;
        }

        case TrustManager::MSG_TRUST_MANAGED_CHANGED: {
            listener->OnTrustManagedChanged(arg1 != 0, arg2);
            break;
        }
    }

    return NOERROR;
}

//======================================================================================
// TrustManager
//======================================================================================

CAR_INTERFACE_IMPL(TrustManager, Object, ITrustManager)

TrustManager::TrustManager()
{}

TrustManager::~TrustManager()
{}

ECode TrustManager::constructor(
    /* [in] */ IBinder* b)
{
    AutoPtr<ILooper> looper = Looper::GetMainLooper();
    mHandler = new MyHandler();
    mHandler->constructor(looper);

    mService = IITrustManager::Probe(b);
    CArrayMap::New((IArrayMap**)&mTrustListeners);
    return NOERROR;
}

ECode TrustManager::ReportUnlockAttempt(
    /* [in] */ Boolean successful,
    /* [in] */ Int32 userId)
{
    ECode ec = NOERROR;
    if (mService != NULL) {
        ec = mService->ReportUnlockAttempt(successful, userId);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            OnError(ec);
            return NOERROR;
        }
    }

    return ec;
}

ECode TrustManager::ReportEnabledTrustAgentsChanged(
    /* [in] */ Int32 userId)
{
    ECode ec = NOERROR;
    if (mService != NULL) {
        ec = mService->ReportEnabledTrustAgentsChanged(userId);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            OnError(ec);
            return NOERROR;
        }
    }
    return ec;
}

ECode TrustManager::ReportRequireCredentialEntry(
    /* [in] */ Int32 userId)
{
    ECode ec = NOERROR;
    if (mService != NULL) {
        ec = mService->ReportRequireCredentialEntry(userId);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            OnError(ec);
            return NOERROR;
        }
    }
    return ec;
}

ECode TrustManager::RegisterTrustListener(
    /* [in] */ ITrustListener* trustListener)
{
    AutoPtr<IITrustListener> iTrustListener;
    CTrustManagerTrustListener::New(trustListener, mHandler, (IITrustListener**)&iTrustListener);

    ECode ec = mService->RegisterTrustListener(iTrustListener);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        OnError(ec);
        return NOERROR;
    }
    else if (FAILED(ec)) {
        OnError(ec);
        return ec;
    }

    mTrustListeners->Put(trustListener, TO_IINTERFACE(iTrustListener));
    return NOERROR;
}

ECode TrustManager::UnregisterTrustListener(
    /* [in] */ ITrustListener* trustListener)
{
    AutoPtr<IInterface> obj;
    mTrustListeners->Remove(trustListener, (IInterface**)&obj);
    IITrustListener* iTrustListener = IITrustListener::Probe(obj);
    ECode ec = NOERROR;
    if (iTrustListener != NULL) {
        ec = mService->UnregisterTrustListener(iTrustListener);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            OnError(ec);
            return NOERROR;
        }
    }
    return ec;
}

void TrustManager::OnError(
    /* [in] */ ECode ec)
{
    Logger::E(TAG, "Error while calling TrustManagerService: %08x", ec);
}

} // namespace Trust
} // namespace App
} // namespace Droid
} // namespace Elastos