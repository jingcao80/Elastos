
#ifndef __ELASTOS_DROID_SPEECH_SREC_CULAWENCODERINPUTSTREAMHELPER_H__
#define __ELASTOS_DROID_SPEECH_SREC_CULAWENCODERINPUTSTREAMHELPER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>
#include "_Elastos_Droid_Speech_Srec_CUlawEncoderInputStreamHelper.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Srec {

CarClass(CUlawEncoderInputStreamHelper)
    , public Singleton
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL()

    CARAPI Encode(
        /* [in] */ ArrayOf<Byte>* pcmBuf,
        /* [in] */ Int32 pcmOffset,
        /* [in] */ ArrayOf<Byte>* ulawBuf,
        /* [in] */ Int32 ulawOffset,
        /* [in] */ Int32 length,
        /* [in] */ Int32 max);

    CARAPI MaxAbsPcm(
        /* [in] */ ArrayOf<Byte>* pcmBuf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* ret);
};

} // namespace Srec
} // namespace Speech
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_SREC_CULAWENCODERINPUTSTREAMHELPER_H__
