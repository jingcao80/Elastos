
#ifndef __ELASTOS_DROID_SPEECH_SREC_CRECOGNIZER_H__
#define __ELASTOS_DROID_SPEECH_SREC_CRECOGNIZER_H__

#include "_Elastos_Droid_Speech_Srec_CRecognizer.h"
#include "elastos/droid/speech/srec/Recognizer.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Srec {

CarClass(CRecognizer)
    , public Recognizer
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Srec
} // namespace Speech
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_SREC_CRECOGNIZER_H__
