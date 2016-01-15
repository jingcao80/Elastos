
#include "CAuthPermission.h"
#include "AllPermissionCollection.h"

using Elastos::Security::AllPermissionCollection;

namespace Elastosx {
namespace Security {
namespace Auth {

CAR_OBJECT_IMPL(CAuthPermission)

#if 0 // TODO: Waiting for BasicPermission
CAR_INTERFACE_IMPL(CAuthPermission, BasicPermission, IAuthPermission)
#else
CAR_INTERFACE_IMPL(CAuthPermission, Object, IAuthPermission)
#endif

ECode CAuthPermission::GetActions(
    /* [out] */ String *actions)
{
    VALIDATE_NOT_NULL(actions)
    *actions = String(NULL);

    return NOERROR;
}

ECode CAuthPermission::Implies(
    /* [in] */ IPermission *permission,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    VALIDATE_NOT_NULL(permission)

    return NOERROR;
}

ECode CAuthPermission::constructor(
    /* [in] */ const String& name)
{
#if 0 // TODO: Waiting for BasicPermission
    return BasicPermission::constructor(String(""));
#else
    return NOERROR;
#endif
}

ECode CAuthPermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& actions)
{
#if 0 // TODO: Waiting for BasicPermission
    return BasicPermission::constructor(String(""), String(""));
#else
    return NOERROR;
#endif
}

}
}
}

