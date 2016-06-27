
#include "elastos/droid/systemui/statusbar/phone/CPhoneStatusBarView.h"
#include "elastos/droid/systemui/statusbar/phone/CPhoneStatusBar.h"
#include "../R.h"
#include "Elastos.Droid.View.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CPhoneStatusBarView::Runnable1::Runnable1(
    /* [in] */ CPhoneStatusBarView* host)
    : mHost(host)
{}

ECode CPhoneStatusBarView::Runnable1::Run()
{
    mHost->mBar->MakeExpandedInvisible();
    return NOERROR;
}

const String CPhoneStatusBarView::TAG("PhoneStatusBarView");
const Boolean CPhoneStatusBarView::DEBUG = CPhoneStatusBar::DEBUG;
const Boolean CPhoneStatusBarView::DEBUG_GESTURES = TRUE;

CAR_OBJECT_IMPL(CPhoneStatusBarView)

CAR_INTERFACE_IMPL(CPhoneStatusBarView, PanelBar, IPhoneStatusBarView)

ECode CPhoneStatusBarView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    PanelBar::constructor(context, attrs);

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);

    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    mBarTransitions = new PhoneStatusBarTransitions(this);
    return NOERROR;
}

ECode CPhoneStatusBarView::GetBarTransitions(
    /* [out] */ IBarTransitions** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBarTransitions;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CPhoneStatusBarView::SetBar(
    /* [in] */ IPhoneStatusBar* bar)
{
    mBar = bar;
    return NOERROR;
}

ECode CPhoneStatusBarView::SetScrimController(
    /* [in] */ IScrimController* scrimController)
{
    mScrimController = scrimController;
    return NOERROR;
}

ECode CPhoneStatusBarView::OnFinishInflate()
{
    mBarTransitions->Init();
    return NOERROR;
}

ECode CPhoneStatusBarView::AddPanel(
    /* [in] */ IPanelView* pv)
{
    PanelBar::AddPanel(pv);
    Int32 id = 0;
    if ((IView::Probe(pv)->GetId(&id), id) == R::id::notification_panel) {
        mNotificationPanel = pv;
    }
    return NOERROR;
}

ECode CPhoneStatusBarView::PanelsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mBar->PanelsEnabled(result);
}

ECode CPhoneStatusBarView::OnRequestSendAccessibilityEvent(
    /* [in] */ IView* child,
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    if (PanelBar::OnRequestSendAccessibilityEvent(child, event, &tmp), tmp) {
        // The status bar is very small so augment the view that the user is touching
        // with the content of the status bar a whole. This way an accessibility service
        // may announce the current item as well as the entire content if appropriate.
        AutoPtr<IAccessibilityEvent> record;
        AutoPtr<IAccessibilityEventHelper> helper;
        CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&helper);
        helper->Obtain((IAccessibilityEvent**)&record);
        OnInitializeAccessibilityEvent(record);
        DispatchPopulateAccessibilityEvent(record, &tmp);
        event->AppendRecord(IAccessibilityRecord::Probe(record));
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CPhoneStatusBarView::SelectPanelForTouch(
    /* [in] */ IMotionEvent* touch,
    /* [out] */ IPanelView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NULL;

    // No double swiping. If either panel is open, nothing else can be pulled down.
    Float v = 0;
    mNotificationPanel->GetExpandedHeight(&v);
    if (v <= 0) {
        *view = mNotificationPanel;
        REFCOUNT_ADD(*view);
    }
    return NOERROR;
}

ECode CPhoneStatusBarView::OnPanelPeeked()
{
    PanelBar::OnPanelPeeked();
    mBar->MakeExpandedVisible(FALSE);
    return NOERROR;
}

ECode CPhoneStatusBarView::OnAllPanelsCollapsed()
{
    PanelBar::OnAllPanelsCollapsed();

    // Close the status bar in the next frame so we can show the end of the animation.
    AutoPtr<Runnable1> run = new Runnable1(this);
    PostOnAnimation(run);
    mLastFullyOpenedPanel = NULL;
    return NOERROR;
}

ECode CPhoneStatusBarView::OnPanelFullyOpened(
    /* [in] */ IPanelView* openPanel)
{
    PanelBar::OnPanelFullyOpened(openPanel);
    if (openPanel != mLastFullyOpenedPanel) {
        IView::Probe(openPanel)->SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
    }
    mLastFullyOpenedPanel = openPanel;
    return NOERROR;
}

ECode CPhoneStatusBarView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    Boolean barConsumedEvent = FALSE;
    mBar->InterceptTouchEvent(event, &barConsumedEvent);

    if (DEBUG_GESTURES) {
        Int32 masked = 0;
        if ((event->GetActionMasked(&masked), masked) != IMotionEvent::ACTION_MOVE) {
            Float x = 0, y = 0;
            event->GetX(&x);
            event->GetY(&y);
            Logger::D("CPhoneStatusBarView", "OnTouchEvent masked=[%d], x=[%d], y=[%d], barConsumedEvent=[%d]"
                    , masked, (Int32)x, (Int32)y, (barConsumedEvent ? 1 : 0));
            // EventLog.writeEvent(EventLogTags.SYSUI_PANELBAR_TOUCH,
            //         event.getActionMasked(), (Int32) event.getX(), (Int32) event.getY(),
            //         barConsumedEvent ? 1 : 0);
        }
    }

    Boolean tmp = FALSE;
    *result = barConsumedEvent || (PanelBar::OnTouchEvent(event, &tmp), tmp);
    return NOERROR;
}

ECode CPhoneStatusBarView::OnTrackingStarted(
    /* [in] */ IPanelView* panel)
{
    PanelBar::OnTrackingStarted(panel);
    mBar->OnTrackingStarted();
    mScrimController->OnTrackingStarted();
    return NOERROR;
}

ECode CPhoneStatusBarView::OnTrackingStopped(
    /* [in] */ IPanelView* panel,
    /* [in] */ Boolean expand)
{
    PanelBar::OnTrackingStopped(panel, expand);
    mBar->OnTrackingStopped(expand);
    return NOERROR;
}

ECode CPhoneStatusBarView::OnExpandingFinished()
{
    PanelBar::OnExpandingFinished();
    mScrimController->OnExpandingFinished();
    return NOERROR;
}

ECode CPhoneStatusBarView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean b1 = FALSE, b2 = FALSE;
    *result = (mBar->InterceptTouchEvent(event, &b1), b1) || (PanelBar::OnInterceptTouchEvent(event, &b2), b2);
    return NOERROR;
}

ECode CPhoneStatusBarView::PanelExpansionChanged(
    /* [in] */ IPanelView* panel,
    /* [in] */ Float frac,
    /* [in] */ Boolean expanded)
{
    PanelBar::PanelExpansionChanged(panel, frac, expanded);
    mScrimController->SetPanelExpansion(frac);
    ((CPhoneStatusBar*)mBar.Get())->UpdateCarrierLabelVisibility(FALSE);
    return NOERROR;
}

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
