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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PANELBAR_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PANELBAR_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class PanelBar
    : public FrameLayout
    , public IPanelBar
{
public:
    CAR_INTERFACE_DECL()

    PanelBar();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Go(
        /* [in] */ Int32 state);

    CARAPI AddPanel(
        /* [in] */ IPanelView* panelView);

    CARAPI SetPanelHolder(
        /* [in] */ IPanelHolder* ph);

    CARAPI GetBarHeight(
        /* [out] */ Float* result);

    CARAPI SelectPanelForTouch(
        /* [in] */ IMotionEvent* touch,
        /* [out] */ IPanelView** view);

    CARAPI PanelsEnabled(
        /* [out] */ Boolean* enabled);

    // called from PanelView when self-expanding, too
    CARAPI StartOpeningPanel(
        /* [in] */ IPanelView* panelView);

    /**
     * @param panel the panel which changed its expansion state
     * @param frac the fraction from the expansion in [0, 1]
     * @param expanded whether the panel is currently expanded; this is independent from the
     *                 fraction as the panel also might be expanded if the fraction is 0
     */
    CARAPI PanelExpansionChanged(
        /* [in] */ IPanelView* panelView,
        /* [in] */ Float frac,
        /* [in] */ Boolean expanded);

    CARAPI CollapseAllPanels(
        /* [in] */ Boolean animate);

    CARAPI OnPanelPeeked();

    CARAPI OnAllPanelsCollapsed();

    CARAPI OnPanelFullyOpened(
        /* [in] */ IPanelView* panelView);

    CARAPI OnTrackingStarted(
        /* [in] */ IPanelView* panelView);

    CARAPI OnTrackingStopped(
        /* [in] */ IPanelView* panelView,
        /* [in] */ Boolean expanded);

    CARAPI OnExpandingFinished();

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

protected:
    //@Override
    virtual CARAPI OnFinishInflate();

protected:
    AutoPtr<IPanelHolder> mPanelHolder;
    List<AutoPtr<IPanelView> > mPanels;
    AutoPtr<IPanelView> mTouchingPanel;
    Float mPanelExpandedFractionSum;

private:
    static const String TAG;
    Int32 mState;//= STATE_CLOSED;
    Boolean mTracking;
};


}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PANELBAR_H__
