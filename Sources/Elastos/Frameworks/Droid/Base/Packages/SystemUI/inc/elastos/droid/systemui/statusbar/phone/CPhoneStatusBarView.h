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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPHONESTATUSBARVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPHONESTATUSBARVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CPhoneStatusBarView.h"
#include "elastos/droid/systemui/statusbar/phone/PhoneStatusBarTransitions.h"
#include "elastos/droid/systemui/statusbar/phone/PanelBar.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class CNotificationPanelView;

CarClass(CPhoneStatusBarView)
    , public PanelBar
    , public IPhoneStatusBarView
{
private:
    class Runnable1: public Runnable
    {
    public:
        Runnable1(
            /* [in] */ CPhoneStatusBarView* host);

        // @Override
        CARAPI Run();

    private:
        CPhoneStatusBarView* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI GetBarTransitions(
        /* [out] */ IBarTransitions** result);

    CARAPI SetBar(
        /* [in] */ IPhoneStatusBar* bar);

    CARAPI SetScrimController(
        /* [in] */ IScrimController* scrimController);

    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI AddPanel(
        /* [in] */ IPanelView* pv);

    // @Override
    CARAPI PanelsEnabled(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnRequestSendAccessibilityEvent(
        /* [in] */ IView* child,
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SelectPanelForTouch(
        /* [in] */ IMotionEvent* touch,
        /* [out] */ IPanelView** view);

    // @Override
    CARAPI OnPanelPeeked();

    // @Override
    CARAPI OnAllPanelsCollapsed();

    // @Override
    CARAPI OnPanelFullyOpened(
        /* [in] */ IPanelView* openPanel);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnTrackingStarted(
        /* [in] */ IPanelView* panel);

    // @Override
    CARAPI OnTrackingStopped(
        /* [in] */ IPanelView* panel,
        /* [in] */ Boolean expand);

    // @Override
    CARAPI OnExpandingFinished();

    // @Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI PanelExpansionChanged(
        /* [in] */ IPanelView* panel,
        /* [in] */ Float frac,
        /* [in] */ Boolean expanded);

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean DEBUG_GESTURES;

    AutoPtr<IPhoneStatusBar> mBar;

    AutoPtr<IPanelView> mLastFullyOpenedPanel;
    AutoPtr<IPanelView> mNotificationPanel;
    AutoPtr<PhoneStatusBarTransitions> mBarTransitions;
    AutoPtr<IScrimController> mScrimController;
    friend class CNotificationPanelView;
};

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPHONESTATUSBARVIEW_H__
