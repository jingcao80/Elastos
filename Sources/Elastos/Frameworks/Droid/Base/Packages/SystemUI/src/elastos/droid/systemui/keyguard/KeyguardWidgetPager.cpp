
#include "elastos/droid/systemui/keyguard/KeyguardWidgetPager.h"
#include "elastos/droid/systemui/keyguard/CKeyguardWidgetFrame.h"
#include "elastos/droid/systemui/keyguard/KeyguardWidgetFrame.h"
#include "elastos/droid/text/format/DateFormat.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Text.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <Elastos.CoreLibrary.Text.h>
#include "R.h"

using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::AppWidget::IAppWidgetHostView;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IPropertyValuesHolder;
using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::Animation::IPropertyValuesHolderHelper;
using Elastos::Droid::Animation::CPropertyValuesHolderHelper;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Text::Format::DateFormat;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;
using Elastos::Core::CoreUtils;
using Elastos::Core::IThread;
using Elastos::Core::Math;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL_2(KeyguardWidgetPager::InnerListener, Object,
    IPagedViewPageSwitchListener, IViewOnLongClickListener)

KeyguardWidgetPager::InnerListener::InnerListener(
    /* [in] */ KeyguardWidgetPager* host)
    : mHost(host)
{}

//@Override
ECode KeyguardWidgetPager::InnerListener::OnPageSwitching(
    /* [in] */ IView* newPage,
    /* [in] */ Int32 newPageIndex)
{
    mHost->OnPageSwitching(newPage, newPageIndex);
}

//@Override
ECode KeyguardWidgetPager::InnerListener::OnPageSwitched(
    /* [in] */ IView* newPage,
    /* [in] */ Int32 newPageIndex)
{
    mHost->OnPageSwitched(newPage, newPageIndex);
}

//@Override
ECode KeyguardWidgetPager::InnerListener::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    mHost->OnLongClick(v, result);
}

ECode KeyguardWidgetPager::MyRunnable::Run()
{
    Boolean res;
    return mHost->mLockPatternUtils->RemoveAppWidget(mAppWidgetId, &res);
}

ECode KeyguardWidgetPager::MyRunnable2::Run()
{
    Boolean res;
    return mHost->mLockPatternUtils->AddAppWidget(mAppWidgetId,
            mIndex - mPagesRange, &res);
}

CAR_INTERFACE_IMPL(KeyguardWidgetPager::ZInterpolator,
        Object, ITimeInterpolator)

KeyguardWidgetPager::ZInterpolator::ZInterpolator(
    /* [in] */ Float foc)
{
    mFocalLength = foc;
}

ECode KeyguardWidgetPager::ZInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* polation)
{
    VALIDATE_NOT_NULL(polation)

    return (1.0f - mFocalLength / (mFocalLength + input)) /
            (1.0f - mFocalLength / (mFocalLength + 1.0f));
}

ECode KeyguardWidgetPager::ZInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    *res = FALSE;
    return NOERROR;
}

ECode KeyguardWidgetPager::MyAnimatorListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    if (mShow) {
        mHost->EnablePageContentLayers();
    }
    return NOERROR;
}

ECode KeyguardWidgetPager::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (!mShow) {
        mHost->DisablePageContentLayers();
        AutoPtr<IKeyguardWidgetFrame> frame;
        mHost->GetWidgetPageAt(mHost->mWidgetToResetAfterFadeOut, (IKeyguardWidgetFrame**)&frame);
        AutoPtr<IKeyguardWidgetFrame> currentPage;
        mHost->GetWidgetPageAt(mHost->mCurrentPage, (IKeyguardWidgetFrame**)&currentPage);
        Boolean res;
        if (frame != NULL && !(TO_IINTERFACE(frame) == TO_IINTERFACE(currentPage) &&
                (mHost->mViewStateManager->IsChallengeOverlapping(&res), res))) {
            frame->ResetSize();
        }
        mHost->mWidgetToResetAfterFadeOut = -1;
        mHost->mShowingInitialHints = FALSE;
    }
    mHost->UpdateWidgetFramesImportantForAccessibility();
    return NOERROR;
}

Float KeyguardWidgetPager::OVERSCROLL_MAX_ROTATION = 30;

Float KeyguardWidgetPager::CAMERA_DISTANCE = 10000;
const Boolean KeyguardWidgetPager::PERFORM_OVERSCROLL_ROTATION = TRUE;
const Int32 KeyguardWidgetPager::FLAG_HAS_LOCAL_HOUR = 0x1;
const Int32 KeyguardWidgetPager::FLAG_HAS_LOCAL_MINUTE = 0x2;

const Int64 KeyguardWidgetPager::CUSTOM_WIDGET_USER_ACTIVITY_TIMEOUT = 30000;
const String KeyguardWidgetPager::TAG("KeyguardWidgetPager");

CAR_INTERFACE_IMPL_2(KeyguardWidgetPager, PagedView, IKeyguardWidgetPager,
    IChallengeLayoutOnBouncerStateChangedListener)

KeyguardWidgetPager::KeyguardWidgetPager()
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

ECode KeyguardWidgetPager::constructor(
    /* [in] */ IContext* context)
{
    return constructor(NULL, NULL, 0);
}

ECode KeyguardWidgetPager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode KeyguardWidgetPager::constructor(
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

    AutoPtr<InnerListener> listener = new InnerListener(this);
    SetPageSwitchListener(listener);

    CHandlerThread::New(String("KeyguardWidgetPager Worker"), (IHandlerThread**)&mBackgroundWorkerThread);
    IThread::Probe(mBackgroundWorkerThread)->Start();
    AutoPtr<ILooper> looper;
    mBackgroundWorkerThread->GetLooper((ILooper**)&looper);
    CHandler::New(looper, (IHandler**)&mBackgroundWorkerHandler);
    return NOERROR;
}

ECode KeyguardWidgetPager::OnDetachedFromWindow()
{
    PagedView::OnDetachedFromWindow();

    // Clean up the worker thread
    Boolean res;
    return mBackgroundWorkerThread->Quit(&res);
}

ECode KeyguardWidgetPager::SetViewStateManager(
    /* [in] */ IKeyguardViewStateManager* viewStateManager)
{
    mViewStateManager = viewStateManager;
    return NOERROR;
}

ECode KeyguardWidgetPager::SetLockPatternUtils(
    /* [in] */ ILockPatternUtils* l)
{
    mLockPatternUtils = l;
    return NOERROR;
}

ECode KeyguardWidgetPager::OnPageSwitching(
    /* [in] */ IView* newPage,
    /* [in] */ Int32 newPageIndex)
{
    if (mViewStateManager != NULL) {
        mViewStateManager->OnPageSwitching(newPage, newPageIndex);
    }
    return NOERROR;
}

ECode KeyguardWidgetPager::OnPageSwitched(
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
    Int32 visibility;
    GetSystemUiVisibility(&visibility);
    if (showingClock) {
        SetSystemUiVisibility(visibility | IView::STATUS_BAR_DISABLE_CLOCK);
    }
    else {
        SetSystemUiVisibility(visibility & ~IView::STATUS_BAR_DISABLE_CLOCK);
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
            IView::Probe(newWidgetPage)->SetImportantForAccessibility(
                    IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
            Boolean res;
            IView::Probe(newWidgetPage)->RequestAccessibilityFocus(&res);
        }
        AutoPtr<IAccessibilityManagerHelper> helper;
        CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&helper);
        AutoPtr<IAccessibilityManager> manager;
        helper->GetInstance(mContext, (IAccessibilityManager**)&manager);
        Boolean res;
        if (mParent != NULL && (manager->IsEnabled(&res), res)) {
            AutoPtr<IAccessibilityEventHelper> helper;
            CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&helper);
            AutoPtr<IAccessibilityEvent> event;
            helper->Obtain(IAccessibilityEvent::TYPE_VIEW_SCROLLED, (IAccessibilityEvent**)&event);
            OnInitializeAccessibilityEvent(event);
            OnPopulateAccessibilityEvent(event);
            mParent->RequestSendAccessibilityEvent(this, event, &res);
        }
    }
    if (mViewStateManager != NULL) {
        mViewStateManager->OnPageSwitched(newPage, newPageIndex);
    }
    return NOERROR;
}

ECode KeyguardWidgetPager::SnPageBeginWarp()
{
    ShowOutlinesAndSidePages();
    return mViewStateManager->OnPageBeginWarp();
}

ECode KeyguardWidgetPager::SnPageEndWarp()
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

ECode KeyguardWidgetPager::SendAccessibilityEvent(
    /* [in] */ Int32  eventType)
{
    Boolean res;
    if (eventType != IAccessibilityEvent::TYPE_VIEW_SCROLLED || (IsPageMoving(&res), res)) {
        PagedView::SendAccessibilityEvent(eventType);
    }
    return NOERROR;
}

void KeyguardWidgetPager::UpdateWidgetFramesImportantForAccessibility()
{
    Int32 pageCount;
    GetPageCount(&pageCount);
    for (Int32 i = 0; i < pageCount; i++) {
        AutoPtr<IKeyguardWidgetFrame> frame;
        GetWidgetPageAt(i, (IKeyguardWidgetFrame**)&frame);
        UpdateWidgetFrameImportantForAccessibility(frame);
    }
}

void KeyguardWidgetPager::UpdateWidgetFrameImportantForAccessibility(
    /* [in] */ IKeyguardWidgetFrame* frame)
{
    Float _alpha;
    frame->GetContentAlpha(&_alpha);
    if (_alpha <= 0) {
        IView::Probe(frame)->SetImportantForAccessibility(
                IView::IMPORTANT_FOR_ACCESSIBILITY_NO);
    }
    else {
        IView::Probe(frame)->SetImportantForAccessibility(
                IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
    }
}

void KeyguardWidgetPager::UserActivity()
{
    if (mCallbacks != NULL) {
        mCallbacks->OnUserActivityTimeoutChanged();
        mCallbacks->UserActivity();
    }
}

ECode KeyguardWidgetPager::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    *result = CaptureUserInteraction(ev) || (PagedView::OnTouchEvent(ev, &res), res);
    return NOERROR;
}

ECode KeyguardWidgetPager::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    *result = CaptureUserInteraction(ev) || (PagedView::OnInterceptTouchEvent(ev, &res), res);
    return NOERROR;
}

Boolean KeyguardWidgetPager::CaptureUserInteraction(
    /* [in] */ IMotionEvent* ev)
{
    Int32 page;
    GetCurrentPage(&page);
    AutoPtr<IKeyguardWidgetFrame> currentWidgetPage;
    GetWidgetPageAt(page, (IKeyguardWidgetFrame**)&currentWidgetPage);
    Boolean res;
    return currentWidgetPage != NULL && (currentWidgetPage->OnUserInteraction(ev, &res), res);
}

ECode KeyguardWidgetPager::ShowPagingFeedback()
{
    // Nothing yet.
    return NOERROR;
}

ECode KeyguardWidgetPager::GetUserActivityTimeout(
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

ECode KeyguardWidgetPager::SetCallbacks(
    /* [in] */ IKeyguardWidgetPagerCallbacks* callbacks)
{
    mCallbacks = callbacks;
    return NOERROR;
}

ECode KeyguardWidgetPager::AddWidget(
    /* [in] */ IView* widget)
{
    return AddWidget(widget, -1);
}

ECode KeyguardWidgetPager::OnRemoveView(
    /* [in] */ IView* v,
    /* [in] */ Boolean deletePermanently)
{
    Int32 appWidgetId;
    IKeyguardWidgetFrame::Probe(v)->GetContentAppWidgetId(&appWidgetId);
    if (mCallbacks != NULL) {
        mCallbacks->OnRemoveView(v, deletePermanently);
    }
    AutoPtr<IRunnable> r = new MyRunnable(this, appWidgetId);
    Boolean res;
    return mBackgroundWorkerHandler->Post(r, &res);
}

ECode KeyguardWidgetPager::OnRemoveViewAnimationCompleted()
{
    if (mCallbacks != NULL) {
        mCallbacks->OnRemoveViewAnimationCompleted();
    }
    return NOERROR;
}

ECode KeyguardWidgetPager::OnAddView(
    /* [in] */ IView* v,
    /* [in] */ Int32 index)
{
    Int32 appWidgetId;
    IKeyguardWidgetFrame::Probe(v)->GetContentAppWidgetId(&appWidgetId);
    AutoPtr<ArrayOf<Int32> > pagesRange = ArrayOf<Int32>::Alloc(mTempVisiblePagesRange->GetLength());
    GetVisiblePages(pagesRange);
    BoundByReorderablePages(TRUE, pagesRange);
    if (mCallbacks != NULL) {
        mCallbacks->OnAddView(v);
    }
    // Subtract from the index to take into account pages before the reorderable
    // pages (e.g. the "add widget" page)
    AutoPtr<IRunnable> r = new MyRunnable2(this, appWidgetId, index, (*pagesRange)[0]);
    Boolean res;
    return mBackgroundWorkerHandler->Post(r, &res);
}

ECode KeyguardWidgetPager::AddWidget(
    /* [in] */ IView* widget,
    /* [in] */ Int32 pageIndex)
{
    AutoPtr<IKeyguardWidgetFrame> frame;
    // All views contained herein should be wrapped in a KeyguardWidgetFrame
    if (IKeyguardWidgetFrame::Probe(widget) == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        CKeyguardWidgetFrame::New(context, (IKeyguardWidgetFrame**)&frame);
        AutoPtr<IFrameLayoutLayoutParams> lp;
        CFrameLayoutLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
                IViewGroupLayoutParams::MATCH_PARENT, (IFrameLayoutLayoutParams**)&lp);
        lp->SetGravity(IGravity::TOP);

        // The framework adds a default padding to AppWidgetHostView. We don't need this padding
        // for the Keyguard, so we override it to be 0.
        widget->SetPadding(0,  0, 0, 0);
        IViewGroup::Probe(frame)->AddView(widget, IViewGroupLayoutParams::Probe(lp));

        // We set whether or not this widget supports vertical resizing.
        if (IAppWidgetHostView::Probe(widget) != NULL) {
            AutoPtr<IAppWidgetHostView> awhv = IAppWidgetHostView::Probe(widget);
            AutoPtr<IAppWidgetProviderInfo> info;
            awhv->GetAppWidgetInfo((IAppWidgetProviderInfo**)&info);

            Int32 mode;
            info->GetResizeMode(&mode);
            if ((mode & IAppWidgetProviderInfo::RESIZE_VERTICAL) != 0) {
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
    AutoPtr<InnerListener> listener = new InnerListener(this);
    IView::Probe(frame)->SetOnLongClickListener(listener);
    frame->SetWorkerHandler(mBackgroundWorkerHandler);

    if (pageIndex == -1) {
        AddView(IView::Probe(frame), pageLp);
    }
    else {
        AddView(IView::Probe(frame), pageIndex, pageLp);
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
        content->GetContentDescription((ICharSequence**)&cchar);
        AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
        array->Set(0, TO_IINTERFACE(cchar));
        String contentDescription;
        mContext->GetString(
            R::string::keyguard_accessibility_widget,
            array, &contentDescription);
        AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(contentDescription);
        IView::Probe(frame)->SetContentDescription(cchar2);
    }
    UpdateWidgetFrameImportantForAccessibility(frame);
    return NOERROR;
}

ECode KeyguardWidgetPager::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index)
{
    FAIL_RETURN(EnforceKeyguardWidgetFrame(child))
    return PagedView::AddView(child, index);
}

ECode KeyguardWidgetPager::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    FAIL_RETURN(EnforceKeyguardWidgetFrame(child))
    return PagedView::AddView(child, width, height);
}

ECode KeyguardWidgetPager::AddView(
    /* [in] */ IView* child,
    /* [in] */ IViewGroupLayoutParams* params)
{
    FAIL_RETURN(EnforceKeyguardWidgetFrame(child))
    return PagedView::AddView(child, params);
}

ECode KeyguardWidgetPager::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index,
    /* [in] */ IViewGroupLayoutParams* params)
{
    FAIL_RETURN(EnforceKeyguardWidgetFrame(child))
    return PagedView::AddView(child, index, params);
}

ECode KeyguardWidgetPager::EnforceKeyguardWidgetFrame(
    /* [in] */ IView* child)
{
    if (IKeyguardWidgetFrame::Probe(child) == NULL) {
        // throw new IllegalArgumentException(
        //         "KeyguardWidgetPager children must be KeyguardWidgetFrames");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode KeyguardWidgetPager::GetWidgetPageAt(
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

ECode KeyguardWidgetPager::OnUnhandledTap(
    /* [in] */ IMotionEvent* ev)
{
    return ShowPagingFeedback();
}

ECode KeyguardWidgetPager::OnPageBeginMoving()
{
    if (mViewStateManager != NULL) {
        mViewStateManager->OnPageBeginMoving();
    }
    Boolean res;
    if (IsReordering(FALSE, &res), !res) {
        ShowOutlinesAndSidePages();
    }
    UserActivity();
    return NOERROR;
}

ECode KeyguardWidgetPager::OnPageEndMoving()
{
    if (mViewStateManager != NULL) {
        mViewStateManager->OnPageEndMoving();
    }

    // In the reordering case, the pages will be faded appropriately on completion
    // of the zoom in animation.
    Boolean res;
    if (IsReordering(FALSE, &res), !res) {
        HideOutlinesAndSidePages();
    }
    return NOERROR;
}

ECode KeyguardWidgetPager::EnablePageContentLayers()
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

ECode KeyguardWidgetPager::DisablePageContentLayers()
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

ECode KeyguardWidgetPager::OverScroll(
    /* [in] */ Float amount)
{
    return AcceleratedOverScroll(amount);
}

ECode KeyguardWidgetPager::BackgroundAlphaInterpolator(
    /* [in] */ Float r,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    *result = Elastos::Core::Math::Min(1.0f, r);
    return NOERROR;
}

void KeyguardWidgetPager::UpdatePageAlphaValues(
    /* [in] */ Int32 screenCenter)
{
    return;
}

ECode KeyguardWidgetPager::GetAlphaForPage(
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
        *result = index == _index ? 1.0f : 0.0f;
        return NOERROR;
    }
    if (showSidePages) {
        *result = 1.0f;
        return NOERROR;
    }
    else {
        *result = index == mCurrentPage ? 1.0f : 0.0f;
        return NOERROR;
    }
}

ECode KeyguardWidgetPager::GetOutlineAlphaForPage(
    /* [in] */ Int32 screenCenter,
    /* [in] */ Int32 index,
    /* [in] */ Boolean showSidePages,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    if (showSidePages) {
        Float _result;
        GetAlphaForPage(screenCenter, index, showSidePages, &_result);
        *result = _result * KeyguardWidgetFrame::OUTLINE_ALPHA_MULTIPLIER;
        return NOERROR;
    }
    else {
        *result = 0.0f;
        return NOERROR;
    }
}

ECode KeyguardWidgetPager::IsOverScrollChild(
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

ECode KeyguardWidgetPager::ScreenScrolled(
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
            GetScrollProgress(screenCenter, IView::Probe(v), i, &scrollProgress);

            IView::Probe(v)->SetCameraDistance(mDensity * CAMERA_DISTANCE);

            Boolean res;
            IsOverScrollChild(i, scrollProgress, &res);
            if (res && PERFORM_OVERSCROLL_ROTATION) {
                Int32 width;
                IView::Probe(v)->GetMeasuredWidth(&width);
                Float pivotX = width / 2;
                Int32 height;
                IView::Probe(v)->GetMeasuredHeight(&height);
                Float pivotY = height / 2;
                IView::Probe(v)->SetPivotX(pivotX);
                IView::Probe(v)->SetPivotY(pivotY);
                IView::Probe(v)->SetRotationY(- OVERSCROLL_MAX_ROTATION * scrollProgress);
                v->SetOverScrollAmount(Elastos::Core::Math::Abs(scrollProgress),
                        scrollProgress < 0);
            }
            else {
                IView::Probe(v)->SetRotationY(0.0f);
                v->SetOverScrollAmount(0, FALSE);
            }

            Float alpha;
            IView::Probe(v)->GetAlpha(&alpha);
            // If the view has 0 alpha, we set it to be invisible so as to prevent
            // it from accepting touches
            Int32 visibility;
            if (alpha == 0) {
                IView::Probe(v)->SetVisibility(INVISIBLE);
            }
            else if ((IView::Probe(v)->GetVisibility(&visibility), visibility)
                    != VISIBLE) {
                IView::Probe(v)->SetVisibility(VISIBLE);
            }
        }
    }
    return NOERROR;
}

ECode KeyguardWidgetPager::IsWidgetPage(
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
    if (v != NULL && IKeyguardWidgetFrame::Probe(v) != NULL) {
        AutoPtr<IKeyguardWidgetFrame> kwf = IKeyguardWidgetFrame::Probe(v);
        Int32 id;
        kwf->GetContentAppWidgetId(&id);
        *result = id != IAppWidgetManager::INVALID_APPWIDGET_ID;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode KeyguardWidgetPager::BoundByReorderablePages(
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

ECode KeyguardWidgetPager::ReorderStarting()
{
    return ShowOutlinesAndSidePages();
}

ECode KeyguardWidgetPager::OnStartReordering()
{
    PagedView::OnStartReordering();
    EnablePageContentLayers();
    return ReorderStarting();
}

ECode KeyguardWidgetPager::OnEndReordering()
{
    PagedView::OnEndReordering();
    return HideOutlinesAndSidePages();
}

ECode KeyguardWidgetPager::ShowOutlinesAndSidePages()
{
    return AnimateOutlinesAndSidePages(TRUE);
}

ECode KeyguardWidgetPager::HideOutlinesAndSidePages()
{
    return AnimateOutlinesAndSidePages(FALSE);
}

ECode KeyguardWidgetPager::UpdateChildrenContentAlpha(
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

ECode KeyguardWidgetPager::ShowInitialPageHints()
{
    mShowingInitialHints = TRUE;
    return UpdateChildrenContentAlpha(KeyguardWidgetFrame::OUTLINE_ALPHA_MULTIPLIER);
}

ECode KeyguardWidgetPager::SetCurrentPage(
    /* [in] */ Int32 currentPage)
{
    PagedView::SetCurrentPage(currentPage);
    UpdateChildrenContentAlpha(0.0f);
    UpdateWidgetFramesImportantForAccessibility();
    return NOERROR;
}

ECode KeyguardWidgetPager::OnAttachedToWindow()
{
    PagedView::OnAttachedToWindow();
    mHasMeasure = FALSE;
    return NOERROR;
}

ECode KeyguardWidgetPager::OnMeasure(
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

ECode KeyguardWidgetPager::AnimateOutlinesAndSidePages(
    /* [in] */ Boolean show)
{
    return AnimateOutlinesAndSidePages(show, -1);
}

ECode KeyguardWidgetPager::SetWidgetToResetOnPageFadeOut(
    /* [in] */ Int32 widget)
{
    mWidgetToResetAfterFadeOut = widget;
    return NOERROR;
}

ECode KeyguardWidgetPager::GetWidgetToResetOnPageFadeOut(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mWidgetToResetAfterFadeOut;
    return NOERROR;
}

ECode KeyguardWidgetPager::AnimateOutlinesAndSidePages(
    /* [in] */ Boolean show,
    /* [in] */ Int32 duration)
{
    if (mChildrenOutlineFadeAnimation != NULL) {
        IAnimator::Probe(mChildrenOutlineFadeAnimation)->Cancel();
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
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = finalContentAlpha;
        helper->OfFloat(String("contentAlpha"), array, (IPropertyValuesHolder**)&alpha);

        AutoPtr<IObjectAnimatorHelper> helper2;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper2);
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > array2 =
                ArrayOf<IPropertyValuesHolder*>::Alloc(1);
        array2->Set(0, alpha);
        AutoPtr<IObjectAnimator> a;
        helper2->OfPropertyValuesHolder(child, array2, (IObjectAnimator**)&a);
        anims->Add(TO_IINTERFACE(a));

        Float finalOutlineAlpha = 0.0f;
        if (show) {
            GetOutlineAlphaForPage(mScreenCenter, i, TRUE, &finalOutlineAlpha);
        }
        child->FadeFrame(TO_IINTERFACE(this), show, finalOutlineAlpha, duration);
    }

    CAnimatorSet::New((IAnimatorSet**)&mChildrenOutlineFadeAnimation);
    mChildrenOutlineFadeAnimation->PlayTogether(ICollection::Probe(anims));

    IAnimator::Probe(mChildrenOutlineFadeAnimation)->SetDuration(duration);
    AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter(this, show);
    IAnimator::Probe(mChildrenOutlineFadeAnimation)->AddListener(lis);
    return IAnimator::Probe(mChildrenOutlineFadeAnimation)->Start();
}

ECode KeyguardWidgetPager::OnLongClick(
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

ECode KeyguardWidgetPager::RemoveWidget(
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
            IViewGroup::Probe(frame)->RemoveView(view);
            RemoveView(IView::Probe(frame));
        }
        else {
            Logger::W(TAG, "removeWidget() can't find: %s", TO_CSTR(view));
        }
    }
    return NOERROR;
}

ECode KeyguardWidgetPager::GetWidgetPageIndex(
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
        return IndexOfChild(IView::Probe(p), index);
    }
    return NOERROR;
}

ECode KeyguardWidgetPager::SetPageHoveringOverDeleteDropTarget(
    /* [in] */ Int32 viewIndex,
    /* [in] */ Boolean isHovering)
{
    AutoPtr<IKeyguardWidgetFrame> child;
    GetWidgetPageAt(viewIndex, (IKeyguardWidgetFrame**)&child);
    return child->SetIsHoveringOverDeleteDropTarget(isHovering);
}

ECode KeyguardWidgetPager::OnBouncerStateChanged(
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

ECode KeyguardWidgetPager::SetBouncerAnimationDuration(
    /* [in] */ Int32 duration)
{
    mBouncerZoomInOutDuration = duration;
    return NOERROR;
}

ECode KeyguardWidgetPager::ZoomInFromBouncer()
{
    Boolean res;
    if (mZoomInOutAnim != NULL && (IAnimator::Probe(mZoomInOutAnim)->IsRunning(&res),
            res)) {
        IAnimator::Probe(mZoomInOutAnim)->Cancel();
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
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = 1.0f;
        AutoPtr<IObjectAnimator> animator1;
        helper->OfFloat(currentPage, String("scaleX"), array, (IObjectAnimator**)&animator1);
        AutoPtr<ArrayOf<Float> > array2 = ArrayOf<Float>::Alloc(1);
        (*array2)[0] = 1.0f;
        AutoPtr<IObjectAnimator> animator2;
        helper->OfFloat(currentPage , String("scaleY"), array2, (IObjectAnimator**)&animator2);

        AutoPtr<ArrayOf<IAnimator*> > array3 = ArrayOf<IAnimator*>::Alloc(2);
        array3->Set(0, IAnimator::Probe(animator1));
        array3->Set(1, IAnimator::Probe(animator2));
        mZoomInOutAnim->PlayTogether(array3);
        IAnimator::Probe(mZoomInOutAnim)->SetDuration(mBouncerZoomInOutDuration);
        AutoPtr<IDecelerateInterpolator> interpolator;
        CDecelerateInterpolator::New(1.5f, (IDecelerateInterpolator**)&interpolator);
        IAnimator::Probe(mZoomInOutAnim)->SetInterpolator(
                ITimeInterpolator::Probe(interpolator));
        IAnimator::Probe(mZoomInOutAnim)->Start();
    }
    if (IKeyguardWidgetFrame::Probe(currentPage) != NULL) {
        IKeyguardWidgetFrame::Probe(currentPage)->OnBouncerShowing(FALSE);
    }
    return NOERROR;
}

ECode KeyguardWidgetPager::ZoomOutToBouncer()
{
    Boolean res;
    if (mZoomInOutAnim != NULL && (IAnimator::Probe(mZoomInOutAnim)->IsRunning(&res),
            res)) {
        IAnimator::Probe(mZoomInOutAnim)->Cancel();
    }
    Int32 curPage;
    GetCurrentPage(&curPage);
    AutoPtr<IView> currentPage;
    GetPageAt(curPage, (IView**)&currentPage);
    if (ShouldSetTopAlignedPivotForWidget(curPage, &res), res) {
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
        CAnimatorSet::New((IAnimatorSet**)&mZoomInOutAnim);

        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = BOUNCER_SCALE_FACTOR;
        AutoPtr<IObjectAnimator> animator1;
        helper->OfFloat(currentPage, String("scaleX"), array,
                (IObjectAnimator**)&animator1);
        AutoPtr<ArrayOf<Float> > array2 = ArrayOf<Float>::Alloc(1);
        (*array2)[0] = BOUNCER_SCALE_FACTOR;
        AutoPtr<IObjectAnimator> animator2;
        helper->OfFloat(currentPage , String("scaleY"), array2,
                (IObjectAnimator**)&animator2);

        AutoPtr<ArrayOf<IAnimator*> > array3 = ArrayOf<IAnimator*>::Alloc(2);
        array3->Set(0, IAnimator::Probe(animator1));
        array3->Set(1, IAnimator::Probe(animator2));
        mZoomInOutAnim->PlayTogether(array3);
        IAnimator::Probe(mZoomInOutAnim)->SetDuration(mBouncerZoomInOutDuration);
        AutoPtr<IDecelerateInterpolator> interpolator;
        CDecelerateInterpolator::New(1.5f, (IDecelerateInterpolator**)&interpolator);
        IAnimator::Probe(mZoomInOutAnim)->SetInterpolator(
                ITimeInterpolator::Probe(interpolator));
        IAnimator::Probe(mZoomInOutAnim)->Start();
    }
    if (IKeyguardWidgetFrame::Probe(currentPage) != NULL) {
        IKeyguardWidgetFrame::Probe(currentPage)->OnBouncerShowing(TRUE);
    }
    return NOERROR;
}

ECode KeyguardWidgetPager::SetAddWidgetEnabled(
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

ECode KeyguardWidgetPager::IsAddPage(
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

ECode KeyguardWidgetPager::IsCameraPage(
    /* [in] */ Int32 pageIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IView> v;
    GetChildAt(pageIndex, (IView**)&v);
    *result = v != NULL && ICameraWidgetFrame::Probe(v) != NULL;
    return NOERROR;
}

ECode KeyguardWidgetPager::ShouldSetTopAlignedPivotForWidget(
    /* [in] */ Int32 childIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res1, res2;
    *result = (IsCameraPage(childIndex, &res1), !res1) &&
            (PagedView::ShouldSetTopAlignedPivotForWidget(childIndex, &res2), res2);
    return NOERROR;
}

Int32 KeyguardWidgetPager::FindClockInHierarchy(
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

Int32 KeyguardWidgetPager::GetClockFlags(
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
        helper->GetTimeZone(timeZone, (ITimeZone**)&timezone);
        Boolean res;
        if(IObject::Probe(defaultzone)->Equals(timezone, &res), !res) {
            // Ignore clocks showing another timezone
            return 0;
        }
    }

    AutoPtr<ICharSequence> format;
    clock->GetFormat((ICharSequence**)&format);
    Char32 hour;
    Boolean res;
    if (clock->Is24HourModeEnabled(&res),res) {
        hour = Elastos::Droid::Text::Format::IDateFormat::HOUR_OF_DAY;
    }
    else {
        hour = Elastos::Droid::Text::Format::IDateFormat::HOUR;
    }

    if (DateFormat::HasDesignator(format, hour)) {
        flags |= FLAG_HAS_LOCAL_HOUR;
    }
    if (DateFormat::HasDesignator(format,
            Elastos::Droid::Text::Format::IDateFormat::MINUTE)) {
        flags |= FLAG_HAS_LOCAL_MINUTE;
    }

    return flags;
}

ECode KeyguardWidgetPager::HandleExternalCameraEvent(
    /* [in] */ IMotionEvent* event)
{
    BeginCameraEvent();
    Int32 cameraPage;
    GetPageCount(&cameraPage);
    cameraPage = cameraPage - 1;
    //Boolean endWarp = FALSE;

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
        DispatchTouchEvent(event, &res);
    }
    return EndCameraEvent();
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
