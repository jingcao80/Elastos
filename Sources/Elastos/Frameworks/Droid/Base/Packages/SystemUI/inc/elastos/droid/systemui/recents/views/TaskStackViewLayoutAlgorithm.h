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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKSTACKVIEWLAYOUTALGORITHM_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKSTACKVIEWLAYOUTALGORITHM_H__

#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include "elastos/droid/systemui/recents/views/TaskViewTransform.h"
#include "elastos/droid/systemui/recents/model/Task.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::SystemUI::Recents::Model::ITaskKey;
using Elastos::Droid::SystemUI::Recents::Model::Task;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/* The layout logic for a TaskStackView.
 *
 * We are using a curve that defines the curve of the tasks as that go back in the recents list.
 * The curve is defined such that at curve progress p = 0 is the end of the curve (the top of the
 * stack rect), and p = 1 at the start of the curve and the bottom of the stack rect.
 */
class TaskStackViewLayoutAlgorithm
    : public Object
    , public ITaskStackViewLayoutAlgorithm
{
public:
    CAR_INTERFACE_DECL()

    TaskStackViewLayoutAlgorithm(
        /* [in] */ RecentsConfiguration* config);

    /** Computes the stack and task rects */
    CARAPI_(void) ComputeRects(
        /* [in] */ Int32 windowWidth,
        /* [in] */ Int32 windowHeight,
        /* [in] */ IRect* taskStackBounds);

    /** Computes the minimum and maximum scroll progress values.  This method may be called before
     * the RecentsConfiguration is set, so we need to pass in the alt-tab state. */
    CARAPI_(void) ComputeMinMaxScroll(
        /* [in] */ IArrayList* tasks,
        /* [in] */ Boolean launchedWithAltTab,
        /* [in] */ Boolean launchedFromHome);

    /** Update/get the transform */
    CARAPI_(AutoPtr<TaskViewTransform>) GetStackTransform(
        /* [in] */ Task* task,
        /* [in] */ Float stackScroll,
        /* [in] */ TaskViewTransform* transformOut,
        /* [in] */ TaskViewTransform* prevTransform);

    /** Update/get the transform */
    CARAPI_(AutoPtr<TaskViewTransform>) GetStackTransform(
        /* [in] */ Float taskProgress,
        /* [in] */ Float stackScroll,
        /* [in] */ TaskViewTransform* transformOut,
        /* [in] */ TaskViewTransform* prevTransform);

    /**
     * Returns the untransformed task view size.
     */
    CARAPI_(AutoPtr<IRect>) GetUntransformedTaskViewSize();

    /**
     * Returns the scroll to such task top = 1.0f;
     */
    CARAPI_(Float) GetStackScrollForTask(
        /* [in] */ Task* t);

    /** Initializes the curve. */
    static CARAPI_(void) InitializeCurve();

    /** Reverses and scales out x. */
    static CARAPI_(Float) Reverse(
        /* [in] */ Float x);

    /** The log function describing the curve. */
    static CARAPI_(Float) LogFunc(
        /* [in] */ Float x);

    /** The inverse of the log function describing the curve. */
    CARAPI_(Float) InvLogFunc(
        /* [in] */ Float y);

    /** Converts from the progress along the curve to a screen coordinate. */
    CARAPI_(Int32) CurveProgressToScreenY(
        /* [in] */ Float p);

    /** Converts from the progress along the curve to a scale. */
    CARAPI_(Float) CurveProgressToScale(
        /* [in] */ Float p);

    /** Converts from a screen coordinate to the progress along the curve. */
    CARAPI_(Float) ScreenYToCurveProgress(
        /* [in] */ Int32 screenY);

public:
    // These are all going to change
    static const Float StackPeekMinScale = 0.8f; // The min scale of the last card in the peek area

    AutoPtr<RecentsConfiguration> mConfig;

    // The various rects that define the stack view
    AutoPtr<IRect> mViewRect;
    AutoPtr<IRect> mStackVisibleRect;
    AutoPtr<IRect> mStackRect;
    AutoPtr<IRect> mTaskRect;

    // The min/max scroll progress
    Float mMinScrollP;
    Float mMaxScrollP;
    Float mInitialScrollP;
    Int32 mWithinAffiliationOffset;
    Int32 mBetweenAffiliationOffset;
    HashMap<AutoPtr<ITaskKey>, Float> mTaskProgressMap;

    // Log function
    static const Float XScale = 1.75f;  // The large the XScale, the longer the flat area of the curve
    static const Float LogBase = 3000;
    static const Int32 PrecisionSteps = 250;
    static AutoPtr<ArrayOf<Float> > sXp;
    static AutoPtr<ArrayOf<Float> > sPx;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKSTACKVIEWLAYOUTALGORITHM_H__
