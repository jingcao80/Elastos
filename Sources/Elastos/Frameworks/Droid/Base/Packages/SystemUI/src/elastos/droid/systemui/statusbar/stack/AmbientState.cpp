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

#include "elastos/droid/systemui/statusbar/stack/AmbientState.h"
#include "Elastos.Droid.View.h"

using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

CAR_INTERFACE_IMPL(AmbientState, Object, IAmbientState)
AmbientState::AmbientState()
    : mScrollY(0)
    , mDimmed(FALSE)
    , mOverScrollTopAmount(0)
    , mOverScrollBottomAmount(0)
    , mSpeedBumpIndex(-1)
    , mDark(FALSE)
    , mHideSensitive(FALSE)
{
    CArrayList::New((IArrayList**)&mDraggedViews);
}

ECode AmbientState::GetScrollY(
    /* [out] */ Int32* y)
{
    VALIDATE_NOT_NULL(y);
    *y = mScrollY;
    return NOERROR;
}

ECode AmbientState::SetScrollY(
    /* [in] */ Int32 scrollY)
{
    mScrollY = scrollY;
    return NOERROR;
}

ECode AmbientState::OnBeginDrag(
    /* [in] */ IView* view)
{
    mDraggedViews->Add(view);
    return NOERROR;
}

ECode AmbientState::OnDragFinished(
    /* [in] */ IView* view)
{
    mDraggedViews->Remove(view);
    return NOERROR;
}

ECode AmbientState::GetDraggedViews(
    /* [out] */ IArrayList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = mDraggedViews;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

/**
 * @param dimmed Whether we are in a dimmed state (on the lockscreen), where the backgrounds are
 *               translucent and everything is scaled back a bit.
 */
ECode AmbientState::SetDimmed(
    /* [in] */ Boolean dimmed)
{
    mDimmed = dimmed;
    return NOERROR;
}

/** In dark mode, we draw as little as possible, assuming a black background */
ECode AmbientState::SetDark(
    /* [in] */ Boolean dark)
{
    mDark = dark;
    return NOERROR;
}

ECode AmbientState::SetHideSensitive(
    /* [in] */ Boolean hideSensitive)
{
    mHideSensitive = hideSensitive;
    return NOERROR;
}

/**
 * In dimmed mode, a child can be activated, which happens on the first tap of the double-tap
 * interaction. This child is then scaled normally and its background is fully opaque.
 */
ECode AmbientState::SetActivatedChild(
    /* [in] */ IActivatableNotificationView* activatedChild)
{
    mActivatedChild = activatedChild;
    return NOERROR;
}

ECode AmbientState::IsDimmed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDimmed;
    return NOERROR;
}

ECode AmbientState::IsDark(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDark;
    return NOERROR;
}

ECode AmbientState::IsHideSensitive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHideSensitive;
    return NOERROR;
}

ECode AmbientState::GetActivatedChild(
    /* [out] */ IActivatableNotificationView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mActivatedChild;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode AmbientState::SetOverScrollAmount(
    /* [in] */ Float amount,
    /* [in] */ Boolean onTop)
{
    if (onTop) {
        mOverScrollTopAmount = amount;
    }
    else {
        mOverScrollBottomAmount = amount;
    }
    return NOERROR;
}

ECode AmbientState::GetOverScrollAmount(
    /* [in] */ Boolean top,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = top ? mOverScrollTopAmount : mOverScrollBottomAmount;
    return NOERROR;
}

ECode AmbientState::GetSpeedBumpIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mSpeedBumpIndex;
    return NOERROR;
}

ECode AmbientState::SetSpeedBumpIndex(
    /* [in] */ Int32 speedBumpIndex)
{
    mSpeedBumpIndex = speedBumpIndex;
    return NOERROR;
}

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
