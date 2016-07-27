
#include "elastos/droid/systemui/keyguard/CKeyguardWidgetPager.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CKeyguardWidgetPager::MyRunnable::Run()
{
    return mHost->mLockPatternUtils->RemoveAppWidget(mAppWidgetId);
}

ECode CKeyguardWidgetPager::MyRunnable2::Run()
{
    return->mLockPatternUtils->AddAppWidget(mAppWidgetId, mIndex - mPagesRange);
}

CAR_INTERFACE_IMPL(CKeyguardWidgetPager::ZInterpolator, Object, ITimeInterpolator)

CKeyguardWidgetPager::ZInterpolator::ZInterpolator(
    /* [in] */ Float foc)
{
    mFocalLength = foc;
}

ECode CKeyguardWidgetPager::ZInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* polation)
{
    VALIDATE_NOT_NULL(polation)

    return (1.0f - mFocalLength / (mFocalLength + input)) /
            (1.0f - mFocalLength / (mFocalLength + 1.0f));
}

ECode CKeyguardWidgetPager::ZInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    *res = FALSE;
    return NOERROR;
}

ECode CKeyguardWidgetPager::MyAnimatorListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    if (mShow) {
        mHost->EnablePageContentLayers();
    }
    return NOERROR;
}

ECode CKeyguardWidgetPager::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (!show) {
        mHost->DisablePageContentLayers();
        AutoPtr<IKeyguardWidgetFrame> frame;
        mHost->GetWidgetPageAt(mHost->mWidgetToResetAfterFadeOut, (IKeyguardWidgetFrame**)&frame);
        AutoPtr<IKeyguardWidgetFrame> currentPage;
        getWidgetPageAt(mHost->mCurrentPage, (IKeyguardWidgetFrame**)&currentPage);
        Boolean res;
        if (frame != NULL && !(TO_IINTERFACE(frame) == TO_IINTERFACE(currentPage) &&
                (mHost->mViewStateManager->IsChallengeOverlapping(&res), res))) {
            frame->ResetSize();
        }
        mHost->mWidgetToResetAfterFadeOut = -1;
        mHost->mShowingInitialHints = FALSE;
    }
    return mHost->UpdateWidgetFramesImportantForAccessibility();
}

Float CKeyguardWidgetPager::OVERSCROLL_MAX_ROTATION = 30;

Float CKeyguardWidgetPager::CAMERA_DISTANCE = 10000;
const Boolean CKeyguardWidgetPager::PERFORM_OVERSCROLL_ROTATION = TRUE;
const Int32 CKeyguardWidgetPager::FLAG_HAS_LOCAL_HOUR = 0x1;
const Int32 CKeyguardWidgetPager::FLAG_HAS_LOCAL_MINUTE = 0x2;

const Int64 CKeyguardWidgetPager::CUSTOM_WIDGET_USER_ACTIVITY_TIMEOUT = 30000;
const String CKeyguardWidgetPager::TAG("KeyguardWidgetPager");

CAR_OBJECT_IMPL(CKeyguardWidgetPager)

CAR_INTERFACE_IMPL_4(CKeyguardWidgetPager, PagedView, IKeyguardWidgetPager, IPagedViewPageSwitchListener
        , IOnLongClickListener, IChallengeLayoutOnBouncerStateChangedListener)

CKeyguardWidgetPager::CKeyguardWidgetPager()
    : mShowHintsAfterLayout(FALSE)
    , mScreenCenter(0)
    , mShowingInitialHints(FALSE)
    , mHasMeasure(FALSE)
    , mCenterSmallWidgetsVertically(FALSE)
    , mPage(0)
    , mWidgetToResetAfterFadeOut(0)
    , mLastWidthMeasureSpec(0)
    , mLastHeightMeasureSpec(0)
    , mBouncerZoomInOutDuration(250)
    , BOUNCER_SCALE_FACTOR(0.67f)
    , mCameraEventInProgress(FALSE)
{
    mZInterpolator = new ZInterpolator(0.5f);
}

ECode CKeyguardWidgetPager::constructor(
    /* [in] */ IContext* context)
{
    return constructor(NULL, NULL, 0);
}

ECode CKeyguardWidgetPager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CKeyguardWidgetPager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    PagedView::constructor(context, attrs, defStyle);
    Int32 accessibility;
    GetImportantForAccessibility(&accessibility);
    if (accessibility == IView::IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
        SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
    }

    SetPageSwitchListener(this);

    CHandlerThread::New(String("KeyguardWidgetPager Worker"), (IHandlerThread**)&mBackgroundWorkerThread);
    mBackgroundWorkerThread->Start();
    AutoPtr<ILooper> looper;
    mBackgroundWorkerThread->GetLooper((ILooper**)&looper);
    CHandler::New(looper, (IHandler**)&mBackgroundWorkerHandler);
    return NOERROR;
}

ECode CKeyguardWidgetPager::OnDetachedFromWindow()
{
    PagedView>>OnDetachedFromWindow();

    // Clean up the worker thread
    return mBackgroundWorkerThread->Quit();
}

ECode CKeyguardWidgetPager::SetViewStateManager(
    /* [in] */ IKeyguardViewStateManager* viewStateManager)
{
    mViewStateManager = viewStateManager;
    return NOERROR;
}

ECode CKeyguardWidgetPager::SetLockPatternUtils(
    /* [in] */ ILockPatternUtils* l)
{
    mLockPatternUtils = l;
    return NOERROR;
}

ECode CKeyguardWidgetPager::OnPageSwitching(
    /* [in] */ IView* newPage,
    /* [in] */ Int32 newPageIndex)
{
    if (mViewStateManager != NULL) {
        mViewStateManager->OnPageSwitching(newPage, newPageIndex);
    }
    return NOERROR;
}

ECode CKeyguardWidgetPager::OnPageSwitched(
    /* [in] */ IView* newPage,
    /* [in] */ Int32 newPageIndex)
{
    Boolean showingClock = FALSE;
    if (IViewGroup::Probe(newPage) != NULL) {
        AutoPtr<IViewGroup> vg = IViewGroup::Probe(newPage);
        AutoPtr<IView> view;
        vg->GetChildAt(0, (IView**)&view);
        if (IKeyguardStatusView::Probe(view) != NULL) {
            showingClock = TRUE;
        }
    }

    if (newPage != NULL &&
            FindClockInHierarchy(newPage) == (FLAG_HAS_LOCAL_HOUR | FLAG_HAS_LOCAL_MINUTE)) {
        showingClock = TRUE;
    }

    // Disable the status bar clock if we're showing the default status widget
    if (showingClock) {
        SetSystemUiVisibility(GetSystemUiVisibility() | IView::STATUS_BAR_DISABLE_CLOCK);
    }
    else {
        SetSystemUiVisibility(GetSystemUiVisibility() & ~IView::STATUS_BAR_DISABLE_CLOCK);
    }

    // Extend the display timeout if the user switches pages
    if (mPage != newPageIndex) {
        Int32 oldPageIndex = mPage;
        mPage = newPageIndex;
        UserActivity();
        AutoPtr<IKeyguardWidgetFrame> oldWidgetPage;
        GetWidgetPageAt(oldPageIndex, (IKeyguardWidgetFrame**)&oldWidgetPage);
        if (oldWidgetPage != NULL) {
            oldWidgetPage->OnActive(FALSE);
        }
        AutoPtr<IKeyguardWidgetFrame> newWidgetPage;
        GetWidgetPageAt(newPageIndex, (IKeyguardWidgetFrame**)&newWidgetPage);
        if (newWidgetPage != NULL) {
            newWidgetPage->OnActive(TRUE);
            newWidgetPage->SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
            newWidgetPage->RequestAccessibilityFocus();
        }
        Boolean res;
        if (mParent != NULL && (AccessibilityManager::GetInstance(mContext)->IsEnabled(&res), res)) {
            AutoPtr<IAccessibilityEventHelper> helper;
            CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&helper);
            AutoPtr<IAccessibilityEvent> event;
            helper->Obtain(IAccessibilityEvent::TYPE_VIEW_SCROLLED, (IAccessibilityEvent**)&event);
            OnInitializeAccessibilityEvent(event);
            OnPopulateAccessibilityEvent(event);
            mParent->RequestSendAccessibilityEvent(this, event);
        }
    }
    if (mViewStateManager != NULL) {
        mViewStateManager->OnPageSwitched(newPage, newPageIndex);
    }
    return NOERROR;
}

ECode CKeyguardWidgetPager::SnPageBeginWarp()
{
    ShowOutlinesAndSidePages();
    return mViewStateManager->OnPageBeginWarp();
}

ECode CKeyguardWidgetPager::SnPageEndWarp()
{
    // if we're moving to the warp page, then immediately hide the other widgets.
    Int32 index;
    GetPageWarpIndex(&index);
    Int32 page;
    GetNextPage(&page);
    Int32 duration = index == page ? 0 : -1;
    AnimateOutlinesAndSidePages(FALSE, duration);
    return mViewStateManager->OnPageEndWarp();
}

ECode CKeyguardWidgetPager::SendAccessibilityEvent(
    /* [in] */ Int32  eventType)
{
    Boolean res;
    if (eventType != IAccessibilityEvent::TYPE_VIEW_SCROLLED || (IsPageMoving(&res), res)) {
        PagedView::SendAccessibilityEvent(eventType);
    }
    return NOERROR;
}

void CKeyguardWidgetPager::UpdateWidgetFramesImportantForAccessibility()
{
    Int32 pageCount;
    GetPageCount(&pageCount);
    for (Int32 i = 0; i < pageCount; i++) {
        AutoPtr<IKeyguardWidgetFrame> frame;
        GetWidgetPageAt(i, (IKeyguardWidgetFrame**)&frame);
        UpdateWidgetFrameImportantForAccessibility(frame);
    }
}

void CKeyguardWidgetPager::UpdateWidgetFrameImportantForAccessibility(
    /* [in] */ IKeyguardWidgetFrame* frame)
{
    Int32 _alpha;
    frame->GetContentAlpha(&_alpha);
    if (_alpha <= 0) {
        frame->SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_NO);
    } else {
        frame->SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
    }
}

void CKeyguardWidgetPager::UserActivity()
{
    if (mCallbacks != NULL) {
        mCallbacks->OnUserActivityTimeoutChanged();
        mCallbacks->UserActivity();
    }
}

ECode CKeyguardWidgetPager::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    *result = CaptureUserInteraction(ev) || (PagedView::OnTouchEvent(ev, &res), res);
    return NOERROR;
}

ECode CKeyguardWidgetPager::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    *result = CaptureUserInteraction(ev) || (PagedView::OnInterceptTouchEvent(ev, &res), res);
    return NOERROR;
}

Boolean CKeyguardWidgetPager::CaptureUserInteraction(
    /* [in] */ IMotionEvent* ev)
{
    Int32 page;
    GetCurrentPage(&page);
    AutoPtr<IKeyguardWidgetFrame> currentWidgetPage;
    GetWidgetPageAt(page, (IKeyguardWidgetFrame**)&currentWidgetPage);
    Boolean res;
    return currentWidgetPage != NULL && (currentWidgetPage->OnUserInteraction(ev, &res), res);
}

ECode CKeyguardWidgetPager::ShowPagingFeedback()
{
    // Nothing yet.
    return NOERROR;
}

ECode CKeyguardWidgetPager::GetUserActivityTimeout(
    /* [out] */ Int64* timetout)
{
    VALIDATE_NOT_NULL(timetout)

    AutoPtr<IView> page;
    GetPageAt(mPage, (IView**)&page);
    if (IViewGroup::Probe(page) != NULL) {
        AutoPtr<IViewGroup> vg = IViewGroup::Probe(page);
        AutoPtr<IView> view;
        vg->GetChildAt(0, (IView**)&view);
        if ((IKeyguardStatusView::Probe(view) == NULL)
                && (IKeyguardMultiUserSelectorView::Probe(view) == NULL)) {
            *timetout = CUSTOM_WIDGET_USER_ACTIVITY_TIMEOUT;
            return NOERROR;
        }
    }
    *timetout = -1;
    return NOERROR;
}

ECode CKeyguardWidgetPager::SetCallbacks(
    /* [in] */ IKeyguardWidgetPageCallbacks* callbacks)
{
    mCallbacks = callbacks;
    return NOERROR;
}

ECode CKeyguardWidgetPager::AddWidget(
    /* [in] */ IView* widget)
{
    return AddWidget(widget, -1);
}

ECode CKeyguardWidgetPager::OnRemoveView(
    /* [in] */ IView* v,
    /* [in] */ Boolean deletePermanently)
{
    Int32 appWidgetId;
    IKeyguardWidgetFrame::Probe(v)->GetContentAppWidgetId(&appWidgetId);
    if (mCallbacks != NULL) {
        mCallbacks->OnRemoveView(v, deletePermanently);
    }
    AutoPtr<IRunnable> r = new MyRunnable(this, appWidgetId);
    return mBackgroundWorkerHandler->Post(r);
}

ECode CKeyguardWidgetPager::OnRemoveViewAnimationCompleted()
{
    if (mCallbacks != NULL) {
        mCallbacks->OnRemoveViewAnimationCompleted();
    }
    return NOERROR;
}

ECode CKeyguardWidgetPager::OnAddView(
    /* [in] */ IView* v,
    /* [in] */ Int32 index)
{
    Int32 appWidgetId;
    IKeyguardWidgetFrame::Probe(v)->GetContentAppWidgetId(&appWidgetId);
    AutoPtr<ArrarOf<Int32> > pagesRange = ArrarOf<Int32>::Alloc(mTempVisiblePagesRange->GetLength());
    GetVisiblePages(pagesRange);
    BoundByReorderablePages(TRUE, pagesRange);
    if (mCallbacks != NULL) {
        mCallbacks->OnAddView(v);
    }
    // Subtract from the index to take into account pages before the reorderable
    // pages (e.g. the "add widget" page)
    AutoPtr<IRunnable> r = new MyRunnable2(this, appWidgetId, index, (*pagesRange)[0]);
    return mBackgroundWorkerHandler->Post(r);
}

ECode CKeyguardWidgetPager::AddWidget(
    /* [in] */ IView* widget,
    /* [in] */ Int32 pageIndex)
{
    AutoPtr<IKeyguardWidgetFrame> frame;
    // All views contained herein should be wrapped in a KeyguardWidgetFrame
    if (IKeyguardWidgetFrame::Proeb(widget) == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        CKeyguardWidgetFrame::New(context, (IKeyguardWidgetFrame**)&frame);
        AutoPtr<IFrameLayoutLayoutParams> lp;
        CFrameLayoutLayoutParams::New(IFrameLayoutLayoutParams::MATCH_PARENT,
                IFrameLayoutLayoutParams::MATCH_PARENT, (IFrameLayoutLayoutParams**)&lp);
        lp->SetGravity(IGravity::TOP);

        // The framework adds a default padding to AppWidgetHostView. We don't need this padding
        // for the Keyguard, so we override it to be 0.
        widget->SetPadding(0,  0, 0, 0);
        frame->AddView(widget, lp);

        // We set whether or not this widget supports vertical resizing.
        if (IAppWidgetHostView::Probe(widget) != NULL) {
            AutoPtr<IAppWidgetHostView> awhv = IAppWidgetHostView::Probe(widget);
            AutoPtr<IAppWidgetProviderInfo> info;
            awhv->GetAppWidgetInfo((IAppWidgetProviderInfo**)&info);

            Int32 mode;
            info->GetResizeMode(&mode);
            if (mode & IAppWidgetProviderInfo::RESIZE_VERTICAL) != 0) {
                frame->SetWidgetLockedSmall(FALSE);
            }
            else {
                // Lock the widget to be small.
                frame->SetWidgetLockedSmall(TRUE);
                if (mCenterSmallWidgetsVertically) {
                    lp->SetGravity(IGravity::CENTER);
                }
            }
        }
    }
    else {
        frame = IKeyguardWidgetFrame::Probe(widget);
    }

    AutoPtr<IViewGroupLayoutParams> pageLp;
    CViewGroupLayoutParams::New(
            IViewGroupLayoutParams::MATCH_PARENT, IViewGroupLayoutParams::MATCH_PARENT,
            (IViewGroupLayoutParams**)&pageLp);
    frame->SetOnLongClickListener(this);
    frame->SetWorkerHandler(mBackgroundWorkerHandler);

    if (pageIndex == -1) {
        AddView(frame, pageLp);
    }
    else {
        AddView(frame, pageIndex, pageLp);
    }

    // Update the frame content description.
    AutoPtr<IView> content;
    if (TO_IINTERFACE(widget) == TO_IINTERFACE(frame)) {
        frame->GetContent((IView**)&content);
    }
    else {
        content = widget;
    }
    if (content != NULL) {
        AutoPtr<ICharSequence> cchar;
        content->GetContentDescription((ICharSequence**)&cchar)
        String contentDescription;
        mContext->GetString(
            R::string::keyguard_accessibility_widget,
            cchar, &contentDescription);
        frame->SetContentDescription(contentDescription);
    }
    return UpdateWidgetFrameImportantForAccessibility(frame);
}

ECode CKeyguardWidgetPager::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index)
{
    FAIL_RETURN(EnforceKeyguardWidgetFrame(child))
    return PagedView::AddView(child, index);
}

ECode CKeyguardWidgetPager::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    FAIL_RETURN(EnforceKeyguardWidgetFrame(child))
    return PagedView::AddView(child, width, height);
}

ECode CKeyguardWidgetPager::AddView(
    /* [in] */ IView* child,
    /* [in] */ IViewGroupLayoutParams* params)
{
    FAIL_RETURN(EnforceKeyguardWidgetFrame(child))
    return PagedView::AddView(child, params);
}

ECode CKeyguardWidgetPager::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index,
    /* [in] */ IViewGroupLayoutParams* params)
{
    FAIL_RETURN(EnforceKeyguardWidgetFrame(child))
    return PagedView::AddView(child, index, params);
}

ECode CKeyguardWidgetPager::EnforceKeyguardWidgetFrame(
    /* [in] */ IView* child)
{
    if (IKeyguardWidgetFrame::Probe(child) == NULL) {
        // throw new IllegalArgumentException(
        //         "KeyguardWidgetPager children must be KeyguardWidgetFrames");
        return IllegalArgumentException;
    }
    return NOERROR;
}

ECode CKeyguardWidgetPager::GetWidgetPageAt(
    /* [in] */ Int32 index,
    /* [out] */ IKeyguardWidgetFrame** frame)
{
    VALIDATE_NOT_NULL(frame)

    // This is always a valid cast as we've guarded the ability to
    AutoPtr<IView> view;
    GetChildAt(index, (IView**)&view);
    *frame = IKeyguardWidgetFrame::Probe(view);
    REFCOUNT_ADD(*frame)
    return NOERROR;
}

ECode CKeyguardWidgetPager::OnUnhandledTap(
    /* [in] */ IMotionEvent* ev)
{
    return ShowPagingFeedback();
}

ECode CKeyguardWidgetPager::OnPageBeginMoving()
{
    if (mViewStateManager != NULL) {
        mViewStateManager->OnPageBeginMoving();
    }
    if (!IsReordering(FALSE)) {
        ShowOutlinesAndSidePages();
    }
    UserActivity();
    return NOERROR
}

ECode CKeyguardWidgetPager::OnPageEndMoving()
{
    if (mViewStateManager != NULL) {
        mViewStateManager->OnPageEndMoving();
    }

    // In the reordering case, the pages will be faded appropriately on completion
    // of the zoom in animation.
    if (!IsReordering(FALSE)) {
        HideOutlinesAndSidePages();
    }
    return NOERROR;
}

ECode CKeyguardWidgetPager::EnablePageContentLayers()
{
    Int32 children;
    GetChildCount(&children);
    for (Int32 i = 0; i < children; i++) {
        AutoPtr<IKeyguardWidgetFrame> frame;
        GetWidgetPageAt(i, (IKeyguardWidgetFrame**)&frame);
        frame->EnableHardwareLayersForContent();
    }
    return NOERROR;
}

ECode CKeyguardWidgetPager::DisablePageContentLayers()
{
    Int32 children;
    GetChildCount(&children);
    for (Int32 i = 0; i < children; i++) {
        AutoPtr<IKeyguardWidgetFrame> frame;
        GetWidgetPageAt(i, (IKeyguardWidgetFrame**)&frame);
        frame->DisableHardwareLayersForContent();
    }
    return NOERROR;
}

ECode CKeyguardWidgetPager::OverScroll(
    /* [in] */ Float amount)
{
    return AcceleratedOverScroll(amount);
}

ECode CKeyguardWidgetPager::BackgroundAlphaInterpolator(
    /* [in] */ Float r,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    *result = Math::Min(1f, r);
    return NOERROR;
}

void CKeyguardWidgetPager::UpdatePageAlphaValues(
    /* [in] */ Int32 screenCenter)
{
    return;
}

ECode CKeyguardWidgetPager::GetAlphaForPage(
    /* [in] */ Int32 screenCenter,
    /* [in] */ Int32 index,
    /* [in] */ Boolean showSidePages,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    if (IsWarping(&res), res) {
        Int32 _index;
        GetPageWarpIndex(&_index);
        *result = index == _index() ? 1.0f : 0.0f;
        return NOERROR;
    }
    if (showSidePages) {
        *result = 1.0f;
        return NOERROR;
    }
    else {
        *result = index == mCurrentPage ? 1.0f : 0f;
        return NOERROR;
    }
}

ECode CKeyguardWidgetPager::GetOutlineAlphaForPage(
    /* [in] */ Int32 screenCenter,
    /* [in] */ Int32 index,
    /* [in] */ Boolean showSidePages,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    if (showSidePages) {
        Float _result;
        GetAlphaForPage(screenCenter, index, showSidePages, &_result);
        *result = _result * IKeyguardWidgetFrame::OUTLINE_ALPHA_MULTIPLIER;
        return NOERROR;
    }
    else {
        *result = 0.0f;
        return NOERROR;
    }
}

ECode CKeyguardWidgetPager::IsOverScrollChild(
    /* [in] */ Int32 index,
    /* [in] */ Float scrollProgress,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isInOverscroll = mOverScrollX < 0 || mOverScrollX > mMaxScrollX;
    Int32 count;
    *result = (isInOverscroll && (index == 0 && scrollProgress < 0 ||
            index == (GetChildCount(&count), count) - 1 && scrollProgress > 0));
    return NOERROR;
}

ECode CKeyguardWidgetPager::ScreenScrolled(
    /* [in] */ Int32 screenCenter)
{
    mScreenCenter = screenCenter;
    UpdatePageAlphaValues(screenCenter);

    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IKeyguardWidgetFrame> v;
        GetWidgetPageAt(i, (IKeyguardWidgetFrame**)&v);
        if (TO_IINTERFACE(v) == TO_IINTERFACE(mDragView)) continue;
        if (v != NULL) {
            Float scrollProgress;
            GetScrollProgress(screenCenter, v, i, &scrollProgress);

            v->SetCameraDistance(mDensity * CAMERA_DISTANCE);

            Boolean res;
            IsOverScrollChild(i, scrollProgress, &res);
            if (res && PERFORM_OVERSCROLL_ROTATION) {
                Int32 width;
                IView::Probe(v)->GetMeasuredWidth(&width);
                Float pivotX = width / 2;
                Int32 height;
                IView::Probe(v)->GetMeasuredHeight(&height);
                Float pivotY = height / 2;
                v->SetPivotX(pivotX);
                v->SetPivotY(pivotY);
                v->SetRotationY(- OVERSCROLL_MAX_ROTATION * scrollProgress);
                v->SetOverScrollAmount(Math::Abs(scrollProgress), scrollProgress < 0);
            }
            else {
                v->SetRotationY(0.0f);
                v->SetOverScrollAmount(0, FALSE);
            }

            Float alpha;
            v->GetAlpha(&alpha);
            // If the view has 0 alpha, we set it to be invisible so as to prevent
            // it from accepting touches
            Int32 visibility;
            if (alpha == 0) {
                v->SetVisibility(INVISIBLE);
            }
            else if ((v->GetVisibility(&visibility), visibility) != VISIBLE) {
                v->SetVisibility(VISIBLE);
            }
        }
    }
    return NOERROR;
}

ECode CKeyguardWidgetPager::IsWidgetPage(
    /* [in] */ Int32 pageIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 count;
    if (pageIndex < 0 || pageIndex >= (GetChildCount(&count), count)) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IView> v;
    GetChildAt(pageIndex, (IView**)&v);
    if (v != NULL && IKeyguardWidgetFrame::Probe(v) ! = NULL) {
        AutoPtr<IKeyguardWidgetFrame> kwf = IKeyguardWidgetFrame::Probe(v);
        Int32 id;
        kwf->GetContentAppWidgetId(&id);
        *result = id != IAppWidgetManager::INVALID_APPWIDGET_ID;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CKeyguardWidgetPager::BoundByReorderablePages(
    /* [in] */ Boolean isReordering,
    /* [in] */ ArrayOf<Int32>* range)
{
    if (isReordering) {
        // Remove non-widget pages from the range
        Boolean res;
        while ((*range)[1] >= (*range)[0] && (IsWidgetPage((*range)[1], &res), !res)) {
            (*range)[1]--;
        }
        while ((*range)[0] <= (*range)[1] && (IsWidgetPage((*range)[0], &res), !res)) {
            (*range)[0]++;
        }
    }
    return NOERROR;
}

ECode CKeyguardWidgetPager::ReorderStarting()
{
    return ShowOutlinesAndSidePages();
}

ECode CKeyguardWidgetPager::OnStartReordering()
{
    PagedView::OnStartReordering();
    EnablePageContentLayers();
    return ReorderStarting();
}

ECode CKeyguardWidgetPager::OnEndReordering()
{
    PagedView::OnEndReordering();
    return HideOutlinesAndSidePages();
}

ECode CKeyguardWidgetPager::ShowOutlinesAndSidePages()
{
    return AnimateOutlinesAndSidePages(TRUE);
}

ECode CKeyguardWidgetPager::HideOutlinesAndSidePages()
{
    return AnimateOutlinesAndSidePages(FALSE);
}

ECode CKeyguardWidgetPager::UpdateChildrenContentAlpha(
    /* [in] */ Float sidePageAlpha)
{
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IKeyguardWidgetFrame> child;
        GetWidgetPageAt(i, (IKeyguardWidgetFrame**)&child);
        if (i != mCurrentPage) {
            child->SetBackgroundAlpha(sidePageAlpha);
            child->SetContentAlpha(0.0f);
        }
        else {
            child->SetBackgroundAlpha(0.0f);
            child->SetContentAlpha(1.0f);
        }
    }
    return NOERROR;
}

ECode CKeyguardWidgetPager::ShowInitialPageHints()
{
    mShowingInitialHints = TRUE;
    return UpdateChildrenContentAlpha(IKeyguardWidgetFrame::OUTLINE_ALPHA_MULTIPLIER);
}

ECode CKeyguardWidgetPager::SetCurrentPage(
    /* [in] */ Int32 currentPage)
{
    PagedView::SetCurrentPage(currentPage);
    UpdateChildrenContentAlpha(0.0f);
    return UpdateWidgetFramesImportantForAccessibility();
}

ECode CKeyguardWidgetPager::OnAttachedToWindow()
{
    PagedView::OnAttachedToWindow();
    mHasMeasure = FALSE;
    return NOERROR;
}

ECode CKeyguardWidgetPager::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    mLastWidthMeasureSpec = widthMeasureSpec;
    mLastHeightMeasureSpec = heightMeasureSpec;

    Int32 maxChallengeTop = -1;
    AutoPtr<IViewParent> vp;
    GetParent((IViewParent**)&vp);
    AutoPtr<IView> parent = IView::Probe(vp);
    Boolean challengeShowing = FALSE;
    // Widget pages need to know where the top of the sliding challenge is so that they
    // now how big the widget should be when the challenge is up. We compute it here and
    // then propagate it to each of our children.
    AutoPtr<IViewParent> p;
    parent->GetParent((IViewParent**)&p);
    if (ISlidingChallengeLayout::Probe(p) != NULL) {
        AutoPtr<ISlidingChallengeLayout> scl = ISlidingChallengeLayout::Probe(p);
        Int32 top;
        scl->GetMaxChallengeTop(&top);

        // This is a bit evil, but we need to map a coordinate relative to the SCL into a
        // coordinate relative to our children, hence we subtract the top padding.s
        Int32 ptop;
        GetPaddingTop(&ptop);
        maxChallengeTop = top - ptop;
        scl->IsChallengeShowing(&challengeShowing);

        Int32 count;
        GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IKeyguardWidgetFrame> frame;
            GetWidgetPageAt(i, (IKeyguardWidgetFrame**)&frame);
            frame->SetMaxChallengeTop(maxChallengeTop);
            // On the very first measure pass, if the challenge is showing, we need to make sure
            // that the widget on the current page is small.
            if (challengeShowing && i == mCurrentPage && !mHasMeasure) {
                frame->ShrinkWidget(TRUE);
            }
        }
    }
    PagedView::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    mHasMeasure = TRUE;
    return NOERROR;
}

ECode CKeyguardWidgetPager::AnimateOutlinesAndSidePages(
    /* [in] */ Boolean show)
{
    return AnimateOutlinesAndSidePages(show, -1);
}

ECode CKeyguardWidgetPager::SetWidgetToResetOnPageFadeOut(
    /* [in] */ Int32 widget)
{
    mWidgetToResetAfterFadeOut = widget;
    return NOERROR;
}

ECode CKeyguardWidgetPager::GetWidgetToResetOnPageFadeOut(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mWidgetToResetAfterFadeOut;
    return NOERROR;
}

ECode CKeyguardWidgetPager::AnimateOutlinesAndSidePages(
    /* [in] */ Boolean show,
    /* [in] */ Int32 duration)
{
    if (mChildrenOutlineFadeAnimation != NULL) {
        mChildrenOutlineFadeAnimation->Cancel();
        mChildrenOutlineFadeAnimation = NULL;
    }
    Int32 count;
    GetChildCount(&count);
    AutoPtr<IPropertyValuesHolder> alpha;
    AutoPtr<IArrayList> anims;
    CArrayList::New((IArrayList**)&anims);

    if (duration == -1) {
        duration = show ? CHILDREN_OUTLINE_FADE_IN_DURATION :
            CHILDREN_OUTLINE_FADE_OUT_DURATION;
    }

    Int32 curPage;
    GetNextPage(&curPage);
    for (Int32 i = 0; i < count; i++) {
        Float finalContentAlpha;
        if (show) {
            GetAlphaForPage(mScreenCenter, i, TRUE, &finalContentAlpha);
        }
        else if (!show && i == curPage) {
            finalContentAlpha = 1.0f;
        }
        else {
            finalContentAlpha = 0.0f;
        }
        AutoPtr<IKeyguardWidgetFrame> child;
        GetWidgetPageAt(i, (IKeyguardWidgetFrame**)&child);

        AutoPtr<IPropertyValuesHolderHelper> helper;
        CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
        helper->OfFloat(String("contentAlpha"), finalContentAlpha, (IPropertyValuesHolder**)&alpha);

        AutoPtr<IObjectAnimatorHelper> helper2;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper2);
        AutoPtr<IObjectAnimator> a;
        helper2->OfPropertyValuesHolder(child, alpha, (IObjectAnimator**)&a);
        anims->Add(TO_IINTERFACE(a));

        Float finalOutlineAlpha = 0.0f;
        if (show) {
            GetOutlineAlphaForPage(mScreenCenter, i, TRUE, &finalOutlineAlpha)
        }
        child->FadeFrame(this, show, finalOutlineAlpha, duration);
    }

    CAnimatorSet::New((IAnimatorSet**)&mChildrenOutlineFadeAnimation);
    mChildrenOutlineFadeAnimation->PlayTogether(anims);

    mChildrenOutlineFadeAnimation->SetDuration(duration);
    AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter(this, show);
    mChildrenOutlineFadeAnimation->AddListener(lis);
    return mChildrenOutlineFadeAnimation->Start();
}

ECode CKeyguardWidgetPager::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Disallow long pressing to reorder if the challenge is showing
    Boolean res1, res2;
    Boolean isChallengeOverlapping = (mViewStateManager->IsChallengeShowing(&res1), res1) &&
            (mViewStateManager->IsChallengeOverlapping(&res2), res2);

    Boolean res;
    if (!isChallengeOverlapping && (StartReordering(&res), res)) {
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CKeyguardWidgetPager::RemoveWidget(
    /* [in] */ IView* view)
{
    if (IKeyguardWidgetFrame::Probe(view) != NULL) {
        RemoveView(view);
    }
    else {
        // Assume view was wrapped by a KeyguardWidgetFrame in KeyguardWidgetPager#addWidget().
        // This supports legacy hard-coded "widgets" like KeyguardTransportControlView.
        Int32 pos;
        GetWidgetPageIndex(view, &pos);
        if (pos != -1) {
            AutoPtr<IView> view;
            GetChildAt(pos, (IView**)&view);
            AutoPtr<IKeyguardWidgetFrame> frame = IKeyguardWidgetFrame::Probe(view);
            frame->RemoveView(view);
            RemoveView(frame);
        }
        else {
            Slogger::W(TAG, "removeWidget() can't find: %s", TO_CSTR(view));
        }
    }
    return NOERROR;
}

ECode CKeyguardWidgetPager::GetWidgetPageIndex(
    /* [in] */ IView* view,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    if (IKeyguardWidgetFrame::Probe(view) != NULL) {
        return IndexOfChild(view, index);
    }
    else {
        // View was wrapped by a KeyguardWidgetFrame by KeyguardWidgetPager#addWidget()
        AutoPtr<IViewParent> p;
        view->GetParent((IViewParent**)&p);
        return IndexOfChild(IKeyguardWidgetFrame::Probe(p), index);
    }
    return NOERROR;
}

ECode CKeyguardWidgetPager::SetPageHoveringOverDeleteDropTarget(
    /* [in] */ Int32 viewIndex,
    /* [in] */ Boolean isHovering)
{
    AutoPtr<IKeyguardWidgetFrame> child;
    GetWidgetPageAt(viewIndex, (IKeyguardWidgetFrame**)&child);
    return child->SetIsHoveringOverDeleteDropTarget(isHovering);
}

ECode CKeyguardWidgetPager::OnBouncerStateChanged(
    /* [in] */ Boolean bouncerActive)
{
    if (bouncerActive) {
        ZoomOutToBouncer();
    }
    else {
        ZoomInFromBouncer();
    }
    return NOERROR;
}

ECode CKeyguardWidgetPager::SetBouncerAnimationDuration(
    /* [in] */ Int32 duration)
{
    mBouncerZoomInOutDuration = duration;
    return NOERROR;
}

ECode CKeyguardWidgetPager::ZoomInFromBouncer()
{
    Boolean res;
    if (mZoomInOutAnim != NULL && (mZoomInOutAnim->IsRunning(&res), res)) {
        mZoomInOutAnim->Cancel();
    }
    Int32 page;
    GetCurrentPage(&page);
    AutoPtr<IView> currentPage;
    GetPageAt(page, (IView**)&currentPage);

    Float x, y;
    if ((currentPage->GetScaleX(&x), x) < 1.0f || (currentPage->GetScaleY(&y), y) < 1.0f) {
        CAnimatorSet::New((IAnimatorSet**)&mZoomInOutAnim);

        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<IObjectAnimator> animator1;
        helper->OfFloat(currentPage, String("scaleX"), 1.0f, (IObjectAnimator**)&animator1);
        AutoPtr<IObjectAnimator> animator2;
        helper->OfFloat(currentPage , String("scaleY"), 1.0f, (IObjectAnimator**)&animator2);

        mZoomInOutAnim->PlayTogether(animator1, animator2);
        mZoomInOutAnim->SetDuration(mBouncerZoomInOutDuration);
        AutoPtr<IDecelerateInterpolator> interpolator;
        CDecelerateInterpolator::New(1.5f, (IDecelerateInterpolator**)&interpolator);
        mZoomInOutAnim->SetInterpolator(interpolator);
        mZoomInOutAnim->Start();
    }
    if (IKeyguardWidgetFrame::Probe(currentPage) != NULL) {
        IKeyguardWidgetFrame::Probe(currentPage)->OnBouncerShowing(FALSE);
    }
    return NOERROR;
}

ECode CKeyguardWidgetPager::ZoomOutToBouncer()
{
    Boolean res;
    if (mZoomInOutAnim != NULL && (mZoomInOutAnim->IsRunning(&res), res)) {
        mZoomInOutAnim->Cancel();
    }
    Int32 curPage;
    GetCurrentPage(&curPage);
    AutoPtr<IView> currentPage;
    GetPageAt(curPage, (IView**)&currentPage);
    if (ShouldSetTopAlignedPivotForWidget(curPage)) {
        currentPage->SetPivotY(0);
        // Note: we are working around the issue that setting the x-pivot to the same value as it
        //       was does not actually work.
        currentPage->SetPivotX(0);
        Int32 width;
        currentPage->GetMeasuredWidth(&width);
        currentPage->SetPivotX(width / 2);
    }

    Float x, y;
    if (!((currentPage->GetScaleX(&x), x) < 1.0f || (currentPage->GetScaleY(&y), y) < 1.0f)) {
        mZoomInOutAnim = new CAnimatorSet::New((IAnimatorSet**)&mZoomInOutAnim);

        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<IObjectAnimator> animator1;
        helper->OfFloat(currentPage, String("scaleX"), BOUNCER_SCALE_FACTOR,
                (IObjectAnimator**)&animator1);
        AutoPtr<IObjectAnimator> animator2;
        helper->OfFloat(currentPage , String("scaleY"), BOUNCER_SCALE_FACTOR,
                (IObjectAnimator**)&animator2);

        mZoomInOutAnim->PlayTogether(animator1, animator2);
        mZoomInOutAnim->SetDuration(mBouncerZoomInOutDuration);
        AutoPtr<IDecelerateInterpolator> interpolator;
        CDecelerateInterpolator::New(1.5f, (IDecelerateInterpolator**)&interpolator);
        mZoomInOutAnim->SetInterpolator(interpolator);
        mZoomInOutAnim->Start();
    }
    if (IKeyguardWidgetFrame::Probe(currentPage) != NULL) {
        IKeyguardWidgetFrame::Probe(currentPage)->OnBouncerShowing(TRUE);
    }
    return NOERROR;
}

ECode CKeyguardWidgetPager::SetAddWidgetEnabled(
    /* [in] */ Boolean enabled)
{
    if (mAddWidgetView != NULL && enabled) {
        AddView(mAddWidgetView, 0);
        // We need to force measure the PagedView so that the calls to update the scroll
        // position below work
        Measure(mLastWidthMeasureSpec, mLastHeightMeasureSpec);
        // Bump up the current page to account for the addition of the new page
        SetCurrentPage(mCurrentPage + 1);
        mAddWidgetView = NULL;
    }
    else if (mAddWidgetView == NULL && !enabled) {
        AutoPtr<IView> addWidget;
        FindViewById(R::id::keyguard_add_widget, (IView**)&addWidget);
        if (addWidget != NULL) {
            mAddWidgetView = addWidget;
            RemoveView(addWidget);
        }
    }
    return NOERROR;
}

ECode CKeyguardWidgetPager::IsAddPage(
    /* [in] */ Int32 pageIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IView> v;
    GetChildAt(pageIndex, (IView**)&v);
    Int32 id;
    *result = v != NULL && (v->GetId(&id), id) == R::id::keyguard_add_widget;
    return NOERROR;
}

ECode CKeyguardWidgetPager::IsCameraPage(
    /* [in] */ Int32 pageIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IView> v;
    GetChildAt(pageIndex, (IView**)&v);
    *result = v != NULL && ICameraWidgetFrame::Probe(v) != NULL;
    return NOERROR;
}

ECode CKeyguardWidgetPager::ShouldSetTopAlignedPivotForWidget(
    /* [in] */ Int32 childIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    *result = !IsCameraPage(childIndex) &&
            (PagedView::ShouldSetTopAlignedPivotForWidget(childIndex, &res), res);
    return NOERROR;
}

Int32 CKeyguardWidgetPager::FindClockInHierarchy(
    /* [in] */ IView* view)
{
    if (ITextClock::Probe(view) != NULL) {
        return GetClockFlags(ITextClock::Probe(view));
    }
    else if (IViewGroup::Probe(view) != NULL) {
        Int32 flags = 0;
        AutoPtr<IViewGroup> group = IViewGroup::Probe(view);
        Int32 size;
        group->GetChildCount(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IView> v;
            group->GetChildAt(i, (IView**)&v);
            flags |= FindClockInHierarchy(v);
        }
        return flags;
    }
    else {
        return 0;
    }
}

Int32 CKeyguardWidgetPager::GetClockFlags(
    /* [in] */ ITextClock* clock)
{
    Int32 flags = 0;

    String timeZone;
    clock->GetTimeZone(&timeZone);
    if (timeZone != NULL) {

        AutoPtr<ITimeZoneHelper> helper;
        CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&helper);
        AutoPtr<ITimeZone> defaultzone;
        helper->GetDefault((ITimeZone**)&defaultzone);
        AutoPtr<ITimeZone> timezone;
        helper->GetTimeZone((ITimeZone**)&timezone);
        if(!IObject::Probe(defaultzone)->Equals(timezone)) {
            // Ignore clocks showing another timezone
            return 0;
        }
    }

    AutoPtr<ICharSequence> format;
    clock->GetFormat((ICharSequence**)&format);
    Char32 hour;
    Boolean res;
    if (clock->Is24HourModeEnabled(&res),res) {
        hour = IDateFormat::HOUR_OF_DAY;
    }
    else {
        hour = IDateFormat::HOUR;
    }

    AutoPtr<IDateFormatHelper> helper;
    CDateFormatHelper::AcquireSingleton((IDateFormatHelper**)&helper);
    if (helper->HasDesignator(format, hour, &res), res) {
        flags |= FLAG_HAS_LOCAL_HOUR;
    }
    if (helper->HasDesignator(format, IDateFormat::MINUTE, &res), res) {
        flags |= FLAG_HAS_LOCAL_MINUTE;
    }

    return flags;
}

ECode CKeyguardWidgetPager::HandleExternalCameraEvent(
    /* [in] */ IMotionEvent* event)
{
    BeginCameraEvent();
    Int cameraPage;
    GetPageCount(&cameraPage);
    cameraPage = cameraPage - 1;
    Boolean endWarp = FALSE;

    Boolean res;
    if ((IsCameraPage(cameraPage, &res), res) || mCameraEventInProgress) {
        Int32 action;
        event->GetAction(&action);
        switch (action) {
            case IMotionEvent::ACTION_DOWN:
                // Once we start dispatching camera events, we must continue to do so
                // to keep event dispatch happy.
                mCameraEventInProgress = TRUE;
                UserActivity();
                break;
            case IMotionEvent::ACTION_UP:
            case IMotionEvent::ACTION_CANCEL:
                mCameraEventInProgress = FALSE;
                break;
        }
        DispatchTouchEvent(event);
    }
    return EndCameraEvent();
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
