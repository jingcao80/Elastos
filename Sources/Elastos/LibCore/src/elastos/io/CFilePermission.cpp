
#include "CFilePermission.h"
// #include "AllPermissionCollection.h"

// using Elastos::Security::AllPermissionCollection;
using Elastos::Security::EIID_IGuard;

namespace Elastos {
namespace IO {

CAR_OBJECT_IMPL(CFilePermission)

CAR_INTERFACE_IMPL_3(CFilePermission, Object, IFilePermission, IPermission, IGuard)

ECode CFilePermission::constructor(
    /* [in] */ const String& path,
    /* [in] */ const String& actions)
{
    // return Permision::constructor(String(""));
    return NOERROR;
}

ECode CFilePermission::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = NULL;
    return NOERROR;
}

ECode CFilePermission::NewPermissionCollection(
    /* [out] */ IPermissionCollection** object)
{
    VALIDATE_NOT_NULL(object);
    // todo delete
    return NOERROR;
}

ECode CFilePermission::GetActions(
    /* [out] */ String* actions)
{
    VALIDATE_NOT_NULL(actions);

    *actions = String(NULL);
    return NOERROR;
}

ECode CFilePermission::Implies(
    /* [in] */ IPermission* p,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = TRUE;
    return NOERROR;
}

ECode CFilePermission::CheckGuard(
    /* [in] */ IInterface* object)
{
    // todo delete
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
