
#include "elastos/droid/systemui/statusbar/policy/RotationLockControllerImpl.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Os::IUserHandle;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::Concurrent::CCopyOnWriteArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

RotationLockControllerImpl::LocalRotationPolicyListener::LocalRotationPolicyListener(
    /* [in] */ RotationLockControllerImpl* host)
    : mHost(host)
{}

ECode RotationLockControllerImpl::LocalRotationPolicyListener::OnChange()
{
    mHost->NotifyChanged();
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(RotationLockControllerImpl, Object, IRotationLockController, IListenable);

ECode RotationLockControllerImpl::constructor(
    /* [in] */ IContext* context)
{
    mRotationPolicyListener = new LocalRotationPolicyListener(this);
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&mCallbacks);
    mContext = context;
    SetListening(TRUE);
    return NOERROR;
}

ECode RotationLockControllerImpl::AddRotationLockControllerCallback(
    /* [in] */ IRotationLockControllerCallback* callback)
{
    IList::Probe(mCallbacks)->Add(callback);
    NotifyChanged(callback);
    return NOERROR;
}

ECode RotationLockControllerImpl::RemoveRotationLockControllerCallback(
    /* [in] */ IRotationLockControllerCallback* callback)
{
    IList::Probe(mCallbacks)->Remove(callback);
    return NOERROR;
}

ECode RotationLockControllerImpl::GetRotationLockOrientation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = RotationPolicy::GetRotationLockOrientation(mContext);
    return NOERROR;
}

ECode RotationLockControllerImpl::IsRotationLocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = RotationPolicy::IsRotationLocked(mContext);
    return NOERROR;
}

ECode RotationLockControllerImpl::SetRotationLocked(
    /* [in] */ Boolean locked)
{
    RotationPolicy::SetRotationLock(mContext, locked);
    return NOERROR;
}

ECode RotationLockControllerImpl::IsRotationLockAffordanceVisible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = RotationPolicy::IsRotationLockToggleVisible(mContext);
    return NOERROR;
}

ECode RotationLockControllerImpl::SetListening(
    /* [in] */ Boolean listening)
{
    if (listening) {
        RotationPolicy::RegisterRotationPolicyListener(mContext, mRotationPolicyListener,
                IUserHandle::USER_ALL);
    }
    else {
        RotationPolicy::UnregisterRotationPolicyListener(mContext, mRotationPolicyListener);
    }
    return NOERROR;
}

void RotationLockControllerImpl::NotifyChanged()
{
    AutoPtr<IIterator> iter;
    IList::Probe(mCallbacks)->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> callback;
        iter->GetNext((IInterface**)&callback);
        NotifyChanged(IRotationLockControllerCallback::Probe(callback));
    }
}

void RotationLockControllerImpl::NotifyChanged(
    /* [in] */ IRotationLockControllerCallback* callback)
{
    callback->OnRotationLockStateChanged(RotationPolicy::IsRotationLocked(mContext),
            RotationPolicy::IsRotationLockToggleVisible(mContext));
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
