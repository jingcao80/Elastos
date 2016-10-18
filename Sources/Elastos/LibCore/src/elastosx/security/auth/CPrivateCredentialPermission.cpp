
#include "CPrivateCredentialPermission.h"

namespace Elastosx {
namespace Security {
namespace Auth {

CAR_OBJECT_IMPL(CPrivateCredentialPermission)
CAR_INTERFACE_IMPL(CPrivateCredentialPermission, Permission, IPrivateCredentialPermission)
ECode CPrivateCredentialPermission::GetActions(
    /* [out] */ String *actions)
{
    VALIDATE_NOT_NULL(actions)
    *actions = String(NULL);
    return NOERROR;
}

ECode CPrivateCredentialPermission::Implies(
    /* [in] */ IPermission *permission,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    VALIDATE_NOT_NULL(permission)

    return NOERROR;
}

ECode CPrivateCredentialPermission::GetPrincipals(
    /* [out, callee] */ ArrayOf<IArrayOf*> **principals)
{
    VALIDATE_NOT_NULL(principals)
    *principals = NULL;

    return NOERROR;
}

ECode CPrivateCredentialPermission::GetCredentialClass(
    /* [out] */ String *credential)
{
    VALIDATE_NOT_NULL(credential)
    *credential = String(NULL);
    return NOERROR;
}

ECode CPrivateCredentialPermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& action)
{
    return Permission::constructor(String(""));
}

} // namespace Auth
} // namespace Security
} // namespace Elastosx
