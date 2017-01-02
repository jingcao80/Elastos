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

#ifndef __ELASTOS_DROID_GESTURE_GESTUREPOINT_H__
#define __ELASTOS_DROID_GESTURE_GESTUREPOINT_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Gesture.h"
#include "elastos/core/Object.h"

using Elastos::IO::IDataInputStream;
using Elastos::Droid::Gesture::IGesturePoint;
using Elastos::Droid::Gesture::IGesturePointHelper;

namespace Elastos {
namespace Droid {
namespace Gesture {

class GesturePoint
    : public Object
    , public IGesturePoint
    , public IGesturePointHelper
{
public:
    CAR_INTERFACE_DECL()

    GesturePoint();

    virtual ~GesturePoint();

    CARAPI constructor(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Int64 t);

    CARAPI GetX(
        /* [out] */ Float * x);

    CARAPI GetY(
        /* [out] */ Float * y);

    CARAPI GetTimestamp(
        /* [out] */ Int64 * timestamp);

    static CARAPI_(AutoPtr<IGesturePoint>) Deserialize(
        /* [in] */ IDataInputStream *in);

    CARAPI Deserialize(
        /* [in] */ IDataInputStream *in,
        /* [out] */ IGesturePoint **instance);

private:
    Float mX;
    Float mY;

    Int64 mTimestamp;
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_GESTUREPOINT_H__
