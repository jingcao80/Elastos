#include "elastos/droid/media/tv/CTvContractChannels.h"
#include "elastos/droid/media/tv/TvContractChannels.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_SINGLETON_IMPL(CTvContractChannels)
CAR_INTERFACE_IMPL(CTvContractChannels, Singleton, ITvContractChannels);

ECode CTvContractChannels::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    return TvContractChannels::GetCONTENT_URI(uri);
}

ECode CTvContractChannels::GetVideoResolution(
    /* [in] */ const String& videoFormat,
    /* [out] */ String* result)
{
    return TvContractChannels::GetVideoResolution(videoFormat, result);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
