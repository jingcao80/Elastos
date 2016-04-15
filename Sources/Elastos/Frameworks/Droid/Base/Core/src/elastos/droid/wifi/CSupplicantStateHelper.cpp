
#include "elastos/droid/wifi/CSupplicantStateHelper.h"
#include "elastos/droid/wifi/SupplicantStateHelper.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CSupplicantStateHelper, Singleton, ISupplicantStateHelper)

CAR_SINGLETON_IMPL(CSupplicantStateHelper)

ECode CSupplicantStateHelper::IsValidState(
    /* [in] */ SupplicantState state,
    /* [out] */ Boolean* isValidState)
{
    VALIDATE_NOT_NULL(isValidState);
    *isValidState = SupplicantStateHelper::IsValidState(state);
    return NOERROR;
}

ECode CSupplicantStateHelper::IsHandshakeState(
    /* [in] */ SupplicantState state,
    /* [out] */ Boolean* isHandshakeState)
{
    VALIDATE_NOT_NULL(isHandshakeState);
    *isHandshakeState = SupplicantStateHelper::IsHandshakeState(state);
    return NOERROR;
}

ECode CSupplicantStateHelper::IsConnecting(
    /* [in] */ SupplicantState state,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SupplicantStateHelper::IsConnecting(state);
    return NOERROR;
}

ECode CSupplicantStateHelper::IsDriverActive(
    /* [in] */ SupplicantState state,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SupplicantStateHelper::IsDriverActive(state);
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
