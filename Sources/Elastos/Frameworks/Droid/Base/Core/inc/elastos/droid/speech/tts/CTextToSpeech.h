
#ifndef __ELASTOS_DROID_SPEECH_TTS_CTEXTTOSPEECH_H__
#define __ELASTOS_DROID_SPEECH_TTS_CTEXTTOSPEECH_H__

#include "_Elastos_Droid_Speech_Tts_CTextToSpeech.h"
#include "elastos/droid/speech/tts/TextToSpeech.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

CarClass(CTextToSpeech)
    , public TextToSpeech
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Tts
} // namespace Speech
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_CTEXTTOSPEECH_H__
