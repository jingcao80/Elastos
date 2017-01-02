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

#include "elastos/droid/systemui/statusbar/phone/CObservableScrollView.h"
#include <elastos/core/Math.h>

using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IMotionEventHelper;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_OBJECT_IMPL(CObservableScrollView)

CAR_INTERFACE_IMPL(CObservableScrollView, ScrollView, IObservableScrollView)

CObservableScrollView::CObservableScrollView()
    : mLastOverscrollAmount(0)
    , mTouchEnabled(TRUE)
    , mHandlingTouchEvent(FALSE)
    , mLastX(0)
    , mLastY(0)
    , mBlockFlinging(FALSE)
    , mTouchCancelled(FALSE)
{}

ECode CObservableScrollView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ScrollView::constructor(context, attrs);
}

ECode CObservableScrollView::SetListener(
    /* [in] */ IObservableScrollViewListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode CObservableScrollView::SetTouchEnabled(
    /* [in] */ Boolean touchEnabled)
{
    mTouchEnabled = touchEnabled;
    return NOERROR;
}

ECode CObservableScrollView::IsScrolledToBottom(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 y = 0;
    GetScrollY(&y);
    *result = y == GetMaxScrollY();
    return NOERROR;
}

ECode CObservableScrollView::IsHandlingTouchEvent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHandlingTouchEvent;
    return NOERROR;
}

Int32 CObservableScrollView::GetMaxScrollY()
{
    Int32 scrollRange = 0, count = 0;
    if ((GetChildCount(&count), count) > 0) {
        AutoPtr<IView> child;
        GetChildAt(0, (IView**)&child);
        Int32 h1 = 0, h2 = 0;
        GetHeight(&h1);
        child->GetHeight(&h2);
        scrollRange = Elastos::Core::Math::Max(0, h2 - (h1 - mPaddingBottom - mPaddingTop));
    }
    return scrollRange;
}

ECode CObservableScrollView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);
    mHandlingTouchEvent = TRUE;
    ev->GetX(&mLastX);
    ev->GetY(&mLastY);
    Boolean result = FALSE;
    ScrollView::OnTouchEvent(ev, &result);
    mHandlingTouchEvent = FALSE;
    *state = result;
    return NOERROR;
}

ECode CObservableScrollView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);
    mHandlingTouchEvent = TRUE;
    ev->GetX(&mLastX);
    ev->GetY(&mLastY);
    Boolean result = FALSE;
    ScrollView::OnInterceptTouchEvent(ev, &result);
    mHandlingTouchEvent = FALSE;
    *state = result;
    return NOERROR;
}

ECode CObservableScrollView::DispatchTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 action = 0;
    ev->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        if (!mTouchEnabled) {
            mTouchCancelled = TRUE;
            *result = FALSE;
            return NOERROR;
        }
        mTouchCancelled = FALSE;
    }
    else if (mTouchCancelled) {
        *result = FALSE;
        return NOERROR;
    }
    else if (!mTouchEnabled) {
        AutoPtr<IMotionEventHelper> helper;
        CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
        AutoPtr<IMotionEvent> cancel;
        helper->Obtain(ev, (IMotionEvent**)&cancel);
        cancel->SetAction(IMotionEvent::ACTION_CANCEL);
        ScrollView::DispatchTouchEvent(ev, result);
        IInputEvent::Probe(cancel)->Recycle();
        mTouchCancelled = TRUE;
        *result = FALSE;
        return NOERROR;
    }
    return ScrollView::DispatchTouchEvent(ev, result);
}

ECode CObservableScrollView::OnScrollChanged(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 oldl,
    /* [in] */ Int32 oldt)
{
    ScrollView::OnScrollChanged(l, t, oldl, oldt);
    if (mListener != NULL) {
        mListener->OnScrollChanged();
    }
    return NOERROR;
}

Boolean CObservableScrollView::OverScrollBy(
    /* [in] */ Int32 deltaX,
    /* [in] */ Int32 deltaY,
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Int32 scrollRangeX,
    /* [in] */ Int32 scrollRangeY,
    /* [in] */ Int32 maxOverScrollX,
    /* [in] */ Int32 maxOverScrollY,
    /* [in] */ Boolean isTouchEvent)
{
    mLastOverscrollAmount = Elastos::Core::Math::Max(0, scrollY + deltaY - GetMaxScrollY());
    return ScrollView::OverScrollBy(deltaX, deltaY, scrollX, scrollY, scrollRangeX, scrollRangeY,
                    maxOverScrollX, maxOverScrollY, isTouchEvent);
}

ECode CObservableScrollView::SetBlockFlinging(
    /* [in] */ Boolean blockFlinging)
{
    mBlockFlinging = blockFlinging;
    return NOERROR;
}

ECode CObservableScrollView::Fling(
    /* [in] */ Int32 velocityY)
{
    if (!mBlockFlinging) {
        ScrollView::Fling(velocityY);
    }
    return NOERROR;
}

ECode CObservableScrollView::OnOverScrolled(
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Boolean clampedX,
    /* [in] */ Boolean clampedY)
{
    ScrollView::OnOverScrolled(scrollX, scrollY, clampedX, clampedY);
    if (mListener != NULL && mLastOverscrollAmount > 0) {
        mListener->OnOverscrolled(mLastX, mLastY, mLastOverscrollAmount);
    }
    return NOERROR;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
