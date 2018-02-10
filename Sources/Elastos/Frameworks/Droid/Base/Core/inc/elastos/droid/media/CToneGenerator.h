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

#ifndef __ELASTOS_DROID_MEDIA_CTONEGENERATOR_H__
#define __ELASTOS_DROID_MEDIA_CTONEGENERATOR_H__

#include "_Elastos_Droid_Media_CToneGenerator.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
/**
 * This class provides methods to play DTMF tones (ITU-T Recommendation Q.23),
 * call supervisory tones (3GPP TS 22.001, CEPT) and proprietary tones (3GPP TS 31.111).
 * Depending on call state and routing options, tones are mixed to the downlink audio
 * or output to the speaker phone or headset.
 * This API is not for generating tones over the uplink audio path.
 */
CarClass(CToneGenerator)
    , public Object
    , public IToneGenerator
{
public:
    CToneGenerator();

    ~CToneGenerator();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 volume);

    CARAPI StartTone(
        /* [in] */ Int32 toneType,
        /* [out] */ Boolean* result);

    CARAPI StartTone(
        /* [in] */ Int32 toneType,
        /* [in] */ Int32 durationMs,
        /* [out] */ Boolean* result);

    /**
     * This method stops the tone currently playing playback.
     * @see #ToneGenerator(int, int)
     */
    CARAPI StopTone();

    /**
     * Releases resources associated with this ToneGenerator object. It is good
     * practice to call this method when you're done using the ToneGenerator.
     */
    CARAPI ReleaseResources();

    /**
    * Returns the audio session ID.
    *
    * @return the ID of the audio session this ToneGenerator belongs to or 0 if an error
    * occured.
    */
    CARAPI GetAudioSessionId(
        /* [out] */ Int32* result);

private:
    CARAPI NativeSetup(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 volume);

    CARAPI_(void) NativeFinalize();

private:
    HANDLE mNativeContext; // accessed by native methods
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CTONEGENERATOR_H__
