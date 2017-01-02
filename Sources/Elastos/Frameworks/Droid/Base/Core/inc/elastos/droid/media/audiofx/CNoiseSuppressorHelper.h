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

#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_CNOISESUPPRESSORHELPER_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_CNOISESUPPRESSORHELPER_H__

#include "_Elastos_Droid_Media_Audiofx_CNoiseSuppressorHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CarClass(CNoiseSuppressorHelper)
    , public Singleton
    , public INoiseSuppressorHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Checks if the device implements noise suppression.
     * @return true if the device implements noise suppression, false otherwise.
     */
    CARAPI IsAvailable(
        /* [out] */ Boolean* result);

    /**
     * Creates a NoiseSuppressor and attaches it to the AudioRecord on the audio
     * session specified.
     * @param audioSession system wide unique audio session identifier. The NoiseSuppressor
     * will be applied to the AudioRecord with the same audio session.
     * @return NoiseSuppressor created or null if the device does not implement noise
     * suppression.
     */
    CARAPI Create(
        /* [in] */ Int32 audioSession,
        /* [out] */ INoiseSuppressor** result);
};

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_AUDIOFX_CNOISESUPPRESSORHELPER_H__

