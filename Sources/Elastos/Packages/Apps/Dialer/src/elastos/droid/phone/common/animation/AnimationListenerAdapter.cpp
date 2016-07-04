
#include "elastos/droid/phone/common/animation/AnimationListenerAdapter.h"

using Elastos::Droid::View::Animation::EIID_IAnimationAnimationListener;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Animation {

CAR_INTERFACE_IMPL(AnimationListenerAdapter, Object, IAnimationAnimationListener);

// @Override
ECode AnimationListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimation* animation)
{
    return NOERROR;
}

// @Override
ECode AnimationListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimation* animation)
{
    return NOERROR;
}

// @Override
ECode AnimationListenerAdapter::OnAnimationRepeat(
    /* [in] */ IAnimation* animation)
{
    return NOERROR;
}

} // namespace Animation
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos
