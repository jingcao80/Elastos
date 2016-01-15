#include "elastos/droid/media/CDecoderCapabilities.h"
#include "elastos/droid/media/DecoderCapabilities.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CDecoderCapabilities)
CAR_INTERFACE_IMPL(CDecoderCapabilities, Singleton, IDecoderCapabilities);

ECode CDecoderCapabilities::GetVideoDecoders(
    /* [out, callee] */ ArrayOf<VideoDecoder>** result)
{
    return DecoderCapabilities::GetVideoDecoders(result);
}

ECode CDecoderCapabilities::GetAudioDecoders(
    /* [out, callee] */ ArrayOf<AudioDecoder>** result)
{
    return DecoderCapabilities::GetAudioDecoders(result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
