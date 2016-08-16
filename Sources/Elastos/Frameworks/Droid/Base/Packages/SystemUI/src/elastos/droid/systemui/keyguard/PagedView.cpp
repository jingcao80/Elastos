
#include "elastos/droid/systemui/keyguard/PagedView.h"
#include <elastos/core/Math.h>
#include "Elastos.Droid.Utility.h"
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::CValueAnimator;
using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CPointF;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IVelocityTrackerHelper;
using Elastos::Droid::View::CVelocityTrackerHelper;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::EIID_IViewGroupOnHierarchyChangeListener;
using Elastos::Droid::View::Animation::CLinearInterpolator;
using Elastos::Droid::View::Animation::CAccelerateInterpolator;
using Elastos::Droid::View::Animation::EIID_IInterpolator;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityManagerHelper;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Widget::CScroller;
using Elastos::Core::Math;
using Elastos::Core::CoreUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

PagedView::SavedState::SavedState()
    : mCurrentPage(-1)
{
}

ECode PagedView::SavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return View::BaseSavedState::constructor(superState);
}

ECode PagedView::SavedState::WriteToParcel(
    /* [in] */ IParcel* out)
{
    View::BaseSavedState::WriteToParcel(out);
    return out->WriteInt32(mCurrentPage);
}

ECode PagedView::SavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    View::BaseSavedState::ReadFromParcel(source);
    return source->ReadInt32(&mCurrentPage);
}

ECode PagedView::MyRunnable::Run()
{
    // Update the down scroll position to account for the fact that the
    // current page is moved
    Int32 coffset;
    mHost->GetChildOffset(mPageUnderPointIndex, &coffset);
    Int32 rcoffset;
    mHost->GetRelativeChildOffset(mPageUnderPointIndex, &rcoffset);
    mHost->mDownScrollX = coffset - rcoffset;

    // Setup the scroll to the correct page before we swap the views
    mHost->SnapToPage(mPageUnderPointIndex);

    // For each of the pages between the paged view and the drag view,
    // animate them from the previous position to the new position in
    // the layout (as a result of the drag view moving in the layout)
    Int32 shiftDelta = (mDragViewIndex < mPageUnderPointIndex) ? -1 : 1;
    Int32 lowerIndex = (mDragViewIndex < mPageUnderPointIndex) ?
            mDragViewIndex + 1 : mPageUnderPointIndex;
    Int32 upperIndex = (mDragViewIndex > mPageUnderPointIndex) ?
            mDragViewIndex - 1 : mPageUnderPointIndex;
    for (Int32 i = lowerIndex; i <= upperIndex; ++i) {
        AutoPtr<IView> v;
        mHost->GetChildAt(i, (IView**)&v);
        // dragViewIndex < pageUnderPointIndex, so after we remove the
        // drag view all subsequent views to pageUnderPointIndex will
        // shift down.
        Int32 offsetX;
        mHost->GetViewportOffsetX(&offsetX);
        Int32 coffset;
        mHost->GetChildOffset(i, &coffset);
        Int32 oldX = offsetX + coffset;
        mHost->GetChildOffset(i + shiftDelta, &coffset);
        Int32 newX = offsetX + coffset;

        // Animate the view translation from its old position to its new
        // position
        AutoPtr<IInterface> obj;
        v->GetTag((IInterface**)&obj);
        AutoPtr<IAnimatorSet> anim = IAnimatorSet::Probe(obj);
        if (anim != NULL) {
            IAnimator::Probe(anim)->Cancel();
        }

        v->SetTranslationX(oldX - newX);
        CAnimatorSet::New((IAnimatorSet**)&anim);
        IAnimator::Probe(anim)->SetDuration(
                mHost->REORDERING_REORDER_REPOSITION_DURATION);

        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = 0.0f;
        AutoPtr<IObjectAnimator> animator;
        helper->OfFloat(v, String("translationX"), array, (IObjectAnimator**)&animator);
        AutoPtr<ArrayOf<IAnimator*> > array2 = ArrayOf<IAnimator*>::Alloc(1);
        array2->Set(0, IAnimator::Probe(animator));
        anim->PlayTogether(array2);
        IAnimator::Probe(anim)->Start();
        v->SetTag(TO_IINTERFACE(anim));
    }

    mHost->RemoveView(mHost->mDragView);
    mHost->OnRemoveView(mHost->mDragView, FALSE);
    mHost->AddView(mHost->mDragView, mPageUnderPointIndex);
    mHost->OnAddView(mHost->mDragView, mPageUnderPointIndex);
    mHost->mSidePageHoverIndex = -1;
    return NOERROR;
}

ECode PagedView::MyRunnable2::Run()
{
    return mHost->HideScrollingIndicator(FALSE);
}

CAR_INTERFACE_IMPL_2(PagedView::ScrollInterpolator, Object, IInterpolator
        , ITimeInterpolator)

ECode PagedView::ScrollInterpolator::GetInterpolation(
    /* [in] */ Float t,
    /* [out] */ Float* output)
{
    VALIDATE_NOT_NULL(output)

    t -= 1.0f;
    *output = t*t*t*t*t + 1;
    return NOERROR;
}

ECode PagedView::ScrollInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    *res = FALSE;
    return NOERROR;
}

ECode PagedView::MyAnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mCancelled = TRUE;
    return NOERROR;
}

ECode PagedView::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (!mCancelled) {
        mHost->mScrollIndicator->SetVisibility(IView::INVISIBLE);
    }
    return NOERROR;
}

ECode PagedView::MyAnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->OnPostReorderingAnimationCompleted();
    return NOERROR;
}

ECode PagedView::MyAnimatorListenerAdapter3::MyAnimatorListenerAdapter4::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    return mHost->mDeleteDropTarget->SetAlpha(0.0f);
}

ECode PagedView::MyAnimatorListenerAdapter3::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    // Show the delete drop target
    if (mHost->mDeleteDropTarget != NULL) {
        mHost->mDeleteDropTarget->SetVisibility(IView::VISIBLE);
        AutoPtr<IViewPropertyAnimator> animator;
        mHost->mDeleteDropTarget->Animate((IViewPropertyAnimator**)&animator);
        animator->Alpha(1.0f);
        animator->SetDuration(mHost->REORDERING_DELETE_DROP_TARGET_FADE_DURATION);
        AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter4(mHost);
        animator->SetListener(lis);
    }
    return NOERROR;
}

ECode PagedView::MyRunnable3::Run()
{
    return mHost->OnEndReordering();
}

ECode PagedView::MyRunnable4::Run()
{
    Boolean res;
    return mHost->ZoomIn(mOnCompleteRunnable, &res);
}

ECode PagedView::MyAnimatorListenerAdapter5::MyAnimatorListenerAdapter6::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return mHost->mDeleteDropTarget->SetVisibility(IView::GONE);
}

ECode PagedView::MyAnimatorListenerAdapter5::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    // Hide the delete drop target
    if (mHost->mDeleteDropTarget != NULL) {
        AutoPtr<IViewPropertyAnimator> animator;
        mHost->mDeleteDropTarget->Animate((IViewPropertyAnimator**)&animator);
        animator->Alpha(0.0f);
        animator->SetDuration(mHost->REORDERING_DELETE_DROP_TARGET_FADE_DURATION);
        AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter6(mHost);
        animator->SetListener(lis);
    }
    return NOERROR;
}

ECode PagedView::MyAnimatorListenerAdapter5::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mHost->mDragView = NULL;
    return NOERROR;
}

ECode PagedView::MyAnimatorListenerAdapter5::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mDragView = NULL;
    if (mOnCompleteRunnable != NULL) {
        mOnCompleteRunnable->Run();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(PagedView::FlingAlongVectorAnimatorUpdateListener, Object,
        IAnimatorUpdateListener)

PagedView::FlingAlongVectorAnimatorUpdateListener::FlingAlongVectorAnimatorUpdateListener()
    : mPrevTime(0)
    , mFriction(0.0f)
{
    CDecelerateInterpolator::New(0.75f, (ITimeInterpolator**)&mAlphaInterpolator);
}

ECode PagedView::FlingAlongVectorAnimatorUpdateListener::constructor(
    /* [in] */ IView* dragView,
    /* [in] */ IPointF* vel,
    /* [in] */ IRect* from,
    /* [in] */ Int64 startTime,
    /* [in] */ Float friction)
{
    mDragView = dragView;
    mVelocity = vel;
    mFrom = from;
    mPrevTime = startTime;

    AutoPtr<IResources> resources;
    mDragView->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> displayMetrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&displayMetrics);
    Float density;
    displayMetrics->GetDensity(&density);
    mFriction = 1.0f - (density * friction);
    return NOERROR;
}

ECode PagedView::FlingAlongVectorAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    AutoPtr<IFloat> value = IFloat::Probe(obj);
    Float t;
    value->GetValue(&t);

    AutoPtr<IAnimationUtils> helper;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&helper);
    Int64 curTime;
    helper->CurrentAnimationTimeMillis(&curTime);

    Float x;
    mVelocity->GetX(&x);
    Int32 left;
    mFrom->GetLeft(&left);
    mFrom->SetLeft(left + (x * (curTime - mPrevTime) / 1000.0f));

    Float y;
    mVelocity->GetY(&y);
    Int32 top;
    mFrom->GetTop(&top);
    mFrom->SetTop(top + (y * (curTime - mPrevTime) / 1000.0f));

    mFrom->GetLeft(&left);
    mDragView->SetTranslationX(left);
    mFrom->GetTop(&top);
    mDragView->SetTranslationY(top);

    Float output;
    mAlphaInterpolator->GetInterpolation(t, &output);
    mDragView->SetAlpha(1.0f - output);

    mVelocity->GetX(&x);
    mVelocity->SetX(x * mFriction);
    mVelocity->GetY(&y);
    mVelocity->SetY(y * mFriction);
    mPrevTime = curTime;
    return NOERROR;
}

ECode PagedView::MyRunnable5::Run()
{
    Int32 dragViewIndex;
    mHost->IndexOfChild(mDragView, &dragViewIndex);

    // For each of the pages around the drag view, animate them from the previous
    // position to the new position in the layout (as a result of the drag view moving
    // in the layout)
    // NOTE: We can make an assumption here because we have side-bound pages that we
    //       will always have pages to animate in from the left
    mHost->GetVisiblePages(mHost->mTempVisiblePagesRange);
    mHost->BoundByReorderablePages(TRUE, mHost->mTempVisiblePagesRange);
    Boolean isLastWidgetPage = ((*(mHost->mTempVisiblePagesRange))[0]
            == (*(mHost->mTempVisiblePagesRange))[1]);
    Boolean slideFromLeft = (isLastWidgetPage ||
            dragViewIndex > (*(mHost->mTempVisiblePagesRange))[0]);

    // Setup the scroll to the correct page before we swap the views
    if (slideFromLeft) {
        mHost->SnapToPageImmediately(dragViewIndex - 1);
    }

    Int32 firstIndex = (isLastWidgetPage ? 0 : (*(mHost->mTempVisiblePagesRange))[0]);
    Int32 count;
    mHost->GetPageCount(&count);
    Int32 lastIndex = Elastos::Core::Math::Min((*(mHost->mTempVisiblePagesRange))[1],
            count - 1);
    Int32 lowerIndex = (slideFromLeft ? firstIndex : dragViewIndex + 1 );
    Int32 upperIndex = (slideFromLeft ? dragViewIndex - 1 : lastIndex);
    AutoPtr<IArrayList> animations;
    CArrayList::New((IArrayList**)&animations);
    for (Int32 i = lowerIndex; i <= upperIndex; ++i) {
        AutoPtr<IView> v;
        mHost->GetChildAt(i, (IView**)&v);
        // dragViewIndex < pageUnderPointIndex, so after we remove the
        // drag view all subsequent views to pageUnderPointIndex will
        // shift down.
        Int32 oldX = 0;
        Int32 newX = 0;
        if (slideFromLeft) {
            Int32 ox;
            mHost->GetViewportOffsetX(&ox);
            if (i == 0) {
                // Simulate the page being offscreen with the page spacing
                Int32 co;
                mHost->GetChildOffset(i, &co);
                Int32 cw;
                mHost->GetChildWidth(i, &cw);
                oldX = ox + co - cw - mHost->mPageSpacing;
            }
            else {
                Int32 co;
                mHost->GetChildOffset(i - 1, &co);
                oldX = ox + co;
            }
            Int32 co;
            mHost->GetChildOffset(i, &co);
            newX = ox + co;
        }
        else {
            Int32 co;
            mHost->GetChildOffset(i, &co);
            Int32 co2;
            mHost->GetChildOffset(i - 1, &co2);
            oldX = co - co2;
            newX = 0;
        }

        // Animate the view translation from its old position to its new
        // position
        AutoPtr<IInterface> obj;
        v->GetTag((IInterface**)&obj);
        AutoPtr<IAnimatorSet> anim = IAnimatorSet::Probe(obj);
        if (anim != NULL) {
            IAnimator::Probe(anim)->Cancel();
        }

        // Note: Hacky, but we want to skip any optimizations to not draw completely
        // hidden views
        Float alpha;
        v->GetAlpha(&alpha);
        v->SetAlpha(Elastos::Core::Math::Max(alpha, 0.01f));
        v->SetTranslationX(oldX - newX);
        CAnimatorSet::New((IAnimatorSet**)&anim);

        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = 0.0f;
        AutoPtr<IObjectAnimator> animatorx;
        helper->OfFloat(v, String("translationX"), array, (IObjectAnimator**)&animatorx);
        AutoPtr<ArrayOf<Float> > array2 = ArrayOf<Float>::Alloc(1);
        (*array2)[0] = 1.0f;
        AutoPtr<IObjectAnimator> animatory;
        helper->OfFloat(v, String("alpha"), array2, (IObjectAnimator**)&animatory);
        AutoPtr<ArrayOf<IAnimator*> > array3 = ArrayOf<IAnimator*>::Alloc(2);
        array3->Set(0, IAnimator::Probe(animatorx));
        array3->Set(1, IAnimator::Probe(animatory));
        anim->PlayTogether(array3);
        animations->Add(anim);
        v->SetTag(anim);
    }

    AutoPtr<IAnimatorSet> slideAnimations;
    CAnimatorSet::New((IAnimatorSet**)&slideAnimations);
    slideAnimations->PlayTogether(ICollection::Probe(animations));
    IAnimator::Probe(slideAnimations)->SetDuration(
            mHost->DELETE_SLIDE_IN_SIDE_PAGE_DURATION);
    AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter7(mHost);
    IAnimator::Probe(slideAnimations)->AddListener(lis);
    IAnimator::Probe(slideAnimations)->Start();

    mHost->RemoveView(mDragView);
    return mHost->OnRemoveView(mDragView, TRUE);
}

ECode PagedView::MyAnimatorListenerAdapter7::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<IRunnable> onCompleteRunnable = new MyRunnable6(mHost);
    Boolean res;
    return mHost->ZoomIn(onCompleteRunnable, &res);
}

ECode PagedView::MyAnimatorListenerAdapter7::MyRunnable6::Run()
{
    mHost->mDeferringForDelete = FALSE;
    mHost->OnEndReordering();
    return mHost->OnRemoveViewAnimationCompleted();
}

CAR_INTERFACE_IMPL(PagedView::MyTimeInterpolator,
        Object, ITimeInterpolator)

ECode PagedView::MyTimeInterpolator::GetInterpolation(
    /* [in] */ Float t,
    /* [out] */ Float* output)
{
    VALIDATE_NOT_NULL(output)

    if (mCount < 0) {
        mCount++;
    }
    else if (mCount == 0) {
        AutoPtr<IAnimationUtils> helper;
        CAnimationUtils::AcquireSingleton((IAnimationUtils**)&helper);
        Int64 timeMillis;
        helper->CurrentAnimationTimeMillis(&timeMillis);
        mOffset = Elastos::Core::Math::Min(0.5f, (Float)(timeMillis -
                mStartTime) / mHost->FLING_TO_DELETE_FADE_OUT_DURATION);
        mCount++;
    }
    *output = Elastos::Core::Math::Min(1.0f, mOffset + t);
    return NOERROR;
}

ECode PagedView::MyTimeInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    *res = FALSE;
    return NOERROR;
}

ECode PagedView::MyAnimatorListenerAdapter8::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return mOnAnimationEndRunnable->Run();
}

ECode PagedView::MyAnimatorListenerAdapter9::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return mOnAnimationEndRunnable->Run();
}

ECode PagedView::MyAnimatorListenerAdapter10::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mWarpAnimation = NULL;
    if (mHost->mTouchState != TOUCH_STATE_SCROLLING &&
            mHost->mTouchState != TOUCH_STATE_READY) {
        mHost->AnimateWarpPageOffScreen(String("onScreen end"), TRUE);
    }
    return NOERROR;
}

ECode PagedView::MyAnimatorListenerAdapter10::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    AnimatorListenerAdapter::OnAnimationCancel(animation);
    mHost->mWarpAnimation = NULL;
    return NOERROR;
}

ECode PagedView::MyAnimatorListenerAdapter11::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mWarpAnimation = NULL;
    mHost->mWarpPageExposed = FALSE;
    Int32 index;
    mHost->GetPageWarpIndex(&index);
    AutoPtr<IView> view;
    mHost->GetPageAt(index, (IView**)&view);
    AutoPtr<IKeyguardWidgetFrame> v = IKeyguardWidgetFrame::Probe(view);
    return IView::Probe(v)->SetTranslationX(0.0f);
}

ECode PagedView::MyAnimatorListenerAdapter11::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    AnimatorListenerAdapter::OnAnimationCancel(animation);
    mHost->mWarpAnimation = NULL;
    return NOERROR;
}

const Int32 PagedView::INVALID_PAGE = -1;

const Int32 PagedView::PAGE_SNAP_ANIMATION_DURATION = 750;
const Int32 PagedView::SLOW_PAGE_SNAP_ANIMATION_DURATION = 950;
const Float PagedView::NANOTIME_DIV = 1000000000.0f;

const Int32 PagedView::TOUCH_STATE_REST = 0;
const Int32 PagedView::TOUCH_STATE_SCROLLING = 1;
const Int32 PagedView::TOUCH_STATE_PREV_PAGE = 2;
const Int32 PagedView::TOUCH_STATE_NEXT_PAGE = 3;
const Int32 PagedView::TOUCH_STATE_REORDERING = 4;
const Int32 PagedView::TOUCH_STATE_READY = 5; // when finger is down

const Float PagedView::ALPHA_QUANTIZE_LEVEL = 0.0001f;
const Float PagedView::TOUCH_SLOP_SCALE = 1.0f;

const Int32 PagedView::INVALID_POINTER = -1;

const Int32 PagedView::sScrollIndicatorFadeInDuration = 150;
const Int32 PagedView::sScrollIndicatorFadeOutDuration = 650;
const Int32 PagedView::sScrollIndicatorFlashDuration = 650;

const Int32 PagedView::WARP_SNAP_DURATION = 160;
const String PagedView::TAG("WidgetPagedView");
const Boolean PagedView::DEBUG = IKeyguardConstants::DEBUG;
const Boolean PagedView::DEBUG_WARP = FALSE;

const Int32 PagedView::WARP_PEEK_ANIMATION_DURATION = 150;
const Float PagedView::WARP_ANIMATE_AMOUNT = -75.0f; // in dip

const Int32 PagedView::MIN_LENGTH_FOR_FLING = 25;

const Float PagedView::OVERSCROLL_ACCELERATE_FACTOR = 2;
const Float PagedView::OVERSCROLL_DAMP_FACTOR = 0.14f;

const Float PagedView::RETURN_TO_ORIGINAL_PAGE_THRESHOLD = 0.33f;
const Float PagedView::SIGNIFICANT_MOVE_THRESHOLD = 0.5f;

const Int32 PagedView::FLING_THRESHOLD_VELOCITY = 1500;
const Int32 PagedView::MIN_SNAP_VELOCITY = 1500;
const Int32 PagedView::MIN_FLING_VELOCITY = 500;

const Boolean PagedView::DISABLE_TOUCH_INTERACTION = FALSE;
const Boolean PagedView::DISABLE_TOUCH_SIDE_PAGES = TRUE;
const Boolean PagedView::DISABLE_FLING_TO_DELETE = FALSE;

CAR_INTERFACE_IMPL_2(PagedView, ViewGroup, IPagedView, IViewGroupOnHierarchyChangeListener)

PagedView::PagedView()
    : mFlingThresholdVelocity(0)
    , mMinFlingVelocity(0)
    , mMinSnapVelocity(0)
    , mDensity(0.0f)
    , mSmoothingTime(0.0f)
    , mTouchX(0.0f)
    , mFirstLayout(TRUE)
    , mCurrentPage(0)
    , mChildCountOnLastMeasure(0)
    , mNextPage(INVALID_PAGE)
    , mMaxScrollX(0)
    , mLastMotionX(0.0f)
    , mLastMotionXRemainder(0.0f)
    , mLastMotionY(0.0f)
    , mTotalMotionX(0.0f)
    , mTouchState(TOUCH_STATE_REST)
    , mForceScreenScrolled(FALSE)
    , mTouchSlop(0)
    , mPageSpacing(0)
    , mCellCountX(0)
    , mCellCountY(0)
    , mAllowOverScroll(TRUE)
    , mUnboundedScrollX(0)
    , mForceDrawAllChildrenNextFrame(FALSE)
    , mOverScrollX(0)
    , mLayoutScale(1.0f)
    , mActivePointerId(INVALID_POINTER)
    , mContentIsRefreshable(TRUE)
    , mFadeInAdjacentScreens(FALSE)
    , mUsePagingTouchSlop(TRUE)
    , mDeferScrollUpdate(FALSE)
    , mIsPageMoving(FALSE)
    , mIsDataReady(TRUE)
    , REORDERING_REORDER_REPOSITION_DURATION(300)
    , REORDERING_ZOOM_IN_OUT_DURATION(250)
    , mFlingToDeleteThresholdVelocity(-1400)
    , mParentDownMotionX(0.0f)
    , mParentDownMotionY(0.0f)
    , mDownMotionX(0.0f)
    , mDownMotionY(0.0f)
    , mDownScrollX(0.0f)
    , mLastScreenCenter(-1)
    , mDeleteString(NULL)
    , mPagingTouchSlop(0)
    , mMaximumVelocity(0)
    , mMinimumWidth(0)
    , mScrollIndicatorPaddingLeft(0)
    , mScrollIndicatorPaddingRight(0)
    , mShouldShowScrollIndicator(FALSE)
    , mShouldShowScrollIndicatorImmediately(FALSE)
    , REORDERING_DROP_REPOSITION_DURATION(200)
    , REORDERING_SIDE_PAGE_HOVER_TIMEOUT(300)
    , REORDERING_SIDE_PAGE_BUFFER_PERCENTAGE(0.1f)
    , REORDERING_DELETE_DROP_TARGET_FADE_DURATION(150)
    , mMinScale(1.0f)
    , mSidePageHoverIndex(-1)
    , mReorderingStarted(FALSE)
    , mIsReordering(FALSE)
    , NUM_ANIMATIONS_RUNNING_BEFORE_ZOOM_OUT(2)
    , mPostReorderingPreZoomInRemainingAnimationCount(0)
    , mOnlyAllowEdgeSwipes(FALSE)
    , mDownEventOnEdge(FALSE)
    , mEdgeSwipeRegionSize(0)
    , FLING_TO_DELETE_FADE_OUT_DURATION(350)
    , FLING_TO_DELETE_FRICTION(0.035f)
    , FLING_TO_DELETE_MAX_FLING_DEGREES(65.0f)
    , mDeferringForDelete(FALSE)
    , DELETE_SLIDE_IN_SIDE_PAGE_DURATION(250)
    , DRAG_TO_DELETE_FADE_OUT_DURATION(350)
    , mTopAlignPageWhenShrinkingForBouncer(FALSE)
    , mPageSwapIndex(-1)
    , mPageWarpIndex(-1)
    , mWarpPageExposed(FALSE)
    , mIsCameraEvent(FALSE)
    , mWarpPeekAmount(0.0f)
    , mOnPageEndWarpCalled(FALSE)
    , mOnPageBeginWarpCalled(FALSE)

{
    mTempVisiblePagesRange = ArrayOf<Int32>::Alloc(2);

    CRect::New((IRect**)&mViewport);

    CMatrix::New((IMatrix**)&mTmpInvMatrix);
    mTmpPoint = ArrayOf<Float>::Alloc(2);
    CRect::New((IRect**)&mTmpRect);
    CRect::New((IRect**)&mAltTmpRect);

    mHideScrollingIndicatorRunnable = new MyRunnable2(this);

    mOnScreenAnimationListener = new MyAnimatorListenerAdapter10(this);

    mOffScreenAnimationListener = new MyAnimatorListenerAdapter11(this);
}

ECode PagedView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode PagedView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode PagedView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    ViewGroup::constructor(context, attrs, defStyle);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::PagedView);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a);

    Int32 size;
    a->GetDimensionPixelSize(R::styleable::PagedView_pageSpacing, 0, &size);
    SetPageSpacing(size);
    a->GetDimensionPixelSize(R::styleable::PagedView_scrollIndicatorPaddingLeft, 0,
            &mScrollIndicatorPaddingLeft);
    a->GetDimensionPixelSize(R::styleable::PagedView_scrollIndicatorPaddingRight, 0,
            &mScrollIndicatorPaddingRight);
    a->Recycle();

    AutoPtr<IResources> r;
    GetResources((IResources**)&r);
    r->GetDimensionPixelSize(R::dimen::kg_edge_swipe_region_size, &mEdgeSwipeRegionSize);
    r->GetBoolean(R::bool_::kg_top_align_page_shrink_on_bouncer_visible,
            &mTopAlignPageWhenShrinkingForBouncer);

    SetHapticFeedbackEnabled(FALSE);
    return Init();
}

ECode PagedView::Init()
{
    CArrayList::New((IArrayList**)&mDirtyPageContent);
    mDirtyPageContent->EnsureCapacity(32);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterpolator> interpolator = new ScrollInterpolator();
    CScroller::New(context, interpolator, (IScroller**)&mScroller);
    mCurrentPage = 0;

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> configuration;
    helper->Get(context, (IViewConfiguration**)&configuration);
    configuration->GetScaledTouchSlop(&mTouchSlop);
    configuration->GetScaledPagingTouchSlop(&mPagingTouchSlop);
    configuration->GetScaledMaximumFlingVelocity(&mMaximumVelocity);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    metrics->GetDensity(&mDensity);
    mWarpPeekAmount = mDensity * WARP_ANIMATE_AMOUNT;

    // Scale the fling-to-delete threshold by the density
    mFlingToDeleteThresholdVelocity = (Int32)(mFlingToDeleteThresholdVelocity * mDensity);

    mFlingThresholdVelocity = (Int32)(FLING_THRESHOLD_VELOCITY * mDensity);
    mMinFlingVelocity = (Int32)(MIN_FLING_VELOCITY * mDensity);
    mMinSnapVelocity = (Int32)(MIN_SNAP_VELOCITY * mDensity);
    return SetOnHierarchyChangeListener(this);
}

ECode PagedView::SetDeleteDropTarget(
    /* [in] */IView* v)
{
    mDeleteDropTarget = v;
    return NOERROR;
}

ECode PagedView::MapPointFromViewToParent(
    /* [in] */ IView* v,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [out, callee] */ ArrayOf<Float>** array)
{
    VALIDATE_NOT_NULL(array)

    (*mTmpPoint)[0] = x;
    (*mTmpPoint)[1] = y;

    AutoPtr<IMatrix> matrix;
    v->GetMatrix((IMatrix**)&matrix);
    matrix->MapPoints(mTmpPoint);

    Int32 left;
    v->GetLeft(&left);
    (*mTmpPoint)[0] += left;

    Int32 top;
    v->GetTop(&top);
    (*mTmpPoint)[1] += top;

    *array = mTmpPoint;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode PagedView::MapPointFromParentToView(
    /* [in] */ IView* v,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [out, callee] */ ArrayOf<Float>** array)
{
    VALIDATE_NOT_NULL(array)

    Int32 left;
    v->GetLeft(&left);
    (*mTmpPoint)[0] = x - left;

    Int32 top;
    v->GetTop(&top);
    (*mTmpPoint)[1] = y - top;

    AutoPtr<IMatrix> matrix;
    v->GetMatrix((IMatrix**)&matrix);
    Boolean res;
    matrix->Invert(mTmpInvMatrix, &res);
    mTmpInvMatrix->MapPoints(mTmpPoint);

    *array = mTmpPoint;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode PagedView::UpdateDragViewTranslationDuringDrag()
{
    Int32 _x;
    GetScrollX(&_x);
    Float x = mLastMotionX - mDownMotionX + _x - mDownScrollX;
    Float y = mLastMotionY - mDownMotionY;
    mDragView->SetTranslationX(x);
    mDragView->SetTranslationY(y);

    if (DEBUG) Logger::D(TAG, "PagedView.updateDragViewTranslationDuringDrag(): %f, %f", x, y);
    return NOERROR;
}

ECode PagedView::SetMinScale(
    /* [in] */ Float f)
{
    mMinScale = f;
    return RequestLayout();
}

ECode PagedView::SetScaleX(
    /* [in] */ Float scaleX)
{
    ViewGroup::SetScaleX(scaleX);

    Boolean res;
    if (IsReordering(TRUE, &res), res) {
        AutoPtr<ArrayOf<Float> > p;
        MapPointFromParentToView(this, mParentDownMotionX, mParentDownMotionY, (ArrayOf<Float>**)&p);
        mLastMotionX = (*p)[0];
        mLastMotionY = (*p)[1];
        UpdateDragViewTranslationDuringDrag();
    }
    return NOERROR;
}

ECode PagedView::GetViewportWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)

    return mViewport->GetWidth(width);
}

ECode PagedView::GetViewportHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)

    return mViewport->GetHeight(height);
}

ECode PagedView::GetViewportOffsetX(
    /* [out] */ Int32* x)
{
    VALIDATE_NOT_NULL(x)

    Int32 mwidth;
    GetMeasuredWidth(&mwidth);
    Int32 vwidth;
    GetViewportWidth(&vwidth);
    *x = (mwidth - vwidth) / 2;
    return NOERROR;
}

ECode PagedView::GetViewportOffsetY(
    /* [out] */ Int32* y)
{
    VALIDATE_NOT_NULL(y)

    Int32 mheight;
    GetMeasuredHeight(&mheight);
    Int32 vheight;
    GetViewportHeight(&vheight);
    *y = (mheight - vheight) / 2;
    return NOERROR;
}

ECode PagedView::SetPageSwitchListener(
    /* [in] */ IPagedViewPageSwitchListener* pageSwitchListener)
{
    mPageSwitchListener = pageSwitchListener;
    if (mPageSwitchListener != NULL) {
        AutoPtr<IView> view;
        GetPageAt(mCurrentPage, (IView**)&view);
        mPageSwitchListener->OnPageSwitched(view, mCurrentPage);
    }
    return NOERROR;
}

ECode PagedView::SetDataIsReady()
{
    mIsDataReady = TRUE;
    return NOERROR;
}

ECode PagedView::IsDataReady(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mIsDataReady;
    return NOERROR;
}

ECode PagedView::GetCurrentPage(
    /* [out] */ Int32* page)
{
    VALIDATE_NOT_NULL(page)

    *page = mCurrentPage;
    return NOERROR;
}

ECode PagedView::GetNextPage(
    /* [out] */ Int32* page)
{
    VALIDATE_NOT_NULL(page)

    *page = (mNextPage != INVALID_PAGE) ? mNextPage : mCurrentPage;
    return NOERROR;
}

ECode PagedView::GetPageCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)

    return GetChildCount(count);
}

ECode PagedView::GetPageAt(
    /* [in] */ Int32 index,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    return GetChildAt(index, view);
}

ECode PagedView::IndexToPage(
    /* [in] */ Int32 index,
    /* [out] */ Int32* page)
{
    VALIDATE_NOT_NULL(page)

    *page = index;
    return NOERROR;
}

ECode PagedView::UpdateCurrentPageScroll()
{
    Int32 offset;
    GetChildOffset(mCurrentPage, &offset);
    Int32 relOffset;
    GetRelativeChildOffset(mCurrentPage, &relOffset);
    Int32 newX = offset - relOffset;
    ScrollTo(newX, 0);
    mScroller->SetFinalX(newX);
    return mScroller->ForceFinished(TRUE);
}

ECode PagedView::SetCurrentPage(
    /* [in] */ Int32 currentPage)
{
    NotifyPageSwitching(currentPage);

    Boolean res;
    if (mScroller->IsFinished(&res), !res) {
        mScroller->AbortAnimation();
    }
    // don't introduce any checks like mCurrentPage == currentPage here-- if we change the
    // the default
    Int32 count;
    if ((GetChildCount(&count), count) == 0) {
        return NOERROR;
    }

    mForceScreenScrolled = TRUE;
    GetPageCount(&count);
    mCurrentPage = Elastos::Core::Math::Max(0,
            Elastos::Core::Math::Min(currentPage, count - 1));
    UpdateCurrentPageScroll();
    UpdateScrollingIndicator();
    NotifyPageSwitched();
    return Invalidate();
}

ECode PagedView::SetOnlyAllowEdgeSwipes(
    /* [in] */ Boolean enable)
{
    mOnlyAllowEdgeSwipes = enable;
    return NOERROR;
}

ECode PagedView::NotifyPageSwitching(
    /* [in] */ Int32 whichPage)
{
    if (mPageSwitchListener != NULL) {
        AutoPtr<IView> view;
        GetPageAt(whichPage, (IView**)&view);
        mPageSwitchListener->OnPageSwitching(view, whichPage);
    }
    return NOERROR;
}

ECode PagedView::NotifyPageSwitched()
{
    if (mPageSwitchListener != NULL) {
        AutoPtr<IView> view;
        GetPageAt(mCurrentPage, (IView**)&view);
        mPageSwitchListener->OnPageSwitched(view, mCurrentPage);
    }
    return NOERROR;
}

ECode PagedView::PageBeginMoving()
{
    if (DEBUG_WARP) Logger::V(TAG, "pageBeginMoving(%d)", mIsPageMoving);
    if (!mIsPageMoving) {
        mIsPageMoving = TRUE;
        Boolean res;
        if (IsWarping(&res), res) {
            DispatchOnPageBeginWarp();
        }
        OnPageBeginMoving();
    }
    return NOERROR;
}

void PagedView::DispatchOnPageBeginWarp()
{
    if (!mOnPageBeginWarpCalled) {
        OnPageBeginWarp();
        mOnPageBeginWarpCalled = TRUE;
    }
    mOnPageEndWarpCalled = FALSE;
}

void PagedView::DispatchOnPageEndWarp()
{
    if (!mOnPageEndWarpCalled) {
        OnPageEndWarp();
        mOnPageEndWarpCalled = TRUE;
    }
    mOnPageBeginWarpCalled = FALSE;
}

ECode PagedView::PageEndMoving()
{
    if (DEBUG_WARP) Logger::V(TAG, "pageEndMoving(%d)", mIsPageMoving);
    if (mIsPageMoving) {
        mIsPageMoving = FALSE;

        Boolean res;
        if (IsWarping(&res), res) {
            DispatchOnPageEndWarp();
            mWarpPageExposed = FALSE;
        }
        OnPageEndMoving();
    }
    return NOERROR;
}

ECode PagedView::IsPageMoving(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mIsPageMoving;
    return NOERROR;
}

ECode PagedView::OnPageBeginMoving()
{
    return NOERROR;
}

ECode PagedView::OnPageEndMoving()
{
    return NOERROR;
}

ECode PagedView::SetOnLongClickListener(
    /* [in] */ IViewOnLongClickListener* l)
{
    mLongClickListener = l;
    Int32 count;
    GetPageCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> view;
        GetPageAt(i, (IView**)&view);
        view->SetOnLongClickListener(l);
    }
    return NOERROR;
}

ECode PagedView::ScrollBy(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 _y;
    GetScrollY(&_y);
    return ScrollTo(mUnboundedScrollX + x, _y + y);
}

ECode PagedView::ScrollTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    mUnboundedScrollX = x;

    if (x < 0) {
        ViewGroup::ScrollTo(0, y);
        if (mAllowOverScroll) {
            OverScroll(x);
        }
    }
    else if (x > mMaxScrollX) {
        ViewGroup::ScrollTo(mMaxScrollX, y);
        if (mAllowOverScroll) {
            OverScroll(x - mMaxScrollX);
        }
    }
    else {
        mOverScrollX = x;
        ViewGroup::ScrollTo(x, y);
    }

    mTouchX = x;
    AutoPtr<ISystem> helper;
    CSystem::AcquireSingleton((ISystem**)&helper);
    Int64 time;
    helper->GetNanoTime(&time);
    mSmoothingTime = time / NANOTIME_DIV;

    // Update the last motion events when scrolling
    Boolean res;
    if (IsReordering(TRUE, &res), res) {
        AutoPtr<ArrayOf<Float> > p;
        MapPointFromParentToView(this, mParentDownMotionX, mParentDownMotionY, (ArrayOf<Float>**)&p);
        mLastMotionX = (*p)[0];
        mLastMotionY = (*p)[1];
        UpdateDragViewTranslationDuringDrag();
    }
    return NOERROR;
}

ECode PagedView::ComputeScrollHelper(
        /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    mScroller->ComputeScrollOffset(&res);
    if (res) {
        // Don't bother scrolling if the page does not need to be moved
        Int32 sx,cx,sy,cy;
        if ((GetScrollX(&sx), sx) != (mScroller->GetCurrX(&cx), cx)
            || (GetScrollY(&sy), sy) != (mScroller->GetCurrY(&cy), cy)
            || mOverScrollX != (mScroller->GetCurrX(&cx), cx)) {

            ScrollTo((mScroller->GetCurrX(&cx), cx), (mScroller->GetCurrY(&cy), cy));
        }
        Invalidate();
        *result = TRUE;
        return NOERROR;
    }
    else if (mNextPage != INVALID_PAGE) {
        Int32 count;
        GetPageCount(&count);
        mCurrentPage = Elastos::Core::Math::Max(0,
                Elastos::Core::Math::Min(mNextPage, count - 1));
        mNextPage = INVALID_PAGE;
        NotifyPageSwitched();

        // We don't want to trigger a page end moving unless the page has settled
        // and the user has stopped scrolling
        if (mTouchState == TOUCH_STATE_REST) {
            PageEndMoving();
        }

        OnPostReorderingAnimationCompleted();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode PagedView::ComputeScroll()
{
    Boolean res;
    return ComputeScrollHelper(&res);
}

ECode PagedView::ShouldSetTopAlignedPivotForWidget(
    /* [in] */ Int32 childIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTopAlignPageWhenShrinkingForBouncer;
    return NOERROR;
}

ECode PagedView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 count;
    GetChildCount(&count);
    if (!mIsDataReady || count == 0) {
        return ViewGroup::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    }

    // We measure the dimensions of the PagedView to be larger than the pages so that when we
    // zoom out (and scale down), the view is still contained in the parent
    AutoPtr<IViewParent> p;
    GetParent((IViewParent**)&p);
    AutoPtr<IView> parent = IView::Probe(p);
    Int32 widthMode = MeasureSpec::GetMode(widthMeasureSpec);
    Int32 widthSize = MeasureSpec::GetSize(widthMeasureSpec);
    Int32 heightMode = MeasureSpec::GetMode(heightMeasureSpec);
    Int32 heightSize = MeasureSpec::GetSize(heightMeasureSpec);
    // NOTE: We multiply by 1.5f to account for the fact that depending on the offset of the
    // viewport, we can be at most one and a half screens offset once we scale down
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> dm;
    resources->GetDisplayMetrics((IDisplayMetrics**)&dm);

    Int32 wp;
    dm->GetWidthPixels(&wp);
    Int32 hp;
    dm->GetHeightPixels(&hp);
    Int32 maxSize = Elastos::Core::Math::Max(wp, hp);
    Int32 parentWidthSize = (Int32) (1.5f * maxSize);
    Int32 parentHeightSize = maxSize;
    Int32 scaledWidthSize = (Int32) (parentWidthSize / mMinScale);
    Int32 scaledHeightSize = (Int32) (parentHeightSize / mMinScale);
    mViewport->Set(0, 0, widthSize, heightSize);

    if (widthMode == MeasureSpec::UNSPECIFIED || heightMode == MeasureSpec::UNSPECIFIED) {
        return ViewGroup::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    }

    // Return early if we aren't given a proper dimension
    if (widthSize <= 0 || heightSize <= 0) {
        return ViewGroup::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    }

    /* Allow the height to be set as WRAP_CONTENT. This allows the particular case
     * of the All apps view on XLarge displays to not take up more space then it needs. Width
     * is still not allowed to be set as WRAP_CONTENT since many parts of the code expect
     * each page to have the same width.
     */
    Int32 top;
    GetPaddingTop(&top);
    Int32 bottom;
    GetPaddingBottom(&bottom);
    Int32 verticalPadding = top + bottom;

    Int32 left;
    GetPaddingLeft(&left);
    Int32 right;
    GetPaddingRight(&right);
    Int32 horizontalPadding = left + right;

    // The children are given the same width and height as the workspace
    // unless they were set to WRAP_CONTENT
    if (DEBUG) Logger::D(TAG, "PagedView.onMeasure(): %d, %d", widthSize, heightSize);
    if (DEBUG) Logger::D(TAG, "PagedView.scaledSize: %d, %d", scaledWidthSize, scaledHeightSize);
    if (DEBUG) Logger::D(TAG, "PagedView.parentSize: %d, %d", parentWidthSize, parentHeightSize);
    if (DEBUG) Logger::D(TAG, "PagedView.horizontalPadding: %d", horizontalPadding);
    if (DEBUG) Logger::D(TAG, "PagedView.verticalPadding: %d", verticalPadding);
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        // disallowing padding in paged view (just pass 0)
        AutoPtr<IView> child;
        GetPageAt(i, (IView**)&child);

        AutoPtr<IViewGroupLayoutParams> params;
        child->GetLayoutParams((IViewGroupLayoutParams**)&params);
        AutoPtr<ViewGroup::LayoutParams> lp = (ViewGroup::LayoutParams*)params.Get();

        Int32 childWidthMode;
        if (lp->mWidth == IViewGroupLayoutParams::WRAP_CONTENT) {
            childWidthMode = MeasureSpec::AT_MOST;
        }
        else {
            childWidthMode = MeasureSpec::EXACTLY;
        }

        Int32 childHeightMode;
        if (lp->mHeight == IViewGroupLayoutParams::WRAP_CONTENT) {
            childHeightMode = MeasureSpec::AT_MOST;
        }
        else {
            childHeightMode = MeasureSpec::EXACTLY;
        }

        Int32 childWidthMeasureSpec =
            MeasureSpec::MakeMeasureSpec(widthSize - horizontalPadding, childWidthMode);
        Int32 childHeightMeasureSpec =
            MeasureSpec::MakeMeasureSpec(heightSize - verticalPadding, childHeightMode);

        child->Measure(childWidthMeasureSpec, childHeightMeasureSpec);
    }
    SetMeasuredDimension(scaledWidthSize, scaledHeightSize);

    // We can't call getChildOffset/getRelativeChildOffset until we set the measured dimensions.
    // We also wait until we set the measured dimensions before flushing the cache as well, to
    // ensure that the cache is filled with good values.
    InvalidateCachedOffsets();

    GetChildCount(&count);
    if (mChildCountOnLastMeasure != count && !mDeferringForDelete) {
        SetCurrentPage(mCurrentPage);
    }
    GetChildCount(&mChildCountOnLastMeasure);

    if (childCount > 0) {
        // if (DEBUG) Logger::D(TAG, "getRelativeChildOffset(): " + getViewportWidth() + ", "
        //         + getChildWidth(0));

        // Calculate the variable page spacing if necessary
        if (mPageSpacing == AUTOMATIC_PAGE_SPACING) {
            // The gap between pages in the PagedView should be equal to the gap from the page
            // to the edge of the screen (so it is not visible in the current screen).  To
            // account for unequal padding on each side of the paged view, we take the maximum
            // of the left/right gap and use that as the gap between each page.
            Int32 offset;
            GetRelativeChildOffset(0, &offset);
            AutoPtr<IView> view;
            GetChildAt(0, (IView**)&view);
            Int32 width;
            view->GetMeasuredWidth(&width);
            Int32 spacing = Elastos::Core::Math::Max(offset, widthSize - offset - width);
            SetPageSpacing(spacing);
        }
    }

    UpdateScrollingIndicatorPosition();

    if (childCount > 0) {
        Int32 coffset;
        GetChildOffset(childCount - 1, &coffset);
        Int32 roffset;
        GetRelativeChildOffset(childCount - 1, &roffset);
        mMaxScrollX = coffset - roffset;
    }
    else {
        mMaxScrollX = 0;
    }
    return NOERROR;
}

ECode PagedView::SetPageSpacing(
    /* [in] */ Int32 pageSpacing)
{
    mPageSpacing = pageSpacing;
    return InvalidateCachedOffsets();
}

ECode PagedView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    Int32 count;
    GetChildCount(&count);
    if (!mIsDataReady || count == 0) {
        return NOERROR;
    }

    if (DEBUG) Logger::D(TAG, "PagedView.onLayout()");
    Int32 childCount;
    GetChildCount(&childCount);

    Int32 offsetX;
    GetViewportOffsetX(&offsetX);
    Int32 offsetY;
    GetViewportOffsetY(&offsetY);

    // Update the viewport offsets
    mViewport->Offset(offsetX,  offsetY);

    Int32 offset;
    GetRelativeChildOffset(0, &offset);
    Int32 childLeft = offsetX + offset;
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetPageAt(i, (IView**)&child);
        Int32 ptop;
        GetPaddingTop(&ptop);
        Int32 childTop = offsetY + ptop;

        Int32 visibility;
        child->GetVisibility(&visibility);
        if (visibility != IView::GONE) {
            Int32 childWidth;
            GetScaledMeasuredWidth(child, &childWidth);
            Int32 childHeight;
            child->GetMeasuredHeight(&childHeight);

            if (DEBUG) Logger::D(TAG, "\tlayout-child %d: %d, %d", i, childLeft, childTop);
            Int32 mwidth;
            child->GetMeasuredWidth(&mwidth);
            child->Layout(childLeft, childTop,
                    childLeft + mwidth, childTop + childHeight);
            childLeft += childWidth + mPageSpacing;
        }
    }

    GetChildCount(&count);
    if (mFirstLayout && mCurrentPage >= 0 && mCurrentPage < count) {
        SetHorizontalScrollBarEnabled(FALSE);
        UpdateCurrentPageScroll();
        SetHorizontalScrollBarEnabled(TRUE);
        mFirstLayout = FALSE;
    }
    return NOERROR;
}

ECode PagedView::ScreenScrolled(
    /* [in] */ Int32 screenCenter)
{
    return NOERROR;
}

ECode PagedView::OnChildViewAdded(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    // This ensures that when children are added, they get the correct transforms / alphas
    // in accordance with any scroll effects.
    mForceScreenScrolled = TRUE;
    Invalidate();
    return InvalidateCachedOffsets();
}

ECode PagedView::OnChildViewRemoved(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    mForceScreenScrolled = TRUE;
    Invalidate();
    return InvalidateCachedOffsets();
}

ECode PagedView::InvalidateCachedOffsets()
{
    Int32 count;
    GetChildCount(&count);
    if (count == 0) {
        mChildOffsets = NULL;
        mChildRelativeOffsets = NULL;
        mChildOffsetsWithLayoutScale = NULL;
        return NOERROR;
    }

    mChildOffsets = ArrayOf<Int32>::Alloc(count);
    mChildRelativeOffsets = ArrayOf<Int32>::Alloc(count);
    mChildOffsetsWithLayoutScale = ArrayOf<Int32>::Alloc(count);
    for (Int32 i = 0; i < count; i++) {
        (*mChildOffsets)[i] = -1;
        (*mChildRelativeOffsets)[i] = -1;
        (*mChildOffsetsWithLayoutScale)[i] = -1;
    }
    return NOERROR;
}

ECode PagedView::GetChildOffset(
    /* [in] */ Int32 index,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset)

    Int32 count;
    GetChildCount(&count);
    if (index < 0 || index > count - 1) {
        *offset = 0;
        return NOERROR;
    }

    AutoPtr<ArrayOf<Int32> > childOffsets;
    if (mLayoutScale == 1.0f) {
        childOffsets = mChildOffsets;
    }
    else {
        childOffsets = mChildOffsetsWithLayoutScale;
    }

    if (childOffsets != NULL && (*childOffsets)[index] != -1) {
        *offset = (*childOffsets)[index];
        return NOERROR;
    }
    else {
        GetChildCount(&count);
        if (count == 0) {
            *offset = 0;
            return NOERROR;
        }

        Int32 _offset;
        GetRelativeChildOffset(0, &_offset);
        for (Int32 i = 0; i < index; ++i) {
            AutoPtr<IView> view;
            GetPageAt(i, (IView**)&view);
            Int32 width;
            GetScaledMeasuredWidth(view, &width);
            _offset += width + mPageSpacing;
        }
        if (childOffsets != NULL) {
            (*childOffsets)[index] = _offset;
        }
        *offset = _offset;
        return NOERROR;
    }
}

ECode PagedView::GetRelativeChildOffset(
    /* [in] */ Int32 index,
    /* [out] */ Int32* outoffset)
{
    VALIDATE_NOT_NULL(outoffset)

    Int32 count;
    if (index < 0 || index > (GetChildCount(&count), count) - 1) {
        *outoffset = 0;
        return NOERROR;
    }

    if (mChildRelativeOffsets != NULL && (*mChildRelativeOffsets)[index] != -1) {
        *outoffset = (*mChildRelativeOffsets)[index];
        return NOERROR;
    }
    else {
        Int32 pleft;
        GetPaddingLeft(&pleft);
        Int32 pright;
        GetPaddingRight(&pright);
        Int32 padding = pleft + pright;

        Int32 vwidth;
        GetViewportWidth(&vwidth);
        Int32 cwidth;
        GetChildWidth(index, &cwidth);
        Int32 offset = pleft +
                (vwidth - padding - cwidth) / 2;
        if (mChildRelativeOffsets != NULL) {
            (*mChildRelativeOffsets)[index] = offset;
        }
        *outoffset = offset;
        return NOERROR;
    }
}

ECode PagedView::GetScaledMeasuredWidth(
    /* [in] */ IView* child,
    /* [out] */ Int32* outwidth)
{
    VALIDATE_NOT_NULL(outwidth)

    // This functions are called enough times that it actually makes a difference in the
    // profiler -- so just inline the max() here
    Int32 measuredWidth;
    child->GetMeasuredWidth(&measuredWidth);
    Int32 minWidth = mMinimumWidth;
    Int32 maxWidth = (minWidth > measuredWidth) ? minWidth : measuredWidth;
    *outwidth = (Int32)(maxWidth * mLayoutScale + 0.5f);
    return NOERROR;
}

ECode PagedView::BoundByReorderablePages(
    /* [in] */ Boolean isReordering,
    /* [in] */ ArrayOf<Int32>* range)
{
    // Do nothing
    return NOERROR;
}

ECode PagedView::GetVisiblePages(
    /* [in] */ ArrayOf<Int32>* range)
{
    (*range)[0] = 0;
    Int32 count;
    GetPageCount(&count);
    (*range)[1] = count - 1;
    return NOERROR;

    /*
    final int pageCount = getChildCount();

    if (pageCount > 0) {
        final int screenWidth = getViewportWidth();
        int leftScreen = 0;
        int rightScreen = 0;
        int offsetX = getViewportOffsetX() + getScrollX();
        View currPage = getPageAt(leftScreen);
        while (leftScreen < pageCount - 1 &&
                currPage.getX() + currPage.getWidth() -
                currPage.getPaddingRight() < offsetX) {
            leftScreen++;
            currPage = getPageAt(leftScreen);
        }
        rightScreen = leftScreen;
        currPage = getPageAt(rightScreen + 1);
        while (rightScreen < pageCount - 1 &&
                currPage.getX() - currPage.getPaddingLeft() < offsetX + screenWidth) {
            rightScreen++;
            currPage = getPageAt(rightScreen + 1);
        }

        // TEMP: this is a hacky way to ensure that animations to new pages are not clipped
        // because we don't draw them while scrolling?
        range[0] = Math.max(0, leftScreen - 1);
        range[1] = Math.min(rightScreen + 1, getChildCount() - 1);
    } else {
        range[0] = -1;
        range[1] = -1;
    }
    */
}

ECode PagedView::ShouldDrawChild(
    /* [in] */ IView* child,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Float alpha;
    child->GetAlpha(&alpha);
    *result = alpha > 0;
    return NOERROR;
}

ECode PagedView::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    Int32 vwidth;
    GetViewportWidth(&vwidth);
    Int32 halfScreenSize = vwidth / 2;
    // mOverScrollX is equal to getScrollX() when we're within the normal scroll range.
    // Otherwise it is equal to the scaled overscroll position.
    Int32 screenCenter = mOverScrollX + halfScreenSize;

    if (screenCenter != mLastScreenCenter || mForceScreenScrolled) {
        // set mForceScreenScrolled before calling screenScrolled so that screenScrolled can
        // set it for the next frame
        mForceScreenScrolled = FALSE;
        ScreenScrolled(screenCenter);
        mLastScreenCenter = screenCenter;
    }

    // Find out which screens are visible; as an optimization we only call draw on them
    Int32 pageCount;
    GetChildCount(&pageCount);
    if (pageCount > 0) {
        GetVisiblePages(mTempVisiblePagesRange);
        Int32 leftScreen = (*mTempVisiblePagesRange)[0];
        Int32 rightScreen = (*mTempVisiblePagesRange)[1];
        if (leftScreen != -1 && rightScreen != -1) {
            Int64 drawingTime;
            GetDrawingTime(&drawingTime);
            // Clip to the bounds
            Int32 tmp;
            canvas->Save(&tmp);

            Int32 sx;
            GetScrollX(&sx);
            Int32 sy;
            GetScrollY(&sy);
            Int32 right;
            GetRight(&right);
            Int32 left;
            GetLeft(&left);
            Int32 bottom;
            GetBottom(&bottom);
            Int32 top;
            GetTop(&top);
            Boolean res;
            canvas->ClipRect(sx, sy, sx + right - left,
                    sy + bottom - top, &res);

            // Draw all the children, leaving the drag view for last
            for (Int32 i = pageCount - 1; i >= 0; i--) {
                AutoPtr<IView> v;
                GetPageAt(i, (IView**)&v);
                if (TO_IINTERFACE(v) == TO_IINTERFACE(mDragView)) continue;

                Boolean res;
                if (mForceDrawAllChildrenNextFrame ||
                           (leftScreen <= i && i <= rightScreen && (ShouldDrawChild(v, &res), res))) {
                    DrawChild(canvas, v, drawingTime);
                }
            }
            // Draw the drag view on top (if there is one)
            if (mDragView != NULL) {
                DrawChild(canvas, mDragView, drawingTime);
            }

            mForceDrawAllChildrenNextFrame = FALSE;
            canvas->Restore();
        }
    }
    return NOERROR;
}

ECode PagedView::RequestChildRectangleOnScreen(
    /* [in] */ IView* child,
    /* [in] */ IRect* rectangle,
    /* [in] */ Boolean immediate,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 index;
    IndexOfChild(child, &index);
    Int32 page;
    IndexToPage(index, &page);

    Boolean res;
    if (page != mCurrentPage || (mScroller->IsFinished(&res), !res)) {
        SnapToPage(page);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode PagedView::OnRequestFocusInDescendants(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 focusablePage;
    if (mNextPage != INVALID_PAGE) {
        focusablePage = mNextPage;
    }
    else {
        focusablePage = mCurrentPage;
    }
    AutoPtr<IView> v;
    GetPageAt(focusablePage, (IView**)&v);
    if (v != NULL) {
        return v->RequestFocus(direction, previouslyFocusedRect, result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode PagedView::DispatchUnhandledMove(
    /* [in] */ IView* focused,
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (direction == IView::FOCUS_LEFT) {
        Int32 page;
        GetCurrentPage(&page);
        if (page > 0) {
            SnapToPage(page - 1);
            *result = TRUE;
            return NOERROR;
        }
    }
    else if (direction == IView::FOCUS_RIGHT) {
        Int32 page;
        GetCurrentPage(&page);
        Int32 count;
        GetPageCount(&count);
        if (page < count - 1) {
            SnapToPage(page + 1);
            *result = TRUE;
            return NOERROR;
        }
    }
    return ViewGroup::DispatchUnhandledMove(focused, direction, result);
}

ECode PagedView::AddFocusables(
    /* [in] */ IArrayList* views,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 focusableMode)
{
    Int32 count;
    GetPageCount(&count);
    if (mCurrentPage >= 0 && mCurrentPage < count) {
        AutoPtr<IView> view;
        GetPageAt(mCurrentPage, (IView**)&view);
        view->AddFocusables(views, direction, focusableMode);
    }
    if (direction == IView::FOCUS_LEFT) {
        if (mCurrentPage > 0) {
            AutoPtr<IView> view;
            GetPageAt(mCurrentPage - 1, (IView**)&view);
            view->AddFocusables(views, direction, focusableMode);
        }
    }
    else if (direction == IView::FOCUS_RIGHT){
        GetPageCount(&count);
        if (mCurrentPage < count - 1) {
            AutoPtr<IView> view;
            GetPageAt(mCurrentPage + 1, (IView**)&view);
            view->AddFocusables(views, direction, focusableMode);
        }
    }
    return NOERROR;
}

ECode PagedView::FocusableViewAvailable(
    /* [in] */ IView* focused)
{
    AutoPtr<IView> current;
    GetPageAt(mCurrentPage, (IView**)&current);
    AutoPtr<IView> v = focused;
    while (TRUE) {
        if (TO_IINTERFACE(v) == TO_IINTERFACE(current)) {
            ViewGroup::FocusableViewAvailable(focused);
            return NOERROR;
        }
        if (TO_IINTERFACE(v) == TO_IINTERFACE(this)) {
            return NOERROR;
        }
        AutoPtr<IViewParent> parent;
        v->GetParent((IViewParent**)&parent);
        if (IView::Probe(parent) != NULL) {
            v = IView::Probe(parent);
        }
        else {
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode PagedView::HitsPreviousPage(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 offsetX;
    GetViewportOffsetX(&offsetX);
    Int32 offset;
    GetRelativeChildOffset(mCurrentPage, &offset);
    *result = (x < offsetX + offset - mPageSpacing);
    return NOERROR;
}

ECode PagedView::HitsNextPage(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 offsetX;
    GetViewportOffsetX(&offsetX);
    Int32 vwidth;
    GetViewportWidth(&vwidth);
    Int32 offset;
    GetRelativeChildOffset(mCurrentPage, &offset);
    *result =  (x > (offsetX + vwidth - offset + mPageSpacing));
    return NOERROR;
}

Boolean PagedView::IsTouchPointInViewportWithBuffer(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 width;
    mViewport->GetWidth(&width);
    Int32 left;
    mViewport->GetLeft(&left);
    Int32 top;
    mViewport->GetTop(&top);
    Int32 right;
    mViewport->GetRight(&right);
    Int32 bottom;
    mViewport->GetBottom(&bottom);
    mTmpRect->Set(left - width / 2, top,
            right + width / 2, bottom);
    Boolean res;
    mTmpRect->Contains(x, y, &res);
    return res;
}

Boolean PagedView::IsTouchPointInCurrentPage(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 page;
    GetCurrentPage(&page);
    AutoPtr<IView> v;
    GetPageAt(page, (IView**)&v);
    if (v != NULL) {
        Int32 left;
        v->GetLeft(&left);
        Int32 scrollX;
        GetScrollX(&scrollX);
        Int32 right;
        v->GetRight(&right);
        Int32 bottom;
        v->GetBottom(&bottom);
        mTmpRect->Set((left - scrollX), 0, (right - scrollX), bottom);

        Boolean res;
        mTmpRect->Contains(x, y, &res);
        return res;
    }
    return FALSE;
}

ECode PagedView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DISABLE_TOUCH_INTERACTION) {
        *result = FALSE;
        return NOERROR;
    }

    /*
     * This method JUST determines whether we want to intercept the motion.
     * If we return true, onTouchEvent will be called and we do the actual
     * scrolling there.
     */
    AcquireVelocityTrackerAndAddMovement(ev);

    // Skip touch handling if there are no pages to swipe
    Int32 count;
    GetChildCount(&count);
    if (count <= 0) return ViewGroup::OnInterceptTouchEvent(ev, result);

    /*
     * Shortcut the most recurring case: the user is in the dragging
     * state and he is moving his finger.  We want to intercept this
     * motion.
     */
    Int32 action;
    ev->GetAction(&action);
    if ((action == IMotionEvent::ACTION_MOVE) &&
            (mTouchState == TOUCH_STATE_SCROLLING)) {
        *result = TRUE;
        return NOERROR;
    }

    switch (action & IMotionEvent::ACTION_MASK) {
        case IMotionEvent::ACTION_MOVE: {
            /*
             * mIsBeingDragged == false, otherwise the shortcut would have caught it. Check
             * whether the user has moved far enough from his original down touch.
             */
            if (mActivePointerId != INVALID_POINTER) {
                Boolean res;
                if (mIsCameraEvent || (DetermineScrollingStart(ev, &res), res)) {
                    StartScrolling(ev);
                }
                break;
            }
            // if mActivePointerId is INVALID_POINTER, then we must have missed an ACTION_DOWN
            // event. in that case, treat the first occurence of a move event as a ACTION_DOWN
            // i.e. fall through to the next case (don't break)
            // (We sometimes miss ACTION_DOWN events in Workspace because it ignores all events
            // while it's small- this was causing a crash before we checked for INVALID_POINTER)

            break;
        }

        case IMotionEvent::ACTION_DOWN: {
            if (mIsCameraEvent) {
                AnimateWarpPageOnScreen(String("interceptTouch(): DOWN"));
            }
            // Remember where the motion event started
            SaveDownState(ev);

            /*
             * If being flinged and user touches the screen, initiate drag;
             * otherwise don't.  mScroller.isFinished should be false when
             * being flinged.
             */
            Int32 finalX;
            mScroller->GetFinalX(&finalX);
            Int32 currX;
            mScroller->GetCurrX(&currX);
            Int32 xDist = Elastos::Core::Math::Abs(finalX - currX);
            Boolean res;
            mScroller->IsFinished(&res);
            Boolean finishedScrolling = (res || xDist < mTouchSlop);
            if (finishedScrolling) {
                SetTouchState(TOUCH_STATE_REST);
                mScroller->AbortAnimation();
            }
            else {
                if (mIsCameraEvent || IsTouchPointInViewportWithBuffer(
                        (Int32)mDownMotionX, (Int32)mDownMotionY)) {
                    SetTouchState(TOUCH_STATE_SCROLLING);
                }
                else {
                    SetTouchState(TOUCH_STATE_REST);
                }
            }

            // check if this can be the beginning of a tap on the side of the pages
            // to scroll the current page
            if (!DISABLE_TOUCH_SIDE_PAGES) {
                if (mTouchState != TOUCH_STATE_PREV_PAGE
                        && mTouchState != TOUCH_STATE_NEXT_PAGE) {
                    Int32 count;
                    GetChildCount(&count);
                    if (count > 0) {
                        Float x;
                        ev->GetX(&x);
                        Float y;
                        ev->GetY(&y);
                        Boolean res;
                        if (HitsPreviousPage(x, y, &res), res) {
                            SetTouchState(TOUCH_STATE_PREV_PAGE);
                        }
                        else if (HitsNextPage(x, y, &res), res) {
                            SetTouchState(TOUCH_STATE_NEXT_PAGE);
                        }
                    }
                }
            }
            break;
        }

        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
        {
            ResetTouchState();
            // Just intercept the touch event on up if we tap outside the strict viewport
            if (!IsTouchPointInCurrentPage((Int32)mLastMotionX, (Int32)mLastMotionY)) {
                *result = TRUE;
                return NOERROR;
            }
            break;
        }
        case IMotionEvent::ACTION_POINTER_UP:
            OnSecondaryPointerUp(ev);
            ReleaseVelocityTracker();
            break;
    }

    /*
     * The only time we want to intercept motion events is if we are in the
     * drag mode.
     */
    *result = mTouchState != TOUCH_STATE_REST;
    return NOERROR;
}

void PagedView::SetTouchState(
    /* [in] */ Int32 touchState)
{
    if (mTouchState != touchState) {
        if (DEBUG_WARP) Logger::V(TAG, "mTouchState changing to %d", touchState);
        OnTouchStateChanged(touchState);
        mTouchState = touchState;
    }
}

ECode PagedView::OnTouchStateChanged(
    /* [in] */ Int32 newTouchState)
{
    if (DEBUG) {
        Logger::V(TAG, "onTouchStateChanged(old=%d, new=%d)", mTouchState, newTouchState);
    }
    return NOERROR;
}

void PagedView::SaveDownState(
    /* [in] */ IMotionEvent* ev)
{
    // Remember where the motion event started
    ev->GetX(&mLastMotionX);
    mDownMotionX = mLastMotionX;
    ev->GetY(&mLastMotionY);
    mDownMotionY = mLastMotionY;
    Int32 tmp;
    GetScrollX(&tmp);
    mDownScrollX = tmp;

    AutoPtr<ArrayOf<Float> > p;
    MapPointFromViewToParent(this, mLastMotionX, mLastMotionY, (ArrayOf<Float>**)&p);
    mParentDownMotionX = (*p)[0];
    mParentDownMotionY = (*p)[1];
    mLastMotionXRemainder = 0;
    mTotalMotionX = 0;
    ev->GetPointerId(0, &mActivePointerId);

    // Determine if the down event is within the threshold to be an edge swipe
    Int32 offsetX;
    GetViewportOffsetX(&offsetX);
    Int32 leftEdgeBoundary = offsetX + mEdgeSwipeRegionSize;
    Int32 width;
    GetMeasuredWidth(&width);
    Int32 rightEdgeBoundary = width - offsetX - mEdgeSwipeRegionSize;
    if ((mDownMotionX <= leftEdgeBoundary || mDownMotionX >= rightEdgeBoundary)) {
        mDownEventOnEdge = TRUE;
    }
}

Boolean PagedView::IsHorizontalCameraScroll(
    /* [in] */ IMotionEvent* ev)
{
    // Disallow scrolling if we don't have a valid pointer index
    Int32 pointerIndex;
    ev->FindPointerIndex(mActivePointerId, &pointerIndex);
    if (pointerIndex == -1) return FALSE;

    // If we're only allowing edge swipes, we break out early if the down event wasn't
    // at the edge.
    if (mOnlyAllowEdgeSwipes && !mDownEventOnEdge) return FALSE;

    Float x;
    ev->GetX(pointerIndex, &x);
    Int32 xDiff = (Int32)Elastos::Core::Math::Abs(x - mDownMotionX);

    Int32 touchSlop = Elastos::Core::Math::Round(TOUCH_SLOP_SCALE * mTouchSlop);
    Boolean xPaged = xDiff > mPagingTouchSlop;
    Boolean xMoved = xDiff > touchSlop;

    return mIsCameraEvent && (mUsePagingTouchSlop ? xPaged : xMoved);
}

ECode PagedView::DetermineScrollingStart(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Disallow scrolling if we don't have a valid pointer index
    Int32 pointerIndex;
    ev->FindPointerIndex(mActivePointerId, &pointerIndex);
    if (pointerIndex == -1) {
        *result = FALSE;
        return NOERROR;
    }

    // Disallow scrolling if we started the gesture from outside the viewport
    Float x;
    ev->GetX(pointerIndex, &x);
    Float y;
    ev->GetY(pointerIndex, &y);
    if (!IsTouchPointInViewportWithBuffer((Int32)x, (Int32)y)) {
        *result = FALSE;
        return NOERROR;
    }

    // If we're only allowing edge swipes, we break out early if the down event wasn't
    // at the edge.
    if (mOnlyAllowEdgeSwipes && !mDownEventOnEdge) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 xDiff = (Int32)Elastos::Core::Math::Abs(x - mLastMotionX);

    Int32 touchSlop = Elastos::Core::Math::Round(TOUCH_SLOP_SCALE * mTouchSlop);
    Boolean xPaged = xDiff > mPagingTouchSlop;
    Boolean xMoved = xDiff > touchSlop;

    *result = mUsePagingTouchSlop ? xPaged : xMoved;
    return NOERROR;
}

void PagedView::StartScrolling(
    /* [in] */ IMotionEvent* ev)
{
    // Ignore if we don't have a valid pointer index
    Int32 pointerIndex;
    ev->FindPointerIndex(mActivePointerId, &pointerIndex);
    if (pointerIndex == -1) return;

    Float x;
    ev->GetX(pointerIndex, &x);
    SetTouchState(TOUCH_STATE_SCROLLING);
    mTotalMotionX += Elastos::Core::Math::Abs(mLastMotionX - x);
    mLastMotionX = x;
    mLastMotionXRemainder = 0;

    Int32 offsetX;
    GetViewportOffsetX(&offsetX);
    Int32 scrollX;
    GetScrollX(&scrollX);
    mTouchX = offsetX + scrollX;

    AutoPtr<ISystem> helper;
    CSystem::AcquireSingleton((ISystem**)&helper);
    Int64 time;
    helper->GetNanoTime(&time);
    mSmoothingTime = time / NANOTIME_DIV;
    PageBeginMoving();
}

ECode PagedView::GetMaxScrollProgress(
    /* [out] */ Float* progress)
{
    VALIDATE_NOT_NULL(progress)

    *progress = 1.0f;
    return NOERROR;
}

ECode PagedView::GetBoundedScrollProgress(
    /* [in] */ Int32 screenCenter,
    /* [in] */ IView* v,
    /* [in] */ Int32 page,
    /* [out] */ Float* progress)
{
    VALIDATE_NOT_NULL(progress)

    Int32 width;
    GetViewportWidth(&width);
    Int32 halfScreenSize = width / 2;

    screenCenter = Elastos::Core::Math::Min(mScrollX + halfScreenSize, screenCenter);
    screenCenter = Elastos::Core::Math::Max(halfScreenSize,  screenCenter);

    return GetScrollProgress(screenCenter, v, page, progress);
}

ECode PagedView::GetScrollProgress(
    /* [in] */ Int32 screenCenter,
    /* [in] */ IView* v,
    /* [in] */ Int32 page,
    /* [out] */ Float* outprogress)
{
    VALIDATE_NOT_NULL(outprogress)

    Int32 width;
    GetViewportWidth(&width);
    Int32 halfScreenSize = width / 2;

    Int32 mwidth;
    GetScaledMeasuredWidth(v, &mwidth);
    Int32 totalDistance = mwidth + mPageSpacing;

    Int32 coffset;
    GetChildOffset(page, &coffset);
    Int32 rcoffset;
    GetRelativeChildOffset(page, &rcoffset);
    Int32 delta = screenCenter - (coffset -
            rcoffset + halfScreenSize);

    Float scrollProgress = delta / (totalDistance * 1.0f);
    Float progress;
    GetMaxScrollProgress(&progress);
    scrollProgress = Elastos::Core::Math::Min(scrollProgress, progress);
    scrollProgress = Elastos::Core::Math::Max(scrollProgress, - progress);
    *outprogress = scrollProgress;
    return NOERROR;
}

Float PagedView::OverScrollInfluenceCurve(
    /* [in] */ Float f)
{
    f -= 1.0f;
    return f * f * f + 1.0f;
}

ECode PagedView::AcceleratedOverScroll(
    /* [in] */ Float amount)
{
    Int32 screenSize;
    GetViewportWidth(&screenSize);

    // We want to reach the max over scroll effect when the user has
    // over scrolled half the size of the screen
    Float f = OVERSCROLL_ACCELERATE_FACTOR * (amount / screenSize);

    if (f == 0) return NOERROR;

    // Clamp this factor, f, to -1 < f < 1
    if (Elastos::Core::Math::Abs(f) >= 1) {
        f /= Elastos::Core::Math::Abs(f);
    }

    Int32 overScrollAmount = (Int32)Elastos::Core::Math::Round(f * screenSize);
    if (amount < 0) {
        mOverScrollX = overScrollAmount;
        Int32 y;
        GetScrollY(&y);
        ViewGroup::ScrollTo(0, y);
    }
    else {
        mOverScrollX = mMaxScrollX + overScrollAmount;
        Int32 y;
        GetScrollY(&y);
        ViewGroup::ScrollTo(mMaxScrollX, y);
    }
    return Invalidate();
}

ECode PagedView::DampedOverScroll(
        /* [in] */ Float amount)
{
    Int32 screenSize;
    GetViewportWidth(&screenSize);

    Float f = (amount / screenSize);

    if (f == 0) return NOERROR;
    f = f / (Elastos::Core::Math::Abs(f)) * (OverScrollInfluenceCurve(
            Elastos::Core::Math::Abs(f)));

    // Clamp this factor, f, to -1 < f < 1
    if (Elastos::Core::Math::Abs(f) >= 1) {
        f /= Elastos::Core::Math::Abs(f);
    }

    Int32 overScrollAmount = (Int32)Elastos::Core::Math::Round(OVERSCROLL_DAMP_FACTOR * f * screenSize);
    if (amount < 0) {
        mOverScrollX = overScrollAmount;
        Int32 y;
        GetScrollY(&y);
        ViewGroup::ScrollTo(0, y);
    } else {
        mOverScrollX = mMaxScrollX + overScrollAmount;
        Int32 y;
        GetScrollY(&y);
        ViewGroup::ScrollTo(mMaxScrollX, y);
    }
    return Invalidate();
}

ECode PagedView::OverScroll(
    /* [in] */ Float amount)
{
    return DampedOverScroll(amount);
}

ECode PagedView::MaxOverScroll(
    /* [out] */ Float* scroll)
{
    VALIDATE_NOT_NULL(scroll)

    // Using the formula in overScroll, assuming that f = 1.0 (which it should generally not
    // exceed). Used to find out how much extra wallpaper we need for the over scroll effect
    Float f = 1.0f;
    f = f / (Elastos::Core::Math::Abs(f)) * (OverScrollInfluenceCurve(
            Elastos::Core::Math::Abs(f)));
    *scroll = OVERSCROLL_DAMP_FACTOR * f;
    return NOERROR;
}

ECode PagedView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DISABLE_TOUCH_INTERACTION) {
        *result = FALSE;
        return NOERROR;
    }

    // Skip touch handling if there are no pages to swipe
    Int32 count;
    GetChildCount(&count);
    if (count <= 0) return ViewGroup::OnTouchEvent(ev, result);

    AcquireVelocityTrackerAndAddMovement(ev);

    Int32 action;
    ev->GetAction(&action);
    switch (action & IMotionEvent::ACTION_MASK) {
    case IMotionEvent::ACTION_DOWN:
    {
        /*
         * If being flinged and user touches, stop the fling. isFinished
         * will be false if being flinged.
         */
        Boolean res;
        mScroller->IsFinished(&res);
        if (!res) {
            mScroller->AbortAnimation();
        }

        // Remember where the motion event started
        SaveDownState(ev);

        if (mTouchState == TOUCH_STATE_SCROLLING) {
            PageBeginMoving();
        }
        else {
            SetTouchState(TOUCH_STATE_READY);
        }

        if (mIsCameraEvent) {
            AnimateWarpPageOnScreen(String("onTouch(): DOWN"));
        }
        break;
    }
    case IMotionEvent::ACTION_MOVE:
    {
        Boolean res;
        if (mTouchState == TOUCH_STATE_SCROLLING) {
            // Scroll to follow the motion event
            Int32 pointerIndex;
            ev->FindPointerIndex(mActivePointerId, &pointerIndex);

            if (pointerIndex == -1) {
                *result = TRUE;
                return NOERROR;
            }

            Float x;
            ev->GetX(pointerIndex, &x);
            Float deltaX = mLastMotionX + mLastMotionXRemainder - x;

            mTotalMotionX += Elastos::Core::Math::Abs(deltaX);

            // Only scroll and update mLastMotionX if we have moved some discrete amount.  We
            // keep the remainder because we are actually testing if we've moved from the last
            // scrolled position (which is discrete).
            if (Elastos::Core::Math::Abs(deltaX) >= 1.0f) {
                mTouchX += deltaX;
                AutoPtr<ISystem> helper;
                CSystem::AcquireSingleton((ISystem**)&helper);
                Int64 time;
                helper->GetNanoTime(&time);
                mSmoothingTime = time / NANOTIME_DIV;
                if (IsWarping(&res), res) {
                    Int32 index;
                    GetPageWarpIndex(&index);
                    AutoPtr<IView> view;
                    GetPageAt(index, (IView**)&view);
                    AutoPtr<IKeyguardWidgetFrame> v = IKeyguardWidgetFrame::Probe(view);
                    Float x;
                    IView::Probe(v)->GetTranslationX(&x);
                    IView::Probe(v)->SetTranslationX(x - deltaX);
                }
                else if (!mDeferScrollUpdate) {
                    ScrollBy((Int32)deltaX, 0);
                    if (DEBUG) Logger::D(TAG, "onTouchEvent().Scrolling: %f", deltaX);
                }
                else {
                    Invalidate();
                }
                mLastMotionX = x;
                mLastMotionXRemainder = deltaX - (Int32)deltaX;
            }
            else {
                AwakenScrollBars();
            }
        } else if (mTouchState == TOUCH_STATE_REORDERING) {
            // Update the last motion position
            ev->GetX(&mLastMotionX);
            ev->GetY(&mLastMotionY);

            // Update the parent down so that our zoom animations take this new movement into
            // account
            AutoPtr<ArrayOf<Float> > pt;
            MapPointFromViewToParent(this, mLastMotionX, mLastMotionY, (ArrayOf<Float>**)&pt);
            mParentDownMotionX = (*pt)[0];
            mParentDownMotionY = (*pt)[1];
            UpdateDragViewTranslationDuringDrag();

            // Find the closest page to the touch point
            Int32 dragViewIndex;
            IndexOfChild(mDragView, &dragViewIndex);
            Int32 vwidth;
            GetViewportWidth(&vwidth);
            Int32 bufferSize = (Int32)(REORDERING_SIDE_PAGE_BUFFER_PERCENTAGE * vwidth);

            Int32 vleft;
            mViewport->GetLeft(&vleft);
            AutoPtr<ArrayOf<Float> > leftBuffer;
            MapPointFromViewToParent(this, vleft, 0, (ArrayOf<Float>**)&leftBuffer);
            Int32 leftBufferEdge = (Int32)((*leftBuffer)[0] + bufferSize);

            Int32 vright;
            mViewport->GetRight(&vright);
            AutoPtr<ArrayOf<Float> > rightBuffer;
            MapPointFromViewToParent(this, vright, 0, (ArrayOf<Float>**)&rightBuffer);
            Int32 rightBufferEdge = (Int32)((*rightBuffer)[0] - bufferSize);

            // Change the drag view if we are hovering over the drop target
            Boolean isHoveringOverDelete = IsHoveringOverDeleteDropTarget(
                    (Int32)mParentDownMotionX, (Int32)mParentDownMotionY);
            SetPageHoveringOverDeleteDropTarget(dragViewIndex, isHoveringOverDelete);

            if (DEBUG) Logger::D(TAG, "leftBufferEdge: %d", leftBufferEdge);
            if (DEBUG) Logger::D(TAG, "rightBufferEdge: %d", rightBufferEdge);
            if (DEBUG) Logger::D(TAG, "mLastMotionX: %d", mLastMotionX);
            if (DEBUG) Logger::D(TAG, "mLastMotionY: %d", mLastMotionY);
            if (DEBUG) Logger::D(TAG, "mParentDownMotionX: %d", mParentDownMotionX);
            if (DEBUG) Logger::D(TAG, "mParentDownMotionY: %d", mParentDownMotionY);

            Float parentX = mParentDownMotionX;
            Int32 pageIndexToSnapTo = -1;
            Int32 count;
            if (parentX < leftBufferEdge && dragViewIndex > 0) {
                pageIndexToSnapTo = dragViewIndex - 1;
            }
            else if (parentX > rightBufferEdge && dragViewIndex <
                    (GetChildCount(&count), count) - 1) {
                pageIndexToSnapTo = dragViewIndex + 1;
            }

            Int32 pageUnderPointIndex = pageIndexToSnapTo;
            if (pageUnderPointIndex > -1 && !isHoveringOverDelete) {
                (*mTempVisiblePagesRange)[0] = 0;
                Int32 pcount;
                GetPageCount(&pcount);
                (*mTempVisiblePagesRange)[1] = pcount - 1;
                BoundByReorderablePages(TRUE, mTempVisiblePagesRange);
                if ((*mTempVisiblePagesRange)[0] <= pageUnderPointIndex &&
                        pageUnderPointIndex <= (*mTempVisiblePagesRange)[1] &&
                        pageUnderPointIndex != mSidePageHoverIndex &&
                        (mScroller->IsFinished(&res), res)) {
                    mSidePageHoverIndex = pageUnderPointIndex;
                    mSidePageHoverRunnable = new MyRunnable(this,
                            pageUnderPointIndex, dragViewIndex);
                    PostDelayed(mSidePageHoverRunnable,
                            REORDERING_SIDE_PAGE_HOVER_TIMEOUT, &res);
                }
            }
            else {
                RemoveCallbacks(mSidePageHoverRunnable, &res);
                mSidePageHoverIndex = -1;
            }
        }
        else if (DetermineScrollingStart(ev, &res), res) {
            StartScrolling(ev);
        }
        else if (IsHorizontalCameraScroll(ev)) {
            StartScrolling(ev);
            // we need to cancel the camera animation
            Int32 index;
            GetPageWarpIndex(&index);
            AutoPtr<IView> view;
            GetPageAt(index, (IView**)&view);
            AutoPtr<IKeyguardWidgetFrame> v = IKeyguardWidgetFrame::Probe(view);

            AutoPtr<IViewPropertyAnimator> animator;
            IView::Probe(v)->Animate((IViewPropertyAnimator**)&animator);
            animator->Cancel();
        }
        break;
    }
    case IMotionEvent::ACTION_UP:
    {
        if (mTouchState == TOUCH_STATE_SCROLLING) {
            Int32 activePointerId = mActivePointerId;
            Int32 pointerIndex;
            ev->FindPointerIndex(activePointerId, &pointerIndex);

            if (pointerIndex == -1) {
                *result = TRUE;
                return NOERROR;
            }

            Float x;
            ev->GetX(pointerIndex, &x);
            AutoPtr<IVelocityTracker> velocityTracker = mVelocityTracker;
            velocityTracker->ComputeCurrentVelocity(1000, mMaximumVelocity);
            Float xVelocity;
            velocityTracker->GetXVelocity(activePointerId, &xVelocity);
            Int32 velocityX = (Int32)xVelocity;
            Int32 deltaX = (Int32)(x - mDownMotionX);
            AutoPtr<IView> view;
            GetPageAt(mCurrentPage, (IView**)&view);
            Int32 pageWidth;
            GetScaledMeasuredWidth(view, &pageWidth);
            Boolean isSignificantMove = Elastos::Core::Math::Abs(deltaX) > pageWidth *
                    SIGNIFICANT_MOVE_THRESHOLD;

            mTotalMotionX += Elastos::Core::Math::Abs(mLastMotionX + mLastMotionXRemainder - x);

            Boolean isFling = mTotalMotionX > MIN_LENGTH_FOR_FLING &&
                    Elastos::Core::Math::Abs(velocityX) > mFlingThresholdVelocity;

            // In the case that the page is moved far to one direction and then is flung
            // in the opposite direction, we use a threshold to determine whether we should
            // just return to the starting page, or if we should skip one further.
            Boolean returnToOriginalPage = FALSE;
            if (Elastos::Core::Math::Abs(deltaX) > pageWidth * RETURN_TO_ORIGINAL_PAGE_THRESHOLD &&
                    Elastos::Core::Math::Signum(velocityX) !=
                    Elastos::Core::Math::Signum(deltaX) && isFling) {
                returnToOriginalPage = TRUE;
            }

            Int32 finalPage;
            Int32 ccount;
            // We give flings precedence over large moves, which is why we short-circuit our
            // test for a large move if a fling has been registered. That is, a large
            // move to the left and fling to the right will register as a fling to the right.
            if (((isSignificantMove && deltaX > 0 && !isFling) ||
                    (isFling && velocityX > 0)) && mCurrentPage > 0) {
                Boolean res;
                finalPage = returnToOriginalPage || (IsWarping(&res), res)
                        ? mCurrentPage : mCurrentPage - 1;
                SnapToPageWithVelocity(finalPage, velocityX);
            }
            else if (((isSignificantMove && deltaX < 0 && !isFling) ||
                    (isFling && velocityX < 0)) &&
                    mCurrentPage < (GetChildCount(&ccount), ccount) - 1) {

                if (returnToOriginalPage) {
                    finalPage = mCurrentPage;
                }
                else {
                    Boolean res;
                    if (IsWarping(&res), res) {
                        GetPageWarpIndex(&finalPage);
                    }
                    else {
                        finalPage = mCurrentPage + 1;
                    }
                }
                SnapToPageWithVelocity(finalPage, velocityX);
            }
            else {
                SnapToDestination();
            }
        }
        else if (mTouchState == TOUCH_STATE_PREV_PAGE) {
            // at this point we have not moved beyond the touch slop
            // (otherwise mTouchState would be TOUCH_STATE_SCROLLING), so
            // we can just page
            Int32 nextPage = Elastos::Core::Math::Max(0, mCurrentPage - 1);
            if (nextPage != mCurrentPage) {
                SnapToPage(nextPage);
            }
            else {
                SnapToDestination();
            }
        }
        else if (mTouchState == TOUCH_STATE_NEXT_PAGE) {
            // at this point we have not moved beyond the touch slop
            // (otherwise mTouchState would be TOUCH_STATE_SCROLLING), so
            // we can just page
            Int32 count;
            GetChildCount(&count);
            Int32 nextPage = Elastos::Core::Math::Min(count - 1, mCurrentPage + 1);
            if (nextPage != mCurrentPage) {
                SnapToPage(nextPage);
            }
            else {
                SnapToDestination();
            }
        }
        else if (mTouchState == TOUCH_STATE_REORDERING) {
            // Update the last motion position
            ev->GetX(&mLastMotionX);
            ev->GetY(&mLastMotionY);

            // Update the parent down so that our zoom animations take this new movement into
            // account
            AutoPtr<ArrayOf<Float> > pt;
            MapPointFromViewToParent(this, mLastMotionX, mLastMotionY, (ArrayOf<Float>**)&pt);
            mParentDownMotionX = (*pt)[0];
            mParentDownMotionY = (*pt)[1];
            UpdateDragViewTranslationDuringDrag();
            Boolean handledFling = FALSE;
            if (!DISABLE_FLING_TO_DELETE) {
                // Check the velocity and see if we are flinging-to-delete
                AutoPtr<IPointF> flingToDeleteVector = IsFlingingToDelete();
                if (flingToDeleteVector != NULL) {
                    OnFlingToDelete(flingToDeleteVector);
                    handledFling = TRUE;
                }
            }
            if (!handledFling && IsHoveringOverDeleteDropTarget((Int32)mParentDownMotionX,
                    (Int32)mParentDownMotionY)) {
                OnDropToDelete();
            }
        }
        else {
            if (DEBUG_WARP) Logger::V(TAG, "calling onUnhandledTap()");
            if (mWarpPageExposed && !IsAnimatingWarpPage()) {
                AnimateWarpPageOffScreen(String("unhandled tap"), TRUE);
            }
            OnUnhandledTap(ev);
        }

        // Remove the callback to wait for the side page hover timeout
        Boolean res;
        RemoveCallbacks(mSidePageHoverRunnable, &res);
        // End any intermediate reordering states
        ResetTouchState();
        break;
    }
    case IMotionEvent::ACTION_CANCEL:
        if (mTouchState == TOUCH_STATE_SCROLLING) {
            SnapToDestination();
        }
        ResetTouchState();
        break;

    case IMotionEvent::ACTION_POINTER_UP:
        OnSecondaryPointerUp(ev);
        break;
    }

    *result = TRUE;
    return NOERROR;
}

void PagedView::ResetTouchState()
{
    ReleaseVelocityTracker();
    EndReordering();
    SetTouchState(TOUCH_STATE_REST);
    mActivePointerId = INVALID_POINTER;
    mDownEventOnEdge = FALSE;
}

ECode PagedView::OnUnhandledTap(
    /* [in] */ IMotionEvent* ev)
{
    return NOERROR;
}

ECode PagedView::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 source;
    IInputEvent::Probe(event)->GetSource(&source);
    if ((source & IInputDevice::SOURCE_CLASS_POINTER) != 0) {
        Int32 action;
        event->GetAction(&action);
        switch (action) {
            case IMotionEvent::ACTION_SCROLL: {
                // Handle mouse (or ext. device) by shifting the page depending on the scroll
                Float vscroll;
                Float hscroll;
                Int32 state;
                event->GetMetaState(&state);
                if ((state & IKeyEvent::META_SHIFT_ON) != 0) {
                    vscroll = 0;
                    event->GetAxisValue(IMotionEvent::AXIS_VSCROLL, &hscroll);
                }
                else {
                    event->GetAxisValue(IMotionEvent::AXIS_VSCROLL, &vscroll);
                    vscroll = -vscroll;
                    event->GetAxisValue(IMotionEvent::AXIS_HSCROLL, &hscroll);
                }
                if (hscroll != 0 || vscroll != 0) {
                    if (hscroll > 0 || vscroll > 0) {
                        ScrollRight();
                    }
                    else {
                        ScrollLeft();
                    }
                    *result = TRUE;
                    return NOERROR;
                }
            }
        }
    }
    return ViewGroup::OnGenericMotionEvent(event, result);
}

void PagedView::AcquireVelocityTrackerAndAddMovement(
    /* [in] */ IMotionEvent* ev)
{
    if (mVelocityTracker == NULL) {
        AutoPtr<IVelocityTrackerHelper> helper;
        CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&helper);
        helper->Obtain((IVelocityTracker**)&mVelocityTracker);
    }
    mVelocityTracker->AddMovement(ev);
}

void PagedView::ReleaseVelocityTracker()
{
    if (mVelocityTracker != NULL) {
        mVelocityTracker->Recycle();
        mVelocityTracker = NULL;
    }
}

void PagedView::OnSecondaryPointerUp(
    /* [in] */ IMotionEvent* ev)
{
    Int32 action;
    ev->GetAction(&action);
    Int32 pointerIndex = (action & IMotionEvent::ACTION_POINTER_INDEX_MASK) >>
            IMotionEvent::ACTION_POINTER_INDEX_SHIFT;
    Int32 pointerId;
    ev->GetPointerId(pointerIndex, &pointerId);
    if (pointerId == mActivePointerId) {
        // This was our active pointer going up. Choose a new
        // active pointer and adjust accordingly.
        // TODO: Make this decision more intelligent.
        Int32 newPointerIndex = pointerIndex == 0 ? 1 : 0;
        ev->GetX(newPointerIndex, &mDownMotionX);
        mLastMotionX = mDownMotionX;
        ev->GetY(newPointerIndex, &mLastMotionY);
        mLastMotionXRemainder = 0;
        ev->GetPointerId(newPointerIndex, &mActivePointerId);
        if (mVelocityTracker != NULL) {
            mVelocityTracker->Clear();
        }
    }
}

ECode PagedView::RequestChildFocus(
    /* [in] */ IView* child,
    /* [in] */ IView* focused)
{
    ViewGroup::RequestChildFocus(child, focused);

    Int32 index;
    IndexOfChild(child, &index);
    Int32 page;
    IndexToPage(index, &page);

    Int32 currentPage;
    GetCurrentPage(&currentPage);
    Boolean res;
    if (page >= 0 && page != currentPage && (IsInTouchMode(&res), !res)) {
        SnapToPage(page);
    }
    return NOERROR;
}

ECode PagedView::GetChildIndexForRelativeOffset(
    /* [in] */ Int32 relativeOffset,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset)

    Int32 childCount;
    GetChildCount(&childCount);
    Int32 left;
    Int32 right;
    for (Int32 i = 0; i < childCount; ++i) {
        GetRelativeChildOffset(i, &left);
        AutoPtr<IView> view;
        GetPageAt(i, (IView**)&view);
        Int32 width;
        GetScaledMeasuredWidth(view, &width);
        right = (left + width);
        if (left <= relativeOffset && relativeOffset <= right) {
            *offset = i;
            return NOERROR;
        }
    }
    *offset = -1;
    return NOERROR;
}

ECode PagedView::GetChildWidth(
    /* [in] */ Int32 index,
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)

    // This functions are called enough times that it actually makes a difference in the
    // profiler -- so just inline the max() here
    AutoPtr<IView> view;
    GetPageAt(index, (IView**)&view);
    Int32 measuredWidth;
    view->GetMeasuredWidth(&measuredWidth);
    Int32 minWidth = mMinimumWidth;
    *width = (minWidth > measuredWidth) ? minWidth : measuredWidth;
    return NOERROR;
}

ECode PagedView::GetPageNearestToPoint(
    /* [in] */ Float x,
    /* [out] */ Int32* page)
{
    VALIDATE_NOT_NULL(page)

    Int32 index = 0;
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IView> view;
        GetPageAt(i, (IView**)&view);
        Int32 right;
        view->GetRight(&right);
        Int32 scrollX;
        GetScrollX(&scrollX);
        if (x < right - scrollX) {
            *page = index;
            return NOERROR;
        }
        else {
            index++;
        }
    }
    *page = Elastos::Core::Math::Min(index, count - 1);
    return NOERROR;
}

ECode PagedView::GetPageNearestToCenterOfScreen(
    /* [out] */ Int32* page)
{
    VALIDATE_NOT_NULL(page)

    Int32 minDistanceFromScreenCenter = Elastos::Core::Math::INT32_MAX_VALUE;
    Int32 minDistanceFromScreenCenterIndex = -1;

    Int32 vOffsetX;
    GetViewportOffsetX(&vOffsetX);
    Int32 scrollX;
    GetScrollX(&scrollX);
    Int32 width;
    GetViewportWidth(&width);
    Int32 screenCenter = vOffsetX + scrollX + (width / 2);
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; ++i) {
        AutoPtr<IView> layout;
        GetPageAt(i, (IView**)&layout);
        Int32 childWidth;
        GetScaledMeasuredWidth(layout, &childWidth);
        Int32 halfChildWidth = (childWidth / 2);

        Int32 vOffsetX;
        GetViewportOffsetX(&vOffsetX);
        Int32 cOffset;
        GetChildOffset(i, &cOffset);
        Int32 childCenter = vOffsetX + cOffset + halfChildWidth;
        Int32 distanceFromScreenCenter = Elastos::Core::Math::Abs(childCenter - screenCenter);
        if (distanceFromScreenCenter < minDistanceFromScreenCenter) {
            minDistanceFromScreenCenter = distanceFromScreenCenter;
            minDistanceFromScreenCenterIndex = i;
        }
    }
    *page = minDistanceFromScreenCenterIndex;
    return NOERROR;
}

ECode PagedView::SnapToDestination()
{
    Int32 newPage;
    GetPageNearestToCenterOfScreen(&newPage);
    Boolean res;
    if (IsWarping(&res), res) {
        CancelWarpAnimation(String("snapToDestination"), mCurrentPage != newPage);
    }
    Int32 duration = GetPageSnapDuration();
    return SnapToPage(newPage, duration);
}

Int32 PagedView::GetPageSnapDuration()
{
    Boolean res;
    IsWarping(&res);
    return res ? WARP_SNAP_DURATION : PAGE_SNAP_ANIMATION_DURATION;
}

ECode PagedView::DistanceInfluenceForSnapDuration(
    /* [in] */ Float f,
    /* [out] */ Float* duration)
{
    VALIDATE_NOT_NULL(duration)

    f -= 0.5f; // center the values about 0.
    f *= 0.3f * Elastos::Core::Math::PI / 2.0f;
    *duration = (Float)Elastos::Core::Math::Sin(f);
    return NOERROR;
}

ECode PagedView::SnapToPageWithVelocity(
    /* [in] */ Int32 whichPage,
    /* [in] */ Int32 velocity)
{
    Int32 count;
    GetChildCount(&count);
    whichPage = Elastos::Core::Math::Max(0,
            Elastos::Core::Math::Min(whichPage, count - 1));
    Int32 width;
    GetViewportWidth(&width);
    Int32 halfScreenSize = width / 2;

    Boolean res;
    if (IsWarping(&res), res) {
        CancelWarpAnimation(String("snapToPageWithVelocity"), mCurrentPage != whichPage);
    }

    if (DEBUG) {
        Int32 cOffset;
        GetChildOffset(whichPage, &cOffset);
        Logger::D(TAG, "snapToPage.getChildOffset(): %d", cOffset);
    }
    if (DEBUG) {
        Int32 vwidth;
        GetViewportWidth(&vwidth);
        Int32 cwidth;
        GetChildWidth(whichPage, &cwidth);
        Logger::D(TAG, "snapToPageWithVelocity.getRelativeChildOffset(): %d, %d",
                vwidth, cwidth);
    }

    Int32 cOffset;
    GetChildOffset(whichPage, &cOffset);
    Int32 rcOffset;
    GetRelativeChildOffset(whichPage, &rcOffset);
    Int32 newX = cOffset - rcOffset;
    Int32 delta = newX - mUnboundedScrollX;
    Int32 duration = 0;

    if (Elastos::Core::Math::Abs(velocity) < mMinFlingVelocity) {
        // If the velocity is low enough, then treat this more as an automatic page advance
        // as opposed to an apparent physical response to flinging
        Int32 duration = GetPageSnapDuration();
        SnapToPage(whichPage, duration);
        return NOERROR;
    }

    // Here we compute a "distance" that will be used in the computation of the overall
    // snap duration. This is a function of the actual distance that needs to be traveled;
    // we keep this value close to half screen size in order to reduce the variance in snap
    // duration as a function of the distance the page needs to travel.
    Float distanceRatio = Elastos::Core::Math::Min(1.0f, 1.0f *
            Elastos::Core::Math::Abs(delta) / (2 * halfScreenSize));
    Float _duration;
    DistanceInfluenceForSnapDuration(distanceRatio, &_duration);
    Float distance = halfScreenSize + halfScreenSize * _duration;

    velocity = Elastos::Core::Math::Abs(velocity);
    velocity = Elastos::Core::Math::Max(mMinSnapVelocity, velocity);

    // we want the page's snap velocity to approximately match the velocity at which the
    // user flings, so we scale the duration by a value near to the derivative of the scroll
    // interpolator at zero, ie. 5. We use 4 to make it a little slower.
    duration = 4 * Elastos::Core::Math::Round(1000 *
            Elastos::Core::Math::Abs(distance / velocity));

    return SnapToPage(whichPage, delta, duration);
}

ECode PagedView::SnapToPage(
    /* [in] */ Int32 whichPage)
{
    Int32 duration = GetPageSnapDuration();
    return SnapToPage(whichPage, duration);
}

ECode PagedView::SnapToPageImmediately(
    /* [in] */ Int32 whichPage)
{
    Int32 duration = GetPageSnapDuration();
    return SnapToPage(whichPage, duration, TRUE);
}

ECode PagedView::SnapToPage(
    /* [in] */ Int32 whichPage,
    /* [in] */ Int32 duration)
{
    return SnapToPage(whichPage, duration, FALSE);
}

ECode PagedView::SnapToPage(
    /* [in] */ Int32 whichPage,
    /* [in] */ Int32 duration,
    /* [in] */ Boolean immediate)
{
    Int32 count;
    GetPageCount(&count);
    whichPage = Elastos::Core::Math::Max(0,
            Elastos::Core::Math::Min(whichPage, count - 1));

    if (DEBUG) {
        Int32 offset;
        GetChildOffset(whichPage, &offset);
        Logger::D(TAG, "snapToPage.getChildOffset(): %d", offset);
    }
    if (DEBUG) {
        Int32 vwidth;
        GetViewportWidth(&vwidth);
        Int32 cwidth;
        GetChildWidth(whichPage, &cwidth);
        Logger::D(TAG, "snapToPage.getRelativeChildOffset(): %d, %d", vwidth, cwidth);
    }
    Int32 cOffset;
    GetChildOffset(whichPage, &cOffset);
    Int32 rcOffset;
    GetRelativeChildOffset(whichPage, &rcOffset);
    Int32 newX = cOffset - rcOffset;
    Int32 sX = mUnboundedScrollX;
    Int32 delta = newX - sX;
    return SnapToPage(whichPage, delta, duration, immediate);
}

ECode PagedView::SnapToPage(
    /* [in] */ Int32 whichPage,
    /* [in] */ Int32 delta,
    /* [in] */ Int32 duration)
{
    return SnapToPage(whichPage, delta, duration, FALSE);
}

ECode PagedView::SnapToPage(
    /* [in] */ Int32 whichPage,
    /* [in] */ Int32 delta,
    /* [in] */ Int32 duration,
    /* [in] */ Boolean immediate)
{
    Boolean res;
    if ((IsWarping(&res), res) && whichPage == mCurrentPage + 1) {
        GetPageWarpIndex(&mNextPage); // jump to the warp page
        if (DEBUG_WARP) Logger::V(TAG, "snapToPage(%d) : reset mPageSwapIndex", whichPage);
    }
    else {
        mNextPage = whichPage;
    }

    if(mWarpPageExposed) {
        DispatchOnPageEndWarp();
        mWarpPageExposed = FALSE;
    }
    NotifyPageSwitching(whichPage);


    AutoPtr<IView> focusedChild;
    GetFocusedChild((IView**)&focusedChild);
    AutoPtr<IView> cpage;
    GetPageAt(mCurrentPage, (IView**)&cpage);
    if (focusedChild != NULL && whichPage != mCurrentPage &&
            TO_IINTERFACE(focusedChild) == TO_IINTERFACE(cpage)) {
        focusedChild->ClearFocus();
    }

    PageBeginMoving();
    AwakenScrollBars(duration);
    if (immediate) {
        duration = 0;
    }
    else if (duration == 0) {
        duration = Elastos::Core::Math::Abs(delta);
    }


    if (mScroller->IsFinished(&res), !res) mScroller->AbortAnimation();
    mScroller->StartScroll(mUnboundedScrollX, 0, delta, 0, duration);

    NotifyPageSwitched();

    // Trigger a compute() to finish switching pages if necessary
    if (immediate) {
        ComputeScroll();
    }

    mForceScreenScrolled = TRUE;
    return Invalidate();
}

ECode PagedView::IsWarping(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mWarpPageExposed;
    return NOERROR;
}

ECode PagedView::ScrollLeft()
{
    Boolean res;
    if (mScroller->IsFinished(&res), res) {
        if (mCurrentPage > 0) SnapToPage(mCurrentPage - 1);
    }
    else {
        if (mNextPage > 0) SnapToPage(mNextPage - 1);
    }
    return NOERROR;
}

ECode PagedView::ScrollRight()
{
    Int32 count;
    GetChildCount(&count);
    Boolean res;
    if (mScroller->IsFinished(&res), res) {
        if (mCurrentPage < count -1) SnapToPage(mCurrentPage + 1);
    }
    else {
        if (mNextPage < count -1) SnapToPage(mNextPage + 1);
    }
    return NOERROR;
}

ECode PagedView::GetPageForView(
    /* [in] */ IView* v,
    /* [out] */ Int32* page)
{
    VALIDATE_NOT_NULL(page)

    Int32 result = -1;
    if (v != NULL) {
        AutoPtr<IViewParent> vp;
        v->GetParent((IViewParent**)&vp);
        Int32 count;
        GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> view;
            GetPageAt(i, (IView**)&view);
            if (TO_IINTERFACE(vp) == TO_IINTERFACE(view)) {
                *page = i;
                return NOERROR;
            }
        }
    }
    *page = result;
    return NOERROR;
}

ECode PagedView::GetScrollingIndicator(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    *view = NULL;
    return NOERROR;
}

ECode PagedView::IsScrollingIndicatorEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode PagedView::FlashScrollingIndicator(
    /* [in] */ Boolean animated)
{
    Boolean res;
    RemoveCallbacks(mHideScrollingIndicatorRunnable, &res);
    ShowScrollingIndicator(!animated);
    return PostDelayed(mHideScrollingIndicatorRunnable,
            sScrollIndicatorFlashDuration, &res);
}

ECode PagedView::ShowScrollingIndicator(
    /* [in] */ Boolean immediately)
{
    mShouldShowScrollIndicator = TRUE;
    mShouldShowScrollIndicatorImmediately = TRUE;

    Int32 count;
    GetChildCount(&count);
    if (count <= 1) return NOERROR;
    Boolean res;
    if (IsScrollingIndicatorEnabled(&res), !res) return NOERROR;

    mShouldShowScrollIndicator = FALSE;
    AutoPtr<IView> tmp;
    GetScrollingIndicator((IView**)&tmp);
    if (mScrollIndicator != NULL) {
        // Fade the indicator in
        UpdateScrollingIndicatorPosition();
        mScrollIndicator->SetVisibility(IView::VISIBLE);
        CancelScrollingIndicatorAnimations();
        if (immediately) {
            mScrollIndicator->SetAlpha(1.0f);
        }
        else {
            AutoPtr<IObjectAnimatorHelper> helper;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
            AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
            (*array)[0] = 1.0f;
            AutoPtr<IObjectAnimator> oa;
            helper->OfFloat(mScrollIndicator, String("alpha"), array, (IObjectAnimator**)&oa);
            mScrollIndicatorAnimator = IValueAnimator::Probe(oa);
            mScrollIndicatorAnimator->SetDuration(sScrollIndicatorFadeInDuration);
            IAnimator::Probe(mScrollIndicatorAnimator)->Start();
        }
    }
    return NOERROR;
}

ECode PagedView::CancelScrollingIndicatorAnimations()
{
    if (mScrollIndicatorAnimator != NULL) {
        IAnimator::Probe(mScrollIndicatorAnimator)->Cancel();
    }
    return NOERROR;
}

ECode PagedView::HideScrollingIndicator(
    /* [in] */ Boolean immediately)
{
    Int32 count;
    GetChildCount(&count);
    if (count <= 1) return NOERROR;
    Boolean res;
    if (IsScrollingIndicatorEnabled(&res), !res) return NOERROR;

    AutoPtr<IView> tmp;
    GetScrollingIndicator((IView**)&tmp);
    if (mScrollIndicator != NULL) {
        // Fade the indicator out
        UpdateScrollingIndicatorPosition();
        CancelScrollingIndicatorAnimations();
        if (immediately) {
            mScrollIndicator->SetVisibility(IView::INVISIBLE);
            mScrollIndicator->SetAlpha(0.0f);
        }
        else {
            AutoPtr<IObjectAnimatorHelper> helper;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
            AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
            (*array)[0] = 0.0f;
            AutoPtr<IObjectAnimator> oa;
            helper->OfFloat(mScrollIndicator, String("alpha"), array, (IObjectAnimator**)&oa);
            mScrollIndicatorAnimator = IValueAnimator::Probe(oa);
            mScrollIndicatorAnimator->SetDuration(sScrollIndicatorFadeOutDuration);
            AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter(this);
            IAnimator::Probe(mScrollIndicatorAnimator)->AddListener(lis);
            IAnimator::Probe(mScrollIndicatorAnimator)->Start();
        }
    }
    return NOERROR;
}

ECode PagedView::HasElasticScrollIndicator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = TRUE;
    return NOERROR;
}

void PagedView::UpdateScrollingIndicator()
{
    Int32 count;
    GetChildCount(&count);
    if (count <= 1) return;
    Boolean res;
    if (IsScrollingIndicatorEnabled(&res), !res) return;

    AutoPtr<IView> tmp;
    GetScrollingIndicator((IView**)&tmp);
    if (mScrollIndicator != NULL) {
        UpdateScrollingIndicatorPosition();
    }
    if (mShouldShowScrollIndicator) {
        ShowScrollingIndicator(mShouldShowScrollIndicatorImmediately);
    }
}

void PagedView::UpdateScrollingIndicatorPosition()
{
    Boolean res;
    if (IsScrollingIndicatorEnabled(&res), !res) return;
    if (mScrollIndicator == NULL) return;
    Int32 numPages;
    GetChildCount(&numPages);
    Int32 pageWidth;
    GetViewportWidth(&pageWidth);
    Int32 lastChildIndex = Elastos::Core::Math::Max(0, numPages - 1);

    Int32 cOffset;
    GetChildOffset(lastChildIndex, &cOffset);
    Int32 rcOffset;
    GetRelativeChildOffset(lastChildIndex, &rcOffset);
    Int32 maxScrollX = cOffset - rcOffset;
    Int32 trackWidth = pageWidth - mScrollIndicatorPaddingLeft - mScrollIndicatorPaddingRight;

    Int32 mwidth;
    mScrollIndicator->GetMeasuredWidth(&mwidth);
    Int32 pleft;
    mScrollIndicator->GetPaddingLeft(&pleft);
    Int32 pright;
    mScrollIndicator->GetPaddingRight(&pright);
    Int32 indicatorWidth = mwidth - pleft - pright;

    Int32 scrollX;
    GetScrollX(&scrollX);
    Float offset = Elastos::Core::Math::Max(0.0f,
            Elastos::Core::Math::Min(1.0f, (Float)scrollX / maxScrollX));
    Int32 indicatorSpace = trackWidth / numPages;
    Int32 indicatorPos = (Int32) (offset * (trackWidth - indicatorSpace)) + mScrollIndicatorPaddingLeft;

    if (HasElasticScrollIndicator(&res), res) {
        Int32 _mwidth;
        mScrollIndicator->GetMeasuredWidth(&_mwidth);
        if (_mwidth != indicatorSpace) {
            AutoPtr<IViewGroupLayoutParams> params;
            mScrollIndicator->GetLayoutParams((IViewGroupLayoutParams**)&params);
            params->SetWidth(indicatorSpace);
            mScrollIndicator->RequestLayout();
        }
    }
    else {
        Int32 indicatorCenterOffset = indicatorSpace / 2 - indicatorWidth / 2;
        indicatorPos += indicatorCenterOffset;
    }
    mScrollIndicator->SetTranslationX(indicatorPos);
}

ECode PagedView::AnimateDragViewToOriginalPosition()
{
    if (mDragView != NULL) {
        AutoPtr<IAnimatorSet> anim;
        CAnimatorSet::New((IAnimatorSet**)&anim);
        IAnimator::Probe(anim)->SetDuration(REORDERING_DROP_REPOSITION_DURATION);

        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = 0.0f;
        AutoPtr<IObjectAnimator> animatorx;
        helper->OfFloat(mDragView, String("translationX"), array, (IObjectAnimator**)&animatorx);
        AutoPtr<ArrayOf<Float> > array2 = ArrayOf<Float>::Alloc(1);
        (*array2)[0] = 0.0f;
        AutoPtr<IObjectAnimator> animatory;
        helper->OfFloat(mDragView, String("translationY"), array2, (IObjectAnimator**)&animatory);

        AutoPtr<ArrayOf<IAnimator*> > array3 = ArrayOf<IAnimator*>::Alloc(2);
        array3->Set(0, IAnimator::Probe(animatorx));
        array3->Set(1, IAnimator::Probe(animatory));
        anim->PlayTogether(array3);
        AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter2(this);
        IAnimator::Probe(anim)->AddListener(lis);
        IAnimator::Probe(anim)->Start();
    }
    return NOERROR;
}

ECode PagedView::ZoomOut(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    if (mZoomInOutAnim != NULL && (IAnimator::Probe(mZoomInOutAnim)->IsRunning(&res), res)) {
        IAnimator::Probe(mZoomInOutAnim)->Cancel();
    }

    Float x, y;
    if (!((GetScaleX(&x), x) < 1.0f || (GetScaleY(&y), x) < 1.0f)) {
        CAnimatorSet::New((IAnimatorSet**)&mZoomInOutAnim);
        IAnimator::Probe(mZoomInOutAnim)->SetDuration(REORDERING_ZOOM_IN_OUT_DURATION);

        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = mMinScale;
        AutoPtr<IObjectAnimator> animatorx;
        helper->OfFloat(TO_IINTERFACE(this), String("scaleX"), array, (IObjectAnimator**)&animatorx);
        AutoPtr<ArrayOf<Float> > array2 = ArrayOf<Float>::Alloc(1);
        (*array2)[0] = mMinScale;
        AutoPtr<IObjectAnimator> animatory;
        helper->OfFloat(TO_IINTERFACE(this), String("scaleY"), array2, (IObjectAnimator**)&animatory);

        AutoPtr<ArrayOf<IAnimator*> > array3 = ArrayOf<IAnimator*>::Alloc(2);
        array3->Set(0, IAnimator::Probe(animatorx));
        array3->Set(1, IAnimator::Probe(animatory));
        mZoomInOutAnim->PlayTogether(array3);
        AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter3(this);
        IAnimator::Probe(mZoomInOutAnim)->AddListener(lis);
        IAnimator::Probe(mZoomInOutAnim)->Start();
        *result= TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode PagedView::OnStartReordering()
{
    AutoPtr<IAccessibilityManagerHelper> helper;
    CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&helper);
    AutoPtr<IAccessibilityManager> manager;
    helper->GetInstance(mContext, (IAccessibilityManager**)&manager);
    Boolean res;
    if (manager->IsEnabled(&res), res) {
        String str;
        mContext->GetString(
                R::string::keyguard_accessibility_widget_reorder_start, &str);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
        AnnounceForAccessibility(cchar);
    }

    // Set the touch state to reordering (allows snapping to pages, dragging a child, etc.)
    SetTouchState(TOUCH_STATE_REORDERING);
    mIsReordering = TRUE;

    // Mark all the non-widget pages as invisible
    GetVisiblePages(mTempVisiblePagesRange);
    BoundByReorderablePages(TRUE, mTempVisiblePagesRange);
    Int32 count;
    GetPageCount(&count);
    for (Int32 i = 0; i < count; ++i) {
        if (i < (*mTempVisiblePagesRange)[0] || i > (*mTempVisiblePagesRange)[1]) {
            AutoPtr<IView> view;
            GetPageAt(i, (IView**)&view);
            view->SetAlpha(0.0f);
        }
    }

    // We must invalidate to trigger a redraw to update the layers such that the drag view
    // is always drawn on top
    return Invalidate();
}

void PagedView::OnPostReorderingAnimationCompleted()
{
    // Trigger the callback when reordering has settled
    --mPostReorderingPreZoomInRemainingAnimationCount;
    if (mPostReorderingPreZoomInRunnable != NULL &&
            mPostReorderingPreZoomInRemainingAnimationCount == 0) {
        mPostReorderingPreZoomInRunnable->Run();
        mPostReorderingPreZoomInRunnable = NULL;
    }
}

ECode PagedView::OnEndReordering()
{
    AutoPtr<IAccessibilityManagerHelper> helper;
    CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&helper);
    AutoPtr<IAccessibilityManager> manager;
    helper->GetInstance(mContext, (IAccessibilityManager**)&manager);
    Boolean res;
    if (manager->IsEnabled(&res), res) {
        if (mDeleteString != NULL) {
            AutoPtr<ICharSequence> cchar = CoreUtils::Convert(mDeleteString);
            AnnounceForAccessibility(cchar);
            mDeleteString = NULL;
        }
        else {
            String str;
            mContext->GetString(
                    R::string::keyguard_accessibility_widget_reorder_end, &str);
            AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
            AnnounceForAccessibility(cchar);
        }
    }
    mIsReordering = FALSE;

    // Mark all the non-widget pages as visible again
    GetVisiblePages(mTempVisiblePagesRange);
    BoundByReorderablePages(TRUE, mTempVisiblePagesRange);
    Int32 count;
    GetPageCount(&count);
    for (Int32 i = 0; i < count; ++i) {
        if (i < (*mTempVisiblePagesRange)[0] || i > (*mTempVisiblePagesRange)[1]) {
            AutoPtr<IView> view;
            GetPageAt(i, (IView**)&view);
            view->SetAlpha(1.0f);
        }
    }
    return NOERROR;
}

ECode PagedView::StartReordering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 dragViewIndex;
    GetPageNearestToCenterOfScreen(&dragViewIndex);
    (*mTempVisiblePagesRange)[0] = 0;
    Int32 count;
    GetPageCount(&count);
    (*mTempVisiblePagesRange)[1] = count - 1;
    BoundByReorderablePages(TRUE, mTempVisiblePagesRange);

    // Check if we are within the reordering range
    if ((*mTempVisiblePagesRange)[0] <= dragViewIndex &&
            dragViewIndex <= (*mTempVisiblePagesRange)[1]) {
        mReorderingStarted = TRUE;
        Boolean res;
        if (ZoomOut(&res), res) {
            // Find the drag view under the pointer
            GetChildAt(dragViewIndex, (IView**)&mDragView);

            OnStartReordering();
        }
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode PagedView::IsReordering(
    /* [in] */ Boolean testTouchState,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean state = mIsReordering;
    if (testTouchState) {
        state &= (mTouchState == TOUCH_STATE_REORDERING);
    }
    *result = state;
    return NOERROR;
}

ECode PagedView::EndReordering()
{
    // For simplicity, we call endReordering sometimes even if reordering was never started.
    // In that case, we don't want to do anything.
    if (!mReorderingStarted) return NOERROR;
    mReorderingStarted = FALSE;

    // If we haven't flung-to-delete the current child, then we just animate the drag view
    // back into position
    AutoPtr<IRunnable> onCompleteRunnable = new MyRunnable3(this);
    if (!mDeferringForDelete) {
        mPostReorderingPreZoomInRunnable = new MyRunnable4(this, onCompleteRunnable);

        mPostReorderingPreZoomInRemainingAnimationCount =
                NUM_ANIMATIONS_RUNNING_BEFORE_ZOOM_OUT;
        // Snap to the current page
        Int32 index;
        IndexOfChild(mDragView, &index);
        SnapToPage(index, 0);
        // Animate the drag view back to the front position
        AnimateDragViewToOriginalPosition();
    }
    else {
        // Handled in post-delete-animation-callbacks
    }
    return NOERROR;
}

ECode PagedView::ZoomIn(
    /* [in] */ IRunnable* onCompleteRunnable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    if (mZoomInOutAnim != NULL && (IAnimator::Probe(mZoomInOutAnim)->IsRunning(&res), res)) {
        IAnimator::Probe(mZoomInOutAnim)->Cancel();
    }

    Float x;
    GetScaleX(&x);
    Float y;
    GetScaleY(&y);
    if (x < 1.0f || y < 1.0f) {
        CAnimatorSet::New((IAnimatorSet**)&mZoomInOutAnim);
        IAnimator::Probe(mZoomInOutAnim)->SetDuration(REORDERING_ZOOM_IN_OUT_DURATION);

        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = 1.0f;
        AutoPtr<IObjectAnimator> animatorx;
        helper->OfFloat(TO_IINTERFACE(this), String("scaleX"), array, (IObjectAnimator**)&animatorx);
        AutoPtr<ArrayOf<Float> > array2 = ArrayOf<Float>::Alloc(1);
        (*array2)[0] = 1.0f;
        AutoPtr<IObjectAnimator> animatory;
        helper->OfFloat(TO_IINTERFACE(this), String("scaleY"), array2, (IObjectAnimator**)&animatory);

        AutoPtr<ArrayOf<IAnimator*> > array3 = ArrayOf<IAnimator*>::Alloc(2);
        array3->Set(0, IAnimator::Probe(animatorx));
        array3->Set(1, IAnimator::Probe(animatory));
        mZoomInOutAnim->PlayTogether(array3);
        AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter5(this,
                onCompleteRunnable);
        IAnimator::Probe(mZoomInOutAnim)->AddListener(lis);
        IAnimator::Probe(mZoomInOutAnim)->Start();
        *result = TRUE;
        return NOERROR;
    }
    else {
        if (onCompleteRunnable != NULL) {
            onCompleteRunnable->Run();
        }
    }
    *result = FALSE;
    return NOERROR;
}

AutoPtr<IPointF> PagedView::IsFlingingToDelete()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> config;
    helper->Get(context, (IViewConfiguration**)&config);
    Int32 maximumFlingVelocity;
    config->GetScaledMaximumFlingVelocity(&maximumFlingVelocity);
    mVelocityTracker->ComputeCurrentVelocity(1000, maximumFlingVelocity);

    Float yVelocity;
    mVelocityTracker->GetYVelocity(&yVelocity);
    if (yVelocity < mFlingToDeleteThresholdVelocity) {
        // Do a quick dot product test to ensure that we are flinging upwards
        Float xVelocity;
        mVelocityTracker->GetXVelocity(&xVelocity);
        mVelocityTracker->GetYVelocity(&yVelocity);
        AutoPtr<IPointF> vel;
        CPointF::New(xVelocity, yVelocity, (IPointF**)&vel);

        AutoPtr<IPointF> upVec;
        CPointF::New(0.0f, -1.0f, (IPointF**)&upVec);
        Float vx;
        vel->GetX(&vx);
        Float ux;
        upVec->GetX(&ux);
        Float vy;
        vel->GetY(&vy);
        Float uy;
        upVec->GetY(&uy);
        Float vlength;
        vel->Length(&vlength);
        Float ulength;
        upVec->Length(&ulength);
        Float theta = (Float) Elastos::Core::Math::Acos(((vx * ux) + (vy * uy)) /
                (vlength * ulength));
        if (theta <= Elastos::Core::Math::ToRadians(FLING_TO_DELETE_MAX_FLING_DEGREES)) {
            return vel;
        }
    }
    return NULL;
}

AutoPtr<IRunnable> PagedView::CreatePostDeleteAnimationRunnable(
    /* [in] */ IView* dragView)
{
    AutoPtr<IRunnable> r = new MyRunnable5(this, dragView);
    return r;
}

ECode PagedView::OnFlingToDelete(
    /* [in] */ IPointF* vel)
{
    AutoPtr<IAnimationUtils> helper;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&helper);
    Int64 startTime;
    helper->CurrentAnimationTimeMillis(&startTime);

    // NOTE: Because it takes time for the first frame of animation to actually be
    // called and we expect the animation to be a continuation of the fling, we have
    // to account for the time that has elapsed since the fling finished.  And since
    // we don't have a startDelay, we will always get call to update when we call
    // start() (which we want to ignore).
    AutoPtr<ITimeInterpolator> tInterpolator = new MyTimeInterpolator(this, startTime);

    AutoPtr<IRect> from;
    CRect::New((IRect**)&from);
    AutoPtr<IView> dragView = mDragView;
    Float x;
    dragView->GetTranslationX(&x);
    from->SetLeft((Int32)x);
    Float y;
    dragView->GetTranslationY(&y);
    from->SetTop((Int32)y);

    AutoPtr<FlingAlongVectorAnimatorUpdateListener> tmp =
            new FlingAlongVectorAnimatorUpdateListener();
    tmp->constructor(dragView, vel, from, startTime, FLING_TO_DELETE_FRICTION);
    AutoPtr<IAnimatorUpdateListener> updateCb = IAnimatorUpdateListener::Probe(tmp);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> cchar;
    mDragView->GetContentDescription((ICharSequence**)&cchar);
    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
    array->Set(0, TO_IINTERFACE(cchar));
    resources->GetString(R::string::keyguard_accessibility_widget_deleted,
            array, &mDeleteString);
    AutoPtr<IRunnable> onAnimationEndRunnable = CreatePostDeleteAnimationRunnable(dragView);

    // Create and start the animation
    AutoPtr<IValueAnimator> mDropAnim;
    CValueAnimator::New((IValueAnimator**)&mDropAnim);
    IAnimator::Probe(mDropAnim)->SetInterpolator(tInterpolator);
    mDropAnim->SetDuration(FLING_TO_DELETE_FADE_OUT_DURATION);

    AutoPtr<ArrayOf<Float> > array2 = ArrayOf<Float>::Alloc(2);
    (*array2)[0] = 0.0f;
    (*array2)[1] = 1.0f;
    mDropAnim->SetFloatValues(array2);
    mDropAnim->AddUpdateListener(updateCb);
    AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter8(onAnimationEndRunnable);
    IAnimator::Probe(mDropAnim)->AddListener(lis);
    IAnimator::Probe(mDropAnim)->Start();
    mDeferringForDelete = TRUE;
    return NOERROR;
}

Boolean PagedView::IsHoveringOverDeleteDropTarget(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    if (mDeleteDropTarget != NULL) {
        mAltTmpRect->Set(0, 0, 0, 0);
        AutoPtr<IViewParent> p;
        mDeleteDropTarget->GetParent((IViewParent**)&p);
        AutoPtr<IView> parent = IView::Probe(p);
        Boolean res;
        if (parent != NULL) {
            parent->GetGlobalVisibleRect(mAltTmpRect, &res);
        }
        mDeleteDropTarget->GetGlobalVisibleRect(mTmpRect, &res);
        Int32 left;
        mAltTmpRect->GetLeft(&left);
        Int32 top;
        mAltTmpRect->GetTop(&top);
        mTmpRect->Offset(-left, -top);
        mTmpRect->Contains(x, y, &res);
        return NOERROR;
    }
    return FALSE;
}

ECode PagedView::SetPageHoveringOverDeleteDropTarget(
    /* [in] */ Int32 viewIndex,
    /* [in] */ Boolean isHovering)
{
    return NOERROR;
}

void PagedView::OnDropToDelete()
{
    AutoPtr<IView> dragView = mDragView;

    Float toScale = 0.0f;
    Float toAlpha = 0.0f;

    // Create and start the complex animation
    AutoPtr<IArrayList> animations;
    CArrayList::New((IArrayList**)&animations);
    AutoPtr<IAnimatorSet> motionAnim;
    CAnimatorSet::New((IAnimatorSet**)&motionAnim);

    AutoPtr<ITimeInterpolator> polator;
    CDecelerateInterpolator::New(2, (ITimeInterpolator**)&polator);
    IAnimator::Probe(motionAnim)->SetInterpolator(polator);

    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
    (*array)[0] = toScale;
    AutoPtr<IObjectAnimator> animatorx;
    helper->OfFloat(dragView, String("scaleX"), array, (IObjectAnimator**)&animatorx);
    AutoPtr<ArrayOf<Float> > array2 = ArrayOf<Float>::Alloc(1);
    (*array2)[0] = toScale;
    AutoPtr<IObjectAnimator> animatory;
    helper->OfFloat(dragView, String("scaleY"), array2, (IObjectAnimator**)&animatory);

    AutoPtr<ArrayOf<IAnimator*> > array3 = ArrayOf<IAnimator*>::Alloc(2);
    array3->Set(0, IAnimator::Probe(animatorx));
    array3->Set(1, IAnimator::Probe(animatory));
    motionAnim->PlayTogether(array3);
    animations->Add(motionAnim);

    AutoPtr<IAnimatorSet> alphaAnim;
    CAnimatorSet::New((IAnimatorSet**)&alphaAnim);
    AutoPtr<ITimeInterpolator> polator2;
    CLinearInterpolator::New((ITimeInterpolator**)&polator2);
    IAnimator::Probe(alphaAnim)->SetInterpolator(polator2);

    AutoPtr<ArrayOf<Float> > array4 = ArrayOf<Float>::Alloc(1);
    (*array4)[0] = toAlpha;
    AutoPtr<IObjectAnimator> animatorz;
    helper->OfFloat(dragView, String("alpha"), array4, (IObjectAnimator**)&animatorz);
    AutoPtr<ArrayOf<IAnimator*> > array5 = ArrayOf<IAnimator*>::Alloc(1);
    array5->Set(0, IAnimator::Probe(animatorz));
    alphaAnim->PlayTogether(array5);
    animations->Add(alphaAnim);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> cchar;
    mDragView->GetContentDescription((ICharSequence**)&cchar);
    AutoPtr<ArrayOf<IInterface*> > array6 = ArrayOf<IInterface*>::Alloc(1);
    array6->Set(0, TO_IINTERFACE(cchar));
    resources->GetString(R::string::keyguard_accessibility_widget_deleted,
            array6, &mDeleteString);
    AutoPtr<IRunnable> onAnimationEndRunnable = CreatePostDeleteAnimationRunnable(dragView);

    AutoPtr<IAnimatorSet> anim;
    CAnimatorSet::New((IAnimatorSet**)&anim);
    anim->PlayTogether(ICollection::Probe(animations));
    IAnimator::Probe(anim)->SetDuration(DRAG_TO_DELETE_FADE_OUT_DURATION);
    AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter9(onAnimationEndRunnable);
    IAnimator::Probe(anim)->AddListener(lis);
    IAnimator::Probe(anim)->Start();

    mDeferringForDelete = TRUE;
}

ECode PagedView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ViewGroup::OnInitializeAccessibilityNodeInfo(info);
    Int32 count;
    GetPageCount(&count);
    info->SetScrollable(count > 1);

    Int32 cpage;
    GetCurrentPage(&cpage);
    if (cpage < count - 1) {
        info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD);
    }
    if (cpage > 0) {
        info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD);
    }
    return NOERROR;
}

ECode PagedView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    ViewGroup::OnInitializeAccessibilityEvent(event);
    IAccessibilityRecord::Probe(event)->SetScrollable(TRUE);
    Int32 type;
    event->GetEventType(&type);
    if (type == IAccessibilityEvent::TYPE_VIEW_SCROLLED) {
        IAccessibilityRecord::Probe(event)->SetFromIndex(mCurrentPage);
        IAccessibilityRecord::Probe(event)->SetToIndex(mCurrentPage);
        Int32 count;
        GetChildCount(&count);
        IAccessibilityRecord::Probe(event)->SetItemCount(count);
    }
    return NOERROR;
}

ECode PagedView::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    if (ViewGroup::PerformAccessibilityAction(action, arguments, &res), res) {
        *result = TRUE;
        return NOERROR;
    }
    switch (action) {
        case IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD: {
            Int32 cpage;
            GetCurrentPage(&cpage);
            Int32 count;
            GetPageCount(&count);
            if (cpage < count - 1) {
                ScrollRight();
                *result = TRUE;
                return NOERROR;
            }
        } break;
        case IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD: {
            Int32 cpage;
            GetCurrentPage(&cpage);
            if (cpage > 0) {
                ScrollLeft();
                *result = TRUE;
                return NOERROR;
            }
        } break;
    }
    *result = FALSE;
    return NOERROR;
}

ECode PagedView::OnHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = TRUE;
    return NOERROR;
}

ECode PagedView::BeginCameraEvent()
{
    mIsCameraEvent = TRUE;
    return NOERROR;
}

ECode PagedView::EndCameraEvent()
{
    mIsCameraEvent = FALSE;
    return NOERROR;
}

void PagedView::CancelWarpAnimation(
    /* [in] */ const String& msg,
    /* [in] */ Boolean abortAnimation)
{
    if (DEBUG_WARP) Logger::V(TAG, "cancelWarpAnimation(%s,abort=%d)",
            msg.string(), abortAnimation);
    if (abortAnimation) {
        // We're done with the animation and moving to a new page.  Let the scroller
        // take over the animation.
        Int32 index;
        GetPageWarpIndex(&index);
        AutoPtr<IView> view;
        GetPageAt(index, (IView**)&view);
        AutoPtr<IKeyguardWidgetFrame> v = IKeyguardWidgetFrame::Probe(view);
        AutoPtr<IViewPropertyAnimator> animator;
        IView::Probe(v)->Animate((IViewPropertyAnimator**)&animator);
        animator->Cancel();
        // Make the scroll amount match the current warp position.
        Float x;
        IView::Probe(v)->GetTranslationX(&x);
        ScrollBy(Elastos::Core::Math::Round(-x), 0);
        IView::Probe(v)->SetTranslationX(0);
    }
    else {
        AnimateWarpPageOffScreen(String("canceled"), TRUE);
    }
}

Boolean PagedView::IsAnimatingWarpPage()
{
    return mWarpAnimation != NULL;
}

void PagedView::AnimateWarpPageOnScreen(
    /* [in] */ const String& reason)
{
    if (DEBUG_WARP) Logger::V(TAG, "animateWarpPageOnScreen(%s)", reason.string());
    if (!mWarpPageExposed) {
        mWarpPageExposed = TRUE;
        DispatchOnPageBeginWarp();

        Int32 index;
        GetPageWarpIndex(&index);
        AutoPtr<IView> view;
        GetPageAt(index, (IView**)&view);
        AutoPtr<IKeyguardWidgetFrame> v = IKeyguardWidgetFrame::Probe(view);

        if (DEBUG_WARP) {
            Float x;
            IView::Probe(v)->GetTranslationX(&x);
            Logger::V(TAG, "moving page on screen: Tx= %f", x);
        }
        AutoPtr<ITimeInterpolator> interp;
        CDecelerateInterpolator::New(1.5f, (ITimeInterpolator**)&interp);
        Int32 totalOffset = GetCurrentWarpOffset();
        IView::Probe(v)->SetTranslationX(totalOffset);
        IView::Probe(v)->Animate((IViewPropertyAnimator**)&mWarpAnimation);
        mWarpAnimation->TranslationX(mWarpPeekAmount+totalOffset);
        mWarpAnimation->SetInterpolator(interp);
        mWarpAnimation->SetDuration(WARP_PEEK_ANIMATION_DURATION);
        mWarpAnimation->SetListener(mOnScreenAnimationListener);
    }
}

Int32 PagedView::GetCurrentWarpOffset()
{
    Int32 index;
    GetPageWarpIndex(&index);
    if (mCurrentPage == index) {
        return 0;
    }
    AutoPtr<IView> viewRight;
    GetPageAt(mCurrentPage + 1, (IView**)&viewRight);
    AutoPtr<IView> warpView;
    GetPageAt(index, (IView**)&warpView);
    if (viewRight != warpView && viewRight != NULL && warpView != NULL) {
        Int32 viewleft;
        viewRight->GetLeft(&viewleft);
        Int32 warpleft;
        warpView->GetLeft(&warpleft);
        return viewleft - warpleft;
    }
    return 0;
}

void PagedView::AnimateWarpPageOffScreen(
    /* [in] */ const String& reason,
    /* [in] */ Boolean animate)
{
    if (DEBUG_WARP) Logger::V(TAG, "animateWarpPageOffScreen(%s anim:%d)",
            reason.string(), animate);
    if (mWarpPageExposed) {
        DispatchOnPageEndWarp();

        Int32 index;
        GetPageWarpIndex(&index);
        AutoPtr<IView> view;
        GetPageAt(index, (IView**)&view);
        AutoPtr<IKeyguardWidgetFrame> v = IKeyguardWidgetFrame::Probe(view);
        if (DEBUG_WARP) {
            Float x;
            IView::Probe(v)->GetTranslationX(&x);
            Logger::V(TAG, "moving page off screen: Tx=%f", x);
        }
        AutoPtr<ITimeInterpolator> interp;
        CAccelerateInterpolator::New(1.5f, (ITimeInterpolator**)&interp);
        Int32 totalOffset = GetCurrentWarpOffset();
        AutoPtr<IViewPropertyAnimator> animator;
        IView::Probe(v)->Animate((IViewPropertyAnimator**)&animator);
        animator->TranslationX(totalOffset);
        animator->SetInterpolator(interp);
        animator->SetDuration(animate ? WARP_PEEK_ANIMATION_DURATION : 0);
        animator->SetListener(mOffScreenAnimationListener);
    }
    else {
        if (DEBUG_WARP) Logger::E(TAG, "animateWarpPageOffScreen(): not warping");
    }
}

ECode PagedView::SwapPages(
    /* [in] */ Int32 indexA,
    /* [in] */ Int32 indexB)
{
    AutoPtr<IView> viewA;
    GetPageAt(indexA, (IView**)&viewA);
    AutoPtr<IView> viewB;
    GetPageAt(indexB, (IView**)&viewB);

    if (TO_IINTERFACE(viewA) != TO_IINTERFACE(viewB) && viewA != NULL && viewB != NULL) {
        Int32 aleft;
        viewA->GetLeft(&aleft);
        Int32 bleft;
        viewB->GetLeft(&bleft);
        Int32 deltaX = aleft - bleft;
        viewA->OffsetLeftAndRight(-deltaX);
        viewB->OffsetLeftAndRight(deltaX);
    }
    return NOERROR;
}

ECode PagedView::StartPageWarp(
    /* [in] */ Int32 pageIndex)
{
    if (DEBUG_WARP) Logger::V(TAG, "START WARP");
    if (pageIndex != mCurrentPage + 1) {
        mPageSwapIndex = mCurrentPage + 1;
    }
    mPageWarpIndex = pageIndex;
    return NOERROR;
}

ECode PagedView::GetPageWarpIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    Int32 count;
    GetPageCount(&count);
    *index = count - 1;
    return NOERROR;
}

ECode PagedView::StopPageWarp()
{
    if (DEBUG_WARP) Logger::V(TAG, "END WARP");
    // mPageSwapIndex is reset in snapToPage() after the scroll animation completes
    return NOERROR;
}

ECode PagedView::OnPageBeginWarp()
{
    return NOERROR;
}

ECode PagedView::OnPageEndWarp()
{
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
