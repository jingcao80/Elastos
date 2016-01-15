
#ifndef __ELASTOSX_SECURITY_AUTH_CAUTHPERMISSION_H__
#define __ELASTOSX_SECURITY_AUTH_CAUTHPERMISSION_H__

#include "_Elastosx_Security_Auth_CAuthPermission.h"
#if 0 // TODO: Waiting for BasicPermission
#include "BasicPermission.h"
#else
#include "Object.h"
#endif

using Elastos::Security::IPermissionCollection;
using Elastos::Security::IPermission;
#if 0 // TODO: Waiting for BasicPermission
using Elastos::Security::BasicPermission;
#else
using Elastos::Core::Object;
#endif

namespace Elastosx {
namespace Security {
namespace Auth {

CarClass(CAuthPermission)
#if 0 // TODO: Waiting for BasicPermission
    , public BasicPermission
#else
    , public Object
#endif
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
