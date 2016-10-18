
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CINHIBITANYPOLICY_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CINHIBITANYPOLICY_H__

#include "_Org_Apache_Harmony_Security_X509_CInhibitAnyPolicy.h"
#include "org/apache/harmony/security/x509/ExtensionValue.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CInhibitAnyPolicy)
    , public ExtensionValue
    , public IInhibitAnyPolicy
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pEncoding);

private:
    /** the value of the extension */
    Int32 mSkipCerts;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CINHIBITANYPOLICY_H__
