
#include "CPermissions.h"

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CPermissions)
ECode CPermissions::Add(
    /* [in] */ IPermission* permission)
{
    return NOERROR;
}

ECode CPermissions::GetElements(
    /* [out] */ IEnumeration** permissions)
{
    VALIDATE_NOT_NULL(*permissions);
    *permissions = NULL;
    return NOERROR;
}

ECode CPermissions::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

}
}
