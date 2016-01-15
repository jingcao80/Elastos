
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
    , IToneGenerator
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
    Handle32 mNativeContext; // accessed by native methods
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CTONEGENERATOR_H__
