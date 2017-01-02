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

#ifndef __ELASTOS_DROID_SYSTEMUI_QS_QSDETAILCLIPPER_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_QSDETAILCLIPPER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Graphics::Drawable::ITransitionDrawable;
using Elastos::Core::Runnable;
using Elastos::Droid::View::IView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

/** Helper for quick settings detail panel clip animations. **/
class QSDetailClipper: public Object
{
private:
    class ReverseBackground: public Runnable
    {
    public:
        ReverseBackground(
            /* [in] */ QSDetailClipper* host);

        // @Override
        CARAPI Run();

    private:
        QSDetailClipper* mHost;
    };

    class VisibleOnStart: public AnimatorListenerAdapter
    {
    public:
        VisibleOnStart(
            /* [in] */ QSDetailClipper* host);

        // @Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        QSDetailClipper* mHost;
    };

    class GoneOnEnd: public AnimatorListenerAdapter
    {
    public:
        GoneOnEnd(
            /* [in] */ QSDetailClipper* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        QSDetailClipper* mHost;
    };

public:
    QSDetailClipper(
        /* [in] */ IView* detail);

    CARAPI_(void) AnimateCircularClip(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Boolean in,
        /* [in] */ IAnimatorListener* listener);

private:
    AutoPtr<IView> mDetail;
    AutoPtr<ITransitionDrawable> mBackground;

    AutoPtr<IAnimator> mAnimator;

    AutoPtr<IRunnable> mReverseBackground;
    AutoPtr<VisibleOnStart> mVisibleOnStart;
    AutoPtr<GoneOnEnd> mGoneOnEnd;
};

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_QSDETAILCLIPPER_H__
