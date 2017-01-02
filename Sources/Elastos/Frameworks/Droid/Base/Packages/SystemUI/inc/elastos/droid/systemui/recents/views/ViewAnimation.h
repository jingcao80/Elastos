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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_VIEWANIMATION_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_VIEWANIMATION_H__

#include "elastos/droid/systemui/recents/views/TaskViewTransform.h"
#include "elastos/droid/systemui/recents/misc/ReferenceCountedTrigger.h"

using Elastos::Droid::SystemUI::Recents::Misc::ReferenceCountedTrigger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/* Common code related to view animations */
class ViewAnimation : public Object
{
public:
    /* The animation context for a task view animation into Recents */
    class TaskViewEnterContext : public Object
    {
    public:
        TaskViewEnterContext(
            /* [in] */ ReferenceCountedTrigger* t)
            : mPostAnimationTrigger(t)
            , mCurrentTaskOccludesLaunchTarget(FALSE)
            , mCurrentStackViewIndex(0)
            , mCurrentStackViewCount(0)
        {
        }

    public:
        // A trigger to run some logic when all the animations complete.  This works around the fact
        // that it is difficult to coordinate ViewPropertyAnimators
        AutoPtr<ReferenceCountedTrigger> mPostAnimationTrigger;
        // An update listener to notify as the enter animation progresses (used for the home transition)
        AutoPtr<IAnimatorUpdateListener> mUpdateListener;

        // These following properties are updated for each task view we start the enter animation on

        // Whether or not the current task occludes the launch target
        Boolean mCurrentTaskOccludesLaunchTarget;
        // The task rect for the current stack
        AutoPtr<IRect> mCurrentTaskRect;
        // The transform of the current task view
        AutoPtr<TaskViewTransform> mCurrentTaskTransform;
        // The view index of the current task view
        Int32 mCurrentStackViewIndex;
        // The total number of task views
        Int32 mCurrentStackViewCount;
    };

    /* The animation context for a task view animation out of Recents */
    class TaskViewExitContext : public Object
    {
    public:
        TaskViewExitContext(
            /* [in] */ ReferenceCountedTrigger* t)
            : mPostAnimationTrigger(t)
            , mOffscreenTranslationY(0)
        {
        }

    public:
        // A trigger to run some logic when all the animations complete.  This works around the fact
        // that it is difficult to coordinate ViewPropertyAnimators
        AutoPtr<ReferenceCountedTrigger> mPostAnimationTrigger;

        // The translationY to apply to a TaskView to move it off the bottom of the task stack
        Int32 mOffscreenTranslationY;
    };
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_VIEWANIMATION_H__
