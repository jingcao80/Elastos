
#include "PermissionCollection.h"

using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Security {

CAR_INTERFACE_IMPL_2(PermissionCollection, Object, IPermissionCollection, ISerializable)

ECode PermissionCollection::IsReadOnly(
    /* [out] */ Boolean *ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = TRUE;
    return NOERROR;
}

ECode PermissionCollection::SetReadOnly()
{
    return NOERROR;
}

} // namespace Security
} // namespace Elastos