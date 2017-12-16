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

#ifndef __ELASTOS_DROID_VIEW_FRAMEINFO_H__
#define __ELASTOS_DROID_VIEW_FRAMEINFO_H__

#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/Object.h>

using Elastos::Core::ISystem;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

class FrameInfo
    : public Object
{
public:
    FrameInfo();

    CARAPI_(void) SetVsync(
        /* [in] */ Int64 intendedVsync,
        /* [in] */ Int64 usedVsync);

    CARAPI_(void) UpdateInputEventTime(
        /* [in] */ Int64 inputEventTime,
        /* [in] */ Int64 inputEventOldestTime);

    CARAPI_(void) MarkInputHandlingStart();

    CARAPI_(void) MarkAnimationsStart();

    CARAPI_(void) MarkPerformTraversalsStart();

    CARAPI_(void) MarkDrawStart();

    CARAPI_(void) AddFlags(
        /* [in] */ Int64 flags);

public:
    AutoPtr< ArrayOf<Int64> > mFrameInfo;

    // Is this the first-draw following a window layout?
    static const Int64 FLAG_WINDOW_LAYOUT_CHANGED = 1;

private:
    // Various flags set to provide extra metadata about the current frame
    static const Int32 FLAGS = 0;

    // The intended vsync time, unadjusted by jitter
    static const Int32 INTENDED_VSYNC = 1;

    // Jitter-adjusted vsync time, this is what was used as input into the
    // animation & drawing system
    static const Int32 VSYNC = 2;

    // The time of the oldest input event
    static const Int32 OLDEST_INPUT_EVENT = 3;

    // The time of the newest input event
    static const Int32 NEWEST_INPUT_EVENT = 4;

    // When input event handling started
    static const Int32 HANDLE_INPUT_START = 5;

    // When animation evaluations started
    static const Int32 ANIMATION_START = 6;

    // When ViewRootImpl#performTraversals() started
    static const Int32 PERFORM_TRAVERSALS_START = 7;

    // When View:draw() started
    static const Int32 DRAW_START = 8;

    AutoPtr<ISystem> mSystem;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_FRAMEINFO_H__
