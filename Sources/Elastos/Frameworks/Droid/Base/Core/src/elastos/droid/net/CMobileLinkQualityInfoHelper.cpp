
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CMobileLinkQualityInfoHelper.h"
#include "elastos/droid/net/MobileLinkQualityInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CMobileLinkQualityInfoHelper, Singleton, IMobileLinkQualityInfoHelper)

CAR_SINGLETON_IMPL(CMobileLinkQualityInfoHelper)

ECode CMobileLinkQualityInfoHelper::CreateFromParcelBody(
        /* [in] */ IParcel* parcel,
        /* [out] */ IMobileLinkQualityInfo** result)
{
    return MobileLinkQualityInfo::CreateFromParcelBody(parcel, result);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos

