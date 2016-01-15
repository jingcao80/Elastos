
#ifndef __ELASTOS_SECURITY_BASICPERMISSION_H__
#define __ELASTOS_SECURITY_BASICPERMISSION_H__

#include "Permission.h"

namespace Elastos {
namespace Security {

class BasicPermission
    : public Permission
{
public:
    BasicPermission(
        /* [in] */ const String& name);

    BasicPermission(
        /* [in] */ const String& name,
        /* [in] */ const String& action);

    CARAPI GetActions(
        /* [out] */ String *actions);

    CARAPI Implies(
        /* [in] */ IPermission *permission,
        /* [out] */ Boolean *ret);

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& action);
};

} // namespace Security
} // namespace Elastos

#endif //__ELASTOS_SECURITY_BASICPERMISSION_H__