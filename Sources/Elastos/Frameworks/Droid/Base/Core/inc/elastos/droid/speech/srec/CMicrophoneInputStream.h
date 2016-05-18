#ifndef __ELASTOS_DROID_SPEECH_SREC_CMicrophoneInputStream_H__
#define __ELASTOS_DROID_SPEECH_SREC_CMicrophoneInputStream_H__

#include "_Elastos_Droid_Speech_Srec_CMicrophoneInputStream.h"
#include "elastos/droid/speech/srec/MicrophoneInputStream.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Srec {

CarClass(CMicrophoneInputStream)
    , public MicrophoneInputStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Srec
} // namespace Speech
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_SREC_CMicrophoneInputStream_H__
