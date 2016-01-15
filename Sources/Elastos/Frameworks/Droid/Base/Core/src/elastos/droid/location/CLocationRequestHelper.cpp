
#include "elastos/droid/location/CLocationRequestHelper.h"

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL(CLocationRequestHelper, Singleton, ILocationRequestHelper)

CAR_SINGLETON_IMPL(CLocationRequestHelper)

ECode CLocationRequestHelper::Create(
    /* [out] */ ILocationRequest** lr)
{
    VALIDATE_NOT_NULL(lr)
    AutoPtr<ILocationRequest> request = LocationRequest::Create();
    *lr = request.Get();
    REFCOUNT_ADD(*lr)
    return NOERROR;
}

ECode CLocationRequestHelper::CreateFromDeprecatedProvider(
    /* [in] */ const String& provider,
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ Boolean singleShot,
    /* [out] */ ILocationRequest** lr)
{
    VALIDATE_NOT_NULL(lr)
    AutoPtr<ILocationRequest> request = LocationRequest::CreateFromDeprecatedProvider(provider,minTime,minDistance,singleShot);
    *lr = request.Get();
    REFCOUNT_ADD(*lr)
    return NOERROR;
}

ECode CLocationRequestHelper::CreateFromDeprecatedCriteria(
    /* [in] */ ICriteria* criteria,
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ Boolean singleShot,
    /* [out] */ ILocationRequest** lr)
{
    VALIDATE_NOT_NULL(lr)
    AutoPtr<ILocationRequest> request = LocationRequest::CreateFromDeprecatedCriteria(criteria, minTime,minDistance,singleShot);
    *lr = request.Get();
    REFCOUNT_ADD(*lr)
    return NOERROR;
}

ECode CLocationRequestHelper::QualityToString(
    /* [in] */ Int32 quality,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = LocationRequest::QualityToString(quality);
    return NOERROR;
}

} // namespace Location
} // namespace Droid
} // namespace Elastos