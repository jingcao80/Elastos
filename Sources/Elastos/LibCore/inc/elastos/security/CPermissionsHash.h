
#ifndef __ELASTOS_SECURITY_CPERMISSIONSHASH_H__
#define __ELASTOS_SECURITY_CPERMISSIONSHASH_H__

#include "_Elastos_Security_CPermissionsHash.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "PermissionCollection.h"

using Elastos::IO::ISerializable;
using Elastos::Utility::IHashTable;

namespace Elastos {
namespace Security {

CarClass(CPermissionsHash)
    , public PermissionCollection
{
public:
    CAR_OBJECT_DECL()

    CPermissionsHash();

    CARAPI Add(
        /* [in] */ IPermission* permission);

    CARAPI GetElements(
        /* [out] */ IEnumeration** permissions);

    CARAPI Implies(
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean* result);

private:
    // private static final long serialVersionUID = -8491988220802933440L;

    AutoPtr<IHashTable> mPerms;
};

}
}

#endif // __ELASTOS_SECURITY_CPERMISSIONSHASH_H__
