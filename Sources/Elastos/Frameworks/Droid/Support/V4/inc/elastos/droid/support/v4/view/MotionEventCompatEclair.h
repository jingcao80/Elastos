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

#ifndef __ELASTOS_DROID_SUPPORT_V4_VIEW_MOTIONEVENTCOMPATECLAIR_H__
#define __ELASTOS_DROID_SUPPORT_V4_VIEW_MOTIONEVENTCOMPATECLAIR_H__

#include "_Elastos.Droid.Support.V4.View.h"
#include "Elastos.Droid.View.h"
#include <elastos/coredef.h>

using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace Support {
namespace V4 {
namespace View {

/**
 * Implementation of motion event compatibility that can call Eclair APIs.
 */
class MotionEventCompatEclair
{
public:
    static CARAPI FindPointerIndex(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 pointerId,
        /* [out] */ Int32* index);

    static CARAPI GetPointerId(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 pointerIndex,
        /* [out] */ Int32* id);

    static CARAPI GetX(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 pointerIndex,
        /* [out] */ Float* x);

    static CARAPI GetY(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 pointerIndex,
        /* [out] */ Float* y);

    static CARAPI GetPointerCount(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Int32* count);
};

} // namespace View
} // namespace V4
} // namespace Support
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SUPPORT_V4_VIEW_MOTIONEVENTCOMPATECLAIR_H__