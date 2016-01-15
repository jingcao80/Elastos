
#include "elastos/droid/wifi/CSupplicantStateHelper.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CSupplicantStateHelper, Singleton, ISupplicantStateHelper)

CAR_SINGLETON_IMPL(CSupplicantStateHelper)

ECode CSupplicantStateHelper::IsValidState(
    /* [in] */ Elastos::Droid::Wifi::SupplicantState state,
    /* [out] */ Boolean* isValidState)
{
    if (isValidState == NULL) {
        return NOERROR;
    }

    *isValidState = (state != SupplicantState_UNINITIALIZED
        && state != SupplicantState_INVALID);

    return NOERROR;
}

ECode CSupplicantStateHelper::IsHandshakeState(
    /* [in] */ Elastos::Droid::Wifi::SupplicantState state,
    /* [out] */ Boolean* isHandshakeState)
{
    if (isHandshakeState == NULL) {
        return NOERROR;
    }

    switch (state) {
        case SupplicantState_AUTHENTICATING:
        case SupplicantState_ASSOCIATING:
        case SupplicantState_ASSOCIATED:
        case SupplicantState_FOUR_WAY_HANDSHAKE:
        case SupplicantState_GROUP_HANDSHAKE:
            *isHandshakeState = TRUE;
            return NOERROR;
        case SupplicantState_COMPLETED:
        case SupplicantState_DISCONNECTED:
        case SupplicantState_INTERFACE_DISABLED:
        case SupplicantState_INACTIVE:
        case SupplicantState_SCANNING:
        case SupplicantState_DORMANT:
        case SupplicantState_UNINITIALIZED:
        case SupplicantState_INVALID:
            *isHandshakeState = FALSE;
            return NOERROR;
        default:
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

ECode CSupplicantStateHelper::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return E_NOT_IMPLEMENTED;
}

ECode CSupplicantStateHelper::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return E_NOT_IMPLEMENTED;
}

ECode CSupplicantStateHelper::IsConnecting(
    /* [in] */ Elastos::Droid::Wifi::SupplicantState state,
    /* [out] */ Boolean* isConnecting)
{
    if (isConnecting == NULL) {
        return NOERROR;
    }

    switch (state) {
        case SupplicantState_AUTHENTICATING:
        case SupplicantState_ASSOCIATING:
        case SupplicantState_ASSOCIATED:
        case SupplicantState_FOUR_WAY_HANDSHAKE:
        case SupplicantState_GROUP_HANDSHAKE:
        case SupplicantState_COMPLETED:
            *isConnecting = TRUE;
            return NOERROR;
        case SupplicantState_DISCONNECTED:
        case SupplicantState_INTERFACE_DISABLED:
        case SupplicantState_INACTIVE:
        case SupplicantState_SCANNING:
        case SupplicantState_DORMANT:
        case SupplicantState_UNINITIALIZED:
        case SupplicantState_INVALID:
            *isConnecting = FALSE;
            return NOERROR;
        default:
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

ECode CSupplicantStateHelper::IsDriverActive(
    /* [in] */ Elastos::Droid::Wifi::SupplicantState state,
    /* [out] */ Boolean* isDriverActive)
{
    if (isDriverActive == NULL) {
        return NOERROR;
    }

    switch (state) {
        case SupplicantState_DISCONNECTED:
        case SupplicantState_DORMANT:
        case SupplicantState_INACTIVE:
        case SupplicantState_AUTHENTICATING:
        case SupplicantState_ASSOCIATING:
        case SupplicantState_ASSOCIATED:
        case SupplicantState_SCANNING:
        case SupplicantState_FOUR_WAY_HANDSHAKE:
        case SupplicantState_GROUP_HANDSHAKE:
        case SupplicantState_COMPLETED:
            *isDriverActive = TRUE;
            return NOERROR;
        case SupplicantState_INTERFACE_DISABLED:
        case SupplicantState_UNINITIALIZED:
        case SupplicantState_INVALID:
            *isDriverActive = FALSE;
            return NOERROR;
        default:
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
