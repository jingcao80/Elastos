
#include "elastos/droid/speech/srec/CUlawEncoderInputStreamHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/speech/srec/UlawEncoderInputStream.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Srec {

CAR_SINGLETON_IMPL(CUlawEncoderInputStreamHelper)

CAR_INTERFACE_IMPL(CUlawEncoderInputStreamHelper, Singleton, IUlawEncoderInputStreamHelper)

ECode CUlawEncoderInputStreamHelper::Encode(
    /* [in] */ ArrayOf<Byte>* pcmBuf,
    /* [in] */ Int32 pcmOffset,
    /* [in] */ ArrayOf<Byte>* ulawBuf,
    /* [in] */ Int32 ulawOffset,
    /* [in] */ Int32 length,
    /* [in] */ Int32 max)
{
    return UlawEncoderInputStream::Encode(pcmBuf, pcmOffset, ulawBuf, ulawOffset, length, max);
}

ECode CUlawEncoderInputStreamHelper::MaxAbsPcm(
    /* [in] */ ArrayOf<Byte>* pcmBuf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* ret)
{
    return UlawEncoderInputStream::MaxAbsPcm(pcmBuf, offset, length, ret);
}

} // namespace Srec
} // namespace Speech
} // namespace Droid
} // namespace Elastos

