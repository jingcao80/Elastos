
#include "elastos/droid/systemui/statusbar/phone/CNotificationsQuickSettingsContainer.h"
#include "../../R.h"

using Elastos::Droid::View::EIID_IViewStubOnInflateListener;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_OBJECT_IMPL(CNotificationsQuickSettingsContainer);
CAR_INTERFACE_IMPL_2(CNotificationsQuickSettingsContainer, FrameLayout, INotificationsQuickSettingsContainer \
    , IViewStubOnInflateListener);
CNotificationsQuickSettingsContainer::CNotificationsQuickSettingsContainer()
    : mInflated(FALSE)
{}

ECode CNotificationsQuickSettingsContainer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return FrameLayout::constructor(context, attrs);
}

ECode CNotificationsQuickSettingsContainer::OnFinishInflate()
{
    FrameLayout::OnFinishInflate();
    FindViewById(R::id::scroll_view, (IView**)&mScrollView);
    FindViewById(R::id::notification_stack_scroller, (IView**)&mStackScroller);
    FindViewById(R::id::keyguard_header, (IView**)&mKeyguardStatusBar);
    AutoPtr<IView> view;
    FindViewById(R::id::keyguard_user_switcher, (IView**)&view);
    AutoPtr<IViewStub> userSwitcher = IViewStub::Probe(view);
    userSwitcher->SetOnInflateListener(this);
    mUserSwitcher = IView::Probe(userSwitcher);
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
