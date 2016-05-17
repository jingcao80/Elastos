
#include "elastos/droid/systemui/statusbar/policy/CHeadsUpNotificationView.h"
#include "elastos/droid/systemui/CExpandHelper.h"
#include "elastos/droid/systemui/CSwipeHelper.h"
#include "../../R.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::RegionOp_DIFFERENCE;
using Elastos::Droid::Service::Notification::IStatusBarNotification;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::EIID_IOnComputeInternalInsetsListener;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::SystemUI::StatusBar::IBaseStatusBar;
using Elastos::Droid::SystemUI::StatusBar::ICommandQueueCallbacks;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const String CHeadsUpNotificationView::TAG("HeadsUpNotificationView");
const Boolean CHeadsUpNotificationView::DEBUG = TRUE;
const Boolean CHeadsUpNotificationView::SPEW = DEBUG;
AutoPtr<IViewOutlineProvider> CHeadsUpNotificationView::CONTENT_HOLDER_OUTLINE_PROVIDER = new _ViewOutlineProvider();

const Boolean CHeadsUpNotificationView::EdgeSwipeHelper::DEBUG_EDGE_SWIPE = FALSE;
CAR_INTERFACE_IMPL(CHeadsUpNotificationView::EdgeSwipeHelper, Object, IGefingerpoken);
CHeadsUpNotificationView::EdgeSwipeHelper::EdgeSwipeHelper(
    /* [in] */ Float touchSlop,
    /* [in] */ CHeadsUpNotificationView* host)
    : mTouchSlop(0)
    , mConsuming(FALSE)
    , mFirstY(0)
    , mFirstX(0)
    , mHost(host)
{
    mTouchSlop = touchSlop;
}

ECode CHeadsUpNotificationView::EdgeSwipeHelper::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Float x = 0, y = 0;
    ev->GetX(&x);
    ev->GetY(&y);
    Int32 mask = 0;
    switch ((ev->GetActionMasked(&mask), mask)) {
        case IMotionEvent::ACTION_DOWN:
            if (DEBUG_EDGE_SWIPE) Logger::D(TAG, "action down %f", y);
            mFirstX = x;
            mFirstY = y;
            mConsuming = FALSE;
            break;

        case IMotionEvent::ACTION_MOVE: {
            if (DEBUG_EDGE_SWIPE) Logger::D(TAG, "action move %f", y);
            Float dY = y - mFirstY;
            Float daX = Elastos::Core::Math::Abs(x - mFirstX);
            Float daY = Elastos::Core::Math::Abs(dY);
            if (!mConsuming && (4.f * daX) < daY && daY > mTouchSlop) {
                if (dY > 0) {
                    if (DEBUG_EDGE_SWIPE) Logger::D(TAG, "found an open");
                    ICommandQueueCallbacks::Probe(mHost->mBar)->AnimateExpandNotificationsPanel();
                }
                if (dY < 0) {
                    if (DEBUG_EDGE_SWIPE) Logger::D(TAG, "found a close");
                    mHost->mBar->OnHeadsUpDismissed();
                }
                mConsuming = TRUE;
            }
        }
            break;

        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            if (DEBUG_EDGE_SWIPE) Logger::D(TAG, "action done" );
            mConsuming = FALSE;
            break;
    }
    *result = mConsuming;
    return NOERROR;
}

ECode CHeadsUpNotificationView::EdgeSwipeHelper::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mConsuming;
    return NOERROR;
}

ECode CHeadsUpNotificationView::_ViewOutlineProvider::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    Int32 outlineLeft = 0;
    view->GetPaddingLeft(&outlineLeft);
    Int32 outlineTop = 0;
    view->GetPaddingTop(&outlineTop);

    // Apply padding to shadow.
    Int32 w = 0, h = 0, r = 0, b = 0;
    view->GetWidth(&w);
    view->GetHeight(&h);
    view->GetPaddingRight(&r);
    view->GetPaddingBottom(&b);
    outline->SetRect(outlineLeft, outlineTop,
            w - outlineLeft - r,
            h - outlineTop - b);
    return NOERROR;
}


CAR_OBJECT_IMPL(CHeadsUpNotificationView);
CAR_INTERFACE_IMPL_4(CHeadsUpNotificationView, FrameLayout, IHeadsUpNotificationView, ISwipeHelperCallback, \
        IExpandHelperCallback, IOnComputeInternalInsetsListener);
CHeadsUpNotificationView::CHeadsUpNotificationView()
    : mTouchSensitivityDelay(0)
    , mMaxAlpha(1.f)
    , mStartTouchTime(0)
{
    CRect::New((IRect**)&mTmpRect);
    mTmpTwoArray = ArrayOf<Int32>::Alloc(2);
}

ECode CHeadsUpNotificationView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CHeadsUpNotificationView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FrameLayout::constructor(context, attrs, defStyle);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetInteger(R::integer::heads_up_sensitivity_delay, &mTouchSensitivityDelay);
    if (DEBUG) Logger::V(TAG, "create() %d", mTouchSensitivityDelay);
    return NOERROR;
}

ECode CHeadsUpNotificationView::UpdateResources()
{
    if (mContentHolder != NULL) {
        AutoPtr<IViewGroupLayoutParams> lp;
        IView::Probe(mContentHolder)->GetLayoutParams((IViewGroupLayoutParams**)&lp);

        AutoPtr<IResources> res;
        GetResources((IResources**)&res);
        Int32 value = 0;
        res->GetDimensionPixelSize(R::dimen::notification_panel_width, &value);
        lp->SetWidth(value);
        res->GetInteger(R::integer::notification_panel_layout_gravity, &value);
        IFrameLayoutLayoutParams::Probe(lp)->SetGravity(value);
        IView::Probe(mContentHolder)->SetLayoutParams(lp);
    }
    return NOERROR;
}

ECode CHeadsUpNotificationView::SetBar(
    /* [in] */ IPhoneStatusBar* bar)
{
    mBar = bar;
    return NOERROR;
}

ECode CHeadsUpNotificationView::GetHolder(
    /* [out] */ IViewGroup** holder)
{
    VALIDATE_NOT_NULL(holder);
    *holder = mContentHolder;
    REFCOUNT_ADD(*holder);
    return NOERROR;
}

ECode CHeadsUpNotificationView::ShowNotification(
    /* [in] */ INotificationDataEntry* headsUp,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mHeadsUp != NULL && headsUp != NULL) {
        String k1, k2;
        mHeadsUp->GetKey(&k1);
        headsUp->GetKey(&k2);
        if (!k1.Equals(k2)) {
            // bump any previous heads up back to the shade
            ReleaseResources();
        }
    }

    mHeadsUp = headsUp;
    if (mContentHolder != NULL) {
        mContentHolder->RemoveAllViews();
    }

    if (mHeadsUp != NULL) {
        AutoPtr<IExpandableNotificationRow> row;
        mHeadsUp->GetRow((IExpandableNotificationRow**)&row);
        row->SetSystemExpanded(TRUE);
        row->SetSensitive(FALSE);
        row->SetHideSensitive(
                FALSE, FALSE /* animated */, 0 /* delay */, 0 /* duration */);
        if (mContentHolder == NULL) {
            // too soon!
            return FALSE;
        }
        IView::Probe(mContentHolder)->SetX(0);
        IView::Probe(mContentHolder)->SetVisibility(IView::VISIBLE);
        IView::Probe(mContentHolder)->SetAlpha(mMaxAlpha);
        mContentHolder->AddView(IView::Probe(row));
        SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_CONTENT_CHANGED);

        mSwipeHelper->SnapChild(IView::Probe(mContentHolder), 1.f);

        Int64 t = 0;
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&t);
        mStartTouchTime = t + mTouchSensitivityDelay;

        mHeadsUp->SetInterruption();

        // 2. Animate mHeadsUpNotificationView in
        IBaseStatusBar::Probe(mBar)->ScheduleHeadsUpOpen();

        // 3. Set alarm to age the notification off
        IBaseStatusBar::Probe(mBar)->ResetHeadsUpDecayTimer();
    }
    return TRUE;
}

ECode CHeadsUpNotificationView::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    FrameLayout::OnVisibilityChanged(changedView, visibility);
    Int32 v = 0;
    if ((changedView->GetVisibility(&v), v) == IView::VISIBLE) {
        SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_CONTENT_CHANGED);
    }
    return NOERROR;
}

ECode CHeadsUpNotificationView::IsShowing(
    /* [in] */ const String& key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    String k1;
    *result = mHeadsUp != NULL && (mHeadsUp->GetKey(&k1), k1).Equals(key);
    return NOERROR;
}

ECode CHeadsUpNotificationView::Clear()
{
    mHeadsUp = NULL;
    IBaseStatusBar::Probe(mBar)->ScheduleHeadsUpClose();
    return NOERROR;
}

ECode CHeadsUpNotificationView::Dismiss()
{
    if (mHeadsUp == NULL) return NOERROR;
    AutoPtr<IStatusBarNotification> notification;
    mHeadsUp->GetNotification((IStatusBarNotification**)&notification);
    Boolean tmp = FALSE;
    if (notification->IsClearable(&tmp), tmp) {
        IBaseStatusBar::Probe(mBar)->OnNotificationClear(notification);
    }
    else {
        ReleaseResources();
    }
    mHeadsUp = NULL;
    IBaseStatusBar::Probe(mBar)->ScheduleHeadsUpClose();
    return NOERROR;
}

ECode CHeadsUpNotificationView::ReleaseResources()
{
    if (mHeadsUp != NULL) {
        AutoPtr<IStatusBarNotification> notification;
        mHeadsUp->GetNotification((IStatusBarNotification**)&notification);
        mBar->DisplayNotificationFromHeadsUp(notification);
    }
    mHeadsUp = NULL;
    return NOERROR;
}

ECode CHeadsUpNotificationView::ReleaseAndClose()
{
    ReleaseResources();
    IBaseStatusBar::Probe(mBar)->ScheduleHeadsUpClose();
    return NOERROR;
}

ECode CHeadsUpNotificationView::GetEntry(
    /* [out] */ INotificationDataEntry** entry)
{
    VALIDATE_NOT_NULL(entry);
    *entry = mHeadsUp;
    REFCOUNT_ADD(*entry);
    return NOERROR;
}

ECode CHeadsUpNotificationView::IsClearable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mHeadsUp == NULL) {
        *result = TRUE;
        return NOERROR;
    }

    AutoPtr<IStatusBarNotification> notification;
    mHeadsUp->GetNotification((IStatusBarNotification**)&notification);
    return notification->IsClearable(result);
}

ECode CHeadsUpNotificationView::OnAttachedToWindow()
{
    AutoPtr<IViewConfiguration> viewConfiguration;
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    helper->Get(context, (IViewConfiguration**)&viewConfiguration);

    Int32 value = 0;
    viewConfiguration->GetScaledTouchSlop(&value);
    Float touchSlop = value;

    CSwipeHelper::New(ISwipeHelper::X, this, context, (ISwipeHelper**)&mSwipeHelper);
    mSwipeHelper->SetMaxSwipeProgress(mMaxAlpha);
    mEdgeSwipeHelper = new EdgeSwipeHelper(touchSlop, this);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 minHeight = 0;
    res->GetDimensionPixelSize(R::dimen::notification_min_height, &minHeight);
    Int32 maxHeight = 0;
    res->GetDimensionPixelSize(R::dimen::notification_max_height, &maxHeight);
    CExpandHelper::New(context, this, minHeight, maxHeight, (IExpandHelper**)&mExpandHelper);

    AutoPtr<IView> view;
    FindViewById(R::id::content_holder, (IView**)&view);
    mContentHolder = IViewGroup::Probe(view);
    IView::Probe(mContentHolder)->SetOutlineProvider(CONTENT_HOLDER_OUTLINE_PROVIDER);

    if (mHeadsUp != NULL) {
        // whoops, we're on already!
        Boolean tmp = FALSE;
        ShowNotification(mHeadsUp, &tmp);
    }

    AutoPtr<IViewTreeObserver> vo;
    GetViewTreeObserver((IViewTreeObserver**)&vo);
    vo->AddOnComputeInternalInsetsListener(this);
    return NOERROR;
}

ECode CHeadsUpNotificationView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DEBUG) Logger::V(TAG, "onInterceptTouchEvent()");

    Int64 t = 0;
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&t);
    if (t < mStartTouchTime) {
        *result = TRUE;
        return NOERROR;
    }

    Boolean tmp = FALSE;
    *result = (mEdgeSwipeHelper->OnInterceptTouchEvent(ev, &tmp), tmp)
            || (mSwipeHelper->OnInterceptTouchEvent(ev, &tmp), tmp)
            || (IGefingerpoken::Probe(mExpandHelper)->OnInterceptTouchEvent(ev, &tmp), tmp)
            || (FrameLayout::OnInterceptTouchEvent(ev, &tmp), tmp);
    return NOERROR;
}

void CHeadsUpNotificationView::OnDraw(
    /* [in] */ ICanvas* c)
{
    FrameLayout::OnDraw(c);
    if (DEBUG) {
        //Logger::D(TAG, "onDraw: canvas height: " + c.getHeight() + "px; measured height: "
        //        + getMeasuredHeight() + "px");
        Int32 value = 0, h = 0;
        c->Save(&value);
        Boolean tmp = FALSE;
        c->ClipRect(6, 6, (c->GetWidth(&value), value) - 6, (GetMeasuredHeight(&h), h) - 6,
                RegionOp_DIFFERENCE, &tmp);
        c->DrawColor(0xFFcc00cc);
        c->Restore();
    }
}

ECode CHeadsUpNotificationView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 t = 0;
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&t);
    if (t < mStartTouchTime) {
        *result = FALSE;
        return NOERROR;
    }
    IBaseStatusBar::Probe(mBar)->ResetHeadsUpDecayTimer();

    Boolean tmp = FALSE;
    *result = (mEdgeSwipeHelper->OnTouchEvent(ev, &tmp), tmp)
            || (mSwipeHelper->OnTouchEvent(ev, &tmp), tmp)
            || (IGefingerpoken::Probe(mExpandHelper)->OnTouchEvent(ev, &tmp), tmp)
            || (FrameLayout::OnTouchEvent(ev, &tmp), tmp);
    return NOERROR;
}

void CHeadsUpNotificationView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    FrameLayout::OnConfigurationChanged(newConfig);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float densityScale = 0;
    dm->GetDensity(&densityScale);
    mSwipeHelper->SetDensityScale(densityScale);

    AutoPtr<IViewConfiguration> viewConfiguration;
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    helper->Get(context, (IViewConfiguration**)&viewConfiguration);

    Int32 value = 0;
    viewConfiguration->GetScaledPagingTouchSlop(&value);
    Float pagingTouchSlop = value;
    mSwipeHelper->SetPagingTouchSlop(pagingTouchSlop);
}

ECode CHeadsUpNotificationView::GetChildAtRawPosition(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [out] */ IExpandableView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IExpandableView> v;
    return GetChildAtPosition(x, y, view);
}

ECode CHeadsUpNotificationView::GetChildAtPosition(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [out] */ IExpandableView** view)
{
    VALIDATE_NOT_NULL(view);
    if (mHeadsUp == NULL) {
        *view = NULL;
        return NOERROR;
    }
    AutoPtr<IExpandableNotificationRow> row;
    mHeadsUp->GetRow((IExpandableNotificationRow**)&row);
    *view = IExpandableView::Probe(row);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CHeadsUpNotificationView::CanChildBeExpanded(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mHeadsUp == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IExpandableNotificationRow> row;
    mHeadsUp->GetRow((IExpandableNotificationRow**)&row);
    Boolean tmp = FALSE;
    *result = IView::Probe(row) == v && (row->IsExpandable(&tmp), tmp);
    return NOERROR;
}

ECode CHeadsUpNotificationView::SetUserExpandedChild(
    /* [in] */ IView* v,
    /* [in] */ Boolean userExpanded)
{
    AutoPtr<IExpandableNotificationRow> row;
    if (mHeadsUp != NULL && (mHeadsUp->GetRow((IExpandableNotificationRow**)&row), IView::Probe(row)) == v) {
        row->SetUserExpanded(userExpanded);
    }
    return NOERROR;
}

ECode CHeadsUpNotificationView::SetUserLockedChild(
    /* [in] */ IView* v,
    /* [in] */ Boolean userLocked)
{
    AutoPtr<IExpandableNotificationRow> row;
    if (mHeadsUp != NULL && (mHeadsUp->GetRow((IExpandableNotificationRow**)&row), IView::Probe(row)) == v) {
        row->SetUserLocked(userLocked);
    }
    return NOERROR;
}

ECode CHeadsUpNotificationView::ExpansionStateChanged(
    /* [in] */ Boolean isExpanding)
{
    return NOERROR;
}

ECode CHeadsUpNotificationView::CanChildBeDismissed(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode CHeadsUpNotificationView::IsAntiFalsingNeeded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode CHeadsUpNotificationView::GetFalsingThresholdFactor(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 1.0f;
    return NOERROR;
}

ECode CHeadsUpNotificationView::OnChildDismissed(
    /* [in] */ IView* v)
{
    Logger::V(TAG, "User swiped heads up to dismiss");
    mBar->OnHeadsUpDismissed();
    return NOERROR;
}

ECode CHeadsUpNotificationView::OnBeginDrag(
    /* [in] */ IView* v)
{
    return NOERROR;
}

ECode CHeadsUpNotificationView::OnDragCancelled(
    /* [in] */ IView* v)
{
    IView::Probe(mContentHolder)->SetAlpha(mMaxAlpha); // sometimes this isn't quite reset
    return NOERROR;
}

ECode CHeadsUpNotificationView::OnChildSnappedBack(
    /* [in] */ IView* animView)
{
    return NOERROR;
}

ECode CHeadsUpNotificationView::UpdateSwipeProgress(
    /* [in] */ IView* animView,
    /* [in] */ Boolean dismissable,
    /* [in] */ Float swipeProgress,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IDrawable> d;
    GetBackground((IDrawable**)&d);
    d->SetAlpha((Int32) (255 * swipeProgress));
    *result = FALSE;
    return NOERROR;
}

ECode CHeadsUpNotificationView::GetChildAtPosition(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = IView::Probe(mContentHolder);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CHeadsUpNotificationView::GetChildContentView(
    /* [in] */ IView* v,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = IView::Probe(mContentHolder);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CHeadsUpNotificationView::OnComputeInternalInsets(
    /* [in] */ IInternalInsetsInfo* info)
{
    IView::Probe(mContentHolder)->GetLocationOnScreen(mTmpTwoArray);

    info->SetTouchableInsets(IInternalInsetsInfo::TOUCHABLE_INSETS_REGION);
    AutoPtr<IRegion> touchableRegion;
    info->GetTouchableRegion((IRegion**)&touchableRegion);
    Int32 w = 0, h = 0;
    IView::Probe(mContentHolder)->GetWidth(&w);
    IView::Probe(mContentHolder)->GetHeight(&h);
    Boolean tmp = FALSE;
    touchableRegion->Set((*mTmpTwoArray)[0], (*mTmpTwoArray)[1],
            (*mTmpTwoArray)[0] + w,
            (*mTmpTwoArray)[1] + h, &tmp);
    return NOERROR;
}

ECode CHeadsUpNotificationView::Escalate()
{
    IBaseStatusBar::Probe(mBar)->ScheduleHeadsUpEscalation();
    return NOERROR;
}

ECode CHeadsUpNotificationView::GetKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key);
    if (mHeadsUp == NULL) {
        *key = NULL;
        return NOERROR;
    }

    AutoPtr<IStatusBarNotification> notification;
    mHeadsUp->GetNotification((IStatusBarNotification**)&notification);
    return notification->GetKey(key);
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
