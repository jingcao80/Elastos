
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATE_H__

#include "_Org_Apache_Harmony_Security_X509_CCertificate.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

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
        /* [out] */ ITBSCertificate** ppTbsCertificate);

    CARAPI GetSignatureValue(
        /* [out, callee] */ ArrayOf<Byte>** ppSignatureValue);

    CARAPI ToString(
        /* [out] */ String* pStr);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI constructor(
        /* [in] */ ITBSCertificate* pTbsCertificate,
        /* [in] */ IAlgorithmIdentifier* pSignatureAlgorithm,
        /* [in] */ ArrayOf<Byte>* pSignatureValue);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATE_H__
