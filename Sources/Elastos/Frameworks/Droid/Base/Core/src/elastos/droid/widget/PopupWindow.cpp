
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/PopupWindow.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/view/Gravity.h"
#include "elastos/droid/widget/CFrameLayoutLayoutParams.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Droid::Graphics::Drawable::IStateListDrawable;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::IDispatcherState;
using Elastos::Droid::View::EIID_IOnScrollChangedListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::Gravity;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::Accessibility::EIID_IAccessibilityEventSource;
using Elastos::Droid::R;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

const Int32 PopupWindow::DEFAULT_ANCHORED_GRAVITY = IGravity::TOP | IGravity::START;

static AutoPtr<ArrayOf<Int32> > InitABOVE_ANCHOR_STATE_SET()
{
    AutoPtr< ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(1);
    (*array)[0] = R::attr::state_above_anchor;
    return array;
}

AutoPtr<ArrayOf<Int32> > PopupWindow::ABOVE_ANCHOR_STATE_SET = InitABOVE_ANCHOR_STATE_SET();

const String PopupWindow::PopupViewContainer::TAG("PopupWindow.PopupViewContainer");

//==============================================================================
//          PopupWindow::PopupViewContainer
//==============================================================================

PopupWindow::PopupViewContainer::PopupViewContainer(
    /* [in] */ IContext* context,
    /* [in] */ PopupWindow* host)
    : mHost(host)
{
    FrameLayout::constructor(context);
}

ECode PopupWindow::PopupViewContainer::OnCreateDrawableState(
    /* [in] */ Int32 extraSpace,
    /* [out] */ ArrayOf<Int32>** drawableState)
{
    VALIDATE_NOT_NULL(drawableState);
    *drawableState = NULL;

    AutoPtr<ArrayOf<Int32> > ds;
    if (mHost->mAboveAnchor) {
        // 1 more needed for the above anchor state
        FAIL_RETURN(FrameLayout::OnCreateDrawableState(extraSpace + 1, (ArrayOf<Int32>**)&ds));
        View::MergeDrawableStates(ds, ABOVE_ANCHOR_STATE_SET);
        *drawableState = ds;
        REFCOUNT_ADD(*drawableState);
        return NOERROR;
    }
    else {
        FAIL_RETURN(FrameLayout::OnCreateDrawableState(extraSpace, (ArrayOf<Int32>**)&ds));
        *drawableState = ds;
        REFCOUNT_ADD(*drawableState);
        return NOERROR;
    }
}

ECode PopupWindow::PopupViewContainer::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Int32 keyCode;
    event->GetKeyCode(&keyCode);
    if (keyCode == IKeyEvent::KEYCODE_BACK) {
        AutoPtr<IDispatcherState> dispatcherState;
        GetKeyDispatcherState((IDispatcherState**)&dispatcherState);
        if (!dispatcherState) {
            return FrameLayout::DispatchKeyEvent(event, res);
        }

        Int32 action, count;
        event->GetAction(&action);
        event->GetRepeatCount(&count);
        if (action == IKeyEvent::ACTION_DOWN && count == 0) {
            AutoPtr<IDispatcherState> state;
            GetKeyDispatcherState((IDispatcherState**)&state);
            if (state != NULL) {
                state->StartTracking(event, TO_IINTERFACE(this));
            }
            *res = TRUE;
            return NOERROR;
        }
        else if (action == IKeyEvent::ACTION_UP) {
                AutoPtr<IDispatcherState> state;
                GetKeyDispatcherState((IDispatcherState**)&state);
                if (state != NULL) {
                    Boolean tracking, canceled;
                    state->IsTracking(event, &tracking);
                    event->IsCanceled(&canceled);
                    if (tracking && !canceled) {
                        mHost->Dismiss();
                        *res = TRUE;
                        return NOERROR;
                    }
                }
            }
        return FrameLayout::DispatchKeyEvent(event, res);
    }
    else {
        return FrameLayout::DispatchKeyEvent(event, res);
    }
}

ECode PopupWindow::PopupViewContainer::DispatchTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    if (mHost->mTouchInterceptor != NULL) {
        Boolean result;
        mHost->mTouchInterceptor->OnTouch(this, event, &result);
        if (result) {
            *res = TRUE;
            return NOERROR;
        }
    }
    return FrameLayout::DispatchTouchEvent(event, res);
}

ECode PopupWindow::PopupViewContainer::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    assert(event != NULL);

    Float _x, _y;
    event->GetX(&_x);
    event->GetY(&_y);
    Int32 x = (Int32)_x;
    Int32 y = (Int32)_y;

    Int32 action;
    event->GetAction(&action);
    Int32 width, height;
    mHost->GetWidth(&width);
    mHost->GetHeight(&height);
    if ((action == IMotionEvent::ACTION_DOWN)
            && ((x < 0) || (x >= width) || (y < 0) || (y >= height))) {
        mHost->Dismiss();
        *res = TRUE;
        return NOERROR;
    }
    else if (action == IMotionEvent::ACTION_OUTSIDE) {
        mHost->Dismiss();
        *res = TRUE;
        return NOERROR;
    }
    else {
        return FrameLayout::OnTouchEvent(event, res);
    }
}

ECode PopupWindow::PopupViewContainer::SendAccessibilityEvent(
    /* [in] */ Int32 eventType)
{
    // clinets are interested in the content not the container, make it event source
    if (mHost->mContentView != NULL) {
        return IAccessibilityEventSource::Probe(mHost->mContentView)->SendAccessibilityEvent(eventType);
    }
    else {
        return FrameLayout::SendAccessibilityEvent(eventType);
    }
    return NOERROR;
}

//==============================================================================
//          PopupWindow::PopupWindowScrollChangedListener
//==============================================================================

CAR_INTERFACE_IMPL(PopupWindow::PopupWindowScrollChangedListener, Object, IOnScrollChangedListener);

PopupWindow::PopupWindowScrollChangedListener::PopupWindowScrollChangedListener(
    /* [in] */ PopupWindow* host)
    : mHost(host)
{}

ECode PopupWindow::PopupWindowScrollChangedListener::OnScrollChanged()
{
    AutoPtr<IView> anchor;
    if (mHost->mAnchor != NULL) {
        AutoPtr<IInterface> objectReference;
        mHost->mAnchor->Resolve(EIID_IInterface, (IInterface**)&objectReference);
        anchor = IView::Probe(objectReference);
    }

    if (anchor != NULL && mHost->mPopupView != NULL) {
        AutoPtr<IViewGroupLayoutParams> params;
        mHost->mPopupView->GetLayoutParams((IViewGroupLayoutParams**)&params);
        AutoPtr<IWindowManagerLayoutParams> p = IWindowManagerLayoutParams::Probe(params);
        assert(p != NULL && "IWindowManagerLayoutParams cannot be null.");
        Int32 x, y;
        p->GetX(&x);
        p->GetY(&y);
        mHost->UpdateAboveAnchor(mHost->FindDropDownPosition(
                anchor, p, mHost->mAnchorXoff, mHost->mAnchorYoff, mHost->mAnchoredGravity));
        mHost->Update(x, y, -1, -1, TRUE);
    }

    return NOERROR;
}

//==============================================================================
//          PopupWindow
//==============================================================================

CAR_INTERFACE_IMPL(PopupWindow, Object, IPopupWindow);

PopupWindow::PopupWindow()
    : mIsShowing(FALSE)
    , mIsDropdown(FALSE)
    , mFocusable(FALSE)
    , mInputMethodMode(0/*INPUT_METHOD_FROM_FOCUSABLE*/)
    , mSoftInputMode(0)
    , mTouchable(TRUE)
    , mOutsideTouchable(FALSE)
    , mClippingEnabled(TRUE)
    , mSplitTouchEnabled(-1)
    , mLayoutInScreen(FALSE)
    , mClipToScreen(FALSE)
    , mAllowScrollingAnchorParent(TRUE)
    , mLayoutInsetDecor(FALSE)
    , mNotTouchModal(FALSE)
    , mWidthMode(0)
    , mWidth(0)
    , mLastWidth(0)
    , mHeightMode(0)
    , mHeight(0)
    , mLastHeight(0)
    , mPopupWidth(0)
    , mPopupHeight(0)
    , mElevation(0.f)
    , mAnchorRelativeX(0)
    , mAnchorRelativeY(0)
    , mAboveAnchor(FALSE)
    , mWindowLayoutType(IWindowManagerLayoutParams::TYPE_APPLICATION_PANEL)
    , mIgnoreCheekPress(FALSE)
    , mAnimationStyle(-1)
    , mAnchor(NULL)
    , mAnchorXoff(0)
    , mAnchorYoff(0)
    , mAnchoredGravity(0)
    , mOverlapAnchor(FALSE)
    , mPopupViewInitialLayoutDirectionInherited(FALSE)
{
    mDrawingLocation = ArrayOf<Int32>::Alloc(2);
    mScreenLocation = ArrayOf<Int32>::Alloc(2);
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mTempRect));
    mOnScrollChangedListener = new PopupWindowScrollChangedListener(this);
}

PopupWindow::~PopupWindow()
{}

ECode PopupWindow::constructor()
{
    return constructor(NULL, 0, 0);
}

ECode PopupWindow::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode PopupWindow::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::popupWindowStyle);
}

ECode PopupWindow::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode PopupWindow::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    assert(ctx != NULL);

    mContext = ctx;
    AutoPtr<IInterface> obj;
    ctx->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
    mWindowManager = IWindowManager::Probe(obj);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::PopupWindow);
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(ctx->ObtainStyledAttributes(
            attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));

    a->GetDrawable(R::styleable::PopupWindow_popupBackground, (IDrawable**)&mBackground);

    a->GetDimension(R::styleable::PopupWindow_popupElevation, 0, &mElevation);
    a->GetBoolean(R::styleable::PopupWindow_overlapAnchor, FALSE, &mOverlapAnchor);

    Int32 animStyle;
    a->GetResourceId(R::styleable::PopupWindow_popupAnimationStyle, -1, &animStyle);
    mAnimationStyle = animStyle == R::style::Animation_PopupWindow ? -1 : animStyle;

    // If this is a StateListDrawable, try to find and store the drawable to be
    // used when the drop-down is placed above its anchor view, and the one to be
    // used when the drop-down is placed below its anchor view. We extract
    // the drawables ourselves to work around a problem with using refreshDrawableState
    // that it will take into account the padding of all drawables specified in a
    // StateListDrawable, thus adding superfluous padding to drop-down views.
    //
    // We assume a StateListDrawable will have a drawable for ABOVE_ANCHOR_STATE_SET and
    // at least one other drawable, intended for the 'below-anchor state'.
    if (mBackground && IStateListDrawable::Probe(mBackground) != NULL) {
        IStateListDrawable* background = IStateListDrawable::Probe(mBackground);

        // Find the above-anchor view - this one's easy, it should be labeled as such.
        Int32 aboveAnchorStateIndex;
        background->GetStateDrawableIndex(ABOVE_ANCHOR_STATE_SET, &aboveAnchorStateIndex);

        // Now, for the below-anchor view, look for any other drawable specified in the
        // StateListDrawable which is not for the above-anchor state and use that.
        Int32 count;
        background->GetStateCount(&count);
        Int32 belowAnchorStateIndex = -1;
        for (Int32 i = 0; i < count; i++) {
            if (i != aboveAnchorStateIndex) {
                belowAnchorStateIndex = i;
                break;
            }
        }

        // Store the drawables we found, if we found them. Otherwise, set them both
        // to NULL so that we'll just use refreshDrawableState.
        if (aboveAnchorStateIndex != -1 && belowAnchorStateIndex != -1) {
            background->GetStateDrawable(aboveAnchorStateIndex,
                    (IDrawable**)&mAboveAnchorBackgroundDrawable);
            background->GetStateDrawable(belowAnchorStateIndex,
                    (IDrawable**)&mBelowAnchorBackgroundDrawable);
        }
        else {
            mBelowAnchorBackgroundDrawable = NULL;
            mAboveAnchorBackgroundDrawable = NULL;
        }
    }

    a->Recycle();

    return NOERROR;
}

ECode PopupWindow::constructor(
    /* [in] */ IView* contentView)
{
    return constructor(contentView, 0, 0);
}

ECode PopupWindow::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return constructor(NULL, width, height);
}

ECode PopupWindow::constructor(
    /* [in] */ IView* contentView,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return constructor(contentView, width, height, FALSE);
}

ECode PopupWindow::constructor(
    /* [in] */ IView* contentView,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Boolean focusable)
{
    if (contentView != NULL) {
        contentView->GetContext((IContext**)&mContext);
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
        mWindowManager = IWindowManager::Probe(obj);
    }
    SetContentView(contentView);
    SetWidth(width);
    SetHeight(height);
    SetFocusable(focusable);

    return NOERROR;
}

ECode PopupWindow::GetBackground(
    /* [out] */ IDrawable** background)
{
    VALIDATE_NOT_NULL(background);
    *background = mBackground;
    REFCOUNT_ADD(*background);
    return NOERROR;
}

ECode PopupWindow::SetBackgroundDrawable(
    /* [in] */ IDrawable* background)
{
    mBackground = background;

    return NOERROR;
}

ECode PopupWindow::GetElevation(
    /* [out] */ Float* elevation)
{
    VALIDATE_NOT_NULL(elevation);
    *elevation = mElevation;
    return NOERROR;
}

ECode PopupWindow::SetElevation(
    /* [in] */ Float elevation)
{
    mElevation = elevation;
    return NOERROR;
}

ECode PopupWindow::GetAnimationStyle(
    /* [out] */ Int32* style)
{
    VALIDATE_NOT_NULL(style);
    *style = mAnimationStyle;
    return NOERROR;
}

ECode PopupWindow::SetIgnoreCheekPress()
{
    mIgnoreCheekPress = TRUE;

    return NOERROR;
}

ECode PopupWindow::SetAnimationStyle(
    /* [in] */ Int32 animationStyle)
{
    mAnimationStyle = animationStyle;

    return NOERROR;
}

ECode PopupWindow::GetContentView(
    /* [out] */ IView** contentView)
{
    VALIDATE_NOT_NULL(contentView);
    *contentView = mContentView;
    REFCOUNT_ADD(*contentView);
    return NOERROR;
}

ECode PopupWindow::SetContentView(
    /* [in] */ IView* contentView)
{
    Boolean res;
    if (IsShowing(&res), res) {
        return NOERROR;
    }

    mContentView = contentView;
    if (mContext == NULL && mContentView != NULL) {
        mContentView->GetContext((IContext**)&mContext);
    }

    if (mWindowManager == NULL && mContentView != NULL) {
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
        mWindowManager = IWindowManager::Probe(obj);
    }

    return NOERROR;
}

ECode PopupWindow::SetTouchInterceptor(
    /* [in] */ IViewOnTouchListener* l)
{
    mTouchInterceptor = l;

    return NOERROR;
}

ECode PopupWindow::IsFocusable(
    /* [out] */ Boolean* isFocusable)
{
    VALIDATE_NOT_NULL(isFocusable);
    *isFocusable = mFocusable;
    return NOERROR;
}

ECode PopupWindow::SetFocusable(
    /* [in] */ Boolean focusable)
{
    mFocusable = focusable;

    return NOERROR;
}

ECode PopupWindow::GetInputMethodMode(
    /* [out] */ Int32* inputMethodMode)
{
    VALIDATE_NOT_NULL(inputMethodMode);
    *inputMethodMode = mInputMethodMode;
    return NOERROR;
}

ECode PopupWindow::SetInputMethodMode(
    /* [in] */ Int32 mode)
{
    mInputMethodMode = mode;

    return NOERROR;
}

ECode PopupWindow::SetSoftInputMode(
    /* [in] */ Int32 mode)
{
    mSoftInputMode = mode;

    return NOERROR;
}

ECode PopupWindow::GetSoftInputMode(
    /* [out] */ Int32* softInputMode)
{
    VALIDATE_NOT_NULL(softInputMode);
    *softInputMode = mSoftInputMode;
    return NOERROR;
}

ECode PopupWindow::IsTouchable(
    /* [out] */ Boolean* touchable)
{
    VALIDATE_NOT_NULL(touchable);
    *touchable = mTouchable;
    return NOERROR;
}

ECode PopupWindow::SetTouchable(
    /* [in] */ Boolean touchable)
{
    mTouchable = touchable;

    return NOERROR;
}

ECode PopupWindow::IsOutsideTouchable(
    /* [out] */ Boolean* touchable)
{
    VALIDATE_NOT_NULL(touchable);
    *touchable = mOutsideTouchable;
    return NOERROR;
}

ECode PopupWindow::SetOutsideTouchable(
    /* [in] */ Boolean touchable)
{
    mOutsideTouchable = touchable;

    return NOERROR;
}

ECode PopupWindow::IsClippingEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = mClippingEnabled;
    return NOERROR;
}

ECode PopupWindow::SetClippingEnabled(
    /* [in] */ Boolean enabled)
{
    mClippingEnabled = enabled;

    return NOERROR;
}

ECode PopupWindow::SetClipToScreenEnabled(
    /* [in] */ Boolean enabled)
{
    mClipToScreen = enabled;
    SetClippingEnabled(!enabled);
    return NOERROR;
}

ECode PopupWindow::SetAllowScrollingAnchorParent(
    /* [in] */ Boolean enabled)
{
    mAllowScrollingAnchorParent = enabled;
    return NOERROR;
}

ECode PopupWindow::IsSplitTouchEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);

    if (mSplitTouchEnabled < 0 && mContext != NULL) {
        AutoPtr<IApplicationInfo> info;
        mContext->GetApplicationInfo((IApplicationInfo**)&info);
        Int32 targetSdkVersion;
        info->GetTargetSdkVersion(&targetSdkVersion);
        *enabled = targetSdkVersion >= Build::VERSION_CODES::HONEYCOMB;
        return NOERROR;
    }
    *enabled = mSplitTouchEnabled == 1;
    return NOERROR;
}

ECode PopupWindow::SetSplitTouchEnabled(
    /* [in] */ Boolean enabled)
{
    mSplitTouchEnabled = enabled ? 1 : 0;

    return NOERROR;
}

ECode PopupWindow::IsLayoutInScreenEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = mLayoutInScreen;
    return NOERROR;
}

ECode PopupWindow::SetLayoutInScreenEnabled(
    /* [in] */ Boolean enabled)
{
    mLayoutInScreen = enabled;

    return NOERROR;
}

ECode PopupWindow::SetLayoutInsetDecor(
    /* [in] */ Boolean enabled)
{
    mLayoutInsetDecor = enabled;
    return NOERROR;
}

ECode PopupWindow::SetWindowLayoutType(
    /* [in] */ Int32 layoutType)
{
    mWindowLayoutType = layoutType;

    return NOERROR;
}

ECode PopupWindow::GetWindowLayoutType(
    /* [out] */ Int32* layoutType)
{
    VALIDATE_NOT_NULL(layoutType);
    *layoutType = mWindowLayoutType;
    return NOERROR;
}

ECode PopupWindow::SetTouchModal(
    /* [in] */ Boolean touchModal)
{
    mNotTouchModal = !touchModal;
    return NOERROR;
}

ECode PopupWindow::SetWindowLayoutMode(
    /* [in] */ Int32 widthSpec,
    /* [in] */ Int32 heightSpec)
{
    mWidthMode = widthSpec;
    mHeightMode = heightSpec;

    return NOERROR;
}

ECode PopupWindow::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mHeight;
    return NOERROR;
}

ECode PopupWindow::SetHeight(
    /* [in] */ Int32 height)
{
    mHeight = height;

    return NOERROR;
}

ECode PopupWindow::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mWidth;
    return NOERROR;
}

ECode PopupWindow::SetWidth(
    /* [in] */ Int32 width)
{
    mWidth = width;

    return NOERROR;
}

ECode PopupWindow::IsShowing(
    /* [out] */ Boolean* isShowing)
{
    VALIDATE_NOT_NULL(isShowing);
    *isShowing = mIsShowing;
    return NOERROR;
}

ECode PopupWindow::ShowAtLocation(
    /* [in] */ IView* parent,
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    AutoPtr<IBinder> token;
    parent->GetWindowToken((IBinder**)&token);
    return ShowAtLocation(token, gravity, x, y);
}

ECode PopupWindow::ShowAtLocation(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Boolean res;
    if ((IsShowing(&res), res) || mContentView == NULL) {
        return NOERROR;
    }

    UnregisterForScrollChanged();

    mIsShowing = TRUE;
    mIsDropdown = FALSE;

    AutoPtr<IWindowManagerLayoutParams> p = CreatePopupLayout(token);
    ((CWindowManagerLayoutParams*)p.Get())->mWindowAnimations = ComputeAnimationResource();

    FAIL_RETURN(PreparePopup(p));
    if (gravity == IGravity::NO_GRAVITY) {
        gravity = IGravity::TOP | IGravity::START;
    }
    ((CWindowManagerLayoutParams*)p.Get())->mGravity = gravity;
    ((CWindowManagerLayoutParams*)p.Get())->mX = x;
    ((CWindowManagerLayoutParams*)p.Get())->mY = y;
    if (mHeightMode < 0) ((CWindowManagerLayoutParams*)p.Get())->mHeight = mLastHeight = mHeightMode;
    if (mWidthMode < 0) ((CWindowManagerLayoutParams*)p.Get())->mWidth = mLastWidth = mWidthMode;
    InvokePopup(p);

    return NOERROR;
}

ECode PopupWindow::ShowAsDropDown(
    /* [in] */ IView* anchor)
{
    return ShowAsDropDown(anchor, 0, 0);
}

ECode PopupWindow::ShowAsDropDown(
    /* [in] */ IView* anchor,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff)
{
    return ShowAsDropDown(anchor, xoff, yoff, DEFAULT_ANCHORED_GRAVITY);
}

ECode PopupWindow::ShowAsDropDown(
    /* [in] */ IView* anchor,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 gravity)
{
    Boolean res;
    if ((IsShowing(&res), res) || mContentView == NULL) {
        return NOERROR;
    }

    RegisterForScrollChanged(anchor, xoff, yoff, gravity);

    mIsShowing = TRUE;
    mIsDropdown = TRUE;

    AutoPtr<IBinder> token;
    anchor->GetWindowToken((IBinder**)&token);
    AutoPtr<IWindowManagerLayoutParams> p = CreatePopupLayout(token);
    FAIL_RETURN(PreparePopup(p));

    UpdateAboveAnchor(FindDropDownPosition(anchor, p, xoff, yoff, gravity));

    if (mHeightMode < 0) {
        ((CWindowManagerLayoutParams*)p.Get())->mHeight = mLastHeight = mHeightMode;
    }
    if (mWidthMode < 0) {
        ((CWindowManagerLayoutParams*)p.Get())->mWidth = mLastWidth = mWidthMode;
    }

    ((CWindowManagerLayoutParams*)p.Get())->mWindowAnimations = ComputeAnimationResource();

    InvokePopup(p);

    return NOERROR;
}

void PopupWindow::UpdateAboveAnchor(
    /* [in] */ Boolean aboveAnchor)
{
    if (aboveAnchor != mAboveAnchor) {
        mAboveAnchor = aboveAnchor;

        if (mBackground != NULL) {
            // If the background drawable provided was a StateListDrawable with above-anchor
            // and below-anchor states, use those. Otherwise rely on refreshDrawableState to
            // do the job.
            if (mAboveAnchorBackgroundDrawable != NULL) {
                if (mAboveAnchor) {
                    mPopupView->SetBackground(mAboveAnchorBackgroundDrawable);
                }
                else {
                    mPopupView->SetBackground(mBelowAnchorBackgroundDrawable);
                }
            }
            else {
                mPopupView->RefreshDrawableState();
            }
        }
    }
}

ECode PopupWindow::IsAboveAnchor(
    /* [out] */ Boolean* isAboveAnchor)
{
    VALIDATE_NOT_NULL(isAboveAnchor);
    *isAboveAnchor = mAboveAnchor;
    return NOERROR;
}

ECode PopupWindow::PreparePopup(
    /* [in] */ IWindowManagerLayoutParams* p)
{
    if (mContentView == NULL || mContext == NULL || mWindowManager == NULL) {
        Logger::E("PopupWindow", "You must specify a valid content view by" \
                "calling setContentView() before attempting to show the popup.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (mBackground != NULL) {
        AutoPtr<IViewGroupLayoutParams> layoutParams;
        mContentView->GetLayoutParams((IViewGroupLayoutParams**)&layoutParams);
        Int32 height = IViewGroupLayoutParams::MATCH_PARENT;
        Int32 h;
        if (layoutParams != NULL && (layoutParams->GetHeight(&h),
            h) == IViewGroupLayoutParams::WRAP_CONTENT) {
            height = IViewGroupLayoutParams::WRAP_CONTENT;
        }

        // when a background is available, we embed the content view
        // within another view that owns the background drawable
        AutoPtr<PopupViewContainer> popupViewContainer = new PopupViewContainer(mContext, this);
        AutoPtr<IFrameLayoutLayoutParams> listParams;
        CFrameLayoutLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT, height,
                (IFrameLayoutLayoutParams**)&listParams);
        popupViewContainer->SetBackground(mBackground);
        IViewGroup::Probe(popupViewContainer)->AddView(mContentView, IViewGroupLayoutParams::Probe(listParams));

        mPopupView = IView::Probe(popupViewContainer);
    }
    else {
        mPopupView = mContentView;
    }

    mPopupView->SetElevation(mElevation);
    Int32 dir;
    mPopupView->GetRawLayoutDirection(&dir);
    mPopupViewInitialLayoutDirectionInherited = (dir == IView::LAYOUT_DIRECTION_INHERIT);
    mPopupWidth = ((CWindowManagerLayoutParams*)p)->mWidth;
    mPopupHeight = ((CWindowManagerLayoutParams*)p)->mHeight;

    return NOERROR;
}

void PopupWindow::InvokePopup(
    /* [in] */ IWindowManagerLayoutParams* p)
{
    if (mContext != NULL) {
        assert(p != NULL);
        String pn;
        mContext->GetPackageName(&pn);
        p->SetPackageName(pn);
    }
    mPopupView->SetFitsSystemWindows(mLayoutInsetDecor);
    SetLayoutDirectionFromAnchor();

    IViewManager::Probe(mWindowManager)->AddView(mPopupView, IViewGroupLayoutParams::Probe(p));
}

void PopupWindow::SetLayoutDirectionFromAnchor()
{
    if (mAnchor != NULL) {
        AutoPtr<IInterface> obj;
        mAnchor->Resolve(EIID_IView, (IInterface**)&obj);
        AutoPtr<IView> anchor = IView::Probe(obj);
        if (anchor != NULL && mPopupViewInitialLayoutDirectionInherited) {
            Int32 dir;
            anchor->GetLayoutDirection(&dir);
            mPopupView->SetLayoutDirection(dir);
        }
    }
}

AutoPtr<IWindowManagerLayoutParams> PopupWindow::CreatePopupLayout(
   /* [in] */ IBinder* token)
{
    // generates the layout parameters for the drop down
    // we want a fixed size view located at the bottom left of the anchor
    AutoPtr<CWindowManagerLayoutParams> p;
    CWindowManagerLayoutParams::NewByFriend((CWindowManagerLayoutParams**)&p);
    // these gravity settings put the view at the top left corner of the
    // screen. The view is then positioned to the appropriate location
    // by setting the x and y offsets to match the anchor's bottom
    // left corner
    p->mGravity = IGravity::START | IGravity::TOP;
    p->mWidth = mLastWidth = mWidth;
    p->mHeight = mLastHeight = mHeight;
    if (mBackground != NULL) {
        mBackground->GetOpacity(&(p->mFormat));
    }
    else {
        p->mFormat = Elastos::Droid::Graphics::IPixelFormat::TRANSLUCENT;
    }
    p->mFlags = ComputeFlags(p->mFlags);
    p->mType = mWindowLayoutType;
    p->mToken = token;
    p->mSoftInputMode = mSoftInputMode;
    AutoPtr<ICharSequence> title;
    Int32 hashCode;
    GetHashCode(&hashCode);
    CString::New(String("PopupWindow:") + StringUtils::ToHexString(hashCode), (ICharSequence**)&title);
    p->SetTitle(title);

    return (IWindowManagerLayoutParams*)p.Get();
}

Int32 PopupWindow::ComputeFlags(
    /* [in] */ Int32 curFlags)
{
    curFlags &= ~(
            IWindowManagerLayoutParams::FLAG_IGNORE_CHEEK_PRESSES |
            IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE |
            IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE |
            IWindowManagerLayoutParams::FLAG_WATCH_OUTSIDE_TOUCH |
            IWindowManagerLayoutParams::FLAG_LAYOUT_NO_LIMITS |
            IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM |
            IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH);
    if(mIgnoreCheekPress) {
        curFlags |= IWindowManagerLayoutParams::FLAG_IGNORE_CHEEK_PRESSES;
    }
    if (!mFocusable) {
        curFlags |= IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE;
        if (mInputMethodMode == IPopupWindow::INPUT_METHOD_NEEDED) {
            curFlags |= IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM;
        }
    }
    else if (mInputMethodMode == IPopupWindow::INPUT_METHOD_NOT_NEEDED) {
        curFlags |= IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM;
    }
    if (!mTouchable) {
        curFlags |= IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE;
    }
    if (mOutsideTouchable) {
        curFlags |= IWindowManagerLayoutParams::FLAG_WATCH_OUTSIDE_TOUCH;
    }
    if (!mClippingEnabled) {
        curFlags |= IWindowManagerLayoutParams::FLAG_LAYOUT_NO_LIMITS;
    }
    Boolean res;
    if (IsSplitTouchEnabled(&res), res) {
        curFlags |= IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH;
    }
    if (mLayoutInScreen) {
        curFlags |= IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN;
    }

    if (mLayoutInsetDecor) {
        curFlags |= IWindowManagerLayoutParams::FLAG_LAYOUT_INSET_DECOR;
    }
    if (mNotTouchModal) {
        curFlags |= IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL;
    }

    return curFlags;
}

Int32 PopupWindow::ComputeAnimationResource()
{
    if (mAnimationStyle == -1) {
        if (mIsDropdown) {
            return mAboveAnchor
                    ? R::style::Animation_DropDownUp
                    : R::style::Animation_DropDownDown;
        }
        return 0;
    }
    return mAnimationStyle;
}

Boolean PopupWindow::FindDropDownPosition(
    /* [in] */ IView* anchor,
    /* [in] */ IWindowManagerLayoutParams* _p,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 gravity)
{
    assert(anchor != NULL);
    Int32 anchorHeight;
    anchor->GetHeight(&anchorHeight);
    Int32 anchorWidth;
    anchor->GetWidth(&anchorWidth);
    if (mOverlapAnchor) {
        yoff -= anchorHeight;
    }

    CWindowManagerLayoutParams* p = (CWindowManagerLayoutParams*)_p;

    anchor->GetLocationInWindow(mDrawingLocation);

    p->mX = (*mDrawingLocation)[0] + xoff;
    p->mY = (*mDrawingLocation)[1] + anchorHeight + yoff;

    Int32 direction;
    anchor->GetLayoutDirection(&direction);
    Int32 data;
    Gravity::GetAbsoluteGravity(gravity, direction, &data);
    Int32 hgrav = data & IGravity::HORIZONTAL_GRAVITY_MASK;
    if (hgrav == IGravity::RIGHT) {
        // Flip the location to align the right sides of the popup and
        // anchor instead of left.
        p->mX -= mPopupWidth - anchorWidth;
    }

    Boolean onTop = FALSE;

    p->mGravity = IGravity::LEFT | IGravity::TOP;

    anchor->GetLocationOnScreen(mScreenLocation);

    AutoPtr<IRect> displayFrame;
    ASSERT_SUCCEEDED(CRect::New((IRect**)&displayFrame));
    anchor->GetWindowVisibleDisplayFrame(displayFrame);

    const Int32 screenY = (*mScreenLocation)[1] + anchorHeight + yoff;

    AutoPtr<IView> root;
    anchor->GetRootView((IView**)&root);
    Int32 width;
    root->GetWidth(&width);
    Int32 l, t, r, b;
    displayFrame->Get(&l, &t, &r, &b);
    if (screenY + mPopupHeight > b|| p->mX + mPopupWidth - width > 0) {
        // if the drop down disappears at the bottom of the screen. we try to
        // scroll a parent scrollview or move the drop down back up on top of
        // the edit box
        if (mAllowScrollingAnchorParent) {
            Int32 scrollX, scrollY;
            anchor->GetScrollX(&scrollX);
            anchor->GetScrollY(&scrollY);

            AutoPtr<IRect> r;
            ASSERT_SUCCEEDED(CRect::New(scrollX, scrollY,  scrollX + mPopupWidth + xoff,
                    scrollY + mPopupHeight + anchorHeight + yoff, (IRect**)&r));
            Boolean result;
            anchor->RequestRectangleOnScreen(r, TRUE, &result);
        }

        // now we re-evaluate the space available, and decide from that
        // whether the pop-up will go above or below the anchor.
        anchor->GetLocationInWindow(mDrawingLocation);

        p->mX = (*mDrawingLocation)[0] + xoff;
        p->mY = (*mDrawingLocation)[1] + anchorHeight + yoff;

        // Preserve the gravity adjustment.
        if (hgrav == IGravity::RIGHT) {
            p->mX -= mPopupWidth - anchorWidth;
        }

        // determine whether there is more space above or below the anchor
        anchor->GetLocationOnScreen(mScreenLocation);

        onTop = (b - (*mScreenLocation)[1] - anchorHeight - yoff) <
                ((*mScreenLocation)[1] - yoff - t);
        if (onTop) {
            p->mGravity = IGravity::LEFT | IGravity::BOTTOM;
            Int32 rootH;
            root->GetHeight(&rootH);
            p->mY = rootH - (*mDrawingLocation)[1] + yoff;
        }
        else {
            p->mY = (*mDrawingLocation)[1] + anchorHeight + yoff;
        }
    }

    if (mClipToScreen) {
        Int32 displayFrameWidth = r - l;

        const Int32 right = p->mX + p->mWidth;
        if (right > displayFrameWidth) {
            p->mX -= right - displayFrameWidth;
        }
        if (p->mX < l) {
            p->mX = l;
            p->mWidth = Elastos::Core::Math::Min(p->mWidth, displayFrameWidth);
        }

        if (onTop) {
            const Int32 popupTop = (*mScreenLocation)[1] + yoff - mPopupHeight;
            if (popupTop < 0) {
                p->mY += popupTop;
            }
        }
        else {
            p->mY = Elastos::Core::Math::Max(p->mY, t);
        }
    }

    p->mGravity |= IGravity::DISPLAY_CLIP_VERTICAL;

    // Compute the position of the anchor relative to the popup.
    mAnchorRelativeX = (*mDrawingLocation)[0] - p->mX + anchorHeight / 2;
    mAnchorRelativeY = (*mDrawingLocation)[1] - p->mY + anchorWidth / 2;

    return onTop;
}

ECode PopupWindow::GetMaxAvailableHeight(
    /* [in] */ IView* anchor,
    /* [out] */ Int32* maxAvailableHeight)
{
    VALIDATE_NOT_NULL(maxAvailableHeight);

    return GetMaxAvailableHeight(anchor, 0, maxAvailableHeight);
}

ECode PopupWindow::GetMaxAvailableHeight(
    /* [in] */ IView* anchor,
    /* [in] */ Int32 yOffset,
    /* [out] */ Int32* maxAvailableHeight)
{
    VALIDATE_NOT_NULL(maxAvailableHeight);

    return GetMaxAvailableHeight(anchor, yOffset, FALSE, maxAvailableHeight);
}

ECode PopupWindow::GetMaxAvailableHeight(
    /* [in] */ IView* anchor,
    /* [in] */ Int32 yOffset,
    /* [in] */ Boolean ignoreBottomDecorations,
    /* [out] */ Int32* maxAvailableHeight)
{
    VALIDATE_NOT_NULL(maxAvailableHeight);

    assert(anchor != NULL);

    AutoPtr<IRect> displayFrame;
    ASSERT_SUCCEEDED(CRect::New((IRect**)&displayFrame));
    anchor->GetWindowVisibleDisplayFrame(displayFrame);
    anchor->GetLocationOnScreen(mDrawingLocation);

    AutoPtr<ArrayOf<Int32> > anchorPos = mDrawingLocation;
    Int32 l, t, r, b;
    displayFrame->Get(&l, &t, &r, &b);

    Int32 bottomEdge = b;
    if (ignoreBottomDecorations) {
        AutoPtr<IContext> context;
        anchor->GetContext((IContext**)&context);
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        AutoPtr<IDisplayMetrics> metrics;
        resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
        metrics->GetHeightPixels(&bottomEdge);
    }
    Int32 height;
    anchor->GetHeight(&height);
    Int32 distanceToBottom = bottomEdge - ((*anchorPos)[1] + height) - yOffset;
    Int32 distanceToTop = (*anchorPos)[1] - t + yOffset;

    // anchorPos[1] is distance from anchor to top of screen
    Int32 returnedHeight = Elastos::Core::Math::Max(distanceToBottom, distanceToTop);
    if (mBackground != NULL) {
        Boolean result;
        mBackground->GetPadding(mTempRect, &result);
        Int32 top, bottom;
        mTempRect->GetTop(&top);
        mTempRect->GetBottom(&bottom);
        returnedHeight -= top + bottom;
    }

    *maxAvailableHeight = returnedHeight;
    return NOERROR;
}

ECode PopupWindow::Dismiss()
{
    Boolean res;
    if ((IsShowing(&res), res) && mPopupView != NULL) {
        mIsShowing = FALSE;
        UnregisterForScrollChanged();

        mWindowManager->RemoveViewImmediate(mPopupView);

        if (mPopupView != mContentView &&
                IViewGroup::Probe(mPopupView) != NULL) {
            IViewGroup::Probe(mPopupView)->RemoveViewInLayout(mContentView);
        }
        mPopupView = NULL;

        if (mOnDismissListener != NULL) {
            mOnDismissListener->OnDismiss();
        }
    }

    return NOERROR;
}

ECode PopupWindow::SetOnDismissListener(
    /* [in] */ IPopupWindowOnDismissListener* onDismissListener)
{
    mOnDismissListener = onDismissListener;

    return NOERROR;
}

ECode PopupWindow::Update()
{
    Boolean res;
    if ((IsShowing(&res), !res) || mContentView == NULL) {
        return NOERROR;
    }

    AutoPtr<IViewGroupLayoutParams> p;
    mPopupView->GetLayoutParams((IViewGroupLayoutParams**)&p);
    CWindowManagerLayoutParams* lp = (CWindowManagerLayoutParams*)IWindowManagerLayoutParams::Probe(p);

    Boolean update = FALSE;

    Int32 newAnim = ComputeAnimationResource();
    if (newAnim != lp->mWindowAnimations) {
        lp->mWindowAnimations = newAnim;
        update = TRUE;
    }

    Int32 newFlags = ComputeFlags(lp->mFlags);
    if (newFlags != lp->mFlags) {
        lp->mFlags = newFlags;
        update = TRUE;
    }

    if (update) {
        SetLayoutDirectionFromAnchor();
        IViewManager::Probe(mWindowManager)->UpdateViewLayout(mPopupView, p);
    }

    return NOERROR;
}

ECode PopupWindow::Update(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    AutoPtr<IViewGroupLayoutParams> p;
    mPopupView->GetLayoutParams((IViewGroupLayoutParams**)&p);
    CWindowManagerLayoutParams* lp = (CWindowManagerLayoutParams*)IWindowManagerLayoutParams::Probe(p);

    return Update(lp->mX, lp->mY, width, height, FALSE);
}

ECode PopupWindow::Update(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return Update(x, y, width, height, FALSE);
}

ECode PopupWindow::Update(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Boolean force)
{
    if (width != -1) {
        mLastWidth = width;
        SetWidth(width);
    }

    if (height != -1) {
        mLastHeight = height;
        SetHeight(height);
    }

    Boolean res;
    if ((IsShowing(&res), !res) || mContentView == NULL) {
        return NOERROR;
    }

    AutoPtr<IViewGroupLayoutParams> p;
    mPopupView->GetLayoutParams((IViewGroupLayoutParams**)&p);
    CWindowManagerLayoutParams* lp = (CWindowManagerLayoutParams*)IWindowManagerLayoutParams::Probe(p);

    Boolean update = force;

    Int32 finalWidth = mWidthMode < 0 ? mWidthMode : mLastWidth;
    if (width != -1 && lp->mWidth != finalWidth) {
        lp->mWidth = mLastWidth = finalWidth;
        update = TRUE;
    }

    Int32 finalHeight = mHeightMode < 0 ? mHeightMode : mLastHeight;
    if (height != -1 && lp->mHeight != finalHeight) {
        lp->mHeight = mLastHeight = finalHeight;
        update = TRUE;
    }

    if (lp->mX != x) {
        lp->mX = x;
        update = TRUE;
    }

    if (lp->mY != y) {
        lp->mY = y;
        update = TRUE;
    }

    Int32 newAnim = ComputeAnimationResource();
    if (newAnim != lp->mWindowAnimations) {
        lp->mWindowAnimations = newAnim;
        update = TRUE;
    }

    Int32 newFlags = ComputeFlags(lp->mFlags);
    if (newFlags != lp->mFlags) {
        lp->mFlags = newFlags;
        update = TRUE;
    }

    if (update) {
        SetLayoutDirectionFromAnchor();
        IViewManager::Probe(mWindowManager)->UpdateViewLayout(mPopupView, p);
    }

    return NOERROR;
}

ECode PopupWindow::Update(
    /* [in] */ IView* anchor,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Update(anchor, FALSE, 0, 0, TRUE, width, height, mAnchoredGravity);
    return NOERROR;
}

ECode PopupWindow::Update(
    /* [in] */ IView* anchor,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Update(anchor, TRUE, xoff, yoff, TRUE, width, height, mAnchoredGravity);
    return NOERROR;
}

void PopupWindow::Update(
    /* [in] */ IView* anchor,
    /* [in] */ Boolean updateLocation,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Boolean updateDimension,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 gravity)
{
    Boolean res;
    if ((IsShowing(&res), !res) || mContentView == NULL) {
        return;
    }

    AutoPtr<IWeakReference> oldAnchor = mAnchor;
    AutoPtr<IView> oldView;
    if (oldAnchor != NULL) {
        AutoPtr<IInterface> obj;
        oldAnchor->Resolve(EIID_IInterface, (IInterface**)&obj);
        oldView = IView::Probe(obj);
    }

    Boolean needsUpdate = updateLocation && (mAnchorXoff != xoff || mAnchorYoff != yoff);
    if (oldAnchor == NULL || oldView.Get() != anchor || (needsUpdate && !mIsDropdown)) {
        RegisterForScrollChanged(anchor, xoff, yoff, gravity);
    }
    else if (needsUpdate) {
        // No need to register again if this is a DropDown, showAsDropDown already did.
        mAnchorXoff = xoff;
        mAnchorYoff = yoff;
        mAnchoredGravity = gravity;
    }

    AutoPtr<IViewGroupLayoutParams> p;
    mPopupView->GetLayoutParams((IViewGroupLayoutParams**)&p);
    IWindowManagerLayoutParams* wmlp = IWindowManagerLayoutParams::Probe(p);
    CWindowManagerLayoutParams* lp = (CWindowManagerLayoutParams*)wmlp;

    if (updateDimension) {
        if (width == -1) {
            width = mPopupWidth;
        }
        else {
            mPopupWidth = width;
        }
        if (height == -1) {
            height = mPopupHeight;
        }
        else {
            mPopupHeight = height;
        }
    }

    Int32 x = lp->mX;
    Int32 y = lp->mY;

    if (updateLocation) {
        UpdateAboveAnchor(FindDropDownPosition(anchor, wmlp, xoff, yoff, gravity));
    }
    else {
        UpdateAboveAnchor(FindDropDownPosition(anchor, wmlp, mAnchorXoff, mAnchorYoff, mAnchoredGravity));
    }

    Update(lp->mX, lp->mY, width, height, x != lp->mX || y != lp->mY);
}

void PopupWindow::UnregisterForScrollChanged()
{
    AutoPtr<IView> anchor;
    if (mAnchor != NULL) {
        AutoPtr<IInterface> obj;
        mAnchor->Resolve(EIID_IInterface, (IInterface**)&obj);
        anchor = IView::Probe(obj);
    }
    if (anchor != NULL) {
        AutoPtr<IViewTreeObserver> vto;
        anchor->GetViewTreeObserver((IViewTreeObserver**)&vto);
        if (vto != NULL) {
            vto->RemoveOnScrollChangedListener(mOnScrollChangedListener);
        }
    }
    mAnchor = NULL;
}

void PopupWindow::RegisterForScrollChanged(
    /* [in] */ IView* anchor,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 gravity)
{
    assert(anchor != NULL);

    UnregisterForScrollChanged();

    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(anchor);
    mAnchor = NULL;
    wrs->GetWeakReference((IWeakReference**)&mAnchor);

    AutoPtr<IViewTreeObserver> vto;
    anchor->GetViewTreeObserver((IViewTreeObserver**)&vto);
    if (vto != NULL) {
        vto->AddOnScrollChangedListener(mOnScrollChangedListener);
    }

    mAnchorXoff = xoff;
    mAnchorYoff = yoff;
    mAnchoredGravity = gravity;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos

