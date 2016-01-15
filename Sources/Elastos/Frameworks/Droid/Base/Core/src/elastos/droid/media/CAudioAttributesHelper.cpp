#include "elastos/droid/media/CAudioAttributesHelper.h"
#include "elastos/droid/media/CAudioAttributes.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioAttributesHelper, Singleton, IAudioAttributesHelper)

CAR_SINGLETON_IMPL(CAudioAttributesHelper)

ECode CAudioAttributesHelper::UsageToString(
    /* [in] */ Int32 usage,
    /* [out] */ String* result)
{
    return CAudioAttributes::UsageToString(usage, result);
}

ECode CAudioAttributesHelper::UsageForLegacyStreamType(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* result)
{
    return CAudioAttributes::UsageForLegacyStreamType(streamType, result);
}

ECode CAudioAttributesHelper::ToLegacyStreamType(
    /* [in] */ IAudioAttributes* aa,
    /* [out] */ Int32* result)
{
    return CAudioAttributes::ToLegacyStreamType(aa, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
