
#ifndef __ELASTOS_SECURITY_CPERMISSIONS_H__
#define __ELASTOS_SECURITY_CPERMISSIONS_H__

#include "_Elastos_Security_CPermissions.h"
#include "PermissionCollection.h"

using Elastos::IO::ISerializable;

namespace Elastos {
namespace Security {

CarClass(CPermissions)
    , public PermissionCollection
{
public:
    CAR_OBJECT_DECL()

    CARAPI Add(
        /* [in] */ IPermission* permission);

    CARAPI GetElements(
        /* [out] */ IEnumeration** permissions);

    CARAPI Implies(
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean* result);
};

}
}

#endif // __ELASTOS_SECURITY_CPERMISSIONS_H__
