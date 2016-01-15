
#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/tv/TvContractWatchedPrograms.h"
#include "elastos/droid/net/CUriHelper.h"

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

TvContractWatchedPrograms::TvContractWatchedPrograms()
{
}

ECode TvContractWatchedPrograms::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    return uriHelper->Parse(String("content://") + ITvContract::AUTHORITY + "/watched_program", uri);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
