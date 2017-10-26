//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/systemui/statusbar/phone/PanelBar.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::IViewGroup;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

const String PanelBar::TAG("PanelBar");
CAR_INTERFACE_IMPL(PanelBar, FrameLayout, IPanelBar)
PanelBar::PanelBar()
    : mPanelExpandedFractionSum(0)
    , mState(IPanelBar::STATE_CLOSED)
    , mTracking(FALSE)
{
}

ECode PanelBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return FrameLayout::constructor(context, attrs);
}

ECode PanelBar::Go(
    /* [in] */ Int32 state)
{
    // if (DEBUG) LOG("go state: %d -> %d", mState, state);
    mState = state;
    return NOERROR;
}

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
        Logger::E(TAG, "SetPanelHolder: NULL PanelHolder");
        return E_NULL_POINTER_EXCEPTION;
    }

    ph->SetBar(this);
    mPanelHolder = ph;
    Int32 N;
    IViewGroup::Probe(ph)->GetChildCount(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> v;
        IViewGroup::Probe(ph)->GetChildAt(i, (IView**)&v);
        IPanelView* pv = IPanelView::Probe(v.Get());
        if (pv != NULL) {
            AddPanel(pv);
        }
    }
    return NOERROR;
}

ECode PanelBar::GetBarHeight(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 h = 0;
    GetMeasuredHeight(&h);
    *result = h;
    return NOERROR;
}

ECode PanelBar::SelectPanelForTouch(
    /* [in] */ IMotionEvent* touch,
    /* [out] */ IPanelView** view)
{
    VALIDATE_NOT_NULL(view);
    Int32 N = mPanels.GetSize();
    Float x;
    touch->GetX(&x);
    Int32 w = 0;
    GetMeasuredWidth(&w);
    Int32 index = (Int32)(N * x / w);
    *view = mPanels[index];
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode PanelBar::PanelsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = TRUE;
    return NOERROR;
}

ECode PanelBar::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 action;
    event->GetAction(&action);
    // Allow subclasses to implement enable/disable semantics
    Boolean enabled;
    if (PanelsEnabled(&enabled), !enabled) {
        if (action == IMotionEvent::ACTION_DOWN) {
            Float x = 0, y = 0;
            event->GetX(&x);
            event->GetY(&y);
            Logger::V(TAG, "onTouch: all panels disabled, ignoring touch at (%d,%d)",
                    (Int32) x, (Int32) y);
        }
        *result = FALSE;
        return NOERROR;
    }

    // figure out which panel needs to be talked to here
    if (action == IMotionEvent::ACTION_DOWN) {
        AutoPtr<IPanelView> panel;
        SelectPanelForTouch(event, (IPanelView**)&panel);
        if (panel == NULL) {
            // panel is not there, so we'll eat the gesture
            Float x = 0, y = 0;
            event->GetX(&x);
            event->GetY(&y);
            Logger::V(TAG, "onTouch: no panel for touch at (%d,%d)", (Int32) x, (Int32) y);
            mTouchingPanel = NULL;
            *result = TRUE;
            return NOERROR;
        }
        Boolean enabled;
        IView::Probe(panel)->IsEnabled(&enabled);
        // if (DEBUG) LOG("PanelBar.onTouch: state=%d ACTION_DOWN: panel %s %s", mState, panel,
        //         (enabled ? "" : " (disabled)"));
        if (!enabled) {
            // panel is disabled, so we'll eat the gesture
            Float x = 0, y = 0;
            event->GetX(&x);
            event->GetY(&y);
            Logger::V(TAG, "onTouch: panel (%s) is disabled, ignoring touch at (%d,%d)",
                TO_CSTR(panel), (Int32) x, (Int32) y);
            mTouchingPanel = NULL;
            *result = TRUE;
            return NOERROR;
        }
        StartOpeningPanel(panel);
    }

    Boolean tmp = TRUE;
    if (mTouchingPanel != NULL) {
        IView::Probe(mTouchingPanel)->OnTouchEvent(event, &tmp);
    }
    *result = tmp;
    return NOERROR;
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
            pv->Collapse(FALSE /* delayed */);
        }
    }
    return NOERROR;
}

ECode PanelBar::PanelExpansionChanged(
    /* [in] */ IPanelView* panel,
    /* [in] */ Float frac,
    /* [in] */ Boolean expanded)
{
    Boolean fullyClosed = TRUE;
    AutoPtr<IPanelView> fullyOpenedPanel;
    // if (DEBUG) LOG("panelExpansionChanged: start state=%d panel=%s", mState, panel.getName());
    mPanelExpandedFractionSum = 0.0f;
    Boolean visible;
    Float exHeight, thisFrac;

    List<AutoPtr<IPanelView> >::Iterator it = mPanels.Begin();
    for (; it != mPanels.End(); ++it) {
        AutoPtr<IPanelView> pv = *it;

        Float tmp = 0;
        visible = (pv->GetExpandedHeight(&tmp), tmp) > 0;
        IView::Probe(pv)->SetVisibility(visible ? IView::VISIBLE : IView::GONE);

        // adjust any other panels that may be partially visible
        pv->GetExpandedHeight(&exHeight);
        if (expanded) {
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
        if (animate && (pv->IsFullyCollapsed(&isFullyCollapsed), !isFullyCollapsed)) {
            pv->Collapse(TRUE /* delayed */);
            waiting = TRUE;
        }
        else {
            pv->ResetViews();
            pv->SetExpandedFraction(0); // just in case
            IView::Probe(pv)->SetVisibility(IView::GONE);
            pv->CancelPeek();
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
    /* [in] */ IPanelView* panel,
    /* [in] */ Boolean expanded)
{
    mTracking = FALSE;
    return NOERROR;
}

ECode PanelBar::OnExpandingFinished()
{
    return NOERROR;
}

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
