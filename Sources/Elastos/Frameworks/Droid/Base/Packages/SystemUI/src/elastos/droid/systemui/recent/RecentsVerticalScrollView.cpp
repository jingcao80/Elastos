#include "elastos/droid/systemui/recent/RecentsVerticalScrollView.h"
#include "elastos/droid/systemui/recent/RecentsPanelView.h"
#include "elastos/droid/systemui/CSwipeHelper.h"
#include "../R.h"
#include "elastos/droid/utility/FloatMath.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Database::EIID_IDataSetObserver;
using Elastos::Droid::SystemUI::CSwipeHelper;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::FloatMath;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IOnGlobalLayoutListener;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

//=================================================================================
// RecentsVerticalScrollView::NoOpListener
//=================================================================================

CAR_INTERFACE_IMPL(RecentsVerticalScrollView::NoOpListener, Object, IViewOnTouchListener)

RecentsVerticalScrollView::NoOpListener::NoOpListener(
    /* [in] */  RecentsVerticalScrollView* host)
    : mHost(host)
{}

ECode RecentsVerticalScrollView::NoOpListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

//=================================================================================
// RecentsVerticalScrollView::ClickListener
//=================================================================================

CAR_INTERFACE_IMPL(RecentsVerticalScrollView::ClickListener, Object, IViewOnClickListener)

RecentsVerticalScrollView::ClickListener::ClickListener(
    /* [in] */  RecentsVerticalScrollView* host)
    : mHost(host)
{}

ECode RecentsVerticalScrollView::ClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->mCallback->Dismiss();
}

//=================================================================================
// RecentsVerticalScrollView::LaunchAppListener
//=================================================================================

CAR_INTERFACE_IMPL(RecentsVerticalScrollView::LaunchAppListener, Object, IViewOnClickListener)

RecentsVerticalScrollView::LaunchAppListener::LaunchAppListener(
    /* [in] */  RecentsVerticalScrollView* host,
    /* [in] */  IView* view)
    : mHost(host)
    , mView(view)
{}

ECode RecentsVerticalScrollView::LaunchAppListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->mCallback->HandleOnClick(mView);
}

//=================================================================================
// RecentsVerticalScrollView::LongClickListener
//=================================================================================

CAR_INTERFACE_IMPL(RecentsVerticalScrollView::LongClickListener, Object, IViewOnLongClickListener)

RecentsVerticalScrollView::LongClickListener::LongClickListener(
   /* [in] */  RecentsVerticalScrollView* host,
    /* [in] */  IView* view,
    /* [in] */  IView* thumbnailView)
    : mHost(host)
    , mView(view)
    , mThumbnailView(thumbnailView)
{}

ECode RecentsVerticalScrollView::LongClickListener::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IView> anchorView;
    mView->FindViewById(R::id::app_description, (IView**)&anchorView);
    mHost->mCallback->HandleLongPress(mView, anchorView, mThumbnailView);
    *result = TRUE;
    return NOERROR;
}

//=================================================================================
// RecentsVerticalScrollView::UpdateScroll
//=================================================================================

CAR_INTERFACE_IMPL(RecentsVerticalScrollView::UpdateScroll, Object, IOnGlobalLayoutListener)

RecentsVerticalScrollView::UpdateScroll::UpdateScroll(
    /* [in] */  RecentsVerticalScrollView* host)
    : mHost(host)
{}

ECode RecentsVerticalScrollView::UpdateScroll::OnGlobalLayout()
{
    mHost->mLastScrollPosition = mHost->ScrollPositionOfMostRecent();
    mHost->ScrollTo(0, mHost->mLastScrollPosition);
    AutoPtr<IViewTreeObserver> observer;
    mHost->GetViewTreeObserver((IViewTreeObserver**)&observer);
    Boolean b;
    observer->IsAlive(&b);
    if (b) {
        observer->RemoveGlobalOnLayoutListener(this);
    }
    return NOERROR;
}

//=================================================================================
// RecentsVerticalScrollView::UpdateScroll
//=================================================================================

CAR_INTERFACE_IMPL(RecentsVerticalScrollView::SizeChangedRunnable, Object, IRunnable)

RecentsVerticalScrollView::SizeChangedRunnable::SizeChangedRunnable(
    /* [in] */  RecentsVerticalScrollView* host)
    : mHost(host)
{}

ECode RecentsVerticalScrollView::SizeChangedRunnable::Run()
{
    // Make sure we're still not clobbering the transition-set values, since this
    // runnable launches asynchronously
    AutoPtr<ILayoutTransition> transition;
    IViewGroup::Probe(mHost->mLinearLayout)->GetLayoutTransition((ILayoutTransition**)&transition);
    Boolean b;
    if (transition == NULL || (transition->IsRunning(&b), !b)) {
        mHost->ScrollTo(0, mHost->mLastScrollPosition);
    }
    return NOERROR;
}

//=================================================================================
// RecentsVerticalScrollView::AdapterDataSetObserver
//=================================================================================

CAR_INTERFACE_IMPL(RecentsVerticalScrollView::AdapterDataSetObserver, Object, IDataSetObserver)

RecentsVerticalScrollView::AdapterDataSetObserver::AdapterDataSetObserver(
   /* [in] */  RecentsVerticalScrollView* host)
    : mHost(host)
{}

ECode RecentsVerticalScrollView::AdapterDataSetObserver::OnChanged()
{
    mHost->Update();
    return NOERROR;
}

ECode RecentsVerticalScrollView::AdapterDataSetObserver::OnInvalidated()
{
    mHost->Update();
    return NOERROR;
}

//=================================================================================
// RecentsVerticalScrollView
//=================================================================================

const String RecentsVerticalScrollView::TAG("RecentsVerticalScrollView");
const Boolean RecentsVerticalScrollView::DEBUG = TRUE;

CAR_INTERFACE_IMPL_2(RecentsVerticalScrollView, ScrollView, ISwipeHelperCallback, IRecentsScrollView)

RecentsVerticalScrollView::RecentsVerticalScrollView()
    : mLastScrollPosition(0)
    , mNumItemsInOneScreenful(0)
{
}

ECode RecentsVerticalScrollView::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    ScrollView::constructor(ctx, attrs, 0);
    CSwipeHelper::New(ISwipeHelper::X, this, ctx, (ISwipeHelper**)&mSwipeHelper);
    mFadedEdgeDrawHelper = FadedEdgeDrawHelper::Create(ctx, attrs, this, TRUE);
    CHashSet::New((IHashSet**)&mRecycledViews);
    return NOERROR;
}

ECode RecentsVerticalScrollView::SetMinSwipeAlpha(
    /* [in] */ Float minAlpha)
{
    mSwipeHelper->SetMinSwipeProgress(minAlpha);
    return NOERROR;
}

Int32 RecentsVerticalScrollView::ScrollPositionOfMostRecent()
{
    Int32 h1, h2, pt;
    IView::Probe(mLinearLayout)->GetHeight(&h1);
    GetHeight(&h2);
    GetPaddingTop(&pt);
    return h1 - h2 + pt;
}

void RecentsVerticalScrollView::AddToRecycledViews(
    /* [in] */ IView* v)
{
    Int32 size;
    mRecycledViews->GetSize(&size);
    if (size < mNumItemsInOneScreenful) {
        mRecycledViews->Add(v);
    }
}

ECode RecentsVerticalScrollView::FindViewForTask(
    /* [in] */ Int32 persistentTaskId,
    /* [out] */ IView** view)
{
    Logger::I(TAG, " >> FindViewForTask: %08x, %d", persistentTaskId, persistentTaskId);
    VALIDATE_NOT_NULL(view);
    Int32 count;
    IViewGroup::Probe(mLinearLayout)->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> v;
        IViewGroup::Probe(mLinearLayout)->GetChildAt(i, (IView**)&v);

        AutoPtr<IInterface> tag;
        v->GetTag((IInterface**)&tag);
        AutoPtr<RecentsPanelView::ViewHolder> holder = (RecentsPanelView::ViewHolder*)(IObject::Probe(tag));
        AutoPtr<TaskDescription> td = (TaskDescription*)(holder->mTaskDescription).Get();
        Int32 pId = td->mPersistentTaskId;
        if (pId == persistentTaskId) {
            *view = v;
            REFCOUNT_ADD(*view);
            return NOERROR;
        }
    }
    *view =  NULL;
    return NOERROR;
}

void RecentsVerticalScrollView::Update()
{
    Logger::I(TAG, " >> Update()");
    Int32 count;
    IViewGroup::Probe(mLinearLayout)->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> v;
        IViewGroup::Probe(mLinearLayout)->GetChildAt(i, (IView**)&v);
        AddToRecycledViews(v);
        mAdapter->RecycleView(v);
    }
    AutoPtr<ILayoutTransition> transitioner;
    GetLayoutTransition((ILayoutTransition**)&transitioner);
    SetLayoutTransition(NULL);

    IViewGroup::Probe(mLinearLayout)->RemoveAllViews();

    // Once we can clear the data associated with individual item views,
    // we can get rid of the removeAllViews() and the code below will
    // recycle them.
    AutoPtr<IIterator> recycledViews;
    IIterable::Probe(mRecycledViews)->GetIterator((IIterator**)&recycledViews);
    Int32 c;
    mAdapter->GetCount(&c);
    for (Int32 i = 0; i < c; i++) {
        AutoPtr<IView> old;
        Boolean hasNext;
        if (recycledViews->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> next;
            recycledViews->GetNext((IInterface**)&next);
            old = IView::Probe(next);
            recycledViews->Remove();
            old->SetVisibility(IView::VISIBLE);
        }
        AutoPtr<IView> view;
        mAdapter->GetView(i, old, IViewGroup::Probe(mLinearLayout), (IView**)&view);

        if (mFadedEdgeDrawHelper != NULL) {
            mFadedEdgeDrawHelper->AddViewCallback(view);
        }

        AutoPtr<NoOpListener> noOpListener = new NoOpListener(this);

        AutoPtr<ClickListener> cl = new ClickListener(this);
        view->SetOnClickListener(cl);
        // We don't want a click sound when we dimiss recents
        view->SetSoundEffectsEnabled(FALSE);

        AutoPtr<LaunchAppListener> launchAppListener = new LaunchAppListener(this, view);

        AutoPtr<IInterface> tag;
        view->GetTag((IInterface**)&tag);
        AutoPtr<RecentsPanelView::ViewHolder> holder = (RecentsPanelView::ViewHolder*)(IObject::Probe(tag));
        AutoPtr<IView> thumbnailView = holder->mThumbnailView;
        AutoPtr<LongClickListener> longClickListener = new LongClickListener(this, view, thumbnailView);
        thumbnailView->SetClickable(TRUE);
        thumbnailView->SetOnClickListener(launchAppListener);
        thumbnailView->SetOnLongClickListener(longClickListener);

        // We don't want to dismiss recents if a user clicks on the app title
        // (we also don't want to launch the app either, though, because the
        // app title is a small target and doesn't have great click feedback)
        AutoPtr<IView> appTitle;
        view->FindViewById(R::id::app_label, (IView**)&appTitle);
        AutoPtr<ICharSequence> cs;
        CString::New(String(" "), (ICharSequence**)&cs);
        appTitle->SetContentDescription(cs);
        appTitle->SetOnTouchListener(noOpListener);
        AutoPtr<IView> calloutLine;
        view->FindViewById(R::id::recents_callout_line, (IView**)&calloutLine);
        if (calloutLine != NULL) {
            calloutLine->SetOnTouchListener(noOpListener);
        }

        IViewGroup::Probe(mLinearLayout)->AddView(view);
    }
    SetLayoutTransition(transitioner);

    // Scroll to end after initial layout.
    AutoPtr<UpdateScroll> updateScroll = new UpdateScroll(this);
    AutoPtr<IViewTreeObserver> vto;
    GetViewTreeObserver((IViewTreeObserver**)&vto);
    vto->AddOnGlobalLayoutListener(updateScroll);
    Logger::I(TAG, " << Update()");
}

ECode RecentsVerticalScrollView::RemoveViewInLayout(
    /* [in] */ IView* view)
{
    return DismissChild(view);
}

ECode RecentsVerticalScrollView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (DEBUG) Logger::V(TAG, "OnInterceptTouchEvent()");
    Boolean b1, b2;
    *result = (mSwipeHelper->OnInterceptTouchEvent(ev, &b1), b1)
        || (ScrollView::OnInterceptTouchEvent(ev, &b2), b2);
    return NOERROR;
}

ECode RecentsVerticalScrollView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b1, b2;
    *result = (mSwipeHelper->OnTouchEvent(ev, &b1), b1)
        || (ScrollView::OnTouchEvent(ev, &b2), b2);
    return NOERROR;
}

ECode RecentsVerticalScrollView::CanChildBeDismissed(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode RecentsVerticalScrollView::IsAntiFalsingNeeded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode RecentsVerticalScrollView::GetFalsingThresholdFactor(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 1.0f;
    return NOERROR;
}

ECode RecentsVerticalScrollView::DismissChild(
    /* [in] */ IView* v)
{
    return mSwipeHelper->DismissChild(v, 0);
}

ECode RecentsVerticalScrollView::OnChildDismissed(
    /* [in] */ IView* v)
{
    AddToRecycledViews(v);
    IViewGroup::Probe(mLinearLayout)->RemoveViewInLayout(v);
    mCallback->HandleSwipe(v);
    // Restore the alpha/translation parameters to what they were before swiping
    // (for when these items are recycled)
    AutoPtr<IView> contentView;
    GetChildContentView(v, (IView**)&contentView);
    contentView->SetAlpha(1);
    contentView->SetTranslationX(0);
    return NOERROR;
}

ECode RecentsVerticalScrollView::OnBeginDrag(
    /* [in] */ IView* v)
{
    // We do this so the underlying ScrollView knows that it won't get
    // the chance to intercept events anymore
    return RequestDisallowInterceptTouchEvent(TRUE);
}

ECode RecentsVerticalScrollView::OnDragCancelled(
    /* [in] */ IView* v)
{
    return NOERROR;
}

ECode RecentsVerticalScrollView::OnChildSnappedBack(
    /* [in] */ IView* animView)
{
    return NOERROR;
}

ECode RecentsVerticalScrollView::UpdateSwipeProgress(
    /* [in] */ IView* animView,
    /* [in] */ Boolean dismissable,
    /* [in] */ Float swipeProgress,
    /* [in] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode RecentsVerticalScrollView::GetChildAtPosition(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    Float x, y, x1, y1;
    Int32 x2, y2;
    ev->GetX(&x1);
    GetScrollX(&x2);
    ev->GetY(&y1);
    GetScrollY(&y2);
    x = x1 + x2;
    y = y1 + y2;

    Logger::I(TAG, "GetChildAtPosition: (%.2f, %.2f)", x, y);

    Int32 count;
    IViewGroup::Probe(mLinearLayout)->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> item;
        IViewGroup::Probe(mLinearLayout)->GetChildAt(i, (IView**)&item);
        Int32 visibility;
        item->GetVisibility(&visibility);
        Int32 left, right, top, bottom;
        item->GetLeft(&left);
        item->GetRight(&right);
        item->GetTop(&top);
        item->GetBottom(&bottom);
        if (visibility == IView::VISIBLE
                && x >= left && x < right
                && y >= top && y < bottom) {
            *view = item;
            REFCOUNT_ADD(*view);
            return NOERROR;
        }
    }
    *view = NULL;
    return NOERROR;
}

ECode RecentsVerticalScrollView::GetChildContentView(
    /* [in] */ IView* v,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(v);
    return v->FindViewById(R::id::recent_item, view);
}

ECode RecentsVerticalScrollView::DrawFadedEdges(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right,
    /* [in] */ Int32 top,
    /* [in] */ Int32 bottom)
{
    if (mFadedEdgeDrawHelper != NULL) {
        Boolean offsetRequired = IsPaddingOffsetRequired();
        Int32 x, y, top, ft;
        GetScrollX(&x);
        GetScrollY(&y);
        GetPaddingTop(&top);
        ft = GetFadeTop(offsetRequired);
        Float ts, bs;
        ts = GetTopFadingEdgeStrength();
        bs = GetBottomFadingEdgeStrength();
        mFadedEdgeDrawHelper->DrawCallback(canvas,
            left, right, top + ft, bottom, x, y, ts, bs, 0, 0, top);
    }
    return NOERROR;
}

void RecentsVerticalScrollView::OnScrollChanged(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 oldl,
    /* [in] */ Int32 oldt)
{
   OnScrollChanged(l, t, oldl, oldt);
   if (mOnScrollListener != NULL) {
       mOnScrollListener->Run();
   }
}

ECode RecentsVerticalScrollView::SetOnScrollListener(
    /* [in] */ IRunnable* listener)
{
    mOnScrollListener = listener;
    return NOERROR;
}

ECode RecentsVerticalScrollView::GetVerticalFadingEdgeLength(
    /* [out] */ Int32* length)
{
    if (mFadedEdgeDrawHelper != NULL) {
        return mFadedEdgeDrawHelper->GetVerticalFadingEdgeLength(length);
    }
    else {
        return ScrollView::GetVerticalFadingEdgeLength(length);
    }
}

ECode RecentsVerticalScrollView::GetHorizontalFadingEdgeLength(
    /* [out] */ Int32* length)
{
    if (mFadedEdgeDrawHelper != NULL) {
        return mFadedEdgeDrawHelper->GetHorizontalFadingEdgeLength(length);
    }
    else {
        return ScrollView::GetHorizontalFadingEdgeLength(length);
    }
}

ECode RecentsVerticalScrollView::OnFinishInflate()
{
    FAIL_RETURN(ScrollView::OnFinishInflate());
    SetScrollbarFadingEnabled(TRUE);
    AutoPtr<IView> tmpObj;
    FindViewById(R::id::recents_linear_layout, (IView**)&tmpObj);
    mLinearLayout = ILinearLayout::Probe(tmpObj.Get());
    Int32 leftPadding;
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    res->GetDimensionPixelOffset(R::dimen::status_bar_recents_thumbnail_left_margin, &leftPadding);
    SetOverScrollEffectPadding(leftPadding, 0);
    return NOERROR;
}

ECode RecentsVerticalScrollView::OnAttachedToWindow()
{
    if (mFadedEdgeDrawHelper != NULL) {
        Boolean isHardwareAccelerated;
        IsHardwareAccelerated(&isHardwareAccelerated);
        mFadedEdgeDrawHelper->OnAttachedToWindowCallback(mLinearLayout, isHardwareAccelerated);
    }
    return NOERROR;
}

void RecentsVerticalScrollView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    ScrollView::OnConfigurationChanged(newConfig);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> dm;
    resources->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float densityScale;
    dm->GetDensity(&densityScale);
    mSwipeHelper->SetDensityScale(densityScale);
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IViewConfiguration> vc;
    helper->Get(ctx, (IViewConfiguration**)&vc);
    Int32 pagingTouchSlop;
    vc->GetScaledPagingTouchSlop(&pagingTouchSlop);
    mSwipeHelper->SetPagingTouchSlop((Float)pagingTouchSlop);
}

void RecentsVerticalScrollView::SetOverScrollEffectPadding(
    /* [in] */ Int32 leftPadding,
    /* [in] */ Int32 i)
{
    return;
}

void RecentsVerticalScrollView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    ScrollView::OnSizeChanged(w, h, oldw, oldh);

    // Skip this work if a transition is running; it sets the scroll values independently
    // and should not have those animated values clobbered by this logic
    AutoPtr<ILayoutTransition> transition;
    IViewGroup::Probe(mLinearLayout)->GetLayoutTransition((ILayoutTransition**)&transition);
    Boolean b;
    if (transition != NULL && (transition->IsRunning(&b), b)) {
        return;
    }
    // Keep track of the last visible item in the list so we can restore it
    // to the bottom when the orientation changes.
    mLastScrollPosition = ScrollPositionOfMostRecent();

    AutoPtr<SizeChangedRunnable> run = new SizeChangedRunnable(this);
    Post(run, &b);
}

ECode RecentsVerticalScrollView::SetAdapter(
    /* [in] */ ITaskDescriptionAdapter* adapter)
{
    Logger::I(TAG, " >> SetAdapter: %s", TO_CSTR(adapter));

    mAdapter = adapter;
    AutoPtr<AdapterDataSetObserver> observer = new AdapterDataSetObserver(this);
    IAdapter::Probe(mAdapter)->RegisterDataSetObserver(observer);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> dm;
    resources->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Int32 widthPixels, heightPixels;
    dm->GetWidthPixels(&widthPixels);
    dm->GetHeightPixels(&heightPixels);
    Int32 childWidthMeasureSpec = MeasureSpec::MakeMeasureSpec(widthPixels, MeasureSpec::AT_MOST);
    Int32 childheightMeasureSpec = MeasureSpec::MakeMeasureSpec(heightPixels, MeasureSpec::AT_MOST);
    AutoPtr<IView> child;
    mAdapter->CreateView(IViewGroup::Probe(mLinearLayout), (IView**)&child);
    child->Measure(childWidthMeasureSpec, childheightMeasureSpec);
    Int32 height;
    child->GetMeasuredHeight(&height);
    mNumItemsInOneScreenful =(Int32)FloatMath::Ceil(heightPixels / (Float)height);
    AddToRecycledViews(child);

    for (Int32 i = 0; i < mNumItemsInOneScreenful - 1; i++) {
        AutoPtr<IView> child;
        mAdapter->CreateView(IViewGroup::Probe(mLinearLayout), (IView**)&child);
        AddToRecycledViews(child);
    }
    return NOERROR;
}

ECode RecentsVerticalScrollView::NumItemsInOneScreenful(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mNumItemsInOneScreenful;
    return NOERROR;
}

ECode RecentsVerticalScrollView::SetLayoutTransition(
    /* [in] */ ILayoutTransition* transition)
{
    // The layout transition applies to our embedded LinearLayout
    return IViewGroup::Probe(mLinearLayout)->SetLayoutTransition(transition);
}

ECode RecentsVerticalScrollView::SetCallback(
    /* [in] */ IRecentsCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
