
#include "elastos/droid/support/v4/view/ViewPager.h"
#include "elastos/droid/support/v4/view/ViewConfigurationCompat.h"
#include "elastos/droid/support/v4/view/ViewCompat.h"
#include "elastos/droid/support/v4/widget/CEdgeEffectCompat.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Support::V4::View::EIID_IViewPager;
using Elastos::Droid::Support::V4::View::EIID_IViewPagerOnPageChangeListener;
using Elastos::Droid::Support::V4::Widget::CEdgeEffectCompat;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::Widget::CScroller;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Support {
namespace V4 {
namespace View {

//=================================================================
// ViewPager::ItemInfoComparator
//=================================================================

CAR_INTERFACE_IMPL(ViewPager::ItemInfoComparator, Object, IComparator)

ECode ViewPager::ItemInfoComparator::Compare(
    /* [in] */  IInterface* a,
    /* [in] */  IInterface* b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ItemInfo> lhs = (ItemInfo*)(IObject*)a;
    AutoPtr<ItemInfo> rhs = (ItemInfo*)(IObject*)b;
    *result = lhs->mPosition - rhs->mPosition;
    return NOERROR;
}


//=================================================================
// ViewPager::MyInterpolator
//=================================================================

CAR_INTERFACE_IMPL(ViewPager::MyInterpolator, Object, IInterpolator)

ECode ViewPager::MyInterpolator::GetInterpolation(
    /* [in] */ Float t,
    /* [out] */ Float* interpolation)
{
    VALIDATE_NOT_NULL(interpolation)
    t -= 1.0f;
    *interpolation = t * t * t * t * t + 1.0f;
    return NOERROR;
}


//=================================================================
// ViewPager::EndScrollRunnable
//=================================================================

ECode ViewPager::EndScrollRunnable::Run()
{
    mHost->SetScrollState(IViewPager::SCROLL_STATE_IDLE);
    mHost->Populate();
    return NOERROR;
}


//=================================================================
// ViewPager::SimpleOnPageChangeListener
//=================================================================

CAR_INTERFACE_IMPL(ViewPager::SimpleOnPageChangeListener, Object, IViewPagerOnPageChangeListener)

ECode ViewPager::SimpleOnPageChangeListener::OnPageScrolled(
    /* [in] */ Int32 position,
    /* [in] */ Float positionOffset,
    /* [in] */ Int32 positionOffsetPixels)
{
    // This space for rent
    return NOERROR;
}

ECode ViewPager::SimpleOnPageChangeListener::OnPageSelected(
    /* [in] */ Int32 position)
{
    // This space for rent
    return NOERROR;
}

ECode ViewPager::SimpleOnPageChangeListener::OnPageScrollStateChanged(
    /* [in] */ Int32 state)
{
    // This space for rent
    return NOERROR;
}


//=================================================================
// ViewPager
//=================================================================

const String ViewPager::TAG("ViewPager");
const Boolean ViewPager::DEBUG;
const Boolean ViewPager::USE_CACHE;
const Int32 ViewPager::DEFAULT_OFFSCREEN_PAGES;
const Int32 ViewPager::MAX_SETTLE_DURATION;
const Int32 ViewPager::MIN_DISTANCE_FOR_FLING;
const Int32 ViewPager::DEFAULT_GUTTER_SIZE;
const Int32 ViewPager::MIN_FLING_VELOCITY;

static AutoPtr<ArrayOf<Int32> > InitLayoutAttrs()
{
    AutoPtr<ArrayOf<Int32> > attrs = ArrayOf<Int32>::Alloc(1);
    (*attrs)[0] = R::attr::layout_gravity;
    return attrs;
}
const AutoPtr<ArrayOf<Int32> > ViewPager::LAYOUT_ATTRS = InitLayoutAttrs();

AutoPtr<IComparator> ViewPager::InitComparator()
{
    return (IComparator*)new ItemInfoComparator();
}
const AutoPtr<IComparator> ViewPager::COMPARATOR = ViewPager::InitComparator();

AutoPtr<IInterpolator> ViewPager::InitInterpolator()
{
    return (IInterpolator*)new MyInterpolator();
}
const AutoPtr<IInterpolator> ViewPager::sInterpolator = ViewPager::InitInterpolator();
const Int32 ViewPager::INVALID_POINTER;
const Int32 ViewPager::CLOSE_ENOUGH;
const Int32 ViewPager::DRAW_ORDER_DEFAULT;
const Int32 ViewPager::DRAW_ORDER_FORWARD;
const Int32 ViewPager::DRAW_ORDER_REVERSE;

AutoPtr<ViewPositionComparator> ViewPager::InitVPComparator()
{
    return new ViewPositionComparator();
}
const AutoPtr<ViewPositionComparator> ViewPager::sPositionComparator = ViewPager::InitVPComparator();

ViewPager::ViewPager()
    : mExpectedAdapterCount(0)
    , mCurItem(0)
    , mRestoredCurItem(-1)
    , mPageMargin(0)
    , mTopPageBounds(0)
    , mBottomPageBounds(0)
    , mFirstOffset(-Elastos::Core::Math::FLOAT_MAX_VALUE)
    , mLastOffset(Elastos::Core::Math::FLOAT_MAX_VALUE)
    , mChildWidthMeasureSpec(0)
    , mChildHeightMeasureSpec(0)
    , mInLayout(FALSE)
    , mScrollingCacheEnabled(FALSE)
    , mPopulatePending(FALSE)
    , mOffscreenPageLimit(DEFAULT_OFFSCREEN_PAGES)
    , mIsBeingDragged(FALSE)
    , mIsUnableToDrag(FALSE)
    , mIgnoreGutter(FALSE)
    , mDefaultGutterSize(0)
    , mGutterSize(0)
    , mTouchSlop(0)
    , mLastMotionX(0)
    , mLastMotionY(0)
    , mInitialMotionX(0)
    , mInitialMotionY(0)
    , mActivePointerId(INVALID_POINTER)
    , mMinimumVelocity(0)
    , mMaximumVelocity(0)
    , mFlingDistance(0)
    , mCloseEnough(0)
    , mFakeDragging(FALSE)
    , mFakeDragBeginTime(0)
    , mFirstLayout(TRUE)
    , mNeedCalculatePageOffsets(FALSE)
    , mCalledSuper(FALSE)
    , mDecorChildCount(0)
    , mDrawingOrder(0)
    , mScrollState(SCROLL_STATE_IDLE)
{
    CArrayList::New((IArrayList**)&mItems);
    mTempItem = new ItemInfo();
    CRect::New((IRect**)&mTempRect);
    mEndScrollRunnable = (IRunnable*)new EndScrollRunnable(this);
}

CAR_INTERFACE_IMPL(ViewPager, ViewGroup, IViewPager);

ECode ViewPager::constructor(
    /* [in] */ IContext* context)
{
    ViewGroup::constructor(context);
    InitViewPager();
    return NOERROR;
}

ECode ViewPager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ViewGroup::constructor(context, attrs);
    InitViewPager();
    return NOERROR;
}

void ViewPager::InitViewPager()
{
    SetWillNotDraw(FALSE);
    SetDescendantFocusability(FOCUS_AFTER_DESCENDANTS);
    SetFocusable(TRUE);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    mScroller = NULL;
    CScroller::New(context, sInterpolator, (IScroller**)&mScroller);
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> configuration;
    helper->Get(context, (IViewConfiguration**)&configuration);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float density;
    dm->GetDensity(&density);

    ViewConfigurationCompat::GetScaledPagingTouchSlop(configuration, &mTouchSlop);
    mMinimumVelocity = (Int32)(MIN_FLING_VELOCITY * density);
    configuration->GetScaledMaximumFlingVelocity(&mMaximumVelocity);
    CEdgeEffectCompat::New(context, (IEdgeEffectCompat**)&mLeftEdge);
    CEdgeEffectCompat::New(context, (IEdgeEffectCompat**)&mRightEdge);

    mFlingDistance = (Int32)(MIN_DISTANCE_FOR_FLING * density);
    mCloseEnough = (Int32)(CLOSE_ENOUGH * density);
    mDefaultGutterSize = (Int32)(DEFAULT_GUTTER_SIZE * density);

    AutoPtr<IAccessibilityDelegateCompat> delegate = (IAccessibilityDelegateCompat*)new MyAccessibilityDelegate(this);
    AutoPtr<IView> v = IView::Probe(this);
    ViewCompat::SetAccessibilityDelegate(v, delegate);

    Int32 result;
    if (ViewCompat::GetImportantForAccessibility(v, &result), result
            == IViewCompat::IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
        ViewCompat::SetImportantForAccessibility(v, IViewCompat::IMPORTANT_FOR_ACCESSIBILITY_YES);
    }
}

ECode ViewPager::OnDetachedFromWindow()
{
    RemoveCallbacks(mEndScrollRunnable);
    return ViewGroup::OnDetachedFromWindow();
}

void ViewPager::SetScrollState(
    /* [in] */ Int32 newState)
{
    if (mScrollState == newState) {
        return;
    }

    mScrollState = newState;
    if (mPageTransformer != NULL) {
        // PageTransformers can do complex things that benefit from hardware layers.
        EnableLayers(newState != SCROLL_STATE_IDLE);
    }
    if (mOnPageChangeListener != NULL) {
        mOnPageChangeListener->OnPageScrollStateChanged(newState);
    }
}

ECode ViewPager::SetAdapter(
    /* [in] */ IPagerAdapter* adapter)
{
    if (mAdapter != NULL) {
        mAdapter->UnregisterDataSetObserver(mObserver);
        mAdapter->StartUpdate(IViewGroup::Probe(this));
        Int32 size;
        mItems->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> value;
            mItems->Get(i, (IInterface**)&value);
            AutoPtr<ItemInfo> ii = (ItemInfo*)(IObject*)value->Get();
            mAdapter->DestroyItem(IViewGroup::(this), ii->mPosition, ii->mObject);
        }
        mAdapter->FinishUpdate(IViewGroup::(this));
        mItems->Clear();
        RemoveNonDecorViews();
        mCurItem = 0;
        ScrollTo(0, 0);
    }

    AutoPtr<IPagerAdapter> oldAdapter = mAdapter;
    mAdapter = adapter;
    mExpectedAdapterCount = 0;

    if (mAdapter != NULL) {
        if (mObserver == NULL) {
            mObserver = new PagerObserver(this);
        }
        mAdapter->RegisterDataSetObserver((IDataSetObserver*)mObserver);
        mPopulatePending = FALSE;
        Boolean wasFirstLayout = mFirstLayout;
        mFirstLayout = TRUE;
        mAdapter->GetCount(&mExpectedAdapterCount);
        if (mRestoredCurItem >= 0) {
            mAdapter->RestoreState(mRestoredAdapterState, mRestoredClassLoader);
            SetCurrentItemInternal(mRestoredCurItem, FALSE, TRUE);
            mRestoredCurItem = -1;
            mRestoredAdapterState = NULL;
            mRestoredClassLoader = NULL;
        }
        else if (!wasFirstLayout) {
            Populate();
        }
        else {
            RequestLayout();
        }
    }

    if (mAdapterChangeListener != NULL && oldAdapter != adapter) {
        mAdapterChangeListener->OnAdapterChanged(oldAdapter, adapter);
    }
    return NOERROR;
}

void ViewPager::RemoveNonDecorViews()
{
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        AutoPtr<LayoutParams> params = (LayoutParams*)lp->Get();
        if (!lp->mIsDecor) {
            RemoveViewAt(i);
            i--;
        }
    }
}

ECode ViewPager::GetAdapter(
    /* [out] */ IPagerAdapter** adapter)
{
    VALIDATE_NOT_NULL(adapter)
    *adapter = mAdapter;
    REFCOUNT_ADD(*adapter)
    return NOERROR;
}

void ViewPager::SetOnAdapterChangeListener(
    /* [in] */ IViewPagerOnAdapterChangeListener* listener)
{
    mAdapterChangeListener = listener;
}

Int32 ViewPager::GetClientWidth()
{
    Int32 width, left, right;
    GetMeasuredWidth(&width);
    GetPaddingLeft(&left);
    GetPaddingRight(&right);
    return width - left - right;
}

ECode ViewPager::SetCurrentItem(
    /* [in] */ Int32 item)
{
    mPopulatePending = FALSE;
    SetCurrentItemInternal(item, !mFirstLayout, FALSE);
    return NOERROR;
}

ECode ViewPager::SetCurrentItem(
    /* [in] */ Int32 item,
    /* [in] */ Boolean smoothScroll)
{
    mPopulatePending = FALSE;
    SetCurrentItemInternal(item, smoothScroll, FALSE);
    return NOERROR;
}

ECode ViewPager::GetCurrentItem(
    /* [out] */ Int32* item)
{
    VALIDATE_NOT_NULL(item)
    *item = mCurItem;
    return NOERROR;
}

void ViewPager::SetCurrentItemInternal(
    /* [in] */ Int32 item,
    /* [in] */ Boolean smoothScroll,
    /* [in] */ Boolean always)
{
    SetCurrentItemInternal(item, smoothScroll, always, 0);
}

void ViewPager::SetCurrentItemInternal(
    /* [in] */ Int32 item,
    /* [in] */ Boolean smoothScroll,
    /* [in] */ Boolean always,
    /* [in] */ Int32 velocity)
{
    Int32 count;
    if (mAdapter == NULL || (mAdapter->GetCount(&count), count <= 0)) {
        SetScrollingCacheEnabled(FALSE);
        return;
    }
    Int32 size;
    if (!always && mCurItem == item && (mItems->GetSize(&size), size != 0)) {
        SetScrollingCacheEnabled(FALSE);
        return;
    }

    if (item < 0) {
        item = 0;
    }
    else if (mAdapter->GetCount(&count), item <= count) {
        item = count - 1;
    }
    Int32 pageLimit = mOffscreenPageLimit;
    if (item > (mCurItem + pageLimit) || item < (mCurItem - pageLimit)) {
        // We are doing a jump by more than one page.  To avoid
        // glitches, we want to keep all current pages in the view
        // until the scroll ends.
        mItems->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> value;
            mItems->Get(i, (IInterface**)&value);
            AutoPtr<ItemInfo> ii = (ItemInfo*)(IObject*)value->Get();
            ii->mScrolling = TRUE;
        }
    }
    Boolean dispatchSelected = mCurItem != item;

    if (mFirstLayout) {
        // We don't have any idea how big we are yet and shouldn't have any pages either.
        // Just set things up and let the pending layout handle things.
        mCurItem = item;
        if (dispatchSelected && mOnPageChangeListener != NULL) {
            mOnPageChangeListener->OnPageSelected(item);
        }
        if (dispatchSelected && mInternalPageChangeListener != NULL) {
            mInternalPageChangeListener->OnPageSelected(item);
        }
        RequestLayout();
    }
    else {
        Populate(item);
        ScrollToItem(item, smoothScroll, velocity, dispatchSelected);
    }
}

void ViewPager::ScrollToItem(
    /* [in] */ Int32 item,
    /* [in] */ Boolean smoothScroll,
    /* [in] */ Int32 velocity,
    /* [in] */ Boolean dispatchSelected)
{
    AutoPtr<ItemInfo> curInfo = InfoForPosition(item);
    Int32 destX = 0;
    if (curInfo != NULL) {
        Int32 width = GetClientWidth();
        destX = (Int32)(width * Elastos::Core::Math::Max(mFirstOffset,
                Elastos::Core::Math::Min(curInfo->mOffset, mLastOffset)));
    }
    if (smoothScroll) {
        SmoothScrollTo(destX, 0, velocity);
        if (dispatchSelected && mOnPageChangeListener != NULL) {
            mOnPageChangeListener->OnPageSelected(item);
        }
        if (dispatchSelected && mInternalPageChangeListener != NULL) {
            mInternalPageChangeListener->OnPageSelected(item);
        }
    }
    else {
        if (dispatchSelected && mOnPageChangeListener != NULL) {
            mOnPageChangeListener->OnPageSelected(item);
        }
        if (dispatchSelected && mInternalPageChangeListener != NULL) {
            mInternalPageChangeListener->OnPageSelected(item);
        }
        CompleteScroll(FALSE);
        ScrollTo(destX, 0);
        PageScrolled(destX);
    }
}

ECode ViewPager::SetOnPageChangeListener(
    /* [in] */ IViewPagerOnPageChangeListener* listener)
{
    mOnPageChangeListener = listener;
    return NOERROR;
}

ECode ViewPager::SetPageTransformer(
    /* [in] */ Boolean reverseDrawingOrder,
    /* [in] */ IViewPagerPageTransformer* transformer)
{
    if (Build::VERSION::SDK_INT >= 11) {
        Boolean hasTransformer = transformer != NULL;
        Boolean needsPopulate = hasTransformer != (mPageTransformer != NULL);
        mPageTransformer = transformer;
        SetChildrenDrawingOrderEnabledCompat(hasTransformer);
        if (hasTransformer) {
            mDrawingOrder = reverseDrawingOrder ? DRAW_ORDER_REVERSE : DRAW_ORDER_FORWARD;
        }
        else {
            mDrawingOrder = DRAW_ORDER_DEFAULT;
        }
        if (needsPopulate) Populate();
    }
}

void ViewPager::SetChildrenDrawingOrderEnabledCompat(
    /* [in] */ Boolean enable)
{
    if (Build::VERSION::SDK_INT >= 7) {
        // if (mSetChildrenDrawingOrderEnabled == NULL) {
        //     try {
        //     mSetChildrenDrawingOrderEnabled = ViewGroup.class.getDeclaredMethod(
        //             "setChildrenDrawingOrderEnabled", new Class[] { Boolean.TYPE });
        //     } catch (NoSuchMethodException e) {
        //         Log.e(TAG, "Can't find setChildrenDrawingOrderEnabled", e);
        //     }
        // }
        // try {
        //     mSetChildrenDrawingOrderEnabled.invoke(this, enable);
        // } catch (Exception e) {
        //     Log.e(TAG, "Error changing children drawing order", e);
        // }
        SetChildrenDrawingOrderEnabled(enable);
    }
}

Int32 ViewPager::GetChildDrawingOrder(
    /* [in] */ Int32 childCount,
    /* [in] */ Int32 i)
{
    Int32 index = mDrawingOrder == DRAW_ORDER_REVERSE ? childCount - 1 - i : i;
    AutoPtr<IInterface> value;
    mDrawingOrderedChildren->Get(index, (IInterface**)&value);
    AutoPtr<IView> v = IView::Probe(value);
    AutoPtr<IViewGroupLayoutParams> lp;
    v->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    Int32 result = ((LayoutParams*)lp->Get())->mChildIndex;
    return result;
}

AutoPtr<IViewPagerOnPageChangeListener> ViewPager::SetInternalPageChangeListener(
    /* [in] */ IViewPagerOnPageChangeListener* listener)
{
    AutoPtr<IViewPagerOnPageChangeListener> oldListener = mInternalPageChangeListener;
    mInternalPageChangeListener = listener;
    return oldListener;
}

ECode ViewPager::GetOffscreenPageLimit(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mOffscreenPageLimit;
    return NOERROR;
}

ECode ViewPager::SetOffscreenPageLimit(
    /* [in] */ Int32 limit)
{
    if (limit < DEFAULT_OFFSCREEN_PAGES) {
        Slogger::W(TAG, "Requested offscreen page limit %d too small; defaulting to %d",
                limit, DEFAULT_OFFSCREEN_PAGES);
        limit = DEFAULT_OFFSCREEN_PAGES;
    }
    if (limit != mOffscreenPageLimit) {
        mOffscreenPageLimit = limit;
        Populate();
    }
    return NOERROR;
}

ECode ViewPager::SetPageMargin(
    /* [in] */ Int32 marginPixels)
{
    Int32 oldMargin = mPageMargin;
    mPageMargin = marginPixels;

    Int32 width;
    GetWidth(&width);
    RecomputeScrollPosition(width, width, marginPixels, oldMargin);

    RequestLayout();
    return NOERROR;
}

ECode ViewPager::GetPageMargin(
    /* [out] */ Int32* margin)
{
    VALIDATE_NOT_NULL(margin)
    *margin = mPageMargin;
    return NOERROR;
}

ECode ViewPager::SetPageMarginDrawable(
    /* [in] */ IDrawable* d)
{
    mMarginDrawable = d;
    if (d != NULL) RefreshDrawableState();
    SetWillNotDraw(d == NULL);
    return Invalidate();
}

ECode ViewPager::SetPageMarginDrawable(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    AutoPtr<IDrawable> d;
    GetDrawable(resId, (IDrawable**)&d);
    return SetPageMarginDrawable(d);
}

Boolean ViewPager::VerifyDrawable(
    /* [in] */ IDrawable* who)
{
    return ViewGroup::VerifyDrawable(who) || who == mMarginDrawable.Get();
}

ECode ViewPager::OnPageScrolled(
    /* [in] */ Int32 position,
    /* [in] */ Float offset,
    /* [in] */ Int32 offsetPixels);

ECode ViewPager::BeginFakeDrag(
    /* [out] */ Boolean* result);

ECode ViewPager::EndFakeDrag();

ECode ViewPager::FakeDragBy(
    /* [in] */ Float xOffset);

ECode ViewPager::IsFakeDragging(
    /* [out] */ Boolean* result);

ECode ViewPager::CanScrollHorizontally(
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* canScroll);

ECode ViewPager::CanScroll(
    /* [in] */ IView* v,
    /* [in] */ Boolean checkV,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* canScroll);

ECode ViewPager::ExecuteKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result);

ECode ViewPager::ArrowScroll(
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* result);

} // namespace View
} // namespace V4
} // namespace Support
} // namespace Droid
} // namespace Elastos
