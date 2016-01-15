
#ifndef __ELASTOS_DROID_SPEECH_CSPEECHRECOGNIZER_H__
#define __ELASTOS_DROID_SPEECH_CSPEECHRECOGNIZER_H__

#include "_Elastos_Droid_Speech_CSpeechRecognizer.h"
#include "elastos/droid/speech/SpeechRecognizer.h"

namespace Elastos {
namespace Droid {
namespace Speech {

CarClass(CSpeechRecognizer)
    , public SpeechRecognizer
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Speech
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_CSPEECHRECOGNIZER_H__
