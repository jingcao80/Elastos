
#include "elastos/droid/os/CDebugMemoryInfoHelper.h"
#include "elastos/droid/os/CDebugMemoryInfo.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CDebugMemoryInfoHelper, Singleton, IDebugMemoryInfoHelper)

CAR_SINGLETON_IMPL(CDebugMemoryInfoHelper)

ECode CDebugMemoryInfoHelper::GetOtherLabel(
    /* [in] */ Int32 which,
    /* [out] */ String* label)
{
    VALIDATE_NOT_NULL(label);
    return CDebugMemoryInfo::GetOtherLabel(which, label);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
