
#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/tv/TvContractPrograms.h"
#include "elastos/droid/media/tv/TvContract.h"
#include "elastos/droid/net/CUriHelper.h"

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

TvContractPrograms::TvContractPrograms()
{
}

ECode TvContractPrograms::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    return uriHelper->Parse(String("content://") +
            ITvContract::AUTHORITY + "/" + TvContract::PATH_PROGRAM, uri);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
