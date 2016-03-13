#include "elastos/droid/media/EncoderCapabilities.h"
#include "elastos/droid/media/CEncoderCapabilities.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CEncoderCapabilities)
CAR_INTERFACE_IMPL(CEncoderCapabilities, Singleton, IEncoderCapabilities);

ECode CEncoderCapabilities::GetOutputFileFormats(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    return EncoderCapabilities::GetOutputFileFormats(result);
}

ECode CEncoderCapabilities::GetVideoEncoders(
    /* [out] */ IList** result)
{
    return EncoderCapabilities::GetVideoEncoders(result);
}

ECode CEncoderCapabilities::GetAudioEncoders(
    /* [out] */ IList** result)
{
    return EncoderCapabilities::GetAudioEncoders(result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
