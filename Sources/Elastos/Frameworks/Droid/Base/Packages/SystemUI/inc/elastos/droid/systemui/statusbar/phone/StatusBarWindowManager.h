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
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * Encapsulates all logic for the status bar window state management.
 */
class StatusBarWindowManager
    : public Object
    , public IStatusBarWindowManager
{
private:
    class State: public Object
    {
    public:
        State();

        CARAPI_(Boolean) IsKeyguardShowingAndNotOccluded();

    public:
        Boolean mKeyguardShowing;
        Boolean mKeyguardOccluded;
        Boolean mKeyguardNeedsInput;
        Boolean mStatusBarExpanded;
        Boolean mStatusBarFocusable;
        Int64 mKeyguardUserActivityTimeout;
        Boolean mBouncerShowing;
        Boolean mKeyguardFadingAway;
        Boolean mQsExpanded;

        /**
         * The {@link BaseStatusBar} state from the status bar.
         */
        Int32 mStatusBarState;
    };

public:
    CAR_INTERFACE_DECL()

    StatusBarWindowManager(
        /* [in] */ IContext* context);

    /**
     * Adds the status bar view to the window manager.
     *
     * @param statusBarView The view to add.
     * @param barHeight The height of the status bar in collapsed state.
     */
    CARAPI Add(
        /* [in] */ IView* statusBarView,
        /* [in] */ Int32 barHeight);

    CARAPI SetKeyguardShowing(
        /* [in] */ Boolean showing);

    CARAPI SetKeyguardOccluded(
        /* [in] */ Boolean occluded);

    CARAPI SetKeyguardNeedsInput(
        /* [in] */ Boolean needsInput);

    CARAPI SetStatusBarExpanded(
        /* [in] */ Boolean expanded);

    CARAPI SetStatusBarFocusable(
        /* [in] */ Boolean focusable);

    CARAPI SetKeyguardUserActivityTimeout(
        /* [in] */ Int64 timeout);

    CARAPI SetBouncerShowing(
        /* [in] */ Boolean showing);

    CARAPI SetKeyguardFadingAway(
        /* [in] */ Boolean keyguardFadingAway);

    CARAPI SetQsExpanded(
        /* [in] */ Boolean expanded);

    /**
     * @param state The {@link StatusBarState} of the status bar.
     */
    CARAPI SetStatusBarState(
        /* [in] */ Int32 state);

private:
    CARAPI_(Boolean) ShouldEnableKeyguardScreenRotation();

    CARAPI_(void) ApplyKeyguardFlags(
        /* [in] */ State* state);

    CARAPI_(void) AdjustScreenOrientation(
        /* [in] */ State* state);

    CARAPI_(void) ApplyFocusableFlag(
        /* [in] */ State* state);

    CARAPI_(void) ApplyHeight(
        /* [in] */ State* state);

    CARAPI_(void) ApplyFitsSystemWindows(
        /* [in] */ State* state);

    CARAPI_(void) ApplyUserActivityTimeout(
        /* [in] */ State* state);

    CARAPI_(void) ApplyInputFeatures(
        /* [in] */ State* state);

    CARAPI_(void) Apply(
        /* [in] */ State* state);

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IWindowManager> mWindowManager;
    AutoPtr<IView> mStatusBarView;
    AutoPtr<IWindowManagerLayoutParams> mLp;
    Int32 mBarHeight;
    Boolean mKeyguardScreenRotation;

    AutoPtr<State> mCurrentState;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_KEYGUARDBOUNCER_H__
