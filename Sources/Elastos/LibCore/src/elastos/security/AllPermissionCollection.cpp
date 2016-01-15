
#include "AllPermissionCollection.h"

namespace Elastos {
namespace Security {

ECode AllPermissionCollection::Add(
    /* [in] */ IPermission *permission)
{
    return NOERROR;
}

ECode AllPermissionCollection::GetElements(
    /* [out] */ IEnumeration **elems)
{
    *elems = NULL;
    return NOERROR;
}

ECode AllPermissionCollection::Implies(
    /* [in] */ IPermission *permission,
    /* [out] */ Boolean *ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = TRUE;
    return NOERROR;
}

} // namespace Security
} // namespace Elastos
