
#include "CGuardedObject.h"

using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CGuardedObject)
CAR_INTERFACE_IMPL_2(CGuardedObject, Object, IGuardedObject, ISerializable)
ECode CGuardedObject::constructor(
    /* [in] */ IInterface* object,
    /* [in] */ IGuard* guard)
{
    mObject = object;
    mGuard = guard;
    return NOERROR;
}

ECode CGuardedObject::GetObject(
    /* [out] */ IInterface** result) /*throws SecurityException*/
{
    if (mGuard != NULL) {
        FAIL_RETURN(mGuard->CheckGuard(mObject));
    }
    *result = mObject;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CGuardedObject::WriteObject(
    /* [in] */ /*java.io.*/IObjectOutputStream* out) /*throws IOException*/
{
    if (mGuard != NULL) {
        FAIL_RETURN(mGuard->CheckGuard(mObject));
    }
    return out->DefaultWriteObject();
}

} // namespace Security
} // namespace Elastos

