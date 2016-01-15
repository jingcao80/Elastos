
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/res/CCompatibilityInfoHelper.h"
#include "elastos/droid/content/res/CCompatibilityInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL(CCompatibilityInfoHelper, Singleton, ICompatibilityInfoHelper)

CAR_SINGLETON_IMPL(CCompatibilityInfoHelper)

ECode CCompatibilityInfoHelper::ComputeCompatibleScaling(
    /* [in] */ IDisplayMetrics* dm,
    /* [in] */ IDisplayMetrics* outDm,
    /* [out] */ Float* scale)
{
    VALIDATE_NOT_NULL(scale);
    *scale = CCompatibilityInfo::ComputeCompatibleScaling(dm, outDm);
    return NOERROR;
}

ECode CCompatibilityInfoHelper::GetDefault(
    /* [out] */ ICompatibilityInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = CCompatibilityInfo::DEFAULT_COMPATIBILITY_INFO;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

