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

#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_FIRSTFRAMEANIMATORHELPER_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_FIRSTFRAMEANIMATORHELPER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include <elastos/core/Runnable.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Core::Runnable;
using Elastos::Droid::View::IOnDrawListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewPropertyAnimator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

/*
 *  This is a helper class that listens to updates from the corresponding animation.
 *  For the first two frames, it adjusts the current play time of the animation to
 *  prevent jank at the beginning of the animation
 */
class FirstFrameAnimatorHelper
    : public AnimatorListenerAdapter
    , public IAnimatorUpdateListener
{
private:
    class MyViewTreeObserverOnDrawListener
        : public Object
        , public IOnDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyViewTreeObserverOnDrawListener();

        // @Override
        CARAPI OnDraw();

    private:
        FirstFrameAnimatorHelper* mHost;
        Int64 mTime;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ IValueAnimator* animation,
            /* [in] */ FirstFrameAnimatorHelper* host);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<IValueAnimator> mAnimation;
        AutoPtr<FirstFrameAnimatorHelper> mHost;    // hold host
    };

public:
    CAR_INTERFACE_DECL()

    FirstFrameAnimatorHelper(
        /* [in] */ IValueAnimator* animator,
        /* [in] */ IView* target);

    FirstFrameAnimatorHelper(
        /* [in] */ IViewPropertyAnimator* vpa,
        /* [in] */ IView* target);

    // only used for ViewPropertyAnimators
    CARAPI OnAnimationStart(
        /* [in] */ IAnimator* animation);

    static CARAPI_(void) InitializeDrawListener(
        /* [in] */ IView* view);

    CARAPI OnAnimationUpdate(
        /* [in] */ IValueAnimator* animation);

    CARAPI Print(
        /* [in] */ IValueAnimator* animation);

private:
    static const Boolean DEBUG;
    static const Int32 MAX_DELAY = 1000;
    static const Int32 IDEAL_FRAME_DURATION = 16;
    AutoPtr<IView> mTarget;
    Int64 mStartFrame;
    Int64 mStartTime;
    Boolean mHandlingOnAnimationUpdate;
    Boolean mAdjustedSecondFrameTime;

    static AutoPtr<IOnDrawListener> sGlobalDrawListener;
    static Int64 sGlobalFrameCounter;
};

} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_RECENT_FIRSTFRAMEANIMATORHELPER_H__