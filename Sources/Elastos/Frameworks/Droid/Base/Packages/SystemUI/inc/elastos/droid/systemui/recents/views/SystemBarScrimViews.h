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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_SYSTEMBARSCRIMVIEWS_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_SYSTEMBARSCRIMVIEWS_H__

#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Runnable.h>

using Elastos::Droid::App::IActivity;
using Elastos::Core::Runnable;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/** Manages the scrims for the various system bars. */
class SystemBarScrimViews : public Object
{
private:
    class StartAcitonRunnable : public Runnable
    {
    public:
        StartAcitonRunnable(
            /* [in] */ IView* view);

        CARAPI Run();

    private:
        AutoPtr<IView> mView;
    };

public:
    SystemBarScrimViews(
        /* [in] */ IActivity* activity,
        /* [in] */ RecentsConfiguration* config);

    /**
     * Prepares the scrim views for animating when entering Recents. This will be called before
     * the first draw.
     */
    CARAPI_(void) PrepareEnterRecentsAnimation();

    /**
     * Starts animating the scrim views when entering Recents.
     */
    CARAPI_(void) StartEnterRecentsAnimation();

    /**
     * Starts animating the scrim views when leaving Recents (either via launching a task, or
     * going home).
     */
    CARAPI_(void) StartExitRecentsAnimation();

public:
    AutoPtr<RecentsConfiguration> mConfig;

    AutoPtr<IView> mStatusBarScrimView;
    AutoPtr<IView> mNavBarScrimView;

    Boolean mHasNavBarScrim;
    Boolean mShouldAnimateStatusBarScrim;
    Boolean mHasStatusBarScrim;
    Boolean mShouldAnimateNavBarScrim;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_SYSTEMBARSCRIMVIEWS_H__
