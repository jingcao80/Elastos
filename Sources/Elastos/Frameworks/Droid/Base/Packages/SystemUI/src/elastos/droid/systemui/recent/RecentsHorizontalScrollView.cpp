#include "elastos/droid/systemui/recent/RecentsHorizontalScrollView.h"
#include "elastos/droid/systemui/recent/RecentsPanelView.h"
#include "elastos/droid/systemui/CSwipeHelper.h"
#include "../R.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Database::EIID_IDataSetObserver;
using Elastos::Droid::SystemUI::CSwipeHelper;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::EIID_IOnGlobalLayoutListener;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::Widget::EIID_IHorizontalScrollView;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::Math;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

//============================================================================================
// RecentsHorizontalScrollView::NoOpListener
//============================================================================================
CAR_INTERFACE_IMPL(RecentsHorizontalScrollView::NoOpListener, Object, IViewOnTouchListener)

ECode RecentsHorizontalScrollView::NoOpListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}


//============================================================================================
// RecentsHorizontalScrollView::ClickListener
//============================================================================================
CAR_INTERFACE_IMPL(RecentsHorizontalScrollView::ClickListener, Object, IViewOnClickListener)

ECode RecentsHorizontalScrollView::ClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->mCallback->Dismiss();
}


//============================================================================================
// RecentsHorizontalScrollView::LaunchAppListener
//============================================================================================
CAR_INTERFACE_IMPL(RecentsHorizontalScrollView::LaunchAppListener, Object, IViewOnClickListener)

ECode RecentsHorizontalScrollView::LaunchAppListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->mCallback->HandleOnClick(mView);
}


//============================================================================================
// RecentsHorizontalScrollView::LongClickListener
//============================================================================================
CAR_INTERFACE_IMPL(RecentsHorizontalScrollView::LongClickListener, Object, IViewOnLongClickListener)

ECode RecentsHorizontalScrollView::LongClickListener::OnLongClick(
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


//============================================================================================
// RecentsHorizontalScrollView::UpdateScroll
//============================================================================================
CAR_INTERFACE_IMPL(RecentsHorizontalScrollView::UpdateScroll, Object, IOnGlobalLayoutListener)

ECode RecentsHorizontalScrollView::UpdateScroll::OnGlobalLayout()
{
    mHost->mLastScrollPosition = mHost->ScrollPositionOfMostRecent();
    mHost->ScrollTo(mHost->mLastScrollPosition, 0);
    AutoPtr<IViewTreeObserver> observer;
    mHost->GetViewTreeObserver((IViewTreeObserver**)&observer);
    Boolean b;
    observer->IsAlive(&b);
    if (b) {
        observer->RemoveGlobalOnLayoutListener(this);
    }
    return NOERROR;
}


//============================================================================================
// RecentsHorizontalScrollView::SizeChangedRunnable
//============================================================================================
CAR_INTERFACE_IMPL(RecentsHorizontalScrollView::SizeChangedRunnable, Object, IRunnable)

ECode RecentsHorizontalScrollView::SizeChangedRunnable::Run()
{
    // Make sure we're still not clobbering the transition-set values, since this
    // runnable launches asynchronously
    AutoPtr<ILayoutTransition> transition;
    IViewGroup::Probe(mHost->mLinearLayout)->GetLayoutTransition((ILayoutTransition**)&transition);
    Boolean b = FALSE;
    if (transition == NULL || (transition->IsRunning(&b), !b)) {
        mHost->ScrollTo(mHost->mLastScrollPosition, 0);
    }
    return NOERROR;
}


//============================================================================================
// RecentsHorizontalScrollView::AdapterDataSetObserver
//============================================================================================
CAR_INTERFACE_IMPL(RecentsHorizontalScrollView::AdapterDataSetObserver, Object, IDataSetObserver)

ECode RecentsHorizontalScrollView::AdapterDataSetObserver::OnChanged()
{
    mHost->Update();
    return NOERROR;
}

ECode RecentsHorizontalScrollView::AdapterDataSetObserver::OnInvalidated()
{
    mHost->Update();
    return NOERROR;
}


//============================================================================================
// RecentsHorizontalScrollView
//============================================================================================
const String RecentsHorizontalScrollView::TAG("RecentsPanelView"); // = RecentsPanelView.TAG;
const Boolean RecentsHorizontalScrollView::DEBUG = FALSE; // = RecentsPanelView.DEBUG;

// CAR_INTERFACE_IMPL_2(RecentsHorizontalScrollView, HorizontalScrollView, ISwipeHelperCallback, IRecentsPanelViewRecentsScrollView)
CAR_INTERFACE_IMPL_3(RecentsHorizontalScrollView, FrameLayout, IHorizontalScrollView, ISwipeHelperCallback, IRecentsPanelViewRecentsScrollView)

RecentsHorizontalScrollView::RecentsHorizontalScrollView(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
    : mLastScrollPosition(0)
    , mCallback(NULL)
    , mNumItemsInOneScreenful(0)
{
    //TODO
    // HorizontalScrollView::constructor(ctx, attrs, 0);

    CSwipeHelper::New(ISwipeHelper::Y, this, ctx, (ISwipeHelper**)&mSwipeHelper);

    mFadedEdgeDrawHelper = FadedEdgeDrawHelper::Create(ctx, attrs, this, FALSE);
    CHashSet::New((IHashSet**)&mRecycledViews);
}

ECode RecentsHorizontalScrollView::SetMinSwipeAlpha(
    /* [in] */ Float minAlpha)
{
    mSwipeHelper->SetMinSwipeProgress(minAlpha);
    return NOERROR;
}

Int32 RecentsHorizontalScrollView::ScrollPositionOfMostRecent()
{
    Int32 h1, h2;
    IView::Probe(mLinearLayout)->GetWidth(&h1);
    GetWidth(&h2);
    return h1 - h2;
}

void RecentsHorizontalScrollView::AddToRecycledViews(
    /* [in] */ IView* v)
{
    Int32 size;
    mRecycledViews->GetSize(&size);
    if (size < mNumItemsInOneScreenful) {
        mRecycledViews->Add(v);
    }
}

ECode RecentsHorizontalScrollView::FindViewForTask(
    /* [in] */ Int32 persistentTaskId,
    /* [out] */ IView** view)
{
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
            REFCOUNT_ADD(*view)
            return NOERROR;
        }
    }
    *view =  NULL;
    return NOERROR;
}

void RecentsHorizontalScrollView::Update()
{
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

        AutoPtr<LaunchAppListener> launchAppListener = new LaunchAppListener(view, this);

        AutoPtr<IInterface> tag;
        view->GetTag((IInterface**)&tag);
        AutoPtr<RecentsPanelView::ViewHolder> holder = (RecentsPanelView::ViewHolder*)(IObject::Probe(tag));
        AutoPtr<IView> thumbnailView = holder->mThumbnailView;
        AutoPtr<LongClickListener> longClickListener = new LongClickListener(view, thumbnailView, this);
        thumbnailView->SetClickable(TRUE);
        thumbnailView->SetOnClickListener(launchAppListener);
        thumbnailView->SetOnLongClickListener(longClickListener);

        // We don't want to dismiss recents if a user clicks on the app title
        // (we also don't want to launch the app either, though, because the
        // app title is a small target and doesn't have great click feedback)
        AutoPtr<IView> appTitle;
        view->FindViewById(R::id::app_label, (IView**)&appTitle);
        AutoPtr<ICharSequence> cs;
        CString::New(String(""), (ICharSequence**)&cs);
        appTitle->SetContentDescription(cs);
        appTitle->SetOnTouchListener(noOpListener);
        IViewGroup::Probe(mLinearLayout)->AddView(view);
    }
    SetLayoutTransition(transitioner);

    // Scroll to end after initial layout.
    AutoPtr<UpdateScroll> updateScroll = new UpdateScroll(this);
    AutoPtr<IViewTreeObserver> vto;
    GetViewTreeObserver((IViewTreeObserver**)&vto);
    vto->AddOnGlobalLayoutListener(updateScroll);
}

ECode RecentsHorizontalScrollView::RemoveViewInLayout(
    /* [in] */ IView* view)
{
    return DismissChild(view);
}

Boolean RecentsHorizontalScrollView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    if (DEBUG) Logger::V(TAG, "onInterceptTouchEvent()");
    Boolean b1, b2;
    mSwipeHelper->OnInterceptTouchEvent(ev, &b1);
    b2 = this->OnInterceptTouchEvent(ev);
    return b1 || b2;
}

ECode RecentsHorizontalScrollView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b1, b2;
    mSwipeHelper->OnTouchEvent(ev, &b1);
    this->OnTouchEvent(ev, &b2);
    *result = b1 || b2;
    return NOERROR;
}

Boolean RecentsHorizontalScrollView::CanChildBeDismissed(
    /* [in] */ IView* v)
{
    return TRUE;
}

ECode RecentsHorizontalScrollView::IsAntiFalsingNeeded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode RecentsHorizontalScrollView::GetFalsingThresholdFactor(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 1.0f;
    return NOERROR;
}

ECode RecentsHorizontalScrollView::DismissChild(
    /* [in] */ IView* v)
{
    return mSwipeHelper->DismissChild(v, 0);
}

ECode RecentsHorizontalScrollView::OnChildDismissed(
    /* [in] */ IView* v)
{
    AddToRecycledViews(v);
    IViewManager::Probe(mLinearLayout)->RemoveView(v);
    mCallback->HandleSwipe(v);
    // Restore the alpha/translation parameters to what they were before swiping
    // (for when these items are recycled)
    AutoPtr<IView> contentView;
    GetChildContentView(v, (IView**)&contentView);
    contentView->SetAlpha(1);
    contentView->SetTranslationY(0);
    return NOERROR;
}

ECode RecentsHorizontalScrollView::OnBeginDrag(
    /* [in] */ IView* v)
{
    // We do this so the underlying ScrollView knows that it won't get
    // the chance to intercept events anymore
    return RequestDisallowInterceptTouchEvent(TRUE);
}

ECode RecentsHorizontalScrollView::OnDragCancelled(
    /* [in] */ IView* v)
{
    return NOERROR;
}

ECode RecentsHorizontalScrollView::OnChildSnappedBack(
    /* [in] */ IView* animView)
{
    return NOERROR;
}

ECode RecentsHorizontalScrollView::UpdateSwipeProgress(
    /* [in] */ IView* animView,
    /* [in] */ Boolean dismissable,
    /* [in] */ Float swipeProgress,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode RecentsHorizontalScrollView::GetChildAtPosition(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    Float x, y;
    Float x1, y1;
    ev->GetX(&x1);
    Int32 x2, y2;
    GetScrollX(&x2);
    x = x1 + x2;
    ev->GetY(&y1);
    GetScrollY(&y2);
    y = y1 + y2;

    Int32 count;
    IViewGroup::Probe(mLinearLayout)->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> item;
        IViewGroup::Probe(mLinearLayout)->GetChildAt(i, (IView**)&item);
        Int32 left, right, top, bottom;
        item->GetLeft(&left);
        item->GetRight(&right);
        item->GetTop(&top);
        item->GetBottom(&bottom);
        if (x >= left && x < right
                && y >= top && y < bottom) {
            *view = item;
            REFCOUNT_ADD(*view);
            return NOERROR;
        }
    }
    *view = NULL;
    return NOERROR;
}

ECode RecentsHorizontalScrollView::GetChildContentView(
    /* [in] */ IView* v,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(v);
    return v->FindViewById(R::id::recent_item, view);
}

ECode RecentsHorizontalScrollView::DrawFadedEdges(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right,
    /* [in] */ Int32 top,
    /* [in] */ Int32 bottom)
{
    if (mFadedEdgeDrawHelper != NULL) {
        Int32 x, y, pt;
        GetScrollX(&x);
        GetScrollY(&y);
        Float f1, f2;
        f1 = GetLeftFadingEdgeStrength();
        f2 = GetRightFadingEdgeStrength();
        GetPaddingTop(&pt);
        mFadedEdgeDrawHelper->DrawCallback(canvas,
            left, right, top, bottom, x, y, 0.0f, 0.0f, f1, f2, pt);
    }
    return NOERROR;
}

void RecentsHorizontalScrollView::OnScrollChanged(
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

ECode RecentsHorizontalScrollView::SetOnScrollListener(
    /* [in] */ IRunnable* listener)
{
    mOnScrollListener = listener;
    return NOERROR;
}

Int32 RecentsHorizontalScrollView::GetVerticalFadingEdgeLength()
{
    Int32 i;
    if (mFadedEdgeDrawHelper != NULL) {
        mFadedEdgeDrawHelper->GetVerticalFadingEdgeLength(&i);
    }
    else {
        i = GetVerticalFadingEdgeLength();
    }
    return i;
}

Int32 RecentsHorizontalScrollView::GetHorizontalFadingEdgeLength()
{
    Int32 i;
    if (mFadedEdgeDrawHelper != NULL) {
        mFadedEdgeDrawHelper->GetHorizontalFadingEdgeLength(&i);
    }
    else {
        i = GetHorizontalFadingEdgeLength();
    }
    return i;
}

ECode RecentsHorizontalScrollView::OnFinishInflate()
{
    OnFinishInflate();
    SetScrollbarFadingEnabled(TRUE);
    AutoPtr<IView> v;
    FindViewById(R::id::recents_linear_layout, (IView**)&v);
    mLinearLayout = ILinearLayout::Probe(v);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Int32 leftPadding;
    res->GetDimensionPixelOffset(R::dimen::status_bar_recents_thumbnail_left_margin, &leftPadding);
    SetOverScrollEffectPadding(leftPadding, 0);
    return NOERROR;
}

ECode RecentsHorizontalScrollView::OnAttachedToWindow()
{
    if (mFadedEdgeDrawHelper != NULL) {
        Boolean b;
        IsHardwareAccelerated(&b);
        mFadedEdgeDrawHelper->OnAttachedToWindowCallback(mLinearLayout, b);
    }
    return NOERROR;
}

void RecentsHorizontalScrollView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    OnConfigurationChanged(newConfig);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> dm;
    resources->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float densityScale;
    dm->GetDensity(&densityScale);
    mSwipeHelper->SetDensityScale(densityScale);
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IViewConfiguration> vc;
    helper->Get(context, (IViewConfiguration**)&vc);
    Int32 pagingTouchSlop;
    vc->GetScaledPagingTouchSlop(&pagingTouchSlop);
    mSwipeHelper->SetPagingTouchSlop((Float)pagingTouchSlop);
}

void RecentsHorizontalScrollView::SetOverScrollEffectPadding(
    /* [in] */ Int32 leftPadding,
    /* [in] */ Int32 i)
{
}

void RecentsHorizontalScrollView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    OnSizeChanged(w, h, oldw, oldh);

    // Skip this work if a transition is running; it sets the scroll values independently
    // and should not have those animated values clobbered by this logic
    AutoPtr<ILayoutTransition> transition;
    IViewGroup::Probe(mLinearLayout)->GetLayoutTransition((ILayoutTransition**)&transition);
    Boolean b = FALSE;
    if (transition != NULL && (transition->IsRunning(&b), b)) {
        return;
    }
    // Keep track of the last visible item in the list so we can restore it
    // to the bottom when the orientation changes.
    mLastScrollPosition = ScrollPositionOfMostRecent();

    AutoPtr<SizeChangedRunnable> run = new SizeChangedRunnable(this);
    Post(run, &b);
}

ECode RecentsHorizontalScrollView::SetAdapter(
    /* [in] */ ITaskDescriptionAdapter* adapter)
{
    mAdapter = adapter;
    AutoPtr<AdapterDataSetObserver> observer = new AdapterDataSetObserver(this);
    mAdapter->RegisterDataSetObserver(observer);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> dm;
    resources->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Int32 widthPixels, heightPixels;
    dm->GetWidthPixels(&widthPixels);
    dm->GetHeightPixels(&heightPixels);
    Int32 childWidthMeasureSpec =
            MeasureSpec::MakeMeasureSpec(widthPixels, MeasureSpec::AT_MOST);
    Int32 childheightMeasureSpec =
            MeasureSpec::MakeMeasureSpec(heightPixels, MeasureSpec::AT_MOST);
    AutoPtr<IView> child;
    mAdapter->CreateView(IViewGroup::Probe(mLinearLayout), (IView**)&child);
    child->Measure(childWidthMeasureSpec, childheightMeasureSpec);
    Int32 width;
    child->GetMeasuredWidth(&width);
    mNumItemsInOneScreenful = (Int32)Elastos::Core::Math::Ceil(widthPixels / (Float)width); //TODO: FloatMath
    AddToRecycledViews(child);

    for (Int32 i = 0; i < mNumItemsInOneScreenful - 1; i++) {
        AutoPtr<IView> child;
        mAdapter->CreateView(IViewGroup::Probe(mLinearLayout), (IView**)&child);
        AddToRecycledViews(child);
    }
    return NOERROR;
}

Int32 RecentsHorizontalScrollView::NumItemsInOneScreenful()
{
    return mNumItemsInOneScreenful;
}

ECode RecentsHorizontalScrollView::SetLayoutTransition(
    /* [in] */ ILayoutTransition* transition)
{
    // The layout transition applies to our embedded LinearLayout
    return IViewGroup::Probe(mLinearLayout)->SetLayoutTransition(transition);
}

ECode RecentsHorizontalScrollView::SetCallback(
    /* [in] */ IRecentsCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
