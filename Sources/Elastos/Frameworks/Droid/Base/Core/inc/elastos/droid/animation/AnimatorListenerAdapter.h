
#ifndef __ELASTOS_DROID_ANIMATION_ANIMATORLISTENERADAPTER_H__
#define __ELASTOS_DROID_ANIMATION_ANIMATORLISTENERADAPTER_H__

#include "Elastos.Droid.Animation.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Animation {

class ECO_PUBLIC AnimatorListenerAdapter
    : public Object
    , public IAnimatorListener
    , public IAnimatorPauseListener
{
public:
    CAR_INTERFACE_DECL();

    AnimatorListenerAdapter();

    virtual ~AnimatorListenerAdapter();

    virtual CARAPI OnAnimationStart(
        /* [in] */ IAnimator* animation);

    virtual CARAPI OnAnimationEnd(
        /* [in] */ IAnimator* animation);

    virtual CARAPI OnAnimationCancel(
        /* [in] */ IAnimator* animation);

    virtual CARAPI OnAnimationRepeat(
        /* [in] */ IAnimator* animation);

    virtual CARAPI OnAnimationPause(
        /* [in] */ IAnimator* animation);

    virtual CARAPI OnAnimationResume(
        /* [in] */ IAnimator* animation);
};

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_ANIMATION_ANIMATORLISTENERADAPTER_H__
