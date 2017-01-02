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

#include "elastos/droid/media/audiofx/CAcousticEchoCancelerHelper.h"
#include "elastos/droid/media/audiofx/CAcousticEchoCanceler.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CAR_INTERFACE_IMPL(CAcousticEchoCancelerHelper, Singleton, IAcousticEchoCancelerHelper)

CAR_SINGLETON_IMPL(CAcousticEchoCancelerHelper)

/**
 * Checks if the device implements acoustic echo cancellation.
 * @return true if the device implements acoustic echo cancellation, false otherwise.
 */
ECode CAcousticEchoCancelerHelper::IsAvailable(
    /* [out] */ Boolean* result)
{
    return CAcousticEchoCanceler::IsAvailable(result);
}

/**
 * Creates an AcousticEchoCanceler and attaches it to the AudioRecord on the audio
 * session specified.
 * @param audioSession system wide unique audio session identifier. The AcousticEchoCanceler
 * will be applied to the AudioRecord with the same audio session.
 * @return AcousticEchoCanceler created or null if the device does not implement AEC.
 */
ECode CAcousticEchoCancelerHelper::Create(
    /* [in] */ Int32 audioSession,
    /* [out] */ IAcousticEchoCanceler** result)
{
    return CAcousticEchoCanceler::New(audioSession, result);
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
