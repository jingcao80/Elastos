
#ifndef __ELASTOSX_SECURITY_AUTH_CPRIVATECREDENTIALPERMISSION_H__
#define __ELASTOSX_SECURITY_AUTH_CPRIVATECREDENTIALPERMISSION_H__

#include "_Elastosx_Security_Auth_CPrivateCredentialPermission.h"
#include "Permission.h"

using Elastos::Security::IPermissionCollection;
using Elastos::Security::IPermission;
using Elastos::Security::Permission;
using Elastos::Core::IArrayOf;

namespace Elastosx {
namespace Security {
namespace Auth {

CarClass(CPrivateCredentialPermission)
    , public Permission
    , public IPrivateCredentialPermission
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetPrincipals(
        /* [out, callee] */ ArrayOf<IArrayOf*> **principals);

    CARAPI GetCredentialClass(
        /* [out] */ String *credential);

    CARAPI GetActions(
        /* [out] */ String *actions);

    CARAPI Implies(
        /* [in] */ IPermission *permission,
        /* [out] */ Boolean *result);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& action);
};

} // namespace Auth
} // namespace Security
} // namespace Elastosx

#endif // __ELASTOSX_SECURITY_AUTH_CPRIVATECREDENTIALPERMISSION_H__
