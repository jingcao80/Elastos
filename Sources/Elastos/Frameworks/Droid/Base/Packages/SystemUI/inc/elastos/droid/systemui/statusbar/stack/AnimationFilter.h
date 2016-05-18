
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_ANIMATIONFILTER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_ANIMATIONFILTER_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

/**
 * Filters the animations for only a certain type of properties.
 */
class AnimationFilter
    : public Object
    , public IAnimationFilter
{
public:
    CAR_INTERFACE_DECL()

    AnimationFilter();

    CARAPI AnimateAlpha();

    CARAPI AnimateY();

    CARAPI HasDelays();

    CARAPI AnimateZ();

    CARAPI AnimateScale();

    CARAPI AnimateHeight();

    CARAPI AnimateTopInset();

    CARAPI AnimateDimmed();

    CARAPI AnimateDark();

    CARAPI AnimateHideSensitive();

    /**
     * Combines multiple filters into {@code this} filter, using or as the operand .
     *
     * @param events The animation events from the filters to combine.
     */
    CARAPI ApplyCombination(
        /* [in] */ IArrayList/*<NotificationStackScrollLayout.AnimationEvent>*/* events);

private:
    CARAPI_(void) CombineFilter(
        /* [in] */ AnimationFilter* filter);

    CARAPI_(void) Reset();

public:
    Boolean mAnimateAlpha;
    Boolean mAnimateY;
    Boolean mAnimateZ;
    Boolean mAnimateScale;
    Boolean mAnimateHeight;
    Boolean mAnimateTopInset;
    Boolean mAnimateDimmed;
    Boolean mAnimateDark;
    Boolean mAnimateHideSensitive;
    Boolean mHasDelays;
    Boolean mHasGoToFullShadeEvent;
};

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_ANIMATIONFILTER_H__
