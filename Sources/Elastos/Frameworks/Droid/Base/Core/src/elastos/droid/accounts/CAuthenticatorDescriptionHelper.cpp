
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/accounts/CAuthenticatorDescriptionHelper.h"
#include "elastos/droid/accounts/CAuthenticatorDescription.h"

namespace Elastos {
namespace Droid {
namespace Accounts {

CAR_SINGLETON_IMPL(CAuthenticatorDescriptionHelper)

CAR_INTERFACE_IMPL(CAuthenticatorDescriptionHelper, Singleton, IAuthenticatorDescriptionHelper)

ECode CAuthenticatorDescriptionHelper::NewKey(
    /* [in] */ const String& type,
    /* [out] */ IAuthenticatorDescription** description)
{
    VALIDATE_NOT_NULL(description);
    return CAuthenticatorDescription::NewKey(type, description);
}

} // namespace Accounts
} // namespace Droid
} // namespace Elastos
