#include "elastos/droid/media/tv/CTvContractWatchedPrograms.h"
#include "elastos/droid/media/tv/TvContractWatchedPrograms.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_SINGLETON_IMPL(CTvContractWatchedPrograms)
CAR_INTERFACE_IMPL(CTvContractWatchedPrograms, Singleton, ITvContractWatchedPrograms);

ECode CTvContractWatchedPrograms::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    return TvContractWatchedPrograms::GetCONTENT_URI(uri);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
