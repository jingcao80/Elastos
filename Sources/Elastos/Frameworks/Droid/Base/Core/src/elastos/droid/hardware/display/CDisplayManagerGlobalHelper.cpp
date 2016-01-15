
// #include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/hardware/display/CDisplayManagerGlobalHelper.h"
#include "elastos/droid/hardware/display/DisplayManagerGlobal.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

CAR_INTERFACE_IMPL(CDisplayManagerGlobalHelper, Singleton, IDisplayManagerGlobalHelper)

CAR_SINGLETON_IMPL(CDisplayManagerGlobalHelper)

ECode CDisplayManagerGlobalHelper::GetInstance(
    /* [out] */ IDisplayManagerGlobal** global)
{
    VALIDATE_NOT_NULL(global);

    AutoPtr<IDisplayManagerGlobal> temp = DisplayManagerGlobal::GetInstance();
    *global = temp;
    REFCOUNT_ADD(*global);
    return NOERROR;
}

} // namespace Display
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
