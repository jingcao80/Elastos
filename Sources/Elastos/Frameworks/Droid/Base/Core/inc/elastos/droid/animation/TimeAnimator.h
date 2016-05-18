
#ifndef __ELASTOS_DROID_ANIMATION_TIMEANIMATOR_H__
#define __ELASTOS_DROID_ANIMATION_TIMEANIMATOR_H__

#include "elastos/droid/animation/ValueAnimator.h"

namespace Elastos {
namespace Droid {
namespace Animation {

class TimeAnimator
    : public ValueAnimator
    , public ITimeAnimator
{
public:
    CAR_INTERFACE_DECL()

    TimeAnimator();

    virtual ~TimeAnimator();

    virtual CARAPI Start();

    virtual CARAPI_(Boolean) AnimationFrame(
        /* [in] */ Int64 currentTime);

    /**
     * Sets a listener that is sent update events throughout the life of
     * an animation.
     *
     * @param listener the listener to be set.
     */
    virtual CARAPI SetTimeListener(
        /* [in] */ ITimeListener* listener);

    //@Override
    virtual CARAPI AnimateValue(
        /* [in] */ Float fraction);

    //@Override
    virtual CARAPI InitAnimation();

    virtual CARAPI Clone(
        /* [out] */ IInterface** object);

private:
    AutoPtr<ITimeListener> mListener;
    Int64 mPreviousTime;
};

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

#endif  // __ELASTOS_DROID_ANIMATION_TIMEANIMATOR_H__
