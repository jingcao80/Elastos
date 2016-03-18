#include "elastos/providers/mediaprovider/media/CMediaThumbRequestHelper.h"

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

CAR_INTERFACE_IMPL(CMediaThumbRequestHelper, Object, IMediaThumbRequestHelper)

CAR_OBJECT_IMPL(CMediaThumbRequestHelper)

CMediaThumbRequestHelper::CMediaThumbRequestHelper(
    /* [in] */ IMediaThumbRequest* r1,
    /* [in] */ IMediaThumbRequest* r2)
    : mR1(r1)
    , mR2(r2)
{}

ECode CMediaThumbRequestHelper::GetComparator(
    /* [out] */ IComparator** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IMediaThumbRequest> r1;
    AutoPtr<IMediaThumbRequest> r2;
    AutoPtr<MediaThumbRequestComparator> mtr = new MediaThumbRequestComparator(r1, r2);
    *result = IComparator::Probe(mtr);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos