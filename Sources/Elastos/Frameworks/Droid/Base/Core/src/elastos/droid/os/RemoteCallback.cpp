
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/RemoteCallback.h"

namespace Elastos {
namespace Droid {
namespace Os {

//================================================================
// RemoteCallback::DeliverResult
//================================================================
RemoteCallback::DeliverResult::DeliverResult(
    /* [in] */ IBundle* result,
    /* [in] */ IWeakReference* wr)
    : mResult(result)
    , mWeakHost(wr)
{
}

ECode RemoteCallback::DeliverResult::Run()
{
    AutoPtr<IObject> obj;
    mWeakHost->Resolve(EIID_IObject, (IInterface**)&obj);

    if (obj) {
        RemoteCallback* rc = (RemoteCallback*)obj.Get();
        rc->OnResult(mResult);
    }

    return NOERROR;
}

//================================================================
// RemoteCallback::LocalCallback
//================================================================
CAR_INTERFACE_IMPL(RemoteCallback::LocalCallback, Object, IIRemoteCallback)

RemoteCallback::LocalCallback::LocalCallback(
    /* [in] */ IWeakReference* wr)
    : mWeakHost(wr)
{
}

ECode RemoteCallback::LocalCallback::SendResult(
    /* [in] */ IBundle* bundle)
{
    AutoPtr<IObject> obj;
    mWeakHost->Resolve(EIID_IObject, (IInterface**)&obj);
    if (obj == NULL) {
        return NOERROR;
    }

    RemoteCallback* rc = (RemoteCallback*)obj.Get();
    AutoPtr<IRunnable> runnable = new DeliverResult(bundle, mWeakHost);
    Boolean result;
    return rc->mHandler->Post(runnable, &result);
}

//================================================================
// RemoteCallback::RemoteCallbackProxy
//================================================================
ECode RemoteCallbackProxy::constructor(
    /* [in] */ IIRemoteCallback* target)
{
    return RemoteCallback::constructor(target);
}

void RemoteCallbackProxy::OnResult(
    /* [in] */ IBundle* bundle)
{
}

CAR_INTERFACE_IMPL(RemoteCallback, Object, IParcelable)

RemoteCallback::RemoteCallback()
{}

ECode RemoteCallback::constructor(
    /* [in] */ IHandler* handler)
{
    mHandler = handler;
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    mTarget = new LocalCallback(wr);
    return NOERROR;
}

ECode RemoteCallback::constructor(
    /* [in] */ IIRemoteCallback* target)
{
    mHandler = NULL;
    mTarget = target;
    return NOERROR;
}

ECode RemoteCallback::SendResult(
    /* [in] */ IBundle* bundle)
{
    mTarget->SendResult(bundle);
    return NOERROR;
}

void RemoteCallback::OnResult(
    /* [in] */ IBundle* bundle)
{
    assert(0 && "pure virtual function called.");
}

ECode RemoteCallback::Equals(
    /* [in] */ IInterface* otherObj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (otherObj == NULL) {
        return NOERROR;
    }
    // try {
    *result = Object::Equals(mTarget, IIRemoteCallback::Probe(otherObj));
    // } catch (ClassCastException e) {
    // }

    return NOERROR;
}

ECode RemoteCallback::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    *hash = Object::GetHashCode(hash);
    return NOERROR;
}

ECode RemoteCallback::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    return in->ReadInterfacePtr((Handle32*)(IIRemoteCallback**)&mTarget);
}

ECode RemoteCallback::WriteToParcel(
    /* [in] */ IParcel* out)
{
    return out->WriteInterfacePtr(mTarget);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
