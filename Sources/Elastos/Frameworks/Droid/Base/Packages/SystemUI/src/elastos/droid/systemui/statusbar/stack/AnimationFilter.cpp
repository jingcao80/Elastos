//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/systemui/statusbar/stack/AnimationFilter.h"
#include "elastos/droid/systemui/statusbar/stack/CNotificationStackScrollLayout.h"
#include "Elastos.CoreLibrary.Utility.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

CAR_INTERFACE_IMPL(AnimationFilter, Object, IAnimationFilter)
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
        CNotificationStackScrollLayout::AnimationEvent* item =
                (CNotificationStackScrollLayout::AnimationEvent*)IObject::Probe(obj);
        CombineFilter((AnimationFilter*)item->mFilter.Get());

        if (item->mAnimationType ==
                CNotificationStackScrollLayout::AnimationEvent::ANIMATION_TYPE_GO_TO_FULL_SHADE) {
            mHasGoToFullShadeEvent = TRUE;
        }
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
