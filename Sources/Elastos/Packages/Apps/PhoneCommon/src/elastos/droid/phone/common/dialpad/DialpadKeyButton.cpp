
#include "elastos/droid/phone/common/dialpad/DialpadKeyButton.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Dialpad {

ECode DialpadKeyButton::MyRunnable::Run()
{
    mHost->SetLongHovered(TRUE);
    mHost->AnnounceForAccessibility(mHost->mLongHoverContentDesc);
    return NOERROR;
}


const Int32 DialpadKeyButton::LONG_HOVER_TIMEOUT = DialpadKeyButton::Init_LONG_HOVER_TIMEOUT();

CAR_INTERFACE_IMPL(DialpadKeyButton, FrameLayout, IDialpadKeyButton);

DialpadKeyButton::DialpadKeyButton()
    : mLongHovered(FALSE)
    , mWasClickable(FALSE)
    , mWasLongClickable(FALSE)
{
    Logger::E("DialpadKeyButton", "====[snow]===== DialpadKeyButton::DialpadKeyButton");
    CRect::New((IRect**)&mHoverBounds);
}

DialpadKeyButton::~DialpadKeyButton()
{
    Logger::E("DialpadKeyButton", "====[snow]===== DialpadKeyButton::~DialpadKeyButton");
}

ECode DialpadKeyButton::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(FrameLayout::constructor(ctx, attrs));
    InitForAccessibility(ctx);
    return NOERROR;
}

ECode DialpadKeyButton::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FAIL_RETURN(FrameLayout::constructor(ctx, attrs, defStyle));
    InitForAccessibility(ctx);
    return NOERROR;
}

ECode DialpadKeyButton::OnFinishInflate()
{
    Int32 id;
    GetId(&id);
    Logger::E("DialpadKeyButton", "====[snow]===== DialpadKeyButton::OnFinishInflate id=%x", id);
    return NOERROR;
}

ECode DialpadKeyButton::SetOnPressedListener(
    /* [in] */ IDialpadKeyButtonOnPressedListener* onPressedListener)
{
    mOnPressedListener = onPressedListener;
    return NOERROR;
}

void DialpadKeyButton::InitForAccessibility(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
    mAccessibilityManager = IAccessibilityManager::Probe(obj);
}

ECode DialpadKeyButton::SetLongHoverContentDescription(
    /* [in] */ ICharSequence* contentDescription)
{
    mLongHoverContentDesc = contentDescription;

    if (mLongHovered) {
        FrameLayout::SetContentDescription(mLongHoverContentDesc);
    }
    return NOERROR;
}

ECode DialpadKeyButton::SetContentDescription(
    /* [in] */ ICharSequence* contentDescription)
{
    if (mLongHovered) {
        mBackupContentDesc = contentDescription;
    }
    else {
        FrameLayout::SetContentDescription(contentDescription);
    }
    return NOERROR;
}

// @Override
ECode DialpadKeyButton::SetPressed(
    /* [in] */ Boolean pressed)
{
    FrameLayout::SetPressed(pressed);
    if (mOnPressedListener != NULL) {
        mOnPressedListener->OnPressed(this, pressed);
    }
    return NOERROR;
}

// @Override
ECode DialpadKeyButton::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    FrameLayout::OnSizeChanged(w, h, oldw, oldh);

    Int32 pl, pr, pt, pb;
    GetPaddingLeft(&pl);
    GetPaddingRight(&pr);
    GetPaddingTop(&pt);
    GetPaddingBottom(&pb);

    mHoverBounds->Set(pl, w - pr, pt, h - pb);
    return NOERROR;
}

// @Override
ECode DialpadKeyButton::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (action == IAccessibilityNodeInfo::ACTION_CLICK) {
        SimulateClickForAccessibility();
        *result = TRUE;
        return NOERROR;
    }

    return FrameLayout::PerformAccessibilityAction(action, arguments, result);
}

// @Override
ECode DialpadKeyButton::OnHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // When touch exploration is turned on, lifting a finger while inside
    // the button's hover target bounds should perform a click action.
    if ((mAccessibilityManager->IsEnabled(result), *result)
            && (mAccessibilityManager->IsTouchExplorationEnabled(result), *result)) {
        Int32 mask;
        event->GetActionMasked(&mask);
        switch (mask) {
            case IMotionEvent::ACTION_HOVER_ENTER:
                // Lift-to-type temporarily disables double-tap activation.
                IsClickable(&mWasClickable);
                IsLongClickable(&mWasLongClickable);
                if (mWasLongClickable && mLongHoverContentDesc != NULL) {
                    if (mLongHoverRunnable == NULL) {
                        mLongHoverRunnable = new MyRunnable(this);
                    }
                    PostDelayed(mLongHoverRunnable, LONG_HOVER_TIMEOUT, result);
                }

                SetClickable(FALSE);
                SetLongClickable(FALSE);
                break;
            case IMotionEvent::ACTION_HOVER_EXIT: {
                Float x, y;
                event->GetX(&x);
                event->GetY(&y);
                if (mHoverBounds->Contains((Int32)x, (Int32)y, result), *result) {
                    if (mLongHovered) {
                        PerformLongClick(result);
                    }
                    else {
                        SimulateClickForAccessibility();
                    }
                }

                CancelLongHover();
                SetClickable(mWasClickable);
                SetLongClickable(mWasLongClickable);
                break;
            }
        }
    }

    return FrameLayout::OnHoverEvent(event, result);
}

void DialpadKeyButton::SimulateClickForAccessibility()
{
    // Checking the press state prevents double activation.
    Boolean pressed;
    if (IsPressed(&pressed), pressed) {
        return;
    }

    SetPressed(TRUE);

    // Stay consistent with performClick() by sending the event after
    // setting the pressed state but before performing the action.
    SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_CLICKED);

    SetPressed(FALSE);
}

void DialpadKeyButton::SetLongHovered(
    /* [in] */ Boolean enabled)
{
    if (mLongHovered != enabled) {
        mLongHovered = enabled;

        // Switch between normal and alternate description, if available.
        if (enabled) {
            mBackupContentDesc = NULL;
            GetContentDescription((ICharSequence**)&mBackupContentDesc);
            FrameLayout::SetContentDescription(mLongHoverContentDesc);
        }
        else {
            FrameLayout::SetContentDescription(mBackupContentDesc);
        }
    }
}

void DialpadKeyButton::CancelLongHover()
{
    if (mLongHoverRunnable != NULL) {
        Boolean result;
        RemoveCallbacks(mLongHoverRunnable, &result);
    }
    SetLongHovered(FALSE);
}

Int32 DialpadKeyButton::Init_LONG_HOVER_TIMEOUT()
{
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    Int32 timeout;
    helper->GetLongPressTimeout(&timeout);
    return timeout * 2;
}

} // namespace Dialpad
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos
