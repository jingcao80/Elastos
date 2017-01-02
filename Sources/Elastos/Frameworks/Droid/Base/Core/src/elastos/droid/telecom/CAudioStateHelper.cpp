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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/CAudioStateHelper.h"
#include "elastos/droid/telecom/AudioState.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// CAudioStateHelper::
//===============================================================
CAR_SINGLETON_IMPL(CAudioStateHelper)

CAR_INTERFACE_IMPL(CAudioStateHelper, Singleton, IAudioStateHelper)

ECode CAudioStateHelper::AudioRouteToString(
    /* [in] */ Int32 route,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = AudioState::AudioRouteToString(route);
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos