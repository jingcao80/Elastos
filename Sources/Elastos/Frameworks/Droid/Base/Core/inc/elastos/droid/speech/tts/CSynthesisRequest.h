
#ifndef __ELASTOS_DROID_SPEECH_TTS_CSynthesisRequest_H__
#define __ELASTOS_DROID_SPEECH_TTS_CSynthesisRequest_H__

#include "_Elastos_Droid_Speech_Tts_CSynthesisRequest.h"
#include "SynthesisRequest.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

CarClass(CSynthesisRequest)
    , public SynthesisRequest
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Tts
} // namespace Speech
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_CSynthesisRequest_H__
