
#include "elastos/droid/server/am/CPermissionController.h"
#include "elastos/droid/server/am/CActivityManagerService.h"

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::EIID_IPermissionController;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_INTERFACE_IMPL_2(CPermissionController, Object, IPermissionController, IBinder)
CAR_OBJECT_IMPL(CPermissionController)

ECode CPermissionController::constructor(
    /* [in] */ Handle32 host)
{
    mActivityManagerService = (CActivityManagerService*)host;
    return NOERROR;
}

ECode CPermissionController::CheckPermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 res;
    mActivityManagerService->CheckPermission(permission, pid, uid, &res);
    *result = res == IPackageManager::PERMISSION_GRANTED;
    return NOERROR;
}

ECode CPermissionController::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos