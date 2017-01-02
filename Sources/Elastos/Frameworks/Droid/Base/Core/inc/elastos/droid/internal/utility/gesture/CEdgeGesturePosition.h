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

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_GESTURE_CEDGEGESTUREPOSITION_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_GESTURE_CEDGEGESTUREPOSITION_H__

#include "_Elastos_Droid_Internal_Utility_Gesture_CEdgeGesturePosition.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Gesture {

CarClass(CEdgeGesturePosition)
    , public Object
    , public IEdgeGesturePosition
{
public:
    CEdgeGesturePosition();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 index,
        /* [in] */ Int32 factor);

    CARAPI GetIndex(
        /* [out] */ Int32* index);

    CARAPI GetFlag(
        /* [out] */ Int32* flag);

    CARAPI GetFactor(
        /* [out] */ Int32* factor);

    static AutoPtr<ArrayOf<IEdgeGesturePosition*> > Values();

public:
    const static AutoPtr<IEdgeGesturePosition> LEFT;
    const static AutoPtr<IEdgeGesturePosition> BOTTOM;
    const static AutoPtr<IEdgeGesturePosition> RIGHT;
    const static AutoPtr<IEdgeGesturePosition> TOP;

private:
    Int32 mIndex;
    Int32 mFlag;
    /**
     * This is 1 when the position is not at the axis (like {@link EdgeGesturePosition.RIGHT} is
     * at {@code Layout.getWidth()} not at {@code 0}).
     */
    Int32 mFactor;
};

} // namespace Gesture
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_GESTURE_CEDGEGESTUREPOSITION_H__
