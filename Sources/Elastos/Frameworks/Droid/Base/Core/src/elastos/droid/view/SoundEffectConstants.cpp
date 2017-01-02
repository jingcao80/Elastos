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

#include "Elastos.Droid.View.h"
#include "elastos/droid/view/SoundEffectConstants.h"

using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace View {

const Int32 SoundEffectConstants::CLICK;
const Int32 SoundEffectConstants::NAVIGATION_LEFT;
const Int32 SoundEffectConstants::NAVIGATION_UP;
const Int32 SoundEffectConstants::NAVIGATION_RIGHT;
const Int32 SoundEffectConstants::NAVIGATION_DOWN;

Int32 SoundEffectConstants::GetContantForFocusDirection(
    /* [in] */ Int32 direction)
{
    switch (direction) {
        case IView::FOCUS_RIGHT:
            return SoundEffectConstants::NAVIGATION_RIGHT;
        case IView::FOCUS_FORWARD:
        case IView::FOCUS_DOWN:
            return SoundEffectConstants::NAVIGATION_DOWN;
        case IView::FOCUS_LEFT:
            return SoundEffectConstants::NAVIGATION_LEFT;
        case IView::FOCUS_BACKWARD:
        case IView::FOCUS_UP:
            return SoundEffectConstants::NAVIGATION_UP;
        default:
            assert(0 && "Invalid direction.");
            return -1;
    }
}

} // namespace View
} // namespace Droid
} // namespace Elastos
