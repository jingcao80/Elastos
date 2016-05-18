
#ifndef __ELASTOS_DROID_SPEECH_TTS_CBlockingAudioTrack_h__
#define __ELASTOS_DROID_SPEECH_TTS_CBlockingAudioTrack_h__

#include "_Elastos_Droid_Speech_Tts_CBlockingAudioTrack.h"
#include "elastos/droid/speech/tts/BlockingAudioTrack.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

CarClass(CBlockingAudioTrack)
    , public BlockingAudioTrack
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Tts
} // namespace Speech
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_CBlockingAudioTrack_h__
