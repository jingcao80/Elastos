
#ifndef __ELASTOS_DROID_SPEECH_TTS_CFileSynthesisCallback_h__
#define __ELASTOS_DROID_SPEECH_TTS_CFileSynthesisCallback_h__

#include "_Elastos_Droid_Speech_Tts_CFileSynthesisCallback.h"
#include "elastos/droid/speech/tts/FileSynthesisCallback.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

CarClass(CFileSynthesisCallback)
    , public FileSynthesisCallback
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Tts
} // namespace Speech
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_CFileSynthesisCallback_h__
