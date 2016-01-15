#include "elastos/droid/internal/policy/impl/PhoneWindowRotationWatcher.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::View::EIID_IRotationWatcher;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CAR_INTERFACE_IMPL_3(PhoneWindowRotationWatcher, Object, IBinder, IPhoneWindowRotationWatcher, IRotationWatcher)

CAR_INTERFACE_IMPL(PhoneWindowRotationWatcher::InnerRunnable1, Object, IRunnable)
PhoneWindowRotationWatcher::InnerRunnable1::InnerRunnable1(
    /* [in] */ PhoneWindowRotationWatcher* owner)
    :mOwner(owner)
{
}

ECode PhoneWindowRotationWatcher::InnerRunnable1::Run()
{
    return NOERROR;
}


PhoneWindowRotationWatcher::PhoneWindowRotationWatcher()
{
}

ECode PhoneWindowRotationWatcher::constructor()
{
    return NOERROR;
}

ECode PhoneWindowRotationWatcher::constructor(
    /* [in] */ IPhoneWindow* owner)
{
    mOwner = owner;
    return NOERROR;
}

ECode PhoneWindowRotationWatcher::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)
    return Object::ToString(info);
}

ECode PhoneWindowRotationWatcher::OnRotationChanged(
    /* [in] */ Int32 rotation)
{
    assert(0);
    return NOERROR;
}

ECode PhoneWindowRotationWatcher::AddWindow(
    /* [in] */ IPhoneWindow* phoneWindow)
{
    assert(0);
    return NOERROR;
}

ECode PhoneWindowRotationWatcher::RemoveWindow(
    /* [in] */ IPhoneWindow* phoneWindow)
{
    assert(0);
    return NOERROR;
}

ECode PhoneWindowRotationWatcher::DispatchRotationChanged()
{
    assert(0);
    return NOERROR;
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
