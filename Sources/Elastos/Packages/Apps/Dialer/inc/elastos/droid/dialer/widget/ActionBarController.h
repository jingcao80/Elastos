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

#ifndef __ELASTOS_DROID_DIALER_WIDGET_ACTIONBARCONTROLLER_H__
#define __ELASTOS_DROID_DIALER_WIDGET_ACTIONBARCONTROLLER_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Animation.h"
#include "elastos/droid/dialer/widget/CSearchEditTextLayout.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Widget {

/**
 * Controls the various animated properties of the actionBar: showing/hiding, fading/revealing,
 * and collapsing/expanding, and assigns suitable properties to the actionBar based on the
 * current state of the UI.
 */
class ActionBarController
    : public Object
{
private:
    class MyAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyAnimatorUpdateListener(
            /* [in] */ ActionBarController* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        ActionBarController* mHost;
    };

public:
    ActionBarController(
        /* [in] */ IActionBarControllerActivityUi* activityUi,
        /* [in] */ CSearchEditTextLayout* searchBox);

    /**
     * @return Whether or not the action bar is currently showing (both slid down and visible)
     */
    CARAPI_(Boolean) IsActionBarShowing();

    /**
     * Called when the user has tapped on the collapsed search box, to start a new search query.
     */
    CARAPI_(void) OnSearchBoxTapped();

    /**
     * Called when search UI has been exited for some reason.
     */
    CARAPI_(void) OnSearchUiExited();

    /**
     * Called to indicate that the user is trying to hide the dialpad. Should be called before
     * any state changes have actually occurred.
     */
    CARAPI_(void) OnDialpadDown();

    /**
     * Called to indicate that the user is trying to show the dialpad. Should be called before
     * any state changes have actually occurred.
     */
    CARAPI_(void) OnDialpadUp();

    CARAPI_(void) SlideActionBar(
        /* [in] */ Boolean slideUp,
        /* [in] */ Boolean animate);

    CARAPI_(void) SetAlpha(
        /* [in] */ Float alphaValue);

    CARAPI_(void) SetHideOffset(
        /* [in] */ Int32 offset);

    /**
     * @return The offset the action bar is being translated upwards by
     */
    CARAPI_(Int32) GetHideOffset();

    /**
     * Saves the current state of the action bar into a provided {@link Bundle}
     */
    CARAPI_(void) SaveInstanceState(
        /* [in] */ IBundle* outState);

    /**
     * Restores the action bar state from a provided {@link Bundle}.
     */
    CARAPI_(void) RestoreInstanceState(
        /* [in] */ IBundle* inState);

    /**
     * This should be called after onCreateOptionsMenu has been called, when the actionbar has
     * been laid out and actually has a height.
     */
    CARAPI_(void) RestoreActionBarOffset();

    // @VisibleForTesting
    CARAPI_(Boolean) GetIsActionBarSlidUp();

public:
    static const Boolean DEBUG;
    static const String TAG;

private:
    static const String KEY_IS_SLID_UP;
    static const String KEY_IS_FADED_OUT;
    static const String KEY_IS_EXPANDED;

    AutoPtr<IActionBarControllerActivityUi> mActivityUi;
    AutoPtr<CSearchEditTextLayout> mSearchBox;

    Boolean mIsActionBarSlidUp;

    // AutoPtr<IAnimationCallback> mFadeOutCallback;
};

} // Widget
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_WIDGET_ACTIONBARCONTROLLER_H__
