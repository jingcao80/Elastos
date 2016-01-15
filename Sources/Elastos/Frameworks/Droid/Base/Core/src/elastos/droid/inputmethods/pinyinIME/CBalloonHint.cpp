
#include "CBalloonHint.h"
#include "elastos/droid/graphics/drawable/CColorDrawable.h"
#include "CBalloonView.h"

using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::View::IGravity;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

const Int32 CBalloonHint::BalloonTimer::ACTION_SHOW;
const Int32 CBalloonHint::BalloonTimer::ACTION_HIDE;
const Int32 CBalloonHint::BalloonTimer::ACTION_UPDATE;

CBalloonHint::BalloonTimer::BalloonTimer(
    /* [in]*/ CBalloonHint* host)
    : mAction(0)
    , mWidth(0)
    , mHeight(0)
    , mTimerPending(FALSE)
    , mHost(host)
{
    memset(mPositionInParent, 0, sizeof(mPositionInParent));
}

void CBalloonHint::BalloonTimer::StartTimer(
    /* [in] */ Int64 tm,
    /* [in] */ Int32 action,
    /* [in] */ ArrayOf<Int32>* positionInParent,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mAction = action;
    if (ACTION_HIDE != action) {
        assert(positionInParent != NULL);
        mPositionInParent[0] = (*positionInParent)[0];
        mPositionInParent[1] = (*positionInParent)[1];
    }
    mWidth = width;
    mHeight = height;
    Boolean result = FALSE;
    Handler::PostDelayed(this, tm, &result);
    mTimerPending = TRUE;
}

Boolean CBalloonHint::BalloonTimer::IsPending()
{
    return mTimerPending;
}

Boolean CBalloonHint::BalloonTimer::RemoveTimer()
{
    if (mTimerPending) {
        mTimerPending = FALSE;
        Handler::RemoveCallbacks(this);
        return TRUE;
    }

    return FALSE;
}

Int32 CBalloonHint::BalloonTimer::GetAction()
{
    return mAction;
}

ECode CBalloonHint::BalloonTimer::Run()
{
    switch (mAction) {
        case ACTION_SHOW: {
            Int32 x = 0, y = 0;
            mHost->mParent->GetLocationInWindow(&x, &y);
            mHost->mParentLocationInWindow[0] = x;
            mHost->mParentLocationInWindow[1] = y;
            mHost->ShowAtLocation(mHost->mParent, IGravity::LEFT | IGravity::TOP,
                    mPositionInParent[0], mPositionInParent[1]
                            + mHost->mParentLocationInWindow[1]);
            break;
        }
        case ACTION_HIDE:
            mHost->Dismiss();
            break;
        case ACTION_UPDATE: {
            Int32 x = 0, y = 0;
            mHost->mParent->GetLocationInWindow(&x, &y);
            mHost->mParentLocationInWindow[0] = x;
            mHost->mParentLocationInWindow[1] = y;
            mHost->Update(mPositionInParent[0], mPositionInParent[1]
                    + mHost->mParentLocationInWindow[1], mWidth, mHeight);
        }
    }
    mTimerPending = FALSE;
    return NOERROR;
}

const Int32 CBalloonHint::TIME_DELAY_SHOW = 0;
const Int32 CBalloonHint::TIME_DELAY_DISMISS = 200;
CAR_OBJECT_IMPL(CBalloonHint);
CAR_INTERFACE_IMPL(CBalloonHint, PopupWindow, IBalloonHint);
CBalloonHint::CBalloonHint()
    : mMeasureSpecMode(0)
    , mForceDismiss(FALSE)
{
    CRect::New((IRect**)&mPaddingRect);
    memset(mParentLocationInWindow, 0, sizeof(mParentLocationInWindow));
}

ECode CBalloonHint::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IView* parent,
    /* [in] */ Int32 measureSpecMode)
{
    Init(context);
    mParent = parent;
    mMeasureSpecMode = measureSpecMode;

    SetInputMethodMode(IPopupWindow::INPUT_METHOD_NOT_NEEDED);
    SetTouchable(FALSE);

    AutoPtr<IColorDrawable> colorDr;
    CColorDrawable::New(0, (IColorDrawable**)&colorDr);
    SetBackgroundDrawable(colorDr);

    CBalloonView::New(context, (IBalloonView**)&mBalloonView);
    mBalloonView->SetClickable(FALSE);
    SetContentView(mBalloonView);

    mBalloonTimer = new BalloonTimer(this);
    return NOERROR;
}

ECode CBalloonHint::GetContext(
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx);
    *ctx = mContext;
    REFCOUNT_ADD(*ctx);
    return NOERROR;
}

ECode CBalloonHint::GetPadding(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);
    *rect = mPaddingRect;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

ECode CBalloonHint::SetBalloonBackground(
    /* [in] */ IDrawable* drawable)
{
    // We usually pick up a background from a soft keyboard template,
    // and the object may has been set to this balloon before.
    AutoPtr<IDrawable> dr;
    mBalloonView->GetBackground((IDrawable**)&dr);
    if (dr.Get() == drawable) return NOERROR;
    mBalloonView->SetBackgroundDrawable(drawable);

    if (NULL != drawable) {
        Boolean result = FALSE;
        drawable->GetPadding(mPaddingRect, &result);
    } else {
        mPaddingRect->Set(0, 0, 0, 0);
    }
    return NOERROR;
}

ECode CBalloonHint::SetBalloonConfig(
    /* [in] */ const String& label,
    /* [in] */ Float textSize,
    /* [in] */ Boolean textBold,
    /* [in] */ Int32 textColor,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mBalloonView->SetTextConfig(label, textSize, textBold, textColor);
    SetBalloonSize(width, height);
    return NOERROR;
}

ECode CBalloonHint::SetBalloonConfig(
    /* [in] */ IDrawable* icon,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mBalloonView->SetIcon(icon);
    SetBalloonSize(width, height);
    return NOERROR;
}

ECode CBalloonHint::NeedForceDismiss(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mForceDismiss;
    return NOERROR;
}

ECode CBalloonHint::GetPaddingLeft(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    return mPaddingRect->GetLeft(value);
}

ECode CBalloonHint::GetPaddingTop(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    return mPaddingRect->GetTop(value);
}

ECode CBalloonHint::GetPaddingRight(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    return mPaddingRect->GetRight(value);
}

ECode CBalloonHint::GetPaddingBottom(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    return mPaddingRect->GetBottom(value);
}

ECode CBalloonHint::DelayedShow(
    /* [in] */ Int64 delay,
    /* [in] */ ArrayOf<Int32>* locationInParent)
{
    assert(locationInParent != NULL);
    if (mBalloonTimer->IsPending()) {
        mBalloonTimer->RemoveTimer();
    }
    if (delay <= 0) {
        Int32 x = 0, y = 0;
        mParent->GetLocationInWindow(&x, &y);
        mParentLocationInWindow[0] = x;
        mParentLocationInWindow[1] = y;
        ShowAtLocation(mParent, IGravity::LEFT | IGravity::TOP,
                (*locationInParent)[0], (*locationInParent)[1]
                        + mParentLocationInWindow[1]);
    } else {
        mBalloonTimer->StartTimer(delay, BalloonTimer::ACTION_SHOW,
                locationInParent, -1, -1);
    }
    return NOERROR;
}

ECode CBalloonHint::DelayedUpdate(
    /* [in] */ Int64 delay,
    /* [in] */ ArrayOf<Int32>* locationInParent,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    assert(locationInParent != NULL);
    mBalloonView->Invalidate();
    if (mBalloonTimer->IsPending()) {
        mBalloonTimer->RemoveTimer();
    }
    if (delay <= 0) {
        Int32 x = 0, y = 0;
        mParent->GetLocationInWindow(&x, &y);
        mParentLocationInWindow[0] = x;
        mParentLocationInWindow[1] = y;
        Update((*locationInParent)[0], (*locationInParent)[1]
                + mParentLocationInWindow[1], width, height);
    } else {
        mBalloonTimer->StartTimer(delay, BalloonTimer::ACTION_UPDATE,
                locationInParent, width, height);
    }

    return NOERROR;
}

ECode CBalloonHint::DelayedDismiss(
    /* [in] */ Int64 delay)
{
    if (mBalloonTimer->IsPending()) {
        mBalloonTimer->RemoveTimer();
        Int32 pendingAction = mBalloonTimer->GetAction();
        if (0 != delay && BalloonTimer::ACTION_HIDE != pendingAction) {
            mBalloonTimer->Run();
        }
    }
    if (delay <= 0) {
        Dismiss();
    } else {
        mBalloonTimer->StartTimer(delay, BalloonTimer::ACTION_HIDE, NULL, -1,
                -1);
    }

    return NOERROR;
}

ECode CBalloonHint::RemoveTimer()
{
    if (mBalloonTimer->IsPending()) {
        mBalloonTimer->RemoveTimer();
    }
    return NOERROR;
}

void CBalloonHint::SetBalloonSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Int32 widthMeasureSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(width,
            mMeasureSpecMode);
    Int32 heightMeasureSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(height,
            mMeasureSpecMode);
    mBalloonView->Measure(widthMeasureSpec, heightMeasureSpec);

    Int32 oldWidth = 0;
    GetWidth(&oldWidth);
    Int32 oldHeight = 0;
    GetHeight(&oldHeight);
    Int32 data1 = 0, data2 = 0, data3 = 0;
    Int32 newWidth = (mBalloonView->GetMeasuredWidth(&data1), data1) + (GetPaddingLeft(&data2), data2)
            + (GetPaddingRight(&data3), data3);
    Int32 newHeight = (mBalloonView->GetMeasuredHeight(&data1), data1) + (GetPaddingTop(&data2), data2)
            + (GetPaddingBottom(&data3), data3);
    SetWidth(newWidth);
    SetHeight(newHeight);

    // If update() is called to update both size and position, the system
    // will first MOVE the PopupWindow to the new position, and then
    // perform a size-updating operation, so there will be a flash in
    // PopupWindow if user presses a key and moves finger to next one whose
    // size is different.
    // PopupWindow will handle the updating issue in one go in the future,
    // but before that, if we find the size is changed, a mandatory dismiss
    // operation is required. In our UI design, normal QWERTY keys' width
    // can be different in 1-pixel, and we do not dismiss the balloon when
    // user move between QWERTY keys.
    mForceDismiss = FALSE;
    Boolean isShowing = FALSE;
    if (IsShowing(&isShowing), isShowing) {
        mForceDismiss = oldWidth - newWidth > 1 || newWidth - oldWidth > 1;
    }
}

ECode CBalloonHint::SetAllowScrollingAnchorParent(
        /* [in] */ Boolean enabled)
{
    return PopupWindow::SetAllowScrollingAnchorParent(enabled);
}

ECode CBalloonHint::GetBackground(
    /* [out] */ IDrawable** background)
{
    VALIDATE_NOT_NULL(background);
    AutoPtr<IDrawable> bk = PopupWindow::GetBackground();
    *background = bk.Get();
    REFCOUNT_ADD(*background);
    return NOERROR;
}

ECode CBalloonHint::SetBackgroundDrawable(
    /* [in] */ IDrawable* background)
{
    return PopupWindow::SetBackgroundDrawable(background);
}

ECode CBalloonHint::GetAnimationStyle(
    /* [out] */ Int32* style)
{
    VALIDATE_NOT_NULL(style);
    *style = PopupWindow::GetAnimationStyle();
    return NOERROR;
}

ECode CBalloonHint::SetIgnoreCheekPress()
{
    return PopupWindow::SetIgnoreCheekPress();
}

ECode CBalloonHint::SetAnimationStyle(
    /* [in] */ Int32 animationStyle)
{
    return PopupWindow::SetAnimationStyle(animationStyle);
}

ECode CBalloonHint::GetContentView(
    /* [out] */ IView** contentView)
{
    VALIDATE_NOT_NULL(contentView);
    AutoPtr<IView> cv = PopupWindow::GetContentView();
    *contentView = cv.Get();
    REFCOUNT_ADD(*contentView);
    return NOERROR;
}

ECode CBalloonHint::SetContentView(
    /* [in] */ IView* contentView)
{
    return PopupWindow::SetContentView(contentView);
}

ECode CBalloonHint::SetTouchInterceptor(
    /* [in] */ IViewOnTouchListener* l)
{
    return PopupWindow::SetTouchInterceptor(l);
}

ECode CBalloonHint::IsFocusable(
    /* [out] */ Boolean* isFocusable)
{
    VALIDATE_NOT_NULL(isFocusable);
    *isFocusable = PopupWindow::IsFocusable();
    return NOERROR;
}

ECode CBalloonHint::SetFocusable(
     /* [in] */ Boolean focusable)
{
    return PopupWindow::SetFocusable(focusable);
}

ECode CBalloonHint::GetInputMethodMode(
    /* [out] */ Int32* inputMethodMode)
{
    VALIDATE_NOT_NULL(inputMethodMode);
    *inputMethodMode = PopupWindow::GetInputMethodMode();
    return NOERROR;
}

ECode CBalloonHint::SetInputMethodMode(
    /* [in] */ Int32 mode)
{
    return PopupWindow::SetInputMethodMode(mode);
}

ECode CBalloonHint::SetSoftInputMode(
    /* [in] */ Int32 mode)
{
    return PopupWindow::SetSoftInputMode(mode);
}

ECode CBalloonHint::GetSoftInputMode(
    /* [out] */ Int32* softInputMode)
{
    VALIDATE_NOT_NULL(softInputMode);
    *softInputMode = PopupWindow::GetSoftInputMode();
    return NOERROR;
}

ECode CBalloonHint::IsTouchable(
    /* [out] */ Boolean* touchable)
{
    VALIDATE_NOT_NULL(touchable);
    *touchable = PopupWindow::IsTouchable();
    return NOERROR;
}

ECode CBalloonHint::SetTouchable(
    /* [in] */ Boolean touchable)
{
    return PopupWindow::SetTouchable(touchable);
}

ECode CBalloonHint::IsOutsideTouchable(
    /* [out] */ Boolean* touchable)
{
    VALIDATE_NOT_NULL(touchable);
    *touchable = PopupWindow::IsOutsideTouchable();
    return NOERROR;
}

ECode CBalloonHint::SetOutsideTouchable(
    /* [in] */ Boolean touchable)
{
    return PopupWindow::SetOutsideTouchable(touchable);
}

ECode CBalloonHint::IsClippingEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = PopupWindow::IsClippingEnabled();
    return NOERROR;
}

ECode CBalloonHint::SetClippingEnabled(
    /* [in] */ Boolean enabled)
{
    return PopupWindow::SetClippingEnabled(enabled);
}

ECode CBalloonHint::SetClipToScreenEnabled(
    /* [in] */ Boolean enabled)
{
    return PopupWindow::SetClipToScreenEnabled(enabled);
}

ECode CBalloonHint::IsSplitTouchEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = PopupWindow::IsSplitTouchEnabled();
    return NOERROR;
}

ECode CBalloonHint::SetSplitTouchEnabled(
    /* [in] */ Boolean enabled)
{
    return PopupWindow::SetSplitTouchEnabled(enabled);
}

ECode CBalloonHint::IsLayoutInScreenEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = PopupWindow::IsLayoutInScreenEnabled();
    return NOERROR;
}

ECode CBalloonHint::SetLayoutInScreenEnabled(
    /* [in] */ Boolean enabled)
{
    return PopupWindow::SetLayoutInScreenEnabled(enabled);
}

ECode CBalloonHint::SetLayoutInsetDecor(
    /* [in] */ Boolean enabled)
{
    return PopupWindow::SetLayoutInsetDecor(enabled);
}

ECode CBalloonHint::SetWindowLayoutType(
    /* [in] */ Int32 layoutType)
{
    return PopupWindow::SetWindowLayoutType(layoutType);
}

ECode CBalloonHint::GetWindowLayoutType(
    /* [out] */ Int32* layoutType)
{
    VALIDATE_NOT_NULL(layoutType);
    *layoutType = PopupWindow::GetWindowLayoutType();
    return NOERROR;
}

ECode CBalloonHint::SetTouchModal(
    /* [in] */ Boolean touchModal)
{
    return PopupWindow::SetTouchModal(touchModal);
}

ECode CBalloonHint::SetWindowLayoutMode(
    /* [in] */ Int32 widthSpec,
    /* [in] */ Int32 heightSpec)
{
    return PopupWindow::SetWindowLayoutMode(widthSpec, heightSpec);
}

ECode CBalloonHint::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = PopupWindow::GetHeight();
    return NOERROR;
}

ECode CBalloonHint::SetHeight(
    /* [in] */ Int32 height)
{
    return PopupWindow::SetHeight(height);
}

ECode CBalloonHint::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = PopupWindow::GetWidth();
    return NOERROR;
}

ECode CBalloonHint::SetWidth(
    /* [in] */ Int32 width)
{
    return PopupWindow::SetWidth(width);
}

ECode CBalloonHint::IsShowing(
    /* [out] */ Boolean* isShowing)
{
    VALIDATE_NOT_NULL(isShowing);
    *isShowing = PopupWindow::IsShowing();
    return NOERROR;
}

ECode CBalloonHint::ShowAtLocation(
    /* [in] */ IView* parent,
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    return PopupWindow::ShowAtLocation(parent, gravity, x, y);
}

ECode CBalloonHint::ShowAtLocation(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    return PopupWindow::ShowAtLocation(token, gravity, x, y);
}

ECode CBalloonHint::ShowAsDropDown(
    /* [in] */ IView* anchor)
{
    return PopupWindow::ShowAsDropDown(anchor);
}

ECode CBalloonHint::ShowAsDropDown(
    /* [in] */ IView* anchor,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff)
{
    return PopupWindow::ShowAsDropDown(anchor, xoff, yoff);
}

ECode CBalloonHint::IsAboveAnchor(
    /* [out] */ Boolean* isAboveAnchor)
{
    VALIDATE_NOT_NULL(isAboveAnchor);
    *isAboveAnchor = PopupWindow::IsAboveAnchor();
    return NOERROR;
}

ECode CBalloonHint::GetMaxAvailableHeight(
    /* [in] */ IView* anchor,
    /* [out] */ Int32* maxAvailableHeight)
{
    VALIDATE_NOT_NULL(maxAvailableHeight);
    *maxAvailableHeight = PopupWindow::GetMaxAvailableHeight(anchor);
    return NOERROR;
}

ECode CBalloonHint::GetMaxAvailableHeight(
    /* [in] */ IView* anchor,
    /* [in] */ Int32 yOffset,
    /* [out] */ Int32* maxAvailableHeight)
{
    VALIDATE_NOT_NULL(maxAvailableHeight);
    *maxAvailableHeight = PopupWindow::GetMaxAvailableHeight(anchor, yOffset);
    return NOERROR;
}

ECode CBalloonHint::GetMaxAvailableHeight(
    /* [in] */ IView* anchor,
    /* [in] */ Int32 yOffset,
    /* [in] */ Boolean ignoreBottomDecorations,
    /* [out] */ Int32* maxAvailableHeight)
{
    VALIDATE_NOT_NULL(maxAvailableHeight);
    *maxAvailableHeight = PopupWindow::GetMaxAvailableHeight(anchor, yOffset, ignoreBottomDecorations);
    return NOERROR;
}

ECode CBalloonHint::Dismiss()
{
    return PopupWindow::Dismiss();
}

ECode CBalloonHint::SetOnDismissListener(
    /* [in] */ IPopupWindowOnDismissListener* l)
{
    return PopupWindow::SetOnDismissListener(l);
}

ECode CBalloonHint::Update()
{
    return PopupWindow::Update();
}

ECode CBalloonHint::Update(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return PopupWindow::Update(width, height);
}

ECode CBalloonHint::Update(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return PopupWindow::Update(x, y, width, height);
}

ECode CBalloonHint::Update(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Boolean force)
{
    return PopupWindow::Update(x, y, width, height, force);
}

ECode CBalloonHint::Update(
    /* [in] */ IView* anchor,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return PopupWindow::Update(anchor, width, height);
}

ECode CBalloonHint::Update(
    /* [in] */ IView* anchor,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return PopupWindow::Update(anchor, xoff, yoff, width, height);
}

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos
