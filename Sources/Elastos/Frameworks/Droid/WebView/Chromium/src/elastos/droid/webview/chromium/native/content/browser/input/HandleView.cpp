
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/HandleView.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::R;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::ITypedValueHelper;
using Elastos::Droid::Utility::CTypedValueHelper;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::Widget::CPopupWindow;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

//==================================================================
//                    HandleView::InnerListener
//==================================================================

HandleView::InnerListener::InnerListener(
    /* [in] */ HandleView* owner)
    : mOwner(owner)
{
}

//@Override
ECode HandleView::InnerListener::OnPositionChanged(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    mOwner->UpdateParentPosition(x, y);
    return NOERROR;
}

//==================================================================
//                            HandleView
//==================================================================

const Float HandleView::FADE_DURATION = 200.f;

const Int32 HandleView::LEFT;
const Int32 HandleView::CENTER;
const Int32 HandleView::RIGHT;

// Number of dips to subtract from the handle's y position to give a suitable
// y coordinate for the corresponding text position. This is to compensate for the fact
// that the handle position is at the base of the line of text.
const Float HandleView::LINE_OFFSET_Y_DIP = 5.0f;

static AutoPtr< ArrayOf<Int32> > TEXT_VIEW_HANDLE_ATTRS_Init()
{
    AutoPtr< ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(3);
    (*array)[0] = R::attr::textSelectHandleLeft;
    (*array)[1] = R::attr::textSelectHandle;
    (*array)[2] = R::attr::textSelectHandleRight;
    return array;
}

const AutoPtr< ArrayOf<Int32> > HandleView::TEXT_VIEW_HANDLE_ATTRS = TEXT_VIEW_HANDLE_ATTRS_Init();

HandleView::HandleView(
    /* [in] */ CursorController* controller,
    /* [in] */ Int32 pos,
    /* [in] */ IView* parent,
    /* [in] */ PositionObserver* parentPositionObserver)
    : mPositionX(0)
    , mPositionY(0)
    , mParentPositionX(0)
    , mParentPositionY(0)
    , mHotspotX(0.0f)
    , mHotspotY(0.0f)
    , mController(controller)
    , mIsDragging(FALSE)
    , mTouchToWindowOffsetX(0.0f)
    , mTouchToWindowOffsetY(0.0f)
    , mDownPositionX(0.0f)
    , mDownPositionY(0.0f)
    , mTouchTimer(0)
    , mIsInsertionHandle(FALSE)
    , mAlpha(0.0f)
    , mFadeStartTime(0)
    , mParent(parent)
{
    AutoPtr<IContext> context;
    mParent->GetContext((IContext**)&context);
    View::constructor(context);
    CPopupWindow::New(context, NULL, R::attr::textSelectHandleWindowStyle,
         (IPopupWindow**)&mContainer);
    mContainer->SetSplitTouchEnabled(TRUE);
    mContainer->SetClippingEnabled(FALSE);
    mContainer->SetAnimationStyle(0);

    SetOrientation(pos);

    // Convert line offset dips to pixels.
    AutoPtr<ITypedValueHelper> helper;
    CTypedValueHelper::AcquireSingleton((ITypedValueHelper**)&helper);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> metrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float offsetY;
    helper->ApplyDimension(ITypedValue::COMPLEX_UNIT_DIP,
            LINE_OFFSET_Y_DIP, metrics, (Float*)&offsetY);
    mLineOffsetY = (Int32)offsetY;

    mAlpha = 1.f;

    mParentPositionListener = new InnerListener(this);
    mParentPositionObserver = parentPositionObserver;
}

void HandleView::SetOrientation(
    /* [in] */ Int32 pos)
{
    AutoPtr<IContext> context;
    mParent->GetContext((IContext**)&context);
    AutoPtr<IResourcesTheme> theme;
    context->GetTheme((IResourcesTheme**)&theme);
    AutoPtr<ITypedArray> a;
    theme->ObtainStyledAttributes(TEXT_VIEW_HANDLE_ATTRS, (ITypedArray**)&a);
    a->GetDrawable(pos, (IDrawable**)&mDrawable);
    a->Recycle();

    mIsInsertionHandle = (pos == CENTER);

    Int32 handleWidth;
    mDrawable->GetIntrinsicWidth(&handleWidth);
    switch (pos) {
        case LEFT: {
            mHotspotX = (handleWidth * 3) / 4.0f;
            break;
        }

        case RIGHT: {
            mHotspotX = handleWidth / 4.0f;
            break;
        }

        case CENTER:
        default: {
            mHotspotX = handleWidth / 2.0f;
            break;
        }
    }
    mHotspotY = 0;

    Invalidate();
}

//@Override
void HandleView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 width, height;
    mDrawable->GetIntrinsicWidth(&width);
    mDrawable->GetIntrinsicHeight(&height);
    SetMeasuredDimension(width, height);
}

void HandleView::UpdateParentPosition(
    /* [in] */ Int32 parentPositionX,
    /* [in] */ Int32 parentPositionY)
{
    // Hide paste popup window as soon as a scroll occurs.
    if (mPastePopupWindow != NULL) mPastePopupWindow->Hide();

    mTouchToWindowOffsetX += parentPositionX - mParentPositionX;
    mTouchToWindowOffsetY += parentPositionY - mParentPositionY;
    mParentPositionX = parentPositionX;
    mParentPositionY = parentPositionY;
    OnPositionChanged();
}

Int32 HandleView::GetContainerPositionX()
{
    return mParentPositionX + mPositionX;
}

Int32 HandleView::GetContainerPositionY()
{
    return mParentPositionY + mPositionY;
}

void HandleView::OnPositionChanged()
{
    // Deferring View invalidation while the handles are hidden prevents
    // scheduling conflicts with the compositor.
    Int32 visibility;
    if ((GetVisibility(&visibility), visibility) != VISIBLE) return;
    Int32 right, left, bottom, top;
    GetRight(&right);
    GetLeft(&left);
    GetBottom(&bottom);
    GetTop(&top);
    mContainer->Update(GetContainerPositionX(), GetContainerPositionY(),
             right - left, bottom - top);
}

void HandleView::ShowContainer()
{
    mContainer->ShowAtLocation(mParent, 0, GetContainerPositionX(), GetContainerPositionY());
}

void HandleView::Show()
{
    // While hidden, the parent position may have become stale. It must be updated before
    // checking isPositionVisible().
    UpdateParentPosition(mParentPositionObserver->GetPositionX(),
            mParentPositionObserver->GetPositionY());
    if (!IsPositionVisible()) {
        Hide();
        return;
    }
    mParentPositionObserver->AddListener(mParentPositionListener);
    mContainer->SetContentView(this);
    ShowContainer();

    // Hide paste view when handle is moved on screen.
    if (mPastePopupWindow != NULL) {
        mPastePopupWindow->Hide();
    }
}

void HandleView::Hide()
{
    mIsDragging = FALSE;
    mContainer->Dismiss();
    mParentPositionObserver->RemoveListener(mParentPositionListener);
    if (mPastePopupWindow != NULL) {
        mPastePopupWindow->Hide();
    }
}

Boolean HandleView::IsShowing()
{
    Boolean bFlag = FALSE;
    mContainer->IsShowing(&bFlag);
    return bFlag;
}

Boolean HandleView::IsPositionVisible()
{
    // Always show a dragging handle.
    if (mIsDragging) {
        return TRUE;
    }

    const AutoPtr<IRect> clip = mTempRect;
    clip->SetLeft(0);
    clip->SetTop(0);
    Int32 width;
    mParent->GetWidth(&width);
    clip->SetRight(width);
    Int32 height;
    mParent->GetHeight(&height);
    clip->SetBottom(height);

    AutoPtr<IViewParent> parent;
    mParent->GetParent((IViewParent**)&parent);
    Boolean bFlag = FALSE;
    if (parent == NULL || !(parent->GetChildVisibleRect(mParent, clip, NULL, &bFlag), bFlag)) {
        return FALSE;
    }

    const Int32 posX = GetContainerPositionX() + (Int32) mHotspotX;
    const Int32 posY = GetContainerPositionY() + (Int32) mHotspotY;

    Int32 left, right, top, bottom;
    clip->GetLeft(&left);
    clip->GetRight(&right);
    clip->GetTop(&top);
    clip->GetBottom(&bottom);
    return posX >= left && posX <= right &&
            posY >= top && posY <= bottom;
}

// x and y are in physical pixels.
void HandleView::MoveTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 previousPositionX = mPositionX;
    Int32 previousPositionY = mPositionY;

    mPositionX = x;
    mPositionY = y;
    if (IsPositionVisible()) {
        Boolean bFlag = FALSE;
        mContainer->IsShowing(&bFlag);
        if (bFlag) {
            OnPositionChanged();
            // Hide paste popup window as soon as the handle is dragged.
            if (mPastePopupWindow != NULL &&
                    (previousPositionX != mPositionX || previousPositionY != mPositionY)) {
                mPastePopupWindow->Hide();
            }
        }
        else {
            Show();
        }

        if (mIsDragging) {
            // Hide paste popup window as soon as the handle is dragged.
            if (mPastePopupWindow != NULL) {
                mPastePopupWindow->Hide();
            }
        }
    }
    else {
        Hide();
    }
}

//@Override
void HandleView::OnDraw(
    /* [in] */ ICanvas* c)
{
    UpdateAlpha();
    Int32 right, left, bottom, top;
    GetRight(&right);
    GetLeft(&left);
    GetBottom(&bottom);
    GetTop(&top);
    mDrawable->SetBounds(0, 0, right - left, bottom - top);
    mDrawable->Draw(c);
}

//@Override
ECode HandleView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(ev);
    VALIDATE_NOT_NULL(result);

    Int32 actionMasked;
    ev->GetActionMasked(&actionMasked);
    switch (actionMasked) {
        case IMotionEvent::ACTION_DOWN: {
            ev->GetRawX(&mDownPositionX);
            ev->GetRawY(&mDownPositionY);
            mTouchToWindowOffsetX = mDownPositionX - mPositionX;
            mTouchToWindowOffsetY = mDownPositionY - mPositionY;
            mIsDragging = TRUE;
            mController->BeforeStartUpdatingPosition(this);
            mTouchTimer = SystemClock::GetUptimeMillis();
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            Float rawX, rawY;
            ev->GetRawY(&rawY);
            ev->GetRawX(&rawX);
            UpdatePosition(rawX, rawY);
            break;
        }

        case IMotionEvent::ACTION_UP:
            if (mIsInsertionHandle) {
                Int64 delay = SystemClock::GetUptimeMillis() - mTouchTimer;
                AutoPtr<IViewConfigurationHelper> helper;
                CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
                Int32 tapTimeout;
                helper->GetTapTimeout(&tapTimeout);
                if (delay < tapTimeout) {
                    if (mPastePopupWindow != NULL && mPastePopupWindow->IsShowing()) {
                        // Tapping on the handle dismisses the displayed paste view,
                        mPastePopupWindow->Hide();
                    }
                    else {
                        ShowPastePopupWindow();
                    }
                }
            }

            mIsDragging = FALSE;
            break;

        case IMotionEvent::ACTION_CANCEL:
            mIsDragging = FALSE;
            break;

        default:
            *result = FALSE;
    }

    *result = TRUE;

    return NOERROR;
}

Boolean HandleView::IsDragging()
{
    return mIsDragging;
}

/**
 * @return Returns the x position of the handle
 */
Int32 HandleView::GetPositionX()
{
    return mPositionX;
}

/**
 * @return Returns the y position of the handle
 */
Int32 HandleView::GetPositionY()
{
    return mPositionY;
}

void HandleView::UpdatePosition(
    /* [in] */ Float rawX,
    /* [in] */ Float rawY)
{
    const Float newPosX = rawX - mTouchToWindowOffsetX + mHotspotX;
    const Float newPosY = rawY - mTouchToWindowOffsetY + mHotspotY - mLineOffsetY;

    mController->UpdatePosition(this, Elastos::Core::Math::Round(newPosX), Elastos::Core::Math::Round(newPosY));
}

// x and y are in physical pixels.
void HandleView::PositionAt(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    MoveTo(x - Elastos::Core::Math::Round(mHotspotX), y - Elastos::Core::Math::Round(mHotspotY));
}

// Returns the x coordinate of the position that the handle appears to be pointing to relative
// to the handles "parent" view.
Int32 HandleView::GetAdjustedPositionX()
{
    return mPositionX + Elastos::Core::Math::Round(mHotspotX);
}

// Returns the y coordinate of the position that the handle appears to be pointing to relative
// to the handles "parent" view.
Int32 HandleView::GetAdjustedPositionY()
{
    return mPositionY + Elastos::Core::Math::Round(mHotspotY);
}

// Returns the x coordinate of the postion that the handle appears to be pointing to relative to
// the root view of the application.
Int32 HandleView::GetRootViewRelativePositionX()
{
    return GetContainerPositionX() + Elastos::Core::Math::Round(mHotspotX);
}

// Returns the y coordinate of the postion that the handle appears to be pointing to relative to
// the root view of the application.
Int32 HandleView::GetRootViewRelativePositionY()
{
    return GetContainerPositionY() + Elastos::Core::Math::Round(mHotspotY);
}

// Returns a suitable y coordinate for the text position corresponding to the handle.
// As the handle points to a position on the base of the line of text, this method
// returns a coordinate a small number of pixels higher (i.e. a slightly smaller number)
// than getAdjustedPositionY.
Int32 HandleView::GetLineAdjustedPositionY()
{
    return (Int32) (mPositionY + mHotspotY - mLineOffsetY);
}

AutoPtr<IDrawable> HandleView::GetDrawable()
{
    return mDrawable;
}

void HandleView::UpdateAlpha()
{
    if (mAlpha == 1.f) return;

    AutoPtr<IAnimationUtils> utils;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&utils);
    Int64 timeMillis;
    utils->CurrentAnimationTimeMillis(&timeMillis);
    mAlpha = Elastos::Core::Math::Min(1.f,
            (timeMillis - mFadeStartTime) / FADE_DURATION);
    mDrawable->SetAlpha((Int32) (255 * mAlpha));
    Invalidate();
}

/**
 * If the handle is not visible, sets its visibility to View.VISIBLE and begins fading it in.
 */
void HandleView::BeginFadeIn()
{
    Int32 visibility;
    if ((GetVisibility(&visibility), visibility) == VISIBLE) return;
    mAlpha = 0.f;
    AutoPtr<IAnimationUtils> utils;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&utils);
    utils->CurrentAnimationTimeMillis(&mFadeStartTime);
    SetVisibility(VISIBLE);
    //Position updates may have been deferred while the handle was hidden.
    OnPositionChanged();
}

void HandleView::ShowPastePopupWindow()
{
    InsertionHandleController* ihc = (InsertionHandleController*) mController.Get();
    if (mIsInsertionHandle && ihc->CanPaste()) {
        if (mPastePopupWindow == NULL) {
            // Lazy initialization: create when actually shown only.
            mPastePopupWindow = new InsertionHandleController::PastePopupMenu(ihc);
        }
        mPastePopupWindow->Show();
    }
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
