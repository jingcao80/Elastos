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

#include "elastos/droid/systemui/statusbar/phone/CNotificationsQuickSettingsContainer.h"
#include "../R.h"

using Elastos::Droid::View::EIID_IViewStubOnInflateListener;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {


CAR_INTERFACE_IMPL(CNotificationsQuickSettingsContainer::InnerListener, Object, IViewStubOnInflateListener);

CNotificationsQuickSettingsContainer::InnerListener::InnerListener(
    /* [in] */ CNotificationsQuickSettingsContainer* host)
    : mHost(host)
{}

// @Override
ECode CNotificationsQuickSettingsContainer::InnerListener::OnInflate(
    /* [in] */ IViewStub* stub,
    /* [in] */ IView* inflated)
{
    return mHost->OnInflate(stub, inflated);
}

CAR_OBJECT_IMPL(CNotificationsQuickSettingsContainer)

CAR_INTERFACE_IMPL(CNotificationsQuickSettingsContainer, FrameLayout, INotificationsQuickSettingsContainer);

CNotificationsQuickSettingsContainer::CNotificationsQuickSettingsContainer()
    : mInflated(FALSE)
{}

ECode CNotificationsQuickSettingsContainer::OnFinishInflate()
{
    FrameLayout::OnFinishInflate();
    FindViewById(R::id::scroll_view, (IView**)&mScrollView);
    FindViewById(R::id::notification_stack_scroller, (IView**)&mStackScroller);
    FindViewById(R::id::keyguard_header, (IView**)&mKeyguardStatusBar);
    AutoPtr<IView> view;
    FindViewById(R::id::keyguard_user_switcher, (IView**)&view);
    AutoPtr<IViewStub> userSwitcher = IViewStub::Probe(view);
    AutoPtr<InnerListener> listener = new InnerListener(this);
    userSwitcher->SetOnInflateListener(listener);
    mUserSwitcher = view;
    return NOERROR;
}

ECode CNotificationsQuickSettingsContainer::OnApplyWindowInsets(
    /* [in] */ IWindowInsets* insets,
    /* [out] */ IWindowInsets** result)
{
    VALIDATE_NOT_NULL(result);
    Int32 b = 0;
    insets->GetSystemWindowInsetBottom(&b);
    SetPadding(0, 0, 0, b);
    *result = insets;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

Boolean CNotificationsQuickSettingsContainer::DrawChild(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IView* child,
    /* [in] */ Int64 drawingTime)
{
    Int32 v = 0;
    Boolean userSwitcherVisible = mInflated && (mUserSwitcher->GetVisibility(&v), v) == IView::VISIBLE;
    Boolean statusBarVisible = (mKeyguardStatusBar->GetVisibility(&v), v) == IView::VISIBLE;

    // Invert the order of the scroll view and user switcher such that the notifications receive
    // touches first but the panel gets drawn above.
    if (child == mScrollView) {
        return FrameLayout::DrawChild(canvas, mStackScroller, drawingTime);
    }
    else if (child == mStackScroller) {
        return FrameLayout::DrawChild(canvas,
                userSwitcherVisible && statusBarVisible ? mUserSwitcher
                : statusBarVisible ? mKeyguardStatusBar
                : userSwitcherVisible ? mUserSwitcher
                : mScrollView,
                drawingTime);
    }
    else if (child == mUserSwitcher) {
        return FrameLayout::DrawChild(canvas,
                userSwitcherVisible && statusBarVisible ? mKeyguardStatusBar
                : mScrollView,
                drawingTime);
    }
    else if (child == mKeyguardStatusBar) {
        return FrameLayout::DrawChild(canvas,
                userSwitcherVisible && statusBarVisible ? mScrollView
                : mScrollView,
                drawingTime);
    }
    return FrameLayout::DrawChild(canvas, child, drawingTime);
}

ECode CNotificationsQuickSettingsContainer::OnInflate(
    /* [in] */ IViewStub* stub,
    /* [in] */ IView* inflated)
{
    if (IView::Probe(stub) == mUserSwitcher) {
        mUserSwitcher = inflated;
        mInflated = TRUE;
    }
    return NOERROR;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
