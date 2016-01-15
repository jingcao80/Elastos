
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CPointFHelper.h"
#include "elastos/droid/graphics/CPointF.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_SINGLETON_IMPL(CPointFHelper);
CAR_INTERFACE_IMPL(CPointFHelper, Singleton, IPointFHelper);
ECode CPointFHelper::Length(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [out] */ Float* length)
{
    VALIDATE_NOT_NULL(length);

    *length = CPointF::Length(x, y);
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
