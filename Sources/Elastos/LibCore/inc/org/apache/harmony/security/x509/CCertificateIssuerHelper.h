
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATEISSUERHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATEISSUERHELPER_H__

#include "_Org_Apache_Harmony_Security_X509_CCertificateIssuerHelper.h"
#include <elastos/core/Singleton.h>

using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CCertificateIssuerHelper)
    , public Singleton
    , public ICertificateIssuerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetASN1(
        /* [out] */ IASN1Type** ppAsn1);

    CARAPI SetASN1(
        /* [in] */ IASN1Type* pAsn1);
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATEISSUERHELPER_H__
