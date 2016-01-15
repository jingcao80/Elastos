
#include "BasicPermission.h"

namespace Elastos {
namespace Security {

BasicPermission::BasicPermission(
    /* [in] */ const String& name)
    : Permission(String(""))
{
}

BasicPermission::BasicPermission(
    /* [in] */ const String& name,
    /* [in] */ const String& action)
    : Permission(String(""))
{
}

ECode BasicPermission::GetActions(
    /* [out] */ String *actions)
{
    VALIDATE_NOT_NULL(actions)
    *actions = String();
    return NOERROR;
}

ECode BasicPermission::Implies(
    /* [in] */ IPermission *permission,
    /* [out] */ Boolean *ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = TRUE;
    return NOERROR;
}

ECode BasicPermission::constructor(
    /* [in] */ const String& name)
{
    return Permission::constructor(String(""));
}

ECode BasicPermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& action)
{
    return Permission::constructor(String(""));
}

} // namespace Security
} // namespace Elastos
