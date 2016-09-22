
#include "Permission.h"
#include "AllPermissionCollection.h"

using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Security {

CAR_INTERFACE_IMPL_3(Permission, Object, IPermission, IGuard, ISerializable)
Permission::Permission()
{}

ECode Permission::constructor(
        /* [in] */ const String& name)
{
    return NOERROR;
}

ECode Permission::GetName(
    /* [out] */ String *name)
{
    VALIDATE_NOT_NULL(name)
    *name = String();
    return NOERROR;
}

ECode Permission::CheckGuard(
    /* [in] */ IInterface *obj)
{
    return NOERROR;
}

ECode Permission::NewPermissionCollection(
    /* [out] */ IPermissionCollection **coll)
{
    VALIDATE_NOT_NULL(coll)
    *coll = new AllPermissionCollection();
    REFCOUNT_ADD(*coll)
    return NOERROR;
}

} // namespace Security
} // namespace Elastos
