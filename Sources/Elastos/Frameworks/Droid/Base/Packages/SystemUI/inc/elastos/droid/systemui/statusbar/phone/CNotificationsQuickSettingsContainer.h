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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDINDICATIONTEXTVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDINDICATIONTEXTVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CNotificationsQuickSettingsContainer.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::View::IViewStub;
using Elastos::Droid::View::IViewStubOnInflateListener;
using Elastos::Droid::View::IWindowInsets;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * The container with notification stack scroller and quick settings inside.
 */
CarClass(CNotificationsQuickSettingsContainer)
    , public FrameLayout
    , public INotificationsQuickSettingsContainer
{
private:
    class InnerListener
        : public Object
        , public IViewStubOnInflateListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CNotificationsQuickSettingsContainer* host);

        // @Override
        CARAPI OnInflate(
            /* [in] */ IViewStub* stub,
            /* [in] */ IView* inflated);
    private:
        CNotificationsQuickSettingsContainer* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CNotificationsQuickSettingsContainer();

    // @Override
    CARAPI OnApplyWindowInsets(
        /* [in] */ IWindowInsets* insets,
        /* [out] */ IWindowInsets** result);

    // @Override
    CARAPI OnInflate(
        /* [in] */ IViewStub* stub,
        /* [in] */ IView* inflated);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI_(Boolean) DrawChild(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IView* child,
        /* [in] */ Int64 drawingTime);

private:
    AutoPtr<IView> mScrollView;
    AutoPtr<IView> mUserSwitcher;
    AutoPtr<IView> mStackScroller;
    AutoPtr<IView> mKeyguardStatusBar;
    Boolean mInflated;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDINDICATIONTEXTVIEW_H__
