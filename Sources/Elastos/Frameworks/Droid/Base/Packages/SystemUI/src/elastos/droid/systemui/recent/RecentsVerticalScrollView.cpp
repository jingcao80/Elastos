#include "elastos/droid/systemui/recent/RecentsVerticalScrollView.h"
#include "elastos/droid/systemui/recent/RecentsPanelView.h"
#include "elastos/droid/systemui/CSwipeHelper.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include <elastos/core/Math.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Database::EIID_IDataSetObserver;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IOnGlobalLayoutListener;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::SystemUI::SystemUIR;
using Elastos::Droid::SystemUI::CSwipeHelper;
using Elastos::Droid::SystemUI::Recent::RecentsPanelView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {


//=================================================================================
// RecentsVerticalScrollView::NoOpListener
//=================================================================================
CAR_INTERFACE_IMPL(RecentsVerticalScrollView::NoOpListener, IViewOnTouchListener)

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
CAR_INTERFACE_IMPL(RecentsVerticalScrollView::ClickListener, IViewOnClickListener)

ECode RecentsVerticalScrollView::ClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->mCallback->Dismiss();
}


//=================================================================================
// RecentsVerticalScrollView::LaunchAppListener
//=================================================================================
CAR_INTERFACE_IMPL(RecentsVerticalScrollView::LaunchAppListener, IViewOnClickListener)

ECode RecentsVerticalScrollView::LaunchAppListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->mCallback->HandleOnClick(mView);
}


//=================================================================================
// RecentsVerticalScrollView::LongClickListener
//=================================================================================
CAR_INTERFACE_IMPL(RecentsVerticalScrollView::LongClickListener, IViewOnLongClickListener)

ECode RecentsVerticalScrollView::LongClickListener::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IView> anchorView;
    mView->FindViewById(SystemUIR::id::app_description, (IView**)&anchorView);
    mHost->mCallback->HandleLongPress(mView, anchorView, mThumbnailView);
    *result = TRUE;
    return NOERROR;
}


//=================================================================================
// RecentsVerticalScrollView::UpdateScroll
//=================================================================================
CAR_INTERFACE_IMPL(RecentsVerticalScrollView::UpdateScroll, IOnGlobalLayoutListener)

ECode RecentsVerticalScrollView::UpdateScroll::OnGlobalLayout()
{
    mHost->mLastScrollPosition = mHost->ScrollPositionOfMostRecent();
    mHost->ScrollTo(0, mHost->mLastScrollPosition);
    AutoPtr<IViewTreeObserver> observer = mHost->GetViewTreeObserver();
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
CAR_INTERFACE_IMPL(RecentsVerticalScrollView::SizeChangedRunnable, IRunnable)

ECode RecentsVerticalScrollView::SizeChangedRunnable::Run()
{
    // Make sure we're still not clobbering the transition-set values, since this
    // runnable launches asynchronously
    AutoPtr<ILayoutTransition> transition;
    mHost->mLinearLayout->GetLayoutTransition((ILayoutTransition**)&transition);
    Boolean b;
    if (transition == NULL || (transition->IsRunning(&b), !b)) {
        mHost->ScrollTo(0, mHost->mLastScrollPosition);
    }
    return NOERROR;
}


//=================================================================================
// RecentsVerticalScrollView::AdapterDataSetObserver
//=================================================================================
CAR_INTERFACE_IMPL(RecentsVerticalScrollView::AdapterDataSetObserver, IDataSetObserver)

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
const String RecentsVerticalScrollView::TAG("RecentsPanelView"); // = RecentsPanelView.TAG;
const Boolean RecentsVerticalScrollView::DEBUG = FALSE; // = RecentsPanelView.DEBUG;

RecentsVerticalScrollView::RecentsVerticalScrollView()
    : mLastScrollPosition(0)
    , mCallback(NULL)
    , mNumItemsInOneScreenful(0)
{}

ECode RecentsVerticalScrollView::Init(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(ScrollView::Init(ctx, attrs, 0));
    AutoPtr<IDisplayMetrics> dm;
    GetResources()->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float densityScale;
    dm->GetDensity(&densityScale);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> vc;
    helper->Get(mContext, (IViewConfiguration**)&vc);
    Float pagingTouchSlop;
    vc->GetScaledPagingTouchSlop((Int32*)&pagingTouchSlop);
    CSwipeHelper::New(ISwipeHelper::X, this, densityScale, pagingTouchSlop, (ISwipeHelper**)&mSwipeHelper);

    mPerformanceHelper = RecentsScrollViewPerformanceHelper::Create(ctx, attrs, this, TRUE);
    return NOERROR;
}

ECode RecentsVerticalScrollView::SetMinSwipeAlpha(
    /* [in] */ Float minAlpha)
{
    mSwipeHelper->SetMinAlpha(minAlpha);
    return NOERROR;
}

Int32 RecentsVerticalScrollView::ScrollPositionOfMostRecent()
{
    Int32 h1, h2;
    mLinearLayout->GetHeight(&h1);
    h2 = GetHeight();
    return h1 - h2;
}

void RecentsVerticalScrollView::AddToRecycledViews(
    /* [in] */ IView* v)
{
    if (mRecycledViews.GetSize() < mNumItemsInOneScreenful) {
        mRecycledViews.Insert(v);
    }
}

ECode RecentsVerticalScrollView::FindViewForTask(
    /* [in] */ Int32 persistentTaskId,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    Int32 count;
    mLinearLayout->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> v;
        mLinearLayout->GetChildAt(i, (IView**)&v);

        AutoPtr<IInterface> tag;
        v->GetTag((IInterface**)&tag);
        AutoPtr<RecentsPanelView::ViewHolder> holder = (RecentsPanelView::ViewHolder*)tag.Get();
        Int32 pId;
        holder->mTaskDescription->GetPersistentTaskId(&pId);
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
    Int32 count;
    mLinearLayout->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> v;
        mLinearLayout->GetChildAt(i, (IView**)&v);
        AddToRecycledViews(v);
        mAdapter->RecycleView(v);
    }
    AutoPtr<ILayoutTransition> transitioner = GetLayoutTransition();
    SetLayoutTransition(NULL);

    mLinearLayout->RemoveAllViews();

    // Once we can clear the data associated with individual item views,
    // we can get rid of the removeAllViews() and the code below will
    // recycle them.
    // Iterator<View> recycledViews = mRecycledViews.iterator();
    HashSet<AutoPtr<IView> >::Iterator it = mRecycledViews.Begin();
    Int32 c;
    mAdapter->GetCount(&c);
    for (Int32 i = 0; i < c; i++) {
        AutoPtr<IView> old;
        if (it != mRecycledViews.End()) {
            old = *it;
            mRecycledViews.Erase(it++);
            old->SetVisibility(IView::VISIBLE);
        }
        AutoPtr<IView> view;
        mAdapter->GetView(i, old, mLinearLayout, (IView**)&view);

        if (mPerformanceHelper != NULL) {
            mPerformanceHelper->AddViewCallback(view);
        }

        AutoPtr<NoOpListener> noOpListener = new NoOpListener(this);

        AutoPtr<ClickListener> cl = new ClickListener(this);
        view->SetOnClickListener(cl);
        // We don't want a click sound when we dimiss recents
        view->SetSoundEffectsEnabled(FALSE);

        AutoPtr<LaunchAppListener> launchAppListener = new LaunchAppListener(this, view);

        AutoPtr<IInterface> tag;
        view->GetTag((IInterface**)&tag);
        AutoPtr<RecentsPanelView::ViewHolder> holder = (RecentsPanelView::ViewHolder*)tag.Get();
        AutoPtr<IView> thumbnailView = holder->mThumbnailView;
        AutoPtr<LongClickListener> longClickListener = new LongClickListener(this, view, thumbnailView);
        thumbnailView->SetClickable(TRUE);
        thumbnailView->SetOnClickListener(launchAppListener);
        thumbnailView->SetOnLongClickListener(longClickListener);

        // We don't want to dismiss recents if a user clicks on the app title
        // (we also don't want to launch the app either, though, because the
        // app title is a small target and doesn't have great click feedback)
        AutoPtr<IView> appTitle;
        view->FindViewById(SystemUIR::id::app_label, (IView**)&appTitle);
        AutoPtr<ICharSequence> cs;
        CString::New(String(" "), (ICharSequence**)&cs);
        appTitle->SetContentDescription(cs);
        appTitle->SetOnTouchListener(noOpListener);
        AutoPtr<IView> calloutLine;
        view->FindViewById(SystemUIR::id::recents_callout_line, (IView**)&calloutLine);
        if (calloutLine != NULL) {
            calloutLine->SetOnTouchListener(noOpListener);
        }

        mLinearLayout->AddView(view);
    }
    SetLayoutTransition(transitioner);

    // Scroll to end after initial layout.
    AutoPtr<UpdateScroll> updateScroll = new UpdateScroll(this);
    GetViewTreeObserver()->AddOnGlobalLayoutListener(updateScroll);
}

ECode RecentsVerticalScrollView::RemoveViewInLayout(
    /* [in] */ IView* view)
{
    return DismissChild(view);
}

Boolean RecentsVerticalScrollView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    // if (DEBUG) Log.v(TAG, "onInterceptTouchEvent()");
    Boolean b1, b2;
    mSwipeHelper->OnInterceptTouchEvent(ev, &b1);
    b2 = ScrollView::OnInterceptTouchEvent(ev);
    return b1 || b2;
}

Boolean RecentsVerticalScrollView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    Boolean b1, b2;
    mSwipeHelper->OnTouchEvent(ev, &b1);
    b2 = ScrollView::OnTouchEvent(ev);
    return b1 || b2;
}

Boolean RecentsVerticalScrollView::CanChildBeDismissed(
    /* [in] */ IView* v)
{
    return TRUE;
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
    mLinearLayout->RemoveViewInLayout(v);
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

ECode RecentsVerticalScrollView::GetChildAtPosition(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    Float x, y;
    Float x1, x2, y1, y2;
    ev->GetX(&x1);
    x2 = GetScrollX();
    x = x1 + x2;
    ev->GetY(&y1);
    y2 = GetScrollY();
    y = y1 + y2;

    Int32 count;
    mLinearLayout->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> item;
        mLinearLayout->GetChildAt(i, (IView**)&item);
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
    return v->FindViewById(SystemUIR::id::recent_item, view);
}

ECode RecentsVerticalScrollView::Draw(
    /* [in] */ ICanvas* canvas)
{
    FAIL_RETURN(ScrollView::Draw(canvas));

    if (mPerformanceHelper != NULL) {
        Int32 paddingLeft = mPaddingLeft;
        Boolean offsetRequired = IsPaddingOffsetRequired();
        if (offsetRequired) {
            paddingLeft += GetLeftPaddingOffset();
        }

        Int32 left = mScrollX + paddingLeft;
        Int32 right = left + mRight - mLeft - mPaddingRight - paddingLeft;
        Int32 top = mScrollY + GetFadeTop(offsetRequired);
        Int32 bottom = top + GetFadeHeight(offsetRequired);

        if (offsetRequired) {
            right += GetRightPaddingOffset();
            bottom += GetBottomPaddingOffset();
        }
        mPerformanceHelper->DrawCallback(canvas,
                left, right, top, bottom, mScrollX, mScrollY,
                GetTopFadingEdgeStrength(), GetBottomFadingEdgeStrength(),
                0, 0);
    }
    return NOERROR;
}

Int32 RecentsVerticalScrollView::GetVerticalFadingEdgeLength()
{
    if (mPerformanceHelper != NULL) {
        return mPerformanceHelper->GetVerticalFadingEdgeLengthCallback();
    }
    else {
        return ScrollView::GetVerticalFadingEdgeLength();
    }
}

Int32 RecentsVerticalScrollView::GetHorizontalFadingEdgeLength()
{
    if (mPerformanceHelper != NULL) {
        return mPerformanceHelper->GetHorizontalFadingEdgeLengthCallback();
    }
    else {
        return ScrollView::GetHorizontalFadingEdgeLength();
    }
}

ECode RecentsVerticalScrollView::OnFinishInflate()
{
    FAIL_RETURN(ScrollView::OnFinishInflate());
    SetScrollbarFadingEnabled(TRUE);
    AutoPtr<IView> tmpObj = FindViewById(SystemUIR::id::recents_linear_layout);
    mLinearLayout = ILinearLayout::Probe(tmpObj.Get());
    Int32 leftPadding;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetDimensionPixelOffset(SystemUIR::dimen::status_bar_recents_thumbnail_left_margin, &leftPadding);
    SetOverScrollEffectPadding(leftPadding, 0);
    return NOERROR;
}

ECode RecentsVerticalScrollView::OnAttachedToWindow()
{
    if (mPerformanceHelper != NULL) {
        mPerformanceHelper->OnAttachedToWindowCallback(
                mCallback, mLinearLayout, IsHardwareAccelerated());
    }
    return NOERROR;
}

void RecentsVerticalScrollView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    ScrollView::OnConfigurationChanged(newConfig);
    AutoPtr<IDisplayMetrics> dm;
    GetResources()->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float densityScale;
    dm->GetDensity(&densityScale);
    mSwipeHelper->SetDensityScale(densityScale);
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> vc;
    helper->Get(mContext, (IViewConfiguration**)&vc);
    Float pagingTouchSlop;
    vc->GetScaledPagingTouchSlop((Int32*)&pagingTouchSlop);
    mSwipeHelper->SetPagingTouchSlop(pagingTouchSlop);
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
    mLinearLayout->GetLayoutTransition((ILayoutTransition**)&transition);
    Boolean b;
    if (transition != NULL && (transition->IsRunning(&b), b)) {
        return;
    }
    // Keep track of the last visible item in the list so we can restore it
    // to the bottom when the orientation changes.
    mLastScrollPosition = ScrollPositionOfMostRecent();

    AutoPtr<SizeChangedRunnable> run = new SizeChangedRunnable(this);
    Post(run);
}

ECode RecentsVerticalScrollView::SetAdapter(
    /* [in] */ ITaskDescriptionAdapter* adapter)
{
    mAdapter = adapter;
    AutoPtr<AdapterDataSetObserver> observer = new AdapterDataSetObserver(this);
    mAdapter->RegisterDataSetObserver(observer);

    AutoPtr<IDisplayMetrics> dm;
    GetResources()->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Int32 widthPixels, heightPixels;
    dm->GetWidthPixels(&widthPixels);
    dm->GetHeightPixels(&heightPixels);
    Int32 childWidthMeasureSpec =
            MeasureSpec::MakeMeasureSpec(widthPixels, MeasureSpec::AT_MOST);
    Int32 childheightMeasureSpec =
            MeasureSpec::MakeMeasureSpec(heightPixels, MeasureSpec::AT_MOST);
    AutoPtr<IView> child;
    mAdapter->CreateView(mLinearLayout, (IView**)&child);
    child->Measure(childWidthMeasureSpec, childheightMeasureSpec);
    Int32 height;
    child->GetMeasuredHeight(&height);
    mNumItemsInOneScreenful =(Int32)Elastos::Core::Math::Ceil(heightPixels / (Float)height); //TODO: FloatMath
    AddToRecycledViews(child);

    for (Int32 i = 0; i < mNumItemsInOneScreenful - 1; i++) {
        AutoPtr<IView> child;
        mAdapter->CreateView(mLinearLayout, (IView**)&child);
        AddToRecycledViews(child);
    }
    return NOERROR;
}

Int32 RecentsVerticalScrollView::NumItemsInOneScreenful()
{
    return mNumItemsInOneScreenful;
}

ECode RecentsVerticalScrollView::SetLayoutTransition(
    /* [in] */ ILayoutTransition* transition)
{
    // The layout transition applies to our embedded LinearLayout
    return mLinearLayout->SetLayoutTransition(transition);
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
