//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/RemoteCallback.h"
#include "elastos/droid/os/CRemoteCallbackLocalCallback.h"

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
CAR_INTERFACE_IMPL_2(RemoteCallback::LocalCallback, Object, IIRemoteCallback, IBinder)

RemoteCallback::LocalCallback::LocalCallback()
{}

ECode RemoteCallback::LocalCallback::constructor(
    /* [in] */ IWeakReference* wr)
{
    mWeakHost = wr;
    return NOERROR;
}

ECode RemoteCallback::LocalCallback::SendResult(
    /* [in] */ IBundle* bundle)
{
    AutoPtr<IRemoteCallback> obj;
    mWeakHost->Resolve(EIID_IRemoteCallback, (IInterface**)&obj);
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

CAR_INTERFACE_IMPL_2(RemoteCallback, Object, IParcelable, IRemoteCallback)

RemoteCallback::RemoteCallback()
{}

ECode RemoteCallback::constructor(
    /* [in] */ IHandler* handler)
{
    mHandler = handler;
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    CRemoteCallbackLocalCallback::New(wr, (IIRemoteCallback**)&mTarget);
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
    return in->ReadInterfacePtr((HANDLE*)(IIRemoteCallback**)&mTarget);
}

ECode RemoteCallback::WriteToParcel(
    /* [in] */ IParcel* out)
{
    return out->WriteInterfacePtr(mTarget);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
