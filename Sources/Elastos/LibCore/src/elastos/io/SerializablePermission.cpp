
#include "SerializablePermission.h"

namespace Elastos {
namespace IO {

ECode SerializablePermission::constructor(
    /* [in] */ const String& permissionName)
{
    return BasicPermission::constructor(String(""));
}

ECode SerializablePermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& actions)
{
    return BasicPermission::constructor(String(""), String(""));
}

ECode SerializablePermission::GetActions(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}

ECode SerializablePermission::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
