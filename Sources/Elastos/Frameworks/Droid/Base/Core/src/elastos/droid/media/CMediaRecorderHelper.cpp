
#include "elastos/droid/media/CMediaRecorderHelper.h"
#include "elastos/droid/media/CMediaRecorder.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaRecorderHelper, Singleton, IMediaRecorderHelper)

CAR_SINGLETON_IMPL(CMediaRecorderHelper)

ECode CMediaRecorderHelper::GetAudioSourceMax(
    /* [out] */ Int32* result)
{
    return CMediaRecorder::GetAudioSourceMax(result);
}

} // namespace Media
} // namespace Droid
} // namespace Elastos
