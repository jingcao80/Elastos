
#include "elastos/droid/internal/widget/multiwaveview/CTweenerHelper.h"
#include "elastos/droid/internal/widget/multiwaveview/Tweener.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {
namespace Multiwaveview {

//=====================================================================
//               CTweenerHelper::
//=====================================================================
CAR_INTERFACE_IMPL(CTweenerHelper, Singleton, ITweenerHelper)

CAR_SINGLETON_IMPL(CTweenerHelper)

ECode CTweenerHelper::To(
    /* [in] */ IInterface* object,
    /* [in] */ Int64 duration,
    /* [in] */ ArrayOf<IInterface*>* vars,
    /* [out] */ ITweener** result)
{
    VALIDATE_NOT_NULL(result)

    return Tweener::To(object, duration, vars, result);
}

ECode CTweenerHelper::Reset()
{
    return Tweener::Reset();
}

} // namespace Multiwaveview
} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos
