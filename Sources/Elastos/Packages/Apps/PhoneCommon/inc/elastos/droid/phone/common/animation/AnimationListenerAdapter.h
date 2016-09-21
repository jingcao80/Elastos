
#ifndef __ELASTOS_DROID_PHONE_COMMON_ANIMATION_ANIMATIONLISTENERADAPTER_H__
#define __ELASTOS_DROID_PHONE_COMMON_ANIMATION_ANIMATIONLISTENERADAPTER_H__

#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::View::Animation::IAnimationAnimationListener;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Animation {

/**
 * Provides empty implementations of the methods in {@link AnimationListener}
 * for convenience reasons.
 */
class AnimationListenerAdapter
    : public Object
    , public IAnimationAnimationListener
{
public:
    CAR_INTERFACE_DECL()

    /**
     * {@inheritDoc}
     */
    // @Override
    CARAPI OnAnimationStart(
        /* [in] */ IAnimation* animation);

    /**
     * {@inheritDoc}
     */
    // @Override
    CARAPI OnAnimationEnd(
        /* [in] */ IAnimation* animation);

    /**
     * {@inheritDoc}
     */
    // @Override
    CARAPI OnAnimationRepeat(
        /* [in] */ IAnimation* animation);
};

} // namespace Animation
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PHONE_COMMON_ANIMATION_ANIMATIONLISTENERADAPTER_H__
