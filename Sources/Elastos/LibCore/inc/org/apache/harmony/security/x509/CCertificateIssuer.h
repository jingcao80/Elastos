
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATEISSUER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATEISSUER_H__

#include "_Org_Apache_Harmony_Security_X509_CCertificateIssuer.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CCertificateIssuer)
    , public Object
    , public ICertificateIssuer
    , public IExtensionValue
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb);

    CARAPI GetIssuer(
        /* [out] */ IX500Principal** ppX500Principal);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pEncoding);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATEISSUER_H__
