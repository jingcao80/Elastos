
#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/CMessenger.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL_2(CMessenger, Object, IMessenger, IParcelable)

CAR_OBJECT_IMPL(CMessenger)

ECode CMessenger::constructor()
{
    return NOERROR;
}

ECode CMessenger::constructor(
    /* [in] */ IHandler* handler)
{
    return handler->GetIMessenger((IIMessenger**)&mTarget);
}

ECode CMessenger::constructor(
    /* [in] */ IIMessenger* target)
{
    mTarget = target;
    return NOERROR;
}

ECode CMessenger::Send(
    /* [in] */ IMessage* message)
{
    return mTarget->Send(message);
}

ECode CMessenger::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    mTarget = IIMessenger::Probe(obj);
    return NOERROR;
}

ECode CMessenger::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteInterfacePtr(mTarget.Get());
}

ECode CMessenger::GetIMessenger(
    /* [in] */ IIMessenger** mgr)
{
    VALIDATE_NOT_NULL(mgr);
    *mgr = mTarget;
    REFCOUNT_ADD(*mgr);
    return NOERROR;
}

ECode CMessenger::GetBinder(
    /* [in] */ IBinder** mgr)
{
    VALIDATE_NOT_NULL(mgr);
    *mgr = IBinder::Probe(mTarget);
    REFCOUNT_ADD(*mgr);
    return NOERROR;
}

ECode CMessenger::GetHashCode(
    /* [out] */ Int32* hashcode)
{
    VALIDATE_NOT_NULL(hashcode);
    *hashcode = (Int32)mTarget.Get();
    return NOERROR;
}

ECode CMessenger::Equals(
    /* [in] */ IMessenger* other,
    /* [out] */ Boolean* equals)
{
    VALIDATE_NOT_NULL(equals);
    *equals = FALSE;

    if (other != NULL) {
        AutoPtr<IIMessenger> msgr;
        other->GetIMessenger((IIMessenger**)&msgr);
        *equals = (mTarget.Get() == msgr.Get());
    }
    return NOERROR;
}

ECode CMessenger::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* equals)
{
    return Equals(IMessenger::Probe(other), equals);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
