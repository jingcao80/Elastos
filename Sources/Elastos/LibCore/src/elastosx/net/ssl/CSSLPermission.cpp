
#include "CSSLPermission.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_OBJECT_IMPL(CSSLPermission)

ECode CSSLPermission::constructor(
    /* [in] */ const String& name)
{
    assert(0 && "TODO");
    //TODO return BasicPermission::constructor(String(""));
    return NOERROR;
}

ECode CSSLPermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& actions)
{
    assert(0 && "TODO");
    //TODO return BasicPermission::constructor(String(""), String(""));
    return NOERROR;
}

ECode CSSLPermission::GetActions(
    /* [out] */ String* action)
{
    VALIDATE_NOT_NULL(action)

    *action = String(NULL);
    return NOERROR;
}

ECode CSSLPermission::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = TRUE;
    return NOERROR;
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
