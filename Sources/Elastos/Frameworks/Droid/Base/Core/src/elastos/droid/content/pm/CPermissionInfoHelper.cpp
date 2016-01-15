
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/CPermissionInfoHelper.h"
#include "elastos/droid/content/pm/CPermissionInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL(CPermissionInfoHelper, Singleton, IPermissionInfoHelper)

CAR_SINGLETON_IMPL(CPermissionInfoHelper)

ECode CPermissionInfoHelper::FixProtectionLevel(
    /* [in] */ Int32 level,
    /* [out] */ Int32* retLevel)
{
    VALIDATE_NOT_NULL(retLevel);
    *retLevel = CPermissionInfo::FixProtectionLevel(level);
    return NOERROR;
}

ECode CPermissionInfoHelper::ProtectionToString(
    /* [in] */ Int32 level,
    /* [out] */ String* protLevel)
{
    VALIDATE_NOT_NULL(protLevel);
    *protLevel = CPermissionInfo::ProtectionToString(level);
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
