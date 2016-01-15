
#ifndef __ELASTOS_SECURITY_PERMISSION_H__
#define __ELASTOS_SECURITY_PERMISSION_H__

#include "Elastos.CoreLibrary.Security.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Security {

class Permission
    : public Object
    , public IPermission
{
public:
    CAR_INTERFACE_DECL()

    Permission(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI GetName(
        /* [out] */ String *name) const;

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
