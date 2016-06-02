
#include "elastos/droid/internal/telephony/SmsAddress.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(SmsAddress, Object, ISmsAddress)

CARAPI SmsAddress::GetAddressString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = address;
    return NOERROR;
}

CARAPI SmsAddress::IsAlphanumeric(
        /* [out] */  Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ton == TON_ALPHANUMERIC;
    return NOERROR;
}

CARAPI SmsAddress::IsNetworkSpecific(
        /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ton == TON_NETWORK;
    return NOERROR;
}

CARAPI SmsAddress::CouldBeEmailGateway(
        /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Some carriers seems to send email gateway messages in this form:
    // from: an UNKNOWN TON, 3 or 4 digits Int64, beginning with a 5
    // PID: 0x00, Data coding scheme 0x03
    // So we just attempt to treat any message from an address length <= 4
    // as an email gateway
    *result = address.GetLength() <= 4;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

