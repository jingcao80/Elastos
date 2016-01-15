
#include "elastos/droid/graphics/CRectHelper.h"
#include "elastos/droid/graphics/CRect.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_SINGLETON_IMPL(CRectHelper);
CAR_INTERFACE_IMPL(CRectHelper, Singleton, IRectHelper);
ECode CRectHelper::UnflattenFromString(
    /* [in] */ const String& str,
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);
    AutoPtr<IRect> r = CRect::UnflattenFromString(str);
    *rect = r;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

ECode CRectHelper::Intersects(
    /* [in] */ IRect* a,
    /* [in] */ IRect* b,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CRect::Intersects(a, b);
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
