#include "elastos/providers/mediaprovider/media/CMediaThumbRequestHelper.h"
#include "elastos/providers/mediaprovider/media/MediaThumbRequest.h"

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

CAR_OBJECT_IMPL(CMediaThumbRequestHelper)

CAR_INTERFACE_IMPL(CMediaThumbRequestHelper, Object, IMediaThumbRequestHelper)

CMediaThumbRequestHelper::CMediaThumbRequestHelper()
{}

CMediaThumbRequestHelper::~CMediaThumbRequestHelper()
{}

ECode CMediaThumbRequestHelper::GetComparator(
    /* [out] */ IComparator** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<MediaThumbRequest::MediaThumbRequestComparator> mtr = new MediaThumbRequest::MediaThumbRequestComparator();
    *result = IComparator::Probe(mtr);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos