
#include "CAllPermission.h"

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CAllPermission)
ECode CAllPermission::constructor()
{
    return Permission::constructor(String(""));
}

ECode CAllPermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& actions)
{
    return Permission::constructor(String(""));
}

ECode CAllPermission::GetActions(
    /* [out] */ String* actions)
{
    VALIDATE_NOT_NULL(actions);
    *actions = String(NULL);
    return NOERROR;
}

ECode CAllPermission::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

}
}
