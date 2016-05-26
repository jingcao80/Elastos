
#ifndef __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTETYPEANDVALUEHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTETYPEANDVALUEHELPER_H__

#include "_Org_Apache_Harmony_Security_X501_CAttributeTypeAndValueHelper.h"
#include <elastos/core/Singleton.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

CarClass(CAttributeTypeAndValueHelper)
    , public Singleton
    , public IAttributeTypeAndValueHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetAttributeValue(
        /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Type ** ppAttributeValue);

    CARAPI SetAttributeValue(
        /* [in] */ Org::Apache::Harmony::Security::Asn1::IASN1Type * pAttributeValue);

    CARAPI GetASN1Sequence(
        /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Sequence ** ppAsn1);

    CARAPI SetASN1Sequence(
        /* [in] */ Org::Apache::Harmony::Security::Asn1::IASN1Sequence * pAsn1);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTETYPEANDVALUEHELPER_H__
