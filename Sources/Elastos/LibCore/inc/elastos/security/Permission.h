
#ifndef __ELASTOS_SECURITY_PERMISSION_H__
#define __ELASTOS_SECURITY_PERMISSION_H__

#include "Elastos.CoreLibrary.Security.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Security {

class Permission
    : public Object
    , public IPermission
    , public IGuard
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    Permission();

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI GetName(
        /* [out] */ String *name);

    CARAPI CheckGuard(
        /* [in] */ IInterface *obj);

    CARAPI NewPermissionCollection(
        /* [out] */ IPermissionCollection **coll);

    virtual CARAPI GetActions(
        /* [out] */ String *actions) = 0;

    virtual CARAPI Implies(
        /* [in] */ IPermission *permission,
        /* [out] */ Boolean *ret) = 0;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_PERMISSION_H__
