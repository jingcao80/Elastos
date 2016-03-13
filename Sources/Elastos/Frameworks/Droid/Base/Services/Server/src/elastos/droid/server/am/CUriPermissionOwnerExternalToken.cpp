
#include "elastos/droid/server/am/CUriPermissionOwnerExternalToken.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_INTERFACE_IMPL_2(CUriPermissionOwnerExternalToken, Object, IUriPermissionOwnerExternalToken, IBinder)

CAR_OBJECT_IMPL(CUriPermissionOwnerExternalToken)

ECode CUriPermissionOwnerExternalToken::constructor(
    /* [in] */ IObject* owner)
{
    mUriOwner = owner;
    return NOERROR;
}

ECode CUriPermissionOwnerExternalToken::GetOwner(
    /* [out] */ IObject** owner)
{
    VALIDATE_NOT_NULL(owner)
    *owner = mUriOwner;
    REFCOUNT_ADD(*owner)
    return NOERROR;
}

ECode CUriPermissionOwnerExternalToken::ToString(
    /* [out] */ String *str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb("UriPermissionOwner::ExternalToken: ");
    sb += (Int32)this;
    *str = sb.ToString();
    return NOERROR;
}

} // namespace Am
} // namespace Droid
} // namespace Droid
} // namespace Elastos