
#ifndef __ELASTOS_SECURITY_CALLPERMISSION_H__
#define __ELASTOS_SECURITY_CALLPERMISSION_H__

#include "_Elastos_Security_CAllPermission.h"
#include "Permission.h"

namespace Elastos {
namespace Security {

CarClass(CAllPermission)
    , public Permission
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& actions);

    CARAPI GetActions(
        /* [out] */ String* actions);

    CARAPI Implies(
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean* result);
};

}
}

#endif // __ELASTOS_SECURITY_CALLPERMISSION_H__
