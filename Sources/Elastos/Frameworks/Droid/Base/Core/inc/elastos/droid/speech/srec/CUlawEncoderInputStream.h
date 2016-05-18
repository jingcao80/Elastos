
#ifndef __ELASTOS_DROID_SPEECH_SREC_CULAWENCODERINPUTSTREAM_H__
#define __ELASTOS_DROID_SPEECH_SREC_CULAWENCODERINPUTSTREAM_H__

#include "_Elastos_Droid_Speech_Srec_CUlawEncoderInputStream.h"
#include "elastos/droid/speech/srec/UlawEncoderInputStream.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Srec {

CarClass(CUlawEncoderInputStream)
    , public UlawEncoderInputStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Srec
} // namespace Speech
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_SREC_CULAWENCODERINPUTSTREAM_H__
