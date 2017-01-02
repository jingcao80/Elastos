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

#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_GESTUREUTILS_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_GESTUREUTILS_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

class GestureUtils
{
public:
    static CARAPI_(Boolean) IsTap(
        /* [in] */ IMotionEvent* down,
        /* [in] */ IMotionEvent* up,
        /* [in] */ Int32 tapTimeSlop,
        /* [in] */ Int32 tapDistanceSlop,
        /* [in] */ Int32 actionIndex);

    static CARAPI_(Boolean) IsMultiTap(
        /* [in] */ IMotionEvent* firstUp,
        /* [in] */ IMotionEvent* secondUp,
        /* [in] */ Int32 multiTapTimeSlop,
        /* [in] */ Int32 multiTapDistanceSlop,
        /* [in] */ Int32 actionIndex);

    static CARAPI_(Double) ComputeDistance(
        /* [in] */ IMotionEvent* first,
        /* [in] */ IMotionEvent* second,
        /* [in] */ Int32 pointerIndex);

    static CARAPI_(Boolean) IsTimedOut(
        /* [in] */ IMotionEvent* firstUp,
        /* [in] */ IMotionEvent* secondUp,
        /* [in] */ Int32 timeout);

    static CARAPI_(Boolean) IsSamePointerContext(
        /* [in] */ IMotionEvent* first,
        /* [in] */ IMotionEvent* second);

    /**
     * Determines whether a two pointer gesture is a dragging one.
     *
     * @param event The event with the pointer data.
     * @return True if the gesture is a dragging one.
     */
    static CARAPI_(Boolean) IsDraggingGesture(
        /* [in] */ Float firstPtrDownX,
        /* [in] */ Float firstPtrDownY,
        /* [in] */ Float secondPtrDownX,
        /* [in] */ Float secondPtrDownY,
        /* [in] */ Float firstPtrX,
        /* [in] */ Float firstPtrY,
        /* [in] */ Float secondPtrX,
        /* [in] */ Float secondPtrY,
        /* [in] */ Float maxDraggingAngleCos);

private:
    GestureUtils() {
        /* cannot be instantiated */
    }

    static CARAPI_(Boolean) EventsWithinTimeAndDistanceSlop(
        /* [in] */ IMotionEvent* first,
        /* [in] */ IMotionEvent* second,
        /* [in] */ Int32 timeout,
        /* [in] */ Int32 distance,
        /* [in] */ Int32 actionIndex);
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_GESTUREUTILS_H__
