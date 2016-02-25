#include "elastos/droid/media/tv/CTvContractPrograms.h"
#include "elastos/droid/media/tv/TvContractPrograms.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_SINGLETON_IMPL(CTvContractPrograms)
CAR_INTERFACE_IMPL(CTvContractPrograms, Singleton, ITvContractPrograms);

ECode CTvContractPrograms::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    return TvContractPrograms::GetCONTENT_URI(uri);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
