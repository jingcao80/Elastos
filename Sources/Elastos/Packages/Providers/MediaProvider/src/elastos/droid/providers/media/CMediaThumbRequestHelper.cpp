#include "elastos/droid/providers/media/CMediaThumbRequestHelper.h"
#include "elastos/droid/providers/media/MediaThumbRequest.h"

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

CAR_SINGLETON_IMPL(CMediaThumbRequestHelper)

CAR_INTERFACE_IMPL(CMediaThumbRequestHelper, Singleton, IMediaThumbRequestHelper)

ECode CMediaThumbRequestHelper::GetComparator(
    /* [out] */ IComparator** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<MediaThumbRequest::MediaThumbRequestComparator> mtr = new MediaThumbRequest::MediaThumbRequestComparator();
    *result = IComparator::Probe(mtr);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos