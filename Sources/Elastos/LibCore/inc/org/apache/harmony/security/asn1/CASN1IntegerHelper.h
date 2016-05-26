
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1INTEGERHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1INTEGERHELPER_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1IntegerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Math::IBigInteger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1IntegerHelper)
    , public Singleton
    , public IASN1IntegerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [out] */ IASN1Integer** instance);

    CARAPI ToIntValue(
        /* [in] */ IInterface* decoded,
        /* [out] */ Int32* intValue);

    CARAPI ToBigIntegerValue(
        /* [in] */ IInterface *decoded,
        /* [out] */ IBigInteger** bigInteger);

    CARAPI FromIntValue(
        /* [in] */ Int32 value,
        /* [out] */ IInterface** fromIntValue);

private:
    // TODO: Add your private member variables here.
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1INTEGERHELPER_H__
