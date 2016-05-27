
#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CERTPATHIMPLHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CERTPATHIMPLHELPER_H__

#include "_Org_Apache_Harmony_Security_Provider_Cert_CX509CertPathImplHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Cert {

CarClass(CX509CertPathImplHelper)
    , public Singleton
    , public IX509CertPathImplHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [in] */ Elastos::IO::IInputStream * pIs,
        /* [out] */ Org::Apache::Harmony::Security::Provider::Cert::IX509CertPathImpl ** ppInstance);

    CARAPI GetInstanceEx(
        /* [in] */ Elastos::IO::IInputStream * pIs,
        /* [in] */ const String& encoding,
        /* [out] */ Org::Apache::Harmony::Security::Provider::Cert::IX509CertPathImpl ** ppInstance);

    CARAPI GetInstanceEx2(
        /* [in] */ ArrayOf<Byte> * pPath,
        /* [out] */ Org::Apache::Harmony::Security::Provider::Cert::IX509CertPathImpl ** ppInstance);

    CARAPI GetInstanceEx3(
        /* [in] */ ArrayOf<Byte> * pBytesArray,
        /* [in] */ const String& encoding,
        /* [out] */ Org::Apache::Harmony::Security::Provider::Cert::IX509CertPathImpl ** ppInstance);

    CARAPI GetASN1(
        /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1SequenceOf ** ppAsn1);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CERTPATHIMPLHELPER_H__
