
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CTableMaskFilterHelper.h"
#include "elastos/droid/graphics/CTableMaskFilter.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_SINGLETON_IMPL(CTableMaskFilterHelper);
CAR_INTERFACE_IMPL(CTableMaskFilterHelper, Singleton, ITableMaskFilterHelper);
ECode CTableMaskFilterHelper::CreateClipTable(
    /* [in] */ Int32 min,
    /* [in] */ Int32 max,
    /* [out] */ ITableMaskFilter** filter)
{
    VALIDATE_NOT_NULL(filter);

    return CTableMaskFilter::CreateClipTable(min, max, filter);
}

ECode CTableMaskFilterHelper::CreateGammaTable(
    /* [in] */ Float gamma,
    /* [out] */ ITableMaskFilter** filter)
{
    VALIDATE_NOT_NULL(filter);

    return CTableMaskFilter::CreateGammaTable(gamma, filter);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
