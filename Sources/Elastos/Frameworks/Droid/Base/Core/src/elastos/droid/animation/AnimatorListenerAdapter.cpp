
#include "elastos/droid/animation/AnimatorListenerAdapter.h"

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_INTERFACE_IMPL_2(AnimatorListenerAdapter, Object, IAnimatorListener, IAnimatorPauseListener);

AnimatorListenerAdapter::AnimatorListenerAdapter()
{}

AnimatorListenerAdapter::~AnimatorListenerAdapter()
{}

ECode AnimatorListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode AnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode AnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode AnimatorListenerAdapter::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode AnimatorListenerAdapter::OnAnimationPause(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode AnimatorListenerAdapter::OnAnimationResume(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
