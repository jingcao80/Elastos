
#ifndef __ELASTOSX_SECURITY_AUTH_CAUTHPERMISSION_H__
#define __ELASTOSX_SECURITY_AUTH_CAUTHPERMISSION_H__

#include "_Elastosx_Security_Auth_CAuthPermission.h"
#include "BasicPermission.h"

using Elastos::Security::IPermissionCollection;
using Elastos::Security::IPermission;
using Elastos::Security::BasicPermission;

namespace Elastosx {
namespace Security {
namespace Auth {

CarClass(CAuthPermission)
    , public BasicPermission
    , public IAuthPermission
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    // @Override
    CARAPI GetActions(
        /* [out] */ String *actions);

    // @Override
    CARAPI Implies(
        /* [in] */ IPermission *permission,
        /* [out] */ Boolean *result);

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& actions);
};

}
}
}

#endif // __ELASTOSX_SECURITY_AUTH_CAUTHPERMISSION_H__
