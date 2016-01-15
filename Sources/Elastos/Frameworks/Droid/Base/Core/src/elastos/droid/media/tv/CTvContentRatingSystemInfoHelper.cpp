#include "elastos/droid/media/tv/CTvContentRatingSystemInfoHelper.h"
#include "elastos/droid/media/tv/CTvContentRatingSystemInfo.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_INTERFACE_IMPL(CTvContentRatingSystemInfoHelper, Singleton, ITvContentRatingSystemInfoHelper)

CAR_SINGLETON_IMPL(CTvContentRatingSystemInfoHelper)

ECode CTvContentRatingSystemInfoHelper::CreateTvContentRatingSystemInfo(
    /* [in] */ Int32 xmlResourceId,
    /* [in] */ IApplicationInfo * applicationInfo,
    /* [out] */ ITvContentRatingSystemInfo ** result)
{
    return CTvContentRatingSystemInfo::CreateTvContentRatingSystemInfo(xmlResourceId, applicationInfo, result);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
