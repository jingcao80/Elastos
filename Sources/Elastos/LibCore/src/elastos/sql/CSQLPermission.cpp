
#include "CSQLPermission.h"

namespace Elastos {
namespace Sql {

//TODO: to delete
CAR_INTERFACE_IMPL(CSQLPermission, Object, ISQLPermission)

CAR_OBJECT_IMPL(CSQLPermission)

ECode CSQLPermission::constructor(
    /* [in] */ const String& name)
{
    // super("");
    //TODO
    assert(0);
    // BasicPermission::Init(String(""));
    return NOERROR;
}

ECode CSQLPermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& actions)
{
    // super("", "");
    //TODO
    assert(0);
    // BasicPermission::Init(String(""), String(""));
    return NOERROR;
}

ECode CSQLPermission::GetActions(
    /* [out] */ String * str)
{
    *str = String(NULL);
    return NOERROR;
}

ECode CSQLPermission::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value);
    *value = TRUE;
    return NOERROR;
}

} // namespace Sql
} // namespace Elastos
