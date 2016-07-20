
#include "elastos/droid/launcher2/PagedView.h"
#include "elastos/droid/launcher2/LauncherAnimUtils.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::CVelocityTrackerHelper;
using Elastos::Droid::View::IVelocityTrackerHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::EIID_IViewGroupOnHierarchyChangeListener;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Animation::EIID_IInterpolator;
using Elastos::Droid::Widget::CScroller;
using Elastos::Core::CoreUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(PagedView::OnHierarchyChangeListener, Object, IViewGroupOnHierarchyChangeListener)

ECode PagedView::OnHierarchyChangeListener::OnChildViewAdded(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    return mHost->OnChildViewAdded(parent, child);
}

ECode PagedView::OnHierarchyChangeListener::OnChildViewRemoved(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    return mHost->OnChildViewRemoved(parent, child);
}

CAR_INTERFACE_IMPL_2(PagedView::ScrollInterpolator, Object, IInterpolator, ITimeInterpolator)

PagedView::ScrollInterpolator::ScrollInterpolator()
{
}

ECode PagedView::ScrollInterpolator::GetInterpolation(
    /* [in] */ Float t,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res);
    t -= 1.0f;
    *res = t*t*t*t*t + 1;
    return NOERROR;
}

ECode PagedView::ScrollInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

PagedView::SavedState::SavedState()
    : mCurrentPage(-1)
{}

ECode PagedView::SavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return BaseSavedState::constructor(superState);
}

ECode PagedView::SavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    BaseSavedState::WriteToParcel(dest);
    return dest->WriteInt32(mCurrentPage);
}

ECode PagedView::SavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    BaseSavedState::ReadFromParcel(source);
    return source->ReadInt32(&mCurrentPage);
}

ECode PagedView::HideScrollingIndicatorRunnable::Run()
{
    mHost->HideScrollingIndicator(FALSE);
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
        return mHost->mScrollIndicator->SetVisibility(IView::INVISIBLE);
    }
    return NOERROR;
}

const Int32 PagedView::AUTOMATIC_PAGE_SPACING;
const Int32 PagedView::INVALID_PAGE;
const Int32 PagedView::PAGE_SNAP_ANIMATION_DURATION;
const Int32 PagedView::MAX_PAGE_SNAP_DURATION;
const Int32 PagedView::SLOW_PAGE_SNAP_ANIMATION_DURATION;
const Float PagedView::NANOTIME_DIV;
const Int32 PagedView::TOUCH_STATE_REST;
const Int32 PagedView::TOUCH_STATE_SCROLLING;
const Int32 PagedView::TOUCH_STATE_PREV_PAGE;
const Int32 PagedView::TOUCH_STATE_NEXT_PAGE;
const Float PagedView::ALPHA_QUANTIZE_LEVEL;
const Int32 PagedView::INVALID_POINTER;
const Int32 PagedView::sScrollIndicatorFadeInDuration;
const Int32 PagedView::sScrollIndicatorFadeOutDuration;
const Int32 PagedView::sScrollIndicatorFlashDuration;

const String PagedView::TAG("PagedView");
const Boolean PagedView::DEBUG = FALSE;
const Int32 PagedView::MIN_LENGTH_FOR_FLING;
const Float PagedView::OVERSCROLL_ACCELERATE_FACTOR;
const Float PagedView::OVERSCROLL_DAMP_FACTOR;
const Float PagedView::RETURN_TO_ORIGINAL_PAGE_THRESHOLD;
const Float PagedView::SIGNIFICANT_MOVE_THRESHOLD;
const Int32 PagedView::FLING_THRESHOLD_VELOCITY;
const Int32 PagedView::MIN_SNAP_VELOCITY;
const Int32 PagedView::MIN_FLING_VELOCITY;

CAR_INTERFACE_IMPL(PagedView, ViewGroup, IPagedView)

PagedView::PagedView()
    : mFlingThresholdVelocity(0)
    , mMinFlingVelocity(0)
    , mMinSnapVelocity(0)
    , mDensity(0.0f)
    , mSmoothingTime(0.0f)
    , mTouchX(0.0f)
    , mFirstLayout(TRUE)
    , mCurrentPage(0)
    , mNextPage(INVALID_PAGE)
    , mMaxScrollX(0)
    , mLastMotionX(0.0f)
    , mLastMotionXRemainder(0.0f)
    , mLastMotionY(0.0f)
    , mTotalMotionX(0.0f)
    , mTouchState(TOUCH_STATE_REST)
    , mForceScreenScrolled(FALSE)
    , mAllowLongPress(TRUE)
    , mTouchSlop(0)
    , mPageSpacing(0)
    , mPageLayoutPaddingTop(0)
    , mPageLayoutPaddingBottom(0)
    , mPageLayoutPaddingLeft(0)
    , mPageLayoutPaddingRight(0)
    , mPageLayoutWidthGap(0)
    , mPageLayoutHeightGap(0)
    , mCellCountX(0)
    , mCellCountY(0)
    , mCenterPagesVertically(FALSE)
    , mAllowOverScroll(TRUE)
    , mUnboundedScrollX(0)
    , mForceDrawAllChildrenNextFrame(FALSE)
    , mOverScrollX(0)
    , mLayoutScale(1.0f)
    , mActivePointerId(INVALID_POINTER)
    , mContentIsRefreshable(TRUE)
    , mFadeInAdjacentScreens(TRUE)
    , mUsePagingTouchSlop(TRUE)
    , mDeferScrollUpdate(FALSE)
    , mIsPageMoving(FALSE)
    , mIsDataReady(FALSE)
    , mDownMotionX(0.0f)
    , mLastScreenCenter(-1)
    , mPagingTouchSlop(0)
    , mMaximumVelocity(0)
    , mMinimumWidth(0)
    , mScrollIndicatorPaddingLeft(0)
    , mScrollIndicatorPaddingRight(0)
    , mHasScrollIndicator(TRUE)
    , mShouldShowScrollIndicator(FALSE)
    , mShouldShowScrollIndicatorImmediately(FALSE)
    , mScrollingPaused(FALSE)
    , mDeferLoadAssociatedPagesUntilScrollCompletes(FALSE)
{
    mTempVisiblePagesRange = ArrayOf<Int32>::Alloc(2);
    mHideScrollingIndicatorRunnable = new HideScrollingIndicatorRunnable(this);
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
    FAIL_RETURN(ViewGroup::constructor(context, attrs, defStyle));

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(Elastos::Droid::Launcher2::R::styleable::PagedView),
            ArraySize(Elastos::Droid::Launcher2::R::styleable::PagedView));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a);
    Int32 size;
    a->GetDimensionPixelSize(R::styleable::PagedView_pageSpacing, 0, &size);
    SetPageSpacing(size);
    a->GetDimensionPixelSize(
        R::styleable::PagedView_pageLayoutPaddingTop, 0, &mPageLayoutPaddingTop);
    a->GetDimensionPixelSize(
        R::styleable::PagedView_pageLayoutPaddingBottom, 0, &mPageLayoutPaddingBottom);
    a->GetDimensionPixelSize(
        R::styleable::PagedView_pageLayoutPaddingLeft, 0, &mPageLayoutPaddingLeft);
    a->GetDimensionPixelSize(
        R::styleable::PagedView_pageLayoutPaddingRight, 0, &mPageLayoutPaddingRight);
    a->GetDimensionPixelSize(
        R::styleable::PagedView_pageLayoutWidthGap, 0, &mPageLayoutWidthGap);
    a->GetDimensionPixelSize(
        R::styleable::PagedView_pageLayoutHeightGap, 0, &mPageLayoutHeightGap);
    a->GetDimensionPixelSize(
        R::styleable::PagedView_scrollIndicatorPaddingLeft, 0, &mScrollIndicatorPaddingLeft);
    a->GetDimensionPixelSize(
        R::styleable::PagedView_scrollIndicatorPaddingRight, 0, &mScrollIndicatorPaddingRight);
    a->Recycle();

    SetHapticFeedbackEnabled(FALSE);
    Init();
    return NOERROR;
}

ECode PagedView::Init()
{
    CArrayList::New((IArrayList**)&mDirtyPageContent);
    mDirtyPageContent->EnsureCapacity(32);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ScrollInterpolator> interpolator = new ScrollInterpolator();
    CScroller::New(context, interpolator, (IScroller**)&mScroller);
    mCurrentPage = 0;
    mCenterPagesVertically = TRUE;

    AutoPtr<IViewConfigurationHelper> vcHelper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&vcHelper);
    AutoPtr<IViewConfiguration> configuration;
    vcHelper->Get(context, (IViewConfiguration**)&configuration);
    configuration->GetScaledTouchSlop(&mTouchSlop);
    configuration->GetScaledPagingTouchSlop(&mPagingTouchSlop);
    configuration->GetScaledMaximumFlingVelocity(&mMaximumVelocity);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    dm->GetDensity(&mDensity);

    mFlingThresholdVelocity = (Int32) (FLING_THRESHOLD_VELOCITY * mDensity);
    mMinFlingVelocity = (Int32) (MIN_FLING_VELOCITY * mDensity);
    mMinSnapVelocity = (Int32) (MIN_SNAP_VELOCITY * mDensity);
    AutoPtr<OnHierarchyChangeListener> listener = new OnHierarchyChangeListener(this);
    SetOnHierarchyChangeListener(listener);
    return NOERROR;
}

ECode PagedView::SetPageSwitchListener(
    /* [in] */ IPageSwitchListener* pageSwitchListener)
{
    mPageSwitchListener = pageSwitchListener;
    if (mPageSwitchListener != NULL) {
        mPageSwitchListener->OnPageSwitch(GetPageAt(mCurrentPage), mCurrentPage);
    }
    return NOERROR;
}

ECode PagedView::IsLayoutRtl(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 layoutDirection;
    GetLayoutDirection(&layoutDirection);
    *result = (layoutDirection == LAYOUT_DIRECTION_RTL);
    return NOERROR;
}

Boolean PagedView::IsLayoutRtl()
{
    Boolean res;
    IsLayoutRtl(&res);
    return res;
}

ECode PagedView::SetDataIsReady()
{
    mIsDataReady = TRUE;
    return NOERROR;
}

Boolean PagedView::IsDataReady()
{
    return mIsDataReady;
}

Int32 PagedView::GetCurrentPage()
{
    Int32 page;
    GetCurrentPage(&page);
    return page;
}

ECode PagedView::GetCurrentPage(
    /* [out] */ Int32* page)
{
    VALIDATE_NOT_NULL(page);

    *page = mCurrentPage;
    return NOERROR;
}

ECode PagedView::GetNextPage(
    /* [out] */ Int32* page)
{
    VALIDATE_NOT_NULL(page);

    *page = (mNextPage != INVALID_PAGE) ? mNextPage : mCurrentPage;
    return NOERROR;
}

Int32 PagedView::GetPageCount()
{
    Int32 count;
    GetPageCount(&count);
    return count;
}

ECode PagedView::GetPageCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    return GetChildCount(count);
}

ECode PagedView::GetPageAt(
    /* [in] */ Int32 index,
    /* [out] */ IView** page)
{
    return GetChildAt(index, page);
}

AutoPtr<IView> PagedView::GetPageAt(
    /* [in] */ Int32 index)
{
    AutoPtr<IView> view;
    GetPageAt(index, (IView**)&view);
    return view;
}

ECode PagedView::IndexToPage(
    /* [in] */ Int32 index,
    /* [out] */ Int32* page)
{
    VALIDATE_NOT_NULL(page);

    *page = index;
    return NOERROR;
}

ECode PagedView::UpdateCurrentPageScroll()
{
    // If the current page is invalid, just reset the scroll position to zero
    Int32 newX = 0;
    if (0 <= mCurrentPage && mCurrentPage < GetPageCount()) {
        Int32 offset = GetChildOffset(mCurrentPage);
        Int32 relOffset = GetRelativeChildOffset(mCurrentPage);
        newX = offset - relOffset;
    }
    ScrollTo(newX, 0);
    mScroller->SetFinalX(newX);
    mScroller->ForceFinished(TRUE);
    return NOERROR;
}

ECode PagedView::PauseScrolling()
{
    mScroller->ForceFinished(TRUE);
    CancelScrollingIndicatorAnimations();
    mScrollingPaused = TRUE;
    return NOERROR;
}

ECode PagedView::ResumeScrolling()
{
    mScrollingPaused = FALSE;
    return NOERROR;
}

ECode PagedView::SetCurrentPage(
    /* [in] */ Int32 currentPage)
{
    Boolean isFinished;
    mScroller->IsFinished(&isFinished);
    if (!isFinished) {
        mScroller->AbortAnimation();
    }
    // don't introduce any checks like mCurrentPage == currentPage here-- if we change the
    // the default
    if (GetPageCount() == 0) {
        return NOERROR;
    }

    using Elastos::Core::Math;
    mCurrentPage = Math::Max(0, Math::Min(currentPage, GetPageCount() - 1));
    UpdateCurrentPageScroll();
    UpdateScrollingIndicator();
    NotifyPageSwitchListener();
    Invalidate();
    return NOERROR;
}

ECode PagedView::NotifyPageSwitchListener()
{
    if (mPageSwitchListener != NULL) {
        mPageSwitchListener->OnPageSwitch(GetPageAt(mCurrentPage), mCurrentPage);
    }
    return NOERROR;
}

ECode PagedView::PageBeginMoving()
{
    if (!mIsPageMoving) {
        mIsPageMoving = TRUE;
        OnPageBeginMoving();
    }
    return NOERROR;
}

ECode PagedView::PageEndMoving()
{
    if (mIsPageMoving) {
        mIsPageMoving = FALSE;
        OnPageEndMoving();
    }
    return NOERROR;
}

Boolean PagedView::IsPageMoving()
{
    return mIsPageMoving;
}

// a method that subclasses can override to add behavior
ECode PagedView::OnPageBeginMoving()
{
    return NOERROR;
}

// a method that subclasses can override to add behavior
ECode PagedView::OnPageEndMoving()
{
    return NOERROR;
}

ECode PagedView::SetOnLongClickListener(
    /* [in] */ IViewOnLongClickListener* l)
{
    mLongClickListener = l;
    Int32 count = GetPageCount();
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> view = GetPageAt(i);
        view->SetOnLongClickListener(l);
    }
    return NOERROR;
}

ECode PagedView::ScrollBy(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 scrollY;
    GetScrollY(&scrollY);
    ScrollTo(mUnboundedScrollX + x, scrollY + y);
    return NOERROR;
}

ECode PagedView::ScrollTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Boolean isRtl = IsLayoutRtl();
    mUnboundedScrollX = x;

    Boolean isXBeforeFirstPage = isRtl ? (x > mMaxScrollX) : (x < 0);
    Boolean isXAfterLastPage = isRtl ? (x < 0) : (x > mMaxScrollX);
    if (isXBeforeFirstPage) {
        ViewGroup::ScrollTo(0, y);
        if (mAllowOverScroll) {
            if (isRtl) {
                OverScroll(x - mMaxScrollX);
            }
            else {
                OverScroll(x);
            }
        }
    }
    else if (isXAfterLastPage) {
        ViewGroup::ScrollTo(mMaxScrollX, y);
        if (mAllowOverScroll) {
            if (isRtl) {
                OverScroll(x);
            }
            else {
                OverScroll(x - mMaxScrollX);
            }
        }
    }
    else {
        mOverScrollX = x;
        ViewGroup::ScrollTo(x, y);
    }

    mTouchX = x;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 nanoTime;
    system->GetNanoTime(&nanoTime);
    mSmoothingTime = nanoTime / NANOTIME_DIV;
    return NOERROR;
}

// we moved this functionality to a helper function so SmoothPagedView can reuse it
Boolean PagedView::ComputeScrollHelper()
{
    Boolean res;
    mScroller->ComputeScrollOffset(&res);
    if (res) {
        // Don't bother scrolling if the page does not need to be moved
        Int32 scrollX, scrollY, currX, currY;
        GetScrollX(&scrollX);
        GetScrollY(&scrollY);
        mScroller->GetCurrX(&currX);
        mScroller->GetCurrY(&currY);
        if (scrollX != currX || scrollY != currY || mOverScrollX != currX) {
            ScrollTo(currX, currY);
        }
        Invalidate();
        return TRUE;
    }
    else if (mNextPage != INVALID_PAGE) {
        mCurrentPage = Elastos::Core::Math::Max(0,
            Elastos::Core::Math::Min(mNextPage, GetPageCount() - 1));
        mNextPage = INVALID_PAGE;
        NotifyPageSwitchListener();

        // Load the associated pages if necessary
        if (mDeferLoadAssociatedPagesUntilScrollCompletes) {
            LoadAssociatedPages(mCurrentPage);
            mDeferLoadAssociatedPagesUntilScrollCompletes = FALSE;
        }

        // We don't want to trigger a page end moving unless the page has settled
        // and the user has stopped scrolling
        if (mTouchState == TOUCH_STATE_REST) {
            PageEndMoving();
        }

        // Notify the user when the page changes
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IInterface> service;
        context->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&service);
        AutoPtr<IAccessibilityManager> accessibilityManager = IAccessibilityManager::Probe(service);
        Boolean isEnabled;
        accessibilityManager->IsEnabled(&isEnabled);
        if (isEnabled) {
            AutoPtr<IAccessibilityEventHelper> aeHelper;
            CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&aeHelper);
            AutoPtr<IAccessibilityEvent> ev;
            aeHelper->Obtain(IAccessibilityEvent::TYPE_VIEW_SCROLLED, (IAccessibilityEvent**)&ev);
            AutoPtr<IList> list;
            IAccessibilityRecord::Probe(ev)->GetText((IList**)&list);
            list->Add(CoreUtils::Convert(GetCurrentPageDescription()));
            SendAccessibilityEventUnchecked(ev);
        }
        return TRUE;
    }
    return FALSE;
}

ECode PagedView::ComputeScroll()
{
    ComputeScrollHelper();
    return NOERROR;
}

ECode PagedView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    if (!mIsDataReady) {
        return ViewGroup::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    }

    Int32 widthMode = MeasureSpec::GetMode(widthMeasureSpec);
    Int32 widthSize = MeasureSpec::GetSize(widthMeasureSpec);
    Int32 heightMode = MeasureSpec::GetMode(heightMeasureSpec);
    Int32 heightSize = MeasureSpec::GetSize(heightMeasureSpec);
    if (widthMode != MeasureSpec::EXACTLY) {
        Logger::E(TAG, "Workspace can only be used in EXACTLY mode.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // Return early if we aren't given a proper dimension
    if (widthSize <= 0 || heightSize <= 0) {
        ViewGroup::OnMeasure(widthMeasureSpec, heightMeasureSpec);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    /* Allow the height to be set as WRAP_CONTENT. This allows the particular case
     * of the All apps view on XLarge displays to not take up more space then it needs. Width
     * is still not allowed to be set as WRAP_CONTENT since many parts of the code expect
     * each page to have the same width.
     */
    Int32 maxChildHeight = 0;

    Int32 pl, pr, pt, pb;
    GetPaddingLeft(&pl);
    GetPaddingRight(&pr);
    GetPaddingTop(&pt);
    GetPaddingBottom(&pb);
    Int32 verticalPadding = pt + pb;
    Int32 horizontalPadding = pl + pr;


    // The children are given the same width and height as the workspace
    // unless they were set to WRAP_CONTENT
    if (DEBUG) Logger::D(TAG, "PagedView::onMeasure(): %d, %d", widthSize, heightSize);
    Int32 childCount = GetPageCount();
    for (Int32 i = 0; i < childCount; i++) {
        // disallowing padding in paged view (just pass 0)
        AutoPtr<IView> child = GetPageAt(i);
        AutoPtr<IViewGroupLayoutParams> _lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
        LayoutParams* lp = (LayoutParams*)_lp.Get();

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
        Int32 mw, mh;
        child->GetMeasuredWidth(&mw);
        child->GetMeasuredHeight(&mh);
        maxChildHeight = Elastos::Core::Math::Max(maxChildHeight, mh);
        if (DEBUG) Logger::D(TAG, "\tmeasure-child %d [%s]: %d, %d", i, TO_CSTR(child), mw, mh);
    }

    if (heightMode == MeasureSpec::AT_MOST) {
        heightSize = maxChildHeight + verticalPadding;
    }

    SetMeasuredDimension(widthSize, heightSize);

    // We can't call GetChildOffset/GetRelativeChildOffset until we set the measured dimensions.
    // We also wait until we set the measured dimensions before flushing the cache as well, to
    // ensure that the cache is filled with good values.
    InvalidateCachedOffsets();

    if (childCount > 0) {
        if (DEBUG) {
            Int32 mw;
            GetMeasuredWidth(&mw);
            Logger::D(TAG, "GetRelativeChildOffset(): %d, %d", mw, GetChildWidth(0));
        }

        // Calculate the variable page spacing if necessary
        if (mPageSpacing == AUTOMATIC_PAGE_SPACING) {
            // The gap between pages in the PagedView should be equal to the gap from the page
            // to the edge of the screen (so it is not visible in the current screen).  To
            // account for unequal padding on each side of the paged view, we take the maximum
            // of the left/right gap and use that as the gap between each page.
            Int32 offset = GetRelativeChildOffset(0);
            AutoPtr<IView> child;
            GetChildAt(0, (IView**)&child);
            Int32 mw;
            child->GetMeasuredWidth(&mw);
            Int32 spacing = Elastos::Core::Math::Max(offset, widthSize - offset - mw);
            SetPageSpacing(spacing);
        }
    }

    UpdateScrollingIndicatorPosition();

    if (childCount > 0) {
        Int32 index = IsLayoutRtl() ? 0 : childCount - 1;
        mMaxScrollX = GetChildOffset(index) - GetRelativeChildOffset(index);
    }
    else {
        mMaxScrollX = 0;
    }
    return NOERROR;
}

ECode PagedView::ScrollToNewPageWithoutMovingPages(
    /* [in] */ Int32 newCurrentPage)
{
    Int32 newX = GetChildOffset(newCurrentPage) - GetRelativeChildOffset(newCurrentPage);
    Int32 scrollX;
    GetScrollX(&scrollX);
    Int32 delta = newX - scrollX;

    Int32 pageCount = GetPageCount();
    for (Int32 i = 0; i < pageCount; i++) {
        AutoPtr<IView> page = GetPageAt(i);
        Float x;
        page->GetX(&x);
        page->SetX(x + delta);
    }
    SetCurrentPage(newCurrentPage);
    return NOERROR;
}

ECode PagedView::SetLayoutScale(
    /* [in] */ Float childrenScale)
{
    mLayoutScale = childrenScale;
    InvalidateCachedOffsets();

    // Now we need to do a re-layout, but preserving absolute X and Y coordinates
    Int32 childCount = GetPageCount();
    AutoPtr<ArrayOf<Float> > childrenX = ArrayOf<Float>::Alloc(childCount);
    AutoPtr<ArrayOf<Float> > childrenY = ArrayOf<Float>::Alloc(childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child = GetPageAt(i);
        child->GetX(&(*childrenX)[i]);
        child->GetY(&(*childrenY)[i]);
    }
    // Trigger a full re-layout (never just call onLayout directly!)
    Int32 measuredWidth, measuredHeight;
    GetMeasuredWidth(&measuredWidth);
    GetMeasuredHeight(&measuredHeight);
    Int32 widthSpec = MeasureSpec::MakeMeasureSpec(measuredWidth, MeasureSpec::EXACTLY);
    Int32 heightSpec = MeasureSpec::MakeMeasureSpec(measuredHeight, MeasureSpec::EXACTLY);
    RequestLayout();
    Measure(widthSpec, heightSpec);
    Int32 l, t, r, b;
    GetLeft(&l);
    GetTop(&t);
    GetRight(&r);
    GetBottom(&b);
    Layout(l, t, r, b);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child = GetPageAt(i);
        child->SetX((*childrenX)[i]);
        child->SetY((*childrenY)[i]);
    }

    // Also, the page offset has changed  (since the pages are now smaller);
    // update the page offset, but again preserving absolute X and Y coordinates
    ScrollToNewPageWithoutMovingPages(mCurrentPage);
    return NOERROR;
}

ECode PagedView::SetPageSpacing(
    /* [in] */ Int32 pageSpacing)
{
    mPageSpacing = pageSpacing;
    InvalidateCachedOffsets();
    return NOERROR;
}

ECode PagedView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    if (!mIsDataReady) {
        return NOERROR;
    }

    if (DEBUG) Logger::D(TAG, "PagedView::onLayout()");
    Int32 pTop;
    Int32 pBottom;
    GetPaddingTop(&pTop);
    GetPaddingBottom(&pBottom);
    Int32 verticalPadding = pTop + pBottom;
    Int32 childCount = GetPageCount();
    Boolean isRtl = IsLayoutRtl();

    Int32 startIndex = isRtl ? childCount - 1 : 0;
    Int32 endIndex = isRtl ? -1 : childCount;
    Int32 delta = isRtl ? -1 : 1;
    Int32 childLeft = GetRelativeChildOffset(startIndex);
    for (Int32 i = startIndex; i != endIndex; i += delta) {
        AutoPtr<IView> child = GetPageAt(i);
        Int32 v;
        child->GetVisibility(&v);
        if (v != IView::GONE) {
            Int32 childWidth = GetScaledMeasuredWidth(child);
            Int32 childHeight;
            child->GetMeasuredHeight(&childHeight);
            Int32 childTop;
            GetPaddingTop(&childTop);
            if (mCenterPagesVertically) {
                Int32 measuredHeight;
                GetMeasuredHeight(&measuredHeight);
                childTop += ((measuredHeight - verticalPadding) - childHeight) / 2;
            }

            if (DEBUG) Logger::D(TAG, "\tlayout-child %d [%s]: %d, %d", i, TO_CSTR(child), childLeft, childTop);
            Int32 measuredWidth;
            child->GetMeasuredWidth(&measuredWidth);
            child->Layout(childLeft, childTop, childLeft + measuredWidth,
                childTop + childHeight);
            childLeft += childWidth + mPageSpacing;
        }
    }

    if (mFirstLayout && mCurrentPage >= 0 && mCurrentPage < GetPageCount()) {
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
    if (IsScrollingIndicatorEnabled()) {
        UpdateScrollingIndicator();
    }
    Boolean isInOverscroll = mOverScrollX < 0 || mOverScrollX > mMaxScrollX;

    if (mFadeInAdjacentScreens && !isInOverscroll) {
        Int32 childCount = GetPageCount();
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> child = GetPageAt(i);
            if (child != NULL) {
                Float scrollProgress = GetScrollProgress(screenCenter, child, i);
                Float alpha = 1 - Elastos::Core::Math::Abs(scrollProgress);
                child->SetAlpha(alpha);
            }
        }
        Invalidate();
    }
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
    InvalidateCachedOffsets();
    return NOERROR;
}

ECode PagedView::OnChildViewRemoved(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    return NOERROR;
}

ECode PagedView::InvalidateCachedOffsets()
{
    Int32 count = GetPageCount();
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

Int32 PagedView::GetChildOffset(
    /* [in] */ Int32 index)
{
    Boolean isRtl = IsLayoutRtl();
    AutoPtr<ArrayOf<Int32> > childOffsets = Elastos::Core::Math::Compare(mLayoutScale, 1.0f) == 0 ?
            mChildOffsets : mChildOffsetsWithLayoutScale;

    if (childOffsets != NULL && (*childOffsets)[index] != -1) {
        return (*childOffsets)[index];
    }
    else {
        if (GetPageCount() == 0)
            return 0;

        Int32 startIndex = isRtl ? GetPageCount() - 1 : 0;
        Int32 endIndex = isRtl ? index : index;
        Int32 delta = isRtl ? -1 : 1;
        Int32 offset = GetRelativeChildOffset(startIndex);
        for (Int32 i = startIndex; i != endIndex; i += delta) {
            offset += GetScaledMeasuredWidth(GetPageAt(i)) + mPageSpacing;
        }
        if (childOffsets != NULL) {
            (*childOffsets)[index] = offset;
        }
        return offset;
    }
}

Int32 PagedView::GetRelativeChildOffset(
    /* [in] */ Int32 index)
{
    if (mChildRelativeOffsets != NULL && (*mChildRelativeOffsets)[index] != -1) {
        return (*mChildRelativeOffsets)[index];
    }
    else {
        Int32 left, right;
        GetPaddingLeft(&left);
        GetPaddingRight(&right);
        Int32 measuredWidth;
        GetMeasuredWidth(&measuredWidth);
        Int32 padding = left + right;
        Int32 offset = left + (measuredWidth - padding - GetChildWidth(index)) / 2;
        if (mChildRelativeOffsets != NULL) {
            (*mChildRelativeOffsets)[index] = offset;
        }
        return offset;
    }
}

Int32 PagedView::GetScaledMeasuredWidth(
    /* [in] */ IView* child)
{
    // This functions are called enough times that it actually makes a difference in the
    // profiler -- so just inline the max() here
    Int32 measuredWidth;
    child->GetMeasuredWidth(&measuredWidth);
    Int32 minWidth = mMinimumWidth;
    Int32 maxWidth = (minWidth > measuredWidth) ? minWidth : measuredWidth;
    return (Int32) (maxWidth * mLayoutScale + 0.5f);
}

ECode PagedView::GetVisiblePages(
    /* [in] */ ArrayOf<Int32>* range)
{
    Boolean isRtl = IsLayoutRtl();
    Int32 pageCount = GetPageCount();

    if (pageCount > 0) {
        Int32 screenWidth;
        GetMeasuredWidth(&screenWidth);
        Int32 leftScreen = isRtl ? pageCount - 1 : 0;
        Int32 rightScreen = 0;
        Int32 endIndex = isRtl ? 0 : pageCount - 1;
        Int32 delta = isRtl ? -1 : 1;
        AutoPtr<IView> currPage = GetPageAt(leftScreen);
        Int32 scrollX;
        Int32 width;
        Int32 left;
        Int32 right;
        GetScrollX(&scrollX);
        Float x;
        while (leftScreen != endIndex &&
            (currPage->GetX(&x), x) + (currPage->GetWidth(&width), width) -
            (currPage->GetPaddingRight(&right), right) < scrollX) {
            leftScreen += delta;
            currPage = GetPageAt(leftScreen);
        }
        rightScreen = leftScreen;
        currPage = GetPageAt(rightScreen + delta);
        while (rightScreen != endIndex &&
            (currPage->GetX(&x), x) - (currPage->GetPaddingLeft(&left), left)
            < scrollX + screenWidth) {
            rightScreen += delta;
            currPage = GetPageAt(rightScreen + delta);
        }
        (*range)[0] = Elastos::Core::Math::Min(leftScreen, rightScreen);
        (*range)[1] = Elastos::Core::Math::Max(leftScreen, rightScreen);
    }
    else {
        (*range)[0] = -1;
        (*range)[1] = -1;
    }
    return NOERROR;
}

Boolean PagedView::ShouldDrawChild(
    /* [in] */ IView* child)
{
    Float alpha;
    child->GetAlpha(&alpha);
    return alpha > 0;
}

ECode PagedView::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    Int32 measuredWidth;
    GetMeasuredWidth(&measuredWidth);
    Int32 halfScreenSize = measuredWidth / 2;
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
    Int32 pageCount = GetPageCount();
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
            Int32 scrollX, scrollY;
            GetScrollX(&scrollX);
            GetScrollY(&scrollY);
            Int32 l, t, r, b;
            GetLeft(&l);
            GetTop(&t);
            GetRight(&r);
            GetBottom(&b);
            Boolean res;
            canvas->ClipRect(scrollX, scrollY, scrollX + r - l, scrollY + b - t, &res);

            for (Int32 i = GetPageCount() - 1; i >= 0; i--) {
                AutoPtr<IView> v = GetPageAt(i);
                if (mForceDrawAllChildrenNextFrame ||
                    (leftScreen <= i && i <= rightScreen && ShouldDrawChild(v))) {
                    DrawChild(canvas, v, drawingTime);
                }
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
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 index;
    IndexOfChild(child, &index);
    Int32 page;
    IndexToPage(index, &page);
    Boolean isFinished;
    if (page != mCurrentPage || !(mScroller->IsFinished(&isFinished), isFinished)) {
        SnapToPage(page);
        *res = TRUE;
        return NOERROR;
    }
    *res = FALSE;
    return NOERROR;
}

Boolean PagedView::OnRequestFocusInDescendants(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    Int32 focusablePage;
    if (mNextPage != INVALID_PAGE) {
        focusablePage = mNextPage;
    }
    else {
        focusablePage = mCurrentPage;
    }
    AutoPtr<IView> v = GetPageAt(focusablePage);
    if (v != NULL) {
        Boolean res;
        v->RequestFocus(direction, previouslyFocusedRect, &res);
        return res;
    }
    return FALSE;
}

ECode PagedView::DispatchUnhandledMove(
    /* [in] */ IView* focused,
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    // XXX-RTL: This will be fixed in a future CL
    if (direction == IView::FOCUS_LEFT) {
        if (GetCurrentPage() > 0) {
            SnapToPage(GetCurrentPage() - 1);
            *res = TRUE;
            return NOERROR;
        }
    }
    else if (direction == IView::FOCUS_RIGHT) {
        if (GetCurrentPage() < GetPageCount() - 1) {
            SnapToPage(GetCurrentPage() + 1);
            *res = TRUE;
            return NOERROR;
        }
    }
    return ViewGroup::DispatchUnhandledMove(focused, direction, res);
}

ECode PagedView::AddFocusables(
    /* [in] */ IArrayList* views,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 focusableMode)
{
    // XXX-RTL: This will be fixed in a future CL
    if (mCurrentPage >= 0 && mCurrentPage < GetPageCount()) {
        GetPageAt(mCurrentPage)->AddFocusables(views, direction, focusableMode);
    }
    if (direction == IView::FOCUS_LEFT) {
        if (mCurrentPage > 0) {
            GetPageAt(mCurrentPage - 1)->AddFocusables(views, direction, focusableMode);
        }
    }
    else if (direction == IView::FOCUS_RIGHT){
        if (mCurrentPage < GetPageCount() - 1) {
            GetPageAt(mCurrentPage + 1)->AddFocusables(views, direction, focusableMode);
        }
    }
    return NOERROR;
}

/**
 * If one of our descendant views decides that it could be focused now, only
 * pass that along if it's on the current page.
 *
 * This happens when live folders requery, and if they're off page, they
 * end up calling requestFocus, which pulls it on page.
 */
ECode PagedView::FocusableViewAvailable(
    /* [in] */ IView* focused)
{
    AutoPtr<IView> current = GetPageAt(mCurrentPage);
    AutoPtr<IView> v = focused;
    while (TRUE) {
        if (v == current) {
            return ViewGroup::FocusableViewAvailable(focused);
        }
        if (v.Get() == (IView*)this) {
            return NOERROR;
        }
        AutoPtr<IViewParent> parent;
        v->GetParent((IViewParent**)&parent);
        if (IView::Probe(parent)) {
            v = IView::Probe(parent);
        }
        else {
            return NOERROR;
        }
    }
    return NOERROR;
}

/**
 * {@inheritDoc}
 */
ECode PagedView::RequestDisallowInterceptTouchEvent(
    /* [in] */ Boolean disallowIntercept)
{
    if (disallowIntercept) {
        // We need to make sure to cancel our long press if
        // a scrollable widget takes over touch events
        AutoPtr<IView> currentPage = GetPageAt(mCurrentPage);
        currentPage->CancelLongPress();
    }
    return ViewGroup::RequestDisallowInterceptTouchEvent(disallowIntercept);
}

/**
 * Return TRUE if a tap at (x, y) should trigger a flip to the previous page.
 */
Boolean PagedView::HitsPreviousPage(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    if (IsLayoutRtl()) {
        Int32 measuredWidth;
        GetMeasuredWidth(&measuredWidth);
        return (x > (measuredWidth - GetRelativeChildOffset(mCurrentPage) + mPageSpacing));
    }
    else {
        return (x < GetRelativeChildOffset(mCurrentPage) - mPageSpacing);
    }
}

/**
 * Return TRUE if a tap at (x, y) should trigger a flip to the next page.
 */
Boolean PagedView::HitsNextPage(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    if (IsLayoutRtl()) {
        return (x < GetRelativeChildOffset(mCurrentPage) - mPageSpacing);
    }
    else {
        Int32 measuredWidth;
        GetMeasuredWidth(&measuredWidth);
        return (x > (measuredWidth - GetRelativeChildOffset(mCurrentPage) + mPageSpacing));
    }

}

ECode PagedView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    /*
     * This method JUST determines whether we want to intercept the motion.
     * If we return TRUE, onTouchEvent will be called and we do the actual
     * scrolling there.
     */
    AcquireVelocityTrackerAndAddMovement(ev);

    // Skip touch handling if there are no pages to swipe
    if (GetPageCount() <= 0) return ViewGroup::OnInterceptTouchEvent(ev, res);

    /*
     * Shortcut the most recurring case: the user is in the dragging
     * state and he is moving his finger.  We want to intercept this
     * motion.
     */
    Int32 action;
    ev->GetAction(&action);
    if ((action == IMotionEvent::ACTION_MOVE) &&
            (mTouchState == TOUCH_STATE_SCROLLING)) {
        *res = TRUE;
        return NOERROR;
    }

    switch (action & IMotionEvent::ACTION_MASK) {
        case IMotionEvent::ACTION_MOVE: {
            /*
             * mIsBeingDragged == FALSE, otherwise the shortcut would have caught it. Check
             * whether the user has moved far enough from his original down touch.
             */
            if (mActivePointerId != INVALID_POINTER) {
                DetermineScrollingStart(ev);
                break;
            }
            // if mActivePointerId is INVALID_POINTER, then we must have missed an ACTION_DOWN
            // event. in that case, treat the first occurence of a move event as a ACTION_DOWN
            // i.e. fall through to the next case (don't break)
            // (We sometimes miss ACTION_DOWN events in Workspace because it ignores all events
            // while it's small- this was causing a crash before we checked for INVALID_POINTER)
        }

        case IMotionEvent::ACTION_DOWN: {
            Float x, y;
            ev->GetX(&x);
            ev->GetY(&y);
            // Remember location of down touch
            mDownMotionX = x;
            mLastMotionX = x;
            mLastMotionY = y;
            mLastMotionXRemainder = 0;
            mTotalMotionX = 0;
            ev->GetPointerId(0, &mActivePointerId);
            mAllowLongPress = TRUE;

            /*
             * If being flinged and user touches the screen, initiate drag;
             * otherwise don't.  mScroller->IsFinished should be FALSE when
             * being flinged.
             */
            Int32 finalX;
            Int32 currX;
            mScroller->GetFinalX(&finalX);
            mScroller->GetCurrX(&currX);
            Int32 xDist = Elastos::Core::Math::Abs(finalX - currX);
            Boolean isFinished;
            mScroller->IsFinished(&isFinished);
            Boolean finishedScrolling = (isFinished || xDist < mTouchSlop);
            if (finishedScrolling) {
                mTouchState = TOUCH_STATE_REST;
                mScroller->AbortAnimation();
            }
            else {
                mTouchState = TOUCH_STATE_SCROLLING;
            }

            // check if this can be the beginning of a tap on the side of the pages
            // to scroll the current page
            if (mTouchState != TOUCH_STATE_PREV_PAGE && mTouchState != TOUCH_STATE_NEXT_PAGE) {
                if (GetPageCount() > 0) {
                    if (HitsPreviousPage(x, y)) {
                        mTouchState = TOUCH_STATE_PREV_PAGE;
                    }
                    else if (HitsNextPage(x, y)) {
                        mTouchState = TOUCH_STATE_NEXT_PAGE;
                    }
                }
            }
            break;
        }

        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            mTouchState = TOUCH_STATE_REST;
            mAllowLongPress = FALSE;
            mActivePointerId = INVALID_POINTER;
            ReleaseVelocityTracker();
            break;

        case IMotionEvent::ACTION_POINTER_UP:
            OnSecondaryPointerUp(ev);
            ReleaseVelocityTracker();
            break;
    }

    /*
     * The only time we want to intercept motion events is if we are in the
     * drag mode.
     */
    *res = mTouchState != TOUCH_STATE_REST;
    return NOERROR;
}

ECode PagedView::DetermineScrollingStart(
    /* [in] */ IMotionEvent* ev)
{
    return DetermineScrollingStart(ev, 1.0f);
}

/*
 * Determines if we should change the touch state to start scrolling after the
 * user moves their touch point too far.
 */
ECode PagedView::DetermineScrollingStart(
    /* [in] */ IMotionEvent* ev,
    /* [in] */ Float touchSlopScale)
{
    /*
     * Locally do absolute value. mLastMotionX is set to the y value
     * of the down event.
     */
    Int32 pointerIndex;
    ev->FindPointerIndex(mActivePointerId, &pointerIndex);
    if (pointerIndex == -1) {
        return NOERROR;
    }
    Float x, y;
    ev->GetX(pointerIndex, &x);
    ev->GetY(pointerIndex, &y);
    Int32 xDiff = (Int32) Elastos::Core::Math::Abs(x - mLastMotionX);
    Int32 yDiff = (Int32) Elastos::Core::Math::Abs(y - mLastMotionY);

    Int32 touchSlop = Elastos::Core::Math::Round(touchSlopScale * mTouchSlop);
    Boolean xPaged = xDiff > mPagingTouchSlop;
    Boolean xMoved = xDiff > touchSlop;
    Boolean yMoved = yDiff > touchSlop;

    if (xMoved || xPaged || yMoved) {
        if (mUsePagingTouchSlop ? xPaged : xMoved) {
            // Scroll if the user moved far enough along the X axis
            mTouchState = TOUCH_STATE_SCROLLING;
            mTotalMotionX += Elastos::Core::Math::Abs(mLastMotionX - x);
            mLastMotionX = x;
            mLastMotionXRemainder = 0;
            Int32 _x;
            GetScrollX(&_x);
            mTouchX = _x;
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            Int64 nanoTime;
            system->GetNanoTime(&nanoTime);
            mSmoothingTime = nanoTime / NANOTIME_DIV;
            PageBeginMoving();
        }
        // Either way, cancel any pending longpress
        CancelCurrentPageLongPress();
    }
    return NOERROR;
}

ECode PagedView::CancelCurrentPageLongPress()
{
    if (mAllowLongPress) {
        mAllowLongPress = FALSE;
        // Try canceling the long press. It could also have been scheduled
        // by a distant descendant, so use the mAllowLongPress flag to block
        // everything
        AutoPtr<IView> currentPage = GetPageAt(mCurrentPage);
        if (currentPage != NULL) {
            currentPage->CancelLongPress();
        }
    }
    return NOERROR;
}

Float PagedView::GetScrollProgress(
    /* [in] */ Int32 screenCenter,
    /* [in] */ IView* v,
    /* [in] */ Int32 page)
{
    Int32 measuredWidth;
    GetMeasuredWidth(&measuredWidth);
    Int32 halfScreenSize = measuredWidth / 2;

    Int32 totalDistance = GetScaledMeasuredWidth(v) + mPageSpacing;
    Int32 delta = screenCenter - (GetChildOffset(page) -
            GetRelativeChildOffset(page) + halfScreenSize);

    Float scrollProgress = delta / (totalDistance * 1.0f);
    scrollProgress = Elastos::Core::Math::Min(scrollProgress, 1.0f);
    scrollProgress = Elastos::Core::Math::Max(scrollProgress, -1.0f);
    return scrollProgress;
}

// This curve determines how the effect of scrolling over the limits of the page dimishes
// as the user pulls further and further from the bounds
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
    GetMeasuredWidth(&screenSize);

    // We want to reach the max over scroll effect when the user has
    // over scrolled half the size of the screen
    Float f = OVERSCROLL_ACCELERATE_FACTOR * (amount / screenSize);

    if (f == 0) return NOERROR;

    // Clamp this factor, f, to -1 < f < 1
    if (Elastos::Core::Math::Abs(f) >= 1) {
        f /= Elastos::Core::Math::Abs(f);
    }

    Int32 overScrollAmount = (Int32) Elastos::Core::Math::Round(f * screenSize);
    Int32 scrollY;
    GetScrollY(&scrollY);
    if (amount < 0) {
        mOverScrollX = overScrollAmount;
        ViewGroup::ScrollTo(0, scrollY);
    }
    else {
        mOverScrollX = mMaxScrollX + overScrollAmount;
        ViewGroup::ScrollTo(mMaxScrollX, scrollY);
    }
    Invalidate();
    return NOERROR;
}

ECode PagedView::DampedOverScroll(
    /* [in] */ Float amount)
{
    Int32 screenSize;
    GetMeasuredWidth(&screenSize);

    Float f = (amount / screenSize);

    if (f == 0) return NOERROR;
    f = f / (Elastos::Core::Math::Abs(f)) * (OverScrollInfluenceCurve(Elastos::Core::Math::Abs(f)));

    // Clamp this factor, f, to -1 < f < 1
    if (Elastos::Core::Math::Abs(f) >= 1) {
        f /= Elastos::Core::Math::Abs(f);
    }

    Int32 overScrollAmount = (Int32) Elastos::Core::Math::Round(OVERSCROLL_DAMP_FACTOR * f * screenSize);
    Int32 scrollY;
    GetScrollY(&scrollY);
    if (amount < 0) {
        mOverScrollX = overScrollAmount;
        ViewGroup::ScrollTo(0, scrollY);
    }
    else {
        mOverScrollX = mMaxScrollX + overScrollAmount;
        ViewGroup::ScrollTo(mMaxScrollX, scrollY);
    }
    Invalidate();
    return NOERROR;
}

ECode PagedView::OverScroll(
    /* [in] */ Float amount)
{
    return DampedOverScroll(amount);
}

Float PagedView::MaxOverScroll()
{
    // Using the formula in overScroll, assuming that f = 1.0 (which it should generally not
    // exceed). Used to find out how much extra wallpaper we need for the over scroll effect
    Float f = 1.0f;
    f = f / (Elastos::Core::Math::Abs(f)) * (OverScrollInfluenceCurve(Elastos::Core::Math::Abs(f)));
    return OVERSCROLL_DAMP_FACTOR * f;
}

ECode PagedView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    // Skip touch handling if there are no pages to swipe
    if (GetPageCount() <= 0) return ViewGroup::OnTouchEvent(ev, res);

    AcquireVelocityTrackerAndAddMovement(ev);

    Int32 action;
    ev->GetAction(&action);

    switch (action & IMotionEvent::ACTION_MASK) {
    case IMotionEvent::ACTION_DOWN:
        /*
         * If being flinged and user touches, stop the fling. isFinished
         * will be FALSE if being flinged.
         */
        Boolean isFinished;
        mScroller->IsFinished(&isFinished);
        if (!isFinished) {
            mScroller->AbortAnimation();
        }

        // Remember where the motion event started
        ev->GetX(&mLastMotionX);
        mDownMotionX = mLastMotionX;
        mLastMotionXRemainder = 0;
        mTotalMotionX = 0;
        ev->GetPointerId(0, &mActivePointerId);
        if (mTouchState == TOUCH_STATE_SCROLLING) {
            PageBeginMoving();
        }
        break;

    case IMotionEvent::ACTION_MOVE:
        if (mTouchState == TOUCH_STATE_SCROLLING) {
            // Scroll to follow the motion event
            Int32 pointerIndex;
            ev->FindPointerIndex(mActivePointerId, &pointerIndex);
            Float x;
            ev->GetX(pointerIndex, &x);
            Float deltaX = mLastMotionX + mLastMotionXRemainder - x;

            mTotalMotionX += Elastos::Core::Math::Abs(deltaX);

            // Only scroll and update mLastMotionX if we have moved some discrete amount.  We
            // keep the remainder because we are actually testing if we've moved from the last
            // scrolled position (which is discrete).
            if (Elastos::Core::Math::Abs(deltaX) >= 1.0f) {
                mTouchX += deltaX;
                AutoPtr<ISystem> system;
                CSystem::AcquireSingleton((ISystem**)&system);
                Int64 nanoTime;
                system->GetNanoTime(&nanoTime);
                mSmoothingTime = nanoTime / NANOTIME_DIV;
                if (!mDeferScrollUpdate) {
                    ScrollBy((Int32) deltaX, 0);
                    if (DEBUG) Logger::D(TAG, "onTouchEvent().Scrolling: %f", deltaX);
                }
                else {
                    Invalidate();
                }
                mLastMotionX = x;
                mLastMotionXRemainder = deltaX - (Int32) deltaX;
            }
            else {
                AwakenScrollBars();
            }
        }
        else {
            DetermineScrollingStart(ev);
        }
        break;

    case IMotionEvent::ACTION_UP:
        if (mTouchState == TOUCH_STATE_SCROLLING) {
            Int32 activePointerId = mActivePointerId;
            Int32 pointerIndex;
            ev->FindPointerIndex(activePointerId, &pointerIndex);
            Float x;
            ev->GetX(pointerIndex, &x);
            AutoPtr<IVelocityTracker> velocityTracker = mVelocityTracker;
            velocityTracker->ComputeCurrentVelocity(1000, mMaximumVelocity);
            Float xv;
            velocityTracker->GetXVelocity(activePointerId, &xv);
            Int32 velocityX = (Int32) xv;
            Int32 deltaX = (Int32) (x - mDownMotionX);
            Int32 pageWidth = GetScaledMeasuredWidth(GetPageAt(mCurrentPage));
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
                    Elastos::Core::Math::Signum(velocityX) != Elastos::Core::Math::Signum(deltaX) && isFling) {
                returnToOriginalPage = TRUE;
            }

            Int32 finalPage;
            // We give flings precedence over large moves, which is why we short-circuit our
            // test for a large move if a fling has been registered. That is, a large
            // move to the left and fling to the right will register as a fling to the right.
            Boolean isRtl = IsLayoutRtl();
            Boolean isDeltaXLeft = isRtl ? deltaX > 0 : deltaX < 0;
            Boolean isVelocityXLeft = isRtl ? velocityX > 0 : velocityX < 0;
            if (((isSignificantMove && !isDeltaXLeft && !isFling) ||
                    (isFling && !isVelocityXLeft)) && mCurrentPage > 0) {
                finalPage = returnToOriginalPage ? mCurrentPage : mCurrentPage - 1;
                SnapToPageWithVelocity(finalPage, velocityX);
            }
            else if (((isSignificantMove && isDeltaXLeft && !isFling) ||
                    (isFling && isVelocityXLeft)) &&
                    mCurrentPage < GetPageCount() - 1) {
                finalPage = returnToOriginalPage ? mCurrentPage : mCurrentPage + 1;
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
            Int32 nextPage = Elastos::Core::Math::Min(GetPageCount() - 1, mCurrentPage + 1);
            if (nextPage != mCurrentPage) {
                SnapToPage(nextPage);
            }
            else {
                SnapToDestination();
            }
        }
        else {
            OnUnhandledTap(ev);
        }
        mTouchState = TOUCH_STATE_REST;
        mActivePointerId = INVALID_POINTER;
        ReleaseVelocityTracker();
        break;

    case IMotionEvent::ACTION_CANCEL:
        if (mTouchState == TOUCH_STATE_SCROLLING) {
            SnapToDestination();
        }
        mTouchState = TOUCH_STATE_REST;
        mActivePointerId = INVALID_POINTER;
        ReleaseVelocityTracker();
        break;

    case IMotionEvent::ACTION_POINTER_UP:
        OnSecondaryPointerUp(ev);
        break;
    }

    *res = TRUE;
    return NOERROR;
}

ECode PagedView::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
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
                Int32 metaState;
                event->GetMetaState(&metaState);
                if ((metaState & IKeyEvent::META_SHIFT_ON) != 0) {
                    vscroll = 0;
                    event->GetAxisValue(IMotionEvent::AXIS_VSCROLL, &hscroll);
                }
                else {
                    event->GetAxisValue(IMotionEvent::AXIS_VSCROLL, &vscroll);
                    vscroll = -vscroll;
                    event->GetAxisValue(IMotionEvent::AXIS_HSCROLL, &hscroll);
                }
                if (hscroll != 0 || vscroll != 0) {
                    Boolean isForwardScroll = IsLayoutRtl() ? (hscroll < 0 || vscroll < 0)
                                                     : (hscroll > 0 || vscroll > 0);
                    if (isForwardScroll) {
                        ScrollRight();
                    }
                    else {
                        ScrollLeft();
                    }
                    *res = TRUE;
                    return NOERROR;
                }
            }
        }
    }
    return ViewGroup::OnGenericMotionEvent(event, res);
}

void PagedView::AcquireVelocityTrackerAndAddMovement(
    /* [in] */ IMotionEvent* ev)
{
    if (mVelocityTracker == NULL) {
        AutoPtr<IVelocityTrackerHelper> vtHelper;
        CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&vtHelper);
        vtHelper->Obtain((IVelocityTracker**)&mVelocityTracker);
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

ECode PagedView::OnUnhandledTap(
    /* [in] */ IMotionEvent* ev)
{
    return NOERROR;
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
    Boolean res;
    if (page >= 0 && page != GetCurrentPage() && (IsInTouchMode(&res), !res)) {
        SnapToPage(page);
    }
    return NOERROR;
}

Int32 PagedView::GetChildIndexForRelativeOffset(
    /* [in] */ Int32 relativeOffset)
{
    Boolean isRtl = IsLayoutRtl();
    Int32 childCount = GetPageCount();
    Int32 left;
    Int32 right;
    Int32 startIndex = isRtl ? childCount - 1 : 0;
    Int32 endIndex = isRtl ? -1 : childCount;
    Int32 delta = isRtl ? -1 : 1;
    for (Int32 i = startIndex; i != endIndex; i += delta) {
        left = GetRelativeChildOffset(i);
        right = (left + GetScaledMeasuredWidth(GetPageAt(i)));
        if (left <= relativeOffset && relativeOffset <= right) {
            return i;
        }
    }
    return -1;
}

Int32 PagedView::GetChildWidth(
    /* [in] */ Int32 index)
{
    // This functions are called enough times that it actually makes a difference in the
    // profiler -- so just inline the max() here
    Int32 measuredWidth;
    GetPageAt(index)->GetMeasuredWidth(&measuredWidth);
    Int32 minWidth = mMinimumWidth;
    return (minWidth > measuredWidth) ? minWidth : measuredWidth;
}

ECode PagedView::GetPageNearestToCenterOfScreen(
    /* [out] */ Int32* page)
{
    VALIDATE_NOT_NULL(page);

    Int32 minDistanceFromScreenCenter = Elastos::Core::Math::INT32_MAX_VALUE;
    Int32 minDistanceFromScreenCenterIndex = -1;
    Int32 scrollX;
    GetScrollX(&scrollX);
    Int32 measuredWidth;
    GetMeasuredWidth(&measuredWidth);
    Int32 screenCenter = scrollX + (measuredWidth / 2);
    Int32 childCount = GetPageCount();
    for (Int32 i = 0; i < childCount; ++i) {
        AutoPtr<IView> layout = GetPageAt(i);
        Int32 childWidth = GetScaledMeasuredWidth(layout);
        Int32 halfChildWidth = (childWidth / 2);
        Int32 childCenter = GetChildOffset(i) + halfChildWidth;
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
    Int32 page;
    GetPageNearestToCenterOfScreen(&page);
    return SnapToPage(page, PAGE_SNAP_ANIMATION_DURATION);
}

ECode PagedView::DistanceInfluenceForSnapDuration(
    /* [in] */ Float f,
    /* [out] */ Float* distance)
{
    VALIDATE_NOT_NULL(distance);

    f -= 0.5f; // center the values about 0.
    f *= 0.3f * Elastos::Core::Math::PI / 2.0f;
    *distance = (Float) Elastos::Core::Math::Sin(f);
    return NOERROR;
}

ECode PagedView::SnapToPageWithVelocity(
    /* [in] */ Int32 whichPage,
    /* [in] */ Int32 velocity)
{
    whichPage = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(whichPage, GetPageCount() - 1));
    Int32 measuredWidth;
    GetMeasuredWidth(&measuredWidth);
    Int32 halfScreenSize = measuredWidth / 2;

    if (DEBUG) Logger::D(TAG, "SnapToPage.GetChildOffset(): %d", GetChildOffset(whichPage));
    if (DEBUG) Logger::D(TAG, "SnapToPageWithVelocity.GetRelativeChildOffset(): %d, %d",
            measuredWidth, GetChildWidth(whichPage));
    Int32 newX = GetChildOffset(whichPage) - GetRelativeChildOffset(whichPage);
    Int32 delta = newX - mUnboundedScrollX;
    Int32 duration = 0;

    if (Elastos::Core::Math::Abs(velocity) < mMinFlingVelocity) {
        // If the velocity is low enough, then treat this more as an automatic page advance
        // as opposed to an apparent physical response to flinging
        SnapToPage(whichPage, PAGE_SNAP_ANIMATION_DURATION);
        return NOERROR;
    }

    // Here we compute a "distance" that will be used in the computation of the overall
    // snap duration. This is a function of the actual distance that needs to be traveled;
    // we keep this value close to half screen size in order to reduce the variance in snap
    // duration as a function of the distance the page needs to travel.
    Float distanceRatio = Elastos::Core::Math::Min(1.0f, 1.0f * Elastos::Core::Math::Abs(delta) / (2 * halfScreenSize));
    Float _distance;
    DistanceInfluenceForSnapDuration(distanceRatio, &_distance);
    Float distance = halfScreenSize + halfScreenSize * _distance;

    velocity = Elastos::Core::Math::Abs(velocity);
    velocity = Elastos::Core::Math::Max(mMinSnapVelocity, velocity);

    // we want the page's snap velocity to approximately match the velocity at which the
    // user flings, so we scale the duration by a value near to the derivative of the scroll
    // interpolator at zero, ie. 5. We use 4 to make it a little slower.
    duration = 4 * Elastos::Core::Math::Round(1000 * Elastos::Core::Math::Abs(distance / velocity));
    duration = Elastos::Core::Math::Min(duration, MAX_PAGE_SNAP_DURATION);

    SnapToPage(whichPage, delta, duration);
    return NOERROR;
}

ECode PagedView::SnapToPage(
    /* [in] */ Int32 whichPage)
{
    return SnapToPage(whichPage, PAGE_SNAP_ANIMATION_DURATION);
}

ECode PagedView::SnapToPage(
    /* [in] */ Int32 whichPage,
    /* [in] */ Int32 duration)
{
    whichPage = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(whichPage, GetPageCount() - 1));

    if (DEBUG) Logger::D(TAG, "SnapToPage.GetChildOffset(): %d", GetChildOffset(whichPage));
    if (DEBUG) {
        Int32 measuredWidth;
        GetMeasuredWidth(&measuredWidth);
        Logger::D(TAG, "SnapToPage.GetRelativeChildOffset(): %d, %d",
            measuredWidth, GetChildWidth(whichPage));
    }
    Int32 newX = GetChildOffset(whichPage) - GetRelativeChildOffset(whichPage);
    Int32 sX = mUnboundedScrollX;
    Int32 delta = newX - sX;
    return SnapToPage(whichPage, delta, duration);
}

ECode PagedView::SnapToPage(
    /* [in] */ Int32 whichPage,
    /* [in] */ Int32 delta,
    /* [in] */ Int32 duration)
{
    mNextPage = whichPage;

    AutoPtr<IView> focusedChild;
    GetFocusedChild((IView**)&focusedChild);
    if (focusedChild != NULL && whichPage != mCurrentPage &&
            focusedChild == GetPageAt(mCurrentPage)) {
        focusedChild->ClearFocus();
    }

    PageBeginMoving();
    AwakenScrollBars(duration);
    if (duration == 0) {
        duration = Elastos::Core::Math::Abs(delta);
    }

    Boolean isFinished;
    mScroller->IsFinished(&isFinished);
    if (!isFinished) mScroller->AbortAnimation();
    mScroller->StartScroll(mUnboundedScrollX, 0, delta, 0, duration);

    // Load associated pages immediately if someone else is handling the scroll, otherwise defer
    // loading associated pages until the scroll settles
    if (mDeferScrollUpdate) {
        LoadAssociatedPages(mNextPage);
    }
    else {
        mDeferLoadAssociatedPagesUntilScrollCompletes = TRUE;
    }
    NotifyPageSwitchListener();
    Invalidate();
    return NOERROR;
}

ECode PagedView::ScrollLeft()
{
    Boolean isFinished;
    mScroller->IsFinished(&isFinished);
    if (isFinished) {
        if (mCurrentPage > 0) SnapToPage(mCurrentPage - 1);
    }
    else {
        if (mNextPage > 0) SnapToPage(mNextPage - 1);
    }
    return NOERROR;
}

ECode PagedView::ScrollRight()
{
    Boolean isFinished;
    mScroller->IsFinished(&isFinished);
    if (isFinished) {
        if (mCurrentPage < GetPageCount() -1) SnapToPage(mCurrentPage + 1);
    }
    else {
        if (mNextPage < GetPageCount() -1) SnapToPage(mNextPage + 1);
    }
    return NOERROR;
}

ECode PagedView::GetPageForView(
    /* [in] */ IView* v,
    /* [out] */ Int32* page)
{
    VALIDATE_NOT_NULL(page);

    Int32 result = -1;
    if (v != NULL) {
        AutoPtr<IViewParent> vp;
        v->GetParent((IViewParent**)&vp);
        Int32 count = GetPageCount();
        for (Int32 i = 0; i < count; i++) {
            if (IView::Probe(vp) == GetPageAt(i)) {
                *page = i;
                return NOERROR;
            }
        }
    }
    *page = result;
    return NOERROR;
}

/**
 * @return True is long presses are still allowed for the current touch
 */
ECode PagedView::AllowLongPress(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mAllowLongPress;
    return NOERROR;
}

/**
 * Set TRUE to allow long-press events to be triggered, usually checked by
 * {@link Launcher} to accept or block dpad-initiated long-presses.
 */
ECode PagedView::SetAllowLongPress(
    /* [in] */ Boolean allowLongPress)
{
    mAllowLongPress = allowLongPress;
    return NOERROR;
}

ECode PagedView::LoadAssociatedPages(
    /* [in] */ Int32 page)
{
    return LoadAssociatedPages(page, FALSE);
}

ECode PagedView::LoadAssociatedPages(
    /* [in] */ Int32 page,
    /* [in] */ Boolean immediateAndOnly)
{
    if (mContentIsRefreshable) {
        Int32 count = GetPageCount();
        if (page < count) {
            Int32 lowerPageBound = GetAssociatedLowerPageBound(page);
            Int32 upperPageBound = GetAssociatedUpperPageBound(page);
            if (DEBUG) Logger::D(TAG, "LoadAssociatedPages: %d/%d",
                lowerPageBound, upperPageBound);
            // First, clear any pages that should no longer be loaded
            for (Int32 i = 0; i < count; ++i) {
                if ((i < lowerPageBound) || (i > upperPageBound)) {
                    AutoPtr<IPage> layout = IPage::Probe(GetPageAt(i));
                    Int32 pageCount;
                    layout->GetPageChildCount(&pageCount);
                    if (pageCount > 0) {
                        layout->RemoveAllViewsOnPage();
                    }
                    mDirtyPageContent->Set(i, CoreUtils::Convert(TRUE));
                }
            }
            // Next, load any new pages
            for (Int32 i = 0; i < count; ++i) {
                if ((i != page) && immediateAndOnly) {
                    continue;
                }
                if (lowerPageBound <= i && i <= upperPageBound) {
                    AutoPtr<IInterface> value;
                    mDirtyPageContent->Get(i, (IInterface**)&value);
                    Boolean b;
                    IBoolean::Probe(value)->GetValue(&b);
                    if (b) {
                        SyncPageItems(i, (i == page) && immediateAndOnly);
                        mDirtyPageContent->Set(i, CoreUtils::Convert(FALSE));
                    }
                }
            }
        }
    }
    return NOERROR;
}

Int32 PagedView::GetAssociatedLowerPageBound(
    /* [in] */ Int32 page)
{
    return Elastos::Core::Math::Max(0, page - 1);
}

Int32 PagedView::GetAssociatedUpperPageBound(
    /* [in] */ Int32 page)
{
    Int32 count = GetPageCount();
    return Elastos::Core::Math::Min(page + 1, count - 1);
}

ECode PagedView::InvalidatePageData()
{
    return InvalidatePageData(-1, FALSE);
}

ECode PagedView::InvalidatePageData(
    /* [in] */ Int32 currentPage)
{
    return InvalidatePageData(currentPage, FALSE);
}

ECode PagedView::InvalidatePageData(
    /* [in] */ Int32 currentPage,
    /* [in] */ Boolean immediateAndOnly)
{
    if (!mIsDataReady) {
        return NOERROR;
    }

    if (mContentIsRefreshable) {
        // Force all scrolling-related behavior to end
        mScroller->ForceFinished(TRUE);
        mNextPage = INVALID_PAGE;

        // Update all the pages
        SyncPages();

        // We must force a measure after we've loaded the pages to update the content width and
        // to determine the full scroll width
        Int32 measuredWidth, measuredHeight;
        GetMeasuredWidth(&measuredWidth);
        GetMeasuredHeight(&measuredHeight);
        Measure(MeasureSpec::MakeMeasureSpec(measuredWidth, MeasureSpec::EXACTLY),
                MeasureSpec::MakeMeasureSpec(measuredHeight, MeasureSpec::EXACTLY));

        // Set a new page as the current page if necessary
        if (currentPage > -1) {
            SetCurrentPage(Elastos::Core::Math::Min(GetPageCount() - 1, currentPage));
        }

        // Mark each of the pages as dirty
        Int32 count = GetPageCount();
        mDirtyPageContent->Clear();
        for (Int32 i = 0; i < count; ++i) {
            mDirtyPageContent->Add(CoreUtils::Convert(TRUE));
        }

        // Load any pages that are necessary for the current window of views
        LoadAssociatedPages(mCurrentPage, immediateAndOnly);
        RequestLayout();
    }
    return NOERROR;
}

AutoPtr<IView> PagedView::GetScrollingIndicator()
{
    // We use mHasScrollIndicator to prevent future lookups if there is no sibling indicator
    // found
    if (mHasScrollIndicator && mScrollIndicator == NULL) {
        AutoPtr<IViewParent> vp;
        GetParent((IViewParent**)&vp);
        AutoPtr<IViewGroup> parent = IViewGroup::Probe(vp);
        if (parent != NULL) {
            IView::Probe(parent)->FindViewById(R::id::paged_view_indicator, (IView**)&mScrollIndicator);
            mHasScrollIndicator = mScrollIndicator != NULL;
            if (mHasScrollIndicator) {
                mScrollIndicator->SetVisibility(IView::VISIBLE);
            }
        }
    }
    return mScrollIndicator;
}

Boolean PagedView::IsScrollingIndicatorEnabled()
{
    return TRUE;
}

ECode PagedView::FlashScrollingIndicator(
    /* [in] */ Boolean animated)
{
    Boolean res;
    RemoveCallbacks(mHideScrollingIndicatorRunnable, &res);
    ShowScrollingIndicator(!animated);
    return PostDelayed(mHideScrollingIndicatorRunnable, sScrollIndicatorFlashDuration, &res);
}

ECode PagedView::ShowScrollingIndicator(
    /* [in] */ Boolean immediately)
{
    mShouldShowScrollIndicator = TRUE;
    mShouldShowScrollIndicatorImmediately = TRUE;
    if (GetPageCount() <= 1) return NOERROR;
    if (!IsScrollingIndicatorEnabled()) return NOERROR;

    mShouldShowScrollIndicator = FALSE;
    GetScrollingIndicator();
    if (mScrollIndicator != NULL) {
        // Fade the indicator in
        UpdateScrollingIndicatorPosition();
        mScrollIndicator->SetVisibility(IView::VISIBLE);
        CancelScrollingIndicatorAnimations();
        if (immediately || mScrollingPaused) {
            mScrollIndicator->SetAlpha(1.0f);
        }
        else {
            AutoPtr<ArrayOf<Float> > params = ArrayOf<Float>::Alloc(1);
            (*params)[0] = 1.0f;
            mScrollIndicatorAnimator = IValueAnimator::Probe(LauncherAnimUtils::OfFloat(
                mScrollIndicator, String("alpha"), params));
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
    if (GetPageCount() <= 1) return NOERROR;
    if (!IsScrollingIndicatorEnabled()) return NOERROR;

    GetScrollingIndicator();
    if (mScrollIndicator != NULL) {
        // Fade the indicator out
        UpdateScrollingIndicatorPosition();
        CancelScrollingIndicatorAnimations();
        if (immediately || mScrollingPaused) {
            mScrollIndicator->SetVisibility(IView::INVISIBLE);
            mScrollIndicator->SetAlpha(0.0f);
        }
        else {
            AutoPtr<ArrayOf<Float> > params = ArrayOf<Float>::Alloc(1);
            (*params)[0] = 0.0f;
            mScrollIndicatorAnimator = IValueAnimator::Probe(LauncherAnimUtils::OfFloat(
                mScrollIndicator, String("alpha"), params));
            mScrollIndicatorAnimator->SetDuration(sScrollIndicatorFadeOutDuration);
            AutoPtr<AnimatorListenerAdapter> adapter = new MyAnimatorListenerAdapter(this);
            IAnimator::Probe(mScrollIndicatorAnimator)->AddListener(adapter);
            IAnimator::Probe(mScrollIndicatorAnimator)->Start();
        }
    }
    return NOERROR;
}

/**
 * To be overridden by subclasses to determine whether the scroll indicator should stretch to
 * fill its space on the track or not.
 */
Boolean PagedView::HasElasticScrollIndicator()
{
    return TRUE;
}

void PagedView::UpdateScrollingIndicator()
{
    if (GetPageCount() <= 1) return;
    if (!IsScrollingIndicatorEnabled()) return;

    GetScrollingIndicator();
    if (mScrollIndicator != NULL) {
        UpdateScrollingIndicatorPosition();
    }
    if (mShouldShowScrollIndicator) {
        ShowScrollingIndicator(mShouldShowScrollIndicatorImmediately);
    }
}

void PagedView::UpdateScrollingIndicatorPosition()
{
    Boolean isRtl = IsLayoutRtl();
    if (!IsScrollingIndicatorEnabled()) return;
    if (mScrollIndicator == NULL) return;
    Int32 numPages = GetPageCount();
    Int32 pageWidth;
    GetMeasuredWidth(&pageWidth);
    Int32 trackWidth = pageWidth - mScrollIndicatorPaddingLeft - mScrollIndicatorPaddingRight;
    Int32 measuredWidth, left, right;
    mScrollIndicator->GetMeasuredWidth(&measuredWidth);
    mScrollIndicator->GetPaddingLeft(&left);
    mScrollIndicator->GetPaddingRight(&right);
    Int32 indicatorWidth = measuredWidth - left - right;

    Int32 scrollX;
    GetScrollX(&scrollX);
    Float scrollPos = isRtl ? mMaxScrollX - scrollX : scrollX;
    Float offset = Elastos::Core::Math::Max(0.0f, Elastos::Core::Math::Min(1.0f, (Float) scrollPos / mMaxScrollX));
    if (isRtl) {
        offset = 1.0f - offset;
    }
    Int32 indicatorSpace = trackWidth / numPages;
    Int32 indicatorPos = (Int32) (offset * (trackWidth - indicatorSpace)) + mScrollIndicatorPaddingLeft;
    if (HasElasticScrollIndicator()) {
        if (measuredWidth != indicatorSpace) {
            AutoPtr<IViewGroupLayoutParams> lp;
            mScrollIndicator->GetLayoutParams((IViewGroupLayoutParams**)&lp);
            lp->SetWidth(indicatorSpace);
            mScrollIndicator->RequestLayout();
        }
    }
    else {
        Int32 indicatorCenterOffset = indicatorSpace / 2 - indicatorWidth / 2;
        indicatorPos += indicatorCenterOffset;
    }
    mScrollIndicator->SetTranslationX(indicatorPos);
}

ECode PagedView::ShowScrollIndicatorTrack()
{
    return NOERROR;
}

ECode PagedView::HideScrollIndicatorTrack()
{
    return NOERROR;
}

/* Accessibility */
ECode PagedView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ViewGroup::OnInitializeAccessibilityNodeInfo(info);
    info->SetScrollable(GetPageCount() > 1);
    if (GetCurrentPage() < GetPageCount() - 1) {
        info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD);
    }
    if (GetCurrentPage() > 0) {
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
        IAccessibilityRecord::Probe(event)->SetItemCount(GetPageCount());
    }
    return NOERROR;
}

ECode PagedView::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Boolean tmp;
    ViewGroup::PerformAccessibilityAction(action, arguments, &tmp);
    if (tmp) {
        return TRUE;
    }
    switch (action) {
        case IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD: {
            if (GetCurrentPage() < GetPageCount() - 1) {
                ScrollRight();
                return TRUE;
            }
        } break;
        case IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD: {
            if (GetCurrentPage() > 0) {
                ScrollLeft();
                return TRUE;
            }
        } break;
    }
    return FALSE;
}

String PagedView::GetCurrentPageDescription()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String format;
    context->GetString(R::string::default_scroll_format, &format);
    String str;
    Int32 page;
    GetNextPage(&page);
    str.AppendFormat(format, page + 1, GetPageCount());
    return str;
}

ECode PagedView::OnHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = TRUE;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos
