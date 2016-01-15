
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/RemoteCallbackList.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(RemoteCallbackList::Callback, Object, IProxyDeathRecipient)

RemoteCallbackList::Callback::Callback(
    /* [in] */ IInterface* callback,
    /* [in] */ IInterface* cookie,
    /* [in] */ RemoteCallbackList* owner)
    : mCallback(callback)
    , mCookie(cookie)
    , mOwner(owner)
{}

ECode RemoteCallbackList::Callback::ProxyDied()
{
    Slogger::W("RemoteCallbackList::Callback", " >> %p ProxyDied()", (IProxyDeathRecipient*)this);
    {
        AutoLock lock(mOwner->mCallbacksLock);

        mOwner->mCallbacks.Erase(IBinder::Probe(mCallback));
    }
    return mOwner->OnCallbackDied(mCallback, mCookie);
}

CAR_INTERFACE_IMPL(RemoteCallbackList, Object, IRemoteCallbackList)

RemoteCallbackList::RemoteCallbackList()
    : mCallbacks(20)
    , mBroadcastCount(-1)
    , mKilled(FALSE)
{}

ECode RemoteCallbackList::Register(
    /* [in] */ IInterface* callback,
    /* [out] */ Boolean* result)
{
    return Register(callback, NULL, result);
}

ECode RemoteCallbackList::Register(
    /* [in] */ IInterface* callback,
    /* [in] */ IInterface* cookie,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    {
        AutoLock lock(mCallbacksLock);

        if (mKilled) {
            *result = FALSE;
            return NOERROR;
        }
        AutoPtr<IBinder> binder = IBinder::Probe(callback);
        // try {
        Callback* cb = new Callback(callback, cookie, this);
        IProxy* proxy = (IProxy*)callback->Probe(EIID_IProxy);
        if (proxy != NULL) {
            proxy->LinkToDeath(cb, 0);
        }
        mCallbacks[binder] = cb;
        *result = TRUE;
        return NOERROR;
        // } catch (RemoteException e) {
        //     return false;
        // }
    }
}

ECode RemoteCallbackList::Unregister(
    /* [in] */ IInterface* callback,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    {
        AutoLock lock(mCallbacksLock);

        HashMap< AutoPtr<IBinder>, AutoPtr<Callback> >::Iterator it =
                mCallbacks.Find(IBinder::Probe(callback));
        if (it != mCallbacks.End()) {
            AutoPtr<Callback> cb = it->mSecond;
            mCallbacks.Erase(it);
            IProxy* proxy = (IProxy*)cb->mCallback->Probe(EIID_IProxy);
            assert(proxy != NULL);
            proxy->UnlinkToDeath(cb, 0, result);
            *result = TRUE;
            return NOERROR;
        }
        *result = FALSE;
        return NOERROR;
    }
}

ECode RemoteCallbackList::Kill()
{
    {
        AutoLock lock(mCallbacksLock);

        HashMap< AutoPtr<IBinder>, AutoPtr<Callback> >::Iterator it;
        for (it = mCallbacks.Begin(); it != mCallbacks.End(); ++it) {
            AutoPtr<Callback> cb = it->mSecond;
            IProxy* proxy = (IProxy*)cb->mCallback->Probe(EIID_IProxy);
            assert(proxy != NULL);
            Boolean result;
            proxy->UnlinkToDeath(cb.Get(), 0, &result);
        }
        mCallbacks.Clear();
        mKilled = TRUE;
    }
    return NOERROR;
}

ECode RemoteCallbackList::OnCallbackDied(
    /* [in] */ IInterface* callback)
{
    return NOERROR;
}

ECode RemoteCallbackList::OnCallbackDied(
    /* [in] */ IInterface* callback,
    /* [in] */ IInterface* cookie)
{
    return OnCallbackDied(callback);
}

ECode RemoteCallbackList::BeginBroadcast(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);

    {
        AutoLock lock(mCallbacksLock);

        if (mBroadcastCount > 0) {
            // throw new IllegalStateException(
            //         "beginBroadcast() called while already in a broadcast");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        const Int32 N = mBroadcastCount = mCallbacks.GetSize();
        if (N <= 0) {
            *number = 0;
            return NOERROR;
        }
        AutoPtr< ArrayOf<IInterface*> > active = mActiveBroadcast;
        if (active == NULL || active->GetLength() < N) {
            mActiveBroadcast = active = ArrayOf<IInterface*>::Alloc(N);
        }
        Int32 i = 0;
        HashMap< AutoPtr<IBinder>, AutoPtr<Callback> >::Iterator it;
        for (it = mCallbacks.Begin(); it != mCallbacks.End(); ++it) {
            active->Set(i++, TO_IINTERFACE(it->mSecond));
        }
        *number = i;
        return NOERROR;
    }
}

ECode RemoteCallbackList::GetBroadcastItem(
    /* [in] */ Int32 index,
    /* [out] */ IInterface** callback)
{
    VALIDATE_NOT_NULL(callback);

    Callback* cb = (Callback*)IObject::Probe((*mActiveBroadcast)[index]);
    *callback = cb->mCallback;
    REFCOUNT_ADD(*callback);
    return NOERROR;
}

ECode RemoteCallbackList::GetBroadcastCookie(
    /* [in] */ Int32 index,
    /* [out] */ IInterface** cookie)
{
    VALIDATE_NOT_NULL(cookie);

    Callback* cb = (Callback*)IObject::Probe((*mActiveBroadcast)[index]);
    *cookie = cb->mCookie;
    REFCOUNT_ADD(*cookie);
    return NOERROR;
}

ECode RemoteCallbackList::FinishBroadcast()
{
    if (mBroadcastCount < 0) {
        // throw new IllegalStateException(
        //         "finishBroadcast() called outside of a broadcast");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr< ArrayOf<IInterface*> > active = mActiveBroadcast;
    if (active != NULL) {
        const Int32 N = mBroadcastCount;
        for (Int32 i = 0; i < N; i++) {
            active->Set(i, NULL);
        }
    }

    mBroadcastCount = -1;
    return NOERROR;
}

ECode RemoteCallbackList::GetRegisteredCallbackCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    {
        AutoLock lock(mCallbacksLock);

        if (mKilled) {
            *count = 0;
            return NOERROR;
        }
        *count = mCallbacks.GetSize();
        return NOERROR;
    }
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
