
#include "elastos/droid/media/CEncoderCapabilitiesHelper.h"
#include "elastos/droid/media/CEncoderCapabilities.h"

namespace Elastos {
namespace Droid {
namespace Media {

ECode CEncoderCapabilitiesHelper::GetOutputFileFormats(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    return CEncoderCapabilities::GetOutputFileFormats(result);
}

ECode CEncoderCapabilitiesHelper::GetVideoEncoders(
    /* [out] */ IObjectContainer** result)
{
    return CEncoderCapabilities::GetVideoEncoders(result);
}

ECode CEncoderCapabilitiesHelper::GetAudioEncoders(
    /* [out] */ IObjectContainer** result)
{
    return CEncoderCapabilities::GetAudioEncoders(result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
