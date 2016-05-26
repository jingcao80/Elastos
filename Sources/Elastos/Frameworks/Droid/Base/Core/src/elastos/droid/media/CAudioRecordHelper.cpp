
#include "elastos/droid/media/CAudioRecordHelper.h"
#include "elastos/droid/media/CAudioRecord.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioRecordHelper, Singleton, IAudioRecordHelper)

CAR_SINGLETON_IMPL(CAudioRecordHelper)

ECode CAudioRecordHelper::GetMinBufferSize(
    /* [in] */ Int32 sampleRateInHz,
    /* [in] */ Int32 channelConfig,
    /* [in] */ Int32 audioFormat,
    /* [out] */ Int32* result)
{
    return CAudioRecord::GetMinBufferSize(sampleRateInHz, channelConfig, audioFormat, result);
}

} // namespace Media
} // namespace Droid
} // namespace Elastos
