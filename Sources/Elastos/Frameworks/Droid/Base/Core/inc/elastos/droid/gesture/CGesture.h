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

#ifndef __ELASTOS_DROID_GESTURE_CGESTURE_H__
#define __ELASTOS_DROID_GESTURE_CGESTURE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/gesture/Gesture.h"

namespace Elastos {
namespace Droid {
namespace Gesture {

CarClass(CGesture)
    , public Gesture
{
public:
    CAR_OBJECT_DECL()

    CGesture();

    virtual ~CGesture();
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_CGESTURE_H__
