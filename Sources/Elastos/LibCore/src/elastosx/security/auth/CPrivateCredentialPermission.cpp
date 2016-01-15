
#include "CPrivateCredentialPermission.h"

namespace Elastosx {
namespace Security {
namespace Auth {

CAR_OBJECT_IMPL(CPrivateCredentialPermission)

#if 0 // TODO: Waiting for Permission
CAR_INTERFACE_IMPL(CPrivateCredentialPermission, Permission, IPrivateCredentialPermission)
#else
CAR_INTERFACE_IMPL(CPrivateCredentialPermission, Object, IPrivateCredentialPermission)
#endif

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
    VALIDATE_NOT_NULL(*principals)
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
#if 0 // TODO: Waiting for Permission
    return Permission::constructor(String(""));
#else
    assert(0);
    return NOERROR;
#endif
}

} // namespace Auth
} // namespace Security
} // namespace Elastosx

