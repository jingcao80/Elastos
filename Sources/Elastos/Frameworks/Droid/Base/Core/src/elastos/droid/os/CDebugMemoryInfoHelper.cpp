
#include "elastos/droid/os/CDebugMemoryInfoHelper.h"
#include "elastos/droid/os/CDebugMemoryInfo.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Os {


ECode CDebugMemoryInfoHelper::constructor()
{
    return NOERROR;
}

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
