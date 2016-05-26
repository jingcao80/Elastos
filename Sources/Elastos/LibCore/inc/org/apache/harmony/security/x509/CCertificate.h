
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATE_H__

#include "_Org_Apache_Harmony_Security_X509_CCertificate.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CCertificate)
    , public Object
    , public ICertificate
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetTbsCertificate(
        /* [out] */ Org::Apache::Harmony::Security::X509::ITBSCertificate ** ppTbsCertificate);

    CARAPI GetSignatureValue(
        /* [out, callee] */ ArrayOf<Byte> ** ppSignatureValue);

    CARAPI ToString(
        /* [out] */ String * pStr);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncode);

    CARAPI constructor(
        /* [in] */ Org::Apache::Harmony::Security::X509::ITBSCertificate * pTbsCertificate,
        /* [in] */ Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier * pSignatureAlgorithm,
        /* [in] */ ArrayOf<Byte> * pSignatureValue);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATE_H__
