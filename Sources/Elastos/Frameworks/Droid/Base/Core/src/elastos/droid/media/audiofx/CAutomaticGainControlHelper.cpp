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
#include "elastos/droid/media/audiofx/CAutomaticGainControlHelper.h"
#include "elastos/droid/media/audiofx/CAutomaticGainControl.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CAR_INTERFACE_IMPL(CAutomaticGainControlHelper, Singleton, IAutomaticGainControlHelper)

CAR_SINGLETON_IMPL(CAutomaticGainControlHelper)

/**
 * Checks if the device implements acoustic echo cancellation.
 * @return true if the device implements acoustic echo cancellation, false otherwise.
 */
ECode CAutomaticGainControlHelper::IsAvailable(
    /* [out] */ Boolean* result)
{
    return CAutomaticGainControl::IsAvailable(result);
}

ECode CAutomaticGainControlHelper::Create(
    /* [in] */ Int32 audioSession,
    /* [out] */ IAutomaticGainControl** result)
{
    return CAutomaticGainControl::New(audioSession, result);
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
