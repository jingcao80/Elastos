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

#ifndef __ELASTOS_DROID_VIEW_SOUNDEFFECTCONSTANT_H__
#define __ELASTOS_DROID_VIEW_SOUNDEFFECTCONSTANT_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace View {

class SoundEffectConstants
    : public Object
{
public:
    static const Int32 CLICK = 0;
    static const Int32 NAVIGATION_LEFT = 1;
    static const Int32 NAVIGATION_UP = 2;
    static const Int32 NAVIGATION_RIGHT = 3;
    static const Int32 NAVIGATION_DOWN = 4;

    /**
     * Get the sonification constant for the focus directions.
     * @param direction One of {@link View#FOCUS_UP}, {@link View#FOCUS_DOWN},
     *     {@link View#FOCUS_LEFT}, {@link View#FOCUS_RIGHT}, {@link View#FOCUS_FORWARD}
     *     or {@link View#FOCUS_BACKWARD}

     * @return The appropriate sonification constant.
     */
    static Int32 GetContantForFocusDirection(
        /* [in] */ Int32 direction);

private:
    SoundEffectConstants();
    SoundEffectConstants(const SoundEffectConstants&);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_SOUNDEFFECTCONSTANT_H__

