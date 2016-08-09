
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSADDRESS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSADDRESS_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class SmsAddress
    : public Object
    , public ISmsAddress
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the address of the SMS message in String form or NULL if unavailable
     */
    CARAPI GetAddressString(
        /* [out] */ String* result);

    /**
     * Returns TRUE if this is an alphanumeric address
     */
    CARAPI IsAlphanumeric(
        /* [out] */  Boolean* result);

    /**
     * Returns TRUE if this is a network address
     */
    CARAPI IsNetworkSpecific(
        /* [out] */ Boolean* result);

    CARAPI CouldBeEmailGateway(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);
public:
    //TODO
    Int32 ton;
    String address;
    AutoPtr<ArrayOf<Byte> > origBytes;

};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSADDRESS_H__
