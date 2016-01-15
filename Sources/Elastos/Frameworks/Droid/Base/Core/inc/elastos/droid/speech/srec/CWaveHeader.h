#ifndef __ELASTOS_DROID_SPEECH_SREC_CWAVEHEADER_H__
#define __ELASTOS_DROID_SPEECH_SREC_CWAVEHEADER_H__

#include "_Elastos_Droid_Speech_Srec_CWaveHeader.h"
#include "elastos/droid/speech/srec/WaveHeader.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Srec {

CarClass(CWaveHeader)
    , public WaveHeader
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Srec
} // namespace Speech
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_SREC_CWAVEHEADER_H__
