
#include "CAuthPermission.h"
#include "AllPermissionCollection.h"

using Elastos::Security::AllPermissionCollection;

namespace Elastosx {
namespace Security {
namespace Auth {

CAR_OBJECT_IMPL(CAuthPermission)
CAR_INTERFACE_IMPL(CAuthPermission, BasicPermission, IAuthPermission)

ECode CAuthPermission::GetActions(
    /* [out] */ String *actions)
{
    VALIDATE_NOT_NULL(actions)
    *actions = String(NULL);

    return NOERROR;
}

ECode CAuthPermission::Implies(
    /* [in] */ IPermission *permission,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    VALIDATE_NOT_NULL(permission)

    return NOERROR;
}

ECode CAuthPermission::constructor(
    /* [in] */ const String& name)
{
    return BasicPermission::constructor(String(""));
}

ECode CAuthPermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& actions)
{
    return BasicPermission::constructor(String(""), String(""));
}

}
}
}
