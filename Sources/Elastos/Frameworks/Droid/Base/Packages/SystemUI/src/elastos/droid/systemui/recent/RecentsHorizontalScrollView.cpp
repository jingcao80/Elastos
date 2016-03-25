#include "elastos/droid/systemui/recent/RecentsHorizontalScrollView.h"
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
using Elastos::Droid::View::IViewManager;
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

//db79c362-50f5-47cd-8852-a4e89f920c90
// extern "C" const InterfaceID EIID_RecentsHorizontalScrollView =
//         { 0xdb79c362, 0x50f5, 0x47cd, { 0x88, 0x52, 0xa4, 0xe8, 0x9f, 0x92, 0x0c, 0x90 } };

//============================================================================================
// RecentsHorizontalScrollView::NoOpListener
//============================================================================================
CAR_INTERFACE_IMPL(RecentsHorizontalScrollView::NoOpListener, IViewOnTouchListener)

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
CAR_INTERFACE_IMPL(RecentsHorizontalScrollView::ClickListener, IViewOnClickListener)

ECode RecentsHorizontalScrollView::ClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->mCallback->Dismiss();
}


//============================================================================================
// RecentsHorizontalScrollView::LaunchAppListener
//============================================================================================
CAR_INTERFACE_IMPL(RecentsHorizontalScrollView::LaunchAppListener, IViewOnClickListener)

ECode RecentsHorizontalScrollView::LaunchAppListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->mCallback->HandleOnClick(mView);
}


//============================================================================================
// RecentsHorizontalScrollView::LongClickListener
//============================================================================================
CAR_INTERFACE_IMPL(RecentsHorizontalScrollView::LongClickListener, IViewOnLongClickListener)

ECode RecentsHorizontalScrollView::LongClickListener::OnLongClick(
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


//============================================================================================
// RecentsHorizontalScrollView::UpdateScroll
//============================================================================================
CAR_INTERFACE_IMPL(RecentsHorizontalScrollView::UpdateScroll, IOnGlobalLayoutListener)

ECode RecentsHorizontalScrollView::UpdateScroll::OnGlobalLayout()
{
    mHost->mLastScrollPosition = mHost->ScrollPositionOfMostRecent();
    mHost->ScrollTo(mHost->mLastScrollPosition, 0);
    AutoPtr<IViewTreeObserver> observer = mHost->GetViewTreeObserver();
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
CAR_INTERFACE_IMPL(RecentsHorizontalScrollView::SizeChangedRunnable, IRunnable)

ECode RecentsHorizontalScrollView::SizeChangedRunnable::Run()
{
    // Make sure we're still not clobbering the transition-set values, since this
    // runnable launches asynchronously
    AutoPtr<ILayoutTransition> transition;
    mHost->mLinearLayout->GetLayoutTransition((ILayoutTransition**)&transition);
    Boolean b = FALSE;
    if (transition == NULL || (transition->IsRunning(&b), !b)) {
        mHost->ScrollTo(mHost->mLastScrollPosition, 0);
    }
    return NOERROR;
}


//============================================================================================
// RecentsHorizontalScrollView::AdapterDataSetObserver
//============================================================================================
CAR_INTERFACE_IMPL(RecentsHorizontalScrollView::AdapterDataSetObserver, IDataSetObserver)

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

RecentsHorizontalScrollView::RecentsHorizontalScrollView()
    : mLastScrollPosition(0)
    , mCallback(NULL)
    , mNumItemsInOneScreenful(0)
{}

ECode RecentsHorizontalScrollView::Init(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(HorizontalScrollView::Init(ctx, attrs, 0));
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
    CSwipeHelper::New(ISwipeHelper::Y, this, densityScale, pagingTouchSlop, (ISwipeHelper**)&mSwipeHelper);
    mPerformanceHelper = RecentsScrollViewPerformanceHelper::Create(ctx, attrs, this, FALSE);
    return NOERROR;
}

ECode RecentsHorizontalScrollView::SetMinSwipeAlpha(
    /* [in] */ Float minAlpha)
{
    mSwipeHelper->SetMinAlpha(minAlpha);
    return NOERROR;
}

Int32 RecentsHorizontalScrollView::ScrollPositionOfMostRecent()
{
    Int32 h1, h2;
    mLinearLayout->GetWidth(&h1);
    h2 = GetWidth();
    return h1 - h2;
}

void RecentsHorizontalScrollView::AddToRecycledViews(
    /* [in] */ IView* v)
{
    if (mRecycledViews.GetSize() < mNumItemsInOneScreenful) {
        mRecycledViews.Insert(v);
    }
}

ECode RecentsHorizontalScrollView::FindViewForTask(
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

        AutoPtr<LaunchAppListener> launchAppListener = new LaunchAppListener(view, this);

        AutoPtr<IInterface> tag;
        view->GetTag((IInterface**)&tag);
        AutoPtr<RecentsPanelView::ViewHolder> holder = (RecentsPanelView::ViewHolder*)tag.Get();
        AutoPtr<IView> thumbnailView = holder->mThumbnailView;
        AutoPtr<LongClickListener> longClickListener = new LongClickListener(view, thumbnailView, this);
        thumbnailView->SetClickable(TRUE);
        thumbnailView->SetOnClickListener(launchAppListener);
        thumbnailView->SetOnLongClickListener(longClickListener);

        // We don't want to dismiss recents if a user clicks on the app title
        // (we also don't want to launch the app either, though, because the
        // app title is a small target and doesn't have great click feedback)
        AutoPtr<IView> appTitle;
        view->FindViewById(SystemUIR::id::app_label, (IView**)&appTitle);
        AutoPtr<ICharSequence> cs;
        CString::New(String(""), (ICharSequence**)&cs);
        appTitle->SetContentDescription(cs);
        appTitle->SetOnTouchListener(noOpListener);
        mLinearLayout->AddView(view);
    }
    SetLayoutTransition(transitioner);

    // Scroll to end after initial layout.
    AutoPtr<UpdateScroll> updateScroll = new UpdateScroll(this);
    GetViewTreeObserver()->AddOnGlobalLayoutListener(updateScroll);
}

ECode RecentsHorizontalScrollView::RemoveViewInLayout(
    /* [in] */ IView* view)
{
    return DismissChild(view);
}

Boolean RecentsHorizontalScrollView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    // if (DEBUG) Log.v(TAG, "onInterceptTouchEvent()");
    Boolean b1, b2;
    mSwipeHelper->OnInterceptTouchEvent(ev, &b1);
    b2 = HorizontalScrollView::OnInterceptTouchEvent(ev);
    return b1 || b2;
}

Boolean RecentsHorizontalScrollView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    Boolean b1, b2;
    mSwipeHelper->OnTouchEvent(ev, &b1);
    b2 = HorizontalScrollView::OnTouchEvent(ev);
    return b1 || b2;
}

Boolean RecentsHorizontalScrollView::CanChildBeDismissed(
    /* [in] */ IView* v)
{
    return TRUE;
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

ECode RecentsHorizontalScrollView::GetChildAtPosition(
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
    return v->FindViewById(SystemUIR::id::recent_item, view);
}

ECode RecentsHorizontalScrollView::Draw(
    /* [in] */ ICanvas* canvas)
{
    HorizontalScrollView::Draw(canvas);

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
                0, 0,
                GetLeftFadingEdgeStrength(), GetRightFadingEdgeStrength());
    }
    return NOERROR;
}

Int32 RecentsHorizontalScrollView::GetVerticalFadingEdgeLength()
{
    if (mPerformanceHelper != NULL) {
        return mPerformanceHelper->GetVerticalFadingEdgeLengthCallback();
    }
    else {
        return HorizontalScrollView::GetVerticalFadingEdgeLength();
    }
}

Int32 RecentsHorizontalScrollView::GetHorizontalFadingEdgeLength()
{
    if (mPerformanceHelper != NULL) {
        return mPerformanceHelper->GetHorizontalFadingEdgeLengthCallback();
    }
    else {
        return HorizontalScrollView::GetHorizontalFadingEdgeLength();
    }
}

ECode RecentsHorizontalScrollView::OnFinishInflate()
{
    HorizontalScrollView::OnFinishInflate();
    SetScrollbarFadingEnabled(TRUE);
    AutoPtr<IView> v = FindViewById(SystemUIR::id::recents_linear_layout);
    mLinearLayout = ILinearLayout::Probe(v);
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 leftPadding;
    res->GetDimensionPixelOffset(SystemUIR::dimen::status_bar_recents_thumbnail_left_margin, &leftPadding);
    SetOverScrollEffectPadding(leftPadding, 0);
    return NOERROR;
}

ECode RecentsHorizontalScrollView::OnAttachedToWindow()
{
    if (mPerformanceHelper != NULL) {
        mPerformanceHelper->OnAttachedToWindowCallback(
                mCallback, mLinearLayout, IsHardwareAccelerated());
    }
    return NOERROR;
}

void RecentsHorizontalScrollView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    HorizontalScrollView::OnConfigurationChanged(newConfig);
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
    HorizontalScrollView::OnSizeChanged(w, h, oldw, oldh);

    // Skip this work if a transition is running; it sets the scroll values independently
    // and should not have those animated values clobbered by this logic
    AutoPtr<ILayoutTransition> transition;
    mLinearLayout->GetLayoutTransition((ILayoutTransition**)&transition);
    Boolean b = FALSE;
    if (transition != NULL && (transition->IsRunning(&b), b)) {
        return;
    }
    // Keep track of the last visible item in the list so we can restore it
    // to the bottom when the orientation changes.
    mLastScrollPosition = ScrollPositionOfMostRecent();

    AutoPtr<SizeChangedRunnable> run = new SizeChangedRunnable(this);
    Post(run);
}

ECode RecentsHorizontalScrollView::SetAdapter(
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
    Int32 width;
    child->GetMeasuredWidth(&width);
    mNumItemsInOneScreenful = (Int32)Elastos::Core::Math::Ceil(widthPixels / (Float)width); //TODO: FloatMath
    AddToRecycledViews(child);

    for (Int32 i = 0; i < mNumItemsInOneScreenful - 1; i++) {
        AutoPtr<IView> child;
        mAdapter->CreateView(mLinearLayout, (IView**)&child);
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
    return mLinearLayout->SetLayoutTransition(transition);
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
