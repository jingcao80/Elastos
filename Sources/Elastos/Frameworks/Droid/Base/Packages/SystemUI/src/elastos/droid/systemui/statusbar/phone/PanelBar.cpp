#include "elastos/droid/systemui/statusbar/phone/PanelBar.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

const String PanelBar::TAG("PanelBar");

PanelBar::PanelBar()
    : mPanelExpandedFractionSum(0)
    , mState(IPanelBar::STATE_CLOSED)
    , mTracking(FALSE)
{
}

PanelBar::PanelBar(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : FrameLayout(context, attrs)
    , mPanelExpandedFractionSum(0)
    , mState(IPanelBar::STATE_CLOSED)
    , mTracking(FALSE)
{
}

ECode PanelBar::Go(
    /* [in] */ Int32 state)
{
    // if (DEBUG) LOG("go state: %d -> %d", mState, state);
    mState = state;
    return NOERROR;
}

//@Override
ECode PanelBar::OnFinishInflate()
{
    return FrameLayout::OnFinishInflate();
}

ECode PanelBar::AddPanel(
    /* [in] */ IPanelView* pv)
{
    mPanels.PushBack(pv);
    pv->SetBar(this);
    return NOERROR;
}

ECode PanelBar::SetPanelHolder(
    /* [in] */ IPanelHolder* ph)
{
    if (ph == NULL) {
        Slogger::E(TAG, "SetPanelHolder: NULL PanelHolder");
        return E_NULL_POINTER_EXCEPTION;
    }

    ph->SetBar(this);
    mPanelHolder = ph;
    Int32 N;
    ph->GetChildCount(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> v;
        ph->GetChildAt(i, (IView**)&v);
        IPanelView* pv = IPanelView::Probe(v.Get());
        if (pv != NULL) {
            AddPanel(pv);
        }
    }
    return NOERROR;
}

Float PanelBar::GetBarHeight()
{
    return GetMeasuredHeight();
}

AutoPtr<IPanelView> PanelBar::SelectPanelForTouch(
    /* [in] */ IMotionEvent* touch)
{
    Int32 N = mPanels.GetSize();
    Float x;
    touch->GetX(&x);
    Int32 index = (Int32)(N * x / GetMeasuredWidth());
    return mPanels[index];
}

Boolean PanelBar::PanelsEnabled()
{
    return TRUE;
}

//@Override
Boolean PanelBar::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    // Allow subclasses to implement enable/disable semantics
    if (!PanelsEnabled()) return FALSE;

    // figure out which panel needs to be talked to here
    Int32 action;
    event->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        AutoPtr<IPanelView> panel = SelectPanelForTouch(event);
        if (panel == NULL) {
            // panel is not there, so we'll eat the gesture
            // if (DEBUG) LOG("PanelBar.onTouch: no panel for x=%d, bailing", event.getX());
            mTouchingPanel = NULL;
            return TRUE;
        }
        Boolean enabled;
        panel->IsEnabled(&enabled);
        // if (DEBUG) LOG("PanelBar.onTouch: state=%d ACTION_DOWN: panel %s %s", mState, panel,
        //         (enabled ? "" : " (disabled)"));
        if (!enabled) {
            // panel is disabled, so we'll eat the gesture
            mTouchingPanel = NULL;
            return TRUE;
        }
        StartOpeningPanel(panel);
    }

    Boolean result = TRUE;
    if (mTouchingPanel != NULL) {
        mTouchingPanel->OnTouchEvent(event, &result);
    }
    return result;
}

// called from PanelView when self-expanding, too
ECode PanelBar::StartOpeningPanel(
    /* [in] */ IPanelView* panel)
{
    // if (DEBUG) LOG("startOpeningPanel: " + panel);
    mTouchingPanel = panel;
    mPanelHolder->SetSelectedPanel(mTouchingPanel);

    List<AutoPtr<IPanelView> >::Iterator it = mPanels.Begin();
    for (; it != mPanels.End(); ++it) {
        AutoPtr<IPanelView> pv = *it;
        if (pv.Get() != panel) {
            pv->Collapse();
        }
    }
    return NOERROR;
}

ECode PanelBar::PanelExpansionChanged(
    /* [in] */ IPanelView* panel,
    /* [in] */ Float frac)
{
    Boolean fullyClosed = TRUE;
    AutoPtr<IPanelView> fullyOpenedPanel;
    // if (DEBUG) LOG("panelExpansionChanged: start state=%d panel=%s", mState, panel.getName());
    mPanelExpandedFractionSum = 0.0f;
    Int32 visibility;
    Boolean visible;
    Float exHeight, thisFrac;

    List<AutoPtr<IPanelView> >::Iterator it = mPanels.Begin();
    for (; it != mPanels.End(); ++it) {
        AutoPtr<IPanelView> pv = *it;
        pv->GetVisibility(&visibility);
        visible = visibility == IView::VISIBLE;
        // adjust any other panels that may be partially visible
        pv->GetExpandedHeight(&exHeight);
        if (exHeight > 0.0f) {
            if (mState == IPanelBar::STATE_CLOSED) {
                Go(IPanelBar::STATE_OPENING);
                OnPanelPeeked();
            }
            fullyClosed = FALSE;
            pv->GetExpandedFraction(&thisFrac);
            mPanelExpandedFractionSum += (visible ? thisFrac : 0);
            // if (DEBUG) LOG("panelExpansionChanged:  -> %s: f=%.1f", pv.getName(), thisFrac);
            if (panel == pv) {
                if (thisFrac == 1.0f) fullyOpenedPanel = panel;
            }
        }

        pv->GetExpandedHeight(&exHeight);
        if (exHeight > 0.0f) {
            if (!visible) pv->SetVisibility(IView::VISIBLE);
        }
        else {
            if (visible) pv->SetVisibility(IView::GONE);
        }
    }

    mPanelExpandedFractionSum /= mPanels.GetSize();
    if (fullyOpenedPanel != NULL && !mTracking) {
        Go(IPanelBar::STATE_OPEN);
        OnPanelFullyOpened(fullyOpenedPanel);
    }
    else if (fullyClosed && !mTracking && mState != IPanelBar::STATE_CLOSED) {
        Go(IPanelBar::STATE_CLOSED);
        OnAllPanelsCollapsed();
    }

    // if (DEBUG) LOG("panelExpansionChanged: end state=%d [%s%s ]", mState,
    //         (fullyOpenedPanel!=NULL)?" fullyOpened":"", fullyClosed?" fullyClosed":"");

    return NOERROR;
}

ECode PanelBar::CollapseAllPanels(
    /* [in] */ Boolean animate)
{
    Boolean waiting = FALSE, isFullyCollapsed = FALSE;
    List<AutoPtr<IPanelView> >::Iterator it = mPanels.Begin();
    for (; it != mPanels.End(); ++it) {
        AutoPtr<IPanelView> pv = *it;
        if (animate) pv->IsFullyCollapsed(&isFullyCollapsed);
        if (animate && !isFullyCollapsed) {
            pv->Collapse();
            waiting = TRUE;
        }
        else {
            pv->SetExpandedFraction(0); // just in case
            pv->SetVisibility(IView::GONE);
        }
    }

    // if (DEBUG) LOG("collapseAllPanels: animate=%s waiting=%s", animate, waiting);

    if (!waiting && mState != IPanelBar::STATE_CLOSED) {
        // it's possible that nothing animated, so we replicate the termination
        // conditions of panelExpansionChanged here
        Go(IPanelBar::STATE_CLOSED);
        OnAllPanelsCollapsed();
    }
    return NOERROR;
}

ECode PanelBar::OnPanelPeeked()
{
    // if (DEBUG) LOG("onPanelPeeked");
    return NOERROR;
}

ECode PanelBar::OnAllPanelsCollapsed()
{
    // if (DEBUG) LOG("onAllPanelsCollapsed");
    return NOERROR;
}

ECode PanelBar::OnPanelFullyOpened(
    /* [in] */ IPanelView* panelView)
{
    // if (DEBUG) LOG("onPanelFullyOpened");
    return NOERROR;
}

ECode PanelBar::OnTrackingStarted(
    /* [in] */ IPanelView* panel)
{
    mTracking = TRUE;
    // if (DEBUG && panel != mTouchingPanel) {
    //     LOG("shouldn't happen: onTrackingStarted(%s) != mTouchingPanel(%s)",
    //             panel, mTouchingPanel);
    // }
    return NOERROR;
}

ECode PanelBar::OnTrackingStopped(
    /* [in] */ IPanelView* panel)
{
    assert(panel);

    mTracking = FALSE;
    Float frac;
    panel->GetExpandedFraction(&frac);
    PanelExpansionChanged(panel, frac);
    return NOERROR;
}


}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
