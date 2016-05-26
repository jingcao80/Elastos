
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CMESSAGEIMPRINT_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CMESSAGEIMPRINT_H__

#include "_Org_Apache_Harmony_Security_X509_Tsp_CMessageImprint.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CarClass(CMessageImprint)
    , public Object
    , public IMessageImprint
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetASN1(
        /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Sequence ** ppAsn1);

    CARAPI SetASN1(
        /* [in] */ Org::Apache::Harmony::Security::Asn1::IASN1Sequence * pAsn1);

    CARAPI constructor(
        /* [in] */ Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier * pAlgId,
        /* [in] */ ArrayOf<Byte> * pHashedMessage);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CMESSAGEIMPRINT_H__
