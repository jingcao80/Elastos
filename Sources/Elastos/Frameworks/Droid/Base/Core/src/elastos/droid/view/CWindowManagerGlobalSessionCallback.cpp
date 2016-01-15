#include "elastos/droid/view/CWindowManagerGlobalSessionCallback.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL_2(CWindowManagerGlobalSessionCallback, Object, IIWindowSessionCallback, IBinder)

CAR_OBJECT_IMPL(CWindowManagerGlobalSessionCallback)

CWindowManagerGlobalSessionCallback::CWindowManagerGlobalSessionCallback()
{}

ECode CWindowManagerGlobalSessionCallback::constructor()
{
    return NOERROR;
}

ECode CWindowManagerGlobalSessionCallback::OnAnimatorScaleChanged(
    /* [in] */ Float scale)
{
    assert(0);
    //ValueAnimator::SetDurationScale(scale);
    return NOERROR;
}

ECode CWindowManagerGlobalSessionCallback::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)
    return Object::ToString(info);
}

} // namespace View
} // namepsace Droid
} // namespace Elastos