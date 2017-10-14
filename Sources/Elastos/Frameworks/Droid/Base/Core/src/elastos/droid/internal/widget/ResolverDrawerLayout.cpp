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

#include "elastos/droid/internal/widget/ResolverDrawerLayout.h"
#include "elastos/droid/internal/widget/CResolverDrawerLayoutSavedState.h"
#include "elastos/droid/internal/widget/CResolverDrawerLayoutParams.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/view/CVelocityTrackerHelper.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/view/animation/CAnimationUtils.h"
#include "elastos/droid/widget/COverScroller.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::R;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IVelocityTrackerHelper;
using Elastos::Droid::View::CVelocityTrackerHelper;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::IAbsSavedState;
using Elastos::Droid::View::IViewBaseSavedState;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::View::EIID_IOnTouchModeChangeListener;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::Widget::COverScroller;
using Elastos::Droid::Widget::IAbsListView;

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

//===============================================================
// ResolverDrawerLayout::ResolverDrawerLayoutParams::
//===============================================================
CAR_INTERFACE_IMPL(ResolverDrawerLayout::ResolverDrawerLayoutParams,
    MarginLayoutParams, IResolverDrawerLayoutParams)

ResolverDrawerLayout::ResolverDrawerLayoutParams::ResolverDrawerLayoutParams()
    : mAlwaysShow(FALSE)
    , mIgnoreOffset(FALSE)
{}

ECode ResolverDrawerLayout::ResolverDrawerLayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    ViewGroup::MarginLayoutParams::constructor(c, attrs);
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::ResolverDrawerLayout_LayoutParams);
    AutoPtr<ITypedArray> a;
    c->ObtainStyledAttributes(attrs,
            attrIds, (ITypedArray**)&a);
    a->GetBoolean(
            R::styleable::ResolverDrawerLayout_LayoutParams_layout_alwaysShow,
            FALSE, &mAlwaysShow);
    a->GetBoolean(
            R::styleable::ResolverDrawerLayout_LayoutParams_layout_ignoreOffset,
            FALSE, &mIgnoreOffset);
    a->Recycle();
    return NOERROR;
}

ECode ResolverDrawerLayout::ResolverDrawerLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return ViewGroup::MarginLayoutParams::constructor(width, height);
}

ECode ResolverDrawerLayout::ResolverDrawerLayoutParams::constructor(
    /* [in] */ IResolverDrawerLayoutParams* source)
{
    ViewGroup::MarginLayoutParams::constructor(IViewGroupLayoutParams::Probe(source));
    AutoPtr<ResolverDrawerLayoutParams> cs = (ResolverDrawerLayoutParams*)source;
    mAlwaysShow = cs->mAlwaysShow;
    mIgnoreOffset = cs->mIgnoreOffset;
    return NOERROR;
}

ECode ResolverDrawerLayout::ResolverDrawerLayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* source)
{
    return ViewGroup::MarginLayoutParams::constructor(IViewGroupLayoutParams::Probe(source));
}

ECode ResolverDrawerLayout::ResolverDrawerLayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    return ViewGroup::MarginLayoutParams::constructor(source);
}

ECode ResolverDrawerLayout::ResolverDrawerLayoutParams::SetAlwaysShow(
    /* [in] */ Boolean alwaysShow)
{
    mAlwaysShow = alwaysShow;
    return NOERROR;
}

ECode ResolverDrawerLayout::ResolverDrawerLayoutParams::GetAlwaysShow(
    /* [out] */ Boolean* alwaysShow)
{
    VALIDATE_NOT_NULL(alwaysShow)
    *alwaysShow = mAlwaysShow;
    return NOERROR;
}

ECode ResolverDrawerLayout::ResolverDrawerLayoutParams::SetIgnoreOffset(
    /* [in] */ Boolean ignoreOffset)
{
    mIgnoreOffset = ignoreOffset;
    return NOERROR;
}

ECode ResolverDrawerLayout::ResolverDrawerLayoutParams::GetIgnoreOffset(
    /* [out] */ Boolean* ignoreOffset)
{
    VALIDATE_NOT_NULL(ignoreOffset)
    *ignoreOffset = mIgnoreOffset;
    return NOERROR;
}

//===============================================================
// ResolverDrawerLayout::ResolverDrawerLayoutSavedState
//===============================================================
ResolverDrawerLayout::ResolverDrawerLayoutSavedState::ResolverDrawerLayoutSavedState()
    : mOpen(FALSE)
{}

ECode ResolverDrawerLayout::ResolverDrawerLayoutSavedState::constructor()
{
    return BaseSavedState::constructor();
}

ECode ResolverDrawerLayout::ResolverDrawerLayoutSavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return BaseSavedState::constructor(superState);
}

ECode ResolverDrawerLayout::ResolverDrawerLayoutSavedState::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    BaseSavedState::ReadFromParcel(in);
    Int32 i = 0;
    in->ReadInt32(&i);
    mOpen = i != 0;
    return NOERROR;
}

ECode ResolverDrawerLayout::ResolverDrawerLayoutSavedState::WriteToParcel(
    /* [in] */ IParcel* out)
{
    BaseSavedState::WriteToParcel(out);
    out->WriteInt32(mOpen ? 1 : 0);
    return NOERROR;
}

//===============================================================
// ResolverDrawerLayout::ResolverDrawerLayoutOnTouchModeChangeListener::
//===============================================================
CAR_INTERFACE_IMPL(ResolverDrawerLayout::ResolverDrawerLayoutOnTouchModeChangeListener, Object, IOnTouchModeChangeListener)

ResolverDrawerLayout::ResolverDrawerLayoutOnTouchModeChangeListener::ResolverDrawerLayoutOnTouchModeChangeListener(
    /* [in] */ ResolverDrawerLayout* host)
    : mHost(host)
{}

ECode ResolverDrawerLayout::ResolverDrawerLayoutOnTouchModeChangeListener::OnTouchModeChanged(
    /* [in] */ Boolean isInTouchMode)
{
    AutoPtr<IView> v;
    mHost->GetFocusedChild((IView**)&v);
    Boolean bF = FALSE;
    if (!isInTouchMode && (mHost->HasFocus(&bF), bF) && mHost->IsDescendantClipped(v)) {
        mHost->SmoothScrollTo(0, 0);
    }
    return NOERROR;
}

//===============================================================
// ResolverDrawerLayout::
//===============================================================
const String ResolverDrawerLayout::TAG("ResolverDrawerLayout");

CAR_INTERFACE_IMPL(ResolverDrawerLayout, ViewGroup, IResolverDrawerLayout)

ResolverDrawerLayout::ResolverDrawerLayout()
    : mMaxWidth(0)
    , mMaxCollapsedHeight(0)
    , mMaxCollapsedHeightSmall(0)
    , mSmallCollapsed(FALSE)
    , mCollapseOffset(0.0f)
    , mCollapsibleHeight(0)
    , mTopOffset(0)
    , mIsDragging(FALSE)
    , mOpenOnClick(FALSE)
    , mOpenOnLayout(FALSE)
    , mTouchSlop(0)
    , mMinFlingVelocity(0.0f)
    , mInitialTouchX(0.0f)
    , mInitialTouchY(0.0f)
    , mLastTouchY(0.0f)
    , mActivePointerId(IMotionEvent::INVALID_POINTER_ID)
{
    CRect::New((IRect**)&mTempRect);
    mTouchModeChangeListener = new ResolverDrawerLayoutOnTouchModeChangeListener(this);
}

ECode ResolverDrawerLayout::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode ResolverDrawerLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode ResolverDrawerLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    ViewGroup::constructor(context, attrs, defStyleAttr);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::ResolverDrawerLayout);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds,
            defStyleAttr, 0, (ITypedArray**)&a);
    a->GetDimensionPixelSize(R::styleable::ResolverDrawerLayout_maxWidth, -1, &mMaxWidth);
    a->GetDimensionPixelSize(
            R::styleable::ResolverDrawerLayout_maxCollapsedHeight, 0, &mMaxCollapsedHeight);
    a->GetDimensionPixelSize(
            R::styleable::ResolverDrawerLayout_maxCollapsedHeightSmall,
            mMaxCollapsedHeight, &mMaxCollapsedHeightSmall);
    a->Recycle();

    AutoPtr<IAnimationUtils> au;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&au);
    AutoPtr<IInterpolator> p;
    au->LoadInterpolator(context,
            R::interpolator::decelerate_quint, (IInterpolator**)&p);
    COverScroller::New(context, p, (IOverScroller**)&mScroller);
    AutoPtr<IVelocityTrackerHelper> hlp;
    CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&hlp);
    hlp->Obtain((IVelocityTracker**)&mVelocityTracker);

    AutoPtr<IViewConfigurationHelper> chlp;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&chlp);
    AutoPtr<IViewConfiguration> vc;
    chlp->Get(context, (IViewConfiguration**)&vc);
    vc->GetScaledTouchSlop(&mTouchSlop);
    Int32 value;
    vc->GetScaledMinimumFlingVelocity(&value);
    mMinFlingVelocity = (Float)value;
    return NOERROR;
}

ECode ResolverDrawerLayout::SetSmallCollapsed(
    /* [in] */ Boolean smallCollapsed)
{
    mSmallCollapsed = smallCollapsed;
    RequestLayout();
    return NOERROR;
}

ECode ResolverDrawerLayout::IsSmallCollapsed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSmallCollapsed;
    return NOERROR;
}

ECode ResolverDrawerLayout::IsCollapsed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCollapseOffset > 0;
    return NOERROR;
}

Boolean ResolverDrawerLayout::IsMoving()
{
    Boolean bF = FALSE;
    return mIsDragging || !(mScroller->IsFinished(&bF), bF);
}

Int32 ResolverDrawerLayout::GetMaxCollapsedHeight()
{
    Boolean bS = FALSE;
    return (IsSmallCollapsed(&bS), bS) ? mMaxCollapsedHeightSmall : mMaxCollapsedHeight;
}

ECode ResolverDrawerLayout::SetOnClickOutsideListener(
    /* [in] */ IViewOnClickListener* listener)
{
    mClickOutsideListener = listener;
    return NOERROR;
}

ECode ResolverDrawerLayout::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 action = 0;
    ev->GetActionMasked(&action);

    if (action == IMotionEvent::ACTION_DOWN) {
        mVelocityTracker->Clear();
    }

    mVelocityTracker->AddMovement(ev);

    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            Float x = 0.0f;
            ev->GetX(&x);
            Float y = 0.0f;
            ev->GetY(&y);
            mInitialTouchX = x;
            mInitialTouchY = mLastTouchY = y;
            mOpenOnClick = IsListChildUnderClipped(x, y) && mCollapsibleHeight > 0;
        }
        break;

        case IMotionEvent::ACTION_MOVE: {
            Float x = 0.0f;
            ev->GetX(&x);
            Float y = 0.0f;
            ev->GetY(&y);
            Float dy = y - mInitialTouchY;
            Int32 sa = 0;
            if (Elastos::Core::Math::Abs(dy) > mTouchSlop && FindChildUnder(x, y) != NULL &&
                    ((GetNestedScrollAxes(&sa), sa) & SCROLL_AXIS_VERTICAL) == 0) {
                ev->GetPointerId(0, &mActivePointerId);
                mIsDragging = TRUE;
                mLastTouchY = Elastos::Core::Math::Max(mLastTouchY - mTouchSlop,
                        Elastos::Core::Math::Min(mLastTouchY + dy, mLastTouchY + mTouchSlop));
            }
        }
        break;

        case IMotionEvent::ACTION_POINTER_UP: {
            OnSecondaryPointerUp(ev);
        }
        break;

        case IMotionEvent::ACTION_CANCEL:
        case IMotionEvent::ACTION_UP: {
            ResetTouch();
        }
        break;
    }

    if (mIsDragging) {
        mScroller->AbortAnimation();
    }
    *result = mIsDragging || mOpenOnClick;
    return NOERROR;
}

ECode ResolverDrawerLayout::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 action = 0;
    ev->GetActionMasked(&action);

    mVelocityTracker->AddMovement(ev);

    Boolean handled = FALSE;
    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            Float x = 0.0f;
            ev->GetX(&x);
            Float y = 0.0f;
            ev->GetY(&y);
            mInitialTouchX = x;
            mInitialTouchY = mLastTouchY = y;
            ev->GetPointerId(0, &mActivePointerId);
            if (FindChildUnder(mInitialTouchX, mInitialTouchY) == NULL &&
                    mClickOutsideListener != NULL) {
                mIsDragging = handled = TRUE;
            }
            handled |= mCollapsibleHeight > 0;
            mScroller->AbortAnimation();
        }
        break;

        case IMotionEvent::ACTION_MOVE: {
            Int32 index = 0;
            ev->FindPointerIndex(mActivePointerId, &index);
            if (index < 0) {
                Logger::E(TAG, "Bad pointer id %d, resetting", mActivePointerId);
                index = 0;
                ev->GetPointerId(0, &mActivePointerId);
                ev->GetX(&mInitialTouchX);
                ev->GetY(&mLastTouchY);
                mInitialTouchY = mLastTouchY;
            }
            Float x = 0.0f;
            ev->GetX(index, &x);
            Float y = 0.0f;
            ev->GetY(index, &y);
            if (!mIsDragging) {
                Float dy = y - mInitialTouchY;
                if (Elastos::Core::Math::Abs(dy) > mTouchSlop && FindChildUnder(x, y) != NULL) {
                    handled = mIsDragging = TRUE;
                    mLastTouchY = Elastos::Core::Math::Max(mLastTouchY - mTouchSlop,
                            Elastos::Core::Math::Min(mLastTouchY + dy, mLastTouchY + mTouchSlop));
                }
            }
            if (mIsDragging) {
                Float dy = y - mLastTouchY;
                PerformDrag(dy);
            }
            mLastTouchY = y;
        }
        break;

        case IMotionEvent::ACTION_POINTER_DOWN: {
            Int32 pointerIndex = 0;
            ev->GetActionIndex(&pointerIndex);
            Int32 pointerId = 0;
            ev->GetPointerId(pointerIndex, &pointerId);
            mActivePointerId = pointerId;
            ev->GetX(pointerIndex, &mInitialTouchX);
            ev->GetY(pointerIndex, &mLastTouchY);
            mInitialTouchY = mLastTouchY;
        }
        break;

        case IMotionEvent::ACTION_POINTER_UP: {
            OnSecondaryPointerUp(ev);
        }
        break;

        case IMotionEvent::ACTION_UP: {
            mIsDragging = FALSE;
            Float x = 0.0f, y = 0.0f;
            ev->GetX(&x);
            ev->GetY(&y);
            if (!mIsDragging && FindChildUnder(mInitialTouchX, mInitialTouchY) == NULL &&
                    FindChildUnder(x, y) == NULL) {
                if (mClickOutsideListener != NULL) {
                    mClickOutsideListener->OnClick(this);
                    ResetTouch();
                    *result = TRUE;
                    return NOERROR;
                }
            }
            if (mOpenOnClick && Elastos::Core::Math::Abs(x - mInitialTouchX) < mTouchSlop &&
                    Elastos::Core::Math::Abs(y - mInitialTouchY) < mTouchSlop) {
                SmoothScrollTo(0, 0);
                *result = TRUE;
                return NOERROR;
            }
            mVelocityTracker->ComputeCurrentVelocity(1000);
            Float yvel = 0.0f;
            mVelocityTracker->GetYVelocity(mActivePointerId, &yvel);
            if (Elastos::Core::Math::Abs(yvel) > mMinFlingVelocity) {
                SmoothScrollTo(yvel < 0 ? 0 : mCollapsibleHeight, yvel);
            }
            else {
                SmoothScrollTo(
                        mCollapseOffset < mCollapsibleHeight / 2 ? 0 : mCollapsibleHeight, 0);
            }
            ResetTouch();
        }
        break;

        case IMotionEvent::ACTION_CANCEL: {
            if (mIsDragging) {
                SmoothScrollTo(
                        mCollapseOffset < mCollapsibleHeight / 2 ? 0 : mCollapsibleHeight, 0);
            }
            ResetTouch();
            *result = TRUE;
            return NOERROR;
        }
    }

    *result = handled;
    return NOERROR;
}

void ResolverDrawerLayout::OnSecondaryPointerUp(
    /* [in] */ IMotionEvent* ev)
{
    Int32 pointerIndex = 0;
    ev->GetActionIndex(&pointerIndex);
    Int32 pointerId = 0;
    ev->GetPointerId(pointerIndex, &pointerId);
    if (pointerId == mActivePointerId) {
        // This was our active pointer going up. Choose a new
        // active pointer and adjust accordingly.
        Int32 newPointerIndex = pointerIndex == 0 ? 1 : 0;
        ev->GetX(newPointerIndex, &mInitialTouchX);
        ev->GetY(newPointerIndex, &mLastTouchY);
        mInitialTouchY = mLastTouchY;
        ev->GetPointerId(newPointerIndex, &mActivePointerId);
    }
}

void ResolverDrawerLayout::ResetTouch()
{
    mActivePointerId = IMotionEvent::INVALID_POINTER_ID;
    mIsDragging = FALSE;
    mOpenOnClick = FALSE;
    mInitialTouchX = mInitialTouchY = mLastTouchY = 0;
    mVelocityTracker->Clear();
}

ECode ResolverDrawerLayout::ComputeScroll()
{
    ViewGroup::ComputeScroll();
    Boolean bF = FALSE;
    if (!(mScroller->IsFinished(&bF), bF)) {
        Boolean keepGoing = FALSE;
        mScroller->ComputeScrollOffset(&keepGoing);
        Int32 y = 0;
        mScroller->GetCurrY(&y);
        PerformDrag(y - mCollapseOffset);
        if (keepGoing) {
            PostInvalidateOnAnimation();
        }
    }
    return NOERROR;
}

Float ResolverDrawerLayout::PerformDrag(
    /* [in] */ Float dy)
{
    Float newPos = Elastos::Core::Math::Max(0.0f, Elastos::Core::Math::Min(mCollapseOffset + dy, (Float)mCollapsibleHeight));
    if (newPos != mCollapseOffset) {
        dy = newPos - mCollapseOffset;
        Int32 childCount = 0;
        GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            AutoPtr<IViewGroupLayoutParams> vlp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&vlp);
            AutoPtr<ResolverDrawerLayoutParams> lp =
                (ResolverDrawerLayoutParams*)(IResolverDrawerLayoutParams::Probe(vlp));
            if (!lp->mIgnoreOffset) {
                child->OffsetTopAndBottom((Int32) dy);
            }
        }
        mCollapseOffset = newPos;
        mTopOffset += dy;
        PostInvalidateOnAnimation();
        return dy;
    }
    return 0;
}

void ResolverDrawerLayout::SmoothScrollTo(
    /* [in] */ Int32 yOffset,
    /* [in] */ Float velocity)
{
    if (GetMaxCollapsedHeight() == 0) {
        return;
    }
    mScroller->AbortAnimation();
    Int32 sy = (Int32) mCollapseOffset;
    Int32 dy = yOffset - sy;
    if (dy == 0) {
        return;
    }

    Int32 height = 0;
    GetHeight(&height);
    Int32 halfHeight = height / 2;
    Float distanceRatio = Elastos::Core::Math::Min(1.0f, 1.0f * Elastos::Core::Math::Abs(dy) / height);
    Float distance = halfHeight + halfHeight *
            DistanceInfluenceForSnapDuration(distanceRatio);

    Int32 duration = 0;
    velocity = Elastos::Core::Math::Abs(velocity);
    if (velocity > 0) {
        duration = 4 * Elastos::Core::Math::Round(1000 * Elastos::Core::Math::Abs(distance / velocity));
    }
    else {
        Float pageDelta = (Float) Elastos::Core::Math::Abs(dy) / height;
        duration = (Int32) ((pageDelta + 1) * 100);
    }
    duration = Elastos::Core::Math::Min(duration, 300);

    mScroller->StartScroll(0, sy, 0, dy, duration);
    PostInvalidateOnAnimation();
}

Float ResolverDrawerLayout::DistanceInfluenceForSnapDuration(
    /* [in] */ Float f)
{
    f -= 0.5f; // center the values about 0.
    f *= 0.3f * Elastos::Core::Math::PI / 2.0f;
    return (Float) Elastos::Core::Math::Sin(f);
}

AutoPtr<IView> ResolverDrawerLayout::FindChildUnder(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return FindChildUnder(this, x, y);
}

AutoPtr<IView> ResolverDrawerLayout::FindChildUnder(
    /* [in] */ IViewGroup* parent,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Int32 childCount = 0;
    parent->GetChildCount(&childCount);
    for (Int32 i = childCount - 1; i >= 0; i--) {
        AutoPtr<IView> child;
        parent->GetChildAt(i, (IView**)&child);
        if (IsChildUnder(child, x, y)) {
            return child;
        }
    }
    return NULL;
}

AutoPtr<IView> ResolverDrawerLayout::FindListChildUnder(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    AutoPtr<IView> v = FindChildUnder(x, y);
    while (v != NULL) {
        Float vx = 0.0f;
        v->GetX(&vx);
        x -= vx;
        Float vy = 0.0f;
        v->GetY(&vy);
        y -= vy;
        if (IAbsListView::Probe(v) != NULL) {
            // One more after this.
            return FindChildUnder(IViewGroup::Probe(v), x, y);
        }
        v = IViewGroup::Probe(v) != NULL ? FindChildUnder(IViewGroup::Probe(v), x, y) : NULL;
    }
    return v;
}

Boolean ResolverDrawerLayout::IsListChildUnderClipped(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    AutoPtr<IView> listChild = FindListChildUnder(x, y);
    return listChild != NULL && IsDescendantClipped(listChild);
}

Boolean ResolverDrawerLayout::IsDescendantClipped(
    /* [in] */ IView* child)
{
    Int32 w = 0, h = 0;
    child->GetWidth(&w);
    child->GetHeight(&h);
    mTempRect->Set(0, 0, w, h);
    OffsetDescendantRectToMyCoords(child, mTempRect);
    AutoPtr<IView> directChild;
    AutoPtr<IViewParent> vp;
    child->GetParent((IViewParent**)&vp);
    if (vp == this) {
        directChild = child;
    }
    else {
        AutoPtr<IView> v = child;
        AutoPtr<IViewParent> p;
        child->GetParent((IViewParent**)&p);
        while (p != this) {
            v = IView::Probe(p);
            AutoPtr<IViewParent> tmp;
            v->GetParent((IViewParent**)&tmp);
            p = tmp;
        }
        directChild = v;
    }

    // ResolverDrawerLayout lays out vertically in child order;
    // the next view and forward is what to check against.
    Int32 height = 0, b = 0;
    GetHeight(&height);
    GetPaddingBottom(&b);
    Int32 clipEdge = height - b;
    Int32 childCount = 0;
    GetChildCount(&childCount);
    Int32 i = 0;
    IndexOfChild(directChild, &i);
    for (i = i + 1; i < childCount; i++) {
        AutoPtr<IView> nextChild;
        GetChildAt(i, (IView**)&nextChild);
        Int32 vis = 0;
        nextChild->GetVisibility(&vis);
        if (vis == GONE) {
            continue;
        }
        Int32 t = 0;
        nextChild->GetTop(&t);
        clipEdge = Elastos::Core::Math::Min(clipEdge, t);
    }
    AutoPtr<CRect> cr = (CRect*)mTempRect.Get();
    return cr->mBottom > clipEdge;
}

Boolean ResolverDrawerLayout::IsChildUnder(
    /* [in] */ IView* child,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Float left = 0.0f;
    child->GetX(&left);
    Float top = 0.0f;
    child->GetY(&top);
    Int32 w = 0, h = 0;
    child->GetWidth(&w);
    child->GetHeight(&h);
    Float right = left + w;
    Float bottom = top + h;
    return x >= left && y >= top && x < right && y < bottom;
}

ECode ResolverDrawerLayout::RequestChildFocus(
    /* [in] */ IView* child,
    /* [in] */ IView* focused)
{
    ViewGroup::RequestChildFocus(child, focused);
    Boolean bM = FALSE;
    if (!(IsInTouchMode(&bM), bM) && IsDescendantClipped(focused)) {
        SmoothScrollTo(0, 0);
    }
    return NOERROR;
}

ECode ResolverDrawerLayout::OnAttachedToWindow()
{
    ViewGroup::OnAttachedToWindow();
    AutoPtr<IViewTreeObserver> ob;
    GetViewTreeObserver((IViewTreeObserver**)&ob);
    ob->AddOnTouchModeChangeListener(mTouchModeChangeListener);
    return NOERROR;
}

ECode ResolverDrawerLayout::OnDetachedFromWindow()
{
    ViewGroup::OnDetachedFromWindow();
    AutoPtr<IViewTreeObserver> ob;
    GetViewTreeObserver((IViewTreeObserver**)&ob);
    ob->RemoveOnTouchModeChangeListener(mTouchModeChangeListener);
    return NOERROR;
}

ECode ResolverDrawerLayout::OnStartNestedScroll(
    /* [in] */ IView* child,
    /* [in] */ IView* target,
    /* [in] */ Int32 nestedScrollAxes,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (nestedScrollAxes & IView::SCROLL_AXIS_VERTICAL) != 0;
    return NOERROR;
}

ECode ResolverDrawerLayout::OnNestedScrollAccepted(
    /* [in] */ IView* child,
    /* [in] */ IView* target,
    /* [in] */ Int32 axes)
{
    ViewGroup::OnNestedScrollAccepted(child, target, axes);
    return NOERROR;
}

ECode ResolverDrawerLayout::OnStopNestedScroll(
    /* [in] */ IView* child)
{
    ViewGroup::OnStopNestedScroll(child);
    Boolean bF = FALSE;
    if ((mScroller->IsFinished(&bF), bF)) {
        SmoothScrollTo(mCollapseOffset < mCollapsibleHeight / 2 ? 0 : mCollapsibleHeight, 0);
    }
    return NOERROR;
}

ECode ResolverDrawerLayout::OnNestedScroll(
    /* [in] */ IView* target,
    /* [in] */ Int32 dxConsumed,
    /* [in] */ Int32 dyConsumed,
    /* [in] */ Int32 dxUnconsumed,
    /* [in] */ Int32 dyUnconsumed)
{
    if (dyUnconsumed < 0) {
        PerformDrag(-dyUnconsumed);
    }
    return NOERROR;
}

ECode ResolverDrawerLayout::OnNestedPreScroll(
    /* [in] */ IView* target,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy,
    /* [in] */ ArrayOf<Int32>* consumed)
{
    if (dy > 0) {
        (*consumed)[1] = (Int32) -PerformDrag(-dy);
    }
    return NOERROR;
}

ECode ResolverDrawerLayout::OnNestedPreFling(
    /* [in] */ IView* target,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (velocityY > mMinFlingVelocity && mCollapseOffset != 0) {
        SmoothScrollTo(0, velocityY);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode ResolverDrawerLayout::OnNestedFling(
    /* [in] */ IView* target,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [in] */ Boolean consumed,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (!consumed && Elastos::Core::Math::Abs(velocityY) > mMinFlingVelocity) {
        SmoothScrollTo(velocityY > 0 ? 0 : mCollapsibleHeight, velocityY);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode ResolverDrawerLayout::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 sourceWidth = MeasureSpec::GetSize(widthMeasureSpec);
    Int32 widthSize = sourceWidth;
    Int32 heightSize = MeasureSpec::GetSize(heightMeasureSpec);

    // Single-use layout; just ignore the mode and use available space.
    // Clamp to maxWidth.
    if (mMaxWidth >= 0) {
        widthSize = Elastos::Core::Math::Min(widthSize, mMaxWidth);
    }

    Int32 widthSpec = MeasureSpec::MakeMeasureSpec(widthSize, MeasureSpec::EXACTLY);
    Int32 heightSpec = MeasureSpec::MakeMeasureSpec(heightSize, MeasureSpec::EXACTLY);
    Int32 l = 0, r = 0, t = 0, b = 0;
    GetPaddingLeft(&l);
    GetPaddingRight(&r);
    GetPaddingTop(&t);
    GetPaddingBottom(&b);
    Int32 widthPadding = l + r;
    Int32 heightUsed = t + b;

    // Measure always-show children first.
    Int32 childCount = 0;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> vlp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&vlp);
        AutoPtr<ResolverDrawerLayoutParams> lp =
                (ResolverDrawerLayoutParams*)(IResolverDrawerLayoutParams::Probe(vlp));
        Int32 vis = 0;
        child->GetVisibility(&vis);
        if (lp->mAlwaysShow && vis != GONE) {
            MeasureChildWithMargins(child, widthSpec, widthPadding, heightSpec, heightUsed);
            Int32 h = 0;
            child->GetMeasuredHeight(&h);
            heightUsed += lp->mTopMargin + h + lp->mBottomMargin;
        }
    }

    Int32 alwaysShowHeight = heightUsed;

    // And now the rest.
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> vlp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&vlp);
        AutoPtr<ResolverDrawerLayoutParams> lp =
                (ResolverDrawerLayoutParams*)(IResolverDrawerLayoutParams::Probe(vlp));
        Int32 vis = 0;
        child->GetVisibility(&vis);
        if (!lp->mAlwaysShow && vis != GONE) {
            MeasureChildWithMargins(child, widthSpec, widthPadding, heightSpec, heightUsed);
            Int32 h = 0;
            child->GetMeasuredHeight(&h);
            heightUsed += lp->mTopMargin + h + lp->mBottomMargin;
        }
    }

    mCollapsibleHeight = Elastos::Core::Math::Max(0,
            heightUsed - alwaysShowHeight - GetMaxCollapsedHeight());

    Boolean bL = FALSE;
    if ((IsLaidOut(&bL), bL)) {
        mCollapseOffset = Elastos::Core::Math::Min(mCollapseOffset, (Float) mCollapsibleHeight);
    }
    else {
        // Start out collapsed at first unless we restored state for otherwise
        mCollapseOffset = mOpenOnLayout ? 0 : mCollapsibleHeight;
    }

    mTopOffset = Elastos::Core::Math::Max(0, heightSize - heightUsed) + (Int32) mCollapseOffset;

    SetMeasuredDimension(sourceWidth, heightSize);
    return NOERROR;
}

ECode ResolverDrawerLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Int32 width = 0;
    GetWidth(&width);

    Int32 ypos = mTopOffset;
    Int32 leftEdge = 0;
    GetPaddingLeft(&leftEdge);
    Int32 right = 0;
    GetPaddingRight(&right);
    Int32 rightEdge = width - right;

    Int32 childCount = 0;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> vlp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&vlp);
        AutoPtr<ResolverDrawerLayoutParams> lp =
                (ResolverDrawerLayoutParams*)(IResolverDrawerLayoutParams::Probe(vlp));

        Int32 vis = 0;
        child->GetVisibility(&vis);
        if (vis == GONE) {
            continue;
        }

        Int32 top = ypos + lp->mTopMargin;
        if (lp->mIgnoreOffset) {
            top -= mCollapseOffset;
        }
        Int32 h = 0;
        child->GetMeasuredHeight(&h);
        Int32 bottom = top + h;

        Int32 w = 0;
        child->GetMeasuredWidth(&w);
        Int32 childWidth = w;
        Int32 widthAvailable = rightEdge - leftEdge;
        Int32 left = leftEdge + (widthAvailable - childWidth) / 2;
        Int32 right = left + childWidth;

        child->Layout(left, top, right, bottom);

        ypos = bottom + lp->mBottomMargin;
    }
    return NOERROR;
}

ECode ResolverDrawerLayout::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    return CResolverDrawerLayoutParams::New(cxt, attrs, result);
}

AutoPtr<IViewGroupLayoutParams> ResolverDrawerLayout::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    AutoPtr<IViewGroupLayoutParams> result;
    CResolverDrawerLayoutParams::New(p, (IViewGroupLayoutParams**)&result);
    return result;
}

ECode ResolverDrawerLayout::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** result)
{
    return CResolverDrawerLayoutParams::New(
        IViewGroupLayoutParams::MATCH_PARENT, IViewGroupLayoutParams::WRAP_CONTENT, result);
}

AutoPtr<IParcelable> ResolverDrawerLayout::OnSaveInstanceState()
{
    AutoPtr<IParcelable> p = View::OnSaveInstanceState();
    AutoPtr<IViewBaseSavedState> ss;
    CResolverDrawerLayoutSavedState::New(p, (IViewBaseSavedState**)&ss);
    ((ResolverDrawerLayoutSavedState*)ss.Get())->mOpen = mCollapsibleHeight > 0 && mCollapseOffset == 0;
    AutoPtr<IParcelable> result = IParcelable::Probe(ss);
    return result;
}

ECode ResolverDrawerLayout::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    ResolverDrawerLayoutSavedState* ss = (ResolverDrawerLayoutSavedState*)state;
    AutoPtr<IParcelable> parcl;
    ss->GetSuperState((IParcelable**)&parcl);
    View::OnRestoreInstanceState(parcl);
    mOpenOnLayout = ss->mOpen;
    return NOERROR;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
