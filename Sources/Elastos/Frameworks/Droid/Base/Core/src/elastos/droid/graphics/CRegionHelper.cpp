
#include "elastos/droid/graphics/CRegionHelper.h"
#include "elastos/droid/graphics/CRegion.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_INTERFACE_IMPL(CRegionHelper, Singleton, IRegionHelper)

CAR_SINGLETON_IMPL(CRegionHelper)

ECode CRegionHelper::Obtain(
    /* [out] */ IRegion** region)
{
    VALIDATE_NOT_NULL(region)
    *region = CRegion::Obtain();
    REFCOUNT_ADD(*region)
    return NOERROR;
}

ECode CRegionHelper::Obtain(
    /* [in] */ IRegion* other,
    /* [out] */ IRegion** region)
{
    VALIDATE_NOT_NULL(region)
    *region = CRegion::Obtain(other);
    REFCOUNT_ADD(*region)
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
