#include "elastos/droid/view/CWindowManagerGlobalSessionCallback.h"
#include "elastos/droid/animation/CValueAnimatorHelper.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;

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
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    helper->SetDurationScale(scale);
    return NOERROR;
}

ECode CWindowManagerGlobalSessionCallback::ToString(
    /* [out] */ String* info)
{
    return Object::ToString(info);
}

} // namespace View
} // namepsace Droid
} // namespace Elastos