
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/inputmethod/pinyin/CBalloonHint.h"

using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::View;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

const Int32 CBalloonHint::BalloonTimer::ACTION_SHOW;
const Int32 CBalloonHint::BalloonTimer::ACTION_HIDE;
const Int32 CBalloonHint::BalloonTimer::ACTION_UPDATE;

CBalloonHint::BalloonTimer::BalloonTimer(
    /* [in]*/ CBalloonHint* host)
    : HandlerRunnable()
    , mAction(0)
    , mWidth(0)
    , mHeight(0)
    , mTimerPending(FALSE)
    , mHost(host)
{
    mPositionInParent = ArrayOf<Int32>::Alloc(2);
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
        (*mPositionInParent)[0] = (*positionInParent)[0];
        (*mPositionInParent)[1] = (*positionInParent)[1];
    }
    mWidth = width;
    mHeight = height;
    Boolean result = FALSE;
    PostDelayed(this, tm, &result);
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
        RemoveCallbacks(this);
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
            mHost->mParent->GetLocationInWindow(mHost->mParentLocationInWindow);
            mHost->ShowAtLocation(mHost->mParent, IGravity::LEFT | IGravity::TOP,
                    (*mPositionInParent)[0], (*mPositionInParent)[1]
                            + (*mHost->mParentLocationInWindow)[1]);
            break;
        }
        case ACTION_HIDE:
            mHost->Dismiss();
            break;
        case ACTION_UPDATE: {
            mHost->mParent->GetLocationInWindow(mHost->mParentLocationInWindow);
            mHost->Update((*mPositionInParent)[0], (*mPositionInParent)[1]
                    + (*mHost->mParentLocationInWindow)[1], mWidth, mHeight);
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
    mParentLocationInWindow = ArrayOf<Int32>::Alloc(2);
}

ECode CBalloonHint::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IView* parent,
    /* [in] */ Int32 measureSpecMode)
{
    FAIL_RETURN(PopupWindow::constructor(context));
    mParent = parent;
    mMeasureSpecMode = measureSpecMode;

    SetInputMethodMode(IPopupWindow::INPUT_METHOD_NOT_NEEDED);
    SetTouchable(FALSE);

    AutoPtr<IColorDrawable> colorDr;
    CColorDrawable::New(0, (IColorDrawable**)&colorDr);
    SetBackgroundDrawable(IDrawable::Probe(colorDr));

    CBalloonView::NewByFriend(context, (CBalloonView**)&mBalloonView);
    mBalloonView->SetClickable(FALSE);
    SetContentView(mBalloonView);

    mBalloonTimer = new BalloonTimer(this);
    return NOERROR;
}

AutoPtr<IContext> CBalloonHint::GetContext()
{
    return mContext;
}

AutoPtr<IRect> CBalloonHint::GetPadding()
{
    return mPaddingRect;
}

void CBalloonHint::SetBalloonBackground(
    /* [in] */ IDrawable* drawable)
{
    // We usually pick up a background from a soft keyboard template,
    // and the object may has been set to this balloon before.
    AutoPtr<IDrawable> dr;
    mBalloonView->GetBackground((IDrawable**)&dr);
    if (dr.Get() == drawable) return;
    mBalloonView->SetBackgroundDrawable(drawable);

    if (NULL != drawable) {
        Boolean result = FALSE;
        drawable->GetPadding(mPaddingRect, &result);
    }
    else {
        mPaddingRect->Set(0, 0, 0, 0);
    }
}

void CBalloonHint::SetBalloonConfig(
    /* [in] */ const String& label,
    /* [in] */ Float textSize,
    /* [in] */ Boolean textBold,
    /* [in] */ Int32 textColor,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mBalloonView->SetTextConfig(label, textSize, textBold, textColor);
    SetBalloonSize(width, height);
}

void CBalloonHint::SetBalloonConfig(
    /* [in] */ IDrawable* icon,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mBalloonView->SetIcon(icon);
    SetBalloonSize(width, height);
}

Boolean CBalloonHint::NeedForceDismiss()
{
    return mForceDismiss;
}

Int32 CBalloonHint::GetPaddingLeft()
{
    Int32 value;
    mPaddingRect->GetLeft(&value);
    return value;
}

Int32 CBalloonHint::GetPaddingTop()
{
    Int32 value;
    mPaddingRect->GetTop(&value);
    return value;
}

Int32 CBalloonHint::GetPaddingRight()
{
    Int32 value;
    mPaddingRect->GetRight(&value);
    return value;
}

Int32 CBalloonHint::GetPaddingBottom()
{
    Int32 value;
    mPaddingRect->GetBottom(&value);
    return value;
}

void CBalloonHint::DelayedShow(
    /* [in] */ Int64 delay,
    /* [in] */ ArrayOf<Int32>* locationInParent)
{
    if (mBalloonTimer->IsPending()) {
        mBalloonTimer->RemoveTimer();
    }
    if (delay <= 0) {
        mParent->GetLocationInWindow(mParentLocationInWindow);
        ShowAtLocation(mParent, IGravity::LEFT | IGravity::TOP,
                (*locationInParent)[0], (*locationInParent)[1]
                        + (*mParentLocationInWindow)[1]);
    }
    else {
        mBalloonTimer->StartTimer(delay, BalloonTimer::ACTION_SHOW,
                locationInParent, -1, -1);
    }
}

void CBalloonHint::DelayedUpdate(
    /* [in] */ Int64 delay,
    /* [in] */ ArrayOf<Int32>* locationInParent,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mBalloonView->Invalidate();
    if (mBalloonTimer->IsPending()) {
        mBalloonTimer->RemoveTimer();
    }
    if (delay <= 0) {
        mParent->GetLocationInWindow(mParentLocationInWindow);
        Update((*locationInParent)[0], (*locationInParent)[1]
                + (*mParentLocationInWindow)[1], width, height);
    }
    else {
        mBalloonTimer->StartTimer(delay, BalloonTimer::ACTION_UPDATE,
                locationInParent, width, height);
    }
}

void CBalloonHint::DelayedDismiss(
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
    }
    else {
        mBalloonTimer->StartTimer(delay, BalloonTimer::ACTION_HIDE, NULL, -1,
                -1);
    }
}

void CBalloonHint::RemoveTimer()
{
    if (mBalloonTimer->IsPending()) {
        mBalloonTimer->RemoveTimer();
    }
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
    Int32 data1 = 0;
    Int32 newWidth = (mBalloonView->GetMeasuredWidth(&data1), data1) + GetPaddingLeft()
            + GetPaddingRight();
    Int32 newHeight = (mBalloonView->GetMeasuredHeight(&data1), data1) + GetPaddingTop()
            + GetPaddingBottom();
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

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
