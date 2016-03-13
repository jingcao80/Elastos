
#include "elastos/droid/systemui/statusbar/stack/AnimationFilter.h"
#include "Elastos.CoreLibrary.Utility.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

CAR_INTERFACE_IMPL(AnimationFilter, Object, IAnimationFilter);
AnimationFilter::AnimationFilter()
    : mAnimateAlpha(FALSE)
    , mAnimateY(FALSE)
    , mAnimateZ(FALSE)
    , mAnimateScale(FALSE)
    , mAnimateHeight(FALSE)
    , mAnimateTopInset(FALSE)
    , mAnimateDimmed(FALSE)
    , mAnimateDark(FALSE)
    , mAnimateHideSensitive(FALSE)
    , mHasDelays(FALSE)
    , mHasGoToFullShadeEvent(FALSE)
{}

ECode AnimationFilter::AnimateAlpha()
{
    mAnimateAlpha = TRUE;
    return NOERROR;
}

ECode AnimationFilter::AnimateY()
{
    mAnimateY = TRUE;
    return NOERROR;
}

ECode AnimationFilter::HasDelays()
{
    mHasDelays = TRUE;
    return NOERROR;
}

ECode AnimationFilter::AnimateZ()
{
    mAnimateZ = TRUE;
    return NOERROR;
}

ECode AnimationFilter::AnimateScale()
{
    mAnimateScale = TRUE;
    return NOERROR;
}

ECode AnimationFilter::AnimateHeight()
{
    mAnimateHeight = TRUE;
    return NOERROR;
}

ECode AnimationFilter::AnimateTopInset()
{
    mAnimateTopInset = TRUE;
    return NOERROR;
}

ECode AnimationFilter::AnimateDimmed()
{
    mAnimateDimmed = TRUE;
    return NOERROR;
}

ECode AnimationFilter::AnimateDark()
{
    mAnimateDark = TRUE;
    return NOERROR;
}

ECode AnimationFilter::AnimateHideSensitive()
{
    mAnimateHideSensitive = TRUE;
    return NOERROR;
}

ECode AnimationFilter::ApplyCombination(
    /* [in] */ IArrayList/*<NotificationStackScrollLayout.AnimationEvent>*/* events)
{
    Reset();
    Int32 size = 0;
    events->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        events->Get(i, (IInterface**)&obj);

        assert(0 && "TODO");
        AutoPtr<IInterface> filter/* = events.get(i).filter*/;
        CombineFilter((AnimationFilter*)IAnimationFilter::Probe(filter));

        assert(0 && "TODO");
        // if (events.get(i).animationType ==
        //         NotificationStackScrollLayout.AnimationEvent.ANIMATION_TYPE_GO_TO_FULL_SHADE) {
        //     mHasGoToFullShadeEvent = TRUE;
        // }
    }
    return NOERROR;
}

void AnimationFilter::CombineFilter(
    /* [in] */ AnimationFilter* filter)
{
    mAnimateAlpha |= filter->mAnimateAlpha;
    mAnimateY |= filter->mAnimateY;
    mAnimateZ |= filter->mAnimateZ;
    mAnimateScale |= filter->mAnimateScale;
    mAnimateHeight |= filter->mAnimateHeight;
    mAnimateTopInset |= filter->mAnimateTopInset;
    mAnimateDimmed |= filter->mAnimateDimmed;
    mAnimateDark |= filter->mAnimateDark;
    mAnimateHideSensitive |= filter->mAnimateHideSensitive;
    mHasDelays |= filter->mHasDelays;
}

void AnimationFilter::Reset()
{
    mAnimateAlpha = FALSE;
    mAnimateY = FALSE;
    mAnimateZ = FALSE;
    mAnimateScale = FALSE;
    mAnimateHeight = FALSE;
    mAnimateTopInset = FALSE;
    mAnimateDimmed = FALSE;
    mAnimateDark = FALSE;
    mAnimateHideSensitive = FALSE;
    mHasDelays = FALSE;
    mHasGoToFullShadeEvent = FALSE;
}

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
