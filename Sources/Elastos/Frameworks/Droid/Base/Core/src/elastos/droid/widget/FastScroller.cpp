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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/FastScroller.h"

#include "elastos/droid/animation/CAnimatorSet.h"
#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/animation/PropertyValuesHolder.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/view/CMotionEvent.h"
#include "elastos/droid/view/CView.h"
#include "elastos/droid/view/CViewConfiguration.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/utility/MathUtils.h"
#include "elastos/droid/widget/AbsListView.h"
#include "elastos/droid/widget/ExpandableListView.h"

#include "elastos/droid/widget/CImageView.h"
#include "elastos/droid/widget/CTextView.h"
#include "elastos/droid/R.h"

#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::R;
using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::ObjectAnimator;
using Elastos::Droid::Animation::IAnimatorSetBuilder;
using Elastos::Droid::Animation::IPropertyValuesHolder;
using Elastos::Droid::Animation::PropertyValuesHolder;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::Drawable::INinePatchDrawable;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Text::TextUtilsTruncateAt_MIDDLE;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::CView;
using Elastos::Droid::View::CViewConfiguration;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::CMotionEvent;
using Elastos::Droid::View::MotionEvent;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOverlay;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::ViewConfiguration;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Widget {

//==============================================================================
//              FastScroller::MyRunnable
//==============================================================================

FastScroller::MyRunnable::MyRunnable(
    /* [in] */ FastScroller* host)
    : mHost(host)
{
}

ECode FastScroller::MyRunnable::Run()
{
    mHost->SetState(FastScroller::STATE_NONE);
    return NOERROR;
}

//==============================================================================
//              FastScroller::MyRunnable
//==============================================================================

FastScroller::MyAnimatorListenerAdapter::MyAnimatorListenerAdapter(
    /* [in] */ FastScroller* host)
    : mHost(host)
{}

ECode FastScroller::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mShowingPrimary = !mHost->mShowingPrimary;
    return NOERROR;
}

//==============================================================================
//              FastScroller::Leftroperty
//==============================================================================

FastScroller::Leftroperty::Leftroperty(
    /* [in] */ const String& name)
    : Int32Property(name)
{}

ECode FastScroller::Leftroperty::Get(
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** rst)
{
    AutoPtr<IView> v = IView::Probe(obj);
    if (v == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 left;
    v->GetLeft(&left);
    AutoPtr<IInteger32> wrapper;
    CInteger32::New(left, (IInteger32**)&wrapper);
    *rst = wrapper;
    REFCOUNT_ADD(*rst)
    return NOERROR;
}

ECode FastScroller::Leftroperty::SetValue(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 value)
{
    AutoPtr<IView> view = IView::Probe(obj);
    view->SetLeft(value);
    return NOERROR;
}

//==============================================================================
//              FastScroller::RightProperty
//==============================================================================

FastScroller::RightProperty::RightProperty(
    /* [in] */ const String& name)
    : Int32Property(name)
{}

ECode FastScroller::RightProperty::Get(
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** rst)
{
    AutoPtr<IView> v = IView::Probe(obj);
    if (v == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 right;
    v->GetRight(&right);
    AutoPtr<IInteger32> wrapper;
    CInteger32::New(right, (IInteger32**)&wrapper);
    *rst = wrapper;
    REFCOUNT_ADD(*rst)
    return NOERROR;
}

ECode FastScroller::RightProperty::SetValue(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 value)
{
    AutoPtr<IView> view = IView::Probe(obj);
    view->SetRight(value);
    return NOERROR;
}
//==============================================================================
//              FastScroller::TopProperty
//==============================================================================

FastScroller::TopProperty::TopProperty(
    /* [in] */ const String& name)
    : Int32Property(name)
{}

ECode FastScroller::TopProperty::Get(
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** rst)
{
    AutoPtr<IView> v = IView::Probe(obj);
    if (v == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 top;
    v->GetTop(&top);
    AutoPtr<IInteger32> wrapper;
    CInteger32::New(top, (IInteger32**)&wrapper);
    *rst = wrapper;
    REFCOUNT_ADD(*rst)
    return NOERROR;
}

ECode FastScroller::TopProperty::SetValue(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 value)
{
    AutoPtr<IView> view = IView::Probe(obj);
    view->SetTop(value);
    return NOERROR;
}
//==============================================================================
//              FastScroller::ScrollFade
//==============================================================================

FastScroller::BottomProperty::BottomProperty(
    /* [in] */ const String& name)
    : Int32Property(name)
{}

ECode FastScroller::BottomProperty::Get(
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** rst)
{
    AutoPtr<IView> v = IView::Probe(obj);
    if (v == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 bottom;
    v->GetBottom(&bottom);
    AutoPtr<IInteger32> wrapper;
    CInteger32::New(bottom, (IInteger32**)&wrapper);
    *rst = wrapper;
    REFCOUNT_ADD(*rst)
    return NOERROR;
}

ECode FastScroller::BottomProperty::SetValue(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 value)
{
    AutoPtr<IView> view = IView::Probe(obj);
    view->SetBottom(value);
    return NOERROR;
}



//==============================================================================
//              FastScroller
//==============================================================================

const Int32 FastScroller::DURATION_FADE_OUT = 300;
const Int32 FastScroller::DURATION_FADE_IN = 150;
const Int32 FastScroller::DURATION_CROSS_FADE = 50;
const Int32 FastScroller::DURATION_RESIZE = 100;
const Int64 FastScroller::FADE_TIMEOUT = 1500;
const Int32 FastScroller::MIN_PAGES = 4;
const Int32 FastScroller::STATE_NONE = 0;
const Int32 FastScroller::STATE_VISIBLE = 1;
const Int32 FastScroller::STATE_DRAGGING = 2;
const Int32 FastScroller::OVERLAY_FLOATING = 0;
const Int32 FastScroller::OVERLAY_AT_THUMB = 1;
const Int32 FastScroller::OVERLAY_ABOVE_THUMB = 2;
const Int32 FastScroller::PREVIEW_LEFT = 0;
const Int32 FastScroller::PREVIEW_RIGHT = 1;

const Int64 FastScroller::TAP_TIMEOUT = ViewConfiguration::GetTapTimeout();
AutoPtr<IProperty> FastScroller::LEFT = new FastScroller::Leftroperty(String("left"));
AutoPtr<IProperty> FastScroller::TOP = new FastScroller::TopProperty(String("top"));
AutoPtr<IProperty> FastScroller::RIGHT = new FastScroller::RightProperty(String("right"));
AutoPtr<IProperty> FastScroller::BOTTOM = new FastScroller::BottomProperty(String("bottom"));

FastScroller::FastScroller()
    : mPreviewPadding(0)
    , mPreviewMinWidth(0)
    , mPreviewMinHeight(0)
    , mThumbMinWidth(0)
    , mThumbMinHeight(0)
    , mTextSize(0.0)
    , mTextAppearance(0)
    , mWidth(0)
    , mShowingPrimary(FALSE)
    , mScrollCompleted(FALSE)
    , mFirstVisibleItem(0)
    , mHeaderCount(0)
    , mCurrentSection(-1)
    , mScrollbarPosition(-1)
    , mLongList(FALSE)
    , mUpdatingLayout(FALSE)
    , mState(0)
    , mShowingPreview(FALSE)
    , mLayoutFromRight(FALSE)
    , mEnabled(FALSE)
    , mAlwaysShow(FALSE)
    , mOverlayPosition(0)
    , mScrollBarStyle(0)
    , mMatchDragPosition(FALSE)
    , mInitialTouchY(0.0)
    , mPendingDrag(-1)
    , mOldItemCount(0)
    , mOldChildCount(0)
{
}

ECode FastScroller::constructor(
    /* [in] */ IAbsListView* listView,
    /* [in] */ Int32 styleResId)
{
    CRect::New((IRect**)&mTempBounds);
    CRect::New((IRect**)&mTempMargins);
    CRect::New((IRect**)&mContainerRect);

    mPreviewResId = ArrayOf<Int32>::Alloc(2);

    mList = listView;
    IAdapterView::Probe(listView)->GetCount(&mOldItemCount);
    IViewGroup::Probe(listView)->GetChildCount(&mOldChildCount);

    AutoPtr<IContext> context;
    IView::Probe(listView)->GetContext((IContext**)&context);
    ViewConfiguration::Get(context)->GetScaledTouchSlop(&mScaledTouchSlop);
    IView::Probe(listView)->GetScrollBarStyle(&mScrollBarStyle);

    mScrollCompleted = TRUE;
    mState = STATE_VISIBLE;
    AutoPtr<IApplicationInfo> info;
    context->GetApplicationInfo((IApplicationInfo**)&info);
    Int32 version;
    info->GetTargetSdkVersion(&version);
    mMatchDragPosition = version >= Build::VERSION_CODES::HONEYCOMB;

    CImageView::New(context, (IImageView**)&mTrackImage);
    mTrackImage->SetScaleType(ImageViewScaleType_FIT_XY);
    CImageView::New(context, (IImageView**)&mThumbImage);
    mThumbImage->SetScaleType(ImageViewScaleType_FIT_XY);
    CView::New(context, (IView**)&mPreviewImage);
    mPreviewImage->SetAlpha(0.f);

    mPrimaryText = CreatePreviewTextView(context);
    mSecondaryText = CreatePreviewTextView(context);

    SetStyle(styleResId);

    AutoPtr<IViewGroupOverlay> overlay;
    AutoPtr<IViewOverlay> tmp;
    IView::Probe(listView)->GetOverlay((IViewOverlay**)&tmp);
    overlay = IViewGroupOverlay::Probe(tmp);
    mOverlay = overlay;
    overlay->Add(IView::Probe(mTrackImage));
    overlay->Add(IView::Probe(mThumbImage));
    overlay->Add(IView::Probe(mPreviewImage));
    overlay->Add(IView::Probe(mPrimaryText));
    overlay->Add(IView::Probe(mSecondaryText));

    GetSectionsFromIndexer();
    UpdateLongList(mOldChildCount, mOldItemCount);
    Int32 position;
    IView::Probe(listView)->GetVerticalScrollbarPosition(&position);
    SetScrollbarPosition(position);
    PostAutoHide();
    return NOERROR;
}

void FastScroller::SetStyle(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> context;
    IView::Probe(mList)->GetContext((IContext**)&context);
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::FastScroll);
    AutoPtr<ITypedArray> ta;

    context->ObtainStyledAttributes(
            NULL, attrIds, R::attr::fastScrollStyle, 0, (ITypedArray**)&ta);

    Int32 N;
    ta->GetIndexCount(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 index;
        ta->GetIndex(i, &index);
        switch (index) {
            case R::styleable::FastScroll_position:
                ta->GetInt32(index, OVERLAY_FLOATING, &mOverlayPosition);
                break;
            case R::styleable::FastScroll_backgroundLeft:
                Int32 backgroundLeft;
                ta->GetResourceId(index, 0, &backgroundLeft);
                (*mPreviewResId)[PREVIEW_LEFT] = backgroundLeft;
                break;
            case R::styleable::FastScroll_backgroundRight:
                Int32 backgroundRight;
                ta->GetResourceId(index, 0, &backgroundRight);
                (*mPreviewResId)[PREVIEW_RIGHT] = backgroundRight;
                break;
            case R::styleable::FastScroll_thumbDrawable:
                ta->GetDrawable(index, (IDrawable**)&mThumbDrawable);
                break;
            case R::styleable::FastScroll_trackDrawable:
                ta->GetDrawable(index, (IDrawable**)&mTrackDrawable);
                break;
            case R::styleable::FastScroll_textAppearance:
                ta->GetResourceId(index, 0, &mTextAppearance);
                break;
            case R::styleable::FastScroll_textColor:
                ta->GetColorStateList(index, (IColorStateList**)&mTextColor);
                break;
            case R::styleable::FastScroll_textSize:
                Int32 sizeTmp;
                ta->GetDimensionPixelSize(index, 0, &sizeTmp);
                mTextSize = sizeTmp;
                break;
            case R::styleable::FastScroll_minWidth:
                ta->GetDimensionPixelSize(index, 0, &mPreviewMinWidth);
                break;
            case R::styleable::FastScroll_minHeight:
                ta->GetDimensionPixelSize(index, 0, &mPreviewMinHeight);
                break;
            case R::styleable::FastScroll_thumbMinWidth:
                ta->GetDimensionPixelSize(index, 0, &mThumbMinWidth);
                break;
            case R::styleable::FastScroll_thumbMinHeight:
                ta->GetDimensionPixelSize(index, 0, &mThumbMinHeight);
                break;
            case R::styleable::FastScroll_padding:
                ta->GetDimensionPixelSize(index, 0, &mPreviewPadding);
                break;
        }
    }
}

/**
 * Removes this FastScroller overlay from the host view.
 */
void FastScroller::Remove()
{
    mOverlay->Remove(IView::Probe(mTrackImage));
    mOverlay->Remove(IView::Probe(mThumbImage));
    mOverlay->Remove(mPreviewImage);
    mOverlay->Remove(IView::Probe(mPrimaryText));
    mOverlay->Remove(IView::Probe(mSecondaryText));
}

/**
 * @param enabled Whether the fast scroll thumb is enabled.
 */
void FastScroller::SetEnabled(
    /* [in] */ Boolean enabled)
{
    if (mEnabled != enabled) {
        mEnabled = enabled;

        OnStateDependencyChanged(TRUE);
    }
}

/**
 * @return Whether the fast scroll thumb is enabled.
 */
Boolean FastScroller::IsEnabled()
{
    return mEnabled && (mLongList || mAlwaysShow);
}

/**
 * @param alwaysShow Whether the fast scroll thumb should always be shown
 */
void FastScroller::SetAlwaysShow(
    /* [in] */ Boolean alwaysShow)
{
    if (mAlwaysShow != alwaysShow) {
        mAlwaysShow = alwaysShow;

        OnStateDependencyChanged(FALSE);
    }
}

/**
 * @return Whether the fast scroll thumb will always be shown
 * @see #setAlwaysShow(boolean)
 */
Boolean FastScroller::IsAlwaysShowEnabled()
{
    return mAlwaysShow;
}

void FastScroller::SetScrollBarStyle(
    /* [in] */ Int32 style)
{
    if (mScrollBarStyle != style) {
        mScrollBarStyle = style;

        UpdateLayout();
    }
}

/**
 * Immediately transitions the fast scroller decorations to a hidden state.
 */
void FastScroller::Stop()
{
    SetState(STATE_NONE);
}

void FastScroller::SetScrollbarPosition(
    /* [in] */ Int32 position)
{
    if (position == IView::SCROLLBAR_POSITION_DEFAULT) {
        Boolean isLayoutRtl;
        IView::Probe(mList)->IsLayoutRtl(&isLayoutRtl);
        position = isLayoutRtl ?
                IView::SCROLLBAR_POSITION_LEFT : IView::SCROLLBAR_POSITION_RIGHT;
    }

    if (mScrollbarPosition != position) {
        mScrollbarPosition = position;
        mLayoutFromRight = position != IView::SCROLLBAR_POSITION_LEFT;

        Int32 previewResId = (*mPreviewResId)[mLayoutFromRight ? PREVIEW_RIGHT : PREVIEW_LEFT];
        mPreviewImage->SetBackgroundResource(previewResId);

        // Add extra padding for text.
        AutoPtr<IDrawable> background;
        mPreviewImage->GetBackground((IDrawable**)&background);
        if (background != NULL) {
            AutoPtr<IRect> padding = mTempBounds;
            Boolean rst;
            background->GetPadding(padding, &rst);
            padding->Offset(mPreviewPadding, mPreviewPadding);
            Int32 left, top, right, bottom;
            padding->GetLeft(&left);
            padding->GetRight(&right);
            padding->GetTop(&top);
            padding->GetBottom(&bottom);
            mPreviewImage->SetPadding(left, top, right, bottom);
        }

        // Requires re-layout.
        UpdateLayout();
    }
}

Int32 FastScroller::GetWidth()
{
    return mWidth;
}

ECode FastScroller::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    UpdateLayout();
    return NOERROR;
}

void FastScroller::OnItemCountChanged(
    /* [in] */ Int32 childCount,
    /* [in] */ Int32 itemCount)
{
    if (mOldItemCount != itemCount || mOldChildCount != childCount) {
        mOldItemCount = itemCount;
        mOldChildCount = childCount;

        Boolean hasMoreItems = itemCount - childCount > 0;
        if (hasMoreItems && mState != STATE_DRAGGING) {
            Int32 firstVisibleItem;
            IAdapterView::Probe(mList)->GetFirstVisiblePosition(&firstVisibleItem);
            SetThumbPos(GetPosFromItemCount(firstVisibleItem, childCount, itemCount));
        }

        UpdateLongList(childCount, itemCount);
    }
}

/**
 * Measures and layouts the scrollbar and decorations.
 */
void FastScroller::UpdateLayout()
{
    // Prevent re-entry when RTL properties change as a side-effect of
    // resolving padding.
    if (mUpdatingLayout) {
        return;
    }

    mUpdatingLayout = TRUE;

    UpdateContainerRect();

    LayoutThumb();
    LayoutTrack();

    AutoPtr<IRect> bounds = mTempBounds;
    MeasurePreview(IView::Probe(mPrimaryText), bounds);
    ApplyLayout(IView::Probe(mPrimaryText), bounds);
    MeasurePreview(IView::Probe(mSecondaryText), bounds);
    ApplyLayout(IView::Probe(mSecondaryText), bounds);

    if (mPreviewImage != NULL) {
        // Apply preview image padding.
        Int32 left, top, right, bottom;
        bounds->GetLeft(&left);
        bounds->GetRight(&right);
        bounds->GetTop(&top);
        bounds->GetBottom(&bottom);

        Int32 pLeft, pTop, pRight, pBottom;
        mPreviewImage->GetPaddingLeft(&pLeft);
        mPreviewImage->GetPaddingTop(&pTop);
        mPreviewImage->GetPaddingRight(&pRight);
        mPreviewImage->GetPaddingBottom(&pBottom);

        bounds->SetLeft(left - pLeft);
        bounds->SetRight(right + pRight);
        bounds->SetTop(top - pTop);
        bounds->SetBottom(bottom + pBottom);

        ApplyLayout(mPreviewImage, bounds);
    }

    mUpdatingLayout = FALSE;
}

void FastScroller::OnScroll(
    /* [in] */ Int32 firstVisibleItem,
    /* [in] */ Int32 visibleItemCount,
    /* [in] */ Int32 totalItemCount)
{
    if (!IsEnabled()) {
        SetState(STATE_NONE);
        return;
    }

    Boolean hasMoreItems = totalItemCount - visibleItemCount > 0;
    if (hasMoreItems && mState != STATE_DRAGGING) {
        SetThumbPos(GetPosFromItemCount(firstVisibleItem, visibleItemCount, totalItemCount));
    }

    mScrollCompleted = TRUE;

    if (mFirstVisibleItem != firstVisibleItem) {
        mFirstVisibleItem = firstVisibleItem;

        // Show the thumb, if necessary, and set up auto-fade.
        if (mState != STATE_DRAGGING) {
            SetState(STATE_VISIBLE);
            PostAutoHide();
        }
    }
}

void FastScroller::OnSectionsChanged()
{
    mListAdapter = NULL;
}

Boolean FastScroller::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    if (!IsEnabled()) {
        return FALSE;
    }

    Int32 mask;
    ev->GetActionMasked(&mask);
    Float x, y;
    switch (mask) {
        case IMotionEvent::ACTION_DOWN:
            ev->GetX(&x);
            ev->GetY(&y);
            if (IsPointInside(x, y)) {
                // If the parent has requested that its children delay
                // pressed state (e.g. is a scrolling container) then we
                // need to allow the parent time to decide whether it wants
                // to intercept events. If it does, we will receive a CANCEL
                // event.
                Boolean isInScrollingContainer;
                IView::Probe(mList)->IsInScrollingContainer(&isInScrollingContainer);

                if (!isInScrollingContainer) {
                    BeginDrag();
                    return TRUE;
                }

                ev->GetY(&mInitialTouchY);
                StartPendingDrag();
            }
            break;
        case IMotionEvent::ACTION_MOVE:
            ev->GetX(&x);
            ev->GetY(&y);
            if (!IsPointInside(x, y)) {
                CancelPendingDrag();
            } else if (mPendingDrag >= 0 && mPendingDrag <= SystemClock::GetUptimeMillis()) {
                BeginDrag();

                Float pos = GetPosFromMotionEvent(mInitialTouchY);
                ScrollTo(pos);

                return OnTouchEvent(ev);
            }
            break;
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            CancelPendingDrag();
            break;
    }

    return FALSE;
}

Boolean FastScroller::OnInterceptHoverEvent(
    /* [in] */ IMotionEvent* ev)
{
    if (!IsEnabled()) {
        return false;
    }

    Int32 actionMasked;
    ev->GetActionMasked(&actionMasked);
    Float x, y;
    ev->GetX(&x);
    ev->GetY(&y);
    if ((actionMasked == IMotionEvent::ACTION_HOVER_ENTER
            || actionMasked == IMotionEvent::ACTION_HOVER_MOVE) && mState == STATE_NONE
            && IsPointInside(x, y)) {
        SetState(STATE_VISIBLE);
        PostAutoHide();
    }

    return FALSE;
}

Boolean FastScroller::OnTouchEvent(
    /* [in] */ IMotionEvent* me)
{
    if (!IsEnabled()) {
        return FALSE;
    }

    Int32 mask;
    Float y;
    me->GetActionMasked(&mask);
    switch (mask) {
        case IMotionEvent::ACTION_UP: {
            if (mPendingDrag >= 0) {
                // Allow a tap to scroll.
                BeginDrag();
                me->GetY(&y);

                Float pos = GetPosFromMotionEvent(y);
                SetThumbPos(pos);
                ScrollTo(pos);

                // Will hit the STATE_DRAGGING check below
            }

            if (mState == STATE_DRAGGING) {
                if (mList != NULL) {
                    // ViewGroup does the right thing already, but there might
                    // be other classes that don't properly reset on touch-up,
                    // so do this explicitly just in case.
                    IViewParent::Probe(mList)->RequestDisallowInterceptTouchEvent(FALSE);
                    ((AbsListView*)mList)->ReportScrollStateChange(IAbsListViewOnScrollListener::SCROLL_STATE_IDLE);
                }

                SetState(STATE_VISIBLE);
                PostAutoHide();

                return TRUE;
            }
        } break;

        case IMotionEvent::ACTION_MOVE: {
            me->GetY(&y);
            if (mPendingDrag >= 0 && Elastos::Core::Math::Abs(y - mInitialTouchY) > mScaledTouchSlop) {
                BeginDrag();

                // Will hit the STATE_DRAGGING check below
            }

            if (mState == STATE_DRAGGING) {
                // TODO: Ignore jitter.
                Float pos = GetPosFromMotionEvent(y);
                SetThumbPos(pos);

                // If the previous scrollTo is still pending
                if (mScrollCompleted) {
                    ScrollTo(pos);
                }

                return TRUE;
            }
        } break;

        case IMotionEvent::ACTION_CANCEL: {
            CancelPendingDrag();
        } break;
    }

    return FALSE;
}

void FastScroller::UpdateAppearance()
{
    AutoPtr<IContext> context;
    IView::Probe(mList)->GetContext((IContext**)&context);
    Int32 width = 0;

    // Add track to overlay if it has an image.
    mTrackImage->SetImageDrawable(mTrackDrawable);
    if (mTrackDrawable != NULL) {
        Int32 intrinsicWidth;
        mTrackDrawable->GetIntrinsicWidth(&intrinsicWidth);
        width = Elastos::Core::Math::Max(width, intrinsicWidth);
    }

    // Add thumb to overlay if it has an image.
    mThumbImage->SetImageDrawable(mThumbDrawable);
    IView::Probe(mThumbImage)->SetMinimumWidth(mThumbMinWidth);
    IView::Probe(mThumbImage)->SetMinimumHeight(mThumbMinHeight);
    if (mThumbDrawable != NULL) {
        Int32 intrinsicWidth;
        mTrackDrawable->GetIntrinsicWidth(&intrinsicWidth);
        width = Elastos::Core::Math::Max(width, intrinsicWidth);
    }

    // Account for minimum thumb width.
    mWidth = Elastos::Core::Math::Max(width, mThumbMinWidth);

    mPreviewImage->SetMinimumWidth(mPreviewMinWidth);
    mPreviewImage->SetMinimumHeight(mPreviewMinHeight);

    if (mTextAppearance != 0) {
        mPrimaryText->SetTextAppearance(context, mTextAppearance);
        mSecondaryText->SetTextAppearance(context, mTextAppearance);
    }

    if (mTextColor != NULL) {
        mPrimaryText->SetTextColor(mTextColor);
        mSecondaryText->SetTextColor(mTextColor);
    }

    if (mTextSize > 0) {
        mPrimaryText->SetTextSize(ITypedValue::COMPLEX_UNIT_PX, mTextSize);
        mSecondaryText->SetTextSize(ITypedValue::COMPLEX_UNIT_PX, mTextSize);
    }

    Int32 textMinSize = Elastos::Core::Math::Max(0, mPreviewMinHeight);
    IView::Probe(mPrimaryText)->SetMinimumWidth(textMinSize);
    IView::Probe(mPrimaryText)->SetMinimumHeight(textMinSize);
    mPrimaryText->SetIncludeFontPadding(FALSE);
    IView::Probe(mSecondaryText)->SetMinimumWidth(textMinSize);
    IView::Probe(mSecondaryText)->SetMinimumHeight(textMinSize);
    mSecondaryText->SetIncludeFontPadding(FALSE);

    RefreshDrawablePressedState();
}

/**
 * Called when one of the variables affecting enabled state changes.
 *
 * @param peekIfEnabled whether the thumb should peek, if enabled
 */
void FastScroller::OnStateDependencyChanged(
    /* [in] */ Boolean peekIfEnabled)
{
    if (IsEnabled()) {
        if (IsAlwaysShowEnabled()) {
            SetState(STATE_VISIBLE);
        } else if (mState == STATE_VISIBLE) {
            PostAutoHide();
        } else if (peekIfEnabled) {
            SetState(STATE_VISIBLE);
            PostAutoHide();
        }
    } else {
        Stop();
    }

    IView::Probe(mList)->ResolvePadding();
}

void FastScroller::UpdateLongList(
    /* [in] */ Int32 childCount,
    /* [in] */ Int32 itemCount)
{
    Boolean longList = childCount > 0 && itemCount / childCount >= MIN_PAGES;
    if (mLongList != longList) {
        mLongList = longList;

        OnStateDependencyChanged(FALSE);
    }
}

/**
 * Creates a view into which preview text can be placed.
 */
AutoPtr<ITextView> FastScroller::CreatePreviewTextView(
    /* [in] */ IContext* context)
{
    AutoPtr<IViewGroupLayoutParams> params;
    CViewGroupLayoutParams::New(
            IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::WRAP_CONTENT, (IViewGroupLayoutParams**)&params);
    AutoPtr<ITextView> textView;
    CTextView::New(context, (ITextView**)&textView);
    IView::Probe(textView)->SetLayoutParams(params);
    textView->SetSingleLine(true);
    textView->SetEllipsize(TextUtilsTruncateAt_MIDDLE);
    textView->SetGravity(IGravity::CENTER);
    IView::Probe(textView)->SetAlpha(0.f);

    // Manually propagate inherited layout direction.
    Int32 layoutDirection;
    IView::Probe(mList)->GetLayoutDirection(&layoutDirection);
    IView::Probe(textView)->SetLayoutDirection(layoutDirection);

    return textView;
}

/**
 * Layouts a view within the specified bounds and pins the pivot point to
 * the appropriate edge.
 *
 * @param view The view to layout.
 * @param bounds Bounds at which to layout the view.
 */
void FastScroller::ApplyLayout(
    /* [in] */ IView* view,
    /* [in] */ IRect* bounds)
{
    Int32 left, top, right, bottom;
    bounds->GetLeft(&left);
    bounds->GetRight(&right);
    bounds->GetTop(&top);
    bounds->GetBottom(&bottom);
    view->Layout(left, top, right, bottom);
    view->SetPivotX(mLayoutFromRight ? right - left : 0);
}

/**
 * Measures the preview text bounds, taking preview image padding into
 * account. This method should only be called after {@link #layoutThumb()}
 * and {@link #layoutTrack()} have both been called at least once.
 *
 * @param v The preview text view to measure.
 * @param out Rectangle into which measured bounds are placed.
 */
void FastScroller::MeasurePreview(
    /* [in] */ IView* v,
    /* [in] */ IRect* out)
{
    // Apply the preview image's padding as layout margins.
    AutoPtr<IRect> margins = mTempMargins;

    Int32 pLeft, pTop, pRight, pBottom;
    mPreviewImage->GetPaddingLeft(&pLeft);
    mPreviewImage->GetPaddingTop(&pTop);
    mPreviewImage->GetPaddingRight(&pRight);
    mPreviewImage->GetPaddingBottom(&pBottom);

    margins->SetLeft(pLeft);
    margins->SetRight(pTop);
    margins->SetTop(pRight);
    margins->SetBottom(pBottom);

    if (mOverlayPosition == OVERLAY_FLOATING) {
        MeasureFloating(v, margins, out);
    } else {
        MeasureViewToSide(v, IView::Probe(mThumbImage), margins, out);
    }
}

/**
 * Measures the bounds for a view that should be laid out against the edge
 * of an adjacent view. If no adjacent view is provided, lays out against
 * the list edge.
 *
 * @param view The view to measure for layout.
 * @param adjacent (Optional) The adjacent view, may be null to align to the
 *            list edge.
 * @param margins Layout margins to apply to the view.
 * @param out Rectangle into which measured bounds are placed.
 */
void FastScroller::MeasureViewToSide(
    /* [in] */ IView* view,
    /* [in] */ IView* adjacent,
    /* [in] */ IRect* margins,
    /* [in] */ IRect* out)
{
    Int32 marginLeft;
    Int32 marginTop;
    Int32 marginRight;
    if (margins == NULL) {
        marginLeft = 0;
        marginTop = 0;
        marginRight = 0;
    } else {
        margins->GetLeft(&marginLeft);
        margins->GetTop(&marginTop);
        margins->GetRight(&marginRight);
    }

    AutoPtr<IRect> container = mContainerRect;
    Int32 containerWidth;
    container->GetWidth(&containerWidth);
    Int32 maxWidth;
    if (adjacent == NULL) {
        maxWidth = containerWidth;
    } else if (mLayoutFromRight) {
        adjacent->GetLeft(&maxWidth);
    } else {
        Int32 r;
        adjacent->GetRight(&r);
        maxWidth = containerWidth - r;
    }

    Int32 adjMaxWidth = maxWidth - marginLeft - marginRight;
    Int32 widthMeasureSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(adjMaxWidth, Elastos::Droid::View::View::MeasureSpec::AT_MOST);
    Int32 heightMeasureSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(0, Elastos::Droid::View::View::MeasureSpec::UNSPECIFIED);
    view->Measure(widthMeasureSpec, heightMeasureSpec);

    // Align to the left or right.
    Int32 measuredWidth;
    view->GetMeasuredWidth(&measuredWidth);
    Int32 width = Elastos::Core::Math::Min(adjMaxWidth, measuredWidth);
    Int32 left;
    Int32 right;
    if (mLayoutFromRight) {
        container->GetRight(&right);
        if (adjacent != NULL) {
            adjacent->GetLeft(&left);
        }
        right = (adjacent == NULL ? right : left) - marginRight;
        left = right - width;
    } else {
        container->GetLeft(&left);
        if (adjacent != NULL) {
            adjacent->GetRight(&right);
        }
        left = (adjacent == NULL ? left : right) + marginLeft;
        right = left + width;
    }

    // Don't adjust the vertical position.
    Int32 top = marginTop;
    Int32 measuredHeight;
    view->GetMeasuredHeight(&measuredHeight);
    Int32 bottom = top + measuredHeight;
    out->Set(left, top, right, bottom);
}

void FastScroller::MeasureFloating(
    /* [in] */ IView* preview,
    /* [in] */ IRect* margins,
    /* [in] */ IRect* out)
{
    Int32 marginLeft;
    Int32 marginTop;
    Int32 marginRight;
    if (margins == NULL) {
        marginLeft = 0;
        marginTop = 0;
        marginRight = 0;
    } else {
        margins->GetLeft(&marginLeft);
        margins->GetTop(&marginTop);
        margins->GetRight(&marginRight);
    }

    AutoPtr<IRect> container = mContainerRect;
    Int32 containerWidth;
    container->GetWidth(&containerWidth);
    Int32 adjMaxWidth = containerWidth - marginLeft - marginRight;
    Int32 widthMeasureSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(adjMaxWidth, Elastos::Droid::View::View::MeasureSpec::AT_MOST);
    Int32 heightMeasureSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(0, Elastos::Droid::View::View::MeasureSpec::UNSPECIFIED);
    preview->Measure(widthMeasureSpec, heightMeasureSpec);

    // Align at the vertical center, 10% from the top.
    Int32 containerHeight;
    container->GetHeight(&containerHeight);
    Int32 width;
    preview->GetMeasuredWidth(&width);
    Int32 containerTop;
    container->GetTop(&containerTop);
    Int32 top = containerHeight / 10 + marginTop + containerTop;
    Int32 measuredHeight;
    preview->GetMeasuredHeight(&measuredHeight);
    Int32 bottom = top + measuredHeight;
    Int32 containerLeft;
    container->GetTop(&containerLeft);
    Int32 left = (containerWidth - width) / 2 + containerLeft;
    Int32 right = left + width;
    out->Set(left, top, right, bottom);
}

/**
 * Updates the container rectangle used for layout.
 */
void FastScroller::UpdateContainerRect()
{
    AutoPtr<IAbsListView> list = mList;
    IView::Probe(list)->ResolvePadding();

    AutoPtr<IRect> container = mContainerRect;
    container->SetLeft(0);
    container->SetTop(0);
    Int32 w, h;
    IView::Probe(list)->GetWidth(&w);
    IView::Probe(list)->GetHeight(&h);
    container->SetRight(w);
    container->SetBottom(h);

    Int32 scrollbarStyle = mScrollBarStyle;
    if (scrollbarStyle == IView::SCROLLBARS_INSIDE_INSET
            || scrollbarStyle == IView::SCROLLBARS_INSIDE_OVERLAY) {
        Int32 pLeft, pTop, pRight, pBottom;
        IView::Probe(list)->GetPaddingLeft(&pLeft);
        IView::Probe(list)->GetPaddingTop(&pTop);
        IView::Probe(list)->GetPaddingRight(&pRight);
        IView::Probe(list)->GetPaddingBottom(&pBottom);

        Int32 left, top, right, bottom;
        container->GetLeft(&left);
        container->GetRight(&right);
        container->GetTop(&top);
        container->GetBottom(&bottom);


        container->SetLeft(left + pLeft);
        container->SetRight(right - pRight);
        container->SetTop(top + pTop);
        container->SetBottom(bottom - pBottom);

        // In inset mode, we need to adjust for padded scrollbar width.
        if (scrollbarStyle == IView::SCROLLBARS_INSIDE_INSET) {
            Int32 width = GetWidth();
            if (mScrollbarPosition == IView::SCROLLBAR_POSITION_RIGHT) {
                container->GetRight(&right);
                container->SetRight(right + width);
            } else {
                container->GetRight(&left);
                container->SetLeft(left - width);
            }
        }
    }
}

/**
 * Lays out the thumb according to the current scrollbar position.
 */
void FastScroller::LayoutThumb()
{
    AutoPtr<IRect> bounds = mTempBounds;
    MeasureViewToSide(IView::Probe(mThumbImage), NULL, NULL, bounds);
    ApplyLayout(IView::Probe(mThumbImage), bounds);
}

/**
 * Lays out the track centered on the thumb. Must be called after
 * {@link #layoutThumb}.
 */
void FastScroller::LayoutTrack()
{
    AutoPtr<IView> track = IView::Probe(mTrackImage);
    AutoPtr<IView> thumb = IView::Probe(mThumbImage);
    AutoPtr<IRect> container = mContainerRect;
    Int32 containerWidth;
    container->GetWidth(&containerWidth);
    Int32 widthMeasureSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(containerWidth, Elastos::Droid::View::View::MeasureSpec::AT_MOST);
    Int32 heightMeasureSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(0, Elastos::Droid::View::View::MeasureSpec::UNSPECIFIED);
    track->Measure(widthMeasureSpec, heightMeasureSpec);

    Int32 trackWidth;
    track->GetMeasuredWidth(&trackWidth);
    Int32 thumbHalfHeight;
    if (thumb == NULL) {
        thumbHalfHeight = 0;
    } else {
        thumb->GetHeight(&thumbHalfHeight);
        thumbHalfHeight = thumbHalfHeight / 2;
    }
    Int32 left;
    if (thumb == NULL) {
        left = 0;
    } else {
        Int32 tLeft, tWidth;
        thumb->GetLeft(&tLeft);
        thumb->GetWidth(&tWidth);
        left = tLeft + (tWidth - trackWidth) / 2;
    }
    Int32 right = left + trackWidth;
    Int32 tTop, tBottom;
    container->GetTop(&tTop);
    container->GetBottom(&tBottom);
    Int32 top = tTop + thumbHalfHeight;
    Int32 bottom = tBottom - thumbHalfHeight;
    track->Layout(left, top, right, bottom);
}

void FastScroller::SetState(
    /* [in] */ Int32 state)
{
    Boolean rst;
    IView::Probe(mList)->RemoveCallbacks(mDeferHide, &rst);

    if (mAlwaysShow && state == STATE_NONE) {
        state = STATE_VISIBLE;
    }

    if (state == mState) {
        return;
    }

    switch (state) {
        case STATE_NONE:
            TransitionToHidden();
            break;
        case STATE_VISIBLE:
            TransitionToVisible();
            break;
        case STATE_DRAGGING:
            if (TransitionPreviewLayout(mCurrentSection)) {
                TransitionToDragging();
            } else {
                TransitionToVisible();
            }
            break;
    }

    mState = state;

    RefreshDrawablePressedState();
}

void FastScroller::RefreshDrawablePressedState()
{
    Boolean isPressed = mState == STATE_DRAGGING;
    IView::Probe(mThumbImage)->SetPressed(isPressed);
    IView::Probe(mTrackImage)->SetPressed(isPressed);
}

/**
 * Shows nothing.
 */
void FastScroller::TransitionToHidden()
{
    if (mDecorAnimation != NULL) {
        IAnimator::Probe(mDecorAnimation)->Cancel();
    }

    AutoPtr<ArrayOf<IView*> > params1 = ArrayOf<IView*>::Alloc(5);
    params1->Set(0, IView::Probe(mThumbImage));
    params1->Set(1, IView::Probe(mTrackImage));
    params1->Set(2, mPreviewImage);
    params1->Set(3, IView::Probe(mPrimaryText));
    params1->Set(4, IView::Probe(mSecondaryText));
    AutoPtr<IAnimator> fadeOut = GroupAnimatorOfFloat(Elastos::Droid::View::View::ALPHA, 0.f, params1);
    fadeOut->SetDuration(DURATION_FADE_OUT);

    // Push the thumb and track outside the list bounds.
    Float offset;
    Int32 rstTmp;
    IView::Probe(mThumbImage)->GetWidth(&rstTmp);
    offset = rstTmp;
    if (mLayoutFromRight) {
        offset = -offset;
    }

    AutoPtr<ArrayOf<IView*> > params2 = ArrayOf<IView*>::Alloc(2);
    params2->Set(0, IView::Probe(mThumbImage));
    params2->Set(1, IView::Probe(mTrackImage));
    AutoPtr<IAnimator> slideOut = GroupAnimatorOfFloat(
            Elastos::Droid::View::View::TRANSLATION_X, offset, params2);
    mDecorAnimation = NULL;
    CAnimatorSet::New((IAnimatorSet**)&mDecorAnimation);
    AutoPtr<ArrayOf<IAnimator*> > params3 = ArrayOf<IAnimator*>::Alloc(2);
    params3->Set(0, fadeOut);
    params3->Set(1, slideOut);
    mDecorAnimation->PlayTogether(params3);
    IAnimator::Probe(mDecorAnimation)->Start();

    mShowingPreview = FALSE;
}

/**
 * Shows the thumb and track.
 */
void FastScroller::TransitionToVisible()
{
    if (mDecorAnimation != NULL) {
        IAnimator::Probe(mDecorAnimation)->Cancel();
    }

    assert(mThumbImage && mTrackImage && mPreviewImage && mPrimaryText && mSecondaryText);

    AutoPtr<ArrayOf<IView*> > params1 = ArrayOf<IView*>::Alloc(2);
    params1->Set(0, IView::Probe(mThumbImage));
    params1->Set(1, IView::Probe(mTrackImage));
    AutoPtr<IAnimator> fadeIn = GroupAnimatorOfFloat(Elastos::Droid::View::View::ALPHA, 1.f, params1);
    fadeIn->SetDuration(DURATION_FADE_IN);

    AutoPtr<ArrayOf<IView*> > params2 = ArrayOf<IView*>::Alloc(3);
    params2->Set(0, IView::Probe(mPreviewImage));
    params2->Set(1, IView::Probe(mPrimaryText));
    params2->Set(1, IView::Probe(mSecondaryText));
    AutoPtr<IAnimator> fadeOut = GroupAnimatorOfFloat(
            Elastos::Droid::View::View::ALPHA, 0.f, params2);
    fadeOut->SetDuration(DURATION_FADE_OUT);

    AutoPtr<IAnimator> slideIn = GroupAnimatorOfFloat(
            Elastos::Droid::View::View::TRANSLATION_X, 0.f, params1);
    slideIn->SetDuration(DURATION_FADE_IN);

    mDecorAnimation = NULL;
    CAnimatorSet::New((IAnimatorSet**)&mDecorAnimation);
    AutoPtr<ArrayOf<IAnimator*> > params3 = ArrayOf<IAnimator*>::Alloc(3);
    params3->Set(0, fadeIn);
    params3->Set(1, fadeOut);
    params3->Set(2, slideIn);
    mDecorAnimation->PlayTogether(params3);
    IAnimator::Probe(mDecorAnimation)->Start();

    mShowingPreview = FALSE;
}

/**
 * Shows the thumb, preview, and track.
 */
void FastScroller::TransitionToDragging()
{
    if (mDecorAnimation != NULL) {
        IAnimator::Probe(mDecorAnimation)->Cancel();
    }

    AutoPtr<ArrayOf<IView*> > params1 = ArrayOf<IView*>::Alloc(3);
    params1->Set(0, IView::Probe(mThumbImage));
    params1->Set(1, IView::Probe(mTrackImage));
    params1->Set(2, IView::Probe(mPreviewImage));
    AutoPtr<IAnimator> fadeIn = GroupAnimatorOfFloat(Elastos::Droid::View::View::ALPHA, 1.f, params1);
    fadeIn->SetDuration(DURATION_FADE_IN);

    AutoPtr<ArrayOf<IView*> > params2 = ArrayOf<IView*>::Alloc(2);
    params2->Set(0, IView::Probe(mThumbImage));
    params2->Set(1, IView::Probe(mTrackImage));

    AutoPtr<IAnimator> slideIn = GroupAnimatorOfFloat(
            Elastos::Droid::View::View::TRANSLATION_X, 0.f, params2);
    slideIn->SetDuration(DURATION_FADE_IN);

    mDecorAnimation = NULL;
    CAnimatorSet::New((IAnimatorSet**)&mDecorAnimation);
    AutoPtr<ArrayOf<IAnimator*> > params3 = ArrayOf<IAnimator*>::Alloc(2);
    params3->Set(0, fadeIn);
    params3->Set(1, slideIn);
    mDecorAnimation->PlayTogether(params3);
    IAnimator::Probe(mDecorAnimation)->Start();

    mShowingPreview = TRUE;
}

void FastScroller::PostAutoHide()
{
    Boolean rst;
    IView::Probe(mList)->RemoveCallbacks(mDeferHide, &rst);
    IView::Probe(mList)->PostDelayed(mDeferHide, FADE_TIMEOUT, &rst);
}

void FastScroller::GetSectionsFromIndexer()
{
    mSectionIndexer = NULL;

    AutoPtr<IAdapter> adapter;
    IAdapterView::Probe(mList)->GetAdapter((IAdapter**)&adapter);
    if (IHeaderViewListAdapter::Probe(adapter) != NULL) {
        IHeaderViewListAdapter::Probe(adapter)->GetHeadersCount(&mHeaderCount);
        AutoPtr<IListAdapter> lAdapter;
        IWrapperListAdapter::Probe(adapter)->GetWrappedAdapter((IListAdapter**)&lAdapter);
        adapter = IAdapter::Probe(lAdapter);
    }

    if (IExpandableListConnector::Probe(adapter) != NULL) {
        AutoPtr<IExpandableListConnector> expConnector = IExpandableListConnector::Probe(adapter);
        AutoPtr<IExpandableListAdapter> expAdapter;
        expConnector->GetAdapter((IExpandableListAdapter**)&expAdapter);

        if (ISectionIndexer::Probe(expAdapter) != NULL) {
            mSectionIndexer = ISectionIndexer::Probe(expAdapter);
            mListAdapter = adapter;
            mSectionIndexer->GetSections((ArrayOf<IInterface*>**)&mSections);
        }
    } else if (ISectionIndexer::Probe(adapter) != NULL) {
        mListAdapter = adapter;
        mSectionIndexer = ISectionIndexer::Probe(adapter);
        mSectionIndexer->GetSections((ArrayOf<IInterface*>**)&mSections);
    } else {
        mListAdapter = adapter;
        mSections = NULL;
    }
}

/**
 * Scrolls to a specific position within the section
 * @param position
 */
void FastScroller::ScrollTo(
    /* [in] */ Float position)
{
    mScrollCompleted = FALSE;

    Int32 count;
    IAdapterView::Probe(mList)->GetCount(&count);
    AutoPtr<ArrayOf<IInterface*> > sections = mSections;
    Int32 sectionCount = sections == NULL ? 0 : sections->GetLength();
    Int32 sectionIndex;
    if (sections != NULL && sectionCount > 1) {
        Int32 exactSection = MathUtils::Constrain(
                (Int32) (position * sectionCount), 0, sectionCount - 1);
        Int32 targetSection = exactSection;
        Int32 targetIndex;
        mSectionIndexer->GetPositionForSection(targetSection, &targetIndex);
        sectionIndex = targetSection;

        // Given the expected section and index, the following code will
        // try to account for missing sections (no names starting with..)
        // It will compute the scroll space of surrounding empty sections
        // and interpolate the currently visible letter's range across the
        // available space, so that there is always some list movement while
        // the user moves the thumb.
        Int32 nextIndex = count;
        Int32 prevIndex = targetIndex;
        Int32 prevSection = targetSection;
        Int32 nextSection = targetSection + 1;

        // Assume the next section is unique
        if (targetSection < sectionCount - 1) {
            mSectionIndexer->GetPositionForSection(targetSection + 1, &nextIndex);
        }

        // Find the previous index if we're slicing the previous section
        if (nextIndex == targetIndex) {
            // Non-existent letter
            while (targetSection > 0) {
                targetSection--;
                mSectionIndexer->GetPositionForSection(targetSection, &prevIndex);
                if (prevIndex != targetIndex) {
                    prevSection = targetSection;
                    sectionIndex = targetSection;
                    break;
                } else if (targetSection == 0) {
                    // When section reaches 0 here, sectionIndex must follow it.
                    // Assuming mSectionIndexer.getPositionForSection(0) == 0.
                    sectionIndex = 0;
                    break;
                }
            }
        }

        // Find the next index, in case the assumed next index is not
        // unique. For instance, if there is no P, then request for P's
        // position actually returns Q's. So we need to look ahead to make
        // sure that there is really a Q at Q's position. If not, move
        // further down...
        Int32 nextNextSection = nextSection + 1;
        Int32 whileIndex;
        while (nextNextSection < sectionCount &&
                (mSectionIndexer->GetPositionForSection(nextNextSection, &whileIndex), whileIndex) == nextIndex) {
            nextNextSection++;
            nextSection++;
        }

        // Compute the beginning and ending scroll range percentage of the
        // currently visible section. This could be equal to or greater than
        // (1 / nSections). If the target position is near the previous
        // position, snap to the previous position.
        Float prevPosition = (Float) prevSection / sectionCount;
        Float nextPosition = (Float) nextSection / sectionCount;
        Float snapThreshold = (count == 0) ? Elastos::Core::Math::FLOAT_MAX_VALUE : .125f / count;
        if (prevSection == exactSection && position - prevPosition < snapThreshold) {
            targetIndex = prevIndex;
        } else {
            targetIndex = prevIndex + (Int32) ((nextIndex - prevIndex) * (position - prevPosition)
                / (nextPosition - prevPosition));
        }

        // Clamp to valid positions.
        targetIndex = MathUtils::Constrain(targetIndex, 0, count - 1);

        if (IExpandableListView::Probe(mList) != NULL) {
            AutoPtr<IExpandableListView> expList = IExpandableListView::Probe(mList);
            Int32 pos;
            expList->GetFlatListPosition(ExpandableListView::GetPackedPositionForGroup(targetIndex + mHeaderCount), &pos);
            IAbsListView::Probe(expList)->SetSelectionFromTop(pos, 0);
        } else if (IListView::Probe(mList) != NULL) {
            AutoPtr<IListView> list = IListView::Probe(mList);
            IAbsListView::Probe(mList)->SetSelectionFromTop(targetIndex + mHeaderCount, 0);
        } else {
            IAdapterView::Probe(mList)->SetSelection(targetIndex + mHeaderCount);
        }
    } else {
        Int32 index = MathUtils::Constrain((Int32) (position * count), 0, count - 1);

        if (IExpandableListView::Probe(mList) != NULL) {
            AutoPtr<IExpandableListView> expList = IExpandableListView::Probe(mList);
            Int32 pos;
            expList->GetFlatListPosition(ExpandableListView::GetPackedPositionForGroup(index + mHeaderCount), &pos);
            IAbsListView::Probe(expList)->SetSelectionFromTop(pos, 0);
        } else if (IListView::Probe(mList) != NULL) {
            IAdapterView::Probe(mList)->SetSelection(index + mHeaderCount);
        } else {
            IAdapterView::Probe(mList)->SetSelection(index + mHeaderCount);
        }

        sectionIndex = -1;
    }

    if (mCurrentSection != sectionIndex) {
        mCurrentSection = sectionIndex;

        Boolean hasPreview = TransitionPreviewLayout(sectionIndex);
        if (!mShowingPreview && hasPreview) {
            TransitionToDragging();
        } else if (mShowingPreview && !hasPreview) {
            TransitionToVisible();
        }
    }
}


/**
 * Transitions the preview text to a new section. Handles animation,
 * measurement, and layout. If the new preview text is empty, returns false.
 *
 * @param sectionIndex The section index to which the preview should
 *            transition.
 * @return False if the new preview text is empty.
 */
Boolean FastScroller::TransitionPreviewLayout(
    /* [in] */ Int32 sectionIndex)
{
    AutoPtr<ArrayOf<IInterface*> > sections = mSections;
    String text;
    if (sections != NULL && sectionIndex >= 0 && sectionIndex < sections->GetLength()) {
        IInterface* section = (*sections)[sectionIndex];
        if (section != NULL) {
            IObject::Probe(section)->ToString(&text);
        }
    }

    AutoPtr<IRect> bounds = mTempBounds;
    AutoPtr<IView> preview = mPreviewImage;
    AutoPtr<ITextView> showing;
    AutoPtr<ITextView> target;
    if (mShowingPrimary) {
        showing = mPrimaryText;
        target = mSecondaryText;
    } else {
        showing = mSecondaryText;
        target = mPrimaryText;
    }

    // Set and layout target immediately.
    target->SetText(CoreUtils::Convert(text));
    MeasurePreview(IView::Probe(target), bounds);
    ApplyLayout(IView::Probe(target), bounds);

    if (mPreviewAnimation != NULL) {
        IAnimator::Probe(mPreviewAnimation)->Cancel();
    }

    // Cross-fade preview text.
    AutoPtr<IAnimator> showTarget = AnimateAlpha(IView::Probe(target), 1.f);
    showTarget->SetDuration(DURATION_CROSS_FADE);
    AutoPtr<IAnimator> hideShowing = AnimateAlpha(IView::Probe(showing), 0.f);
    hideShowing->SetDuration(DURATION_CROSS_FADE);
    hideShowing->AddListener(mSwitchPrimaryListener);

    // Apply preview image padding and animate bounds, if necessary.

    Int32 pLeft, pTop, pRight, pBottom;
    preview->GetPadding(&pLeft, &pTop, &pRight, &pBottom);

    Int32 left, top, right, bottom;
    bounds->Get(&left, &top, &right, &bottom);
    bounds->Set(left - pLeft, top - pTop, right + pRight, bottom + pBottom);

    AutoPtr<IAnimator> resizePreview = AnimateBounds(preview, bounds);
    resizePreview->SetDuration(DURATION_RESIZE);

    mPreviewAnimation = NULL;
    CAnimatorSet::New((IAnimatorSet**)&mPreviewAnimation);
    AutoPtr<IAnimatorSetBuilder> builder;
    mPreviewAnimation->Play(hideShowing, (IAnimatorSetBuilder**)&builder);
    builder->With(showTarget);
    builder->With(resizePreview);

    // The current preview size is unaffected by hidden or showing. It's
    // used to set starting scales for things that need to be scaled down.
    Int32 pWidth;
    preview->GetWidth(&pWidth);
    preview->GetPaddingLeft(&pLeft);
    preview->GetPaddingRight(&pRight);
    Int32 previewWidth = pWidth - pLeft - pRight;

    // If target is too large, shrink it immediately to fit and expand to
    // target size. Otherwise, start at target size.
    Int32 targetWidth;
    IView::Probe(target)->GetWidth(&targetWidth);
    if (targetWidth > previewWidth) {
        IView::Probe(target)->SetScaleX((Float) previewWidth / targetWidth);
        AutoPtr<IAnimator> scaleAnim = AnimateScaleX(IView::Probe(target), 1.f);
        scaleAnim->SetDuration(DURATION_RESIZE);
        builder->With(scaleAnim);
    } else {
        IView::Probe(target)->SetScaleX(1.f);
    }

    // If showing is larger than target, shrink to target size.
    Int32 showingWidth;
    IView::Probe(showing)->GetWidth(&showingWidth);
    if (showingWidth > targetWidth) {
        Float scale = (Float) targetWidth / showingWidth;
        AutoPtr<IAnimator> scaleAnim = AnimateScaleX(IView::Probe(showing), scale);
        scaleAnim->SetDuration(DURATION_RESIZE);
        builder->With(scaleAnim);
    }

    IAnimator::Probe(mPreviewAnimation)->Start();

    return !TextUtils::IsEmpty(text);
}

/**
 * Positions the thumb and preview widgets.
 *
 * @param position The position, between 0 and 1, along the track at which
 *            to place the thumb.
 */
void FastScroller::SetThumbPos(
    /* [in] */ Float position)
{
    AutoPtr<IRect> container = mContainerRect;
    Int32 top, bottom;
    container->GetTop(&top);
    container->GetBottom(&bottom);

    AutoPtr<IView> trackImage = IView::Probe(mTrackImage);
    AutoPtr<IView> thumbImage = IView::Probe(mThumbImage);
    Int32 min, max;
    trackImage->GetTop(&min);
    trackImage->GetBottom(&max);
    Float offset = min;
    Float range = max - min;
    Float thumbMiddle = position * range + offset;
    Int32 tHeight;
    thumbImage->GetHeight(&tHeight);
    thumbImage->SetTranslationY(thumbMiddle - tHeight / 2);

    AutoPtr<IView> previewImage = mPreviewImage;
    Int32 pHeight;
    previewImage->GetHeight(&pHeight);
    Float previewHalfHeight = pHeight / 2.f;
    Float previewPos;
    switch (mOverlayPosition) {
        case OVERLAY_AT_THUMB:
            previewPos = thumbMiddle;
            break;
        case OVERLAY_ABOVE_THUMB:
            previewPos = thumbMiddle - previewHalfHeight;
            break;
        case OVERLAY_FLOATING:
        default:
            previewPos = 0;
            break;
    }

    // Center the preview on the thumb, constrained to the list bounds.
    Float minP = top + previewHalfHeight;
    Float maxP = bottom - previewHalfHeight;
    Float previewMiddle = MathUtils::Constrain(previewPos, minP, maxP);
    Float previewTop = previewMiddle - previewHalfHeight;
    previewImage->SetTranslationY(previewTop);

    IView::Probe(mPrimaryText)->SetTranslationY(previewTop);
    IView::Probe(mSecondaryText)->SetTranslationY(previewTop);
}

Float FastScroller::GetPosFromMotionEvent(
    /* [in] */ Float y)
{
    AutoPtr<IView> trackImage = IView::Probe(mTrackImage);
    Int32 min, max;
    trackImage->GetTop(&min);
    trackImage->GetBottom(&max);
    Float offset = min;
    Float range = max - min;

    // If the list is the same height as the thumbnail or shorter,
    // effectively disable scrolling.
    if (range <= 0) {
        return 0.f;
    }

    return MathUtils::Constrain((y - offset) / range, 0.f, 1.f);
}

Float FastScroller::GetPosFromItemCount(
    /* [in] */ Int32 firstVisibleItem,
    /* [in] */ Int32 visibleItemCount,
    /* [in] */ Int32 totalItemCount)
{
    if (mSectionIndexer == NULL || mListAdapter == NULL) {
        GetSectionsFromIndexer();
    }

    Boolean hasSections = mSectionIndexer != NULL && mSections != NULL
            && mSections->GetLength() > 0;
    if (!hasSections || !mMatchDragPosition) {
        return (Float) firstVisibleItem / (totalItemCount - visibleItemCount);
    }
    // Ignore headers.
    firstVisibleItem -= mHeaderCount;
    if (firstVisibleItem < 0) {
        return 0;
    }
    totalItemCount -= mHeaderCount;

    // Hidden portion of the first visible row.
    AutoPtr<IView> child;
    IViewGroup::Probe(mList)->GetChildAt(0, (IView**)&child);
    Float incrementalPos;
    Int32 cHeight;
    child->GetHeight(&cHeight);
    if (child == NULL || cHeight == 0) {
        incrementalPos = 0;
    } else {
        Int32 lpTop, cTop, cHeight;
        IView::Probe(mList)->GetPaddingTop(&lpTop);
        child->GetTop(&cTop);
        child->GetHeight(&cHeight);
        incrementalPos = (Float) (lpTop - cTop) / cHeight;
    }

    // Number of rows in this section.
    Int32 section;
    mSectionIndexer->GetSectionForPosition(firstVisibleItem, &section);
    Int32 sectionPos;
    mSectionIndexer->GetPositionForSection(section, &sectionPos);
    Int32 sectionCount = mSections->GetLength();
    Int32 positionsInSection;
    if (section < sectionCount - 1) {
        Int32 nextSectionPos;
        if (section + 1 < sectionCount) {
            mSectionIndexer->GetPositionForSection(section + 1, &nextSectionPos);
        } else {
            nextSectionPos = totalItemCount - 1;
        }
        positionsInSection = nextSectionPos - sectionPos;
    } else {
        positionsInSection = totalItemCount - sectionPos;
    }

    // Position within this section.
    Float posWithinSection;
    if (positionsInSection == 0) {
        posWithinSection = 0;
    } else {
        posWithinSection = (firstVisibleItem + incrementalPos - sectionPos)
                / positionsInSection;
    }

    Float result = (section + posWithinSection) / sectionCount;

    // Fake out the scroll bar for the last item. Since the section indexer
    // won't ever actually move the list in this end space, make scrolling
    // across the last item account for whatever space is remaining.
    if (firstVisibleItem > 0 && firstVisibleItem + visibleItemCount == totalItemCount) {
        AutoPtr<IView> lastChild;
        IViewGroup::Probe(mList)->GetChildAt(visibleItemCount - 1, (IView**)&lastChild);
        Int32 bottomPadding;
        IView::Probe(mList)->GetPaddingBottom(&bottomPadding);
        Int32 maxSize;
        Int32 currentVisibleSize;
        Boolean clipToPadding;
        IViewGroup::Probe(mList)->GetClipToPadding(&clipToPadding);
        if (clipToPadding) {
            lastChild->GetHeight(&maxSize);
            Int32 lHeight, lastTop;
            IView::Probe(mList)->GetHeight(&lHeight);
            lastChild->GetTop(&lastTop);
            currentVisibleSize = lHeight - bottomPadding - lastTop;
        } else {
            lastChild->GetHeight(&maxSize);
            maxSize += bottomPadding;
            Int32 lHeight, lastTop;
            IView::Probe(mList)->GetHeight(&lHeight);
            lastChild->GetTop(&lastTop);
            currentVisibleSize = lHeight - lastTop;
        }
        if (currentVisibleSize > 0 && maxSize > 0) {
            result += (1 - result) * ((Float) currentVisibleSize / maxSize );
        }
    }

    return result;
}

/**
 * Cancels an ongoing fling event by injecting a
 * {@link MotionEvent#ACTION_CANCEL} into the host view.
 */
void FastScroller::CancelFling()
{
    AutoPtr<IMotionEvent> cancelFling;
    MotionEvent::Obtain(
            0, 0, IMotionEvent::ACTION_CANCEL, 0, 0, 0, (IMotionEvent**)&cancelFling);
    Boolean rst;
    IView::Probe(mList)->OnTouchEvent(cancelFling, &rst);
    IInputEvent::Probe(cancelFling)->Recycle();
}

/**
 * Cancels a pending drag.
 *
 * @see #startPendingDrag()
 */
void FastScroller::CancelPendingDrag()
{
    mPendingDrag = -1;
}

/**
 * Delays dragging until after the framework has determined that the user is
 * scrolling, rather than tapping.
 */
void FastScroller::StartPendingDrag()
{
    mPendingDrag = SystemClock::GetUptimeMillis() + TAP_TIMEOUT;
}

void FastScroller::BeginDrag()
{
    mPendingDrag = -1;

    SetState(STATE_DRAGGING);

    if (mListAdapter == NULL && mList != NULL) {
        GetSectionsFromIndexer();
    }

    if (mList != NULL) {
       IViewParent::Probe(mList)->RequestDisallowInterceptTouchEvent(TRUE);
       ((AbsListView*)mList)->ReportScrollStateChange(IAbsListViewOnScrollListener::SCROLL_STATE_TOUCH_SCROLL);
    }

    CancelFling();
}

/**
 * Returns whether a coordinate is inside the scroller's activation area. If
 * there is a track image, touching anywhere within the thumb-width of the
 * track activates scrolling. Otherwise, the user has to touch inside thumb
 * itself.
 *
 * @param x The x-coordinate.
 * @param y The y-coordinate.
 * @return Whether the coordinate is inside the scroller's activation area.
 */
Boolean FastScroller::IsPointInside(
    /* [in] */ Float x,
    /* [in] */ Float y)
{

    return IsPointInsideX(x) && (mTrackDrawable != NULL || IsPointInsideY(y));
}

Boolean FastScroller::IsPointInsideX(
    /* [in] */ Float x)
{
    if (mLayoutFromRight) {
        Int32 tLeft;
        IView::Probe(mThumbImage)->GetLeft(&tLeft);
        return x >= tLeft;
    } else {
        Int32 tRight;
        IView::Probe(mThumbImage)->GetRight(&tRight);
        return x <= tRight;
    }
}

Boolean FastScroller::IsPointInsideY(
    /* [in] */ Float y)
{
    Float offset;
    IView::Probe(mThumbImage)->GetTranslationY(&offset);
    Int32 tTop, tBottom;
    IView::Probe(mThumbImage)->GetTop(&tTop);
    Float top = tTop + offset;
    IView::Probe(mThumbImage)->GetBottom(&tBottom);
    Float bottom = tBottom + offset;
    return y >= top && y <= bottom;
}

AutoPtr<IAnimator> FastScroller::GroupAnimatorOfFloat(
    /* [in] */ IProperty* property,
    /* [in] */ Float value,
    /* [in] */ ArrayOf<IView*>* views)
{
    AutoPtr<IAnimatorSet> animSet;
    CAnimatorSet::New((IAnimatorSet**)&animSet);
    AutoPtr<IAnimatorSetBuilder> builder;

    for (Int32 i = views->GetLength() - 1; i >= 0; i--) {
        AutoPtr<ArrayOf<Float> > param = ArrayOf<Float>::Alloc(1);
        param->Set(0, value);
        AutoPtr<IAnimator> anim = IAnimator::Probe(ObjectAnimator::OfFloat((*views)[i], property, param));
        if (builder == NULL) {
            animSet->Play(anim, (IAnimatorSetBuilder**)&builder);
        } else {
            builder->With(anim);
        }
    }

    AutoPtr<IAnimator> result = IAnimator::Probe(animSet);
    return result;
}

/**
 * Returns an animator for the view's scaleX value.
 */
AutoPtr<IAnimator> FastScroller::AnimateScaleX(
    /* [in] */ IView* v,
    /* [in] */ Float target)
{
    AutoPtr<ArrayOf<Float> > param = ArrayOf<Float>::Alloc(1);
    param->Set(0, target);
    return IAnimator::Probe(ObjectAnimator::OfFloat(v, Elastos::Droid::View::View::SCALE_X, param));
}

/**
 * Returns an animator for the view's alpha value.
 */
AutoPtr<IAnimator> FastScroller::AnimateAlpha(
    /* [in] */ IView* v,
    /* [in] */ Float alpha)
{
    AutoPtr<ArrayOf<Float> > param = ArrayOf<Float>::Alloc(1);
    param->Set(0, alpha);
    return IAnimator::Probe(ObjectAnimator::OfFloat(v, Elastos::Droid::View::View::ALPHA, param));
}

AutoPtr<IAnimator> FastScroller::AnimateBounds(
    /* [in] */ IView* v,
    /* [in] */ IRect* bounds)
{
    Int32 l, t, r, b;
    bounds->GetLeft(&l);
    bounds->GetTop(&t);
    bounds->GetRight(&r);
    bounds->GetBottom(&b);

    AutoPtr<ArrayOf<Int32> > param1 = ArrayOf<Int32>::Alloc(1);
    param1->Set(0, l);
    AutoPtr<IPropertyValuesHolder> left = PropertyValuesHolder::OfInt32(LEFT, param1);

    AutoPtr<ArrayOf<Int32> > param2 = ArrayOf<Int32>::Alloc(1);
    param2->Set(0, t);
    AutoPtr<IPropertyValuesHolder> top = PropertyValuesHolder::OfInt32(TOP, param2);

    AutoPtr<ArrayOf<Int32> > param3 = ArrayOf<Int32>::Alloc(1);
    param3->Set(0, r);
    AutoPtr<IPropertyValuesHolder> right = PropertyValuesHolder::OfInt32(RIGHT, param3);

    AutoPtr<ArrayOf<Int32> > param4 = ArrayOf<Int32>::Alloc(1);
    param4->Set(0, b);
    AutoPtr<IPropertyValuesHolder> bottom = PropertyValuesHolder::OfInt32(BOTTOM, param4);

    AutoPtr<ArrayOf<IPropertyValuesHolder*> > param = ArrayOf<IPropertyValuesHolder*>::Alloc(4);
    param->Set(0, left);
    param->Set(1, top);
    param->Set(2, right);
    param->Set(3, bottom);
    return IAnimator::Probe(ObjectAnimator::OfPropertyValuesHolder(v, param));
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
