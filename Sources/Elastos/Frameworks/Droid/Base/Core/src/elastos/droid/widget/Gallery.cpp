
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/Gallery.h"
#include "elastos/droid/view/ViewConfiguration.h"
#include "elastos/droid/view/SoundEffectConstants.h"
#include "elastos/droid/view/CGestureDetector.h"
#include "elastos/droid/widget/CGalleryLayoutParams.h"
#include "elastos/droid/widget/CScroller.h"
#include <elastos/core/Math.h>

using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::SoundEffectConstants;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IHapticFeedbackConstants;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::ViewConfiguration;
using Elastos::Droid::View::CGestureDetector;
using Elastos::Droid::View::EIID_IGestureDetectorOnGestureListener;
using Elastos::Droid::Widget::CGalleryLayoutParams;
using Elastos::Droid::Widget::CScroller;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Widget {

//==============================================================================
//              Gallery::GalleryLayoutParams
//==============================================================================
CAR_INTERFACE_IMPL(Gallery::GalleryLayoutParams, ViewGroup::LayoutParams, IGalleryLayoutParams);

Gallery::GalleryLayoutParams::GalleryLayoutParams()
{}

Gallery::GalleryLayoutParams::~GalleryLayoutParams()
{}

ECode Gallery::GalleryLayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    return ViewGroup::LayoutParams::constructor(c, attrs);
}

ECode Gallery::GalleryLayoutParams::constructor(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    return ViewGroup::LayoutParams::constructor(w, h);
}

ECode Gallery::GalleryLayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    return ViewGroup::LayoutParams::constructor(source);
}

//==============================================================================
//              Gallery::FlingRunnable
//==============================================================================
Gallery::FlingRunnable::FlingRunnable(
    /* [in] */ Gallery* host)
    : mHost(host)
    , mLastFlingX(0)
{
    AutoPtr<IContext> ctx;
    mHost->GetContext((IContext**)&ctx);
    CScroller::New(ctx, (IScroller**)&mScroller);
}

void Gallery::FlingRunnable::StartCommon()
{
    Boolean result = FALSE;
    mHost->RemoveCallbacks(this, &result);
}

void Gallery::FlingRunnable::StartUsingVelocity(
    /* [in] */ Int32 initialVelocity)
{
    if (initialVelocity == 0) return;

    StartCommon();

    Int32 initialX = initialVelocity < 0 ? Elastos::Core::Math::INT32_MAX_VALUE : 0;
    mLastFlingX = initialX;
    mScroller->Fling(initialX, 0, initialVelocity, 0,
        0, Elastos::Core::Math::INT32_MAX_VALUE, 0, Elastos::Core::Math::INT32_MAX_VALUE);
    AutoPtr<IRunnable> r = this;
    Boolean result = FALSE;
    mHost->Post(r, &result);
}

void Gallery::FlingRunnable::StartUsingDistance(
    /* [in] */ Int32 distance)
{
    if (distance == 0) return;

    StartCommon();

    mLastFlingX = 0;
    mScroller->StartScroll(0, 0, -distance, 0, mHost->mAnimationDuration);
    AutoPtr<IRunnable> r = this;
    Boolean result = FALSE;
    mHost->Post(r, &result);
}

void Gallery::FlingRunnable::Stop(
    /* [in] */ Boolean scrollIntoSlots)
{
    Boolean result = FALSE;
    mHost->RemoveCallbacks(this, &result);
    EndFling(scrollIntoSlots);
}

void Gallery::FlingRunnable::EndFling(
    /* [in] */ Boolean scrollIntoSlots)
{
    mScroller->ForceFinished(TRUE);

    if (scrollIntoSlots) mHost->ScrollIntoSlots();
}

ECode Gallery::FlingRunnable::Run()
{
    if (mHost->mItemCount == 0) {
        EndFling(TRUE);
        return NOERROR;
    }

    mHost->mShouldStopFling = FALSE;

    AutoPtr<IScroller> scroller = mScroller;
    Boolean more = FALSE;
    scroller->ComputeScrollOffset(&more);
    Int32 x = 0;
    scroller->GetCurrX(&x);

    Int32 delta = mLastFlingX - x;

    if (delta > 0) {
        Int32 count = 0;
        mHost->GetChildCount(&count);
        mHost->mDownTouchPosition = mHost->mIsRtl ? (mHost->mFirstPosition + count - 1) :
            mHost->mFirstPosition;

        Int32 width = 0;
        mHost->GetWidth(&width);
        delta = Elastos::Core::Math::Min(width - mHost->mPaddingLeft - mHost->mPaddingRight - 1, delta);
    } else {
        Int32 count = 0;
        mHost->GetChildCount(&count);
        Int32 UNUSED(offsetToLast) = count - 1;
        mHost->mDownTouchPosition = mHost->mIsRtl ? mHost->mFirstPosition :
            (mHost->mFirstPosition + count - 1);

        Int32 width = 0;
        mHost->GetWidth(&width);
        delta = Elastos::Core::Math::Max(-(width - mHost->mPaddingRight - mHost->mPaddingLeft - 1), delta);
    }

    mHost->TrackMotionScroll(delta);

    if (more && !mHost->mShouldStopFling) {
        mLastFlingX = x;
        AutoPtr<IRunnable> r = this;
        Boolean result = FALSE;
        mHost->Post(r, &result);
    }
    else {
       EndFling(TRUE);
    }
    return NOERROR;
}

//==============================================================================
//              Gallery::DisableSuppressSelectionChangedRunnable
//==============================================================================
Gallery::DisableSuppressSelectionChangedRunnable::DisableSuppressSelectionChangedRunnable(
    /* [in] */ Gallery* host)
    : mHost(host)
{}

ECode Gallery::DisableSuppressSelectionChangedRunnable::Run()
{
    mHost->mSuppressSelectionChanged = FALSE;
    mHost->SelectionChanged();

    return NOERROR;
}

//==============================================================================
//              Gallery::KeyUpRunnable
//==============================================================================
Gallery::KeyUpRunnable::KeyUpRunnable(
    /* [in] */ Gallery* host)
    : mHost(host)
{}

ECode Gallery::KeyUpRunnable::Run()
{
    mHost->DispatchUnpress();

    return NOERROR;
}

//==============================================================================
//              Gallery::DetectorOnGestureListener
//==============================================================================
CAR_INTERFACE_IMPL(Gallery::DetectorOnGestureListener, Object, IGestureDetectorOnGestureListener);
Gallery::DetectorOnGestureListener::DetectorOnGestureListener(
    /* [in] */ Gallery* host)
    : mHost(host)
{}

ECode Gallery::DetectorOnGestureListener::OnDown(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    return mHost->OnDown(e, res);
}

ECode Gallery::DetectorOnGestureListener::OnShowPress(
    /* [in] */ IMotionEvent* e)
{
    return mHost->OnShowPress(e);
}

ECode Gallery::DetectorOnGestureListener::OnSingleTapUp(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    return mHost->OnSingleTapUp(e, res);
}

ECode Gallery::DetectorOnGestureListener::OnScroll(
    /* [in] */ IMotionEvent* e1,
    /* [in] */ IMotionEvent* e2,
    /* [in] */ Float distanceX,
    /* [in] */ Float distanceY,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    return mHost->OnScroll(e1, e2, distanceX, distanceY, res);
}

ECode Gallery::DetectorOnGestureListener::OnLongPress(
    /* [in] */ IMotionEvent* e)
{
    return mHost->OnLongPress(e);
}

ECode Gallery::DetectorOnGestureListener::OnFling(
    /* [in] */ IMotionEvent* e1,
    /* [in] */ IMotionEvent* e2,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    return mHost->OnFling(e1, e2, velocityX, velocityY, res);
}

//==============================================================================
//              Gallery
//==============================================================================
const Boolean Gallery::localLOGV = FALSE;
const Int32 Gallery::SCROLL_TO_FLING_UNCERTAINTY_TIMEOUT = 250;
const String Gallery::GALLERY_NAME("Gallery");

CAR_INTERFACE_IMPL_2(Gallery, View, IGallery, IGestureDetectorOnGestureListener);
Gallery::Gallery()
    : mSpacing(0)
    , mAnimationDuration(400)
    , mUnselectedAlpha(0.0)
    , mLeftMost(0)
    , mRightMost(0)
    , mGravity(0)
    , mDownTouchPosition(0)
    , mShouldStopFling(FALSE)
    , mShouldCallbackDuringFling(TRUE)
    , mShouldCallbackOnUnselectedItemClick(TRUE)
    , mSuppressSelectionChanged(FALSE)
    , mReceivedInvokeKeyDown(FALSE)
    , mIsFirstScroll(FALSE)
    , mIsRtl(TRUE)
    , mSelectedCenterOffset(0)
{
}

Gallery::~Gallery()
{
}

ECode Gallery::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode Gallery::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::galleryStyle);
}

ECode Gallery::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode Gallery::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ASSERT_SUCCEEDED(AbsSpinner::constructor(context, attrs, defStyleAttr, defStyleRes));

    AutoPtr<DetectorOnGestureListener> listener = new DetectorOnGestureListener(this);
    FAIL_RETURN(CGestureDetector::New(context,
        (IGestureDetectorOnGestureListener*)listener,
        (IGestureDetector**)&mGestureDetector));
    mGestureDetector->SetIsLongpressEnabled(TRUE);

    ASSERT_SUCCEEDED(InitFromAttributes(context, attrs, defStyleAttr, defStyleRes));
    mFlingRunnable = new FlingRunnable(this);
    mDisableSuppressSelectionChangedRunnable =
            new DisableSuppressSelectionChangedRunnable(this);

    // We draw the selected item last (because otherwise the item to the
    // right overlaps it)
    mGroupFlags |= FLAG_USE_CHILD_DRAWING_ORDER;
    mGroupFlags |= FLAG_SUPPORT_STATIC_TRANSFORMATIONS;
    return NOERROR;
}

ECode Gallery::InitFromAttributes(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::Gallery);
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(
        attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));

    Int32 index = 0;
    a->GetInt32(R::styleable::Gallery_gravity, -1, &index);
    if (index >= 0) {
        SetGravity(index);
    }

    Int32 animationDuration = 0;
    a->GetInt32(R::styleable::Gallery_animationDuration, -1, &animationDuration);
    if (animationDuration > 0) {
        SetAnimationDuration(animationDuration);
    }

    Int32 spacing = 0;
    a->GetDimensionPixelOffset(R::styleable::Gallery_spacing, 0, &spacing);
    SetSpacing(spacing);

    Float unselectedAlpha = 0;
    a->GetFloat(
           R::styleable::Gallery_unselectedAlpha, 0.5f, &unselectedAlpha);
    SetUnselectedAlpha(unselectedAlpha);

    a->Recycle();
    return NOERROR;
}

ECode Gallery::SetCallbackDuringFling(
    /* [in] */ Boolean shouldCallback)
{
    mShouldCallbackDuringFling = shouldCallback;

    return NOERROR;
}

ECode Gallery::SetCallbackOnUnselectedItemClick(
    /* [in] */ Boolean shouldCallback)
{
    mShouldCallbackOnUnselectedItemClick = shouldCallback;

    return NOERROR;
}

ECode Gallery::SetAnimationDuration(
    /* [in] */ Int32 animationDurationMillis)
{
    mAnimationDuration = animationDurationMillis;

    return NOERROR;
}

ECode Gallery::SetSpacing(
    /* [in] */ Int32 spacing)
{
    mSpacing = spacing;

    return NOERROR;
}

ECode Gallery::SetUnselectedAlpha(
    /* [in] */ Float unselectedAlpha)
{
    mUnselectedAlpha = unselectedAlpha;

    return NOERROR;
}

Boolean Gallery::GetChildStaticTransformation(
    /* [in] */ IView* child,
    /* [in] */ ITransformation* t)
{
    t->Clear();
    t->SetAlpha(child == mSelectedChild ? 1.0f : mUnselectedAlpha);

    return TRUE;
}

Int32 Gallery::ComputeHorizontalScrollExtent()
{
    return 1;
}

Int32 Gallery::ComputeHorizontalScrollOffset()
{
    return mSelectedPosition;
}

Int32 Gallery::ComputeHorizontalScrollRange()
{
    return mItemCount;
}

Boolean Gallery::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return IGalleryLayoutParams::Probe(p) != NULL;
}

AutoPtr<IViewGroupLayoutParams> Gallery::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    AutoPtr<IGalleryLayoutParams> lp;
    CGalleryLayoutParams::New(p, (IGalleryLayoutParams**)&lp);
    return IViewGroupLayoutParams::Probe(lp);
}

ECode Gallery::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    AutoPtr<IGalleryLayoutParams> lp;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    FAIL_RETURN(CGalleryLayoutParams::New(context, attrs, (IGalleryLayoutParams**)&lp));
    *params = IViewGroupLayoutParams::Probe(lp);
    REFCOUNT_ADD(*params);
    return NOERROR;
}

AutoPtr<IViewGroupLayoutParams> Gallery::GenerateDefaultLayoutParams()
{
    AutoPtr<IGalleryLayoutParams> lp;
    CGalleryLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::WRAP_CONTENT, (IGalleryLayoutParams**)&lp);
    return IViewGroupLayoutParams::Probe(lp);
}

ECode Gallery::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    AbsSpinner::OnLayout(changed, l, t, r, b);

    mInLayout = TRUE;
    Layout(0, FALSE);
    mInLayout = FALSE;
    return NOERROR;
}

Int32 Gallery::GetChildHeight(
    /* [in] */ IView* child)
{
    Int32 height = 0;
    child->GetMeasuredHeight(&height);

    return height;
}

void Gallery::TrackMotionScroll(
    /* [in] */ Int32 deltaX)
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) == 0) {
        return;
    }

    Boolean toLeft = deltaX < 0;

    Int32 limitedDeltaX = GetLimitedMotionScrollAmount(toLeft, deltaX);
    if (limitedDeltaX != deltaX) {
        mFlingRunnable->EndFling(FALSE);
        OnFinishedMovement();
    }

    OffsetChildrenLeftAndRight(limitedDeltaX);

    DetachOffScreenChildren(toLeft);

    if (toLeft) {
        FillToGalleryRight();
    }
    else {
        FillToGalleryLeft();
    }

    mRecycler->Clear();

    SetSelectionToCenterChild();

    AutoPtr<IView> selChild = mSelectedChild;
    if (selChild != NULL) {
        Int32 childLeft = 0;
        selChild->GetLeft(&childLeft);
        Int32 childCenter = 0;
        selChild->GetWidth(&childCenter);
        childCenter = childCenter / 2;
        Int32 galleryCenter = 0;
        selChild->GetWidth(&galleryCenter);
        galleryCenter = galleryCenter / 2;
        mSelectedCenterOffset = childLeft + childCenter - galleryCenter;
    }

    OnScrollChanged(0, 0, 0, 0);

    Invalidate();
}

Int32 Gallery::GetLimitedMotionScrollAmount(
    /* [in] */ Boolean motionToLeft,
    /* [in] */ Int32 deltaX)
{
    Int32 extremeItemPosition = motionToLeft != mIsRtl ? mItemCount - 1 : 0;
    AutoPtr<IView> extremeChild;
    GetChildAt(extremeItemPosition - mFirstPosition, (IView**)&extremeChild);

    if (extremeChild == NULL) {
        return deltaX;
    }

    Int32 extremeChildCenter = GetCenterOfView(extremeChild);
    Int32 galleryCenter = GetCenterOfGallery();

    if (motionToLeft) {
        if (extremeChildCenter <= galleryCenter) {
            return 0;
        }
    }
    else {
        if (extremeChildCenter >= galleryCenter) {
            return 0;
        }
    }

    Int32 centerDifference = galleryCenter - extremeChildCenter;

    return motionToLeft
        ? Elastos::Core::Math::Max(centerDifference, deltaX)
        : Elastos::Core::Math::Min(centerDifference, deltaX);
}

void Gallery::OffsetChildrenLeftAndRight(
    /* [in] */ Int32 offset)
{
    Int32 count = 0;
    GetChildCount(&count);
    for (Int32 i = count - 1; i >= 0; i--) {
        AutoPtr<IView> view;
        GetChildAt(i, (IView**)&view);
        view->OffsetLeftAndRight(offset);
    }
}

Int32 Gallery::GetCenterOfGallery()
{
    Int32 width = 0;
    GetWidth(&width);
    return (width - mPaddingLeft - mPaddingRight) / 2 + mPaddingLeft;
}

Int32 Gallery::GetCenterOfView(
    /* [in] */ IView* view)
{
    Int32 left, width;
    view->GetLeft(&left);
    view->GetWidth(&width);

    return left + width / 2;
}

void Gallery::DetachOffScreenChildren(
    /* [in] */ Boolean toLeft)
{
    Int32 numChildren = 0;
    GetChildCount(&numChildren);
    Int32 firstPosition = mFirstPosition;
    Int32 start = 0;
    Int32 count = 0;

    if (toLeft) {
        Int32 galleryLeft = mPaddingLeft;
        for (Int32 i = 0; i < numChildren; i++) {
            int n = mIsRtl ? (numChildren - 1 - i) : i;

            AutoPtr<IView> child;
            GetChildAt(n, (IView**)&child);
            Int32 right;
            child->GetRight(&right);
            if (right >= galleryLeft) {
                break;
            }
            else {
                start = n;
                count++;
                mRecycler->Put(firstPosition + n, child);
            }
        }
        if (!mIsRtl) {
            start = 0;
        }
    }
    else {
        Int32 w = 0;
        GetWidth(&w);
        Int32 galleryRight = w - mPaddingRight;
        for (Int32 i = numChildren - 1; i >= 0; i--) {
            int n = mIsRtl ? numChildren - 1 - i : i;
            AutoPtr<IView> child;
            GetChildAt(n, (IView**)&child);
            Int32 left;
            child->GetLeft(&left);
            if (left <= galleryRight) {
                break;
            }
            else {
                start = n;
                count++;
                mRecycler->Put(firstPosition + n, child);
            }
        }
        if (mIsRtl) {
            start = 0;
        }
    }

    DetachViewsFromParent(start, count);

    if (toLeft != mIsRtl) {
        mFirstPosition += count;
    }
}

void Gallery::ScrollIntoSlots()
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) == 0 || mSelectedChild == NULL) return;

    Int32 selectedCenter = GetCenterOfView(mSelectedChild);
    Int32 targetCenter = GetCenterOfGallery();

    Int32 scrollAmount = targetCenter - selectedCenter;
    if (scrollAmount != 0) {
        mFlingRunnable->StartUsingDistance(scrollAmount);
    }
    else {
        OnFinishedMovement();
    }
}

void Gallery::OnFinishedMovement()
{
    if (mSuppressSelectionChanged) {
        mSuppressSelectionChanged = FALSE;
        AbsSpinner::SelectionChanged();
    }
    mSelectedCenterOffset = 0;

    Invalidate();
}


void Gallery::SelectionChanged()
{
    if (!mSuppressSelectionChanged) {
        AbsSpinner::SelectionChanged();
    }
}

void Gallery::SetSelectionToCenterChild()
{
    AutoPtr<IView> selView = mSelectedChild;
    if (mSelectedChild == NULL) return;

    Int32 galleryCenter = GetCenterOfGallery();

    Int32 left, right;
    selView->GetLeft(&left);
    selView->GetRight(&right);
    if (left <= galleryCenter && right >= galleryCenter) {
        return;
    }

    Int32 closestEdgeDistance = Elastos::Core::Math::INT32_MAX_VALUE;
    Int32 newSelectedChildIndex = 0;
    Int32 count = 0;
    GetChildCount(&count);
    for (Int32 i = count - 1; i >= 0; i--) {

        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);

        child->GetLeft(&left);
        child->GetRight(&right);
        if (left <= galleryCenter && right >= galleryCenter) {
            newSelectedChildIndex = i;
            break;
        }

        Int32 childClosestEdgeDistance = Elastos::Core::Math::Min(Elastos::Core::Math::Abs(left - galleryCenter),
            Elastos::Core::Math::Abs(right - galleryCenter));
        if (childClosestEdgeDistance < closestEdgeDistance) {
            closestEdgeDistance = childClosestEdgeDistance;
            newSelectedChildIndex = i;
        }
    }

    Int32 newPos = mFirstPosition + newSelectedChildIndex;

    if (newPos != mSelectedPosition) {
        SetSelectedPositionInt(newPos);
        SetNextSelectedPositionInt(newPos);
        CheckSelectionChanged();
    }
}

void Gallery::Layout(
    /* [in] */ Int32 delta,
    /* [in] */ Boolean animate)
{
    IsLayoutRtl(&mIsRtl);

    Int32 left, right;
    mSpinnerPadding->GetLeft(&left);
    mSpinnerPadding->GetRight(&right);
    Int32 childrenLeft = left;
    Int32 childrenWidth = mRight - mLeft - left - right;

    if (mDataChanged) {
        HandleDataChanged();
    }

    if (mItemCount == 0) {
        ResetList();
        return;
    }

    if (mNextSelectedPosition >= 0) {
        SetSelectedPositionInt(mNextSelectedPosition);
    }

    RecycleAllViews();

    DetachAllViewsFromParent();

    mRightMost = 0;
    mLeftMost = 0;


    mFirstPosition = mSelectedPosition;
    AutoPtr<IView> sel = MakeAndAddView(mSelectedPosition, 0, 0, TRUE);

    Int32 width = 0;
    sel->GetWidth(&width);
    Int32 selectedOffset = childrenLeft + (childrenWidth / 2) - (width / 2) +
        mSelectedCenterOffset;
    sel->OffsetLeftAndRight(selectedOffset);

    FillToGalleryRight();
    FillToGalleryLeft();

    mRecycler->Clear();

    Invalidate();
    CheckSelectionChanged();

    mDataChanged = FALSE;
    mNeedSync = FALSE;
    SetNextSelectedPositionInt(mSelectedPosition);

    UpdateSelectedItemMetadata();
}

void Gallery::FillToGalleryLeft()
{
    if (mIsRtl) {
        FillToGalleryLeftRtl();
    } else {
        FillToGalleryLeftLtr();
    }
}

void Gallery::FillToGalleryLeftRtl()
{
    Int32 itemSpacing = mSpacing;
    Int32 galleryLeft = mPaddingLeft;
    Int32 numChildren = 0;
    GetChildCount(&numChildren);

    AutoPtr<IView> prevIterationView;
    GetChildAt(numChildren - 1, (IView**)&prevIterationView);
    Int32 curPosition = 0;
    Int32 curRightEdge = 0;


    Int32 preLeft = 0;
    if (prevIterationView != NULL) {
        curPosition = mFirstPosition + numChildren;
        prevIterationView->GetLeft(&preLeft);
        curRightEdge =  preLeft - itemSpacing;
    } else {
        mFirstPosition = curPosition = mItemCount - 1;
        curRightEdge = mRight - mLeft - mPaddingRight;
        mShouldStopFling = TRUE;
    }

    while (curRightEdge > galleryLeft && curPosition < mItemCount) {
        prevIterationView = MakeAndAddView(curPosition, curPosition - mSelectedPosition,
            curRightEdge, FALSE);

        prevIterationView->GetLeft(&preLeft);
        curRightEdge = preLeft - itemSpacing;
        curPosition++;
    }
}

void Gallery::FillToGalleryLeftLtr()
{
    Int32 itemSpacing = mSpacing;
    Int32 galleryLeft = mPaddingLeft;

    AutoPtr<IView> prevIterationView;
    GetChildAt(0, (IView**)&prevIterationView);
    Int32 curPosition = 0;
    Int32 curRightEdge = 0;

    Int32 left = 0;
    if (prevIterationView != NULL) {
        curPosition = mFirstPosition - 1;
        prevIterationView->GetLeft(&left);
        curRightEdge = left - itemSpacing;
    }
    else {
        curPosition = 0;
        curRightEdge = mRight - mLeft - mPaddingRight;
        mShouldStopFling = TRUE;
    }

    while (curRightEdge > galleryLeft && curPosition >= 0) {
        prevIterationView = MakeAndAddView(curPosition,
                curPosition - mSelectedPosition,
                curRightEdge, FALSE);

        mFirstPosition = curPosition;

        prevIterationView->GetLeft(&left);
        curRightEdge = left - itemSpacing;
        curPosition--;
    }
}

void Gallery::FillToGalleryRight()
{
    if (mIsRtl) {
        FillToGalleryRightRtl();
    } else {
        FillToGalleryRightLtr();
    }
}

void Gallery::FillToGalleryRightRtl()
{
    Int32 itemSpacing = mSpacing;
    Int32 galleryRight = mRight - mLeft - mPaddingRight;

    AutoPtr<IView> prevIterationView;
    GetChildAt(0, (IView**)&prevIterationView);
    Int32 curPosition = 0;
    Int32 curLeftEdge = 0;

    Int32 preRight = 0;
    if (prevIterationView != NULL) {
        curPosition = mFirstPosition -1;
        prevIterationView->GetRight(&preRight);
        curLeftEdge = preRight + itemSpacing;
    } else {
        curPosition = 0;
        curLeftEdge = mPaddingLeft;
        mShouldStopFling = TRUE;
    }

    while (curLeftEdge < galleryRight && curPosition >= 0) {
        prevIterationView = MakeAndAddView(curPosition, curPosition - mSelectedPosition,
            curLeftEdge, TRUE);

        mFirstPosition = curPosition;

        prevIterationView->GetRight(&preRight);
        curLeftEdge = preRight + itemSpacing;
        curPosition--;
    }
}

void Gallery::FillToGalleryRightLtr()
{
    Int32 itemSpacing = mSpacing;
    Int32 galleryRight = mRight - mLeft - mPaddingRight;
    Int32 numChildren = 0;
    GetChildCount(&numChildren);
    Int32 numItems = mItemCount;

    AutoPtr<IView> prevIterationView;
    GetChildAt(numChildren - 1, (IView**)&prevIterationView);
    Int32 curPosition = 0;
    Int32 curLeftEdge = 0;

    Int32 right = 0;
    if (prevIterationView.Get() != NULL) {
        curPosition = mFirstPosition + numChildren;
        prevIterationView->GetRight(&right);
        curLeftEdge = right + itemSpacing;
    }
    else {
        mFirstPosition = curPosition = mItemCount - 1;
        curLeftEdge = mPaddingLeft;
        mShouldStopFling = TRUE;
    }

    while (curLeftEdge < galleryRight && curPosition < numItems) {
        prevIterationView = MakeAndAddView(curPosition,
                curPosition - mSelectedPosition,
                curLeftEdge, TRUE);

        prevIterationView->GetRight(&right);
        curLeftEdge = right + itemSpacing;
        curPosition++;
    }
}
AutoPtr<IView> Gallery::MakeAndAddView(
    /* [in] */ Int32 position,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 x,
    /* [in] */ Boolean fromLeft)
{
    AutoPtr<IView> child;

    if (!mDataChanged) {
        child = mRecycler->Get(position);
        if (child != NULL) {
            Int32 childLeft = 0;
            child->GetLeft(&childLeft);

            Int32 width = 0;
            child->GetMeasuredWidth(&width);
            mRightMost = Elastos::Core::Math::Max(mRightMost, childLeft + width);
            mLeftMost = Elastos::Core::Math::Min(mLeftMost, childLeft);

            SetUpChild(child, offset, x, fromLeft);

            return child;
        }
    }
    child = NULL;
    IAdapter::Probe(mAdapter)->GetView(position, NULL, this, (IView**)&child);

    SetUpChild(child, offset, x, fromLeft);

    return child;
}

void Gallery::SetUpChild(
    /* [in] */ IView* child,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 x,
    /* [in] */ Boolean fromLeft)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    if (lp == NULL) {
        lp = GenerateDefaultLayoutParams();
    }

    AddViewInLayout(child, fromLeft != mIsRtl ? -1 : 0, lp);

    child->SetSelected(offset == 0);

    Int32 pl, pt, pr, pb;
    mSpinnerPadding->Get(&pl, &pt, &pr, &pb);
    Int32 h;
    lp->GetHeight(&h);
    Int32 childHeightSpec = ViewGroup::GetChildMeasureSpec(mHeightMeasureSpec, pt + pb, h);
    Int32 w;
    lp->GetWidth(&w);
    Int32 childWidthSpec = ViewGroup::GetChildMeasureSpec(mWidthMeasureSpec, pl + pr, w);

    child->Measure(childWidthSpec, childHeightSpec);

    Int32 childLeft = 0;
    Int32 childRight = 0;

    Int32 childTop = CalculateTop(child, TRUE);

    Int32 height = 0;
    child->GetMeasuredHeight(&height);
    Int32 childBottom = childTop + height;

    Int32 width;
    child->GetMeasuredWidth(&width);
    if (fromLeft) {
        childLeft = x;
        childRight = childLeft + width;
    }
    else {
        childLeft = x - width;
        childRight = x;
    }

    child->Layout(childLeft, childTop, childRight, childBottom);
}

Int32 Gallery::CalculateTop(
    /* [in] */ IView* child,
    /* [in] */ Boolean duringLayout)
{
    Int32 myHeight = 0;
    if (duringLayout) {
        GetMeasuredHeight(&myHeight);
    }
    else {
        GetHeight(&myHeight);
    }

    Int32 measuredHeight = 0;
    Int32 height = 0;
    child->GetMeasuredHeight(&measuredHeight);
    child->GetHeight(&height);
    Int32 childHeight = duringLayout ? measuredHeight : height;

    Int32 childTop = 0;
    Int32 t, b;
    mSpinnerPadding->GetTop(&t);
    mSpinnerPadding->GetBottom(&b);
    switch (mGravity) {
    case IGravity::TOP:
        childTop = t;
        break;
    case IGravity::CENTER_VERTICAL: {
        Int32 availableSpace = myHeight - b - t - childHeight;
        childTop = t + (availableSpace / 2);
        break;
    }
    case IGravity::BOTTOM:
        childTop = myHeight - b - childHeight;
        break;
    }
    return childTop;
}

ECode Gallery::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean retValue = FALSE;
    mGestureDetector->OnTouchEvent(event, &retValue);

    Int32 action = 0;
    event->GetAction(&action);
    if (action == IMotionEvent::ACTION_UP) {
        OnUp();
    }
    else if (action == IMotionEvent::ACTION_CANCEL) {
        OnCancel();
    }

    *result = retValue;
    return NOERROR;
}

ECode Gallery::OnSingleTapUp(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    if (mDownTouchPosition >= 0) {
        ScrollToChild(mDownTouchPosition - mFirstPosition);
        if (mShouldCallbackOnUnselectedItemClick || mDownTouchPosition == mSelectedPosition) {
            Int64 id = 0;
            IAdapter::Probe(mAdapter)->GetItemId(mDownTouchPosition, &id);
            Boolean tmp = FALSE;
            PerformItemClick(mDownTouchView, mDownTouchPosition, id, &tmp);
        }

        *res = TRUE;
        return NOERROR;
    }
    *res = FALSE;
    return NOERROR;
}

ECode Gallery::OnFling(
    /* [in] */ IMotionEvent* e1,
    /* [in] */ IMotionEvent* e2,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    if (!mShouldCallbackDuringFling) {
        Boolean result = FALSE;
        RemoveCallbacks(mDisableSuppressSelectionChangedRunnable, &result);

        if (!mSuppressSelectionChanged) {
            mSuppressSelectionChanged = TRUE;
        }
    }

    mFlingRunnable->StartUsingVelocity((Int32)-velocityX);
    *res = TRUE;
    return NOERROR;
}

ECode Gallery::OnScroll(
    /* [in] */ IMotionEvent* e1,
    /* [in] */ IMotionEvent* e2,
    /* [in] */ Float distanceX,
    /* [in] */ Float distanceY,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    mParent->RequestDisallowInterceptTouchEvent(TRUE);

    if (!mShouldCallbackDuringFling) {
        if (mIsFirstScroll) {
            if (!mSuppressSelectionChanged) {
                mSuppressSelectionChanged = TRUE;
            }
            Boolean result = FALSE;
            PostDelayed(mDisableSuppressSelectionChangedRunnable, SCROLL_TO_FLING_UNCERTAINTY_TIMEOUT, &result);
        }
    }
    else {
        if (mSuppressSelectionChanged) mSuppressSelectionChanged = FALSE;
    }

    TrackMotionScroll(-1 * (Int32) distanceX);

    mIsFirstScroll = FALSE;
    *res = TRUE;
    return NOERROR;
}

ECode Gallery::OnDown(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    mFlingRunnable->Stop(FALSE);

    Float X = 0;
    Float Y = 0;
    e->GetX(&X);
    e->GetY(&Y);
    PointToPosition((Int32)X, (Int32)Y, &mDownTouchPosition);

    if (mDownTouchPosition >= 0) {
        GetChildAt(mDownTouchPosition - mFirstPosition, (IView**)&mDownTouchView);
        mDownTouchView->SetPressed(TRUE);
    }
    mIsFirstScroll = TRUE;

    *res = TRUE;
    return NOERROR;
}

ECode Gallery::OnUp()
{
    Boolean res = FALSE;
    mFlingRunnable->mScroller->IsFinished(&res);
    if (res) {
        ScrollIntoSlots();
    }

    DispatchUnpress();
    return NOERROR;
}

ECode Gallery::OnCancel()
{
    return OnUp();
}

ECode Gallery::OnLongPress(
    /* [in] */ IMotionEvent* e)
{
    if (mDownTouchPosition < 0) {
        return NOERROR;
    }

    Boolean tmp = FALSE;
    PerformHapticFeedback(IHapticFeedbackConstants::LONG_PRESS, &tmp);
    Int64 id = 0;
    GetItemIdAtPosition(mDownTouchPosition, &id);
    DispatchLongPress(mDownTouchView, mDownTouchPosition, id);
    return NOERROR;
}

ECode Gallery::OnShowPress(
    /* [in] */ IMotionEvent* e)
{
    return NOERROR;
}

void Gallery::DispatchPress(
    /* [in] */ IView* child)
{
    if (child != NULL) {
        child->SetPressed(TRUE);
    }

    SetPressed(TRUE);
}

void Gallery::DispatchUnpress()
{
    Int32 count = 0;
    GetChildCount(&count);
    for (Int32 i = count - 1; i >= 0; i--) {
        AutoPtr<IView> view;
        GetChildAt(i, (IView**)&view);
        view->SetPressed(FALSE);
    }

    SetPressed(FALSE);
}

ECode Gallery::DispatchSetSelected(
    /* [in] */ Boolean selected)
{
    return NOERROR;
}

void Gallery::DispatchSetPressed(
    /* [in] */ Boolean pressed)
{
    if (mSelectedChild != NULL) {
        mSelectedChild->SetPressed(pressed);
    }
}

AutoPtr<IContextMenuInfo> Gallery::GetContextMenuInfo()
{
    return (IContextMenuInfo*)mContextMenuInfo.Get();
}

ECode Gallery::ShowContextMenuForChild(
    /* [in] */ IView* originalView,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 longPressPosition = 0;
    GetPositionForView(originalView, &longPressPosition);
    if (longPressPosition < 0) {
        *res = FALSE;
        return NOERROR;
    }

    Int64 longPressId = 0;
    IAdapter::Probe(mAdapter)->GetItemId(longPressPosition, &longPressId);
    *res = DispatchLongPress(originalView, longPressPosition, longPressId);
    return NOERROR;
}

ECode Gallery::ShowContextMenu(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    if ((IsPressed(res), *res) && mSelectedPosition >= 0) {
        Int32 index = mSelectedPosition - mFirstPosition;
        AutoPtr<IView> v;
        GetChildAt(index, (IView**)&v);
        *res = DispatchLongPress(v, mSelectedPosition, mSelectedRowId);
        return NOERROR;
    }

    *res = FALSE;
    return NOERROR;
}

Boolean Gallery::DispatchLongPress(
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    Boolean handled = FALSE;

    if (mOnItemLongClickListener != NULL) {
        mOnItemLongClickListener->OnItemLongClick(this,
                mDownTouchView, mDownTouchPosition, id, &handled);
    }

    if (!handled) {
        mContextMenuInfo = new AdapterContextMenuInfo(view, position, id);
        AbsSpinner::ShowContextMenuForChild(this, &handled);
    }

    if (handled) {
        Boolean tmp = FALSE;
        PerformHapticFeedback(IHapticFeedbackConstants::LONG_PRESS, &tmp);
    }

    return handled;
}

ECode Gallery::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    return event->Dispatch(this, NULL, NULL, res);
}

ECode Gallery::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_LEFT:
            if (MovePrevious()) {
                PlaySoundEffect(SoundEffectConstants::NAVIGATION_LEFT);
                *res = TRUE;
                return NOERROR;
            }
            break;

        case IKeyEvent::KEYCODE_DPAD_RIGHT:
            if (MoveNext()) {
                PlaySoundEffect(SoundEffectConstants::NAVIGATION_RIGHT);
                *res = TRUE;
                return NOERROR;
            }
            break;

        case IKeyEvent::KEYCODE_DPAD_CENTER:
        case IKeyEvent::KEYCODE_ENTER:
            mReceivedInvokeKeyDown = TRUE;
    }

    return AbsSpinner::OnKeyDown(keyCode, event, res);
}

ECode Gallery::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_CENTER:
        case IKeyEvent::KEYCODE_ENTER: {
            if (mReceivedInvokeKeyDown) {
                if (mItemCount > 0) {
                    if (mKeyUpRunnable = NULL) {
                        mKeyUpRunnable = new KeyUpRunnable(this);
                    }
                    DispatchPress(mSelectedChild);
                    Boolean result = FALSE;
                    PostDelayed(mKeyUpRunnable, ViewConfiguration::GetPressedStateDuration(), &result);

                    Int32 selectedIndex = mSelectedPosition - mFirstPosition;

                    Int64 id = 0;
                    IAdapter::Probe(mAdapter)->GetItemId(mSelectedPosition, &id);
                    AutoPtr<IView> view;
                    GetChildAt(selectedIndex, (IView**)&view);
                    Boolean tmp = FALSE;
                    PerformItemClick(view, mSelectedPosition, id, &tmp);
                }
            }

            mReceivedInvokeKeyDown = FALSE;

            *res = TRUE;
            return NOERROR;
        }
    }

    return AbsSpinner::OnKeyUp(keyCode, event, res);
}

Boolean Gallery::MovePrevious()
{
    if (mItemCount > 0 && mSelectedPosition > 0) {
        ScrollToChild(mSelectedPosition - mFirstPosition - 1);
        return TRUE;
    }
    else {
        return FALSE;
    }
}

Boolean Gallery::MoveNext()
{
    if (mItemCount > 0 && mSelectedPosition < mItemCount - 1) {
        ScrollToChild(mSelectedPosition - mFirstPosition + 1);
        return TRUE;
    }
    else {
        return FALSE;
    }
}

Boolean Gallery::ScrollToChild(
    /* [in] */ Int32 childPosition)
{
    AutoPtr<IView> child;
    GetChildAt(childPosition, (IView**)&child);

    if (child != NULL) {
        Int32 distance = GetCenterOfGallery() - GetCenterOfView(child);
        mFlingRunnable->StartUsingDistance(distance);
        return TRUE;
    }

    return FALSE;
}

void Gallery::SetSelectedPositionInt(
    /* [in] */ Int32 position)
{
    AbsSpinner::SetSelectedPositionInt(position);

    UpdateSelectedItemMetadata();
}

void Gallery::UpdateSelectedItemMetadata()
{
    AutoPtr<IView> oldSelectedChild = mSelectedChild;

    AutoPtr<IView> view;
    GetChildAt(mSelectedPosition - mFirstPosition, (IView**)&view);
    AutoPtr<IView> child = mSelectedChild = view;
    if (child == NULL) {
        return;
    }

    child->SetSelected(TRUE);
    child->SetFocusable(TRUE);

    Boolean hasFocus = FALSE;
    if (HasFocus(&hasFocus), hasFocus) {
        Boolean result = FALSE;
        child->RequestFocus(&result);
    }

    if (oldSelectedChild != NULL && oldSelectedChild != child) {
        oldSelectedChild->SetSelected(FALSE);
        oldSelectedChild->SetFocusable(FALSE);
    }
}

ECode Gallery::SetGravity(
    /* [in] */ Int32 gravity)
{
    if (mGravity != gravity) {
        mGravity = gravity;
        RequestLayout();
    }

    return NOERROR;
}

Int32 Gallery::GetChildDrawingOrder(
    /* [in] */ Int32 childCount,
    /* [in] */ Int32 i)
{
    Int32 selectedIndex = mSelectedPosition - mFirstPosition;

    if (selectedIndex < 0) return i;

    if (i == childCount - 1) {
        return selectedIndex;
    }
    else if (i >= selectedIndex) {
        return i + 1;
    }
    else {
        return i;
    }
}

void Gallery::OnFocusChanged(
    /* [in] */ Boolean gainFocus,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    AbsSpinner::OnFocusChanged(gainFocus, direction, previouslyFocusedRect);

    if (gainFocus && mSelectedChild != NULL) {
        Boolean isFocus = FALSE;
        mSelectedChild->RequestFocus(direction, &isFocus);
        mSelectedChild->SetSelected(TRUE);
    }
}

ECode Gallery::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    AbsSpinner::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(GALLERY_NAME, (ICharSequence**)&seq));
    IAccessibilityRecord::Probe(event)->SetClassName(seq);
    return NOERROR;
}

ECode Gallery::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    AbsSpinner::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(GALLERY_NAME, (ICharSequence**)&seq));
    info->SetClassName(seq);
    info->SetScrollable(mItemCount > 1);
    Boolean enabled = FALSE;
    if(IsEnabled(&enabled), enabled) {
        if(mItemCount > 0 && mSelectedPosition < mItemCount - 1){
            info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD);
        }
        if((IsEnabled(&enabled), enabled) && mItemCount > 0 && mSelectedPosition > 0){
            info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD);
        }
    }
    return NOERROR;
}

ECode Gallery::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    if (AbsSpinner::PerformAccessibilityAction(action, arguments, res)) {
        *res = TRUE;
        return NOERROR;
    }
    switch (action) {
        case IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD: {
            Boolean enabled = FALSE;
            if ((IsEnabled(&enabled), enabled) && mItemCount > 0 && mSelectedPosition < mItemCount - 1) {
                Int32 currentChildIndex = mSelectedPosition - mFirstPosition;
                *res = ScrollToChild(currentChildIndex + 1);
                return NOERROR;
            }
            return NOERROR;
        }
        case IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD: {
            Boolean enabled = FALSE;
            if ((IsEnabled(&enabled), enabled) && mItemCount > 0 && mSelectedPosition > 0) {
                Int32 currentChildIndex = mSelectedPosition - mFirstPosition;
                *res = ScrollToChild(currentChildIndex - 1);
                return NOERROR;
            }
            return NOERROR;
        }
    }
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
