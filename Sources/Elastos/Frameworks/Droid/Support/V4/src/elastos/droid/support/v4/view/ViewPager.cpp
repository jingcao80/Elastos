
#include "elastos/droid/support/v4/view/ViewPager.h"
#include "elastos/droid/support/v4/view/ViewConfigurationCompat.h"
#include "elastos/droid/support/v4/view/ViewCompat.h"
#include "elastos/droid/support/v4/widget/CEdgeEffectCompat.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
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
using Elastos::Core::StringUtils;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
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
            AutoPtr<ItemInfo> ii = (ItemInfo*)(IObject*)value.Get();
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
        AutoPtr<LayoutParams> params = (LayoutParams*)lp.Get();
        if (!params->mIsDecor) {
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
            AutoPtr<ItemInfo> ii = (ItemInfo*)(IObject*)value.Get();
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
    Int32 result = ((LayoutParams*)lp.Get())->mChildIndex;
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

ECode ViewPager::DrawableStateChanged()
{
    ViewGroup::DrawableStateChanged();
    AutoPtr<IDrawable> d = mMarginDrawable;
    Boolean isStateful;
    if (d != NULL && (d->IsStateful(&isStateful), isStateful)) {
        AutoPtr<ArrayOf<Int32> > state;
        GetDrawableState((ArrayOf<Int32>**)&state);
        Boolean result;
        d->SetState(state, &result);
    }
    return NOERROR;
}

Float ViewPager::DistanceInfluenceForSnapDuration(
    /* [in] */ Float f)
{
    f -= 0.5f; // center the values about 0.
    f *= 0.3f * Elastos::Core::Math::PI / 2.0f;
    return (Float) Elastos::Core::Math::Sin(f);
}

void ViewPager::SmoothScrollTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    SmoothScrollTo(x, y, 0);
}

void ViewPager::SmoothScrollTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 velocity)
{
    Int32 count;
    if (GetChildCount() == 0) {
        // Nothing to do.
        SetScrollingCacheEnabled(FALSE);
        return;
    }
    Int32 sx, sy;
    GetScrollX(&sx);
    GetScrollY(&sy);
    Int32 dx = x - sx;
    Int32 dy = y - sy;
    if (dx == 0 && dy == 0) {
        CompleteScroll(FALSE);
        Populate();
        SetScrollState(SCROLL_STATE_IDLE);
        return;
    }

    SetScrollingCacheEnabled(TRUE);
    SetScrollState(SCROLL_STATE_SETTLING);

    Int32 width;
    GetClientWidth(&width);
    Int32 halfWidth = width / 2;
    Float distanceRatio = Elastos::Core::Math::Min(1f, 1.0f * Elastos::Core::Math::Abs(dx) / width);
    Float distance = halfWidth + halfWidth *
            DistanceInfluenceForSnapDuration(distanceRatio);

    Int32 duration = 0;
    velocity = Elastos::Core::Math::Abs(velocity);
    if (velocity > 0) {
        duration = 4 * Elastos::Core::Math::Round(1000 * Elastos::Core::Math::Abs(distance / velocity));
    }
    else {
        Int32 pageW;
        mAdapter->GetPageWidth(mCurItem, &pageW)
        Float pageWidth = width * pageW;
        Float pageDelta = (Float) Elastos::Core::Math::Abs(dx) / (pageWidth + mPageMargin);
        duration = (Int32)((pageDelta + 1) * 100);
    }
    duration = Elastos::Core::Math::Min(duration, MAX_SETTLE_DURATION);

    mScroller->StartScroll(sx, sy, dx, dy, duration);
    ViewCompat:PostInvalidateOnAnimation(this);
}

AutoPtr<ItemInfo> ViewPager::AddNewItem(
    /* [in] */ Int32 position,
    /* [in] */ Int32 index)
{
    AutoPtr<ItemInfo> ii = new ItemInfo();
    ii->mPosition = position;
    ii->mObject = NULL;
    mAdapter->InstantiateItem(IView::Probe(this), position, &ii->mObject);
    mAdapter->GetPageWidth(position, &ii->mWidthFactor);
    Int32 size;
    if (index < 0 || (mItems->GetSize(&size), index >= size)) {
        mItems->Add(ii);
    }
    else {
        mItems->Add(index, ii);
    }
    return ii;
}

void ViewPager::DataSetChanged()
{
    // This method only gets called if our observer is attached, so mAdapter is non-null.

    Int32 adapterCount;
    mAdapter->GetCount(&adapterCount);
    mExpectedAdapterCount = adapterCount;
    Int32 size;
    mItems->GetSize(&size);
    Boolean needPopulate = size < mOffscreenPageLimit * 2 + 1 && size < adapterCount;
    Int32 newCurrItem = mCurItem;

    Boolean isUpdating = FALSE;
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> value;
        mItems->Get(i, (IInterface**)&value);
        AutoPtr<ItemInfo> ii = (ItemInfo*)(IObject*)value.Get();
        Int32 newPos;
        mAdapter->GetItemPosition(ii->mObject, &newPos);

        if (newPos == IPagerAdapter::POSITION_UNCHANGED) {
            continue;
        }

        if (newPos == IPagerAdapter::POSITION_NONE) {
            mItems->Remove(i);
            i--;

            if (!isUpdating) {
                mAdapter->StartUpdate(IView::Probe(this));
                isUpdating = TRUE;
            }

            mAdapter->DestroyItem(IView::Probe(this), ii->mPosition, ii->mObject);
            needPopulate = TRUE;

            if (mCurItem == ii->mPosition) {
                // Keep the current item in the valid range
                newCurrItem = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(mCurItem, adapterCount - 1));
                needPopulate = TRUE;
            }
            continue;
        }

        if (ii->mPosition != newPos) {
            if (ii->mPosition == mCurItem) {
                // Our current item changed position. Follow it.
                newCurrItem = newPos;
            }

            ii->mPosition = newPos;
            needPopulate = TRUE;
        }
    }

    if (isUpdating) {
        mAdapter->FinishUpdate(IView::Probe(this));
    }

    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    coll->Sort(IList::Probe(mItems), COMPARATOR);

    if (needPopulate) {
        // Reset our known page widths; populate will recompute them.
        Int32 childCount;
        GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            AutoPtr<IViewGroupLayoutParams> lp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
            AutoPtr<LayoutParams> params = (LayoutParams*)lp.Get();
            if (!params->mIsDecor) {
                params->mWidthFactor = 0.f;
            }
        }

        SetCurrentItemInternal(newCurrItem, FALSE, TRUE);
        RequestLayout();
    }
}

ECode ViewPager::Populate()
{
    return Populate(mCurItem);
}

ECode ViewPager::Populate(
    /* [in] */ Int32 newCurrentItem)
{
    AutoPtr<ItemInfo> oldCurInfo;
    Int32 focusDirection = IView::FOCUS_FORWARD;
    if (mCurItem != newCurrentItem) {
        focusDirection = mCurItem < newCurrentItem ? IView::FOCUS_RIGHT : IView::FOCUS_LEFT;
        oldCurInfo = InfoForPosition(mCurItem);
        mCurItem = newCurrentItem;
    }

    if (mAdapter == NULL) {
        SortChildDrawingOrder();
        return;
    }

    // Bail now if we are waiting to populate.  This is to hold off
    // on creating views from the time the user releases their finger to
    // fling to a new position until we have finished the scroll to
    // that position, avoiding glitches from happening at that point.
    if (mPopulatePending) {
        if (DEBUG) Slogger::I(TAG, "populate is pending, skipping for now...");
        SortChildDrawingOrder();
        return;
    }

    // Also, don't populate until we are attached to a window.  This is to
    // avoid trying to populate before we have restored our view hierarchy
    // state and conflicting with what is restored.
    AutoPtr<IBinder> token;
    if (GetWindowToken((IBinder**)&token), token == NULL) {
        return;
    }

    mAdapter->StartUpdate(IView::Probe(this));

    Int32 pageLimit = mOffscreenPageLimit;
    Int32 startPos = Elastos::Core::Math::Max(0, mCurItem - pageLimit);
    Int32 N;
    mAdapter->GetCount(&N);
    Int32 endPos = Elastos::Core::Math::Min(N - 1, mCurItem + pageLimit);

    if (N != mExpectedAdapterCount) {
        String resName;
        // try {
        AutoPtr<IResources> res;
        GetResources((IResources**)&res);
        Int32 id;
        GetId(&id);
        if (FAILED(res->GetResourcesName(id, &resName))) {
            resName = StringUtils::ToHexString(id);
        }
        // } catch (Resources.NotFoundException e) {
        //     resName = Integer.toHexString(getId());
        // }
        Slogger::E(TAG, "The application's PagerAdapter changed the adapter's contents without calling PagerAdapter#notifyDataSetChanged! Expected adapter item count: %d, found: %d Pager id: %s Pager class:  Problematic adapter: "
                , mExpectedAdapterCount, N, resName.string()/*, getClass(), mAdapter.getClass()*/);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // Locate the currently focused item or add it if needed.
    Int32 curIndex = -1;
    AutoPtr<ItemInfo> curItem;
    Int32 size;
    mItems->GetSize(&size);
    for (curIndex = 0; curIndex < size; curIndex++) {
        AutoPtr<IInterface> value;
        mItems->Get(curIndex, (IInterface**)&value);
        AutoPtr<ItemInfo> ii = (ItemInfo*)(IObject*)value.Get();
        if (ii->mPosition >= mCurItem) {
            if (ii->mPosition == mCurItem) curItem = ii;
            break;
        }
    }

    if (curItem == NULL && N > 0) {
        curItem = AddNewItem(mCurItem, curIndex);
    }

    // Fill 3x the available width or up to the number of offscreen
    // pages requested to either side, whichever is larger.
    // If we have no current item we have no work to do.
    if (curItem != NULL) {
        Float extraWidthLeft = 0.f;
        Int32 itemIndex = curIndex - 1;
        AutoPtr<ItemInfo> ii;
        if (itemIndex >= 0) {
            AutoPtr<IInterface> value;
            mItems->Get(itemIndex, (IInterface**)&value);
            ii = (ItemInfo*)(IObject*)value.Get();
        }
        Int32 clientWidth;
        GetClientWidth(&clientWidth);
        Int32 paddingL;
        GetPaddingLeft(&paddingL);
        Float leftWidthNeeded = clientWidth <= 0 ? 0 :
                2.f - curItem->mWidthFactor + (Float)paddingL / (Float) clientWidth;
        for (Int32 pos = mCurItem - 1; pos >= 0; pos--) {
            if (extraWidthLeft >= leftWidthNeeded && pos < startPos) {
                if (ii == NULL) {
                    break;
                }
                if (pos == ii->mPosition && !ii->mScrolling) {
                    mItems->Remove(itemIndex);
                    mAdapter->DestroyItem(IView::Probe(this), pos, ii->mObject);
                    if (DEBUG) {
                        Slogger::I(TAG, "populate() - destroyItem() with pos: %d view: %s", pos, TO_CSTR(ii->mObject));
                    }
                    itemIndex--;
                    curIndex--;
                    ii = NULL;
                    if (itemIndex >= 0) {
                        AutoPtr<IInterface> value;
                        mItems->Get(itemIndex, (IInterface**)&value);
                        ii = (ItemInfo*)(IObject*)value.Get();
                    }
                }
            }
            else if (ii != NULL && pos == ii->mPosition) {
                extraWidthLeft += ii->mWidthFactor;
                itemIndex--;
                ii = NULL;
                if (itemIndex >= 0) {
                    AutoPtr<IInterface> value;
                    mItems->Get(itemIndex, (IInterface**)&value);
                    ii = (ItemInfo*)(IObject*)value.Get();
                }
            }
            else {
                ii = AddNewItem(pos, itemIndex + 1);
                extraWidthLeft += ii->mWidthFactor;
                curIndex++;
                ii = NULL;
                if (itemIndex >= 0) {
                    AutoPtr<IInterface> value;
                    mItems->Get(itemIndex, (IInterface**)&value);
                    ii = (ItemInfo*)(IObject*)value.Get();
                }
            }
        }

        Float extraWidthRight = curItem->mWidthFactor;
        itemIndex = curIndex + 1;
        if (extraWidthRight < 2.f) {
            mItems->GetSize(&size);
            ii = NULL;
            if (itemIndex < size) {
                AutoPtr<IInterface> value;
                mItems->Get(itemIndex, (IInterface**)&value);
                ii = (ItemInfo*)(IObject*)value.Get();
            }
            Int32 paddingR;
            GetPaddingRight(&paddingR);
            Float rightWidthNeeded = clientWidth <= 0 ? 0 : (Float)paddingR / (Float)clientWidth + 2.f;
            for (Int32 pos = mCurItem + 1; pos < N; pos++) {
                if (extraWidthRight >= rightWidthNeeded && pos > endPos) {
                    if (ii == NULL) {
                        break;
                    }
                    if (pos == ii->mPosition && !ii->mScrolling) {
                        mItems->Remove(itemIndex);
                        mAdapter->DestroyItem(this, pos, ii->mObject);
                        if (DEBUG) {
                            Slogger::I(TAG, "populate() - destroyItem() with pos: %d view: %s", pos, TO_CSTR(ii->mObject));
                        }
                        mItems->GetSize(&size);
                        ii = NULL;
                        if (itemIndex < size) {
                            AutoPtr<IInterface> value;
                            mItems->Get(itemIndex, (IInterface**)&value);
                            ii = (ItemInfo*)(IObject*)value.Get();
                        }
                    }
                }
                else if (ii != NULL && pos == ii->mPosition) {
                    extraWidthRight += ii->mWidthFactor;
                    itemIndex++;
                    mItems->GetSize(&size);
                    ii = NULL;
                    if (itemIndex < size) {
                        AutoPtr<IInterface> value;
                        mItems->Get(itemIndex, (IInterface**)&value);
                        ii = (ItemInfo*)(IObject*)value.Get();
                    }
                }
                else {
                    ii = AddNewItem(pos, itemIndex);
                    itemIndex++;
                    extraWidthRight += ii->mWidthFactor;
                    mItems->GetSize(&size);
                    ii = NULL;
                    if (itemIndex < size) {
                        AutoPtr<IInterface> value;
                        mItems->Get(itemIndex, (IInterface**)&value);
                        ii = (ItemInfo*)(IObject*)value.Get();
                    }
                }
            }
        }

        CalculatePageOffsets(curItem, curIndex, oldCurInfo);
    }

    if (DEBUG) {
        Slogger::I(TAG, "Current page list:");
        mItems->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> value;
            mItems->Get(i, (IInterface**)&value);
            AutoPtr<ItemInfo> itemInfo = (ItemInfo*)(IObject*)value.Get();
            Slogger::I(TAG, "#%d: page %d", itemInfo->mPosition);
        }
    }

    AutoPtr<IView> v = IView::Probe(this);
    mAdapter->SetPrimaryItem(v, mCurItem, curItem != NULL ? curItem->mObject : NULL);

    mAdapter->FinishUpdate(v);

    // Check width measurement of current pages and drawing sort order.
    // Update LayoutParams as needed.
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        AutoPtr<LayoutParams> params = (LayoutParams*)lp.Get();
        params->mChildIndex = i;
        if (!params->mIsDecor && params->mWidthFactor == 0.f) {
            // 0 means requery the adapter for this, it doesn't have a valid width.
            AutoPtr<ItemInfo> ii = InfoForChild(child);
            if (ii != NULL) {
                params->mWidthFactor = ii->mWidthFactor;
                params->mPosition = ii->mPosition;
            }
        }
    }
    SortChildDrawingOrder();

    Boolean hasFocus;
    if (HasFocus(&hasFocus), hasFocus) {
        AutoPtr<IView> currentFocused;
        FindFocus((IView**)&currentFocused);
        AutoPtr<ItemInfo> ii = currentFocused != NULL ? InfoForAnyChild(currentFocused) : NULL;
        if (ii == NULL || ii->mPosition != mCurItem) {
            Int32 c;
            GetChildCount(&c);
            for (Int32 i = 0; i < c; i++) {
                AutoPtr<IView> child;
                GetChildAt(i, (IView**)&child);
                ii = InfoForChild(child);
                if (ii != NULL && ii->mPosition == mCurItem) {
                    Boolean isFocus;
                    if (child->RequestFocus(focusDirection, &isFocus), isFocus) {
                        break;
                    }
                }
            }
        }
    }
    return NOERROR;
}

void ViewPager::SortChildDrawingOrder()
{
    if (mDrawingOrder != DRAW_ORDER_DEFAULT) {
        if (mDrawingOrderedChildren == NULL) {
            CArrayList::New((IArrayList**)&mDrawingOrderedChildren);
        }
        else {
            mDrawingOrderedChildren->Clear();
        }
        Int32 childCount;
        GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            mDrawingOrderedChildren->Add(child);
        }
        AutoPtr<ICollections> coll;
        CCollections::AcquireSingleton((ICollections**)&coll);
        coll->Sort(IList::Probe(mDrawingOrderedChildren), sPositionComparator);
    }
}

void ViewPager::CalculatePageOffsets(
    /* [in] */ ItemInfo* curItem,
    /* [in] */ Int32 curIndex,
    /* [in] */ ItemInfo* oldCurInfo)
{
    Int32 N;
    mAdapter->GetCount(&N);
    Int32 width;
    GetClientWidth(&width);
    Float marginOffset = width > 0 ? (Float) mPageMargin / width : 0;
    // Fix up offsets for later layout.
    if (oldCurInfo != NULL) {
        Int32 oldCurPosition = oldCurInfo->mPosition;
        // Base offsets off of oldCurInfo.
        if (oldCurPosition < curItem->mPosition) {
            Int32 itemIndex = 0;
            AutoPtr<ItemInfo> ii;
            Float offset = oldCurInfo->mOffset + oldCurInfo->mWidthFactor + marginOffset;
            Int32 size;
            mItems->GetSize(&size);
            for (Int32 pos = oldCurPosition + 1;
                    pos <= curItem->mPosition && itemIndex < size; pos++) {
                AutoPtr<IInterface> value;
                mItems->Get(itemIndex, (IInterface**)&value);
                ii = (ItemInfo*)(IObject*)value.Get();
                while (pos > ii->mPosition && itemIndex < size - 1) {
                    itemIndex++;
                    AutoPtr<IInterface> v;
                    mItems->Get(itemIndex, (IInterface**)&v);
                    ii = (ItemInfo*)(IObject*)v.Get();
                }
                while (pos < ii->mPosition) {
                    // We don't have an item populated for this,
                    // ask the adapter for an offset.
                    Int32 width;
                    mAdapter->GetPageWidth(pos, &width);
                    offset += width + marginOffset;
                    pos++;
                }
                ii->mOffset = offset;
                offset += ii->mWidthFactor + marginOffset;
            }
        }
        else if (oldCurPosition > curItem->mPosition) {
            Int32 size;
            mItems->GetSize(&size);
            Int32 itemIndex = size - 1;
            AutoPtr<ItemInfo> ii;
            Float offset = oldCurInfo->mOffset;
            for (Int32 pos = oldCurPosition - 1;
                    pos >= curItem->mPosition && itemIndex >= 0; pos--) {
                AutoPtr<IInterface> value;
                mItems->Get(itemIndex, (IInterface**)&value);
                ii = (ItemInfo*)(IObject*)value.Get();
                while (pos < ii->mPosition && itemIndex > 0) {
                    itemIndex--;
                    AutoPtr<IInterface> v;
                    mItems->Get(itemIndex, (IInterface**)&v);
                    ii = (ItemInfo*)(IObject*)v.Get();
                }
                while (pos > ii->mPosition) {
                    // We don't have an item populated for this,
                    // ask the adapter for an offset.
                    Int32 width;
                    mAdapter->GetPageWidth(pos, &width);
                    offset -= width + marginOffset;
                    pos--;
                }
                offset -= ii->mWidthFactor + marginOffset;
                ii->mOffset = offset;
            }
        }
    }

    // Base all offsets off of curItem.
    Int32 itemCount;
    mItems->GetSize(&itemCount);
    Float offset = curItem->mOffset;
    Int32 pos = curItem->mPosition - 1;
    mFirstOffset = curItem->mPosition == 0 ? curItem->mOffset : -Elastos::Core::Math::Float_MAX_VALUE;
    mLastOffset = curItem->mPosition == N - 1 ?
            curItem->mOffset + curItem->mWidthFactor - 1 : Elastos::Core::Math::Float_MAX_VALUE;
    // Previous pages
    for (Int32 i = curIndex - 1; i >= 0; i--, pos--) {
        AutoPtr<IInterface> value;
        mItems->Get(i, (IInterface**)&value);
        AutoPtr<ItemInfo> ii = (ItemInfo*)(IObject*)value.Get();
        while (pos > ii->Mposition) {
            Int32 width;
            mAdapter->GetPageWidth(pos--, &width);
            offset -= width + marginOffset;
        }
        offset -= ii->mWidthFactor + marginOffset;
        ii->mOffset = offset;
        if (ii->mPosition == 0) mFirstOffset = offset;
    }
    offset = curItem->mOffset + curItem->mWidthFactor + marginOffset;
    pos = curItem->mPosition + 1;
    // Next pages
    for (Int32 i = curIndex + 1; i < itemCount; i++, pos++) {
        AutoPtr<IInterface> value;
        mItems->Get(i, (IInterface**)&value);
        AutoPtr<ItemInfo> ii = (ItemInfo*)(IObject*)value.Get();
        while (pos < ii->mPosition) {
            Int32 width;
            mAdapter->GetPageWidth(pos++, &width);
            offset += width + marginOffset;
        }
        if (ii->mPosition == N - 1) {
            mLastOffset = offset + ii->mWidthFactor - 1;
        }
        ii->mOffset = offset;
        offset += ii->mWidthFactor + marginOffset;
    }

    mNeedCalculatePageOffsets = FALSE;
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
