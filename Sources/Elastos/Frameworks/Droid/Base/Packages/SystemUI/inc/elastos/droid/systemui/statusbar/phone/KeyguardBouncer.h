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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_KEYGUARDBOUNCER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_KEYGUARDBOUNCER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "_Elastos.Droid.Core.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Core::Runnable;
using Elastos::Droid::View::IChoreographer;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::SystemUI::Keyguard::IViewMediatorCallback;
using Elastos::Droid::SystemUI::Keyguard::IKeyguardViewBase;
using Elastos::Droid::SystemUI::Keyguard::IKeyguardHostViewOnDismissAction;
using Elastos::Core::IRunnable;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * A class which manages the bouncer on the lockscreen.
 */
class KeyguardBouncer
    : public Object
    , public IKeyguardBouncer
{
private:
    class ShowRunnable
        : public Runnable
    {
    public:
        ShowRunnable(
            /* [in] */ KeyguardBouncer* host);

        // @Override
        CARAPI Run();

    private:
        KeyguardBouncer* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    KeyguardBouncer();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IViewMediatorCallback* callback,
        /* [in] */ ILockPatternUtils* lockPatternUtils,
        /* [in] */ IStatusBarWindowManager* windowManager,
        /* [in] */ IViewGroup* container);

    CARAPI Show();

    CARAPI ShowWithDismissAction(
        /* [in] */ IKeyguardHostViewOnDismissAction* r);

    CARAPI Hide(
        /* [in] */ Boolean destroyView);

    /**
     * See {@link StatusBarKeyguardViewManager#startPreHideAnimation}.
     */
    CARAPI StartPreHideAnimation(
        /* [in] */ IRunnable* runnable);

    /**
     * Reset the state of the view.
     */
    CARAPI Reset();

    CARAPI OnScreenTurnedOff();

    CARAPI GetUserActivityTimeout(
        /* [out] */ Int64* result);

    CARAPI IsShowing(
        /* [out] */ Boolean* value);

    CARAPI Prepare();

    CARAPI OnBackPressed(
        /* [out] */ Boolean* value);

    /**
     * @return True if and only if the current security method should be shown before showing
     *         the notifications on Keyguard, like SIM PIN/PUK.
     */
    CARAPI NeedsFullscreenBouncer(
        /* [out] */ Boolean* value);

    CARAPI IsSecure(
        /* [out] */ Boolean* value);

    CARAPI OnMenuPressed(
        /* [out] */ Boolean* value);

    CARAPI InterceptMediaKey(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* value);

private:
    CARAPI CancelShowRunnable();

    CARAPI EnsureView();

    CARAPI InflateView();

    CARAPI RemoveView();

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IViewMediatorCallback> mCallback;
    AutoPtr<ILockPatternUtils> mLockPatternUtils;
    AutoPtr<IViewGroup> mContainer;
    AutoPtr<IStatusBarWindowManager> mWindowManager;
    AutoPtr<IKeyguardViewBase> mKeyguardView;
    AutoPtr<IViewGroup> mRoot;
    Boolean mShowingSoon;
    AutoPtr<IChoreographer> mChoreographer;
    AutoPtr<IRunnable> mShowRunnable;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_KEYGUARDBOUNCER_H__
