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

#include "elastos/droid/systemui/recents/views/TaskViewFooter.h"

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IObjectAnimatorHelper;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

CAR_INTERFACE_IMPL(TaskViewFooter, FrameLayout, ITaskViewFooter)

TaskViewFooter::TaskViewFooter()
    : mFooterHeight(0)
    , mMaxFooterHeight(0)
{}

ECode TaskViewFooter::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode TaskViewFooter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode TaskViewFooter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode TaskViewFooter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(FrameLayout::constructor(context, attrs, defStyleAttr, defStyleRes));
    mConfig = RecentsConfiguration::GetInstance();
    mMaxFooterHeight = mConfig->mTaskViewLockToAppButtonHeight;
    Int32 height;
    GetFooterHeight(&height);
    SetFooterHeight(height);
    return NOERROR;
}

/** Sets the callbacks for when the footer height changes. */
ECode TaskViewFooter::SetCallbacks(
    /* [in] */ ITaskFooterViewCallbacks* cb)
{
    mCb = cb;
    mCb->OnTaskFooterHeightChanged(mFooterHeight, mMaxFooterHeight);
    return NOERROR;
}

/** Sets the footer height. */
ECode TaskViewFooter::SetFooterHeight(
    /* [in] */ Int32 footerHeight)
{
    if (footerHeight != mFooterHeight) {
        mFooterHeight = footerHeight;
        mCb->OnTaskFooterHeightChanged(footerHeight, mMaxFooterHeight);
    }
    return NOERROR;
}

/** Gets the footer height. */
ECode TaskViewFooter::GetFooterHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mFooterHeight;
    return NOERROR;
}

/** Animates the footer into and out of view. */
ECode TaskViewFooter::AnimateFooterVisibility(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 duration)
{
    // Return early if there is no footer
    if (mMaxFooterHeight <= 0) return NOERROR;

    // Cancel the previous animation
    if (mFooterAnimator != NULL) {
        mFooterAnimator->RemoveAllListeners();
        mFooterAnimator->Cancel();
    }
    Int32 finalHeight = visible ? mMaxFooterHeight : 0;
    if (duration > 0) {
        AutoPtr<IObjectAnimatorHelper> oaHelper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelper);
        AutoPtr<ArrayOf<Int32> > params = ArrayOf<Int32>::Alloc(1);
        (*params)[0] = finalHeight;
        AutoPtr<IObjectAnimator> animator;
        oaHelper->OfInt32(TO_IINTERFACE(this), String("footerHeight"), params, (IObjectAnimator**)&animator);
        mFooterAnimator = IAnimator::Probe(animator);
        mFooterAnimator->SetDuration(duration);
        mFooterAnimator->SetInterpolator(ITimeInterpolator::Probe(mConfig->mFastOutSlowInInterpolator));
        mFooterAnimator->Start();
    }
    else {
        SetFooterHeight(finalHeight);
    }
    return NOERROR;
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
